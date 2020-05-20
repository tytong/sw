// {C} Copyright 2019 Pensando Systems Inc. All rights reserved
#include <boost/algorithm/string/predicate.hpp>
#include "asic/asic.hpp"
#include "asic/cmn/asic_init.hpp"
#include "asic/cmn/asic_cfg.hpp"
#include "platform/elba/elba_tm_rw.hpp"
#include "platform/elba/elba_txs_scheduler.hpp"
#include "platform/elba/elba_quiesce.hpp"
#include "platform/elba/elba_barco_crypto.hpp"
#include "nic/sdk/asic/rw/asicrw.hpp"
#include "nic/sdk/lib/p4/p4_api.hpp"
#include "include/sdk/mem.hpp"
#include "platform/elba/elba_pxb_pcie.hpp"
#include "platform/elba/elba_state.hpp"
#include "platform/elba/elba_common.hpp"
#include "platform/elba/elba_hbm_rw.hpp"
#include "platform/elba/elba_tbl_rw.hpp"

#include "third-party/asic/elba/verif/apis/elb_npv_sw_api.h"
#include "third-party/asic/elba/verif/apis/elb_dpa_sw_api.h"
#include "third-party/asic/elba/verif/apis/elb_pics_sw_api.h"
#include "third-party/asic/elba/verif/apis/elb_pict_sw_api.h"
#include "third-party/asic/elba/verif/apis/elb_ppa_sw_api.h"
#include "third-party/asic/elba/verif/apis/elb_prd_sw_api.h"
#include "third-party/asic/elba/verif/apis/elb_psp_sw_api.h"
#include "third-party/asic/elba/verif/apis/elb_ptd_sw_api.h"
#include "third-party/asic/elba/verif/apis/elb_stg_sw_api.h"
#include "third-party/asic/elba/verif/apis/elb_wa_sw_api.h"
#include "third-party/asic/elba/model/elb_top/elb_top_csr.h"
#include "third-party/asic/elba/model/elb_top/elb_top_csr_defines.h"
#include "third-party/asic/elba/model/elb_wa/elb_wa_csr_define.h"
#include "third-party/asic/elba/model/elb_prd/elb_prd_csr.h"
#include "third-party/asic/elba/model/utils/elb_csr_py_if.h"
#include "third-party/asic/elba/verif/apis/elb_txs_sw_api.h"

using namespace sdk::asic;

namespace sdk {
namespace platform {
namespace elba {

class elba_state_pd *g_elba_state_pd;
/* elba_default_config_init
 * Load any bin files needed for initializing default configs
 */
sdk_ret_t
elba_default_config_init (asic_cfg_t *cfg)
{
    sdk_ret_t   ret = SDK_RET_OK;
    std::string hbm_full_path;
    std::string full_path;
    int         num_phases = 2;
    int         i;

    for (i = 0; i < num_phases; i++) {
        full_path =  std::string(cfg->cfg_path) + "/init_bins/" +
            cfg->default_config_dir + "/init_" + std::to_string(i) + "_bin" + "/elba";

        SDK_TRACE_DEBUG("Init phase %d Binaries dir: %s", i, full_path.c_str());

        // Check if directory is present
        if (access(full_path.c_str(), R_OK) < 0) {
            SDK_TRACE_DEBUG("Skipping init binaries");
            return SDK_RET_OK;
        }

        ret = sdk::asic::asic_load_config((char *)full_path.c_str());
        SDK_ASSERT_TRACE_RETURN((ret == SDK_RET_OK), ret,
                                "Error loading init phase %d binaries ret %d",
                                i, ret);

        // Now do any polling for init complete for this phase
        elba_tm_hw_config_load_poll(i);
    }

    return ret;
}


static sdk_ret_t
elba_timer_hbm_init (void)
{
    sdk_ret_t ret = SDK_RET_OK;
    uint64_t timer_key_hbm_base_addr;
    uint64_t timer_key_hbm_addr;
    uint64_t zero_data[8] = { 0 };

    timer_key_hbm_base_addr = get_mem_addr(MEM_REGION_TIMERS_NAME);
    SDK_TRACE_DEBUG("HBM timer key base addr %lx", timer_key_hbm_base_addr);
    timer_key_hbm_addr = timer_key_hbm_base_addr;
    while (timer_key_hbm_addr < timer_key_hbm_base_addr +
                                ELBA_TIMER_HBM_KEY_SPACE) {
        sdk::asic::asic_mem_write(timer_key_hbm_addr, (uint8_t *)zero_data,
                                  sizeof(zero_data),
                                  ASIC_WRITE_MODE_WRITE_THRU);
        timer_key_hbm_addr += sizeof(zero_data);
    }

    return ret;
}

sdk_ret_t
elba_pgm_init (void)
{
    sdk_ret_t      ret;
    std::string    full_path;
    asic_cfg_t     *cfg = g_elba_state_pd->cfg();

    for (uint8_t i = 0; i < cfg->num_pgm_cfgs; i++) {
        full_path =  std::string(cfg->cfg_path) + "/" + cfg->pgm_name +
            "/" + cfg->pgm_cfg[i].path;
        SDK_TRACE_DEBUG("Loading programs from dir %s", full_path.c_str());

        // check if directory is present
        if (access(full_path.c_str(), R_OK) < 0) {
            SDK_TRACE_ERR("%s not present/no read permissions",
                          full_path.c_str());
            return SDK_RET_ERR;
        }
        ret = sdk::asic::asic_load_config((char *)full_path.c_str());
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR("Failed to load config %s", full_path.c_str());
            return ret;
        }
    }

    return SDK_RET_OK;
}


static sdk_ret_t
elba_cache_init (asic_cfg_t *cfg)
{
    sdk_ret_t ret = SDK_RET_OK;

    // Program Global parameters of the cache.
    ret = elba_hbm_cache_init(cfg);
    if (ret != SDK_RET_OK) {
        return ret;
    }

    // Process all the regions.
    ret = elba_hbm_cache_regions_init();
    if (ret != SDK_RET_OK) {
        return ret;
    }

    return ret;
}

/*
 * ASIC teams wants to disable Error recovery of Seq ID check pRDMA,
 * as this recovery path is not tested thoroughly, and we might be
 * runing into for an outstanding issue is suspicion.
 * Disabling from SDK for now, but Helen will commit this
 * into prd_asic_init api called from SDK, then this will be removed
 */
static sdk_ret_t
elba_prd_init(void)
{
    elb_top_csr_t &cap0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    elb_pr_csr_t &pr_csr = cap0.pr.pr;

    pr_csr.prd.cfg_ctrl.read();
    pr_csr.prd.cfg_ctrl.pkt_phv_sync_err_recovery_en(0);
    pr_csr.prd.cfg_ctrl.write();
    SDK_TRACE_DEBUG("Disabled pkt_phv_sync_err_recovery_en in pr_prd_cfg_ctrl");

    return SDK_RET_OK;
}

/*
 * For Capri backward compatability, disable the Qstate address shift
 */
static sdk_ret_t
elba_psp_init(void)
{
    elb_top_csr_t &cap0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    elb_psp_csr_t &pr_psp_csr = cap0.pr.pr.psp;

    pr_psp_csr.cfg_qstate_map_rsp.read();
    pr_psp_csr.cfg_qstate_map_rsp.addr_shift_enable(0);
    pr_psp_csr.cfg_qstate_map_rsp.addr_shift_value(0);
    pr_psp_csr.cfg_qstate_map_rsp.write();
    SDK_TRACE_DEBUG("Disabled Qstate address shift for Elba");

    return SDK_RET_OK;
}

/*
 * For Capri backward compatability, disable the Qstate address shift
 */
static sdk_ret_t
elba_npv_init(void)
{
    elb_top_csr_t & cap0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    elb_psp_csr_t &pt_npv_csr = cap0.pt.pt.psp;

    pt_npv_csr.cfg_qstate_map_rsp.read();
    pt_npv_csr.cfg_qstate_map_rsp.addr_shift_enable(0);
    pt_npv_csr.cfg_qstate_map_rsp.addr_shift_value(0);
    pt_npv_csr.cfg_qstate_map_rsp.write();
    SDK_TRACE_DEBUG("Disabled Qstate address shift for Elba");

    return SDK_RET_OK;
}

static sdk_ret_t
elba_sxdma_psp_init(void)
{
    elb_top_csr_t &cap0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    elb_psp_csr_t &pt_npv_csr = cap0.xd.pt.psp;

    pt_npv_csr.cfg_qstate_map_rsp.read();
    pt_npv_csr.cfg_qstate_map_rsp.addr_shift_enable(0);
    pt_npv_csr.cfg_qstate_map_rsp.addr_shift_value(0);
    pt_npv_csr.cfg_qstate_map_rsp.write();
    SDK_TRACE_DEBUG("Disabled Qstate address shift for Elba sxdma");

    return SDK_RET_OK;
}

static sdk_ret_t
elba_repl_init (asic_cfg_t *cfg)
{
#ifdef MEM_REGION_MCAST_REPL_NAME
    uint64_t hbm_repl_table_offset = get_mem_offset(MEM_REGION_MCAST_REPL_NAME);
    if (hbm_repl_table_offset != INVALID_MEM_ADDRESS) {
        elba_tm_repl_table_base_addr_set(hbm_repl_table_offset /
                                         SDK_ASIC_REPL_ENTRY_WIDTH);
        elba_tm_repl_table_token_size_set(cfg->repl_entry_width * 8);
    }
#endif

    return SDK_RET_OK;
}

typedef struct block_info_s {
    int  inst_count;
    void (*soft_reset)(int chip_id, int inst);
    void (*init_start)(int chip_id, int inst);
    void (*init_done) (int chip_id, int inst);
} block_info_t;

#define MAX_INIT_BLOCKS 10

block_info_t blocks_info[MAX_INIT_BLOCKS];

static void
elba_block_info_init(void)
{
    blocks_info[0].inst_count = 2;
    blocks_info[0].soft_reset = elb_npv_soft_reset;
    blocks_info[0].init_start = elb_npv_init_start;
    blocks_info[0].init_done  = elb_npv_init_done;

    blocks_info[1].inst_count = 2;
    blocks_info[1].soft_reset = elb_dpa_soft_reset;
    blocks_info[1].init_start = elb_dpa_init_start;
    blocks_info[1].init_done  = elb_dpa_init_done;

    blocks_info[2].inst_count = 4;
    blocks_info[2].soft_reset = elb_pics_soft_reset;
    blocks_info[2].init_start = elb_pics_init_start;
    blocks_info[2].init_done  = elb_pics_init_done;

    blocks_info[3].inst_count = 2;
    blocks_info[3].soft_reset = elb_pict_soft_reset;
    blocks_info[3].init_start = elb_pict_init_start;
    blocks_info[3].init_done  = elb_pict_init_done;

    blocks_info[4].inst_count = 2;
    blocks_info[4].soft_reset = elb_ppa_soft_reset;
    blocks_info[4].init_start = elb_ppa_init_start;
    blocks_info[4].init_done  = elb_ppa_init_done;

    blocks_info[5].inst_count = 1;
    blocks_info[5].soft_reset = elb_prd_soft_reset;
    blocks_info[5].init_start = elb_prd_init_start;
    blocks_info[5].init_done  = elb_prd_init_done;

    blocks_info[6].inst_count = 1;
    blocks_info[6].soft_reset = elb_psp_soft_reset;
    blocks_info[6].init_start = elb_psp_init_start;
    blocks_info[6].init_done  = elb_psp_init_done;

    blocks_info[7].inst_count = 2;
    blocks_info[7].soft_reset = elb_ptd_soft_reset;
    blocks_info[7].init_start = elb_ptd_init_start;
    blocks_info[7].init_done  = elb_ptd_init_done;

    blocks_info[8].inst_count = 28;
    blocks_info[8].soft_reset = elb_stg_soft_reset;
    blocks_info[8].init_start = elb_stg_init_start;
    blocks_info[8].init_done  = elb_stg_init_done;

    blocks_info[9].inst_count = 1;
    blocks_info[9].soft_reset = elb_wa_soft_reset;
    blocks_info[9].init_start = elb_wa_init_start;
    blocks_info[9].init_done  = elb_wa_init_done;

    return;
}

//------------------------------------------------------------------------------
// Reset all the elba blocks
//------------------------------------------------------------------------------
static sdk_ret_t
elba_block_reset (asic_cfg_t *cfg)
{
    sdk_ret_t    ret         = SDK_RET_OK;
    int          chip_id     = 0;
    block_info_t *block_info = NULL;

    for (int block = 0; block < MAX_INIT_BLOCKS; ++block) {
        block_info = &blocks_info[block];

        for(int inst = 0; inst < block_info->inst_count; ++inst) {
            block_info->soft_reset(chip_id, inst);
        }
    }

    return ret;
}

//------------------------------------------------------------------------------
// Start the init of blocks
//------------------------------------------------------------------------------
static sdk_ret_t
elba_block_init_start (asic_cfg_t *cfg)
{
    sdk_ret_t    ret         = SDK_RET_OK;
    int          chip_id     = 0;
    block_info_t *block_info = NULL;

    for (int block = 0; block < MAX_INIT_BLOCKS; ++block) {
        block_info = &blocks_info[block];

        for(int inst = 0; inst < block_info->inst_count; ++inst) {
            block_info->init_start(chip_id, inst);
        }
    }

    return ret;
}

//------------------------------------------------------------------------------
// Wait for the blocks to initialize
//------------------------------------------------------------------------------
static sdk_ret_t
elba_block_init_done (asic_cfg_t *cfg)
{
    sdk_ret_t ret = SDK_RET_OK;
    int chip_id = 0;
    block_info_t *block_info = NULL;

    for (int block = 0; block < MAX_INIT_BLOCKS; ++block) {
        block_info = &blocks_info[block];

        for(int inst = 0; inst < block_info->inst_count; ++inst) {
            block_info->init_done(chip_id, inst);
        }
    }

    return ret;
}

//------------------------------------------------------------------------------
// Init all the elba blocks owned by SDK
//------------------------------------------------------------------------------
sdk_ret_t
elba_block_init (asic_cfg_t *cfg)
{
    sdk_ret_t ret = SDK_RET_OK;

    // initialize block info
    elba_block_info_init();

    // soft reset
    ret = elba_block_reset(cfg);
    if (ret != SDK_RET_OK) {
        return ret;
    }

    // init blocks
    ret = elba_block_init_start(cfg);
    if (ret != SDK_RET_OK) {
        return ret;
    }

    // wait for blocks to be inited
    ret = elba_block_init_done(cfg);
    if (ret != SDK_RET_OK) {
        return ret;
    }

    return ret;
}

//------------------------------------------------------------------------------
// perform all the ELBA specific initialization
// - link all the P4 programs, by resolving symbols, labels etc.
// - load the P4/P4+ programs in HBM
// - do all the parser/deparser related register programming
// - do all the table configuration related register programming
//------------------------------------------------------------------------------
sdk_ret_t
elba_init (asic_cfg_t *cfg)
{
    sdk_ret_t ret;
#if 0
    int sxdma_lifs[] = {35};
#endif
    char *tm_binary_init = getenv("ELBA_TM_BINARY_INIT");

    SDK_ASSERT_TRACE_RETURN((cfg != NULL), SDK_RET_INVALID_ARG, "Invalid cfg");
    SDK_TRACE_DEBUG("Initializing Elba");

    ret = elba_state_pd_init(cfg);
    SDK_ASSERT_TRACE_RETURN((ret == SDK_RET_OK), ret,
                            "elba_state_pd_init failure, err : %d", ret);

    ret = elba_table_rw_init(cfg);
    SDK_ASSERT_TRACE_RETURN((ret == SDK_RET_OK), ret,
                            "elba_table_rw_init failure, err : %d", ret);

    ret = sdk::asic::asic_hbm_regions_init(cfg);
    SDK_ASSERT_TRACE_RETURN((ret == SDK_RET_OK), ret,
                            "Elba HBM region init failure, err : %d", ret);

    ret = elba_timer_hbm_init();
    SDK_ASSERT_TRACE_RETURN((ret == SDK_RET_OK), ret,
                            "Elba HBM timer init failure, err : %d", ret);

    ret = elba_block_init(cfg);
    SDK_ASSERT_TRACE_RETURN((ret == SDK_RET_OK), ret,
                            "Elba block region init failure, err : %d", ret);

    ret = elba_cache_init(cfg);
    SDK_ASSERT_TRACE_RETURN((ret == SDK_RET_OK), ret,
                            "Elba cache init failure, err : %d", ret);

    // do asic init before overwriting with the default configs
    ret = elba_tm_asic_init();
    SDK_ASSERT_TRACE_RETURN((ret == SDK_RET_OK), ret,
                            "Elba TM ASIC init failure, err : %d", ret);

    ret = elba_txs_scheduler_init(cfg->admin_cos, cfg);
    SDK_ASSERT_TRACE_RETURN((ret == SDK_RET_OK), ret,
                             "Elba scheduler init failure, err : %d", ret);

    // Init the LIF/QOS-GROUP/Scheduler tabels for default: 512 LIFS,
    // 8 COSs, each map to unique QGroup with 2K QIDs
#if 0
    // replaced by txs cfg
    elb_txs_cfg_sch_qgrp_tab(0, 0, 512, 0xff, 1, 4,
                             sizeof(sxdma_lifs)/sizeof(sxdma_lifs[0]),
                             sxdma_lifs);
    SDK_TRACE_DEBUG("Elba scheduler init Done");
#endif

    // Call PXB/PCIE init only in MODEL and RTL simulation
    // This will be done by PCIe manager for the actual chip
    if (cfg->platform == platform_type_t::PLATFORM_TYPE_SIM ||
        cfg->platform == platform_type_t::PLATFORM_TYPE_RTL) {
        ret = elba_pxb_pcie_init();
        SDK_ASSERT_TRACE_RETURN((ret == SDK_RET_OK), ret,
                                "PXB/PCIE init failure, err : %d", ret);
    }

    if(tm_binary_init) {
      SDK_TRACE_DEBUG("Elba TM Binary Init ");
      ret = elba_default_config_init(cfg);
      SDK_ASSERT_TRACE_RETURN((ret == SDK_RET_OK), ret,
                            "Elba default config init failure, err: %d", ret);

    } /* else */ {
      SDK_TRACE_DEBUG("Elba TM Real Init ");
      ret = elba_tm_init(cfg->catalog,
                       &cfg->device_profile->qos_profile);
      SDK_ASSERT_TRACE_RETURN((ret == SDK_RET_OK), ret,
                            "Elba TM init failure, err : %d", ret);
#if 0
      /* TBD-ELBA-REBASE */
      ret = elba_pf_init();
      SDK_ASSERT_TRACE_RETURN((ret == SDK_RET_OK), ret,
        "Error intializing pbc ret %d", ret);

      ret = elba_tm_port_program_uplink_byte_count();
      SDK_ASSERT_TRACE_RETURN((ret == SDK_RET_OK), ret,
                            "Elba TM port program defaults, err : %d", ret);
#endif
    }
    ret = elba_repl_init(cfg);
    SDK_ASSERT_TRACE_RETURN((ret == SDK_RET_OK), ret,
                            "Elba replication init failure, err : %d", ret);

    if(cfg->platform != platform_type_t::PLATFORM_TYPE_HAPS) {
      ret = elba_barco_crypto_init(cfg->platform);
      SDK_ASSERT_TRACE_RETURN((ret == SDK_RET_OK), ret,
                            "Elba barco crypto init failure, err : %d", ret);
    }

#if 0
    if (!cfg->loader_info_file.empty()) {
        sdk::p4::p4_list_program_addr(
            cfg->cfg_path.c_str(), cfg->loader_info_file.c_str());
    }
#endif

    sdk::p4::p4_dump_program_info(cfg->cfg_path.c_str());

    ret = elba_quiesce_init();
    SDK_ASSERT_TRACE_RETURN((ret == SDK_RET_OK), ret,
                            "Elba quiesce init failure, err : %d", ret);

    ret = elba_prd_init();
    SDK_ASSERT_TRACE_RETURN((ret == SDK_RET_OK), ret,
                            "Elba PRD init failure, err : %d", ret);
    ret = elba_psp_init();
    SDK_ASSERT_TRACE_RETURN((ret == SDK_RET_OK), ret,
                            "Elba PSP init failure, err : %d", ret);

    ret = elba_npv_init();
    SDK_ASSERT_TRACE_RETURN((ret == SDK_RET_OK), ret,
                            "Elba NPV init failure, err : %d", ret);

    ret = elba_sxdma_psp_init();
    SDK_ASSERT_TRACE_RETURN((ret == SDK_RET_OK), ret,
                            "Elba sxdma psp init failure, err : %d", ret);

    ret = elba_te_enable_capri_mode();
    SDK_ASSERT_TRACE_RETURN((ret == SDK_RET_OK), ret,
                 "Elba enable capri mode failure, err : %d", ret);

    ret = elba_ipsec_inline_enable();
    SDK_ASSERT_TRACE_RETURN((ret == SDK_RET_OK), ret,
                 "Elba enable ipsec_inline failure, err : %d", ret);

#if 0   /* TBD-ELBA-REBASE: */
#ifndef ELEKTRA
    if (hal::pd::elba_barco_crypto_init(cfg->platform) != HAL_RET_OK) {
        ret = SDK_RET_INVALID_ARG;
    }
    else {
        ret = SDK_RET_OK;
    }
    SDK_ASSERT_TRACE_RETURN((ret == SDK_RET_OK), ret,
                 "Elba Barco initialization failure, err : %d", ret);
#endif
#endif

    if (cfg->completion_func) {
        cfg->completion_func(sdk::SDK_STATUS_ASIC_INIT_DONE);
    }

    return SDK_RET_OK;
}

uint64_t
elba_local_dbaddr (void)
{
    uint64_t db_addr =
#ifdef __aarch64__
       ELB_ADDR_BASE_DB_WA_OFFSET +
#endif // __aarch64__
       ELB_WA_CSR_DHS_LOCAL_DOORBELL_BYTE_ADDRESS;

    return db_addr;
}

uint64_t
elba_local_db32_addr (void)
{
    uint64_t db_addr =
#ifdef __aarch64__
       ELB_ADDR_BASE_DB_WA_OFFSET +
#endif // __aarch64__
       ELB_WA_CSR_DHS_32B_DOORBELL_BYTE_ADDRESS;

    return db_addr;
}

uint64_t
elba_host_dbaddr (void)
{
    return ELB_WA_CSR_DHS_HOST_DOORBELL_BYTE_ADDRESS;
}

} // namespace elba
} // namespace platform
} // namespace sdk
