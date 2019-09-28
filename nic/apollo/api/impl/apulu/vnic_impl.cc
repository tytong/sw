//
// {C} Copyright 2019 Pensando Systems Inc. All rights reserved
//
//----------------------------------------------------------------------------
///
/// \file
/// datapath implementation of VNIC
///
//----------------------------------------------------------------------------

#include "nic/sdk/lib/p4/p4_api.hpp"
#include "nic/sdk/lib/utils/utils.hpp"
#include "nic/apollo/core/mem.hpp"
#include "nic/apollo/core/trace.hpp"
#include "nic/apollo/framework/api_engine.hpp"
#include "nic/apollo/api/pds_state.hpp"
#include "nic/apollo/api/vpc.hpp"
#include "nic/apollo/api/subnet.hpp"
#include "nic/apollo/api/vnic.hpp"
#include "nic/apollo/api/impl/apulu/apulu_impl.hpp"
#include "nic/apollo/api/impl/apulu/pds_impl_state.hpp"
#include "nic/apollo/api/impl/apulu/route_impl.hpp"
#include "nic/apollo/api/impl/apulu/security_policy_impl.hpp"
#include "nic/apollo/api/impl/apulu/vnic_impl.hpp"
#include "nic/apollo/p4/include/apulu_table_sizes.h"
#include "nic/apollo/p4/include/apulu_defines.h"

#define vnic_tx_stats_action          action_u.vnic_tx_stats_vnic_tx_stats
#define vnic_rx_stats_action          action_u.vnic_rx_stats_vnic_rx_stats

namespace api {
namespace impl {

/// \defgroup PDS_VNIC_IMPL - VNIC entry datapath implementation
/// \ingroup PDS_VNIC
/// @{

vnic_impl *
vnic_impl::factory(pds_vnic_spec_t *spec) {
    vnic_impl *impl;

    // TODO: move to slab later
    impl = (vnic_impl *)SDK_CALLOC(SDK_MEM_ALLOC_PDS_VNIC_IMPL,
                                  sizeof(vnic_impl));
    new (impl) vnic_impl();
    return impl;
}

void
vnic_impl::destroy(vnic_impl *impl) {
    impl->~vnic_impl();
    SDK_FREE(SDK_MEM_ALLOC_PDS_VNIC_IMPL, impl);
}

sdk_ret_t
vnic_impl::reserve_resources(api_base *orig_obj, obj_ctxt_t *obj_ctxt) {
    uint32_t idx;
    sdk_ret_t ret;
    subnet_entry *subnet;
    sdk_table_api_params_t tparams;
    mapping_swkey_t mapping_key = { 0 };
    local_mapping_swkey_t local_mapping_key = { 0 };
    pds_vnic_spec_t *spec = &obj_ctxt->api_params->vnic_spec;

    // allocate hw id for this vnic
    if (vnic_impl_db()->vnic_idxr()->alloc(&idx) != SDK_RET_OK) {
        PDS_TRACE_ERR("Failed to allocate hw id for vnic %u, err %u",
                      spec->key.id, ret);
        return sdk::SDK_RET_NO_RESOURCE;
    }
    hw_id_ = idx;

    // reserve an entry in LOCAL_MAPPING table
    local_mapping_key.key_metadata_local_mapping_lkp_type = KEY_TYPE_MAC;
    local_mapping_key.key_metadata_local_mapping_lkp_id = subnet->hw_id();
    sdk::lib::memrev(local_mapping_key.key_metadata_local_mapping_lkp_addr,
                     spec->mac_addr, ETH_ADDR_LEN);
    PDS_IMPL_FILL_TABLE_API_PARAMS(&tparams, &local_mapping_key, NULL, NULL, 0,
                                   sdk::table::handle_t::null());
    ret = mapping_impl_db()->local_mapping_tbl()->reserve(&tparams);
    if (ret != SDK_RET_OK) {
        PDS_TRACE_ERR("Failed to reserve entry in LOCAL_MAPPING"
                      "table for vnic %u, err %u", spec->key.id, ret);
        goto error;
    }
    local_mapping_hdl_ = tparams.handle;

    // reserve an entry in MAPPING table
    mapping_key.txdma_to_p4e_mapping_lkp_id = subnet->hw_id();
    mapping_key.p4e_i2e_mapping_lkp_type = KEY_TYPE_MAC;
    sdk::lib::memrev(mapping_key.p4e_i2e_mapping_lkp_addr,
                     spec->mac_addr, ETH_ADDR_LEN);
    PDS_IMPL_FILL_TABLE_API_PARAMS(&tparams, &mapping_key, NULL, NULL, 0,
                                   sdk::table::handle_t::null());
    ret = mapping_impl_db()->mapping_tbl()->reserve(&tparams);
    if (ret != SDK_RET_OK) {
        PDS_TRACE_ERR("Failed to reserve entry in MAPPING "
                      "table for vnic %u, err %u", spec->key.id, ret);
        goto error;
    }
    mapping_hdl_ = tparams.handle;
    return SDK_RET_OK;

error:

    PDS_TRACE_ERR("Failed to acquire all h/w resources for vnic %u, err %u",
                  spec->key.id, ret);
    return ret;
}

sdk_ret_t
vnic_impl::release_resources(api_base *api_obj) {
    sdk_table_api_params_t tparams;

    if (hw_id_ != 0xFFFF) {
        vnic_impl_db()->vnic_idxr()->free(hw_id_);
    }

    memset(&tparams, 0, sizeof(tparams));
    if (local_mapping_hdl_.valid()) {
        tparams.handle = local_mapping_hdl_;
        mapping_impl_db()->local_mapping_tbl()->release(&tparams);
    }

    if (mapping_hdl_.valid()) {
        tparams.handle = mapping_hdl_;
        mapping_impl_db()->mapping_tbl()->release(&tparams);
    }
    return SDK_RET_OK;
}

sdk_ret_t
vnic_impl::nuke_resources(api_base *api_obj) {
    sdk_table_api_params_t tparams;

    if (hw_id_ != 0xFFFF) {
        vnic_impl_db()->vnic_idxr()->free(hw_id_);
    }
    memset(&tparams, 0, sizeof(tparams));
    // TODO:
    return SDK_RET_OK;
}

#define rxdma_vnic_info    action_u.vnic_info_rxdma_vnic_info_rxdma
#define txdma_vnic_info    action_u.vnic_info_txdma_vnic_info_txdma
sdk_ret_t
vnic_impl::populate_rxdma_vnic_info_policy_root_(
               vnic_info_rxdma_actiondata_t *tx_vnic_info_data,
               uint32_t idx, mem_addr_t addr) {
    switch (idx) {
    case 0:
        MEM_ADDR_TO_P4_MEM_ADDR(
            tx_vnic_info_data->rxdma_vnic_info.lpm_base2, addr,
            sizeof(tx_vnic_info_data->rxdma_vnic_info.lpm_base2));
        break;
    case 1:
        MEM_ADDR_TO_P4_MEM_ADDR(
            tx_vnic_info_data->rxdma_vnic_info.lpm_base3, addr,
            sizeof(tx_vnic_info_data->rxdma_vnic_info.lpm_base3));
        break;
    case 2:
        MEM_ADDR_TO_P4_MEM_ADDR(
            tx_vnic_info_data->rxdma_vnic_info.lpm_base4, addr,
            sizeof(tx_vnic_info_data->rxdma_vnic_info.lpm_base4));
        break;
    case 3:
        MEM_ADDR_TO_P4_MEM_ADDR(
            tx_vnic_info_data->rxdma_vnic_info.lpm_base5, addr,
            sizeof(tx_vnic_info_data->rxdma_vnic_info.lpm_base5));
        break;
    case 4:
        MEM_ADDR_TO_P4_MEM_ADDR(
            tx_vnic_info_data->rxdma_vnic_info.lpm_base6, addr,
            sizeof(tx_vnic_info_data->rxdma_vnic_info.lpm_base6));
        break;
    case 5:
        MEM_ADDR_TO_P4_MEM_ADDR(
            tx_vnic_info_data->rxdma_vnic_info.lpm_base7, addr,
            sizeof(tx_vnic_info_data->rxdma_vnic_info.lpm_base7));
        break;
    default:
        PDS_TRACE_ERR("Failed to pack %uth entry in VNIC_INFO_RXDMA table",
                      idx);
        return SDK_RET_INVALID_ARG;
    }
    return SDK_RET_OK;
}

//------------------------------------------------------------------------------
// VNIC_INFO table in RxDMA is partitioned into two halves - one is used on Tx
// direction and other half in Rx direction. In each direction, two adjacent
// entries are taken per vnic, 1st one for IPv4 and 2nd one IPv6 so overall
// four entries are consumed for each vnic
//------------------------------------------------------------------------------
sdk_ret_t
vnic_impl::program_vnic_info_(vpc_entry *vpc, subnet_entry *subnet,
                              pds_vnic_spec_t *spec) {
    sdk_ret_t ret;
    mem_addr_t addr;
    uint32_t i, hw_id;
    policy *sec_policy;
    route_table *rtable;
    p4pd_error_t p4pd_ret;
    pds_policy_key_t policy_key;
    pds_route_table_key_t route_table_key;
    vnic_info_rxdma_actiondata_t rxdma_rx_vnic_info_v4_data = { 0 };
    vnic_info_rxdma_actiondata_t rxdma_rx_vnic_info_v6_data = { 0 };
    vnic_info_rxdma_actiondata_t rxdma_tx_vnic_info_v4_data = { 0 };
    vnic_info_rxdma_actiondata_t rxdma_tx_vnic_info_v6_data = { 0 };


    hw_id = hw_id_ << 1;
    rxdma_rx_vnic_info_v4_data.action_id = VNIC_INFO_RXDMA_VNIC_INFO_RXDMA_ID;
    // populate ingress IPv4 policy roots in the Rx direction entry
    for (i = 0; i < spec->num_ing_v4_policy; i++) {
        sec_policy = policy_db()->policy_find(&spec->ing_v4_policy[i]);
        addr = ((impl::security_policy_impl *)(sec_policy->impl()))->security_policy_root_addr();
        PDS_TRACE_DEBUG("IPv4 ing policy root addr 0x%llx", addr);
        populate_rxdma_vnic_info_policy_root_(&rxdma_rx_vnic_info_v4_data,
                                              i, addr);
    }
    // TODO: traverse up the hierarchy !!

    rxdma_rx_vnic_info_v6_data.action_id = VNIC_INFO_RXDMA_VNIC_INFO_RXDMA_ID;
    // populate ingress IPv6 policy roots in the Rx direction entry
    for (i = 0; i < spec->num_ing_v6_policy; i++) {
        sec_policy = policy_db()->policy_find(&spec->ing_v6_policy[i]);
        addr = ((impl::security_policy_impl *)(sec_policy->impl()))->security_policy_root_addr();
        PDS_TRACE_DEBUG("IPv6 ing policy root addr 0x%llx", addr);
        populate_rxdma_vnic_info_policy_root_(&rxdma_rx_vnic_info_v6_data,
                                              i, addr);
    }
    // TODO: traverse up the hierarchy !!

    // program v4 VNIC_INFO_RXDMA entry for Rx direction at index = hw_id
    // NOTE: In the Rx direction, we are not doing route lookups yet, not
    // populating them
    p4pd_ret = p4pd_global_entry_write(P4_APULU_RXDMA_TBL_ID_VNIC_INFO_RXDMA,
                                       hw_id, NULL, NULL,
                                       &rxdma_rx_vnic_info_v4_data);
    if (p4pd_ret != P4PD_SUCCESS) {
        PDS_TRACE_ERR("Failed to program v4 entry in VNIC_INFO_RXDMA table "
                      "at %u", hw_id);
        return sdk::SDK_RET_HW_PROGRAM_ERR;
    }

    // program v6 VNIC_INFO_RXDMA entry for Rx direction at index = hw_id + 1
    p4pd_ret = p4pd_global_entry_write(P4_APULU_RXDMA_TBL_ID_VNIC_INFO_RXDMA,
                                       hw_id + 1, NULL, NULL,
                                       &rxdma_rx_vnic_info_v6_data);
    if (p4pd_ret != P4PD_SUCCESS) {
        PDS_TRACE_ERR("Failed to program v6 entry in VNIC_INFO_RXDMA table "
                      "at %u", hw_id + 1);
        return sdk::SDK_RET_HW_PROGRAM_ERR;
    }

    rxdma_tx_vnic_info_v4_data.action_id = VNIC_INFO_RXDMA_VNIC_INFO_RXDMA_ID;
    // populate IPv4 route table root address in Tx direction entry
    route_table_key = subnet->v4_route_table();
    rtable = route_table_db()->find(&route_table_key);
    if (rtable) {
        addr =
            ((impl::route_table_impl *)(rtable->impl()))->lpm_root_addr();
        PDS_TRACE_DEBUG("IPv4 lpm root addr 0x%llx", addr);
        MEM_ADDR_TO_P4_MEM_ADDR(
            rxdma_tx_vnic_info_v4_data.rxdma_vnic_info.lpm_base1, addr, 5);
    }
    // populate egress IPv4 policy roots in the Tx direction entry
    for (i = 0; i < spec->num_egr_v4_policy; i++) {
        sec_policy = policy_db()->policy_find(&spec->egr_v4_policy[i]);
        addr = ((impl::security_policy_impl *)(sec_policy->impl()))->security_policy_root_addr();
        PDS_TRACE_DEBUG("IPv4 egr policy root addr 0x%llx", addr);
        populate_rxdma_vnic_info_policy_root_(&rxdma_tx_vnic_info_v4_data,
                                              i, addr);
    }
    // TODO: traverse up the hierarchy !!

    rxdma_tx_vnic_info_v6_data.action_id = VNIC_INFO_RXDMA_VNIC_INFO_RXDMA_ID;
    // populate IPv6 route table root address in Tx direction entry
    route_table_key = subnet->v6_route_table();
    if (route_table_key.id != PDS_ROUTE_TABLE_ID_INVALID) {
        rtable = route_table_db()->find(&route_table_key);
    } else {
        rtable = NULL;
    }
    if (rtable) {
        addr = ((impl::route_table_impl *)(rtable->impl()))->lpm_root_addr();
        PDS_TRACE_DEBUG("IPv6 lpm root addr 0x%llx", addr);
        MEM_ADDR_TO_P4_MEM_ADDR(
            rxdma_tx_vnic_info_v6_data.rxdma_vnic_info.lpm_base1, addr, 5);
    }
    // populate egress IPv6 policy roots in the Tx direction entry
    for (i = 0; i < spec->num_egr_v6_policy; i++) {
        sec_policy = policy_db()->policy_find(&spec->egr_v6_policy[i]);
        addr = ((impl::security_policy_impl *)(sec_policy->impl()))->security_policy_root_addr();
        PDS_TRACE_DEBUG("IPv6 egr policy root addr 0x%llx", addr);
        populate_rxdma_vnic_info_policy_root_(&rxdma_tx_vnic_info_v6_data,
                                              i, addr);
    }
    // TODO: traverse up the hierarchy !!

    // program v4 VNIC_INFO_RXDMA entry for Tx direction in 2nd half of the
    // table at VNIC_INFO_TABLE_SIZE + hw_id index
    p4pd_ret = p4pd_global_entry_write(P4_APULU_RXDMA_TBL_ID_VNIC_INFO_RXDMA,
                                       VNIC_INFO_TABLE_SIZE + hw_id,
                                       NULL, NULL, &rxdma_tx_vnic_info_v4_data);
    if (p4pd_ret != P4PD_SUCCESS) {
        PDS_TRACE_ERR("Failed to program VNIC_INFO_RXDMA table at %u", hw_id);
        return sdk::SDK_RET_HW_PROGRAM_ERR;
    }

    // program v6 TXDMA_VNIC_INFO entry for Tx direction in 2nd half of the
    // table at VNIC_INFO_TABLE_SIZE + hw_id + 1 index
    p4pd_ret = p4pd_global_entry_write(P4_APULU_TXDMA_TBL_ID_VNIC_INFO_TXDMA,
                                       VNIC_INFO_TABLE_SIZE + hw_id + 1,
                                       NULL, NULL, &rxdma_tx_vnic_info_v6_data);
    if (p4pd_ret != P4PD_SUCCESS) {
        PDS_TRACE_ERR("Failed to program VNIC_INFO_TXDMA table at %u",
                      VNIC_INFO_TABLE_SIZE + hw_id + 1);
        return sdk::SDK_RET_HW_PROGRAM_ERR;
    }
    return SDK_RET_OK;
}

// TODO: undo stuff if something goes wrong here !!
sdk_ret_t
vnic_impl::program_hw(api_base *api_obj, obj_ctxt_t *obj_ctxt) {
    sdk_ret_t ret;
    vpc_entry *vpc;
    subnet_entry *subnet;
    pds_vpc_key_t vpc_key;
    pds_vnic_spec_t *spec;
    p4pd_error_t p4pd_ret;
    vnic_rx_stats_actiondata_t vnic_rx_stats_data = { 0 };
    vnic_tx_stats_actiondata_t vnic_tx_stats_data = { 0 };

    spec = &obj_ctxt->api_params->vnic_spec;

    // fetch the relevant objects
    subnet = subnet_db()->find(&spec->subnet);
    if (subnet == NULL) {
        PDS_TRACE_ERR("Failed to find subnet %u, vpc %u",
                      spec->subnet.id, spec->vpc.id);
        return sdk::SDK_RET_INVALID_ARG;
    }

    // get the vpc of this subnet
    vpc_key = subnet->vpc();
    vpc = vpc_db()->find(&vpc_key);
    if (unlikely(vpc == NULL)) {
        return sdk::SDK_RET_INVALID_ARG;
    }

    // initialize tx stats table for this vnic
    vnic_tx_stats_data.action_id = VNIC_TX_STATS_VNIC_TX_STATS_ID;
    p4pd_ret = p4pd_global_entry_write(P4TBL_ID_VNIC_TX_STATS,
                                       hw_id_, NULL, NULL,
                                       &vnic_tx_stats_data);
    if (p4pd_ret != P4PD_SUCCESS) {
        PDS_TRACE_ERR("Failed to program vnic %u TX_STATS table entry",
                      spec->key.id);
        return sdk::SDK_RET_HW_PROGRAM_ERR;
    }

    // initialize rx stats tables for this vnic
    vnic_rx_stats_data.action_id = VNIC_RX_STATS_VNIC_RX_STATS_ID;
    p4pd_ret = p4pd_global_entry_write(P4TBL_ID_VNIC_RX_STATS,
                                       hw_id_, NULL, NULL,
                                       &vnic_rx_stats_data);
    if (p4pd_ret != P4PD_SUCCESS) {
        PDS_TRACE_ERR("Failed to program vnic %u RX_STATS table entry",
                      spec->key.id);
        return sdk::SDK_RET_HW_PROGRAM_ERR;
    }

    // program vnic info tables (in rxdma and txdma pipes)
    ret = program_vnic_info_(vpc, subnet, spec);
    return SDK_RET_OK;
}

sdk_ret_t
vnic_impl::reprogram_hw(api_base *api_obj, api_op_t api_op) {
    sdk_ret_t ret;
    pds_subnet_key_t subnet_key;
    vnic_entry *vnic = (vnic_entry *)api_obj;
    subnet_entry *subnet;

    subnet_key = vnic->subnet();
    subnet = (subnet_entry *)api_base::find_obj(OBJ_ID_SUBNET, &subnet_key);

    // TODO
    return ret;
}

sdk_ret_t
vnic_impl::cleanup_hw(api_base *api_obj, obj_ctxt_t *obj_ctxt) {
    return sdk::SDK_RET_INVALID_OP;
}

sdk_ret_t
vnic_impl::update_hw(api_base *orig_obj, api_base *curr_obj,
                     obj_ctxt_t *obj_ctxt) {
    return sdk::SDK_RET_INVALID_OP;
}

sdk_ret_t
vnic_impl::add_local_mapping_entry_(pds_epoch_t epoch, vpc_entry *vpc,
                                    subnet_entry *subnet, vnic_entry *vnic,
                                    pds_vnic_spec_t *spec) {
    sdk_ret_t ret;
    sdk_table_api_params_t tparams;
    local_mapping_swkey_t local_mapping_key = { 0 };
    local_mapping_appdata_t local_mapping_data = { 0 };

    // fill the key
    local_mapping_key.key_metadata_local_mapping_lkp_type = KEY_TYPE_MAC;
    local_mapping_key.key_metadata_local_mapping_lkp_id = subnet->hw_id();
    sdk::lib::memrev(local_mapping_key.key_metadata_local_mapping_lkp_addr,
                     spec->mac_addr, ETH_ADDR_LEN);

    // fill the data
    local_mapping_data.vnic_id = hw_id_;

    // program LOCAL_MAPPING entry
    PDS_IMPL_FILL_TABLE_API_PARAMS(&tparams, &local_mapping_key, NULL,
                                   &local_mapping_data,
                                   LOCAL_MAPPING_LOCAL_MAPPING_INFO_ID,
                                   local_mapping_hdl_);
    ret = mapping_impl_db()->local_mapping_tbl()->insert(&tparams);
    if (ret != SDK_RET_OK) {
        PDS_TRACE_ERR("Failed to program LOCAL_MAPPING entry for vnic %u "
                      "(subnet %u, mac %s)", spec->key.id, spec->subnet.id,
                      macaddr2str(spec->mac_addr));
        goto error;
    }
    return SDK_RET_OK;

error:
    return ret;
}

sdk_ret_t
vnic_impl::add_mapping_entry_(pds_epoch_t epoch, vpc_entry *vpc,
                              subnet_entry *subnet, vnic_entry *vnic,
                              pds_vnic_spec_t *spec) {
    sdk_ret_t ret;
    sdk_table_api_params_t tparams;
    mapping_swkey_t mapping_key = { 0 };
    mapping_appdata_t mapping_data = { 0 };

    // fill the key
    mapping_key.txdma_to_p4e_mapping_lkp_id = subnet->hw_id();
    mapping_key.p4e_i2e_mapping_lkp_type = KEY_TYPE_MAC;
    sdk::lib::memrev(mapping_key.p4e_i2e_mapping_lkp_addr,
                     spec->mac_addr, ETH_ADDR_LEN);

    // fill the data
    sdk::lib::memrev(mapping_data.dmaci, spec->mac_addr, ETH_ADDR_LEN);

    // program MAPPING table entry
    PDS_IMPL_FILL_TABLE_API_PARAMS(&tparams, &mapping_key, NULL, &mapping_data,
                                   MAPPING_MAPPING_INFO_ID,
                                   mapping_hdl_);
    ret = mapping_impl_db()->mapping_tbl()->insert(&tparams);
    if (ret != SDK_RET_OK) {
        PDS_TRACE_ERR("Failed to program MAPPING entry for vnic %u"
                      "(subnet %u, mac %s)", spec->key.id, spec->subnet.id,
                      macaddr2str(spec->mac_addr));
        goto error;
    }
    return SDK_RET_OK;

error:
    return ret;
}

sdk_ret_t
vnic_impl::activate_vnic_create_(pds_epoch_t epoch, vnic_entry *vnic,
                                 pds_vnic_spec_t *spec) {
    sdk_ret_t                ret;
    vpc_entry                *vpc;
    subnet_entry             *subnet;
    pds_route_table_key_t    route_table_key;
    route_table              *v4_route_table, *v6_route_table;
    pds_policy_key_t         policy_key;
    policy                   *ing_v4_policy, *ing_v6_policy;
    policy                   *egr_v4_policy, *egr_v6_policy;

    // fetch the subnet of this vnic
    subnet = subnet_db()->find(&spec->subnet);
    if (unlikely(subnet == NULL)) {
        return SDK_RET_INVALID_ARG;
    }

    vpc = vpc_db()->find(&spec->vpc);
    if (unlikely(vpc == NULL)) {
        return SDK_RET_INVALID_ARG;
    }
    route_table_key = subnet->v4_route_table();
    v4_route_table = route_table_db()->find(&route_table_key);
    route_table_key = subnet->v6_route_table();
    if (route_table_key.id != PDS_ROUTE_TABLE_ID_INVALID) {
        v6_route_table =
            route_table_db()->find(&route_table_key);
    } else {
        v6_route_table = NULL;
    }

    policy_key = subnet->ing_v4_policy();
    ing_v4_policy = policy_db()->policy_find(&policy_key);
    policy_key = subnet->ing_v6_policy();
    ing_v6_policy = policy_db()->policy_find(&policy_key);
    policy_key = subnet->egr_v4_policy();
    egr_v4_policy = policy_db()->policy_find(&policy_key);
    policy_key = subnet->egr_v6_policy();
    egr_v6_policy = policy_db()->policy_find(&policy_key);

    ret = add_local_mapping_entry_(epoch, vpc, subnet, vnic, spec);
    if (unlikely(ret != SDK_RET_OK)) {
        goto error;
    }

    ret = add_mapping_entry_(epoch, vpc, subnet, vnic, spec);
    if (unlikely(ret != SDK_RET_OK)) {
        goto error;
    }
    return SDK_RET_OK;

error:
    return ret;
}

sdk_ret_t
vnic_impl::activate_vnic_delete_(pds_epoch_t epoch, vnic_entry *vnic) {
    sdk_ret_t              ret;
    sdk_table_api_params_t tparams = { 0 };

    // TODO: read LOCAL_MAPPING table entry and cleanup the entry

    // TODO: read MAPPING table entry and cleanup the entry

    return ret;
}

sdk_ret_t
vnic_impl::activate_hw(api_base *api_obj, pds_epoch_t epoch,
                       api_op_t api_op, obj_ctxt_t *obj_ctxt) {
    sdk_ret_t          ret;
    pds_vnic_spec_t    *spec;

    switch (api_op) {
    case api::API_OP_CREATE:
        spec = &obj_ctxt->api_params->vnic_spec;
        ret = activate_vnic_create_(epoch, (vnic_entry *)api_obj, spec);
        break;

    case api::API_OP_DELETE:
        // spec is not available for DELETE operations
        ret = activate_vnic_delete_(epoch, (vnic_entry *)api_obj);
        break;

    case api::API_OP_UPDATE:
    default:
        ret = SDK_RET_INVALID_OP;
        break;
    }
    return ret;
}

sdk_ret_t
vnic_impl::reactivate_hw(api_base *api_obj, pds_epoch_t epoch,
                         api_op_t api_op) {
    return SDK_RET_INVALID_OP;
}

void
vnic_impl::fill_status_(pds_vnic_status_t *status) {
    status->hw_id = hw_id_;
}

sdk_ret_t
vnic_impl::fill_stats_(pds_vnic_stats_t *stats) {
    p4pd_error_t p4pd_ret;
    vnic_tx_stats_actiondata_t tx_stats = { 0 };
    vnic_rx_stats_actiondata_t rx_stats = { 0 };

    // read P4TBL_ID_VNIC_TX_STATS table
    p4pd_ret = p4pd_global_entry_read(P4TBL_ID_VNIC_TX_STATS, hw_id_, NULL,
                                      NULL, &tx_stats);
    if (p4pd_ret != P4PD_SUCCESS) {
        PDS_TRACE_ERR("Failed to read VNIC_TX_STATS table; hw_id:%u", hw_id_);
        return sdk::SDK_RET_HW_READ_ERR;
    }
    stats->tx_pkts  = *(uint64_t *)tx_stats.vnic_tx_stats_action.out_packets;
    stats->tx_bytes = *(uint64_t *)tx_stats.vnic_tx_stats_action.out_bytes;

    // read P4TBL_ID_VNIC_RX_STATS table
    p4pd_ret = p4pd_global_entry_read(P4TBL_ID_VNIC_RX_STATS, hw_id_, NULL,
                                      NULL, &rx_stats);
    if (p4pd_ret != P4PD_SUCCESS) {
        PDS_TRACE_ERR("Failed to read VNIC_RX_STATS table hw_id:%u", hw_id_);
        return sdk::SDK_RET_HW_READ_ERR;
    }
    stats->rx_pkts  = *(uint64_t *)rx_stats.vnic_rx_stats_action.in_packets;
    stats->rx_bytes = *(uint64_t *)rx_stats.vnic_rx_stats_action.in_bytes;
    return SDK_RET_OK;
}

sdk_ret_t
vnic_impl::fill_spec_(pds_vnic_spec_t *spec) {
    return SDK_RET_INVALID_OP;
}

sdk_ret_t
vnic_impl::read_hw(api_base *api_obj, obj_key_t *key, obj_info_t *info) {
    sdk_ret_t rv;
    pds_vnic_info_t *vnic_info = (pds_vnic_info_t *)info;

    rv = fill_spec_(&vnic_info->spec);
    if (unlikely(rv != SDK_RET_OK)) {
        PDS_TRACE_ERR("Failed to read hardware spec tables for vnic %s",
                      api_obj->key2str().c_str());
        return rv;
    }

    rv = fill_stats_(&vnic_info->stats);
    if (unlikely(rv != SDK_RET_OK)) {
        PDS_TRACE_ERR("Failed to read hardware stats tables for vnic %s",
                      api_obj->key2str().c_str());
        return rv;
    }
    fill_status_(&vnic_info->status);
    return SDK_RET_OK;
}

/// \@}

}    // namespace impl
}    // namespace api
