#ifndef __ASICPD_SCHEDULER_HPP_
#define __ASICPD_SCHEDULER_HPP_

typedef struct asicpd_scheduler_lif_params_s_ {
    uint32_t lif_id;
    uint32_t tx_sched_table_offset;             // base_addr in lif
    uint32_t tx_sched_num_table_entries;        // total number of 2k blocks in lif (num of 2k block per qgrp * count_bit_set(actiev_cos))
    uint32_t tx_sched_qgrp_offset;              // q_grp_start for the 1st qgrp in lif (qgrp offset at start of lif)
    uint32_t tx_sched_num_qgrp_entries;         // number of qgrps / lif (1 qgrp/cos)
    uint32_t total_qcount;                      // total number of queue / qgrp (not per lif)
    uint32_t hw_lif_id;                         // lif number
    uint16_t cos_bmp;                           // active_cos
} __PACK__ asicpd_scheduler_lif_params_t;

namespace sdk {
namespace asic {
namespace pd {

sdk_ret_t asicpd_tx_scheduler_map_alloc(asicpd_scheduler_lif_params_t *lif);
sdk_ret_t asicpd_tx_scheduler_map_free(asicpd_scheduler_lif_params_t *lif);
sdk_ret_t asicpd_tx_scheduler_map_program(asicpd_scheduler_lif_params_t *lif);
sdk_ret_t asicpd_tx_scheduler_map_cleanup(asicpd_scheduler_lif_params_t *lif);
sdk_ret_t asicpd_tx_policer_program(asicpd_scheduler_lif_params_t *lif);

}   // namespace pd
}   // namespace asic
}   // namespace sdk

#endif
