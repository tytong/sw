// {C} Copyright 2020 Pensando Systems Inc. All rights reserved

#include "platform/elba/elba_txs_scheduler.hpp"
#include "asic/cmn/asic_common.hpp"
#include "platform/elba/elba_common.hpp"
#include "platform/elba/elba_state.hpp"
#include "nic/sdk/lib/utils/utils.hpp"
#include "platform/utils/mpartition.hpp"

#include "third-party/asic/elba/model/elb_psp/elb_psp_csr.h"
#include "third-party/asic/elba/model/utils/elb_blk_reg_model.h"
#include "third-party/asic/elba/model/elb_top/elb_top_csr.h"
#include "third-party/asic/elba/model/elb_txs/elb_txs_csr.h"
#include "third-party/asic/elba/verif/apis/elb_txs_sw_api.h"
#include "third-party/asic/elba/model/elb_wa/elb_wa_csr.h"

namespace sdk {
namespace platform {
namespace elba {

uint32_t
elba_get_coreclk_freq (platform_type_t platform_type)
{
   return CORECLK_FREQ_ASIC_10;
}

void
elba_txs_timer_init_hsh_depth (uint32_t key_lines)
{
    uint64_t timer_key_hbm_base_addr;
    elb_top_csr_t & elb0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    elb_txs_csr_t *txs_csr = &elb0.txs.txs;

    timer_key_hbm_base_addr =
        g_elba_state_pd->mempartition()->start_addr(MEM_REGION_TIMERS_NAME);

    txs_csr->cfg_timer_static.read();
    SDK_TRACE_DEBUG("hbm_base 0x%lx",
                    (uint64_t)txs_csr->cfg_timer_static.hbm_base());
    SDK_TRACE_DEBUG("timer hash depth %u",
                    int(txs_csr->cfg_timer_static.tmr_hsh_depth()));
    SDK_TRACE_DEBUG("timer wheel depth %u",
                    int(txs_csr->cfg_timer_static.tmr_wheel_depth()));
    txs_csr->cfg_timer_static.hbm_base(timer_key_hbm_base_addr);
    txs_csr->cfg_timer_static.tmr_hsh_depth(key_lines - 1);
    txs_csr->cfg_timer_static.tmr_wheel_depth(ELBA_TIMER_WHEEL_DEPTH - 1);
    txs_csr->cfg_timer_static.write();
}

// pre init and call timer hbm and sram init
static void
elba_txs_timer_init_pre (uint32_t key_lines, asic_cfg_t *elba_cfg)
{
    elb_top_csr_t & elb0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    elb_txs_csr_t *txs_csr = &elb0.txs.txs;

    // Set timer_hsh_depth to actual value + 1
    // Per Cino we need to add 1 for an ASIC bug workaround
    elba_txs_timer_init_hsh_depth(key_lines + 1);

    // timer hbm and sram init

    // sram_hw_init is not implemented in the C++ model, so skip it there
    SDK_ASSERT(elba_cfg);
    txs_csr->cfw_timer_glb.read();
    if (elba_cfg->platform != platform_type_t::PLATFORM_TYPE_SIM) {
        SDK_TRACE_DEBUG("timer sram init");
        txs_csr->cfw_timer_glb.sram_hw_init(1);
    }

    // skip hbm init in model (C++ and RTL) as memory is 0 there and this
    // takes a long time
    if (elba_cfg->platform != platform_type_t::PLATFORM_TYPE_SIM &&
          elba_cfg->platform != platform_type_t::PLATFORM_TYPE_RTL) {
        SDK_TRACE_DEBUG("timer hbm init");
        txs_csr->cfw_timer_glb.hbm_hw_init(1);
    }
    txs_csr->cfw_timer_glb.write();

    SDK_TRACE_DEBUG("Done timer pre init");
}

// This is called after hbm and sram init
static void
elba_txs_timer_init_post (uint32_t key_lines, asic_cfg_t *elba_cfg)
{
    elb_top_csr_t & elb0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    elb_txs_csr_t *txs_csr = &elb0.txs.txs;
    uint32_t elba_coreclk_freq;

    // 0 the last element of sram
    // Per Cino this is needed for an ASIC bug workaround
    txs_csr->dhs_tmr_cnt_sram.entry[ELBA_TIMER_WHEEL_DEPTH - 1].read();
    txs_csr->dhs_tmr_cnt_sram.entry[ELBA_TIMER_WHEEL_DEPTH - 1].slow_cbcnt(0);
    txs_csr->dhs_tmr_cnt_sram.entry[ELBA_TIMER_WHEEL_DEPTH - 1].slow_bcnt(0);
    txs_csr->dhs_tmr_cnt_sram.entry[ELBA_TIMER_WHEEL_DEPTH - 1].slow_lcnt(0);
    txs_csr->dhs_tmr_cnt_sram.entry[ELBA_TIMER_WHEEL_DEPTH - 1].fast_cbcnt(0);
    txs_csr->dhs_tmr_cnt_sram.entry[ELBA_TIMER_WHEEL_DEPTH - 1].fast_bcnt(0);
    txs_csr->dhs_tmr_cnt_sram.entry[ELBA_TIMER_WHEEL_DEPTH - 1].fast_lcnt(0);
    txs_csr->dhs_tmr_cnt_sram.entry[ELBA_TIMER_WHEEL_DEPTH - 1].write();

    // Set timer_hsh_depth back to original size
    // Per Cino this is needed for an ASIC bug workaround
    elba_txs_timer_init_hsh_depth(key_lines);

    elba_coreclk_freq =
        (uint32_t)(elba_get_coreclk_freq(elba_cfg->platform) / 1000000);

    // Set the tick resolution
    txs_csr->cfg_fast_timer.read();
    txs_csr->cfg_fast_timer.tick(elba_coreclk_freq);
    txs_csr->cfg_fast_timer.write();

    txs_csr->cfg_slow_timer.read();
    txs_csr->cfg_slow_timer.tick(elba_coreclk_freq * 1000);
    txs_csr->cfg_slow_timer.write();

    // Timer doorbell config
    txs_csr->cfg_fast_timer_dbell.read();
    txs_csr->cfg_fast_timer_dbell.addr_update(DB_IDX_UPD_PIDX_INC |
                                              DB_SCHED_UPD_EVAL);
    txs_csr->cfg_fast_timer_dbell.write();

    txs_csr->cfg_slow_timer_dbell.read();
    txs_csr->cfg_slow_timer_dbell.addr_update(DB_IDX_UPD_PIDX_INC |
                                              DB_SCHED_UPD_EVAL);
    txs_csr->cfg_slow_timer_dbell.write();

    // Enable slow and fast timers
    txs_csr->cfw_timer_glb.read();
    txs_csr->cfw_timer_glb.ftmr_enable(1);
    txs_csr->cfw_timer_glb.stmr_enable(1);
    txs_csr->cfw_timer_glb.write();

    SDK_TRACE_DEBUG("Done timer post init");
}

sdk_ret_t
elba_txs_scheduler_init (uint32_t admin_cos, asic_cfg_t *elba_cfg)
{

    elb_top_csr_t       &elb0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    elb_txs_csr_t       &txs_csr = elb0.txs.txs;
    elb_psp_csr_t       &psp_pt_csr = elb0.pt.pt.psp,
                        &psp_pr_csr = elb0.pr.pr.psp;
    uint64_t            txs_sched_hbm_base_addr;

    // init all sch tables
    txs_csr.dhs_sch_cache_idx_sram.entry[0].all(1);
    txs_csr.dhs_sch_cache_idx_sram.entry[0].write();
    txs_csr.dhs_sch_cache_idx_sram.entry[0].read();
    for (int ii=0; ii<txs_csr.dhs_sch_cache_idx_sram.get_depth_entry(); ii++) {
       txs_csr.dhs_sch_cache_idx_sram.entry[ii].all(0);
       txs_csr.dhs_sch_cache_idx_sram.entry[ii].write();
    }

    txs_csr.dhs_sch_cache_data_wb_sram.entry[0].all(1);
    txs_csr.dhs_sch_cache_data_wb_sram.entry[0].write();
    txs_csr.dhs_sch_cache_data_wb_sram.entry[0].read();
    for (int ii=0; ii<txs_csr.dhs_sch_cache_data_wb_sram.get_depth_entry(); ii++) {
       txs_csr.dhs_sch_cache_data_wb_sram.entry[ii].all(0);
       txs_csr.dhs_sch_cache_data_wb_sram.entry[ii].write();
    }


    txs_csr.dhs_sch_cache_data_sram.entry[0].all(cpp_int ("0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"));
    txs_csr.dhs_sch_cache_data_sram.entry[0].write();
    txs_csr.dhs_sch_cache_data_sram.entry[0].read();
    for (int ii=0; ii<txs_csr.dhs_sch_cache_data_sram.get_depth_entry(); ii++) {
       txs_csr.dhs_sch_cache_data_sram.entry[ii].all(0);
       txs_csr.dhs_sch_cache_data_sram.entry[ii].write();
    }

    txs_csr.dhs_sch_rlid_map_sram.entry[0].all(1);
    txs_csr.dhs_sch_rlid_map_sram.entry[0].write();
    txs_csr.dhs_sch_rlid_map_sram.entry[0].read();
    for (int ii=0; ii<txs_csr.dhs_sch_rlid_map_sram.get_depth_entry(); ii++) {
      txs_csr.dhs_sch_rlid_map_sram.entry[ii].all(0);
      txs_csr.dhs_sch_rlid_map_sram.entry[ii].write();
    }

    txs_csr.dhs_sch_lif_dwrr_cfg_sram.entry[0].all(1);
    txs_csr.dhs_sch_lif_dwrr_cfg_sram.entry[0].write();
    txs_csr.dhs_sch_lif_dwrr_cfg_sram.entry[0].read();
    for (int ii=0; ii<txs_csr.dhs_sch_lif_dwrr_cfg_sram.get_depth_entry(); ii++) {
       txs_csr.dhs_sch_lif_dwrr_cfg_sram.entry[ii].all(0);
       txs_csr.dhs_sch_lif_dwrr_cfg_sram.entry[ii].write();
    }

    txs_csr.dhs_sch_lif_dwrr_counter_sram.entry[0].all(1);
    txs_csr.dhs_sch_lif_dwrr_counter_sram.entry[0].write();
    txs_csr.dhs_sch_lif_dwrr_counter_sram.entry[0].read();
    for (int ii=0; ii<txs_csr.dhs_sch_lif_dwrr_counter_sram.get_depth_entry(); ii++) {
       txs_csr.dhs_sch_lif_dwrr_counter_sram.entry[ii].all(0);
       txs_csr.dhs_sch_lif_dwrr_counter_sram.entry[ii].write();
    }

    txs_csr.dhs_sch_lif_base_map_sram.entry[0].all(1);
    txs_csr.dhs_sch_lif_base_map_sram.entry[0].write();
    txs_csr.dhs_sch_lif_base_map_sram.entry[0].read();
    for (int ii=0; ii<txs_csr.dhs_sch_lif_base_map_sram.get_depth_entry(); ii++) {
       txs_csr.dhs_sch_lif_base_map_sram.entry[ii].all(0);
       txs_csr.dhs_sch_lif_base_map_sram.entry[ii].write();
    }

    txs_csr.dhs_sch_qgrp_cfg_0_sram.entry[0].all(1);
    txs_csr.dhs_sch_qgrp_cfg_0_sram.entry[0].write();
    txs_csr.dhs_sch_qgrp_cfg_0_sram.entry[0].read();
    for (int ii=0; ii<txs_csr.dhs_sch_qgrp_cfg_0_sram.get_depth_entry(); ii++) {
       txs_csr.dhs_sch_qgrp_cfg_0_sram.entry[ii].all(0);
       txs_csr.dhs_sch_qgrp_cfg_0_sram.entry[ii].write();
    }

    txs_csr.dhs_sch_qgrp_cfg_1_sram.entry[0].all(1);
    txs_csr.dhs_sch_qgrp_cfg_1_sram.entry[0].write();
    txs_csr.dhs_sch_qgrp_cfg_1_sram.entry[0].read();
    for (int ii=0; ii<txs_csr.dhs_sch_qgrp_cfg_1_sram.get_depth_entry(); ii++) {
       txs_csr.dhs_sch_qgrp_cfg_1_sram.entry[ii].all(0);
       txs_csr.dhs_sch_qgrp_cfg_1_sram.entry[ii].write();
    }

    for (int ii=0; ii<txs_csr.dhs_sch_lif_cfg_sram.get_depth_entry(); ii++) {
       txs_csr.dhs_sch_lif_cfg_sram.entry[ii].all(0);
       txs_csr.dhs_sch_lif_cfg_sram.entry[ii].disabled(1);
       txs_csr.dhs_sch_lif_cfg_sram.entry[ii].write();
    }

    txs_csr.dhs_sch_lif_counter_sram.entry[0].all(1);
    txs_csr.dhs_sch_lif_counter_sram.entry[0].write();
    txs_csr.dhs_sch_lif_counter_sram.entry[0].read();
    for (int ii=0; ii<txs_csr.dhs_sch_lif_counter_sram.get_depth_entry(); ii++) {
       txs_csr.dhs_sch_lif_counter_sram.entry[ii].all(0);
       txs_csr.dhs_sch_lif_counter_sram.entry[ii].write();
    }

    // LIF status
    // Lif_next_qgrp_sram.valid = 0, not_empty = 0
    txs_csr.dhs_sch_lif_next_qgrp_sram.entry[0].all(1);
    txs_csr.dhs_sch_lif_next_qgrp_sram.entry[0].write();
    txs_csr.dhs_sch_lif_next_qgrp_sram.entry[0].read();
    for (int ii=0; ii<txs_csr.dhs_sch_lif_next_qgrp_sram.get_depth_entry(); ii++) {
       txs_csr.dhs_sch_lif_next_qgrp_sram.entry[ii].all(0);
       txs_csr.dhs_sch_lif_next_qgrp_sram.entry[ii].write();
    }

    // Qgrp status
    // Qgrp_counter_sram.qid0_vld =0, not_empty = 0
    txs_csr.dhs_sch_qgrp_counter_0_sram.entry[0].all(1);
    txs_csr.dhs_sch_qgrp_counter_0_sram.entry[0].write();
    txs_csr.dhs_sch_qgrp_counter_0_sram.entry[0].read();
    for (int ii=0; ii<txs_csr.dhs_sch_qgrp_counter_0_sram.get_depth_entry(); ii++) {
       txs_csr.dhs_sch_qgrp_counter_0_sram.entry[ii].all(0);
       txs_csr.dhs_sch_qgrp_counter_0_sram.entry[ii].write();
    }
    txs_csr.dhs_sch_qgrp_counter_1_sram.entry[0].all(1);
    txs_csr.dhs_sch_qgrp_counter_1_sram.entry[0].write();
    txs_csr.dhs_sch_qgrp_counter_1_sram.entry[0].read();
    for (int ii=0; ii<txs_csr.dhs_sch_qgrp_counter_1_sram.get_depth_entry(); ii++) {
       txs_csr.dhs_sch_qgrp_counter_1_sram.entry[ii].all(0);
       txs_csr.dhs_sch_qgrp_counter_1_sram.entry[ii].write();
    }
    // Qgrp_next_qid_sram.qid1_vld = 0, next_zone_vld = 0, cache_read = 0
    PLOG_MSG("inside init_txs txs_csr.dhs_sch_qgrp_next_qid.entry\n");
    txs_csr.dhs_sch_qgrp_next_qid_0_sram.entry[0].all(1);
    txs_csr.dhs_sch_qgrp_next_qid_0_sram.entry[0].write();
    txs_csr.dhs_sch_qgrp_next_qid_0_sram.entry[0].read();
    for (int ii=0; ii<txs_csr.dhs_sch_qgrp_next_qid_0_sram.get_depth_entry(); ii++) {
       txs_csr.dhs_sch_qgrp_next_qid_0_sram.entry[ii].all(0);
       txs_csr.dhs_sch_qgrp_next_qid_0_sram.entry[ii].write();
    }
    txs_csr.dhs_sch_qgrp_next_qid_1_sram.entry[0].all(1);
    txs_csr.dhs_sch_qgrp_next_qid_1_sram.entry[0].write();
    txs_csr.dhs_sch_qgrp_next_qid_1_sram.entry[0].read();
    for (int ii=0; ii<txs_csr.dhs_sch_qgrp_next_qid_1_sram.get_depth_entry(); ii++) {
       txs_csr.dhs_sch_qgrp_next_qid_1_sram.entry[ii].all(0);
       txs_csr.dhs_sch_qgrp_next_qid_1_sram.entry[ii].write();
    }

    // init timer/sch cfg
    txs_csr.cfw_timer_glb.read();
    txs_csr.cfw_timer_glb.ftmr_enable(0);
    txs_csr.cfw_timer_glb.stmr_enable(0);
    txs_csr.cfw_timer_glb.hbm_hw_init(0);
    txs_csr.cfw_timer_glb.sram_hw_init(0);
    txs_csr.cfw_timer_glb.show();
    txs_csr.cfw_timer_glb.write();

    txs_csr.cfw_scheduler_glb.read();
    txs_csr.cfw_scheduler_glb.hbm_hw_init(0);
    txs_csr.cfw_scheduler_glb.sram_hw_init(0);
    txs_csr.cfw_scheduler_glb.lif_dwrr_en(0);
    txs_csr.cfw_scheduler_glb.cache_miss_bypass(0);
    txs_csr.cfw_scheduler_glb.cache_test_mode(0);
    txs_csr.cfw_scheduler_glb.show();
    txs_csr.cfw_scheduler_glb.write();

    txs_sched_hbm_base_addr =
        g_elba_state_pd->mempartition()->start_addr(MEM_REGION_TX_SCHEDULER_NAME);

    // Update HBM base addr.
    txs_csr.cfw_scheduler_static.read();
    txs_csr.cfw_scheduler_static.hbm_base(txs_sched_hbm_base_addr);

    // Init sram.
    txs_csr.cfw_scheduler_glb.read();
    // skip init on RTL/Model.
    SDK_ASSERT(elba_cfg);
    if (elba_cfg->platform != platform_type_t::PLATFORM_TYPE_SIM &&
        elba_cfg->platform != platform_type_t::PLATFORM_TYPE_RTL) {
        txs_csr.cfw_scheduler_glb.hbm_hw_init(1);
    }
    txs_csr.cfw_scheduler_glb.sram_hw_init(1);

    // Disabling scheduler bypass/collapse setting for QoS functionality to work.
    // txs_csr.cfw_scheduler_glb.enable_set_byp(0); TODO_ELBA

    txs_csr.cfw_scheduler_static.write();
    txs_csr.cfw_scheduler_glb.write();

    //TxDMA
    psp_pt_csr.cfg_profile.read();
    psp_pt_csr.cfg_profile.collapse_enable(0);
    psp_pt_csr.cfg_profile.write();

    // RxDMA
    psp_pr_csr.cfg_profile.read();
    psp_pr_csr.cfg_profile.collapse_enable(0);
    psp_pr_csr.cfg_profile.write();

    // init timer
    elba_txs_timer_init_pre(ELBA_TIMER_NUM_KEY_CACHE_LINES, elba_cfg);

    // Asic polling routine to check if init is done and kickstart scheduler.
    elb_txs_init_done(0, 0);

    //Init PSP block to enable mapping tm_oq value in PHV.
    psp_pt_csr.cfg_npv_values.read();
    psp_pt_csr.cfg_npv_values.tm_oq_map_enable(1);
    psp_pt_csr.cfg_npv_values.write();

    //Program one-to-one mapping from cos to tm_oq.
    for (int i = 0; i < NUM_MAX_COSES ; i++) {
        psp_pt_csr.cfg_npv_cos_to_tm_oq_map[i].tm_oq(i);
        psp_pt_csr.cfg_npv_cos_to_tm_oq_map[i].write();
    }

    // init timer post init done
    elba_txs_timer_init_post(ELBA_TIMER_NUM_KEY_CACHE_LINES, elba_cfg);
    if(elba_cfg->completion_func) {
        elba_cfg->completion_func(sdk_status_t::SDK_STATUS_SCHEDULER_INIT_DONE);
    }
    SDK_TRACE_DEBUG("Set hbm base addr for TXS sched to 0x%lx",
                    txs_sched_hbm_base_addr);
    return SDK_RET_OK;
}

sdk_ret_t
elba_txs_scheduler_lif_params_update (uint32_t hw_lif_id,
                                      elba_txs_sched_lif_params_t *txs_hw_params)
{

    elb_top_csr_t &elb0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    elb_txs_csr_t &txs_csr = elb0.txs.txs;
    uint32_t      disable = (txs_hw_params->cos_bmp == 0) ? 1 : 0;
    uint32_t      block_size;
    uint16_t      lif_cos_bmp = txs_hw_params->cos_bmp;

    // start of qgrp entry in 4k dhs_sch_qgrp_cfg_0_sram/dhs_sch_qgrp_cfg_1_sram tables
    uint32_t q_grp_start = txs_hw_params->sched_qgrp_offset;
    uint32_t q_grp_end   = txs_hw_params->sched_qgrp_offset + sdk::lib::count_bits_set(lif_cos_bmp) - 1;

    uint32_t sch_qgrp_map_idx;

    uint32_t cos_idx;
    uint32_t qid_start;
    uint32_t qid_end;
    int      qgrp;

    if ((hw_lif_id > ELBA_TXS_MAX_TABLE_ENTRIES) ||
        (txs_hw_params->sched_table_offset > ELBA_TXS_SCHEDULER_MAP_MAX_ENTRIES)) {
        SDK_TRACE_ERR("Invalid parameters to function hw_lif_id:%u,sched_table_offset:%u",
                       hw_lif_id, txs_hw_params->sched_table_offset);
        return SDK_RET_INVALID_ARG;
    }

    if ((hw_lif_id > ELBA_TXS_MAX_TABLE_ENTRIES) ||
        (txs_hw_params->sched_qgrp_offset > ELBA_TXS_MAX_QGRP_TABLE_ENTRIES)) {
        SDK_TRACE_ERR("Invalid parameters to function hw_lif_id:%u,sched_qgrp_offset:%u",
                       hw_lif_id, txs_hw_params->sched_qgrp_offset);
        return SDK_RET_INVALID_ARG;
    }

    // total_qcount: total_qcount/cos in hw_lif_id
    // with assumption of 1 qgrp/cos
    // block_size: number of 2k chunk (qids)
    //same: block_size  = tx_params->total_qcount/ELBA_TXS_SCHEDULER_NUM_QUEUES_PER_ENTRY;
    //same: block_size += ((tx_params->total_qcount%ELBA_TXS_SCHEDULER_NUM_QUEUES_PER_ENTRY) ? 1 : 0);
    block_size = txs_hw_params->num_entries_per_cos;

    //Program mapping from (lif,queue,cos) to scheduler table entries.
    // dhs_sch_lif_base_map_sram
    txs_csr.dhs_sch_lif_base_map_sram.entry[hw_lif_id].read();
    txs_csr.dhs_sch_lif_base_map_sram.entry[hw_lif_id].base_addr(txs_hw_params->sched_table_offset);
    txs_csr.dhs_sch_lif_base_map_sram.entry[hw_lif_id].block_size(block_size);
    txs_csr.dhs_sch_lif_base_map_sram.entry[hw_lif_id].active_cos(lif_cos_bmp);
    txs_csr.dhs_sch_lif_base_map_sram.entry[hw_lif_id].active(!disable);
    txs_csr.dhs_sch_lif_base_map_sram.entry[hw_lif_id].write();

    SDK_TRACE_DEBUG("Programmed TXS dhs_sch_lif_base_map_sram.entry[lif:%u] base_addr:0x%lx block_size:%u active_cos:0x%x active:%u",
                    hw_lif_id, txs_hw_params->sched_table_offset, block_size,
                    lif_cos_bmp, !disable);


    // dhs_sch_lif_cfg_sram
    txs_csr.dhs_sch_lif_cfg_sram.entry[hw_lif_id].read();
    txs_csr.dhs_sch_lif_cfg_sram.entry[hw_lif_id].disabled(disable);
    txs_csr.dhs_sch_lif_cfg_sram.entry[hw_lif_id].max_weight(0x3ffff);
    txs_csr.dhs_sch_lif_cfg_sram.entry[hw_lif_id].min_weight(0x3ffff);
    txs_csr.dhs_sch_lif_cfg_sram.entry[hw_lif_id].q_grp_start(q_grp_start);
    txs_csr.dhs_sch_lif_cfg_sram.entry[hw_lif_id].q_grp_end(q_grp_end);
    txs_csr.dhs_sch_lif_cfg_sram.entry[hw_lif_id].write();

    SDK_TRACE_DEBUG("Programmed TXS dhs_sch_lif_cfg_sram.entry[lif:%u] q_grp_start:%u q_grp_end:%u disable:%u ",
                    hw_lif_id, q_grp_start, q_grp_end, disable);

    if (disable == 0) {
       // dhs_sch_qgrp_map_sram
       sch_qgrp_map_idx = txs_hw_params->sched_table_offset; // base_addr: offset for dhs_sch_qgrp_map_sram table: 16k entries
       for (uint32_t qg=q_grp_start; qg<=q_grp_end; qg++) {
          for (uint32_t bb=0; bb<block_size; bb++) {
             txs_csr.dhs_sch_qgrp_map_sram.entry[sch_qgrp_map_idx].valid(!disable);
             txs_csr.dhs_sch_qgrp_map_sram.entry[sch_qgrp_map_idx].q_grp(qg);
             txs_csr.dhs_sch_qgrp_map_sram.entry[sch_qgrp_map_idx].write();
             sch_qgrp_map_idx++;
             SDK_TRACE_DEBUG("Programmed TXS dhs_sch_qgrp_map_sram.entry[%u] q_grp:%u valid:%u ",
                       sch_qgrp_map_idx, qg, !disable);
          }
       }

       cos_idx   = 0;
       qgrp      = q_grp_start;
       qid_start = txs_hw_params->sched_table_offset;
       for (uint32_t cc=0; cc<16; cc++) {
          if ((lif_cos_bmp>>cc) & 0x1) {
             qid_end = qid_start + block_size - 1;
             if (qgrp < txs_csr.dhs_sch_qgrp_cfg_0_sram.get_depth_entry()) {
                txs_csr.dhs_sch_qgrp_cfg_0_sram.entry[qgrp].read();
                txs_csr.dhs_sch_qgrp_cfg_0_sram.entry[qgrp].no_fb_upd(0);
                txs_csr.dhs_sch_qgrp_cfg_0_sram.entry[qgrp].rx_sxdma(0);
                txs_csr.dhs_sch_qgrp_cfg_0_sram.entry[qgrp].cos(cc);
                txs_csr.dhs_sch_qgrp_cfg_0_sram.entry[qgrp].low_latency(0);
                txs_csr.dhs_sch_qgrp_cfg_0_sram.entry[qgrp].disabled(disable);
                txs_csr.dhs_sch_qgrp_cfg_0_sram.entry[qgrp].lif_idx(hw_lif_id);
                txs_csr.dhs_sch_qgrp_cfg_0_sram.entry[qgrp].max_weight(0x3fff);
                txs_csr.dhs_sch_qgrp_cfg_0_sram.entry[qgrp].min_weight(0x3fff);
                txs_csr.dhs_sch_qgrp_cfg_0_sram.entry[qgrp].def_weight(0x40);
                txs_csr.dhs_sch_qgrp_cfg_0_sram.entry[qgrp].lif_cos_base(txs_hw_params->sched_table_offset + cos_idx*block_size);
                txs_csr.dhs_sch_qgrp_cfg_0_sram.entry[qgrp].qid_start(qid_start);
                txs_csr.dhs_sch_qgrp_cfg_0_sram.entry[qgrp].qid_end(qid_end);
                txs_csr.dhs_sch_qgrp_cfg_0_sram.entry[qgrp].write();
                SDK_TRACE_DEBUG("Programmed TXS dhs_sch_qgrp_cfg_0_sram.entry[q_grp:0x%x] lif_idx:%u lif_cos_base:0x%x cos:%u qid_start:0x%x qid_end:0x%x disabled:%u ",
                       qgrp, hw_lif_id,
                       txs_hw_params->sched_table_offset + cos_idx*block_size,
                       cc, qid_start, qid_end, disable);
             } else {
                txs_csr.dhs_sch_qgrp_cfg_1_sram.entry[qgrp].read();
                txs_csr.dhs_sch_qgrp_cfg_1_sram.entry[qgrp].no_fb_upd(0);
                txs_csr.dhs_sch_qgrp_cfg_1_sram.entry[qgrp].rx_sxdma(0);
                txs_csr.dhs_sch_qgrp_cfg_1_sram.entry[qgrp].cos(cc);
                txs_csr.dhs_sch_qgrp_cfg_1_sram.entry[qgrp].low_latency(0);
                txs_csr.dhs_sch_qgrp_cfg_1_sram.entry[qgrp].disabled(disable);
                txs_csr.dhs_sch_qgrp_cfg_1_sram.entry[qgrp].lif_idx(hw_lif_id);
                txs_csr.dhs_sch_qgrp_cfg_1_sram.entry[qgrp].max_weight(0x3fff);
                txs_csr.dhs_sch_qgrp_cfg_1_sram.entry[qgrp].min_weight(0x3fff);
                txs_csr.dhs_sch_qgrp_cfg_1_sram.entry[qgrp].def_weight(0x40);
                txs_csr.dhs_sch_qgrp_cfg_1_sram.entry[qgrp].lif_cos_base(txs_hw_params->sched_table_offset + cos_idx*block_size);
                txs_csr.dhs_sch_qgrp_cfg_1_sram.entry[qgrp].qid_start(qid_start);
                txs_csr.dhs_sch_qgrp_cfg_1_sram.entry[qgrp].qid_end(qid_end);
                txs_csr.dhs_sch_qgrp_cfg_1_sram.entry[qgrp].write();
                SDK_TRACE_DEBUG("Programmed TXS dhs_sch_qgrp_cfg_1_sram entry[q_grp:0x%x] lif_idx:%u lif_cos_base:0x%lx cos:%u qid_start:0x%x qid_end:0x%x disabled:%u ",
                       qgrp, hw_lif_id,
                       txs_hw_params->sched_table_offset + cos_idx*block_size,
                       cc, qid_start, qid_end, disable);
             }
             cos_idx++;
             qid_start += block_size;
             qgrp++;
          }
       }
    }

    ///DEBUG get_txs_lif_qgrp_cfg(0, 0);
    ///DEBUG elb_txs_consistency_chk(0, 0);
    SDK_TRACE_DEBUG("Programmed sched-table-offset : %u and entries-per-cos : %u "
                    "and cos-bmp 0x%lx for hw-lif-id : %u", txs_hw_params->sched_table_offset,
                     txs_hw_params->num_entries_per_cos, lif_cos_bmp, hw_lif_id);

    return SDK_RET_OK;
}

sdk_ret_t
elba_txs_policer_lif_params_update (uint32_t hw_lif_id,
                            elba_txs_policer_lif_params_t *txs_hw_params)
{
    elb_top_csr_t &elb0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    elb_txs_csr_t &txs_csr = elb0.txs.txs;

    uint32_t  q_grp_start;
    uint32_t  q_grp_end;

    if ((hw_lif_id >= ELBA_TXS_MAX_TABLE_ENTRIES) ||
        (txs_hw_params->sched_table_end_offset >= ELBA_TXS_MAX_TABLE_ENTRIES)) {
        SDK_TRACE_ERR("Invalid parameters to function %u,%u",
                      hw_lif_id, txs_hw_params->sched_table_end_offset);
        return SDK_RET_INVALID_ARG;
    }

    // Program mapping from rate-limiter-table entry (indexed by hw-lif-id) to scheduler table entries.
    // The scheduler table entries (also called Rate-limiter-group, RLG)  will be paused when rate-limiter entry goes red.
    txs_csr.dhs_sch_rlid_map_sram.entry[hw_lif_id].read();
    txs_csr.dhs_sch_lif_cfg_sram.entry[hw_lif_id].read();
    q_grp_start = txs_csr.dhs_sch_lif_cfg_sram.entry[hw_lif_id].q_grp_start().convert_to<uint32_t>();
    q_grp_end   = txs_csr.dhs_sch_lif_cfg_sram.entry[hw_lif_id].q_grp_end().convert_to<uint32_t>();

    txs_csr.dhs_sch_rlid_map_sram.entry[hw_lif_id].q_grp_start(q_grp_start);
    txs_csr.dhs_sch_rlid_map_sram.entry[hw_lif_id].q_grp_end(q_grp_end);
    txs_csr.dhs_sch_rlid_map_sram.entry[hw_lif_id].write();

    SDK_TRACE_DEBUG("Programmed sched-table-start-offset %u and sched-table-end-offset %u"
                    "for hw-lif-id %u", txs_hw_params->sched_table_start_offset,
                     txs_hw_params->sched_table_end_offset, hw_lif_id);

    return SDK_RET_OK;
}

// *alloc_units  : number of 2k blocks (qids) / qgrp
// *alloc_offset : offset for dhs_sch_qgrp_map_sram table: 16k entries
sdk_ret_t
elba_txs_scheduler_tx_alloc (elba_txs_sched_lif_params_t *tx_params,
                             uint32_t *alloc_offset, uint32_t *alloc_units)
{
    sdk_ret_t     ret = SDK_RET_OK;
    uint32_t      total_qcount = 0;

    *alloc_offset = INVALID_INDEXER_INDEX;
    *alloc_units = 0;
    // Sched table can hold 2K queues per index and mandates new index for each cos.
    total_qcount = tx_params->total_qcount;
    *alloc_units  =  (total_qcount / ELBA_TXS_SCHEDULER_NUM_QUEUES_PER_ENTRY);
    *alloc_units +=
        ((total_qcount % ELBA_TXS_SCHEDULER_NUM_QUEUES_PER_ENTRY) ? 1 : 0);
    *alloc_units *=   sdk::lib::count_bits_set(tx_params->cos_bmp);

    if (*alloc_units > 0) {
        //Allocate consecutive alloc_unit num of entries in sched table.
        *alloc_offset =
            g_elba_state_pd->txs_scheduler_map_idxr()->Alloc(*alloc_units);
        if (*alloc_offset < 0) {
            ret = SDK_RET_NO_RESOURCE;
        }
    }

    SDK_TRACE_DEBUG("Programmed u qcount : %u  lif alloc_units : %u offset : %u",  tx_params->total_qcount, *alloc_units, *alloc_offset);

    return ret;
}

// *alloc_qgrp_units  : number of qgrp / lif, assuming 1 qgrp/cos
// *alloc_qgrp_offset : qgrp offset (at start of lif) for dhs_sch_qgrp_cfg_0/1_sram tables: 4k entries
sdk_ret_t
elba_txs_scheduler_tx_qgrp_alloc (elba_txs_sched_lif_params_t *tx_params,
                             uint32_t *alloc_qgrp_offset, uint32_t *alloc_qgrp_units)
{
    sdk_ret_t     ret = SDK_RET_OK;

    *alloc_qgrp_offset = INVALID_INDEXER_INDEX;
    *alloc_qgrp_units  = sdk::lib::count_bits_set(tx_params->cos_bmp);

    if (*alloc_qgrp_units > 0) {
        // 1 qgrp/cos => number of bits in active_cos
        *alloc_qgrp_offset = g_elba_state_pd->txs_scheduler_qgrp_idxr()->Alloc(*alloc_qgrp_units);
        if (*alloc_qgrp_offset < 0) {
           ret = SDK_RET_NO_RESOURCE;
        }
    }

    SDK_TRACE_DEBUG("Programmed alloc_qgrp_units : %u alloc_qgrp_offset : %u",  *alloc_qgrp_units, *alloc_qgrp_offset);

    return ret;
}

sdk_ret_t
elba_txs_scheduler_tx_dealloc (uint32_t alloc_offset, uint32_t alloc_units)
{
    sdk_ret_t     ret = SDK_RET_OK;
    g_elba_state_pd->txs_scheduler_map_idxr()->Free(alloc_offset, alloc_units);
    return ret;
}

sdk_ret_t
elba_txs_scheduler_tx_qgrp_dealloc (uint32_t alloc_qgrp_offset, uint32_t alloc_qgrp_units)
{
    sdk_ret_t     ret = SDK_RET_OK;
    g_elba_state_pd->txs_scheduler_qgrp_idxr()->Free(alloc_qgrp_offset, alloc_qgrp_units);
    return ret;
}


sdk_ret_t
elba_txs_scheduler_stats_get (elba_txs_scheduler_stats_t *scheduler_stats)
{
    elb_top_csr_t &elb0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    elb_txs_csr_t &txs_csr = elb0.txs.txs;
    uint16_t xon_status;

    txs_csr.cnt_sch_doorbell_set.read();
    txs_csr.cnt_sch_doorbell_clr.read();
    txs_csr.cnt_sch_pic_rl_stop.read();
    txs_csr.cnt_sch_pic_rl_start.read();
    txs_csr.cnt_sch_txdma_ptd_feedback.read();
    txs_csr.cnt_sch_txdma_phb_feedback.read();
    txs_csr.cnt_sch_txdma_sent.read();
    txs_csr.cnt_sch_sxdma_sent.read();

    txs_csr.sta_glb.read();

    xon_status = txs_csr.sta_glb.pb_xoff().convert_to<uint16_t>();

    scheduler_stats->doorbell_set_count =
        txs_csr.cnt_sch_doorbell_set.val().convert_to<uint64_t>();
    scheduler_stats->doorbell_clear_count =
        txs_csr.cnt_sch_doorbell_clr.val().convert_to<uint64_t>();
    scheduler_stats->ratelimit_start_count =
        txs_csr.cnt_sch_pic_rl_stop.val().convert_to<uint32_t>();
    scheduler_stats->ratelimit_stop_count =
        txs_csr.cnt_sch_pic_rl_start.val().convert_to<uint32_t>();
    scheduler_stats->txdma_ptd_feedback_count =
        txs_csr.cnt_sch_txdma_ptd_feedback.val().convert_to<uint32_t>();
    scheduler_stats->txdma_phb_feedback_count =
        txs_csr.cnt_sch_txdma_phb_feedback.val().convert_to<uint32_t>();
    scheduler_stats->txdma_sent_count =
        txs_csr.cnt_sch_txdma_sent.val().convert_to<uint32_t>();
    scheduler_stats->sxdma_sent_count =
        txs_csr.cnt_sch_sxdma_sent.val().convert_to<uint32_t>();
    for (unsigned i = 0; i < SDK_ARRAY_SIZE(scheduler_stats->cos_stats); i++) {
        scheduler_stats->cos_stats[i].cos = i;
        scheduler_stats->cos_stats[i].xon_status =
                                        (xon_status >> i) & 0x1 ? true : false;
    }

    return SDK_RET_OK;
}

}    // namespace elba
}    // namespace platform
}    // namespace sdk
