// {C} Copyright 2017 Pensando Systems Inc. All rights reserved

#ifndef __DOS_HPP__
#define __DOS_HPP__

#include "nic/include/base.h"
#include "nic/include/hal_state.hpp"
#include "sdk/ht.hpp"
#include "nic/gen/proto/hal/nwsec.pb.h"
#include "nic/include/pd.hpp"

#define HAL_NWSEC_INVALID_SG_ID        uint32_t (~0)
using sdk::lib::ht_ctxt_t;

using nwsec::DoSPolicySpec;
using nwsec::DoSPolicyStatus;
using nwsec::DoSPolicyRequestMsg;
using nwsec::DoSPolicyResponse;
using nwsec::DoSPolicyResponseMsg;
using nwsec::DoSPolicyDeleteRequest;
using nwsec::DoSPolicyDeleteRequestMsg;
using nwsec::DoSPolicyDeleteResponse;
using nwsec::DoSPolicyDeleteResponseMsg;
using nwsec::DoSPolicyGetRequest;
using nwsec::DoSPolicyStats;
using nwsec::DoSPolicyGetResponse;
using nwsec::DoSPolicyGetRequestMsg;
using nwsec::DoSPolicyGetResponseMsg;
using nwsec::IngressDoSPolicy;
using nwsec::EgressDoSPolicy;
using nwsec::DoSProtectionSpec;
using nwsec::DoSService;

namespace hal {

typedef struct dos_service_s {
    uint8_t         ip_proto;       // IP protocol
    bool            is_icmp;        // Is ICMP msg type ?
    union {
        uint16_t    dport;          // TCP or UDP port, 0 for other protos
        struct {
            uint8_t    icmp_msg_type;   // ICMP msg type
            uint8_t    icmp_msg_code;   // ICMP code
        } __PACK__;
    };
} __PACK__ dos_service_t;

typedef struct dos_session_limits_s {
    uint32_t    max_sessions;       // max. no of sessions
    uint32_t    blocking_timeout;   // cool off period once session count
                                    // comes below the above limit
} __PACK__ dos_session_limits_t;

// DoS aggregate policer
typedef struct dos_policer_s {
    uint32_t    bytes_per_sec;      // max. bytes per sec
    uint32_t    peak_rate;          // bytes-per-sec
    uint32_t    burst_size;         // bytes
} __PACK__ dos_policer_t;

typedef struct dos_flood_limits_s {
    uint32_t restrict_pps;          // restrict pps limit
    uint32_t restrict_burst_pps;    // restrict pps burst
    uint32_t restrict_duration;     // restrict action duration (secs)

    uint32_t protect_pps;           // protect pps limit
    uint32_t protect_burst_pps;     // protect pps burst
    uint32_t protect_duration;      // protect action duration (secs)
} __PACK__ dos_flood_limits_t;

typedef struct dos_policy_prop_s {
    dos_service_t         service;                // service attachment point
    dos_policer_t         policer;                // DoS agg. policer config
    dos_flood_limits_t    tcp_syn_flood_limits;   // TCP syn flood limits
    dos_flood_limits_t    udp_flood_limits;       // UDP flood limits
    dos_flood_limits_t    icmp_flood_limits;      // ICMP flood limits
    dos_flood_limits_t    other_flood_limits;     // Other flood limits
    dos_session_limits_t  session_limits;         // Session limits
    uint32_t              session_setup_rate;     // 0 means no limits
    uint32_t              peer_sg_id;             // Peer security group
} __PACK__ dos_policy_prop_t;

typedef struct dos_policy_s {
    hal_spinlock_t        slock;                  // lock to protect this structure
    hal_handle_t          hal_handle;             // HAL allocated handle
    hal_handle_t          vrf_handle;          // vrf handle 
    
    bool                  ingr_pol_valid;
    bool                  egr_pol_valid;
    
    dos_policy_prop_t     ingress;
    dos_policy_prop_t     egress;

    // PD state
    void                  *pd;                    // all PD specific state
    // Security group list
    dllist_ctxt_t         sg_list_head;           // List of security groups
} __PACK__ dos_policy_t;

typedef struct dos_policy_sg_list_entry_s {
    int              sg_id;
    dllist_ctxt_t    dllist_ctxt;
} __PACK__ dos_policy_sg_list_entry_t;

typedef struct dos_policy_create_app_ctx_s {
} __PACK__ dos_policy_create_app_ctx_t;

typedef struct dos_policy_update_app_ctx_s {
} __PACK__ dos_policy_update_app_ctx_t;

// max. number of dos policies supported
#define HAL_MAX_DOS_POLICIES                       256

// allocate a dos policy instance
static inline dos_policy_t *
dos_policy_alloc (void)
{
    dos_policy_t    *dos_policy;

    dos_policy = (dos_policy_t *)g_hal_state->dos_policy_slab()->alloc();
    if (dos_policy == NULL) {
        return NULL;
    }
    return dos_policy;
}

// initialize a dos policy instance
static inline dos_policy_t *
dos_policy_init (dos_policy_t *dos_policy)
{
    if (!dos_policy) {
        return NULL;
    }
    HAL_SPINLOCK_INIT(&dos_policy->slock, PTHREAD_PROCESS_PRIVATE);

    // initialize the operational state

    // initialize meta information
    // dos_policy->ht_ctxt.reset();
    // dos_policy->hal_handle_ht_ctxt.reset();

    return dos_policy;
}

// allocate and initialize a dos policy instance
static inline dos_policy_t *
dos_policy_alloc_init (void)
{
    return dos_policy_init(dos_policy_alloc());
}

// free dos policy instance
static inline hal_ret_t
dos_policy_free (dos_policy_t *dos_policy)
{
    HAL_SPINLOCK_DESTROY(&dos_policy->slock);
    g_hal_state->dos_policy_slab()->free(dos_policy);
    return HAL_RET_OK;
}

// find a dos policy instance by its handle
static inline dos_policy_t *
find_dos_policy_by_handle (hal_handle_t handle)
{
    if (handle == HAL_HANDLE_INVALID) {
        return NULL;
    }
    // check for object type
    HAL_ASSERT(hal_handle_get_from_handle_id(handle)->obj_id() == 
               HAL_OBJ_ID_DOS_POLICY);
    return (dos_policy_t *)hal_handle_get_obj(handle); 
}

extern uint32_t dos_policy_id_compute_hash_func(void *key, uint32_t ht_size);
extern bool dos_policy_id_compare_key_func(void *key1, void *key2);
dos_policy_t *dos_policy_lookup_handle (const nwsec::DoSPolicySpec& dp);

// extern void *dos_policy_get_handle_key_func(void *entry);
// extern uint32_t dos_policy_compute_handle_hash_func(void *key, uint32_t ht_size);
// extern bool dos_policy_compare_handle_key_func(void *key1, void *key2);

hal_ret_t dos_policy_create(nwsec::DoSPolicySpec& spec,
                            nwsec::DoSPolicyResponse *rsp);

hal_ret_t dos_policy_update(nwsec::DoSPolicySpec& spec,
                            nwsec::DoSPolicyResponse *rsp);

hal_ret_t dos_policy_delete(nwsec::DoSPolicyDeleteRequest& req,
                            nwsec::DoSPolicyDeleteResponse *rsp);

hal_ret_t dos_policy_get(nwsec::DoSPolicyGetRequest& req,
                         nwsec::DoSPolicyGetResponse *rsp);

}    // namespace hal

#endif    // __DOS_HPP__

