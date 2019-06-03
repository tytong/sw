//-----------------------------------------------------------------------------
// {C} Copyright 2017 Pensando Systems Inc. All rights reserved
//-----------------------------------------------------------------------------

#include <cstdlib>
#include "nic/include/base.hpp"
#include "nic/hal/hal.hpp"
#include "nic/hal/iris/include/hal_state.hpp"
#include "nic/hal/plugins/cfg/nw/interface.hpp"
#include "nic/hal/plugins/cfg/lif/lif.hpp"
#include "nic/include/pd.hpp"
#include "nic/include/pd_api.hpp"
#include "nic/hal/plugins/cfg/nvme/nvme.hpp"
#include "nic/utils/host_mem/host_mem.hpp"
#include "nic/p4/common/defines.h"
#include "nic/hal/plugins/cfg/mcast/oif_list_api.hpp"
#include "nic/sdk/nvme/nvme_common.h"
#include "nic/include/nvme_dpath.h"
#include "nic/hal/src/internal/wring.hpp"
#include "nic/hal/pd/capri/capri_hbm.hpp"
#include "nic/hal/pd/capri/capri_barco_rings.hpp"
#include "nvme_sesscb.hpp"
#include "nvme_global.hpp"


namespace hal {

typedef struct nvme_global_info_s {
    uint32_t max_ns;
    uint32_t cur_ns;
    uint64_t nscb_base_addr;
    uint32_t max_sess;
    uint32_t cur_sess;
    uint64_t txsessprodcb_base;
    uint64_t rxsessprodcb_base;
    uint64_t tx_sess_xtsq_base;
    uint64_t tx_sess_dgstq_base;
    uint64_t rx_sess_xtsq_base;
    uint64_t rx_sess_dgstq_base;
    uint64_t sess_bitmap_addr;
    uint32_t max_cmd_context;
    uint64_t cmd_context_page_base;
    uint64_t cmd_context_ring_base;
    uint32_t tx_max_pdu_context;
    uint64_t tx_pdu_context_page_base;
    uint64_t tx_pdu_context_ring_base;
    uint64_t tx_nmdpr_ring_base;
    uint64_t tx_nmdpr_ring_size;
    uint32_t rx_max_pdu_context;
    uint64_t rx_pdu_context_page_base;
    uint64_t rx_pdu_context_ring_base;
    uint64_t rx_nmdpr_ring_base;
    uint64_t rx_nmdpr_ring_size;
    uint64_t resourcecb_addr;
    uint64_t tx_hwxtscb_addr;
    uint64_t rx_hwxtscb_addr;
    uint64_t tx_hwdgstcb_addr;
    uint64_t rx_hwdgstcb_addr;
    uint64_t tx_xts_aol_array_addr;
    uint64_t tx_xts_iv_array_addr;
    uint64_t rx_xts_aol_array_addr;
    uint64_t rx_xts_iv_array_addr;

    //tuning params
    uint16_t sess_q_depth;
} nvme_global_info_t;

typedef struct nvme_lif_info_s {
    uint32_t max_ns;
    uint64_t nscb_base_addr;
    uint32_t max_sess;
    uint32_t cur_sess;
    uint64_t sess_bitmap_addr;
    uint32_t sess_start;
    uint32_t max_cq;
    uint64_t cqcb_base_addr;
    uint32_t max_sq;
    uint64_t sqcb_base_addr;
    uint32_t log_host_page_size;
    uint32_t ns_start;
} nvme_lif_info_t;

typedef struct nvme_ns_info_s {
    uint32_t sess_start;
    uint32_t max_sess;
    uint32_t cur_sess;
    uint32_t key_index;
    uint32_t sec_key_index;
    uint16_t log_lba_size;
} nvme_ns_info_t;

static nvme_global_info_t g_nvme_global_info;
static nvme_lif_info_t g_nvme_lif_info[MAX_LIFS];
static nvme_ns_info_t *g_nvme_ns_info = NULL;


//NVMEManager *g_nvme_manager = nullptr;
// extern sdk::platform::capri::LIFManager *lif_manager();
extern lif_mgr *lif_manager();

NVMEManager::NVMEManager() {
}

uint64_t
NVMEManager::MRStartAddress(const char *hbm_reg_name) {
    sdk::platform::utils::mpartition *mp = lif_manager()->get_mpartition();
    HAL_TRACE_DEBUG("HBM region name: {}, start_addr: {:#x}, size: {:#x}",
                    hbm_reg_name, 
                    mp->start_addr(hbm_reg_name), 
                    mp->size(hbm_reg_name));
    return (mp->start_addr(hbm_reg_name));
}

uint64_t
NVMEManager::MRSize(const char *hbm_reg_name) {
    sdk::platform::utils::mpartition *mp = lif_manager()->get_mpartition();
    HAL_TRACE_DEBUG("HBM region name: {}, start_addr: {:#x}, size: {:#x}",
                    hbm_reg_name, 
                    mp->start_addr(hbm_reg_name), 
                    mp->size(hbm_reg_name));
    return (mp->size(hbm_reg_name));
}

hal_ret_t 
nvme_hbm_write (uint64_t dst_addr, void *src_addr, uint16_t size)
{
    pd::pd_capri_hbm_write_mem_args_t args = {0};
    pd::pd_func_args_t          pd_func_args = {0};
    args.addr = dst_addr;
    args.buf = (uint8_t *)src_addr;
    args.size = size;
    pd_func_args.pd_capri_hbm_write_mem = &args;
    pd::hal_pd_call(pd::PD_FUNC_ID_HBM_WRITE, &pd_func_args);

    HAL_TRACE_DEBUG("Writing from: {:#x} to: {:#x} of size: {}",
                    src_addr, dst_addr, size);
    return (HAL_RET_OK);
}

hal_ret_t 
nvme_hbm_read (uint64_t src_addr, void *dst_addr, uint16_t size)
{
    pd::pd_capri_hbm_write_mem_args_t args = {0};
    pd::pd_func_args_t          pd_func_args = {0};
    args.addr = src_addr;
    args.buf = (uint8_t *)dst_addr;
    args.size = size;
    pd_func_args.pd_capri_hbm_write_mem = &args;
    pd::hal_pd_call(pd::PD_FUNC_ID_HBM_READ, &pd_func_args);

    HAL_TRACE_DEBUG("Reading from: {:#x} to: {:#x} of size: {}",
                    src_addr, dst_addr, size);
    return (HAL_RET_OK);
}


static uint8_t *
memrev (uint8_t *block, size_t elnum)
{
     uint8_t *s, *t, tmp;

    for (s = block, t = s + (elnum - 1); s < t; s++, t--) {
        tmp = *s;
        *s = *t;
        *t = tmp;
    }
     return block;
}

uint32_t
roundup_to_pow_2(uint32_t x)
{
    uint32_t power = 1;

    if (x == 1)
        return (power << 1);

    while(power < x)
        power*=2;
    return power;
}

hal_ret_t
nvme_enable (NvmeEnableRequest& spec, NvmeEnableResponse *rsp)
{
    int32_t            max_ns;
    int32_t            max_sess;
    int32_t            max_cmd_context;
    int32_t            tx_max_pdu_context;
    int32_t            rx_max_pdu_context;
    hal_ret_t          ret;
    wring_t            wring;
    uint64_t           opaque_tag_addr;

    HAL_TRACE_DEBUG("--------------------- API Start ------------------------");
 
    HAL_TRACE_DEBUG("NVME Enable Request");

    max_ns  = spec.max_ns();
    max_sess  = spec.max_sess();
    max_cmd_context  = spec.max_cmd_context();
    tx_max_pdu_context = spec.tx_max_pdu_context();
    rx_max_pdu_context = spec.rx_max_pdu_context();

    HAL_TRACE_DEBUG("max_ns: {}, max_sess: {}, max_cmd_context: {}, "
                    "tx_max_pdu_context: {}, rx_max_pdu_context: {}\n",
                     max_ns, max_sess, max_cmd_context, 
                     tx_max_pdu_context, rx_max_pdu_context);

    SDK_ASSERT(max_ns <= nvme_hbm_resource_max(NVME_TYPE_NSCB));
    SDK_ASSERT(max_sess <= nvme_hbm_resource_max(NVME_TYPE_TX_SESSPRODCB));
    SDK_ASSERT(max_cmd_context <= nvme_hbm_resource_max(NVME_TYPE_CMD_CONTEXT));
    SDK_ASSERT(tx_max_pdu_context <= nvme_hbm_resource_max(NVME_TYPE_TX_PDU_CONTEXT));
    SDK_ASSERT(rx_max_pdu_context <= nvme_hbm_resource_max(NVME_TYPE_RX_PDU_CONTEXT));
    SDK_ASSERT(nvme_hbm_offset(NVME_TYPE_MAX) <= (int)nvme_manager()->MRSize(CAPRI_HBM_REG_NVME));

    memset(&g_nvme_global_info, 0, sizeof(g_nvme_global_info));

    g_nvme_global_info.max_ns = max_ns;
    g_nvme_global_info.max_sess = max_sess;
    g_nvme_global_info.max_cmd_context = max_cmd_context;
    g_nvme_global_info.tx_max_pdu_context = tx_max_pdu_context;
    g_nvme_global_info.rx_max_pdu_context = rx_max_pdu_context;

    uint64_t nvme_hbm_start = nvme_manager()->MRStartAddress(CAPRI_HBM_REG_NVME);
    //ns
    g_nvme_global_info.nscb_base_addr = nvme_hbm_start + nvme_hbm_offset(NVME_TYPE_NSCB);

    //txsessprodcb
    g_nvme_global_info.txsessprodcb_base = nvme_hbm_start + nvme_hbm_offset(NVME_TYPE_TX_SESSPRODCB);

    //rxsessprodcb
    g_nvme_global_info.rxsessprodcb_base = nvme_hbm_start + nvme_hbm_offset(NVME_TYPE_RX_SESSPRODCB);

    //tx_sess_xtsq
    g_nvme_global_info.tx_sess_xtsq_base = nvme_hbm_start + nvme_hbm_offset(NVME_TYPE_TX_SESS_XTSQ);

    //tx_sess_dgstq
    g_nvme_global_info.tx_sess_dgstq_base = nvme_hbm_start + nvme_hbm_offset(NVME_TYPE_TX_SESS_DGSTQ);

    //rx_sess_xtsq
    g_nvme_global_info.rx_sess_xtsq_base = nvme_hbm_start + nvme_hbm_offset(NVME_TYPE_RX_SESS_XTSQ);

    //rx_sess_dgstq
    g_nvme_global_info.rx_sess_dgstq_base = nvme_hbm_start + nvme_hbm_offset(NVME_TYPE_RX_SESS_DGSTQ);

    //resourcecb
    g_nvme_global_info.resourcecb_addr = nvme_hbm_start + nvme_hbm_offset(NVME_TYPE_RESOURCECB);

    //tx_hwxtscb
    opaque_tag_addr = get_mem_addr(CAPRI_HBM_REG_OPAQUE_TAG) + hal::pd::get_opaque_tag_offset(types::BARCO_RING_XTS0);
    g_nvme_global_info.tx_hwxtscb_addr = opaque_tag_addr;

    //rx_hwxtscb
    opaque_tag_addr = get_mem_addr(CAPRI_HBM_REG_OPAQUE_TAG) + hal::pd::get_opaque_tag_offset(types::BARCO_RING_XTS1);
    g_nvme_global_info.rx_hwxtscb_addr = opaque_tag_addr;

    //tx_hwdgstcb
    opaque_tag_addr = get_mem_addr(CAPRI_HBM_REG_OPAQUE_TAG) + hal::pd::get_opaque_tag_offset(types::BARCO_RING_CP);
    g_nvme_global_info.tx_hwdgstcb_addr = opaque_tag_addr;

    //rx_hwdgstcb
    opaque_tag_addr = get_mem_addr(CAPRI_HBM_REG_OPAQUE_TAG) + hal::pd::get_opaque_tag_offset(types::BARCO_RING_DC);
    g_nvme_global_info.rx_hwdgstcb_addr = opaque_tag_addr;

    //cmd context page
    g_nvme_global_info.cmd_context_page_base = nvme_hbm_start + nvme_hbm_offset(NVME_TYPE_CMD_CONTEXT); 

    //cmd context ring
    g_nvme_global_info.cmd_context_ring_base = nvme_hbm_start + nvme_hbm_offset(NVME_TYPE_CMD_CONTEXT_RING); 

    //tx pdu context page
    g_nvme_global_info.tx_pdu_context_page_base = nvme_hbm_start + nvme_hbm_offset(NVME_TYPE_TX_PDU_CONTEXT); 

    //tx pdu context ring
    g_nvme_global_info.tx_pdu_context_ring_base = nvme_hbm_start + nvme_hbm_offset(NVME_TYPE_TX_PDU_CONTEXT_RING); 

    //rx pdu context page
    g_nvme_global_info.rx_pdu_context_page_base = nvme_hbm_start + nvme_hbm_offset(NVME_TYPE_RX_PDU_CONTEXT); 

    //rx pdu context ring
    g_nvme_global_info.rx_pdu_context_ring_base = nvme_hbm_start + nvme_hbm_offset(NVME_TYPE_RX_PDU_CONTEXT_RING); 

    //tx aol ring
    g_nvme_global_info.tx_xts_aol_array_addr = nvme_hbm_start + nvme_hbm_offset(NVME_TYPE_TX_XTS_AOL_ARRAY); 

    //tx iv ring
    g_nvme_global_info.tx_xts_iv_array_addr = nvme_hbm_start + nvme_hbm_offset(NVME_TYPE_TX_XTS_IV_ARRAY); 

    //rx aol ring
    g_nvme_global_info.rx_xts_aol_array_addr = nvme_hbm_start + nvme_hbm_offset(NVME_TYPE_RX_XTS_AOL_ARRAY); 

    //rx iv ring
    g_nvme_global_info.rx_xts_iv_array_addr = nvme_hbm_start + nvme_hbm_offset(NVME_TYPE_RX_XTS_IV_ARRAY); 

    //NS runtime info
    g_nvme_ns_info = (nvme_ns_info_t *)malloc(sizeof(nvme_ns_info_t) * max_ns);
    SDK_ASSERT(g_nvme_ns_info != NULL);
    memset(g_nvme_ns_info, 0, sizeof(nvme_ns_info_t) * max_ns);

    HAL_TRACE_DEBUG("nscb_base_addr: {:#x} "
                    "txsessprodcb_base: {:#x}, "
                    "tx_sess_xtsq_base: {:#x}, "
                    "tx_sess_dgstq_base: {:#x}, "
                    "resourcecb_addr: {:#x}, "
                    "tx_hwxtscb_addr: {:#x}, "
                    "tx_hwdgstcb_addr: {:#x}, "
                    "rxsessprodcb_base: {:#x}, "
                    "rx_sess_xtsq_base: {:#x}, "
                    "rx_sess_dgstq_base: {:#x}, "
                    "rx_hwxtscb_addr: {:#x}, "
                    "rx_hwdgstcb_addr: {:#x}, "
                    "sess_bitmap_addr: {:#x}\n",
                    g_nvme_global_info.nscb_base_addr, 
                    g_nvme_global_info.txsessprodcb_base,
                    g_nvme_global_info.tx_sess_xtsq_base,
                    g_nvme_global_info.tx_sess_dgstq_base,
                    g_nvme_global_info.resourcecb_addr,
                    g_nvme_global_info.tx_hwxtscb_addr,
                    g_nvme_global_info.tx_hwdgstcb_addr,
                    g_nvme_global_info.rxsessprodcb_base,
                    g_nvme_global_info.rx_sess_xtsq_base,
                    g_nvme_global_info.rx_sess_dgstq_base,
                    g_nvme_global_info.rx_hwxtscb_addr,
                    g_nvme_global_info.rx_hwdgstcb_addr,
                    g_nvme_global_info.sess_bitmap_addr);

    HAL_TRACE_DEBUG("cmd_context_page_base: {:#x}, "
                    "g_nvme_ns_info: {:#x}, " 
                    "cmd_context_ring_base: {:#x}, "
                    "tx_pdu_context_page_base: {:#x}, tx_pdu_context_ring_base: {:#x}, "
                    "rx_pdu_context_page_base: {:#x}, rx_pdu_context_ring_base: {:#x}, "
                    "tx_xts_aol_array_addr: {:#x}, tx_xts_iv_array_addr : {:#x}, "
                    "rx_xts_aol_array_addr: {:#x}, rx_xts_iv_array_addr : {:#x}",
                    g_nvme_global_info.cmd_context_page_base,
                    (uint64_t)g_nvme_ns_info,
                    g_nvme_global_info.cmd_context_ring_base,
                    g_nvme_global_info.tx_pdu_context_page_base, 
                    g_nvme_global_info.tx_pdu_context_ring_base,
                    g_nvme_global_info.rx_pdu_context_page_base, 
                    g_nvme_global_info.rx_pdu_context_ring_base,
                    g_nvme_global_info.tx_xts_aol_array_addr, 
                    g_nvme_global_info.tx_xts_iv_array_addr, 
                    g_nvme_global_info.rx_xts_aol_array_addr, 
                    g_nvme_global_info.rx_xts_iv_array_addr);

    rsp->set_cmd_context_page_base(g_nvme_global_info.cmd_context_page_base);
    rsp->set_cmd_context_ring_base(g_nvme_global_info.cmd_context_ring_base);

    // Get TX_NMDPR_RING_BASE
    ret = wring_get_meta(types::WRING_TYPE_NMDPR_BIG_TX,
                         0,
                         &wring);
    if(ret != HAL_RET_OK) {
        HAL_TRACE_ERR("Failed to receive NMDPR ring base");
        return HAL_RET_ERR;
    } else {
        g_nvme_global_info.tx_nmdpr_ring_base = wring.phys_base_addr;
        g_nvme_global_info.tx_nmdpr_ring_size = wring.num_entries;
        HAL_TRACE_DEBUG("tx_nmdpr_ring_base: {:#x}, size: {}", 
                        g_nvme_global_info.tx_nmdpr_ring_base, 
                        g_nvme_global_info.tx_nmdpr_ring_size);
    }

    // Get RX_NMDPR_RING_BASE
    ret = wring_get_meta(types::WRING_TYPE_NMDPR_BIG_RX,
                         0,
                         &wring);
    if(ret != HAL_RET_OK) {
        HAL_TRACE_ERR("Failed to receive NMDPR ring base");
        return HAL_RET_ERR;
    } else {
        g_nvme_global_info.rx_nmdpr_ring_base = wring.phys_base_addr;
        g_nvme_global_info.rx_nmdpr_ring_size = wring.num_entries;
        HAL_TRACE_DEBUG("rx_nmdpr_ring_base: {:#x}, size: {}", 
                        g_nvme_global_info.rx_nmdpr_ring_base,
                        g_nvme_global_info.rx_nmdpr_ring_size);
    }

    ret = nvme_global_create(MAX_LIFS, max_ns, max_sess, 
                             max_cmd_context, tx_max_pdu_context,
                             rx_max_pdu_context);
    
    if (ret != HAL_RET_OK) {
        HAL_TRACE_ERR("Failed(ret: {:#x}) to create PD Nvme Global context",
                      ret);
        return HAL_RET_ERR;
    }

    rsp->set_api_status(types::API_STATUS_OK);
    HAL_TRACE_DEBUG("----------------------- API End ------------------------");

    return (HAL_RET_OK);
}

hal_ret_t
nvme_lif_init (intf::LifSpec& spec, uint32_t lif)
{
    uint32_t            max_ns, max_sess;
    uint32_t            max_cqs, max_sqs;
    uint64_t            cqcb_base_addr; //address in HBM memory
    uint64_t            sqcb_base_addr; //address in HBM memory
    nvme_lif_info_t     *nvme_lif_info_p;

    HAL_TRACE_DEBUG("cur_ns: {}, max_ns: {}, cur_sess: {}, max_sess: {}\n",
                    g_nvme_global_info.cur_ns, g_nvme_global_info.max_ns,
                    g_nvme_global_info.cur_sess, g_nvme_global_info.max_sess);

    HAL_TRACE_DEBUG("lif: {} spec_lif: {}", lif, spec.key_or_handle().lif_id());

    //SDK_ASSERT(lif == spec.key_or_handle().lif_id());
    SDK_ASSERT(lif < MAX_LIFS);

    max_ns = spec.nvme_max_ns();
    max_sess = spec.nvme_max_sess();

    HAL_TRACE_DEBUG("LIF: {}, max_ns: {}, max_sess: {}",
                    lif, max_ns, max_sess);

    SDK_ASSERT((g_nvme_global_info.cur_ns + max_ns) < g_nvme_global_info.max_ns);
    SDK_ASSERT((g_nvme_global_info.cur_sess + max_sess) < g_nvme_global_info.max_sess);

    nvme_lif_info_p = &g_nvme_lif_info[lif];

    memset(nvme_lif_info_p, 0, sizeof(nvme_lif_info_t));

    // LIFQState *qstate = lif_manager()->GetLIFQState(lif);
    lif_qstate_t *qstate = lif_manager()->get_lif_qstate(lif);
    if (qstate == nullptr)
        return HAL_RET_ERR;

    max_cqs  = qstate->type[NVME_QTYPE_CQ].num_queues;
    max_sqs  = qstate->type[NVME_QTYPE_SQ].num_queues;

    HAL_TRACE_DEBUG("LIF {}, max_CQ: {}, max_SQ: {}",
                    lif, max_cqs, max_sqs);

    nvme_lif_info_p->nscb_base_addr = g_nvme_global_info.nscb_base_addr + 
                                          g_nvme_global_info.cur_ns * sizeof(nvme_nscb_t);
    nvme_lif_info_p->ns_start = g_nvme_global_info.cur_ns;
    g_nvme_global_info.cur_ns += max_ns;
    
    nvme_lif_info_p->sess_start = g_nvme_global_info.cur_sess;
    g_nvme_global_info.cur_sess += max_sess;
    
    nvme_lif_info_p->sess_bitmap_addr = g_nvme_global_info.sess_bitmap_addr + 
                                            BITS_TO_BYTES(g_nvme_global_info.cur_sess);

    nvme_lif_info_p->max_ns = max_ns;
    nvme_lif_info_p->max_sess = max_sess;
    nvme_lif_info_p->cur_sess = 0;

    cqcb_base_addr = lif_manager()->get_lif_qstate_base_addr(lif, NVME_QTYPE_CQ);
    HAL_TRACE_DEBUG("Lif {} cqcb_base_addr: {:#x}, max_cqs: {} log_max_cq_entries: {}",
                    lif, cqcb_base_addr,
                    max_cqs, log2(roundup_to_pow_2(max_cqs)));
                    nvme_lif_info_p->cqcb_base_addr = cqcb_base_addr;
                    nvme_lif_info_p->max_cq = max_cqs;

    sqcb_base_addr = lif_manager()->get_lif_qstate_base_addr(lif, NVME_QTYPE_SQ);
    HAL_TRACE_DEBUG("Lif {} sqcb_base_addr: {:#x}, max_sqs: {} log_max_sq_entries: {}",
                    lif, sqcb_base_addr,
                    max_sqs, log2(roundup_to_pow_2(max_sqs)));
                    nvme_lif_info_p->sqcb_base_addr = sqcb_base_addr;
                    nvme_lif_info_p->max_sq = max_sqs;

    SDK_ASSERT((spec.nvme_host_page_size() & (spec.nvme_host_page_size() - 1)) == 0);
    nvme_lif_info_p->log_host_page_size = log2(spec.nvme_host_page_size());

    HAL_TRACE_DEBUG("Lif: {}: max_sq: {}, sqcb_base_addr: {:#x}, "
                    "max_cq: {}, cqcb_base_addr: {:#x}, "
                    "max_ns: {}, nscb_base_addr: {:#x}, ns_start: {}, "
                    "max_sess: {}, sess_start: {} "
                    "sess_bitmap_addr: {}, log_host_page_size: {}",
                    lif,
                    nvme_lif_info_p->max_sq, 
                    nvme_lif_info_p->sqcb_base_addr,
                    nvme_lif_info_p->max_cq, 
                    nvme_lif_info_p->cqcb_base_addr,
                    nvme_lif_info_p->max_ns, 
                    nvme_lif_info_p->nscb_base_addr,
                    nvme_lif_info_p->ns_start,
                    nvme_lif_info_p->max_sess, 
                    nvme_lif_info_p->sess_start, 
                    nvme_lif_info_p->sess_bitmap_addr,
                    nvme_lif_info_p->log_host_page_size);

    HAL_TRACE_DEBUG("Lif: {}: LIF Init successful\n", lif);

    return HAL_RET_OK;
}

/*
 * Utility functions to read/write PT entries.
 * Ideally, HAL should never program PT entries.
 */
void
nvme_ns_entry_write (uint16_t lif, uint32_t index, uint64_t *ns_ptr)
{
    uint64_t            ns_table_base_addr;

    SDK_ASSERT(lif < MAX_LIFS);

    ns_table_base_addr = g_nvme_lif_info[lif].nscb_base_addr;

    pd::pd_capri_hbm_write_mem_args_t args = {0};
    pd::pd_func_args_t          pd_func_args = {0};
    args.addr = (uint64_t)(ns_table_base_addr + (index * sizeof(nvme_nscb_t)));
    args.buf = (uint8_t*)ns_ptr;
    args.size = sizeof(nvme_nscb_t);
    pd_func_args.pd_capri_hbm_write_mem = &args;
    pd::hal_pd_call(pd::PD_FUNC_ID_HBM_WRITE, &pd_func_args);
}

void
nvme_ns_entry_read (uint16_t lif, uint32_t index, uint64_t *ns_ptr)
{
    uint64_t            ns_table_base_addr;

    SDK_ASSERT(lif < MAX_LIFS);

    ns_table_base_addr = g_nvme_lif_info[lif].nscb_base_addr;

    pd::pd_capri_hbm_write_mem_args_t args = {0};
    pd::pd_func_args_t          pd_func_args = {0};
    args.addr = (uint64_t)(ns_table_base_addr + (index * sizeof(nvme_nscb_t)));
    args.buf = (uint8_t*)ns_ptr;
    args.size = sizeof(nvme_nscb_t);
    pd_func_args.pd_capri_hbm_write_mem = &args;
    pd::hal_pd_call(pd::PD_FUNC_ID_HBM_READ, &pd_func_args);
}

/*
 * TODO: Need to remove this hardcoded values. They will go away
 * anyway once we move the code out to nicmgr.
 */
#define CAP_ADDR_BASE_INTR_INTR_OFFSET 0x6000000
#define CAP_INTR_CSR_DHS_INTR_ASSERT_BYTE_OFFSET 0x68000
#define INTR_BASE               CAP_ADDR_BASE_INTR_INTR_OFFSET
#define INTR_ASSERT_OFFSET      CAP_INTR_CSR_DHS_INTR_ASSERT_BYTE_OFFSET
#define INTR_ASSERT_BASE        (INTR_BASE + INTR_ASSERT_OFFSET)
#define INTR_ASSERT_STRIDE      0x4

static u_int64_t
intr_assert_addr(const int intr)
{
    return INTR_ASSERT_BASE + (intr * INTR_ASSERT_STRIDE);
}

hal_ret_t
get_program_offset (char *progname, char *labelname, uint64_t *offset)
{
    pd::pd_capri_program_label_to_offset_args_t args = {0};
    pd::pd_func_args_t          pd_func_args = {0};
    args.handle = "p4plus";
    args.prog_name = progname;
    args.label_name = labelname;
    args.offset = offset;
    pd_func_args.pd_capri_program_label_to_offset = &args;
    hal_ret_t ret = pd::hal_pd_call(pd::PD_FUNC_ID_PROG_LBL_TO_OFFSET, &pd_func_args);

    if(ret != HAL_RET_OK) {
        HAL_TRACE_ERR("{}: ret: {}\n", __FUNCTION__, ret);
        return HAL_RET_HW_FAIL;
   }
    return HAL_RET_OK;
}

hal_ret_t
nvme_sq_create (NvmeSqSpec& spec, NvmeSqResponse *rsp)
{
    uint32_t     lif = spec.hw_lif_id();
    uint32_t     num_sq_wqes, sqwqe_size;
    nvme_sqcb_t       sqcb;
    uint64_t     offset;

    HAL_TRACE_DEBUG("--------------------- API Start ------------------------");
    HAL_TRACE_DEBUG("PI-LIF: NVME SQ Create for lif {}", lif);


    HAL_TRACE_DEBUG("Inputs: sq_num: {} sq_wqe_size: {} num_sq_wqes: {} "
                    "base_addr: {} cq_num: {} lif_ns_start: {}",
                    spec.sq_num(),
                    spec.sq_wqe_size(), spec.num_sq_wqes(), 
                    spec.base_addr(), spec.cq_num(),
                    g_nvme_lif_info[lif].ns_start);

    SDK_ASSERT(lif < MAX_LIFS);
    SDK_ASSERT(spec.sq_num() < g_nvme_lif_info[lif].max_sq);
    SDK_ASSERT(spec.cq_num() < g_nvme_lif_info[lif].max_cq);

    sqwqe_size = roundup_to_pow_2(spec.sq_wqe_size());
    num_sq_wqes = roundup_to_pow_2(spec.num_sq_wqes());

    HAL_TRACE_DEBUG("sqwqe_size: {} num_sqwqes: {}", sqwqe_size, num_sq_wqes);

    memset(&sqcb, 0, sizeof(nvme_sqcb_t));
    sqcb.ring_header.total_rings = MAX_SQ_RINGS;
    sqcb.ring_header.host_rings = MAX_SQ_HOST_RINGS;

    sqcb.base_addr = spec.base_addr();

    sqcb.log_wqe_size = log2(sqwqe_size);
    sqcb.log_num_wqes = log2(num_sq_wqes);
    sqcb.log_host_page_size = g_nvme_lif_info[lif].log_host_page_size;
    sqcb.cq_id = spec.cq_num();
    sqcb.lif_ns_start = g_nvme_lif_info[lif].ns_start;
    get_program_offset((char *)"txdma_stage0.bin", 
                       (char *)"nvme_sq_stage0",
                       &offset);
    sqcb.ring_header.pc = offset >> 6;

    HAL_TRACE_DEBUG("sqid: {}, pc: {:#x}", spec.sq_num(), offset);

    // write to hardware
    HAL_TRACE_DEBUG("LIF: {}: Writing initial SQCB State, baseaddr: {:#x} sqcb_size: {}",
                    lif, sqcb.base_addr, sizeof(nvme_sqcb_t));
    // Convert data before writting to HBM
    memrev((uint8_t*)&sqcb, sizeof(nvme_sqcb_t));
    lif_manager()->write_qstate(lif, NVME_QTYPE_SQ, spec.sq_num(), (uint8_t *)&sqcb, sizeof(nvme_sqcb_t));
    HAL_TRACE_DEBUG("QstateAddr = {:#x}\n", 
                    lif_manager()->get_lif_qstate_addr(lif, NVME_QTYPE_SQ, spec.sq_num()));

    rsp->set_api_status(types::API_STATUS_OK);
    HAL_TRACE_DEBUG("----------------------- API End ------------------------");

    return (HAL_RET_OK);
}

hal_ret_t
nvme_cq_create (NvmeCqSpec& spec, NvmeCqResponse *rsp)
{
    uint32_t     lif = spec.hw_lif_id();
    uint32_t     num_cq_wqes, cqwqe_size;
    nvme_cqcb_t       cqcb;

    HAL_TRACE_DEBUG("--------------------- API Start ------------------------");
    HAL_TRACE_DEBUG("PI-LIF:NVME CQ Create for lif {}", lif);


    HAL_TRACE_DEBUG("Inputs: cq_num: {} cq_wqe_size: {} num_cq_wqes: {} "
                    "base_addr: {} int_num: {}",
                    spec.cq_num(),
                    spec.cq_wqe_size(), spec.num_cq_wqes(), 
                    spec.base_addr(), spec.int_num());

    SDK_ASSERT(lif < MAX_LIFS);
    SDK_ASSERT(spec.cq_num() < g_nvme_lif_info[lif].max_cq);

    cqwqe_size = roundup_to_pow_2(spec.cq_wqe_size());
    num_cq_wqes = roundup_to_pow_2(spec.num_cq_wqes());

    HAL_TRACE_DEBUG("cqwqe_size: {} num_cq_wqes: {}", cqwqe_size, num_cq_wqes);

    memset(&cqcb, 0, sizeof(nvme_cqcb_t));
    cqcb.ring_header.total_rings = MAX_CQ_RINGS;
    cqcb.ring_header.host_rings = MAX_CQ_HOST_RINGS;

    cqcb.base_addr = spec.base_addr();

    cqcb.log_wqe_size = log2(cqwqe_size);
    cqcb.log_num_wqes = log2(num_cq_wqes);
    cqcb.color = 0;

    cqcb.int_assert_addr = intr_assert_addr(spec.int_num());
    rsp->set_cq_intr_tbl_addr(cqcb.int_assert_addr);

    // write to hardware
    HAL_TRACE_DEBUG("LIF: {}: Writing initial CQCB State, baseaddr: {:#x} cqcb_size: {}",
                    lif, cqcb.base_addr, sizeof(nvme_cqcb_t));
    // Convert data before writting to HBM
    memrev((uint8_t*)&cqcb, sizeof(nvme_cqcb_t));
    lif_manager()->write_qstate(lif, NVME_QTYPE_CQ, spec.cq_num(), (uint8_t *)&cqcb, sizeof(nvme_cqcb_t));
    HAL_TRACE_DEBUG("QstateAddr = {:#x}\n",
                    lif_manager()->get_lif_qstate_addr(lif, NVME_QTYPE_CQ, spec.cq_num()));

    rsp->set_api_status(types::API_STATUS_OK);
    HAL_TRACE_DEBUG("----------------------- API End ------------------------");

    return (HAL_RET_OK);
}


hal_ret_t
nvme_ns_create (NvmeNsSpec& spec, NvmeNsResponse *rsp)
{
    uint32_t     lif = spec.hw_lif_id();
    nvme_nscb_t  nscb;
    uint64_t     nscb_addr;
    uint32_t     g_nsid;
    nvme_ns_info_t *ns_info_p;

    HAL_TRACE_DEBUG("--------------------- API Start ------------------------");
    HAL_TRACE_DEBUG("PI-LIF: NVME NS Create for lif {}", lif);


    HAL_TRACE_DEBUG("Inputs: nsid: {} backend_nsid: {} max_sessions: {} "
                    "size: {} lba_size: {} key_index: {} sec_key_index: {}",
                    spec.nsid(),
                    spec.backend_nsid(), spec.max_sess(), 
                    spec.size(), spec.lba_size(),
                    spec.key_index(), spec.sec_key_index());

    SDK_ASSERT(lif < MAX_LIFS);
    SDK_ASSERT(spec.nsid() != 0);
    SDK_ASSERT(spec.nsid() <= g_nvme_lif_info[lif].max_ns);
    SDK_ASSERT((spec.lba_size() & (spec.lba_size() - 1)) == 0); //power of 2 check
    SDK_ASSERT(g_nvme_lif_info[lif].cur_sess + spec.max_sess() <= g_nvme_lif_info[lif].max_sess);

    //1 based
    nscb_addr = g_nvme_lif_info[lif].nscb_base_addr + (spec.nsid() - 1) * sizeof(nvme_nscb_t);

    memset(&nscb, 0, sizeof(nvme_nscb_t));
    nscb.backend_ns_id = spec.backend_nsid();
    nscb.ns_size = spec.size(); //size in LBAs
    nscb.key_index = spec.key_index();
    nscb.sec_key_index = spec.sec_key_index();
    nscb.log_lba_size = log2(spec.lba_size());
    nscb.sess_prodcb_start = (g_nvme_lif_info[lif].sess_start + g_nvme_lif_info[lif].cur_sess);

    //update global ns info
    g_nsid = g_nvme_lif_info[lif].ns_start + spec.nsid() - 1;
    SDK_ASSERT(g_nsid <= g_nvme_global_info.max_ns);

    ns_info_p = &g_nvme_ns_info[g_nsid];
    ns_info_p->sess_start = g_nvme_lif_info[lif].sess_start + g_nvme_lif_info[lif].cur_sess;
    ns_info_p->max_sess = spec.max_sess();
    ns_info_p->cur_sess = 0;
    ns_info_p->key_index = spec.key_index();
    ns_info_p->sec_key_index = spec.sec_key_index();
    ns_info_p->log_lba_size = log2(spec.lba_size());

    g_nvme_lif_info[lif].cur_sess += spec.max_sess();

    HAL_TRACE_DEBUG("ns->sess_start: {}, lif->curr_sess: {},  key_index: {} sec_key_index: {}",
                    ns_info_p->sess_start, 
                    g_nvme_lif_info[lif].cur_sess,
                    ns_info_p->key_index, ns_info_p->sec_key_index);

    rsp->set_nscb_addr(nscb_addr);

    // write to hardware
    HAL_TRACE_DEBUG("LIF: {}: Writing initial NSCB State, addr: {:#x}, size: {}",
                    lif, nscb_addr, sizeof(nvme_nscb_t));

    // Convert data before writting to HBM
    memrev((uint8_t*)&nscb, sizeof(nvme_nscb_t));

    nvme_hbm_write(nscb_addr, (void *)&nscb, sizeof(nvme_nscb_t));

    rsp->set_api_status(types::API_STATUS_OK);
    HAL_TRACE_DEBUG("----------------------- API End ------------------------");

    return (HAL_RET_OK);
}

static hal_ret_t
nvme_ns_update_session_id (uint32_t lif, uint32_t nsid, uint16_t sess_id)
{
    nvme_ns_info_t *ns_info_p;
    uint64_t       nscb_addr;
    uint32_t       g_nsid;
    nvme_nscb_t    nscb;
    uint8_t        bit_index, byte_index;

    SDK_ASSERT(lif < MAX_LIFS);
    SDK_ASSERT(nsid <= g_nvme_lif_info[lif].max_ns);

    g_nsid = g_nvme_lif_info[lif].ns_start + nsid - 1;
    SDK_ASSERT(g_nsid <= g_nvme_global_info.max_ns);

    SDK_ASSERT(sess_id < 256); //XXX

    ns_info_p = &g_nvme_ns_info[g_nsid];
    SDK_ASSERT(sess_id < ns_info_p->max_sess);

    nscb_addr = g_nvme_lif_info[lif].nscb_base_addr + (nsid - 1) * sizeof(nvme_nscb_t);

    nvme_hbm_read(nscb_addr, (void *)&nscb, sizeof(nvme_nscb_t));
    // Convert data after reading from HBM
    memrev((uint8_t*)&nscb, sizeof(nvme_nscb_t));

    byte_index = sess_id / 8; //Byte number
    bit_index  = sess_id % 8; //Bit in Byte
    nscb.valid_session_bitmap[byte_index] |= (1 << bit_index);

    // Convert data before writting to HBM
    memrev((uint8_t*)&nscb, sizeof(nvme_nscb_t));
    nvme_hbm_write(nscb_addr, (void *)&nscb, sizeof(nvme_nscb_t));

    return (HAL_RET_OK);
}

hal_ret_t
nvme_sess_create (NvmeSessSpec& spec, NvmeSessResponse *rsp)
{
    uint32_t      lif = spec.hw_lif_id();
    uint64_t      txsessprodcb_addr;
    uint64_t      tx_sess_xtsq_base;
    uint64_t      tx_sess_dgstq_base;
    uint64_t      rxsessprodcb_addr;
    uint64_t      rx_sess_xtsq_base;
    uint64_t      rx_sess_dgstq_base;
    uint32_t      ns_sess_id; //NS local
    uint32_t      lif_sess_id; //LIF local
    uint32_t      g_sess_id; //Global
    uint32_t      sesq_qid;
    uint32_t      serq_qid;
    uint64_t      sesq_base;
    uint64_t      serq_base;
    uint32_t      sesq_size;
    uint32_t      serq_size;
    hal_ret_t     ret;
    uint32_t      g_nsid;
    nvme_ns_info_t *ns_info_p;
    wring_t       wring;

    HAL_TRACE_DEBUG("--------------------- API Start ------------------------");
    HAL_TRACE_DEBUG("PI-LIF: NVME Sess Create for lif {}", lif);


    HAL_TRACE_DEBUG("Inputs: nsid: {}",
                    spec.nsid())

    SDK_ASSERT(lif < MAX_LIFS);
    SDK_ASSERT(spec.nsid() != 0);
    SDK_ASSERT(spec.nsid() <= g_nvme_lif_info[lif].max_ns);

    g_nsid = g_nvme_lif_info[lif].ns_start + spec.nsid() - 1;
    SDK_ASSERT(g_nsid <= g_nvme_global_info.max_ns);

    ns_info_p = &g_nvme_ns_info[g_nsid];

    SDK_ASSERT(ns_info_p->cur_sess < (ns_info_p->max_sess - 1));

    //Get tcp qid associated with the flow
    proxy_flow_info_t*  pfi = NULL;
    flow_key_t          flow_key = {0};
    vrf_id_t            tid = 0;

    tid = spec.vrf_key_handle().vrf_id();
    extract_flow_key_from_spec(tid, &flow_key, spec.flow_key());

    HAL_TRACE_DEBUG("vrf_id: {}, tid: {}, flow_key: {}",
                    spec.vrf_key_handle().vrf_id(),
                    tid, flow_key);

    pfi = proxy_get_flow_info(types::PROXY_TYPE_TCP, &flow_key);
    if(!pfi) {
        HAL_TRACE_ERR("flow info not found for the flow {}", flow_key);
        rsp->set_api_status(types::API_STATUS_NOT_FOUND);
        return HAL_RET_PROXY_NOT_FOUND;
    }

    SDK_ASSERT(pfi != NULL);
    SDK_ASSERT(pfi->proxy != NULL);
    SDK_ASSERT(pfi->proxy->type == types::PROXY_TYPE_TCP);

    serq_qid = sesq_qid = pfi->qid2;

    HAL_TRACE_DEBUG("TCP Flow LIF: {}, QType: {}, QID1: {}, QID2: {}",
                    pfi->proxy->meta->lif_info[0].lif_id,
                    pfi->proxy->meta->lif_info[0].qtype_info[0].qtype_val,
                    pfi->qid1, pfi->qid2);

    //global session id
    ns_sess_id = ns_info_p->cur_sess++;
    g_sess_id = ns_info_p->sess_start + ns_sess_id;
    SDK_ASSERT(g_sess_id >= g_nvme_lif_info[lif].sess_start);
    lif_sess_id = g_sess_id - g_nvme_lif_info[lif].sess_start;
    SDK_ASSERT(g_sess_id < g_nvme_global_info.max_sess);

    HAL_TRACE_DEBUG("NS Local session id: {}, LIF local session id: {}, Global session id: {}",
                    ns_sess_id, lif_sess_id, g_sess_id);
    //LIF local session id
    rsp->set_sess_id(lif_sess_id);

    ret = nvme_sesscb_create(lif, g_sess_id, lif_sess_id, ns_sess_id, sesq_qid, serq_qid);

    if(ret != HAL_RET_OK) {
        HAL_TRACE_ERR("Failed(ret: {}) to create nvme_sesscb for lif: {} sess_id: {}",
                      ret, lif, g_sess_id);
        return HAL_RET_ERR;
    }

    //update NSCB active session bitmap
    nvme_ns_update_session_id(lif, spec.nsid(), ns_sess_id);


    // Get Sesq address
    ret = wring_get_meta(types::WRING_TYPE_SESQ,
                         sesq_qid,
                         &wring);
    if(ret != HAL_RET_OK) {
        HAL_TRACE_ERR("Failed to receive serq base for transport TCPQ: {}",
                      sesq_qid);
        return HAL_RET_ERR;
    } else {
        sesq_base = wring.phys_base_addr;
        sesq_size = wring.num_entries;
        HAL_TRACE_DEBUG("Sesq id: {:#x} Sesq base: {:#x}, size: {}", 
                        sesq_qid, sesq_base, sesq_size);
    }
    rsp->set_tx_sesq_base(sesq_base);
    rsp->set_tx_sesq_num_entries(sesq_size);

    // Get Serq address
    ret = wring_get_meta(types::WRING_TYPE_SERQ,
                         serq_qid,
                         &wring);
    if(ret != HAL_RET_OK) {
        HAL_TRACE_ERR("Failed to receive serq base for transport TCPQ: {}",
                      serq_qid);
        return HAL_RET_ERR;
    } else {
        serq_base = wring.phys_base_addr;
        serq_size = wring.num_entries;
        HAL_TRACE_DEBUG("Serq id: {:#x} Serq base: {:#x}, size: {}", 
                        serq_qid, serq_base, serq_size);
    }
    rsp->set_rx_serq_base(serq_base);
    rsp->set_rx_serq_num_entries(serq_size);

    txsessprodcb_addr = g_nvme_global_info.txsessprodcb_base + g_sess_id * sizeof(nvme_txsessprodcb_t);
    rsp->set_txsessprodcb_addr(txsessprodcb_addr);

    // Get Tx Sess XTSQ base address
    tx_sess_xtsq_base = g_nvme_global_info.tx_sess_xtsq_base + g_sess_id * NVME_TX_SESS_XTSQ_SIZE;
    rsp->set_tx_xtsq_base(tx_sess_xtsq_base);
    rsp->set_tx_xtsq_num_entries(NVME_TX_SESS_XTSQ_DEPTH);

    // Get Tx Sess DGSTQ base address
    tx_sess_dgstq_base = g_nvme_global_info.tx_sess_dgstq_base + g_sess_id * NVME_TX_SESS_DGSTQ_SIZE;
    SDK_ASSERT((NVME_TX_SESS_DGSTQ_DEPTH & (NVME_TX_SESS_DGSTQ_DEPTH - 1)) == 0);
    rsp->set_tx_dgstq_base(tx_sess_dgstq_base);
    rsp->set_tx_dgstq_num_entries(NVME_TX_SESS_DGSTQ_DEPTH);

    rxsessprodcb_addr = g_nvme_global_info.rxsessprodcb_base + g_sess_id * sizeof(nvme_rxsessprodcb_t);
    rsp->set_rxsessprodcb_addr(rxsessprodcb_addr);

    // Get Rx Sess XTSQ base address
    rx_sess_xtsq_base = g_nvme_global_info.rx_sess_xtsq_base + g_sess_id * NVME_RX_SESS_XTSQ_SIZE;
    SDK_ASSERT((NVME_RX_SESS_XTSQ_DEPTH & (NVME_RX_SESS_XTSQ_DEPTH - 1)) == 0);
    rsp->set_rx_xtsq_base(rx_sess_xtsq_base);
    rsp->set_rx_xtsq_num_entries(NVME_RX_SESS_XTSQ_DEPTH);

    // Get Rx Sess DGSTQ base address
    rx_sess_dgstq_base = g_nvme_global_info.rx_sess_dgstq_base + g_sess_id * NVME_RX_SESS_DGSTQ_SIZE;
    SDK_ASSERT((NVME_RX_SESS_DGSTQ_DEPTH & (NVME_RX_SESS_DGSTQ_DEPTH - 1)) == 0);
    rsp->set_rx_dgstq_base(rx_sess_dgstq_base);
    rsp->set_rx_dgstq_num_entries(NVME_RX_SESS_DGSTQ_DEPTH);

    rsp->set_api_status(types::API_STATUS_OK);
    HAL_TRACE_DEBUG("----------------------- API End ------------------------");

    return (HAL_RET_OK);
}


}    // namespace hal
