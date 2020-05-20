// {C} Copyright 2020 Pensando Systems Inc. All rights reserved

#ifndef __ELBA_TXS_SCHEDULER_HPP__
#define __ELBA_TXS_SCHEDULER_HPP__

#include <stdio.h>
#include <string>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include "asic/cmn/asic_cfg.hpp"

namespace sdk {
namespace platform {
namespace elba {

#define NUM_MAX_COSES                           16

#define ELBA_TXS_MAX_TABLE_ENTRIES              2048
#define ELBA_TXS_MAX_QGRP_TABLE_ENTRIES         4096

// 16k x 2K chunk qids (32M queues total)
#define ELBA_TXS_SCHEDULER_MAP_MAX_ENTRIES      16384 // 16k

// number of qid/chunk of qgrp
#define ELBA_TXS_SCHEDULER_NUM_QUEUES_PER_ENTRY 2048

// Timer definitions
#define ELBA_TIMER_WHEEL_DEPTH                  4096
#define ELBA_TIMER_NUM_KEY_PER_CACHE_LINE       16
#define ELBA_TIMER_NUM_DATA_PER_CACHE_LINE      12
// This needs to be a power of 2
#define ELBA_TIMER_NUM_KEY_CACHE_LINES          1024

// each line is 64B
// Each key in key line takes up 1 line in data space
#define ELBA_TIMER_HBM_DATA_SPACE \
        (ELBA_TIMER_NUM_KEY_CACHE_LINES * ELBA_TIMER_NUM_KEY_PER_CACHE_LINE * 64)

#define ELBA_TIMER_HBM_KEY_SPACE \
        (ELBA_TIMER_NUM_KEY_CACHE_LINES * 64)

#define ELBA_MAX_TIMERS \
        (ELBA_TIMER_NUM_KEY_CACHE_LINES * ELBA_TIMER_NUM_KEY_PER_CACHE_LINE * \
         ELBA_TIMER_NUM_DATA_PER_CACHE_LINE)

// (lif,queue,cos) mapping params in scheduler table
typedef struct elba_txs_sched_lif_params_s_ {
    uint32_t             sched_table_offset;    // offset for dhs_sch_qgrp_map_sram table: 16k entries
    uint32_t             sched_qgrp_offset;     // offset for dhs_sch_qgrp_cfg_0/1_sram tables: 4k entries
    uint32_t             num_entries_per_cos;   // block_size field in dhs_sch_lif_base_map_sram (number of 2k blocks / cos)
    uint32_t             total_qcount;          // total number of qids / qgrp (with assumption of 1 qgrp/cos)
    uint16_t             cos_bmp;               // active_cos
} __PACK__ elba_txs_sched_lif_params_t;

// (lif,cos) mapping params in policer table
typedef struct elba_txs_policer_lif_params_s {
    uint32_t             sched_table_start_offset;
    uint32_t             sched_table_end_offset;
} __PACK__ elba_txs_policer_lif_params_t;

// elba_txs_scheduler_init
// API to init the txs scheduler module
//
// @return sdk_ret_t: Status of the operation
//

sdk_ret_t elba_txs_scheduler_init(uint32_t admin_cos, asic_cfg_t *elba_cfg);

void elba_txs_timer_init_hsh_depth(uint32_t key_lines);

// elba_txs_scheduler_lif_params_update
// API to program txs scheduler table with lif,queue,cos mappings.
//
// @param  hw_lif_id[in]: hw lif id for this entry.
// @param  txs_hw_params[in]. scheduler table params for this entry.
// @return sdk_ret_t: Status of the operation
//
sdk_ret_t elba_txs_scheduler_lif_params_update(uint32_t hw_lif_id,
                                               elba_txs_sched_lif_params_t *txs_hw_params);

sdk_ret_t elba_txs_scheduler_tx_alloc(elba_txs_sched_lif_params_t *tx_params,
                                      uint32_t *alloc_offset,
                                      uint32_t *alloc_units);

sdk_ret_t elba_txs_scheduler_tx_qgrp_alloc (elba_txs_sched_lif_params_t *tx_params,
                                        uint32_t *alloc_qgrp_offset, 
                                        uint32_t *alloc_qgrp_units);

sdk_ret_t elba_txs_scheduler_tx_dealloc(uint32_t alloc_offset,
                                        uint32_t alloc_units);

sdk_ret_t elba_txs_scheduler_tx_qgrp_dealloc (
                                        uint32_t alloc_qgrp_offset, 
                                        uint32_t alloc_qgrp_units);

// elba_txs_policer_lif_params_update
// API to program txs policer table with lif,cos scheduler-table mappings.
//
// @param  hw_lif_id[in]: hw lif id for this entry.
// @param  txs_hw_params[in]. scheduler table params for this entry.
// @return sdk_ret_t: Status of the operation
//
sdk_ret_t elba_txs_policer_lif_params_update(uint32_t hw_lif_id,
                                             elba_txs_policer_lif_params_t *txs_hw_params);

typedef struct elba_txs_scheduler_cos_stats_s {
    uint32_t cos;
    bool xon_status;
} elba_txs_scheduler_cos_stats_t;

typedef struct elba_txs_scheduler_stats_s {
    uint64_t doorbell_set_count;
    uint64_t doorbell_clear_count;
    uint32_t ratelimit_start_count;
    uint32_t ratelimit_stop_count;
    uint32_t txdma_ptd_feedback_count;
    uint32_t txdma_phb_feedback_count;
    uint32_t txdma_sent_count;
    uint32_t sxdma_sent_count;
    elba_txs_scheduler_cos_stats_t cos_stats[NUM_MAX_COSES];
} elba_txs_scheduler_stats_t;

sdk_ret_t elba_txs_scheduler_stats_get(elba_txs_scheduler_stats_t *scheduler_stats);

uint32_t elba_get_coreclk_freq(platform_type_t platform_type);

}    // namespace elba
}    // namespace platform
}    // namespace sdk

using sdk::platform::elba::elba_txs_sched_lif_params_t;
using sdk::platform::elba::elba_txs_policer_lif_params_t;
using sdk::platform::elba::elba_txs_scheduler_stats_t;

#endif    // __ELBA_TXS_SCHEDULER_HPP__
