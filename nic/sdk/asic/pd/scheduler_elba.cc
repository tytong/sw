#include "platform/elba/elba_txs_scheduler.hpp"
#include "asic/pd/scheduler.hpp"
#include "lib/utils/utils.hpp"

using namespace sdk::platform::elba;

namespace sdk {
namespace asic {
namespace pd {

using namespace sdk::platform::elba;

sdk_ret_t
asicpd_tx_scheduler_map_alloc (asicpd_scheduler_lif_params_t *lif)
{
    sdk_ret_t     ret = SDK_RET_ERR;
    uint32_t      alloc_offset = INVALID_INDEXER_INDEX;
    uint32_t      alloc_units = 0;
    uint32_t      alloc_qgrp_offset = INVALID_INDEXER_INDEX;
    uint32_t      alloc_qgrp_units = 0;
    elba_txs_sched_lif_params_t params;

    if (lif->tx_sched_table_offset != INVALID_INDEXER_INDEX) {
        SDK_TRACE_DEBUG("Scheduler resource already allocated for"
                        "lif_id %u,at offset %u and num-of-entries %u",
                        lif->lif_id,
                        lif->tx_sched_table_offset,
                        lif->tx_sched_num_table_entries);
        goto end;
    }

    // Allocate txs scheduler resource for this lif.
    // Capri: Sched table can hold 8K queues per index and mandates new index for each cos.
    // Elba: Sched table can hold blocks of 2K queues per index and mandates new index for each cos.
    params.total_qcount = lif->total_qcount;
    params.cos_bmp = lif->cos_bmp;
    ret = elba_txs_scheduler_tx_alloc(&params, &alloc_offset, &alloc_units);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR("lif_id : %u,failed to alloc txs sched res",
                       lif->lif_id);
        lif->tx_sched_table_offset = INVALID_INDEXER_INDEX;
        lif->tx_sched_num_table_entries = 0;
        ret = SDK_RET_NO_RESOURCE;
        goto end;
    }
    ret = elba_txs_scheduler_tx_qgrp_alloc(&params, &alloc_qgrp_offset, &alloc_qgrp_units);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR("lif_id : %u,failed to qgrp alloc txs sched res",
                       lif->lif_id);
        lif->tx_sched_qgrp_offset = INVALID_INDEXER_INDEX;
        lif->tx_sched_num_qgrp_entries = 0;
        ret = SDK_RET_NO_RESOURCE;
        goto end;
    }
    lif->tx_sched_table_offset      = alloc_offset;      // base_addr in lif
    lif->tx_sched_num_table_entries = alloc_units;       // total number of 2k blocks in lif (num of 2k block per qgrp * count_bit_set(actiev_cos))
    lif->tx_sched_qgrp_offset       = alloc_qgrp_offset; // qgrp num at start of lif
    lif->tx_sched_num_qgrp_entries  = alloc_qgrp_units;  // number of qgrps / lif (1 qgrp/cos)
    SDK_TRACE_DEBUG("lif_id : %u,allocated : %u 2k block resource at 2k block offset : %u, %u qgrp (start), qgrp offset : %u",
                    lif->lif_id,
                    lif->tx_sched_num_table_entries,
                    lif->tx_sched_table_offset,
                    lif->tx_sched_num_qgrp_entries,
                    lif->tx_sched_qgrp_offset);
end:
    return ret;
}

sdk_ret_t
asicpd_tx_scheduler_map_free (asicpd_scheduler_lif_params_t *lif)
{
    sdk_ret_t      ret = SDK_RET_OK;

    if (!lif) {
        goto end;
    }
    //Free the scheduler table indices.
    ret = elba_txs_scheduler_tx_dealloc(lif->tx_sched_table_offset,
                                         lif->tx_sched_num_table_entries);
    if (ret != SDK_RET_OK) {
        goto end;
    }

    ret = elba_txs_scheduler_tx_qgrp_dealloc(lif->tx_sched_qgrp_offset,
                                             lif->tx_sched_num_qgrp_entries);
    if (ret != SDK_RET_OK) {
        goto end;
    }
    ret =  SDK_RET_OK;
    lif->tx_sched_table_offset      = INVALID_INDEXER_INDEX;
    lif->tx_sched_num_table_entries = 0;
end:
    return ret;
}

sdk_ret_t
asicpd_tx_scheduler_map_program (asicpd_scheduler_lif_params_t *lif)
{
    sdk_ret_t                    ret = SDK_RET_OK;
    uint32_t                     num_cos_values = 0;
    elba_txs_sched_lif_params_t txs_hw_params = {0};

    if (!lif) {
        goto end;
    }
    num_cos_values = sdk::lib::count_bits_set(lif->cos_bmp);
    if (num_cos_values && lif->tx_sched_num_table_entries) {
        txs_hw_params.sched_table_offset  = lif->tx_sched_table_offset;
        txs_hw_params.num_entries_per_cos = (lif->tx_sched_num_table_entries / num_cos_values);
        txs_hw_params.cos_bmp             = lif->cos_bmp;
        txs_hw_params.sched_qgrp_offset   = lif->tx_sched_qgrp_offset;
        ret = elba_txs_scheduler_lif_params_update(lif->hw_lif_id, &txs_hw_params);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR("lif_id : %u,failed to program tx sched lif params in hw",
                           lif->lif_id);
        }
        SDK_TRACE_DEBUG("lif_id %u, offset %u, qgrp_offset %u #entries_per_cos %u, cos_bmp %u",
                        lif->lif_id,
                        txs_hw_params.sched_table_offset,
                        txs_hw_params.sched_qgrp_offset,
                        txs_hw_params.num_entries_per_cos,
                        txs_hw_params.cos_bmp);
    }
end:
    return ret;
}

sdk_ret_t
asicpd_tx_scheduler_map_cleanup (asicpd_scheduler_lif_params_t *lif)
{
    sdk_ret_t                    ret = SDK_RET_OK;
    elba_txs_sched_lif_params_t txs_hw_params = {0};

    if (!lif) {
        goto end;
    }
    // Pass txs params for delete case.
    txs_hw_params.num_entries_per_cos = 0;
    txs_hw_params.cos_bmp             = 0x0;
    ret = elba_txs_scheduler_lif_params_update(lif->hw_lif_id, &txs_hw_params);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR("lif_id %u,failed to program tx sched lif params in hw",
                       lif->lif_id);
    }
end:
    return ret;
}

sdk_ret_t
asicpd_tx_policer_program (asicpd_scheduler_lif_params_t *lif)
{
    sdk_ret_t                         ret = SDK_RET_OK;
    uint32_t                          num_cos_values = 0;
    elba_txs_policer_lif_params_t    txs_hw_params = {0};


    if (!lif) {
        goto end;
    }
    num_cos_values = sdk::lib::count_bits_set(lif->cos_bmp);
    if (num_cos_values && lif->tx_sched_num_table_entries) {
        txs_hw_params.sched_table_start_offset  = lif->tx_sched_table_offset;
        txs_hw_params.sched_table_end_offset = (lif->tx_sched_table_offset + lif->tx_sched_num_table_entries - 1);

        ret = elba_txs_policer_lif_params_update(lif->hw_lif_id, &txs_hw_params);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR("lif_id %u,failed to program tx policer lif params in hw",
                           lif->lif_id);
        }
    }
end:
    return ret;
}

}    // namespace pd
}    // namespace asic
}    // namespace sdk
