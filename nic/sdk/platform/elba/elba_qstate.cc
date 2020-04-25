// {C} Copyright 2020 Pensando Systems Inc. All rights reserved

#include "platform/elba/elba_state.hpp"
#include "platform/elba/elba_qstate.hpp"
#include "asic/cmn/asic_qstate.hpp"
#include "asic/rw/asicrw.hpp"
#include "third-party/asic/elba/model/elb_top/elb_top_csr.h"

using namespace sdk::platform::utils;
using namespace sdk::asic;

namespace sdk {
namespace platform {
namespace elba {

#define ELBA_SET_QSTATE_MAP_ENTRY(QID)                                    \
    entry->length ## QID(qstate->type[QID].qtype_info.entries);           \
    entry->size ## QID(qstate->type[QID].qtype_info.size);

#define ELBA_GET_QSTATE_MAP_ENTRY(QID)                                     \
    qstate->type[QID].qtype_info.entries = (uint8_t)entry->length ## QID();\
    qstate->type[QID].qtype_info.size = (uint8_t)entry->size ## QID();

sdk_ret_t
elba_clear_qstate_mem (uint64_t base_addr, uint32_t size)
{
    sdk_ret_t ret = SDK_RET_OK;
    // qstate is a multiple for 4K So it is safe to assume
    // 256 byte boundary.
    static uint8_t zeros[256] = {0};

    for (uint32_t i = 0; i < (size / sizeof(zeros)); i++) {
        ret = sdk::asic::asic_mem_write(base_addr + (i * sizeof(zeros)),
                                        zeros, sizeof(zeros));
        if (ret != SDK_RET_OK) {
            return ret;
        }
    }

    return ret;
}

sdk_ret_t
elba_clear_qstate (lif_qstate_t *qstate)
{
    return elba_clear_qstate_mem(qstate->hbm_address, qstate->allocation_size);
}


template <typename T>
void
set_qstate_entry (LIFQState *qstate, T *entry, int cos)
{
    entry->qstate_base(qstate->hbm_address >> 12);
    entry->length0(qstate->params_in.type[0].entries);
    entry->size0(qstate->params_in.type[0].size);
    entry->length1(qstate->params_in.type[1].entries);
    entry->size1(qstate->params_in.type[1].size);
    entry->length2(qstate->params_in.type[2].entries);
    entry->size2(qstate->params_in.type[2].size);
    entry->length3(qstate->params_in.type[3].entries);
    entry->size3(qstate->params_in.type[3].size);
    entry->length4(qstate->params_in.type[4].entries);
    entry->size4(qstate->params_in.type[4].size);
    entry->length5(qstate->params_in.type[5].entries);
    entry->size5(qstate->params_in.type[5].size);
    entry->length6(qstate->params_in.type[6].entries);
    entry->size6(qstate->params_in.type[6].size);
    entry->length7(qstate->params_in.type[7].entries);
    entry->size7(qstate->params_in.type[7].size);
    entry->vld(1);
    entry->write();
}

template <typename T>
void
clear_qstate_entry (T *entry)
{
    entry->vld(1);
    entry->write();
}

template <typename T>
void
reset_qstate_entry (T *entry)
{
    entry->vld(0);
    entry->write();
}

void
elba_clear_qstate_map (uint32_t lif_id)
{
    elb_top_csr_t & elb0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);

    auto *wa_entry = &elb0.db.wa.dhs_lif_qstate_map.entry[lif_id];
    clear_qstate_entry(wa_entry);
    auto *psp_entry = &elb0.pt.pt.psp.dhs_lif_qstate_map.entry[lif_id];
    clear_qstate_entry(psp_entry);
    auto *xd_psp_entry = &elb0.xd.pt.psp.dhs_lif_qstate_map.entry[lif_id];
    clear_qstate_entry(xd_psp_entry);
    auto *pr_entry = &elb0.pr.pr.psp.dhs_lif_qstate_map.entry[lif_id];
    clear_qstate_entry(pr_entry);
}

void
elba_reset_qstate_map (uint32_t lif_id)
{
    elb_top_csr_t & elb0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);

    auto *wa_entry = &elb0.db.wa.dhs_lif_qstate_map.entry[lif_id];
    reset_qstate_entry(wa_entry);
    auto *psp_entry = &elb0.pt.pt.psp.dhs_lif_qstate_map.entry[lif_id];
    reset_qstate_entry(psp_entry);
    auto *xd_psp_entry = &elb0.xd.pt.psp.dhs_lif_qstate_map.entry[lif_id];
    reset_qstate_entry(xd_psp_entry);
    auto *pr_entry = &elb0.pr.pr.psp.dhs_lif_qstate_map.entry[lif_id];
    reset_qstate_entry(pr_entry);
}

template <typename T>
void
elba_set_qstate_map (lif_qstate_t *qstate, T *entry, uint8_t enable)
{
    entry->qstate_base(qstate->hbm_address >> 12);
    ELBA_SET_QSTATE_MAP_ENTRY(0);
    ELBA_SET_QSTATE_MAP_ENTRY(1);
    ELBA_SET_QSTATE_MAP_ENTRY(2);
    ELBA_SET_QSTATE_MAP_ENTRY(3);
    ELBA_SET_QSTATE_MAP_ENTRY(4);
    ELBA_SET_QSTATE_MAP_ENTRY(5);
    ELBA_SET_QSTATE_MAP_ENTRY(6);
    ELBA_SET_QSTATE_MAP_ENTRY(7);
    entry->vld(enable);
    entry->write();
}

void
elba_program_qstate_map (lif_qstate_t *qstate, uint8_t enable)
{
    elb_top_csr_t & elb0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);

    auto *wa_entry = &elb0.db.wa.dhs_lif_qstate_map.entry[qstate->lif_id];
    elba_set_qstate_map(qstate, wa_entry, enable);
    auto *psp_entry = &elb0.pt.pt.psp.dhs_lif_qstate_map.entry[qstate->lif_id];
    elba_set_qstate_map(qstate, psp_entry, enable);
    auto *xd_psp_entry = &elb0.xd.pt.psp.dhs_lif_qstate_map.entry[qstate->lif_id];
    elba_set_qstate_map(qstate, xd_psp_entry, enable);
    auto *pr_entry = &elb0.pr.pr.psp.dhs_lif_qstate_map.entry[qstate->lif_id];
    elba_set_qstate_map(qstate, pr_entry, enable);
}


template <typename T>
void
elba_get_qstate_map (lif_qstate_t *qstate, T *entry)
{
    entry->read();
    qstate->enable = (uint8_t)entry->vld();
    qstate->hbm_address = (uint64_t) (entry->qstate_base() << 12);
    //qstate->hint_cos = (uint8_t) entry->sched_hint_cos();
    ELBA_GET_QSTATE_MAP_ENTRY(0);
    ELBA_GET_QSTATE_MAP_ENTRY(1);
    ELBA_GET_QSTATE_MAP_ENTRY(2);
    ELBA_GET_QSTATE_MAP_ENTRY(3);
    ELBA_GET_QSTATE_MAP_ENTRY(4);
    ELBA_GET_QSTATE_MAP_ENTRY(5);
    ELBA_GET_QSTATE_MAP_ENTRY(6);
    ELBA_GET_QSTATE_MAP_ENTRY(7);
}

void
elba_read_qstate_map (lif_qstate_t *qstate)
{
    elb_top_csr_t & elb0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);

    auto *psp_entry = &elb0.pt.pt.psp.dhs_lif_qstate_map.entry[qstate->lif_id];
    elba_get_qstate_map(qstate, psp_entry);
}

void
elba_reprogram_qstate_map (uint32_t lif_id, uint8_t enable)
{
    lif_qstate_t qstate;
    memset(&qstate, 0, sizeof(lif_qstate_t));
    qstate.lif_id = lif_id;
    elba_read_qstate_map(&qstate);
    elba_program_qstate_map(&qstate, enable);
}

// In case we just need hbm base and nothing else, we can use this
// Below 2 APIs are for retrieving lif base addr and other params upon upgrade
// we want to recover state by reading ASIC
template <typename T>
void
get_qstate_entry_base_address (T *entry, uint64_t *lif_base_addr)
{
    entry->read();
    if (entry->vld() == 1) {
        *lif_base_addr = (uint64_t) (entry->qstate_base() << 12);
    } else {
        *lif_base_addr = 0;
    }
}

template <typename T>
void
get_qstate_lif_params (LIFQState *qstate, T *entry, uint32_t *is_valid)
{
    entry->read();
    *is_valid = (uint32_t)entry->vld();
    qstate->hbm_address = (uint64_t) (entry->qstate_base() << 12);
    qstate->params_in.type[0].entries = (uint8_t) entry->length0();
    qstate->params_in.type[0].size = (uint8_t) entry->size0();
    qstate->params_in.type[1].entries = (uint8_t) entry->length1();
    qstate->params_in.type[1].size = (uint8_t) entry->size1();
    qstate->params_in.type[2].entries = (uint8_t) entry->length2();
    qstate->params_in.type[2].size = (uint8_t) entry->size2();
    qstate->params_in.type[3].entries = (uint8_t) entry->length3();
    qstate->params_in.type[3].size = (uint8_t) entry->size3();
    qstate->params_in.type[4].entries = (uint8_t) entry->length4();
    qstate->params_in.type[4].size = (uint8_t) entry->size4();
    qstate->params_in.type[5].entries = (uint8_t) entry->length5();
    qstate->params_in.type[5].size = (uint8_t) entry->size5();
    qstate->params_in.type[6].entries = (uint8_t) entry->length6();
    qstate->params_in.type[6].size = (uint8_t) entry->size6();
    qstate->params_in.type[7].entries = (uint8_t) entry->length7();
    qstate->params_in.type[7].size = (uint8_t) entry->size7();
}

void
push_qstate_to_elba (LIFQState *qstate, int cos)
{
    elb_top_csr_t & elb0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);

    if (!qstate->params_in.dont_zero_memory) {
        clear_qstate_mem(qstate->hbm_address, qstate->allocation_size);
    }
    auto *wa_entry = &elb0.db.wa.dhs_lif_qstate_map.entry[qstate->lif_id];
    set_qstate_entry(qstate, wa_entry, cos);
    auto *psp_entry = &elb0.pt.pt.psp.dhs_lif_qstate_map.entry[qstate->lif_id];
    set_qstate_entry(qstate, psp_entry, cos);
    auto *xd_psp_entry =
        &elb0.xd.pt.psp.dhs_lif_qstate_map.entry[qstate->lif_id];
    set_qstate_entry(qstate, xd_psp_entry, cos);
    auto *pr_entry = &elb0.pr.pr.psp.dhs_lif_qstate_map.entry[qstate->lif_id];
    set_qstate_entry(qstate, pr_entry, cos);
}

void
clear_qstate (LIFQState *qstate)
{
    elb_top_csr_t & elb0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);

    auto *wa_entry = &elb0.db.wa.dhs_lif_qstate_map.entry[qstate->lif_id];
    clear_qstate_entry(wa_entry);
    auto *psp_entry = &elb0.pt.pt.psp.dhs_lif_qstate_map.entry[qstate->lif_id];
    clear_qstate_entry(psp_entry);
    auto *xd_psp_entry =
        &elb0.xd.pt.psp.dhs_lif_qstate_map.entry[qstate->lif_id];
    clear_qstate_entry(xd_psp_entry);
    auto *pr_entry = &elb0.pr.pr.psp.dhs_lif_qstate_map.entry[qstate->lif_id];
    clear_qstate_entry(pr_entry);
}

void
read_lif_params_from_elba (LIFQState *qstate)
{
    elb_top_csr_t & elb0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    uint32_t is_valid = 0;
    auto *psp_entry = &elb0.pt.pt.psp.dhs_lif_qstate_map.entry[qstate->lif_id];

    // Since content is going to be same in ASIC across all
    // 3 blocks - reading from one is enough ??
    get_qstate_lif_params(qstate, psp_entry, &is_valid);
}

sdk_ret_t
elba_read_qstate (uint64_t q_addr, uint8_t *buf, uint32_t q_size)
{
    return sdk::asic::asic_mem_read(q_addr, buf, q_size);
    // TBD-ELBA-REBASE: hal to sdk Missing Function compared to Capri
    //return SDK_RET_OK;
}

sdk_ret_t
elba_write_qstate (uint64_t q_addr, const uint8_t *buf, uint32_t q_size)
{
    return sdk::asic::asic_mem_write(q_addr, (uint8_t *)buf, q_size);
    // TBD-ELBA-REBASE: hal to sdk Missing Function compared to Capri
    //return SDK_RET_OK;
}

sdk_ret_t
elba_clear_qstate (LIFQState *qstate)
{
    elb_top_csr_t & elb0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);

    auto *wa_entry = &elb0.db.wa.dhs_lif_qstate_map.entry[qstate->lif_id];
    clear_qstate_entry(wa_entry);
    auto *psp_entry = &elb0.pt.pt.psp.dhs_lif_qstate_map.entry[qstate->lif_id];
    clear_qstate_entry(psp_entry);
    auto *xd_psp_entry =
        &elb0.xd.pt.psp.dhs_lif_qstate_map.entry[qstate->lif_id];
    clear_qstate_entry(xd_psp_entry);
    auto *pr_entry = &elb0.pr.pr.psp.dhs_lif_qstate_map.entry[qstate->lif_id];

    clear_qstate_entry(pr_entry);

    return SDK_RET_OK;
}

}    // namespace elba
}    // namespace platform
}    // namespace sdk
