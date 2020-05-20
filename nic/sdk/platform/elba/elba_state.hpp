// {C} Copyright 2018 Pensando Systems Inc. All rights reserved

#ifndef __ELBA_STATE_HPP__
#define __ELBA_STATE_HPP__

#include "platform/utils/mpartition.hpp"
#include "lib/bm_allocator/bm_allocator.hpp"
#include "asic/asic.hpp"
#include "third-party/asic/elba/model/elb_top/elb_top_csr.h"

namespace sdk {
namespace platform {
namespace elba {

class elba_state_pd {
public:
    static elba_state_pd *factory(asic_cfg_t *cfg);
    ~elba_state_pd();

    // get APIs for TXS scheduler related state
    sdk::lib::BMAllocator *txs_scheduler_map_idxr(void)
    { return txs_scheduler_map_idxr_; }
    sdk::lib::BMAllocator *txs_scheduler_qgrp_idxr(void) 
    { return txs_scheduler_qgrp_idxr_; }

    std::string cfg_path(void) const { return cfg_.cfg_path; }
    mpartition *mempartition(void) const { return cfg_.mempartition; }
    elb_top_csr_t& elb_top(void) { return *elb_top_; }
    asic_cfg_t *cfg(void) { return &cfg_; }

private:
    // TXS scheduler related state
    struct {
        sdk::lib::BMAllocator    *txs_scheduler_map_idxr_;
        sdk::lib::BMAllocator    *txs_scheduler_qgrp_idxr_;
    } __PACK__;
    asic_cfg_t cfg_; // config
    elb_top_csr_t *elb_top_;

private:
    elba_state_pd();
    bool init(asic_cfg_t *cfg);
};

extern class elba_state_pd *g_elba_state_pd;

extern sdk_ret_t elba_state_pd_init(asic_cfg_t *cfg);

}    // namespace elba
}    // namespace platform
}    // namespace sdk

#endif    // __ELBA_STATE_HPP__

