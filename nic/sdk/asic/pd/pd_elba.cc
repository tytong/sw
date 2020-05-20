// {C} Copyright 2020 Pensando Systems Inc. All rights reserved

#include "platform/elba/elba_p4.hpp"
#include "platform/elba/elba_hbm_rw.hpp"
#include "platform/elba/elba_tbl_rw.hpp"
#include "platform/elba/elba_txs_scheduler.hpp"
#include "platform/elba/elba_qstate.hpp"
#include "platform/elba/elba_state.hpp"
#include "platform/elba/elba_mon.hpp"
#include "platform/elba/elba_tm_utils.hpp"
#include "platform/elba/elba_tm_rw.hpp"
#include "platform/elba/elba_sw_phv.hpp"
#include "platform/elba/elba_quiesce.hpp"
#include "platform/elba/elba_pxb_pcie.hpp"
#include "platform/elba/csrint/csr_init.hpp"
#include "platform/elba/elba_cfg.hpp"
#include "asic/asic.hpp"
#include "asic/pd/pd.hpp"
#include "asic/cmn/asic_state.hpp"
#include "asic/port.hpp"
#include "asic/pd/pd_internal.hpp"
#include "lib/utils/time_profile.hpp"
#include "platform/utils/mpartition.hpp"
#include "platform/elba/elba_toeplitz.hpp"
#include "include/sdk/crypto_apis.hpp"
#include "asic/cmn/asic_hbm.hpp"
#include "platform/elba/elba_barco_rings.hpp"
#include "platform/elba/elba_barco_sym_apis.hpp"
#include "platform/elba/elba_barco_asym_apis.hpp"
#include "platform/elba/elba_barco_crypto.hpp"
// TODO: move out pipeline related code out of sdk
#if defined(APOLLO) || defined(ARTEMIS) || defined(APULU) || defined(ATHENA)
#include "gen/p4gen/p4plus_rxdma/include/p4plus_rxdma_p4pd.h"
#include "gen/p4gen/p4plus_txdma/include/p4plus_txdma_p4pd.h"
#endif

using namespace sdk::platform::elba;

namespace sdk {
namespace asic {
namespace pd {

extern bool g_mock_mode_;

#define MAX_IPSEC_PAD_SIZE      256
#define CRYPTO_KEY_SIZE_MAX     64  /* 2 * 256 bit key */

typedef struct crypto_key_s {
    crypto_key_type_t       key_type;
    uint32_t                key_size;
    uint8_t                 key[CRYPTO_KEY_SIZE_MAX];
} __PACK__ crypto_key_t;

typedef struct crypto_asym_key_s {
    uint64_t                key_param_list;     /* Address to the DMA
                                                   descriptor that contains the
                                                   key/param */
    uint32_t                command_reg;        /* Command associated with this
                                                   key descriptor */
    uint32_t                reserved;
} __PACK__ crypto_asym_key_t;

static inline bool
asicpd_p4plus_hbm_write (uint64_t addr, uint8_t* data, uint32_t size_in_bytes,
                  p4plus_cache_action_t action)
{
    sdk_ret_t rv = sdk::asic::asic_mem_write(addr, data, size_in_bytes);
    if (action != P4PLUS_CACHE_ACTION_NONE) {
        sdk::asic::pd::asicpd_p4plus_invalidate_cache(addr, size_in_bytes, action);
    }
    return rv == SDK_RET_OK ? true : false;
}

sdk_ret_t
asic_program_hbm_table_base_addr (int tableid, int stage_tableid,
                                  char *tablename, int stage,
                                  int pipe)
{
    elba_program_hbm_table_base_addr(tableid, stage_tableid, tablename,
                                     stage, pipe);
    return SDK_RET_OK;
}

void
asicpd_p4_invalidate_cache (uint64_t addr, uint32_t size_in_bytes,
                            p4pd_table_cache_t cache)
{
    return p4_invalidate_cache(addr, size_in_bytes, cache);
}

bool
asicpd_p4plus_invalidate_cache (uint64_t addr, uint32_t size_in_bytes,
                                p4plus_cache_action_t action)
{
    return p4plus_invalidate_cache(addr, size_in_bytes, action);
}

bool
asicpd_p4plus_invalidate_cache_all(p4plus_cache_action_t action)
{
    return p4plus_invalidate_cache_all(action);
}

uint8_t
asicpd_get_action_pc (uint32_t tableid, uint8_t actionid)
{
    return elba_get_action_pc(tableid, actionid);
}

uint8_t
asicpd_get_action_id (uint32_t tableid, uint8_t actionpc)
{
    return elba_get_action_id(tableid, actionpc);
}

int
asicpd_table_hw_entry_read (uint32_t tableid, uint32_t index, uint8_t  *hwentry,
                            uint16_t *hwentry_bit_len)
{
    int ret;
    uint32_t oflow_parent_tbl_depth = 0;
    p4pd_table_properties_t tbl_ctx;
    p4_table_mem_layout_t cap_tbl_info = {0};

    p4pd_global_table_properties_get(tableid, &tbl_ctx);
    assert(tbl_ctx.table_location != P4_TBL_LOCATION_HBM);
    if (tbl_ctx.is_oflow_table) {
        p4pd_table_properties_t ofl_tbl_parent_ctx;
        p4pd_table_properties_get(tbl_ctx.oflow_table_id, &ofl_tbl_parent_ctx);
        oflow_parent_tbl_depth = ofl_tbl_parent_ctx.tabledepth;
    }
    asicpd_copy_table_info(&cap_tbl_info, &tbl_ctx.sram_layout, &tbl_ctx);

    if (g_mock_mode_) {
        ret = asicpd_table_entry_read(tableid, index,
                hwentry, hwentry_bit_len);
    } else {
        ret = elba_table_hw_entry_read(tableid, index,
                                       hwentry, hwentry_bit_len,
                                       cap_tbl_info, tbl_ctx.gress,
                                       tbl_ctx.is_oflow_table,
                                       (tbl_ctx.gress == P4_GRESS_INGRESS),
                                       oflow_parent_tbl_depth);
    }

    return ret;
}

int
asicpd_tcam_table_hw_entry_read (uint32_t tableid, uint32_t index,
                                 uint8_t  *trit_x, uint8_t  *trit_y,
                                 uint16_t *hwentry_bit_len)
{
    int ret;
    p4pd_table_properties_t tbl_ctx;
    p4_table_mem_layout_t cap_tbl_info = {0};

    p4pd_global_table_properties_get(tableid, &tbl_ctx);
    assert(tbl_ctx.table_location != P4_TBL_LOCATION_HBM);
    asicpd_copy_table_info(&cap_tbl_info, &tbl_ctx.tcam_layout, &tbl_ctx);

    if (g_mock_mode_) {
        ret = asicpd_tcam_table_entry_read(tableid, index, trit_x, trit_y,
                                           hwentry_bit_len);
    } else {
        ret = elba_tcam_table_hw_entry_read(tableid, index, trit_x, trit_y,
                                            hwentry_bit_len, cap_tbl_info,
                                            (tbl_ctx.gress ==
                                             P4_GRESS_INGRESS));
    }

    return ret;
}

int
asicpd_table_entry_write (uint32_t tableid, uint32_t index, uint8_t *hwentry,
                          uint16_t hwentry_bit_len, uint8_t  *hwentry_mask)
{
    int ret;
    uint32_t oflow_parent_tbl_depth = 0;
    p4pd_table_properties_t tbl_ctx;
    p4_table_mem_layout_t cap_tbl_info = {0};

    p4pd_global_table_properties_get(tableid, &tbl_ctx);
    assert(tbl_ctx.table_location != P4_TBL_LOCATION_HBM);
    if (tbl_ctx.is_oflow_table) {
        p4pd_table_properties_t ofl_tbl_parent_ctx;
        p4pd_table_properties_get(tbl_ctx.oflow_table_id, &ofl_tbl_parent_ctx);
        oflow_parent_tbl_depth = ofl_tbl_parent_ctx.tabledepth;
    }
    asicpd_copy_table_info(&cap_tbl_info, &tbl_ctx.sram_layout, &tbl_ctx);
    ret = elba_table_entry_write(tableid, index,
                                  hwentry, hwentry_mask, hwentry_bit_len,
                                  cap_tbl_info, tbl_ctx.gress,
                                  tbl_ctx.is_oflow_table,
                                  (tbl_ctx.gress == P4_GRESS_INGRESS),
                                  oflow_parent_tbl_depth);

#if SDK_LOG_TABLE_WRITE
    if ((tbl_ctx.table_type == P4_TBL_TYPE_HASH)     ||
        (tbl_ctx.table_type == P4_TBL_TYPE_HASHTCAM) ||
        (tbl_ctx.table_type == P4_TBL_TYPE_INDEX)) {
        char    buffer[2048];
        memset(buffer, 0, sizeof(buffer));

        uint8_t key[128] = {0}; // Atmost key is 64B. Assuming each
                                // key byte has worst case byte padding
        uint8_t keymask[128] = {0};
        uint8_t data[128] = {0};
        SDK_TRACE_DEBUG("%s", "Read last installed table entry back into table "
                        "key and action structures");

        p4pd_global_entry_read(tableid, index, (void*)key,
                               (void*)keymask, (void*)data);

        p4pd_global_table_ds_decoded_string_get(tableid, index, (void*)key,
                                                (void*)keymask, (void*)data,
                                                buffer, sizeof(buffer));
        SDK_TRACE_DEBUG("%s", buffer);
    }
#endif

    return ret;
}

sdk_ret_t
asicpd_read_table_constant (uint32_t tableid, uint64_t *value)
{
    p4pd_table_properties_t       tbl_ctx;

    p4pd_global_table_properties_get(tableid, &tbl_ctx);
    if ((tableid >= p4pd_tableid_min_get()) &&
        (tableid <= p4pd_tableid_max_get())) {
        elba_table_constant_read(value, tbl_ctx.stage, tbl_ctx.stage_tableid,
                                 (tbl_ctx.gress == P4_GRESS_INGRESS) ?
                                 P4_PIPELINE_INGRESS : P4_PIPELINE_EGRESS);
    } else if ((tableid >= p4pd_rxdma_tableid_min_get()) &&
               (tableid <= p4pd_rxdma_tableid_max_get())) {
        elba_table_constant_read(value, tbl_ctx.stage, tbl_ctx.stage_tableid,
                                 P4_PIPELINE_RXDMA);
    } else if ((tableid >= p4pd_txdma_tableid_min_get()) &&
               (tableid <= p4pd_txdma_tableid_max_get())) {
        elba_table_constant_read(value, tbl_ctx.stage, tbl_ctx.stage_tableid,
                                 P4_PIPELINE_TXDMA);
    } else {
        SDK_ASSERT_RETURN(FALSE, SDK_RET_INVALID_ARG);
    }
    return SDK_RET_OK;
}

sdk_ret_t
asicpd_program_table_constant (uint32_t tableid, uint64_t const_value)
{
    p4pd_table_properties_t tbl_ctx;

    p4pd_global_table_properties_get(tableid, &tbl_ctx);
    if ((tableid >= p4pd_tableid_min_get()) &&
        (tableid <= p4pd_tableid_max_get())) {
        elba_table_constant_write(const_value, tbl_ctx.stage,
                                  tbl_ctx.stage_tableid,
                                  (tbl_ctx.gress == P4_GRESS_INGRESS) ?
                                  P4_PIPELINE_INGRESS : P4_PIPELINE_EGRESS);
    } else if ((tableid >= p4pd_rxdma_tableid_min_get()) &&
               (tableid <= p4pd_rxdma_tableid_max_get())) {
        elba_table_constant_write(const_value, tbl_ctx.stage,
                                  tbl_ctx.stage_tableid,
                                  P4_PIPELINE_RXDMA);
    } else if ((tableid >= p4pd_txdma_tableid_min_get()) &&
               (tableid <= p4pd_txdma_tableid_max_get())) {
        elba_table_constant_write(const_value, tbl_ctx.stage,
                                  tbl_ctx.stage_tableid,
                                  P4_PIPELINE_TXDMA);
    } else {
        SDK_ASSERT_RETURN(FALSE, SDK_RET_INVALID_ARG);
    }
    return SDK_RET_OK;
}

sdk_ret_t
asicpd_program_table_thread_constant (uint32_t tableid, uint8_t table_thread_id,
                                      uint64_t const_value)
{
    p4pd_table_properties_t       tbl_ctx;

    p4pd_table_properties_get(tableid, &tbl_ctx);
    if (table_thread_id < tbl_ctx.table_thread_count) {
        uint8_t tid = 0;
        if (table_thread_id != 0) {
            tid = tbl_ctx.thread_table_id[table_thread_id];
        } else {
            tid = tbl_ctx.stage_tableid;
        }
        elba_table_constant_write(const_value, tbl_ctx.stage, tid,
                                  (tbl_ctx.gress == P4_GRESS_INGRESS) ?
                                  P4_PIPELINE_INGRESS : P4_PIPELINE_EGRESS);
    }

    return SDK_RET_OK;
}

sdk_ret_t
asicpd_deparser_init (void)
{
    elba_deparser_init(TM_PORT_INGRESS, TM_PORT_EGRESS);

    return SDK_RET_OK;
}

int
asicpd_table_entry_read (uint32_t tableid, uint32_t index, uint8_t  *hwentry,
                         uint16_t *hwentry_bit_len)
{
    int ret;
    uint32_t oflow_parent_tbl_depth = 0;
    p4pd_table_properties_t tbl_ctx;
    p4_table_mem_layout_t cap_tbl_info = {0};

    p4pd_global_table_properties_get(tableid, &tbl_ctx);
    assert(tbl_ctx.table_location != P4_TBL_LOCATION_HBM);
    if (tbl_ctx.is_oflow_table) {
        p4pd_table_properties_t ofl_tbl_parent_ctx;
        p4pd_table_properties_get(tbl_ctx.oflow_table_id, &ofl_tbl_parent_ctx);
        oflow_parent_tbl_depth = ofl_tbl_parent_ctx.tabledepth;
    }
    asicpd_copy_table_info(&cap_tbl_info, &tbl_ctx.sram_layout, &tbl_ctx);
    ret = elba_table_entry_read(tableid, index, hwentry, hwentry_bit_len,
                                cap_tbl_info, tbl_ctx.gress,
                                tbl_ctx.is_oflow_table, oflow_parent_tbl_depth);

    return (ret);
}

int
asicpd_tcam_table_entry_write (uint32_t tableid, uint32_t index,
                               uint8_t  *trit_x, uint8_t  *trit_y,
                               uint16_t hwentry_bit_len)
{
    int ret;
    p4pd_table_properties_t tbl_ctx;
    p4_table_mem_layout_t cap_tbl_info = {0};

    p4pd_global_table_properties_get(tableid, &tbl_ctx);
    assert(tbl_ctx.table_location != P4_TBL_LOCATION_HBM);
    asicpd_copy_table_info(&cap_tbl_info, &tbl_ctx.tcam_layout, &tbl_ctx);
    ret = elba_tcam_table_entry_write(tableid, index, trit_x, trit_y,
                                      hwentry_bit_len,
                                      cap_tbl_info, tbl_ctx.gress,
                                      (tbl_ctx.gress == P4_GRESS_INGRESS));

#if SDK_LOG_TABLE_WRITE
    if ((tbl_ctx.table_type != P4_TBL_TYPE_HASH) &&
        (tbl_ctx.table_type != P4_TBL_TYPE_INDEX)) {
        char    buffer[2048];
        memset(buffer, 0, sizeof(buffer));

        uint8_t key[128] = {0}; // Atmost key is 64B. Assuming each
                                // key byte has worst case byte padding
        uint8_t keymask[128] = {0};
        uint8_t data[128] = {0};
        SDK_TRACE_DEBUG("%s", "Read last installed table entry back into table "
                        "key and action structures");
        p4pd_global_entry_read(tableid, index, (void*)key,
                               (void*)keymask, (void*)data);
        p4pd_global_table_ds_decoded_string_get(tableid, index,
                                                (void*)key, (void*)keymask,
                                                (void*)data, buffer,
                                                sizeof(buffer));
        SDK_TRACE_DEBUG("%s", buffer);
    }
#endif

    return ret;
}

int
asicpd_tcam_table_entry_read (uint32_t tableid, uint32_t index,
                              uint8_t  *trit_x, uint8_t  *trit_y,
                              uint16_t *hwentry_bit_len)
{
    int ret;
    p4pd_table_properties_t tbl_ctx;
    p4_table_mem_layout_t cap_tbl_info = {0};

    p4pd_global_table_properties_get(tableid, &tbl_ctx);
    assert(tbl_ctx.table_location != P4_TBL_LOCATION_HBM);
    asicpd_copy_table_info(&cap_tbl_info, &tbl_ctx.tcam_layout, &tbl_ctx);

    ret = elba_tcam_table_entry_read(tableid, index, trit_x, trit_y,
                                      hwentry_bit_len, cap_tbl_info,
                                      tbl_ctx.gress);

    return ret;
}

int
asicpd_hbm_table_entry_write (uint32_t tableid, uint32_t index,
                              uint8_t *hwentry, uint16_t entry_size)
{
    int ret;
    p4pd_table_properties_t tbl_ctx;

    time_profile_begin(sdk::utils::time_profile::ASICPD_HBM_TABLE_ENTRY_WRITE);
    p4pd_global_table_properties_get(tableid, &tbl_ctx);
    ret = elba_hbm_table_entry_write(tableid, index, hwentry, entry_size,
                                      tbl_ctx.hbm_layout.entry_width, &tbl_ctx);

    uint64_t entry_addr = (index * tbl_ctx.hbm_layout.entry_width);
    ret |= elba_hbm_table_entry_cache_invalidate(tbl_ctx.cache,
                                                 entry_addr,
                                                 tbl_ctx.hbm_layout.entry_width,
                                                 tbl_ctx.base_mem_pa);

#if SDK_LOG_TABLE_WRITE
    char    buffer[2048];
    memset(buffer, 0, sizeof(buffer));
    uint8_t key[128] = {0}; // Atmost key is 64B. Assuming each
                            // key byte has worst case byte padding
    uint8_t keymask[128] = {0};
    uint8_t data[128] = {0};
    SDK_TRACE_DEBUG("%s", "read last installed hbm table entry back into table "
                          "key and action structures");

    p4pd_global_entry_read(tableid, index, (void*) key, (void*) keymask,
                           (void*)data);

    p4pd_global_table_ds_decoded_string_get(tableid, index,
                                            (void*) key, (void*) keymask,
                                            (void*) data, buffer,
                                            sizeof(buffer));
    SDK_TRACE_DEBUG("%s", buffer);
#endif
    time_profile_end(sdk::utils::time_profile::ASICPD_HBM_TABLE_ENTRY_WRITE);
    return ret;
}

int
asicpd_hbm_table_entry_read (uint32_t tableid, uint32_t index, uint8_t *hwentry,
                             uint16_t *entry_size)
{
    int ret;
    p4pd_table_properties_t tbl_ctx;
    p4_table_mem_layout_t cap_tbl_info = {0};

    time_profile_begin(sdk::utils::time_profile::ASICPD_HBM_TABLE_ENTRY_READ);
    p4pd_global_table_properties_get(tableid, &tbl_ctx);
    asicpd_copy_table_info(&cap_tbl_info, &tbl_ctx.hbm_layout, &tbl_ctx);
    ret = elba_hbm_table_entry_read(tableid, index, hwentry,
                                    entry_size, cap_tbl_info,
                                    tbl_ctx.read_thru_mode);
    time_profile_end(sdk::utils::time_profile::ASICPD_HBM_TABLE_ENTRY_READ);

    return ret;
}

sdk_ret_t
asicpd_p4plus_recirc_init (void)
{
    elba_p4plus_recirc_init();
    return SDK_RET_OK;
}

sdk_ret_t
asicpd_hbm_bw_get (hbm_bw_samples_t *hbm_bw_samples)
{
    return elba_hbm_bw(hbm_bw_samples->num_samples,
                       hbm_bw_samples->sleep_interval, true,
                       hbm_bw_samples->hbm_bw);
}

sdk_ret_t
asicpd_llc_setup (llc_counters_t *llc)
{
    return elba_nx_setup_llc_counters(llc->mask);
}

sdk_ret_t
asicpd_toeplitz_init (const char *handle, uint32_t tableid,
                      uint32_t rss_indir_tbl_entry_size)
{
    p4pd_table_properties_t tbl_ctx;

    p4pd_global_table_properties_get(tableid, &tbl_ctx);

    return elba_toeplitz_init(handle, tbl_ctx.stage, tbl_ctx.stage_tableid, rss_indir_tbl_entry_size);
}

sdk_ret_t
asicpd_llc_get (llc_counters_t *llc)
{
    return elba_nx_get_llc_counters(&llc->mask, llc->data);
}

sdk_ret_t
asicpd_scheduler_stats_get (scheduler_stats_t *sch_stats)
{
    sdk_ret_t ret;
    elba_txs_scheduler_stats_t asic_stats = {};

    ret = elba_txs_scheduler_stats_get(&asic_stats);
    if (ret != SDK_RET_OK) {
        return ret;
    }
    if (SDK_ARRAY_SIZE(asic_stats.cos_stats) > ASIC_NUM_MAX_COSES) {
        return SDK_RET_ERR;
    }
    sch_stats->num_coses = SDK_ARRAY_SIZE(asic_stats.cos_stats);
    sch_stats->doorbell_set_count = asic_stats.doorbell_set_count;
    sch_stats->doorbell_clear_count = asic_stats.doorbell_clear_count;
    sch_stats->ratelimit_start_count = asic_stats.ratelimit_start_count;
    sch_stats->ratelimit_stop_count = asic_stats.ratelimit_stop_count;
    for (unsigned i = 0; i < SDK_ARRAY_SIZE(asic_stats.cos_stats); i++) {
        sch_stats->cos_stats[i].cos = asic_stats.cos_stats[i].cos;
        sch_stats->cos_stats[i].xon_status =
            asic_stats.cos_stats[i].xon_status;
    }
    return SDK_RET_OK;
}

sdk_ret_t
asicpd_qstate_map_clear (uint32_t lif_id)
{
    elba_clear_qstate_map(lif_id);
    return SDK_RET_OK;
}

sdk_ret_t
asicpd_qstate_map_write (lif_qstate_t *qstate, uint8_t enable)
{
    elba_program_qstate_map(qstate, enable);
    return SDK_RET_OK;
}

sdk_ret_t
asicpd_qstate_map_rewrite (uint32_t lif_id, uint8_t enable)
{
    elba_reprogram_qstate_map(lif_id, enable);
    return SDK_RET_OK;
}

sdk_ret_t
asicpd_qstate_map_read (lif_qstate_t *qstate)
{
    elba_read_qstate_map(qstate);
    return SDK_RET_OK;
}

sdk_ret_t
asicpd_qstate_write (uint64_t addr, const uint8_t *buf, uint32_t size)
{
    return elba_write_qstate(addr, buf, size);
}

sdk_ret_t
asicpd_qstate_read (uint64_t addr, uint8_t *buf, uint32_t size)
{
    return elba_read_qstate(addr, buf, size);
}

sdk_ret_t
asicpd_qstate_clear (lif_qstate_t *qstate)
{
    return elba_clear_qstate(qstate);
}

void
asicpd_reset_qstate_map (uint32_t lif_id)
{
    elba_reset_qstate_map(lif_id);
}

sdk_ret_t
asicpd_p4plus_invalidate_cache (mpartition_region_t *reg, uint64_t q_addr,
                                uint32_t size)
{
    p4plus_cache_action_t action = P4PLUS_CACHE_ACTION_NONE;

    if(is_region_cache_pipe_p4plus_all(reg)) {
        action = P4PLUS_CACHE_INVALIDATE_BOTH;
    } else if (is_region_cache_pipe_p4plus_rxdma(reg)) {
        action = P4PLUS_CACHE_INVALIDATE_RXDMA;
    } else if (is_region_cache_pipe_p4plus_txdma(reg)) {
        action = P4PLUS_CACHE_INVALIDATE_TXDMA;
    }

    if (action != P4PLUS_CACHE_ACTION_NONE) {
        p4plus_invalidate_cache(q_addr, size, action);
    }

    return SDK_RET_OK;
}

uint32_t
asicpd_clock_freq_get (void)
{
    return 0;   /* ASIC yet to implement */
}

sdk_ret_t
asicpd_adjust_perf (int chip_id, int inst_id, pd_adjust_perf_index_t &idx,
                    pd_adjust_perf_type_t perf_type)
{
    pen_adjust_perf_status_t status;
    status = elba_adjust_perf(chip_id, inst_id, (pen_adjust_index_t &) idx,
                              (pen_adjust_perf_type_t) perf_type);
    return (status == PEN_PERF_SUCCESS) ? SDK_RET_OK : SDK_RET_ERR;
}

void
asicpd_set_half_clock (int chip_id, int inst_id)
{
    elba_set_half_clock(chip_id, inst_id);
}

sdk_ret_t
asicpd_unravel_hbm_intrs (bool *iscattrip, bool *iseccerr, bool logging)
{
    return elba_unravel_hbm_intrs(iscattrip, iseccerr, logging);
}

int
asicpd_p4plus_table_init (platform_type_t platform_type,
                          int stage_apphdr, int stage_tableid_apphdr,
                          int stage_apphdr_ext, int stage_tableid_apphdr_ext,
                          int stage_apphdr_off, int stage_tableid_apphdr_off,
                          int stage_apphdr_ext_off,
                          int stage_tableid_apphdr_ext_off,
                          int stage_txdma_act, int stage_tableid_txdma_act,
                          int stage_txdma_act_ext,
                          int stage_tableid_txdma_act_ext,
                          int stage_sxdma_act, int stage_tableid_sxdma_act)
{
    return elba_p4plus_table_init(platform_type,
                                  stage_apphdr, stage_tableid_apphdr,
                                  stage_apphdr_ext, stage_tableid_apphdr_ext,
                                  stage_apphdr_off, stage_tableid_apphdr_off,
                                  stage_apphdr_ext_off,
                                  stage_tableid_apphdr_ext_off,
                                  stage_txdma_act, stage_tableid_txdma_act,
                                  stage_txdma_act_ext,
                                  stage_tableid_txdma_act_ext,
                                  stage_sxdma_act, stage_tableid_sxdma_act);
}

sdk_ret_t
asicpd_tm_get_clock_tick (uint64_t *tick)
{
    return elba_tm_get_clock_tick(tick);
}

sdk_ret_t
asicpd_tm_debug_stats_get (tm_port_t port, tm_debug_stats_t *debug_stats,
                           bool reset)
{
    return elba_tm_debug_stats_get(port, debug_stats, reset);
}

sdk_ret_t
asicpd_sw_phv_init (void)
{
    return elba_sw_phv_init();
}

sdk_ret_t
asicpd_sw_phv_get (asic_swphv_type_t type, uint8_t prof_num,
                   asic_sw_phv_state_t *state)
{
    return elba_sw_phv_get(type, prof_num, state);
}

sdk_ret_t
asicpd_sw_phv_inject (asic_swphv_type_t type, uint8_t prof_num,
                      uint8_t start_idx, uint8_t num_flits, void *data)
{
    return elba_sw_phv_inject(type, prof_num, start_idx, num_flits, data);
}

uint32_t
asicpd_get_coreclk_freq (platform_type_t platform_type)
{
    return elba_get_coreclk_freq(platform_type);
}

void
asicpd_txs_timer_init_hsh_depth (uint32_t key_lines)
{
    return elba_txs_timer_init_hsh_depth(key_lines);
}

sdk_ret_t
queue_credits_get (queue_credits_get_cb_t cb, void *ctxt)
{
    return elba_queue_credits_get(cb, ctxt);
}

// called during upgrade in quiesced state
sdk_ret_t
asicpd_tbl_eng_cfg_modify (p4pd_pipeline_t pipeline, p4_tbl_eng_cfg_t *cfg,
                           uint32_t ncfgs)
{
    return elba_tbl_eng_cfg_modify(pipeline, cfg, ncfgs);
}

sdk_ret_t
asicpd_rss_tbl_eng_cfg_get (const char *handle, uint32_t tableid,
                            p4_tbl_eng_cfg_t *rss)
{
    p4pd_table_properties_t tbl_ctx;

    p4pd_global_table_properties_get(tableid, &tbl_ctx);
    rss->tableid = tableid;
    rss->stage = tbl_ctx.stage;
    rss->stage_tableid = tbl_ctx.stage_tableid;

    return elba_rss_table_base_pc_get(handle, &rss->mem_offset, &rss->asm_base);
}

void
asicpd_rss_tbl_eng_cfg_modify (p4_tbl_eng_cfg_t *rss)
{
    return;  /* TBD-ELBA-REBASE: */
}

//------------------------------------------------------------------------------
// perform all the ELBA specific initialization
//------------------------------------------------------------------------------
static sdk_ret_t
elba_cfg_init (asic_cfg_t *cfg, asic_cfg_t& elba_cfg)
{
    sdk_ret_t     ret;

    SDK_ASSERT(cfg != NULL);

    ret = sdk::asic::asic_state_init(cfg);
    SDK_ASSERT_TRACE_RETURN((ret == SDK_RET_OK), ret,
                            "asic_state_init failure, err : %d", ret);

    elba_cfg.default_config_dir = cfg->default_config_dir;
    elba_cfg.cfg_path = cfg->cfg_path;
    elba_cfg.admin_cos = cfg->admin_cos;
    elba_cfg.repl_entry_width = cfg->repl_entry_width;
    elba_cfg.catalog = cfg->catalog;
    elba_cfg.mempartition = cfg->mempartition;
    elba_cfg.p4_cache = true;
    elba_cfg.p4plus_cache = true;
    elba_cfg.llc_cache = true;
    elba_cfg.platform = cfg->platform;
    elba_cfg.num_pgm_cfgs = cfg->num_pgm_cfgs;
    elba_cfg.pgm_name = cfg->pgm_name;
    for (int i = 0; i < cfg->num_pgm_cfgs; i++) {
        elba_cfg.pgm_cfg[i].path = cfg->pgm_cfg[i].path;
    }
    elba_cfg.num_asm_cfgs = cfg->num_asm_cfgs;
    for (int i = 0; i < cfg->num_asm_cfgs; i++) {
        elba_cfg.asm_cfg[i].name = cfg->asm_cfg[i].name;
        elba_cfg.asm_cfg[i].path = cfg->asm_cfg[i].path;
        elba_cfg.asm_cfg[i].symbols_func = cfg->asm_cfg[i].symbols_func;
        elba_cfg.asm_cfg[i].base_addr = cfg->asm_cfg[i].base_addr;
        elba_cfg.asm_cfg[i].sort_func =
            cfg->asm_cfg[i].sort_func;
    }
    for (int i = 0; i < cfg->num_rings; i++) {
        sdk::platform::ring ring;
        ring.init(&cfg->ring_meta[i], cfg->mempartition);
    }

    elba_cfg.completion_func = cfg->completion_func;
    elba_cfg.device_profile = cfg->device_profile;
    return SDK_RET_OK;
}

sdk_ret_t
asicpd_init (asic_cfg_t *cfg)
{
    asic_cfg_t elba_cfg;

    elba_cfg_init(cfg, elba_cfg);
    if (sdk::asic::asic_is_hard_init()) {
        //@@TODO - check and update redundant initializations within
        // asic_state_init() and elba_init().
        return elba_init(&elba_cfg);
    } else {
        return elba_soft_init(&elba_cfg);
    }
}

sdk_ret_t
asicpd_soft_init (asic_cfg_t *cfg)
{
    asic_cfg_t elba_cfg;

    elba_cfg_init(cfg, elba_cfg);
    return elba_soft_init(&elba_cfg);
}

sdk_ret_t
asicpd_upgrade_init (asic_cfg_t *cfg)
{
    asic_cfg_t elba_cfg;

    // in upgrade mode asic should be in hard-init mode
    SDK_ASSERT(sdk::asic::asic_is_hard_init());
    elba_cfg_init(cfg, elba_cfg);

    if (sdk::platform::upgrade_mode_graceful(cfg->upg_init_mode)) {
        // it is same as hardinit. memory regions marked as reset will
        // be zeroed out by asic_reset_hbm_regions function during elba_init
        return elba_init(&elba_cfg);
    } else if (sdk::platform::upgrade_mode_hitless(cfg->upg_init_mode)) {
        SDK_ASSERT(0);
        // return elba_hitless_init(&elba_cfg);
    } else {
        SDK_TRACE_ERR("Invalid upgrade mode");
        SDK_ASSERT(0);
    }
}

sdk_ret_t
asicpd_pgm_init (void)
{
    return elba_pgm_init();
}

sdk_ret_t
asicpd_p4plus_table_rw_init (void)
{
    return elba_p4plus_table_rw_init();
}

sdk_ret_t
asicpd_state_pd_init (asic_cfg_t *cfg)
{
    return elba_state_pd_init(cfg);
}

void
asicpd_csr_init (void)
{
    csr_init();
}

sdk_ret_t
asicpd_init_hw_fifo (int fifo_num, uint64_t addr, int n, hw_fifo_prof_t *prof)
{
    return SDK_RET_OK;
}

sdk_ret_t
asicpd_get_hw_fifo_info (int fifo_num, hw_fifo_stats_t *stats)
{
    return SDK_RET_OK;
}

sdk_ret_t
asicpd_set_hw_fifo_info (int fifo_num, hw_fifo_stats_t *stats)
{
    return SDK_RET_OK;
}

inline bool
asicpd_tm_q_valid (int32_t tm_q)
{
    return (tm_q < 0) ? false : true;
}

uint64_t
asicpd_get_p4plus_table_mpu_pc (int table_id)
{
    return elba_get_p4plus_table_mpu_pc(table_id);
}

void
asicpd_program_p4plus_tbl_mpu_pc (int tableid, int stage_tbl_id, int stage)
{
    elba_program_p4plus_table_mpu_pc(tableid, stage_tbl_id, stage);
}

void
asicpd_program_tbl_mpu_pc (int tableid, bool gress, int stage,
                           int stage_tableid, uint64_t table_asm_err_offset,
                           uint64_t table_asm_base)
{
    elba_program_table_mpu_pc(tableid, gress, stage, stage_tableid,
                              table_asm_err_offset, table_asm_base);
}

void
asicpd_set_action_asm_base (int tableid, int actionid, uint64_t asm_base)
{
    elba_set_action_asm_base(tableid, actionid, asm_base);
}

void
asicpd_set_action_rxdma_asm_base (int tableid, int actionid, uint64_t asm_base)
{
    elba_set_action_rxdma_asm_base(tableid, actionid, asm_base);
}

void
asicpd_set_action_txdma_asm_base (int tableid, int actionid, uint64_t asm_base)
{
    elba_set_action_txdma_asm_base(tableid, actionid, asm_base);
}

void
asicpd_set_table_rxdma_asm_base (int tableid, uint64_t asm_base)
{
    elba_set_table_rxdma_asm_base(tableid, asm_base);
}

void
asicpd_set_table_txdma_asm_base (int tableid, uint64_t asm_base)
{
    elba_set_table_txdma_asm_base(tableid, asm_base);
}

uint64_t
asicpd_host_dbaddr_get (void)
{
    return elba_host_dbaddr();
}

uint64_t
asicpd_local_dbaddr_get (void)
{
    return elba_local_dbaddr();
}

uint64_t
asicpd_local_db32_addr_get (void)
{
    return elba_local_db32_addr();
}

sdk_ret_t
asicpd_tm_uplink_lif_set (tm_port_t port, uint32_t lif)
{
    return elba_tm_uplink_lif_set(port, lif);
}

sdk_ret_t
asicpd_tm_enable_disable_uplink_port (tm_port_t port, bool enable)
{
    return elba_tm_enable_disable_uplink_port(port, enable);
}

sdk_ret_t
asicpd_tm_flush_uplink_port (tm_port_t port, bool enable)
{
    return elba_tm_flush_uplink_port(port, enable);
}

sdk_ret_t
asicpd_tm_write_control_uplink_port (tm_port_t port, bool enable)
{
    return elba_tm_write_control_uplink_port(port, enable);
}

sdk_ret_t
asicpd_tm_drain_uplink_port (tm_port_t port)
{
    return elba_tm_drain_uplink_port(port);
}

sdk_ret_t
asicpd_p4p_rxdma_mpu_trace_enable (uint32_t stage_id, uint32_t mpu,
                                   uint8_t  enable, uint8_t  trace_enable,
                                   uint8_t  phv_debug, uint8_t  phv_error,
                                   uint64_t watch_pc, uint64_t base_addr,
                                   uint8_t  table_key, uint8_t  instructions,
                                   uint8_t  wrap, uint8_t  reset,
                                   uint32_t buf_size)
{
    return SDK_RET_INVALID_OP;
}

sdk_ret_t
asicpd_p4p_txdma_mpu_trace_enable (uint32_t stage_id, uint32_t mpu,
                                   uint8_t  enable, uint8_t  trace_enable,
                                   uint8_t  phv_debug, uint8_t  phv_error,
                                   uint64_t watch_pc, uint64_t base_addr,
                                   uint8_t  table_key, uint8_t  instructions,
                                   uint8_t  wrap, uint8_t  reset,
                                   uint32_t buf_size)
{
    return SDK_RET_INVALID_OP;
}

sdk_ret_t
asicpd_p4_ingress_mpu_trace_enable (uint32_t stage_id, uint32_t mpu,
                                    uint8_t  enable, uint8_t  trace_enable,
                                    uint8_t  phv_debug, uint8_t  phv_error,
                                    uint64_t watch_pc, uint64_t base_addr,
                                    uint8_t  table_key, uint8_t  instructions,
                                    uint8_t  wrap, uint8_t  reset,
                                    uint32_t buf_size)
{
    return SDK_RET_INVALID_OP;
}

sdk_ret_t
asicpd_p4_egress_mpu_trace_enable (uint32_t stage_id, uint32_t mpu,
                                   uint8_t  enable, uint8_t  trace_enable,
                                   uint8_t  phv_debug, uint8_t  phv_error,
                                   uint64_t watch_pc, uint64_t base_addr,
                                   uint8_t  table_key, uint8_t  instructions,
                                   uint8_t  wrap, uint8_t  reset,
                                   uint32_t buf_size)
{
    return SDK_RET_INVALID_OP;
}

sdk_ret_t
asicpd_quiesce_start(void)
{
    return elba_quiesce_start();
}

sdk_ret_t
asicpd_quiesce_stop (void)
{
    return elba_quiesce_stop();
}

sdk_ret_t
asicpd_barco_asym_ecc_point_mul (uint16_t key_size, uint8_t *p,
                                 uint8_t *n, uint8_t *xg, uint8_t *yg,
                                 uint8_t *a, uint8_t *b, uint8_t *x1,
                                 uint8_t *y1, uint8_t *k, uint8_t *x3,
                                 uint8_t *y3)
{
    return elba_barco_asym_ecc_point_mul(key_size, p, n, xg, yg, a, b, x1,
                                          y1, k, x3, y3);
}

sdk_ret_t
asicpd_barco_asym_ecdsa_p256_setup_priv_key (uint8_t *p, uint8_t *n,
                                             uint8_t *xg, uint8_t *yg,
                                             uint8_t *a, uint8_t *b,
                                             uint8_t *da, int32_t *key_idx)
{
    return elba_barco_asym_ecdsa_p256_setup_priv_key(p, n, xg, yg,
                                                       a, b, da, key_idx);
}

sdk_ret_t
asicpd_barco_asym_ecdsa_p256_sig_gen (int32_t key_idx, uint8_t *p,
                                      uint8_t *n, uint8_t *xg,
                                      uint8_t *yg, uint8_t *a,
                                      uint8_t *b, uint8_t *da,
                                      uint8_t *k, uint8_t *h,
                                      uint8_t *r, uint8_t *s,
                                      bool async_en,
                                      const uint8_t *unique_key)
{
    return elba_barco_asym_ecdsa_p256_sig_gen(key_idx, p, n, xg, yg, a,
                                               b, da, k, h, r, s,
                                               async_en, unique_key);
}

sdk_ret_t
asicpd_barco_asym_ecdsa_p256_sig_verify (uint8_t *p, uint8_t *n,
                                         uint8_t *xg, uint8_t *yg,
                                         uint8_t *a, uint8_t *b,
                                         uint8_t *xq, uint8_t *yq,
                                         uint8_t *r, uint8_t *s,
                                         uint8_t *h, bool async_en,
                                         const uint8_t *unique_key)
{
    return elba_barco_asym_ecdsa_p256_sig_verify(p, n, xg, yg, a, b,
                                                  xq, yq, r, s, h,
                                                  async_en, unique_key);
}

sdk_ret_t
asicpd_barco_asym_rsa2k_encrypt (uint8_t *n, uint8_t *e,
                                 uint8_t *m,  uint8_t *c,
                                 bool async_en,
                                 const uint8_t *unique_key)
{
    return elba_barco_asym_rsa2k_encrypt(n, e, m, c, async_en, unique_key);
}

sdk_ret_t
asicpd_barco_asym_rsa_encrypt (uint16_t key_size, uint8_t *n,
                               uint8_t *e, uint8_t *m,  uint8_t *c,
                               bool async_en, const uint8_t *unique_key)
{
    return elba_barco_asym_rsa_encrypt(key_size, n, e, m, c, async_en,
                                        unique_key);
}

sdk_ret_t
asicpd_barco_asym_rsa2k_decrypt (uint8_t *n, uint8_t *d, uint8_t *c, uint8_t *m)
{
    return elba_barco_asym_rsa2k_decrypt(n, d, c, m);
}

sdk_ret_t
asicpd_barco_asym_rsa2k_crt_decrypt (int32_t key_idx, uint8_t *p,
                                     uint8_t *q, uint8_t *dp,
                                     uint8_t *dq, uint8_t *qinv,
                                     uint8_t *c, uint8_t *m,
                                     bool async_en,
                                     const uint8_t *unique_key)
{
    return elba_barco_asym_rsa2k_crt_decrypt(key_idx, p, q, dp, dq, qinv,
                                              c, m, async_en, unique_key);
}

sdk_ret_t
asicpd_barco_asym_rsa2k_setup_sig_gen_priv_key (uint8_t *n, uint8_t *d,
                                                int32_t *key_idx)
{
    return elba_barco_asym_rsa2k_setup_sig_gen_priv_key(n, d, key_idx);
}

sdk_ret_t
asicpd_barco_asym_rsa2k_crt_setup_decrypt_priv_key (uint8_t *p, uint8_t *q,
                                                    uint8_t *dp, uint8_t *dq,
                                                    uint8_t *qinv,
                                                    int32_t* key_idx)
{
    return elba_barco_asym_rsa2k_crt_setup_decrypt_priv_key(p, q, dp, dq,
                                                             qinv, key_idx);
}

sdk_ret_t
asicpd_barco_asym_rsa_setup_priv_key (uint16_t key_size, uint8_t *n,
                                      uint8_t *d, int32_t* key_idx)
{
    return elba_barco_asym_rsa_setup_priv_key(key_size, n, d, key_idx);
}

sdk_ret_t
asicpd_barco_asym_rsa2k_sig_gen (int32_t key_idx, uint8_t *n,
                                 uint8_t *d, uint8_t *h, uint8_t *s,
                                 bool async_en, const uint8_t *unique_key)
{
    return elba_barco_asym_rsa2k_sig_gen(key_idx, n, d, h, s, async_en,
                                          unique_key);
}

sdk_ret_t
asicpd_barco_asym_rsa_sig_gen (uint16_t key_size, int32_t key_idx,
                               uint8_t *n, uint8_t *d,
                               uint8_t *h, uint8_t *s,
                               bool async_en, const uint8_t *unique_key)
{
    return elba_barco_asym_rsa_sig_gen(key_size, key_idx, n, d, h, s,
                                        async_en, unique_key);
}

sdk_ret_t
asicpd_barco_asym_fips_rsa_sig_gen (uint16_t key_size, int32_t key_idx,
                                    uint8_t *n, uint8_t *e, uint8_t *msg,
                                    uint16_t msg_len, uint8_t *s,
                                    uint8_t hash_type, uint8_t sig_scheme,
                                    bool async_en, const uint8_t *unique_key)
{
    return elba_barco_asym_fips_rsa_sig_gen(key_size, key_idx, n, e, msg,
                                          msg_len, s,
                                          (hash_type_t) hash_type,
                                          (rsa_signature_scheme_t) sig_scheme,
                                          async_en, unique_key);
}

sdk_ret_t
asicpd_barco_asym_fips_rsa_sig_verify (uint16_t key_size, uint8_t *n,
                                       uint8_t *e, uint8_t *msg,
                                       uint16_t msg_len, uint8_t *s,
                                       uint8_t hash_type, uint8_t sig_scheme,
                                       bool async_en,
                                       const uint8_t *unique_key)
{
    return elba_barco_asym_fips_rsa_sig_verify(key_size, n, e, msg, msg_len, s,
                                          (hash_type_t) hash_type,
                                          (rsa_signature_scheme_t) sig_scheme,
                                          async_en, unique_key);
}

sdk_ret_t
asicpd_barco_asym_rsa2k_sig_verify (uint8_t *n, uint8_t *e,
                                    uint8_t *h, uint8_t *s)
{
    return elba_barco_asym_rsa2k_sig_verify(n, e, h, s);
}

sdk_ret_t
asicpd_dpp_int_credit (uint32_t instance, uint32_t value)
{
    return SDK_RET_INVALID_OP;
}

sdk_ret_t
asicpd_pxb_cfg_lif_bdf (uint32_t lif, uint16_t bdf)
{
    return elba_pxb_cfg_lif_bdf(lif, bdf);
}

sdk_ret_t
asicpd_barco_asym_req_descr_get (uint32_t slot_index,
                                 void *asym_req_descr)
{
    return elba_barco_asym_req_descr_get(slot_index,
                                          (barco_asym_descr_t *)asym_req_descr);
}

sdk_ret_t
asicpd_barco_symm_req_descr_get (uint8_t ring_type, uint32_t slot_index,
                                 void *symm_req_descr)
{
    return elba_barco_symm_req_descr_get((barco_rings_t) ring_type, slot_index,
                                          (barco_symm_descr_t *)symm_req_descr);
}

sdk_ret_t
asicpd_barco_ring_meta_get (uint8_t ring_type, uint32_t *pi, uint32_t *ci)
{
    return elba_barco_ring_meta_get((barco_rings_t) ring_type, pi, ci);
}

sdk_ret_t
asicpd_barco_get_meta_config_info (uint8_t ring_type,
                                   void *meta)
{
    return elba_barco_get_meta_config_info((barco_rings_t) ring_type,
                                            (barco_ring_meta_config_t *)meta);
    //barco_ring_meta_config_t *meta_tmp = (barco_ring_meta_config_t *) meta;
    //return elba_barco_get_meta_config_info((barco_rings_t) ring_type,
    //                                        &meta_tmp->shadow_pndx_addr,
    //                                        &meta_tmp->pndx_size,
    //                                        &meta_tmp->desc_size,
    //                                        &meta_tmp->opaque_tag_size);
    //return SDK_RET_INVALID_OP;
}

sdk_ret_t
asicpd_barco_asym_add_pend_req (uint32_t hw_id, uint32_t sw_id)
{
    return elba_barco_asym_add_pend_req(hw_id, sw_id);
}

sdk_ret_t
asicpd_barco_asym_poll_pend_req (uint32_t batch_size, uint32_t* id_count,
                                 uint32_t *ids)
{
    return elba_barco_asym_poll_pend_req(batch_size, id_count, ids);
}

sdk_ret_t
asicpd_barco_sym_hash_process_request (uint8_t hash_type, bool generate,
                                       unsigned char *key, int key_len,
                                       unsigned char *data, int data_len,
                                       uint8_t *output_digest, int digest_len)
{
    return elba_barco_sym_hash_process_request((CryptoApiHashType) hash_type,
                                                generate, key, key_len, data,
                                                data_len, output_digest,
                                                digest_len);
}

sdk_ret_t
asicpd_barco_get_capabilities (uint8_t ring_type, bool *sw_reset_capable,
                               bool *sw_enable_capable)
{
    return elba_barco_get_capabilities((barco_rings_t) ring_type,
                                         sw_reset_capable,
                                         sw_enable_capable);
}

sdk_ret_t
asicpd_crypto_alloc_key (uint32_t *key_idx)
{
    sdk_ret_t              sdk_ret;

    sdk_ret = elba_barco_sym_alloc_key((int32_t *) key_idx);

    if (sdk_ret != SDK_RET_OK) {
        SDK_TRACE_ERR("SessKey: Failed to allocate key");
        return sdk_ret;
    }

    SDK_TRACE_DEBUG("SessKey:%s: Allocated key memory @ index: %0d",
                    __FUNCTION__, *key_idx);
    return sdk_ret;
}

sdk_ret_t
asicpd_crypto_alloc_key_withid (uint32_t key_idx, bool allow_dup_alloc)
{
    sdk_ret_t              sdk_ret;

    sdk_ret = elba_barco_sym_alloc_key_withid(key_idx, allow_dup_alloc);

    if (sdk_ret != SDK_RET_OK) {
        SDK_TRACE_ERR("SessKey: Failed to allocate key by ID: %0d", key_idx);
        return sdk_ret;
    }
    SDK_TRACE_DEBUG("SessKey:%s: Allocated key memory @ index: %0d",
                    __FUNCTION__, key_idx);
    return sdk_ret;
}

sdk_ret_t
asicpd_crypto_free_key (uint32_t key_idx)
{
    sdk_ret_t              sdk_ret;

    sdk_ret = elba_barco_sym_free_key(key_idx);

    if (sdk_ret != SDK_RET_OK) {
        SDK_TRACE_ERR("SessKey: Failed to free key memory: %0d", key_idx);
        return sdk_ret;
    }

    SDK_TRACE_DEBUG("SessKey:%s: Freed key memory @ index: %0d",
                    __FUNCTION__, key_idx);
    return sdk_ret;
}

sdk_ret_t
asicpd_crypto_write_key (uint32_t key_idx, crypto_key_t *key)
{
    sdk_ret_t              sdk_ret;

    sdk_ret = elba_barco_setup_key(key_idx, (crypto_key_type_t)key->key_type,
                                    key->key, key->key_size);

    return sdk_ret;
}

sdk_ret_t
asicpd_crypto_read_key (uint32_t key_idx, crypto_key_t *key)
{
    sdk_ret_t              sdk_ret;

    sdk_ret = elba_barco_read_key(key_idx, (crypto_key_type_t*)&key->key_type,
                                   key->key, &key->key_size);
    return sdk_ret;
}

sdk_ret_t
asicpd_crypto_asym_alloc_key (uint32_t *key_idx)
{
    sdk_ret_t              sdk_ret;

    sdk_ret = elba_barco_asym_alloc_key((int32_t *) key_idx);

    if (sdk_ret != SDK_RET_OK) {
        SDK_TRACE_ERR("SessKey: Failed to allocate key memory");
        *key_idx = -1;
        return sdk_ret;
    }
    return sdk_ret;
}

sdk_ret_t
asicpd_crypto_asym_free_key (uint32_t key_idx)
{
    sdk_ret_t              sdk_ret;

    /* TODO: Also free up the DMA descriptor and corresponding memory regions
     * if any referenced by the key descriptor
     */

    sdk_ret = elba_barco_asym_free_key(key_idx);

    if (sdk_ret != SDK_RET_OK) {
        SDK_TRACE_ERR("AsymKey: Failed to free key memory: %0d", key_idx);
        return sdk_ret;
    }

    return sdk_ret;
}

sdk_ret_t
asicpd_crypto_asym_write_key (uint32_t key_idx, crypto_asym_key_t *key)
{
    sdk_ret_t              sdk_ret;

    elba_barco_asym_key_desc_t     key_desc;

    key_desc.key_param_list = key->key_param_list;
    key_desc.command_reg = key->command_reg;

    sdk_ret = elba_barco_asym_write_key(key_idx, &key_desc);

    if (sdk_ret != SDK_RET_OK) {
        SDK_TRACE_ERR("Failed to write Barco Asym key descriptor @ %0d",
                      key_idx);
        return sdk_ret;
    }
    SDK_TRACE_DEBUG("AsymKey Write: Setup key @ %0d", key_idx);

    return sdk_ret;
}

sdk_ret_t
asicpd_crypto_asym_read_key (uint32_t key_idx, crypto_asym_key_t *key)
{
    sdk_ret_t              sdk_ret;

    elba_barco_asym_key_desc_t     key_desc;

    sdk_ret = elba_barco_asym_read_key(key_idx, &key_desc);

    if (sdk_ret != SDK_RET_OK) {
        SDK_TRACE_ERR("Failed to read Barco Asym key descriptor from %0d", key_idx);
        return sdk_ret;
    }

    key->key_param_list = key_desc.key_param_list;
    key->command_reg = key_desc.command_reg;

    return sdk_ret;
}

sdk_ret_t
asicpd_crypto_init_ipsec_pad_table (void)
{
    uint8_t ipsec_pad_bytes[MAX_IPSEC_PAD_SIZE];
    uint64_t ipsec_pad_base_addr = 0;

    SDK_TRACE_DEBUG("Initializing IPSEC Pad Bytes table");
    // Increasing number pattern as per RFC 1-255
    for (int i = 0; i < MAX_IPSEC_PAD_SIZE; i++) {
        ipsec_pad_bytes[i] = i+1;
    }

    ipsec_pad_base_addr = asicpd_get_mem_addr(ASIC_HBM_REG_IPSEC_PAD_TABLE);
    if (ipsec_pad_base_addr != INVALID_MEM_ADDRESS) {
        asicpd_p4plus_hbm_write(ipsec_pad_base_addr, ipsec_pad_bytes, MAX_IPSEC_PAD_SIZE,
                P4PLUS_CACHE_ACTION_NONE);
    }

    return SDK_RET_OK;
}

sdk_ret_t
asicpd_crypto_pd_init (void)
{
    sdk_ret_t           sdk_ret = SDK_RET_OK;

    sdk_ret = asicpd_crypto_init_ipsec_pad_table();

    if (sdk_ret != SDK_RET_OK) {
        return sdk_ret;
    }

    return sdk_ret;
}

sdk_ret_t
asicpd_tm_set_span_threshold (uint32_t span_threshold)
{
    return SDK_RET_INVALID_OP;
}

sdk_ret_t
asicpd_table_rw_init (asic_cfg_t *cfg)
{
    return elba_table_rw_init(cfg);
}

sdk_ret_t
asicpd_tm_set_reserved_min (uint32_t reserved_min)
{
    return elba_tm_set_reserved_min(reserved_min);
}

sdk_ret_t
asicpd_queue_stats_get (tm_port_t port, void *stats)
{
    return elba_queue_stats_get(port, stats);
}

void
asicpd_set_margin_by_value (const char *name, uint64_t tgtVoutMv)
{
    return; /* not applicable for elba */
}

sdk_ret_t
asicpd_sbus_cpu_1666 (int chip_id, int inst_id)
{
    return SDK_RET_OK;  /* not applicable for elba */
}

sdk_ret_t
asicpd_sbus_cpu_2200 (int chip_id, int inst_id)
{
    return SDK_RET_OK;  /* not applicable for elba */
}

void
asicpd_qstate_push (sdk::platform::utils::LIFQState *qstate, int cos)
{
    push_qstate_to_elba(qstate, cos);
}

// qos related asic APIs
tm_q_t
asicpd_qos_span_queue (void)
{
    SDK_TRACE_DEBUG("Not implemented");
    return 0;
}

tm_q_t
asicpd_qos_p4eg_uplink_span_q_replacement (void)
{
    SDK_TRACE_DEBUG("Not implemented");
    return 0;
}

tm_q_t
asicpd_qos_cpu_copy_queue (void)
{
    SDK_TRACE_DEBUG("Not implemented");
    return 0;
}

tm_q_t
asicpd_qos_p4eg_uplink_cpu_copy_q_replacement (void)
{
    SDK_TRACE_DEBUG("Not implemented");
    return 0;
}

tm_q_t
asicpd_qos_max_tx_qs_per_class (void)
{
    SDK_TRACE_DEBUG("Not implemented");
    return 0;
}

tm_q_t
asicpd_qos_p4ig_uplink_iq_offset (void)
{
    SDK_TRACE_DEBUG("Not implemented");
    return 0;
}

tm_q_t
asicpd_qos_rxdma_oq_offset (void)
{
    SDK_TRACE_DEBUG("Not implemented");
    return 0;
}

uint32_t
asicpd_qos_default_xon_threshold (void)
{
    SDK_TRACE_DEBUG("Not implemented");
    return 0;
}

uint32_t
asicpd_qos_default_xoff_threshold (void)
{
    SDK_TRACE_DEBUG("Not implemented");
    return 0;
}

tm_port_t
asicpd_qos_uplink_port_start (void)
{
    SDK_TRACE_DEBUG("Not implemented");
    return 0;
}

tm_port_t
asicpd_qos_uplink_port_end (void)
{
    SDK_TRACE_DEBUG("Not implemented");
    return 0;
}

uint32_t
asicpd_qos_max_txdma_iqs (void)
{
    SDK_TRACE_DEBUG("Not implemented");
    return 0;
}

uint32_t
asicpd_qos_max_uplink_iqs (void)
{
    SDK_TRACE_DEBUG("Not implemented");
    return 0;
}

uint32_t
asicpd_qos_max_common_oqs (void)
{
    SDK_TRACE_DEBUG("Not implemented");
    return 0;
}

uint32_t
asicpd_qos_max_rxdma_oqs (void)
{
    SDK_TRACE_DEBUG("Not implemented");
    return 0;
}

sdk_ret_t
asicpd_qos_uplink_q_params_update (tm_port_t port,
                                   tm_uplink_q_params_t *q_params)
{
    SDK_TRACE_DEBUG("Not implemented");
    return SDK_RET_OK;
}

sdk_ret_t
asicpd_qos_uplink_iq_nodrop_update (tm_port_t port, tm_q_t iq,
                                    bool no_drop)
{
    SDK_TRACE_DEBUG("Not implemented");
    return SDK_RET_OK;
}

sdk_ret_t
asicpd_qos_uplink_input_map_update (tm_port_t port, uint32_t dot1q_pcp,
                                    tm_q_t iq)
{
    SDK_TRACE_DEBUG("Not implemented");
    return SDK_RET_OK;
}

int
asicpd_hbm_table_entry_cache_invalidate (p4pd_table_cache_t cache,
                                         uint64_t entry_addr,
                                         uint16_t entry_width,
                                         mem_addr_t base_mem_pa)
{
    return elba_hbm_table_entry_cache_invalidate(cache, entry_addr,
                                                 entry_width, base_mem_pa);
}

}    // namespace pd
}    // namespace asic
}    // namespace sdk
