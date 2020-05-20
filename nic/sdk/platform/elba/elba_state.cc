// {C} Copyright 2020 Pensando Systems Inc. All rights reserved

#include "include/sdk/base.hpp"
#include "platform/elba/csrint/csr_init.hpp"
#include "asic/asic.hpp"
#include "platform/elba/elba_state.hpp"
#include "platform/elba/elba_txs_scheduler.hpp"
#include "third-party/asic/elba/model/elb_top/elb_top_csr.h"

namespace sdk {
namespace platform {
namespace elba {

//------------------------------------------------------------------------------
// (private) constructor method
//------------------------------------------------------------------------------
elba_state_pd::elba_state_pd (void)
{
    txs_scheduler_map_idxr_ = NULL;
    txs_scheduler_qgrp_idxr_ = NULL;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
elba_state_pd::~elba_state_pd (void)
{
    txs_scheduler_map_idxr_ ? delete txs_scheduler_map_idxr_ : (void)0;
    txs_scheduler_qgrp_idxr_ ? delete txs_scheduler_qgrp_idxr_ : (void)0;
}

//------------------------------------------------------------------------------
// init() function to instantiate the state
//------------------------------------------------------------------------------
bool
elba_state_pd::init (asic_cfg_t *cfg)
{
    if (cfg) {
        // BMAllocator based bmp range allocator to manage txs scheduler mapping
        txs_scheduler_map_idxr_ =
                        new sdk::lib::BMAllocator(ELBA_TXS_SCHEDULER_MAP_MAX_ENTRIES);
        SDK_ASSERT_RETURN((txs_scheduler_map_idxr_ != NULL), false);
        txs_scheduler_qgrp_idxr_ = 
                        new sdk::lib::BMAllocator(ELBA_TXS_MAX_QGRP_TABLE_ENTRIES);
        SDK_ASSERT_RETURN((txs_scheduler_qgrp_idxr_ != NULL), false);
        cfg_ = *cfg;
    }

    elb_top_ = &ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);

    return true;
}

//------------------------------------------------------------------------------
// factory method
//------------------------------------------------------------------------------
elba_state_pd *
elba_state_pd::factory (asic_cfg_t *cfg)
{
    elba_state_pd *state;

    state = new elba_state_pd();
    SDK_ASSERT_RETURN((state != NULL), NULL);
    if (state->init(cfg) == false) {
        delete state;
        return NULL;
    }
    return state;
}

sdk_ret_t
elba_state_pd_init (asic_cfg_t *cfg)
{
    if (g_elba_state_pd) {
        return SDK_RET_OK;
    }
    csr_init();

    g_elba_state_pd = elba_state_pd::factory(cfg);
    SDK_ASSERT_TRACE_RETURN((g_elba_state_pd != NULL), SDK_RET_INVALID_ARG,
                            "Failed to instantiate Capri PD");
    return SDK_RET_OK;
}

}    // namespace elba
}    // namespace platform
}    // namespace sdk
