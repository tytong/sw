// {C} Copyright 2017 Pensando Systems Inc. All rights reserved
#ifndef __ACL_HPP__
#define __ACL_HPP__

#include "nic/include/base.h"
#include "nic/include/hal_state.hpp"
#include "sdk/ht.hpp"
#include "nic/include/ip.h"
#include "nic/gen/proto/hal/acl.pb.h"
#include "nic/gen/proto/hal/kh.pb.h"
#include "nic/include/pd.hpp"

// Include key of internal fields for use only with DOL/testing infra
// For production builds this needs to be removed
// TODO: REMOVE
#define ACL_DOL_TEST_ONLY

using sdk::lib::ht_ctxt_t;
using kh::AclKeyHandle;

using acl::AclSpec;
using acl::AclStatus;
using acl::AclResponse;
using acl::AclRequestMsg;
using acl::AclResponseMsg;
using acl::AclDeleteRequest;
using acl::AclDeleteResponse;
using acl::AclDeleteRequestMsg;
using acl::AclDeleteResponseMsg;
using acl::AclGetRequest;
using acl::AclGetRequestMsg;
using acl::AclGetResponse;
using acl::AclGetResponseMsg;

namespace hal {

#define HAL_MAX_ACLS 512 

typedef struct acl_eth_match_spec_s {
    uint16_t    ether_type;
    mac_addr_t  mac_sa;
    mac_addr_t  mac_da;
} __PACK__ acl_eth_match_spec_t;

typedef struct acl_icmp_match_spec_s {
    uint8_t        icmp_type;    // ICMP type
    uint8_t        icmp_code;    // ICMP code
} __PACK__ acl_icmp_match_spec_t;

typedef struct acl_udp_match_spec_s {
    uint16_t sport; // Src port
    uint16_t dport; // Dest port
} __PACK__ acl_udp_match_spec_t;

typedef struct acl_tcp_match_spec_s {
    uint16_t sport;        // Src port
    uint16_t dport;        // Dest port
    uint8_t  tcp_flags;    // TCP flags
} __PACK__ acl_tcp_match_spec_t;

typedef struct acl_ip_match_spec_s {
    ip_addr_t sip;         // Source IP address
    ip_addr_t dip;         // Dest IP address
    uint8_t   ip_proto;    // IP Protocol
    union {
        acl_icmp_match_spec_t   icmp;
        acl_udp_match_spec_t    udp;
        acl_tcp_match_spec_t    tcp;
    } __PACK__ u;
} __PACK__ acl_ip_match_spec_t;

typedef enum {
    ACL_TYPE_NONE = 0,
    ACL_TYPE_ETH,       // Eth type
    ACL_TYPE_IP,        // IP type - match on both v4/v6
    ACL_TYPE_IPv4,      // IPv4 type - match on v4 only
    ACL_TYPE_IPv6,      // IPv6 type - match on v6 only
    ACL_TYPE_INVALID
} acl_type_e;

#ifdef ACL_DOL_TEST_ONLY
// Key of internal fields for use only with DOL/testing infra
// For production builds this needs to be removed
// TODO: REMOVE
typedef struct acl_internal_match_spec_s {
    bool       direction;
    bool       flow_miss;
    bool       ip_options;
    bool       ip_frag;
    bool       tunnel_terminate;
    uint64_t   drop_reason;
    mac_addr_t outer_mac_da;
} __PACK__ acl_internal_match_spec_t;
#endif

// Specifications for the Acl
typedef struct acl_match_spec_s {
    bool            vrf_match;
    hal_handle_t    vrf_handle;     // vrf handle
    bool            src_if_match;
    hal_handle_t    src_if_handle;     // source if handle
    bool            dest_if_match;
    hal_handle_t    dest_if_handle;    // dest if handle
    bool            l2seg_match;
    hal_handle_t    l2seg_handle;      // l2 segment handle
    acl_type_e      acl_type;
    union {
        acl_eth_match_spec_t eth;
        acl_ip_match_spec_t  ip;
    } __PACK__ key;
    union {
        acl_eth_match_spec_t eth;
        acl_ip_match_spec_t  ip;
    } __PACK__ mask;

#ifdef ACL_DOL_TEST_ONLY
    // Key of internal fields for use only with DOL/testing infra
    // For production builds this needs to be removed
    // TODO: REMOVE
    acl_internal_match_spec_t int_key;
    acl_internal_match_spec_t int_mask;
#endif

} __PACK__ acl_match_spec_t;

#ifdef ACL_DOL_TEST_ONLY
    // Internal fields for use only with DOL/testing infra
    // For production builds this needs to be removed
    // TODO: REMOVE
typedef struct acl_internal_action_spec_s {
    bool     mac_sa_rewrite;
    bool     mac_da_rewrite;
    bool     ttl_dec;
    uint32_t rw_idx;              // rewrite index
    uint32_t tnnl_vnid;           // tunnel vnid / encap vlan
} __PACK__ acl_internal_action_spec_t;
#endif

// Action specifications for the Acl
typedef struct acl_action_spec_s {
    acl::AclAction action;
    bool           ing_mirror_en;
    hal_handle_t   ing_mirror_session_handle;
    bool           egr_mirror_en;
    hal_handle_t   egr_mirror_session_handle;
    uint8_t        egr_mirror_session;   // Mirror sessions in egress direction
    uint8_t        ing_mirror_session;   // Mirror sessions in ingress direction
    hal_handle_t   copp_policer_handle;
    hal_handle_t   redirect_if_handle;
#ifdef ACL_DOL_TEST_ONLY
    // Internal fields for use only with DOL/testing infra
    // For production builds this needs to be removed
    // TODO: REMOVE
    acl_internal_action_spec_t int_as;
#endif

} __PACK__ acl_action_spec_t;

typedef struct acl_key_s {
    acl_id_t acl_id;    // Acl id assigned
} __PACK__ acl_key_t;

inline std::ostream& operator<<(std::ostream& os, const acl_key_t& s)
{
   return os << fmt::format("{{acl_id={}}}", s.acl_id);
}

// Acl structure
typedef struct acl_s {
    hal_spinlock_t    slock;         // lock to protect this structure
    acl_key_t         key;           // acl key information

    uint32_t          priority;
    acl_match_spec_t  match_spec;
    acl_action_spec_t action_spec;

    hal_handle_t      hal_handle;    // HAL allocated handle

    pd::pd_acl_t      *pd;
} __PACK__ acl_t;

// CB data structures
typedef struct acl_create_app_ctxt_s {
} __PACK__ acl_create_app_ctxt_t;

typedef struct acl_update_app_ctxt_s {
} __PACK__ acl_update_app_ctxt_t;

// allocate a Acl instance
static inline acl_t *
acl_alloc (void)
{
    acl_t    *acl;

    acl = (acl_t *)g_hal_state->acl_slab()->alloc();
    if (acl == NULL) {
        return NULL;
    }
    return acl;
}

// initialize a Acl instance
static inline acl_t *
acl_init (acl_t *acl)
{
    if (!acl) {
        return NULL;
    }
    HAL_SPINLOCK_INIT(&acl->slock, PTHREAD_PROCESS_PRIVATE);

    return acl;
}

// allocate and initialize a acl instance
static inline acl_t *
acl_alloc_init (void)
{
    return acl_init(acl_alloc());
}

static inline acl_t *
find_acl_by_id (acl_id_t acl_id)
{
    hal_handle_id_ht_entry_t *entry;
    acl_key_t                acl_key;
    acl_t                    *acl;

    acl_key.acl_id= acl_id;

    entry = (hal_handle_id_ht_entry_t *)g_hal_state->acl_ht()->lookup(&acl_key);
    if (entry && (entry->handle_id != HAL_HANDLE_INVALID)) {
        // check for object type
        HAL_ASSERT(hal_handle_get_from_handle_id(entry->handle_id)->obj_id() == 
                   HAL_OBJ_ID_ACL);
        acl = (acl_t *)hal_handle_get_obj(entry->handle_id);
        return acl;
    }
    return NULL;
}

static inline acl_t *
find_acl_by_handle (hal_handle_t handle)
{
    if (handle == HAL_HANDLE_INVALID) {
        return NULL;
    }
    auto hal_handle = hal_handle_get_from_handle_id(handle);
    if (!hal_handle) {
        HAL_TRACE_ERR("{}:failed to find object with handle:{}",
                        __FUNCTION__, handle);
        return NULL;
    }
    if (hal_handle->obj_id() != HAL_OBJ_ID_ACL) {
        HAL_TRACE_ERR("{}:failed to find acl with handle:{}",
                        __FUNCTION__, handle);
        return NULL;
    }
    return (acl_t *)hal_handle_get_obj(handle);
}

static inline acl_t *
acl_lookup_by_key_or_handle (const AclKeyHandle& kh)
{
    if (kh.key_or_handle_case() == AclKeyHandle::kAclId) {
        return find_acl_by_id(kh.acl_id());
    } else if (kh.key_or_handle_case() == AclKeyHandle::kAclHandle) {
        return find_acl_by_handle(kh.acl_handle());
    }
    return NULL;
}

extern void *acl_get_key_func(void *entry);
extern uint32_t acl_compute_hash_func(void *key, uint32_t ht_size);
extern bool acl_compare_key_func(void *key1, void *key2);

hal_ret_t acl_create(acl::AclSpec& spec,
                     acl::AclResponse *rsp);
hal_ret_t acl_update(acl::AclSpec& spec,
                     acl::AclResponse *rsp);
hal_ret_t acl_delete(acl::AclDeleteRequest& req,
                     acl::AclDeleteResponse *rsp);
hal_ret_t acl_get(acl::AclGetRequest& req,
                  acl::AclGetResponse *rsp);


}    // namespace hal

#endif    // __ACL_HPP__
