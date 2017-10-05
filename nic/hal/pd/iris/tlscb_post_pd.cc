#include "nic/include/base.h"
#include <arpa/inet.h>
#include "nic/include/hal_lock.hpp"
#include "nic/include/pd_api.hpp"
#include "nic/hal/pd/iris/tlscb_pd.hpp"
#include "nic/hal/pd/capri/capri_loader.h"
#include "nic/hal/pd/capri/capri_hbm.hpp"
#include "nic/hal/src/proxy.hpp"
#include "nic/hal/hal.hpp"
#include "nic/hal/src/lif_manager.hpp"
#include "nic/gen/tls_txdma_post_crypto_enc/include/tls_txdma_post_crypto_enc_p4plus_ingress.h"
#include "nic/hal/pd/iris/wring_pd.hpp"
#include "nic/hal/pd/iris/p4plus_pd_api.h"

namespace hal {
namespace pd {

hal_ret_t 
p4pd_get_tls_tx_s5_t0_post_crypto_stats_entry(pd_tlscb_t* tlscb_pd)
{
    tx_table_s7_t0_d                   data = {0};
    hal_ret_t                          ret = HAL_RET_OK;

    // hardware index for this entry
    tlscb_hw_id_t hwid = tlscb_pd->hw_id + 
        (P4PD_TLSCB_STAGE_ENTRY_OFFSET * P4PD_HWID_TLS_TX_POST_CRYPTO_STATS_U16);
    
    if(!p4plus_hbm_read(hwid,  (uint8_t *)&data, sizeof(data))){
        HAL_TRACE_ERR("Failed to create tx: s6_t0_pre_crypto_stats_entry for TLS CB");
        return HAL_RET_HW_FAIL;
    }
    tlscb_pd->tlscb->rnmdr_free = ntohs(data.u.tls_post_crypto_stats5_d.rnmdr_free);
    tlscb_pd->tlscb->rnmpr_free = ntohs(data.u.tls_post_crypto_stats5_d.rnmpr_free);
    tlscb_pd->tlscb->enc_completions = ntohs(data.u.tls_post_crypto_stats5_d.enc_completions);
    tlscb_pd->tlscb->dec_completions = ntohs(data.u.tls_post_crypto_stats5_d.dec_completions);
    tlscb_pd->tlscb->post_debug_stage0_7_thread = 
      (ntohs(data.u.tls_post_crypto_stats5_d.debug_stage4_7_thread) << 16) |
      ntohs(data.u.tls_post_crypto_stats5_d.debug_stage0_3_thread);
    HAL_TRACE_DEBUG("hwid : 0x{0:x}", hwid);    
    HAL_TRACE_DEBUG("Received rnmdr free: 0x{0:x}", tlscb_pd->tlscb->rnmdr_free);
    HAL_TRACE_DEBUG("Received rnmpr free: 0x{0:x}", tlscb_pd->tlscb->rnmpr_free);
    HAL_TRACE_DEBUG("Received enc completions: 0x{0:x}", tlscb_pd->tlscb->enc_completions);
    HAL_TRACE_DEBUG("Received dec completions: 0x{0:x}", tlscb_pd->tlscb->dec_completions);
    HAL_TRACE_DEBUG("Received post debug stage0_7 thread: 0x{0:x}", tlscb_pd->tlscb->post_debug_stage0_7_thread);
    return ret;
}

}    // namespace pd
}    // namespace hal
