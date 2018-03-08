#include "nic/include/base.h"
#include "nic/hal/hal.hpp"
#include "nic/include/hal_lock.hpp"
#include "nic/include/hal_state.hpp"
#include "nic/include/hal_api_stats.hpp"
#include "nic/hal/src/utils.hpp"
#include "nic/hal/src/if_utils.hpp"
#include "nic/hal/src/nw.hpp"
#include "nic/include/pd_api.hpp"
#include "nic/include/nwsec_group_api.hpp"
#include "nic/hal/src/nwsec_group.hpp"


namespace hal {

// ----------------------------------------------------------------------------
// Get key function for network hash table
// ----------------------------------------------------------------------------
void *
network_get_key_func (void *entry)
{
    hal_handle_id_ht_entry_t    *ht_entry;
    network_t                   *nw = NULL;

    HAL_ASSERT(entry != NULL);
    ht_entry = (hal_handle_id_ht_entry_t *)entry;
    if (ht_entry == NULL) {
        return NULL;
    }
    nw = find_network_by_handle(ht_entry->handle_id);
    return (void *)&(nw->nw_key);

#if 0
    HAL_ASSERT(entry != NULL);
    return (void *)&(((network_t *)entry)->nw_key);
#endif
}

// ----------------------------------------------------------------------------
// Compute hash function for network hash table
// ----------------------------------------------------------------------------
uint32_t
network_compute_hash_func (void *key, uint32_t ht_size)
{
    return sdk::lib::hash_algo::fnv_hash(key, sizeof(network_key_t)) % ht_size;
}

// ----------------------------------------------------------------------------
// Compare key function for network hash table
// ----------------------------------------------------------------------------
bool
network_compare_key_func (void *key1, void *key2)
{
    HAL_ASSERT((key1 != NULL) && (key2 != NULL));
    if (!memcmp(key1, key2, sizeof(network_key_t))) {
        return true;
    }
    return false;
}

//------------------------------------------------------------------------------
// insert network to db
//------------------------------------------------------------------------------
static inline hal_ret_t
network_add_to_db (network_t *nw, hal_handle_t handle)
{
    hal_ret_t                   ret;
    sdk_ret_t                   sdk_ret;
    hal_handle_id_ht_entry_t    *entry;

    HAL_TRACE_DEBUG("{}:adding to network key hash table", 
                    __FUNCTION__);
    // allocate an entry to establish mapping from l2key to its handle
    entry =
        (hal_handle_id_ht_entry_t *)g_hal_state->
        hal_handle_id_ht_entry_slab()->alloc();
    if (entry == NULL) {
        return HAL_RET_OOM;
    }

    // add mapping from vrf id to its handle
    entry->handle_id = handle;
    sdk_ret = g_hal_state->network_key_ht()->insert_with_key(&nw->nw_key,
                                                         entry, 
                                                         &entry->ht_ctxt);
    if (sdk_ret != sdk::SDK_RET_OK) {
        HAL_TRACE_ERR("{}:failed to network key to handle mapping, "
                      "err : {}", __FUNCTION__, ret);
        hal::delay_delete_to_slab(HAL_SLAB_HANDLE_ID_HT_ENTRY, entry);
    }
    ret = hal_sdk_ret_to_hal_ret(sdk_ret);

    // TODO: Check if this is the right place
    nw->hal_handle = handle;

    return ret;
}

//------------------------------------------------------------------------------
// delete an ep from the config database
//------------------------------------------------------------------------------
static inline hal_ret_t
network_del_from_db (network_t *nw)
{
    hal_ret_t                   ret = HAL_RET_OK;
    hal_handle_id_ht_entry_t    *entry;

    HAL_TRACE_DEBUG("{}:removing from network key hash table", 
                    __FUNCTION__);
    // remove from hash table
    entry = (hal_handle_id_ht_entry_t *)g_hal_state->network_key_ht()->
            remove(&nw->nw_key);

    if (entry) {
        // free up
        hal::delay_delete_to_slab(HAL_SLAB_HANDLE_ID_HT_ENTRY, entry);
    } else {
        HAL_TRACE_ERR("{}:unable to find network:{}",
                      __FUNCTION__, network_to_str(nw));
        ret = HAL_RET_NETWORK_NOT_FOUND;
        goto end;
    }

end:
    return ret;;
}



//------------------------------------------------------------------------------
// validate an incoming network create request
// TODO:
// 1. check if vrf exists
// 2. validate L4 profile existence if that handle is valid
//------------------------------------------------------------------------------
static hal_ret_t
validate_network_create (NetworkSpec& spec, NetworkResponse *rsp)
{
    // key-handle field must be set
    if (!spec.has_key_or_handle()) {
        HAL_TRACE_ERR("{}:spec has no key or handle", __FUNCTION__);
        rsp->set_api_status(types::API_STATUS_INVALID_ARG);
        return HAL_RET_INVALID_ARG;
    }

    if (!spec.has_vrf_key_handle() ||
        spec.vrf_key_handle().vrf_id() == HAL_VRF_ID_INVALID) {
        HAL_TRACE_ERR("{}:vrf not found", __FUNCTION__);
        rsp->set_api_status(types::API_STATUS_VRF_ID_INVALID);
        return HAL_RET_INVALID_ARG;
    }

    return HAL_RET_OK;
}

hal_ret_t
network_create_add_cb (cfg_op_ctxt_t *cfg_ctxt)
{
    hal_ret_t       ret = HAL_RET_OK;

    // No PD calls
    return ret;
}

hal_ret_t
network_create_commit_cb (cfg_op_ctxt_t *cfg_ctxt)
{
    hal_ret_t                       ret = HAL_RET_OK;
    network_t                       *nw = NULL;
    dllist_ctxt_t                   *lnode = NULL;
    dhl_entry_t                     *dhl_entry = NULL;
    // network_create_app_ctxt_t       *app_ctxt = NULL; 
    hal_handle_t                    hal_handle = 0;

    if (cfg_ctxt == NULL) {
        HAL_TRACE_ERR("{}:invalid cfg_ctxt", __FUNCTION__);
        ret = HAL_RET_INVALID_ARG;
        goto end;
    }

    // assumption is there is only one element in the list
    lnode = cfg_ctxt->dhl.next;
    dhl_entry = dllist_entry(lnode, dhl_entry_t, dllist_ctxt);
    // app_ctxt = (network_create_app_ctxt_t *)cfg_ctxt->app_ctxt;

    nw = (network_t *)dhl_entry->obj;
    hal_handle = dhl_entry->handle;
    
    HAL_TRACE_DEBUG("{}:create commit CB {}",
                    __FUNCTION__, network_to_str(nw));

    // Add network to key DB
    ret = network_add_to_db (nw, hal_handle);
    if (ret != HAL_RET_OK) {
        HAL_TRACE_ERR("{}:unable to add network to DB", 
                      __FUNCTION__);
        goto end;
    }

    // Setup backward refs
    ret = network_update_sg_relation(&nw->sg_list_head, nw, true);
    if (ret != HAL_RET_OK) {
        HAL_TRACE_ERR("{}:failed to add sg -> network"
                      " relation ret:{}", __FUNCTION__, ret);
        goto end;
    }

    HAL_TRACE_DEBUG("{}:added network to DB", 
                    __FUNCTION__);
end:
    return ret;
}

// ----------------------------------------------------------------------------
// create abort callback
// ----------------------------------------------------------------------------
hal_ret_t
network_create_abort_cb (cfg_op_ctxt_t *cfg_ctxt)
{
    hal_ret_t                       ret = HAL_RET_OK;
    dhl_entry_t                     *dhl_entry = NULL;
    network_t                       *nw = NULL;
    hal_handle_t                    hal_handle = 0;
    dllist_ctxt_t                   *lnode = NULL;
    // dllist_ctxt_t                   *curr, *next;
    // hal_handle_id_list_entry_t      *entry = NULL;

    if (cfg_ctxt == NULL) {
        HAL_TRACE_ERR("pi-if:{}:invalid cfg_ctxt", __FUNCTION__);
        ret = HAL_RET_INVALID_ARG;
        goto end;
    }

    lnode = cfg_ctxt->dhl.next;
    dhl_entry = dllist_entry(lnode, dhl_entry_t, dllist_ctxt);

    nw = (network_t *)dhl_entry->obj;
    hal_handle = dhl_entry->handle;

    HAL_TRACE_DEBUG("{}:network:{}:create abort CB",
                    __FUNCTION__, network_to_str(nw));

    // clean up sgs as these are inserted before callbacks
    HAL_TRACE_DEBUG("{}:freeing up sg list entries", __FUNCTION__);
    hal_free_handles_list(&nw->sg_list_head);

    // remove the object
    hal_handle_free(hal_handle);

    // free PI if
    network_free(nw);
end:
    return ret;
}

// ----------------------------------------------------------------------------
// Dummy create cleanup callback
// ----------------------------------------------------------------------------
hal_ret_t
network_create_cleanup_cb (cfg_op_ctxt_t *cfg_ctxt)
{
    hal_ret_t   ret = HAL_RET_OK;

    return ret;
}

//------------------------------------------------------------------------------
// Converts hal_ret_t to API status
//------------------------------------------------------------------------------
hal_ret_t
network_prepare_rsp (NetworkResponse *rsp, hal_ret_t ret, 
                     hal_handle_t hal_handle)
{
    if (ret == HAL_RET_OK) {
        rsp->mutable_status()->set_nw_handle(hal_handle);
    }

    rsp->set_api_status(hal_prepare_rsp(ret));

    return HAL_RET_OK;
}

//------------------------------------------------------------------------------
// Reads security groups from spec and populate SG object
//------------------------------------------------------------------------------
hal_ret_t
network_read_security_groups (network_t *nw, NetworkSpec& spec)
{
    hal_ret_t               ret        = HAL_RET_OK;
    uint32_t                num_sgs    = 0, i        = 0;
    nwsec_group_t           *sg        = NULL;
    hal_handle_t             sg_handle = 0;

    num_sgs = spec.sg_key_handle_size();

    HAL_TRACE_DEBUG("{}:adding {} no. of sgs", __FUNCTION__,
                    num_sgs);

    for (i = 0; i < num_sgs; i++) {
        sg = nwsec_group_lookup_key_or_handle(spec.sg_key_handle(i));
        if (!sg) {
            return HAL_RET_INVALID_ARG;
        }
        
        // Add to aggregated list
        sg_handle = sg->hal_handle;
        hal_add_to_handle_list(&nw->sg_list_head, sg_handle);
        // ret = add_nw_to_security_group(sg->sg_id, nw->hal_handle);
        // HAL_ASSERT_RETURN(ret == HAL_RET_OK, ret);
    }

    return ret;
}

//------------------------------------------------------------------------------
// process a vrf create request
// TODO: if vrf exists, treat this as modify
//------------------------------------------------------------------------------
hal_ret_t
network_create (NetworkSpec& spec, NetworkResponse *rsp)
{
    hal_ret_t                       ret = HAL_RET_OK;
    network_t                       *nw = NULL;
    vrf_id_t                     tid;
    ep_t                            *gw_ep = NULL;
    vrf_t                        *vrf = NULL;
    network_create_app_ctxt_t       app_ctxt;
    dhl_entry_t                     dhl_entry = { 0 };
    cfg_op_ctxt_t                   cfg_ctxt = { 0 };
    ip_prefix_t                     ip_pfx;

    hal_api_trace(" API Begin: network create ");

    auto kh = spec.key_or_handle();
    auto nw_pfx = kh.ip_prefix();

    // validate the request message
    ret = validate_network_create(spec, rsp);
    if (ret != HAL_RET_OK) {
        goto end;
    }

    // fetch the vrf information
    tid = spec.vrf_key_handle().vrf_id();
    vrf = vrf_lookup_by_id(tid);
    if (vrf == NULL) {
        HAL_TRACE_ERR("{}: unable to retrieve vrf_id:{}",
                __FUNCTION__, tid);
        ret = HAL_RET_VRF_NOT_FOUND;
        goto end;
    }

    // check if gateway ep is present
    if (spec.gateway_ep_handle() != HAL_HANDLE_INVALID) {
        gw_ep = find_ep_by_handle(spec.gateway_ep_handle());
        if (gw_ep == NULL) {
            HAL_TRACE_ERR("{}: unable to retrieve gateway endpoint:{}",
                    __FUNCTION__);
            ret = HAL_RET_EP_NOT_FOUND;
            goto end;
        }
    } else {
        HAL_TRACE_DEBUG("{}:gateway way ep is not present."
                        " flows using this network will not have reachability info.");
    }

    // check if network with pfx already exists
    ip_pfx_spec_to_pfx_spec(&ip_pfx, nw_pfx);
    if (find_network_by_key(tid, &ip_pfx)) {
        HAL_TRACE_ERR("{}:network already exists (tid,ippfx) : {}:{}",
                      __FUNCTION__, tid, ippfx2str(&ip_pfx));
        ret = HAL_RET_ENTRY_EXISTS;
        goto end;
    }

    // instantiate a network
    nw = network_alloc_init();
    if (nw == NULL) {
        ret = HAL_RET_OOM;
        HAL_TRACE_ERR("{}: out of memory. err: {}", 
                      ret);
        goto end;
    }

    // allocate hal handle id
    nw->hal_handle = hal_handle_alloc(HAL_OBJ_ID_NETWORK);
    if (nw->hal_handle == HAL_HANDLE_INVALID) {
        HAL_TRACE_ERR("{}: failed to alloc handle", 
                      __FUNCTION__);
        ret = HAL_RET_HANDLE_INVALID;
        goto end;
    }

    nw->nw_key.vrf_id = tid;
    nw->gw_ep_handle = spec.gateway_ep_handle();
    MAC_UINT64_TO_ADDR(nw->rmac_addr, spec.rmac());
    ip_pfx_spec_to_pfx_spec(&nw->nw_key.ip_pfx, nw_pfx);
    network_read_security_groups(nw, spec);

    HAL_TRACE_DEBUG("{}:nw: {}, rmac: {}", 
                    __FUNCTION__,
                    ippfx2str(&nw->nw_key.ip_pfx),
                    macaddr2str(nw->rmac_addr));

    dhl_entry.handle = nw->hal_handle;
    dhl_entry.obj = nw;
    cfg_ctxt.app_ctxt = &app_ctxt;
    sdk::lib::dllist_reset(&cfg_ctxt.dhl);
    sdk::lib::dllist_reset(&dhl_entry.dllist_ctxt);
    sdk::lib::dllist_add(&cfg_ctxt.dhl, &dhl_entry.dllist_ctxt);
    ret = hal_handle_add_obj(nw->hal_handle, &cfg_ctxt, 
                             network_create_add_cb,
                             network_create_commit_cb,
                             network_create_abort_cb, 
                             network_create_cleanup_cb);

end:
    
    if (ret != HAL_RET_OK) {
	    if (nw != NULL) {
            // if there is an error, nw will be freed in abort CB
            // network_free(nw);
            nw = NULL;
	    }
	    HAL_API_STATS_INC(HAL_API_NETWORK_CREATE_FAIL);
    } else {
	    HAL_API_STATS_INC(HAL_API_NETWORK_CREATE_SUCCESS);
    }
    network_prepare_rsp(rsp, ret, nw ? nw->hal_handle : HAL_HANDLE_INVALID);
    hal_api_trace(" API End: network create ");
    return ret;
}

//------------------------------------------------------------------------------
// validate network update request
//------------------------------------------------------------------------------
hal_ret_t
validate_network_update (NetworkSpec& spec, NetworkResponse *rsp)
{
    hal_ret_t   ret = HAL_RET_OK;

    // key-handle field must be set
    if (!spec.has_key_or_handle()) {
        HAL_TRACE_ERR("{}:spec has no key or handle", __FUNCTION__);
        ret =  HAL_RET_INVALID_ARG;
    }

    return ret;
}

//------------------------------------------------------------------------------
// Lookup network from key or handle
//------------------------------------------------------------------------------
network_t *
network_lookup_key_or_handle (NetworkKeyHandle& kh, vrf_id_t tid)
{
    // vrf_id_t                     tid;
    network_t                       *nw = NULL;
    ip_prefix_t                     ip_pfx;

    auto nw_pfx = kh.ip_prefix();

    // tid = req.meta().vrf_id();
    ip_pfx_spec_to_pfx_spec(&ip_pfx, nw_pfx);

    if (kh.key_or_handle_case() == NetworkKeyHandle::kIpPrefix) {
        nw = find_network_by_key(tid, &ip_pfx);
    } else if (kh.key_or_handle_case() == NetworkKeyHandle::kNwHandle) {
        nw = find_network_by_handle(kh.nw_handle());
    }

    return nw;
}

//------------------------------------------------------------------------------
// Make a clone
// - Both PI and PD objects cloned. 
//------------------------------------------------------------------------------
hal_ret_t
network_make_clone (network_t *nw, network_t **nw_clone)
{
    *nw_clone = network_alloc_init();
    memcpy(*nw_clone, nw, sizeof(network_t));


    // After clone always reset lists
    dllist_reset(&(*nw_clone)->sg_list_head);
    dllist_reset(&(*nw_clone)->l2seg_list_head);
    dllist_reset(&(*nw_clone)->session_list_head);


    // nw doesnt have PD
    // pd::pd_nwsec_profile_make_clone(nwsec, *nwsec_clone);


    // Keep new values in the clone
    // nwsec_profile_init_from_spec(*nwsec_clone, spec);


    return HAL_RET_OK;
}

//------------------------------------------------------------------------------
// network update gateway ep change
//------------------------------------------------------------------------------
hal_ret_t
network_handle_gateway_ep_change (network_t *nw, network_t *nw_clone,
                                  network_update_app_ctxt_t *app_ctxt)
{
    hal_ret_t                   ret = HAL_RET_OK;
    // TODO: Uncomment once FTE has the API to handle reachability change
#if 0
    dllist_ctxt_t               *curr = NULL, *next = NULL;
    hal_handle_id_list_entry_t  *entry = NULL;
    session_t                   *session = NULL;

    // walk the sessions and give a callback
    dllist_for_each_safe(curr, next, &nw->session_list_head) {
        entry = dllist_entry(curr, hal_handle_id_list_entry_t, dllist_ctxt);
        session = find_session_by_handle(entry->handle_id);
        ret = fte::fte_handle_network_reachability_change(session,
                                                          app_ctxt->new_gw_ep_handle);
        if (ret != HAL_RET_OK) {
            HAL_TRACE_ERR("{}:failed in fte for reachability change",
                          __FUNCTION__);
            goto end;
        }
    }

end:
#endif
    return ret;
}

// ----------------------------------------------------------------------------
// Add/Del relation sg -> network for all sgs in the list
// ----------------------------------------------------------------------------
hal_ret_t
network_update_sg_relation (dllist_ctxt_t *sg_list, network_t *nw, bool add)
{
    hal_ret_t                   ret           = HAL_RET_OK;
    dllist_ctxt_t               *curr, *next;
    hal_handle_id_list_entry_t  *entry        = NULL;
    nwsec_group_t               *sg           = NULL;

    dllist_for_each_safe(curr, next, sg_list) {
        entry = dllist_entry(curr, hal_handle_id_list_entry_t, dllist_ctxt);
        sg = nwsec_group_lookup_by_handle(entry->handle_id);
        if (!sg) {
            HAL_TRACE_ERR("{}:unable to find sg with handle:{}",
                          __FUNCTION__, entry->handle_id);
            ret = HAL_RET_SG_NOT_FOUND;
            goto end;
        }
        if (add) {
            ret = add_nw_to_security_group(sg->sg_id, nw->hal_handle);
        } else {
            // TODO: security_group
            // ret = security_group_del_network(sg->sg_id, nw->hal_handle);
        }
    }

end:
    return ret;
}

// ----------------------------------------------------------------------------
// updates PI with new sg list from GRPC
// ----------------------------------------------------------------------------
hal_ret_t
network_update_pi_with_sg_list (network_t *nw, network_update_app_ctxt_t *app_ctxt)
{
    hal_ret_t                       ret = HAL_RET_OK;

    // lock network
    // Revisit: this is a clone and may be we dont have to take the lock
    network_lock(nw, __FILENAME__, __LINE__, __func__);

    // Free list in clone
    hal_free_handles_list(&nw->sg_list_head);

    // Move aggregated list to clone
    dllist_move(&nw->sg_list_head, app_ctxt->aggr_sglist);

    // add/del relations from sg
    ret = network_update_sg_relation(app_ctxt->add_sglist,
                                     nw, true);
    if (ret != HAL_RET_OK) {
        HAL_TRACE_ERR("{}:failed to add sg -> network "
                "relation ret:{}", 
                __FUNCTION__,  ret);
        goto end;
    }

    ret = network_update_sg_relation(app_ctxt->del_sglist,
                                     nw, false);
    if (ret != HAL_RET_OK) {
        HAL_TRACE_ERR("{}:failed to del sg -/-> network "
                "relation ret:{}", 
                __FUNCTION__,  ret);
        goto end;
    }

end:
    // Free add & del list
    hal_cleanup_handle_list(&app_ctxt->add_sglist);
    hal_cleanup_handle_list(&app_ctxt->del_sglist);
    hal_cleanup_handle_list(&app_ctxt->aggr_sglist);

    // Unlock network
    network_unlock(nw, __FILENAME__, __LINE__, __func__);

    return ret;
}

//------------------------------------------------------------------------------
// network update main CB
//------------------------------------------------------------------------------
hal_ret_t
network_update_upd_cb (cfg_op_ctxt_t *cfg_ctxt)
{
    hal_ret_t                       ret = HAL_RET_OK;
    dllist_ctxt_t                   *lnode = NULL;
    dhl_entry_t                     *dhl_entry = NULL;
    network_t                       *nw = NULL, *nw_clone = NULL;
    network_update_app_ctxt_t       *app_ctxt = NULL;

    if (cfg_ctxt == NULL) {
        HAL_TRACE_ERR("pi-network{}:invalid cfg_ctxt", __FUNCTION__);
        ret = HAL_RET_INVALID_ARG;
        goto end;
    }

    lnode = cfg_ctxt->dhl.next;
    dhl_entry = dllist_entry(lnode, dhl_entry_t, dllist_ctxt);
    app_ctxt = (network_update_app_ctxt_t *)cfg_ctxt->app_ctxt;

    nw = (network_t *)dhl_entry->obj;
    nw_clone = (network_t *)dhl_entry->cloned_obj;

    HAL_TRACE_DEBUG("{}: update upd cb {}",
                    __FUNCTION__, network_to_str(nw));

    // trigger session to handle change of reachability
    if (app_ctxt->gw_ep_changed) {
        ret = network_handle_gateway_ep_change(nw, nw_clone, app_ctxt);
    }

    if (app_ctxt->sglist_changed) {
        // TODO: any call backs?
    }

    // no pd call

end:
    return ret;
}

//------------------------------------------------------------------------------
// network update commit CB
//------------------------------------------------------------------------------
hal_ret_t
network_update_commit_cb (cfg_op_ctxt_t *cfg_ctxt)
{
    hal_ret_t                       ret = HAL_RET_OK;
    dllist_ctxt_t                   *lnode = NULL;
    dhl_entry_t                     *dhl_entry = NULL;
    network_t                       *nw = NULL, *nw_clone = NULL;
    network_update_app_ctxt_t       *app_ctxt = NULL;

    if (cfg_ctxt == NULL) {
        HAL_TRACE_ERR("pi-networkf{}:invalid cfg_ctxt", __FUNCTION__);
        ret = HAL_RET_INVALID_ARG;
        goto end;
    }

    lnode = cfg_ctxt->dhl.next;
    dhl_entry = dllist_entry(lnode, dhl_entry_t, dllist_ctxt);
    app_ctxt = (network_update_app_ctxt_t *)cfg_ctxt->app_ctxt;

    nw = (network_t *)dhl_entry->obj;
    nw_clone = (network_t *)dhl_entry->cloned_obj;

    HAL_TRACE_DEBUG("{}:update commit CB {}",
                    __FUNCTION__, network_to_str(nw));

    // move lists
    dllist_move(&nw_clone->sg_list_head, &nw->sg_list_head);
    dllist_move(&nw_clone->l2seg_list_head, &nw->l2seg_list_head);
    dllist_move(&nw_clone->session_list_head, &nw->session_list_head);

    // update clone with new values
    if (app_ctxt->gw_ep_changed) {
        nw_clone->gw_ep_handle = app_ctxt->new_gw_ep_handle;
    }

    if (app_ctxt->sglist_changed) {
        ret = network_update_pi_with_sg_list(nw_clone, app_ctxt);
    }

    // Free PI
    network_free(nw);
end:

    return ret;
}

//------------------------------------------------------------------------------
// network update abort CB
//------------------------------------------------------------------------------
hal_ret_t
network_update_abort_cb (cfg_op_ctxt_t *cfg_ctxt)
{
    hal_ret_t                       ret = HAL_RET_OK;
    dllist_ctxt_t                   *lnode = NULL;
    dhl_entry_t                     *dhl_entry = NULL;
    network_t                       *nw = NULL/*, *nw_clone = NULL*/;
    network_update_app_ctxt_t       *app_ctxt = NULL;

    if (cfg_ctxt == NULL) {
        HAL_TRACE_ERR("pi-networkf{}:invalid cfg_ctxt", __FUNCTION__);
        ret = HAL_RET_INVALID_ARG;
        goto end;
    }

    lnode = cfg_ctxt->dhl.next;
    dhl_entry = dllist_entry(lnode, dhl_entry_t, dllist_ctxt);
    app_ctxt = (network_update_app_ctxt_t *)cfg_ctxt->app_ctxt;

    nw = (network_t *)dhl_entry->obj;
    // nw_clone = (network_t *)dhl_entry->cloned_obj;

    HAL_TRACE_DEBUG("{}:update abort CB {}",
                    __FUNCTION__, network_to_str(nw));

    // Free sg lists
    hal_cleanup_handle_list(&app_ctxt->add_sglist);
    hal_cleanup_handle_list(&app_ctxt->del_sglist);
    hal_cleanup_handle_list(&app_ctxt->aggr_sglist);

    // Free PI
    network_free(nw);

end:
    return ret;
}

//------------------------------------------------------------------------------
// network update cleanup CB
//------------------------------------------------------------------------------
hal_ret_t
network_update_cleanup_cb (cfg_op_ctxt_t *cfg_ctxt)
{
    hal_ret_t                       ret = HAL_RET_OK;

    return ret;
}

//------------------------------------------------------------------------------
// checks if sg in network
//------------------------------------------------------------------------------
bool 
sg_in_network (network_t *nw, uint32_t sg_id, hal_handle_id_list_entry_t **handle_entry)
{
    dllist_ctxt_t                   *lnode = NULL;
    hal_handle_id_list_entry_t      *entry = NULL;
    nwsec_group_t                   *sg    = NULL;

    dllist_for_each(lnode, &(nw->sg_list_head)) {
        entry = dllist_entry(lnode, hal_handle_id_list_entry_t, dllist_ctxt);
        sg = nwsec_group_lookup_by_key(sg_id);
        if (entry->handle_id == sg->hal_handle) {
            if (handle_entry) {
                *handle_entry = entry;
            }
            return true;
        }
    }

    return false;
}

//------------------------------------------------------------------------------
// checks sg list change for network update
//------------------------------------------------------------------------------
hal_ret_t
network_check_sglist_update(NetworkSpec& spec, network_t *nw,
                            bool *sglist_change,
                            dllist_ctxt_t **add_sglist, 
                            dllist_ctxt_t **del_sglist,
                            dllist_ctxt_t **aggr_sglist)
{
    hal_ret_t                       ret       = HAL_RET_OK;
    uint16_t                        num_sgs   = 0, i          = 0;
    dllist_ctxt_t                   *lnode    = NULL;
    bool                            sg_exists = false;
    uint64_t                        sg_handle = 0;
    uint32_t                        sg_id     = 0;
    nwsec_group_t                   *sg       = NULL;
    hal_handle_id_list_entry_t      *entry    = NULL, *lentry = NULL;

    *sglist_change = false;

    *add_sglist = (dllist_ctxt_t *)HAL_CALLOC(HAL_MEM_ALLOC_DLLIST, 
                                               sizeof(dllist_ctxt_t));
    HAL_ABORT(*add_sglist != NULL);
    *del_sglist = (dllist_ctxt_t *)HAL_CALLOC(HAL_MEM_ALLOC_DLLIST, 
                                               sizeof(dllist_ctxt_t));
    HAL_ABORT(*del_sglist != NULL);
    *aggr_sglist = (dllist_ctxt_t *)HAL_CALLOC(HAL_MEM_ALLOC_DLLIST,
                                                sizeof(dllist_ctxt_t));
    HAL_ABORT(*aggr_sglist != NULL);

    sdk::lib::dllist_reset(*add_sglist);
    sdk::lib::dllist_reset(*del_sglist);
    sdk::lib::dllist_reset(*aggr_sglist);

    num_sgs = spec.sg_key_handle_size();
    HAL_TRACE_DEBUG("{}:num_sgs:{}", 
                    __FUNCTION__, num_sgs);
    for (i = 0; i < num_sgs; i++) {
        sg = nwsec_group_lookup_key_or_handle(spec.sg_key_handle(i));
        HAL_ASSERT_RETURN(sg != NULL, HAL_RET_INVALID_ARG);
        sg_id = sg->sg_id;

        // Add to aggregated list
        sg_handle = sg->hal_handle;
        hal_add_to_handle_list(*aggr_sglist, sg_handle);

        if (sg_in_network(nw, sg_id, NULL)) {
            continue;
        } else {
            // Add to added list
            hal_add_to_handle_list(*add_sglist, sg_handle);
            *sglist_change = true;
            HAL_TRACE_DEBUG("{}: added to add list hdl: {}", 
                    __FUNCTION__, sg_handle);
        }
    }

    HAL_TRACE_DEBUG("{}:Existing sgs:", __FUNCTION__);
    hal_print_handles_list(&nw->sg_list_head);
    HAL_TRACE_DEBUG("{}:New Aggregated sgs:", __FUNCTION__);
    hal_print_handles_list(*aggr_sglist);
    HAL_TRACE_DEBUG("{}:added sgs:", __FUNCTION__);
    hal_print_handles_list(*add_sglist);

    dllist_for_each(lnode, &(nw->sg_list_head)) {
        entry = dllist_entry(lnode, hal_handle_id_list_entry_t, dllist_ctxt);
        HAL_TRACE_DEBUG("{}: Checking for sg: {}", 
                __FUNCTION__, entry->handle_id);
        for (i = 0; i < num_sgs; i++) {
            auto sg = nwsec_group_lookup_key_or_handle(spec.sg_key_handle(i));
            sg_handle = sg->hal_handle;
            HAL_TRACE_DEBUG("{}:grpc sg hdl: {}", __FUNCTION__, sg_handle);
            if (entry->handle_id == sg_handle) {
                sg_exists = true;
                break;
            }
        }
        if (!sg_exists) {
            // Have to delet the sg
            lentry = (hal_handle_id_list_entry_t *)g_hal_state->
                hal_handle_id_list_entry_slab()->alloc();
            if (lentry == NULL) {
                ret = HAL_RET_OOM;
                goto end;
            }
            lentry->handle_id = entry->handle_id;

            // Insert into the list
            sdk::lib::dllist_add(*del_sglist, &lentry->dllist_ctxt);
            *sglist_change = true;
            HAL_TRACE_DEBUG("{}: added to delete list hdl: {}", 
                    __FUNCTION__, lentry->handle_id);
        }
        sg_exists = false;
    }

    HAL_TRACE_DEBUG("{}:deleted sgs:", __FUNCTION__);
    hal_print_handles_list(*del_sglist);

    if (!*sglist_change) {
        // Got same sgs as existing
        interface_cleanup_handle_list(add_sglist);
        interface_cleanup_handle_list(del_sglist);
        interface_cleanup_handle_list(aggr_sglist);
    }
end:
    return ret;
}

//------------------------------------------------------------------------------
// check what changes in the network update
//------------------------------------------------------------------------------
hal_ret_t
network_check_update (NetworkSpec& spec, network_t *nw, 
                      network_update_app_ctxt_t *app_ctxt)
{
    hal_ret_t           ret = HAL_RET_OK;

    // check for the gateway ep change
    if (nw->gw_ep_handle != spec.gateway_ep_handle()) {
        app_ctxt->gw_ep_changed = true;
        app_ctxt->new_gw_ep_handle = spec.gateway_ep_handle();
    }
    
    // check for sg list change
    ret = network_check_sglist_update(spec, nw, &app_ctxt->sglist_changed,
                                      &app_ctxt->add_sglist,
                                      &app_ctxt->del_sglist,
                                      &app_ctxt->aggr_sglist);
    if (ret != HAL_RET_OK) {
        HAL_TRACE_ERR("{}:failed to check sg list change. ret:{}",
                      __FUNCTION__, ret);
        goto end;
    }

    if (app_ctxt->gw_ep_changed || app_ctxt->sglist_changed) {
        app_ctxt->network_changed = true;
    }

end:
    return ret;
}


//------------------------------------------------------------------------------
// process a network update request
//------------------------------------------------------------------------------
hal_ret_t
network_update (NetworkSpec& spec, NetworkResponse *rsp)
{
    hal_ret_t                       ret = HAL_RET_OK;
    network_t                       *nw = NULL;
    cfg_op_ctxt_t                   cfg_ctxt = { 0 };
    dhl_entry_t                     dhl_entry = { 0 };
    network_update_app_ctxt_t       app_ctxt = { 0 };

    hal_api_trace(" API Begin: network update ");

    auto kh = spec.key_or_handle();
    auto nw_pfx = kh.ip_prefix();

    // validate the request message
    ret = validate_network_update(spec, rsp);
    if (ret != HAL_RET_OK) {
        HAL_TRACE_ERR("{}:network update validation failed, ret : {}",
                      __FUNCTION__, ret);
        goto end;
    }

    // retrieve network object
    nw = network_lookup_key_or_handle(kh, spec.vrf_key_handle().vrf_id());
    if (nw == NULL) {
        HAL_TRACE_ERR("{}:failed to find nw handle {}",
                      __FUNCTION__, kh.nw_handle());
        ret = HAL_RET_NETWORK_NOT_FOUND;
        goto end;
    }

    HAL_TRACE_DEBUG("{}: network update for {}",
                    __FUNCTION__, network_to_str(nw));

    ret = network_check_update(spec, nw, &app_ctxt);
    if (ret != HAL_RET_OK) {
        HAL_TRACE_ERR("{}:network check update failed, ret : {}", 
                      __FUNCTION__, ret);
        goto end;
    }

    // check if anything changed
    if (!app_ctxt.network_changed) {
        HAL_TRACE_ERR("{}:no change in network update: noop", 
                      __FUNCTION__);
        goto end;
    }

    network_make_clone(nw, (network_t **)&dhl_entry.cloned_obj);

    // form ctxt and call infra update object
    // app_ctxt.spec = &spec;
    dhl_entry.handle = nw->hal_handle;
    dhl_entry.obj = nw;
    cfg_ctxt.app_ctxt = &app_ctxt;
    sdk::lib::dllist_reset(&cfg_ctxt.dhl);
    sdk::lib::dllist_reset(&dhl_entry.dllist_ctxt);
    sdk::lib::dllist_add(&cfg_ctxt.dhl, &dhl_entry.dllist_ctxt);
    ret = hal_handle_upd_obj(nw->hal_handle, &cfg_ctxt, 
                             network_update_upd_cb,
                             network_update_commit_cb,
                             network_update_abort_cb, 
                             network_update_cleanup_cb);

end:
    if (ret == HAL_RET_OK) {
	    HAL_API_STATS_INC(HAL_API_NETWORK_UPDATE_SUCCESS);
    } else {
	    HAL_API_STATS_INC(HAL_API_NETWORK_UPDATE_FAIL);
    }
    network_prepare_rsp(rsp, ret, 
                        nw ? nw->hal_handle : HAL_HANDLE_INVALID);
    hal_api_trace(" API End: network update ");
    return ret;

}

//------------------------------------------------------------------------------
// process a vrf get request
//------------------------------------------------------------------------------
hal_ret_t
network_get (NetworkGetRequest& req, NetworkGetResponseMsg *rsp)
{
    network_key_t         nw_key = { 0 };
    ip_prefix_t           ip_pfx = { 0 };
    network_t             *nw;
    NetworkGetResponse    *response;

    if (!req.has_vrf_key_handle() ||
        req.vrf_key_handle().vrf_id() == HAL_VRF_ID_INVALID) {
        g_hal_state->network_key_ht()->walk(network_get_ht_cb, rsp);
	    HAL_API_STATS_INC(HAL_API_NETWORK_GET_SUCCESS);
        return HAL_RET_OK;
    }

    response = rsp->add_response();

    if (req.has_key_or_handle()) {
        auto kh = req.key_or_handle();
        if (kh.key_or_handle_case() == NetworkKeyHandle::kIpPrefix) {
            auto nw_pfx = kh.ip_prefix();

            nw_key.vrf_id = req.vrf_key_handle().vrf_id();
            ip_pfx_spec_to_pfx_spec(&ip_pfx, nw_pfx);

            nw = find_network_by_key(nw_key.vrf_id, &ip_pfx);

        } else if (kh.key_or_handle_case() ==
                       NetworkKeyHandle::kNwHandle) {
            nw = find_network_by_handle(kh.nw_handle());
        } else {
            response->set_api_status(types::API_STATUS_INVALID_ARG);
	        HAL_API_STATS_INC(HAL_API_NETWORK_GET_FAIL);
            return HAL_RET_INVALID_ARG;
        }
    } else {
        response->set_api_status(types::API_STATUS_INVALID_ARG);
	    HAL_API_STATS_INC(HAL_API_NETWORK_GET_FAIL);
        return HAL_RET_INVALID_ARG;
    }

    if (nw == NULL) {
        response->set_api_status(types::API_STATUS_NOT_FOUND);
	    HAL_API_STATS_INC(HAL_API_NETWORK_GET_FAIL);
        return HAL_RET_EP_NOT_FOUND;
    }

    // fill config spec of this vrf
    response->mutable_spec()->mutable_vrf_key_handle()->set_vrf_id(nw->nw_key.vrf_id);
    response->mutable_spec()->set_rmac(MAC_TO_UINT64(nw->rmac_addr));

    response->set_api_status(types::API_STATUS_OK);
	HAL_API_STATS_INC(HAL_API_NETWORK_GET_SUCCESS);
    return HAL_RET_OK;
}

hal_ret_t
validate_network_delete_req (NetworkDeleteRequest& req, 
                         NetworkDeleteResponse* rsp)
{
    hal_ret_t   ret = HAL_RET_OK;

    // key-handle field must be set
    if (!req.has_key_or_handle()) {
        HAL_TRACE_ERR("{}:spec has no key or handle", __FUNCTION__);
        ret =  HAL_RET_INVALID_ARG;
    }

    return ret;
}

//------------------------------------------------------------------------------
// validate network delete request
//------------------------------------------------------------------------------
hal_ret_t
validate_network_delete (network_t *nw)
{
    hal_ret_t   ret = HAL_RET_OK;

    // check for no presence of back references
    if (dllist_count(&nw->l2seg_list_head)) {
        ret = HAL_RET_OBJECT_IN_USE;
        HAL_TRACE_ERR("{}:l2segs still referring:", __FUNCTION__);
        hal_print_handles_list(&nw->l2seg_list_head);
    }
    if (dllist_count(&nw->session_list_head)) {
        ret = HAL_RET_OBJECT_IN_USE;
        HAL_TRACE_ERR("{}:sessions still referring:", __FUNCTION__);
        hal_print_handles_list(&nw->session_list_head);
    }

    return ret;
}
//------------------------------------------------------------------------------
// Delete main CB
//------------------------------------------------------------------------------
hal_ret_t
network_delete_del_cb (cfg_op_ctxt_t *cfg_ctxt)
{
    hal_ret_t                   ret = HAL_RET_OK;

    return ret;
}

hal_ret_t
network_detach_from_security_groups (network_t *nw) 
{
    hal_ret_t                   ret = HAL_RET_OK;

    ret = network_update_sg_relation(&nw->sg_list_head, nw, false);
    if (ret != HAL_RET_OK) {
        HAL_TRACE_ERR("{}:failed to del sg -/-> network"
                "relation ret:{}", 
                __FUNCTION__,  ret);
        goto end;
    }

    // clean up sgs list
    HAL_TRACE_DEBUG("{}:cleaning up sg list", __FUNCTION__);
    hal_free_handles_list(&nw->sg_list_head);
end:

    return ret;
}


//------------------------------------------------------------------------------
// Update PI DBs as network_delete_del_cb() was a succcess
//      a. Delete from network key hash table
//      b. Remove object from handle id based hash table
//      c. Free PI network
//------------------------------------------------------------------------------
hal_ret_t
network_delete_commit_cb (cfg_op_ctxt_t *cfg_ctxt)
{
    hal_ret_t                   ret = HAL_RET_OK;
    dllist_ctxt_t               *lnode = NULL;
    dhl_entry_t                 *dhl_entry = NULL;
    network_t                   *nw = NULL;
    hal_handle_t                hal_handle = 0;

    if (cfg_ctxt == NULL) {
        HAL_TRACE_ERR("{}:invalid cfg_ctxt", __FUNCTION__);
        ret = HAL_RET_INVALID_ARG;
        goto end;
    }

    lnode = cfg_ctxt->dhl.next;
    dhl_entry = dllist_entry(lnode, dhl_entry_t, dllist_ctxt);

    nw = (network_t *)dhl_entry->obj;
    hal_handle = dhl_entry->handle;

    HAL_TRACE_DEBUG("{}:delete commit CB {}",
                    __FUNCTION__, network_to_str(nw));

    
    // remove back refs from security groups and free up list
    ret = network_detach_from_security_groups(nw);
    if (ret != HAL_RET_OK) {
        HAL_TRACE_ERR("{}:failed to detach from security groups, "
                      "ret:{}", __FUNCTION__, ret);
        goto end;
    }

    // Remove from network key hash table
    ret = network_del_from_db(nw);
    if (ret != HAL_RET_OK) {
        HAL_TRACE_ERR("{}:failed to del network {} from db, err : {}", 
                      __FUNCTION__, network_to_str(nw), ret);
        goto end;
    }

    // Remove object from handle id based hash table
    hal_handle_free(hal_handle);

    // Free PI network 
    network_free(nw);

    // TODO: Decrement the ref counts of dependent objects

end:
    if (ret != HAL_RET_OK) {
        HAL_TRACE_ERR("{}:commit CBs can't fail: ret:{}",
                      __FUNCTION__, ret);
        HAL_ASSERT(0);
    }
    return ret;
}

//------------------------------------------------------------------------------
// If delete fails, nothing to do
//------------------------------------------------------------------------------
hal_ret_t
network_delete_abort_cb (cfg_op_ctxt_t *cfg_ctxt)
{
    return HAL_RET_OK;
}

//------------------------------------------------------------------------------
// If delete fails, nothing to do
//------------------------------------------------------------------------------
hal_ret_t
network_delete_cleanup_cb (cfg_op_ctxt_t *cfg_ctxt)
{
    return HAL_RET_OK;
}

//------------------------------------------------------------------------------
// process a vrf delete request
//------------------------------------------------------------------------------
hal_ret_t
network_delete (NetworkDeleteRequest& req, NetworkDeleteResponse *rsp)
{
    hal_ret_t           ret = HAL_RET_OK;
    network_t           *nw = NULL;
    cfg_op_ctxt_t       cfg_ctxt = { 0 };
    dhl_entry_t         dhl_entry = { 0 };

    hal_api_trace(" API Begin: network delete ");

    auto kh = req.key_or_handle();
    auto nw_pfx = kh.ip_prefix();

    // validate the request message
    ret = validate_network_delete_req(req, rsp);
    if (ret != HAL_RET_OK) {
        HAL_TRACE_ERR("{}:network delete validation failed, ret : {}",
                      __FUNCTION__, ret);
        goto end;
    }

    nw = network_lookup_key_or_handle(kh, req.vrf_key_handle().vrf_id());
    if (nw == NULL) {
        HAL_TRACE_ERR("{}:failed to find nw handle {}",
                      __FUNCTION__, kh.nw_handle());
        ret = HAL_RET_NETWORK_NOT_FOUND;
        goto end;
    }

    HAL_TRACE_DEBUG("{}:deleting nw :{}", 
                    __FUNCTION__, network_to_str(nw));


    ret = validate_network_delete(nw);
    if (ret != HAL_RET_OK) {
        HAL_TRACE_ERR("pi-if:{}:if delete validation failed, ret : {}",
                      __FUNCTION__, ret);
        goto end;
    }

    // form ctxt and call infra add
    dhl_entry.handle = nw->hal_handle;
    dhl_entry.obj = nw;
    cfg_ctxt.app_ctxt = NULL;
    sdk::lib::dllist_reset(&cfg_ctxt.dhl);
    sdk::lib::dllist_reset(&dhl_entry.dllist_ctxt);
    sdk::lib::dllist_add(&cfg_ctxt.dhl, &dhl_entry.dllist_ctxt);
    ret = hal_handle_del_obj(nw->hal_handle, &cfg_ctxt, 
                             network_delete_del_cb,
                             network_delete_commit_cb,
                             network_delete_abort_cb, 
                             network_delete_cleanup_cb);

end:
    if (ret == HAL_RET_OK) {
	    HAL_API_STATS_INC(HAL_API_NETWORK_DELETE_SUCCESS);
    } else {
	    HAL_API_STATS_INC(HAL_API_NETWORK_DELETE_FAIL);
    }
    rsp->set_api_status(hal_prepare_rsp(ret));
    hal_api_trace(" API End: network delete ");
    return ret;
}

//-----------------------------------------------------------------------------
// Adds l2segs into nw list
//-----------------------------------------------------------------------------
hal_ret_t
network_add_l2seg (network_t *nw, l2seg_t *l2seg)
{
    hal_ret_t                   ret = HAL_RET_OK;
    hal_handle_id_list_entry_t  *entry = NULL;

    if (l2seg == NULL || nw == NULL) {
        ret = HAL_RET_INVALID_ARG;
        goto end;
    }

    // Allocate the entry
    entry = (hal_handle_id_list_entry_t *)g_hal_state->
        hal_handle_id_list_entry_slab()->alloc();
    if (entry == NULL) {
        ret = HAL_RET_OOM;
        goto end;
    }
    entry->handle_id = l2seg->hal_handle;

    network_lock(nw, __FILENAME__, __LINE__, __func__);      // lock
    // Insert into the list
    sdk::lib::dllist_add(&nw->l2seg_list_head, &entry->dllist_ctxt);
    network_unlock(nw, __FILENAME__, __LINE__, __func__);    // unlock

end:
    HAL_TRACE_DEBUG("{}:add network => l2seg(id:hdl), {} => {}:{}, ret:{}",
                    __FUNCTION__, network_to_str(nw), 
                    l2seg->seg_id, l2seg->hal_handle, ret);
    return ret;
}

//-----------------------------------------------------------------------------
// Remove l2seg from nw list
//-----------------------------------------------------------------------------
hal_ret_t
network_del_l2seg (network_t *nw, l2seg_t *l2seg)
{
    hal_ret_t                   ret = HAL_RET_L2SEG_NOT_FOUND;
    hal_handle_id_list_entry_t  *entry = NULL;
    dllist_ctxt_t               *curr = NULL, *next = NULL;


    network_lock(nw, __FILENAME__, __LINE__, __func__);      // lock
    dllist_for_each_safe(curr, next, &nw->l2seg_list_head) {
        entry = dllist_entry(curr, hal_handle_id_list_entry_t, dllist_ctxt);
        HAL_TRACE_DEBUG("pi-nw:{}:checking l2seg_hdl:{}", entry->handle_id);
        if (entry->handle_id == l2seg->hal_handle) {
            // Remove from list
            sdk::lib::dllist_del(&entry->dllist_ctxt);
            // Free the entry
            hal::delay_delete_to_slab(HAL_SLAB_HANDLE_ID_LIST_ENTRY, entry);
            ret = HAL_RET_OK;
        }
    }
    network_unlock(nw, __FILENAME__, __LINE__, __func__);    // unlock

    HAL_TRACE_DEBUG("{}:del network =/=> l2seg(id:hdl), "
                    "{} =/=> {}:{}, ret:{}",
                    __FUNCTION__, network_to_str(nw), l2seg->seg_id, 
                    l2seg->hal_handle, ret);
    return ret;
}

//-----------------------------------------------------------------------------
// Adds sessions into list in nw
//-----------------------------------------------------------------------------
hal_ret_t
network_add_session (network_t *nw, session_t *sess)
{
    hal_ret_t                   ret = HAL_RET_OK;
    hal_handle_id_list_entry_t  *entry = NULL;

    if (sess == NULL || nw == NULL) {
        ret = HAL_RET_INVALID_ARG;
        goto end;
    }

    // Allocate the entry
    entry = (hal_handle_id_list_entry_t *)g_hal_state->
        hal_handle_id_list_entry_slab()->alloc();
    if (entry == NULL) {
        ret = HAL_RET_OOM;
        goto end;
    }
    entry->handle_id = sess->hal_handle;

    network_lock(nw, __FILENAME__, __LINE__, __func__);      // lock
    // Insert into the list
    sdk::lib::dllist_add(&nw->session_list_head, &entry->dllist_ctxt);
    network_unlock(nw, __FILENAME__, __LINE__, __func__);    // unlock

end:
    HAL_TRACE_DEBUG("{}:add network => session_hdl, {} => {}, "
                    "ret:{}",
                    __FUNCTION__, network_to_str(nw), 
                    sess->hal_handle, ret);
    return ret;
}

//-----------------------------------------------------------------------------
// Remove l2seg from nw list
//-----------------------------------------------------------------------------
hal_ret_t
network_del_session (network_t *nw, session_t *sess)
{
    hal_ret_t                   ret = HAL_RET_IF_NOT_FOUND;
    hal_handle_id_list_entry_t  *entry = NULL;
    dllist_ctxt_t               *curr = NULL, *next = NULL;


    network_lock(nw, __FILENAME__, __LINE__, __func__);      // lock
    dllist_for_each_safe(curr, next, &nw->session_list_head) {
        entry = dllist_entry(curr, hal_handle_id_list_entry_t, dllist_ctxt);
        if (entry->handle_id == sess->hal_handle) {
            // Remove from list
            sdk::lib::dllist_del(&entry->dllist_ctxt);
            // Free the entry
            hal::delay_delete_to_slab(HAL_SLAB_HANDLE_ID_LIST_ENTRY, entry);
            ret = HAL_RET_OK;
        }
    }
    network_unlock(nw, __FILENAME__, __LINE__, __func__);    // unlock

    HAL_TRACE_DEBUG("{}:del network =/=> session_hdl, {} =/=> {}, "
                    "ret:{}",
                    __FUNCTION__, network_to_str(nw), 
                    sess->hal_handle, ret);
    return ret;
}

const char *
network_to_str (network_t *nw)
{
    static thread_local char       nw_str[4][50];
    static thread_local uint8_t    nw_str_next = 0;
    char                           *buf;

    buf = nw_str[nw_str_next++ & 0x3];
    memset(buf, 0, 50);
    if (nw) {
        snprintf(buf, 50, "(tid:ippfx) : (%lu:%s)", nw->nw_key.vrf_id,
                 ippfx2str(&nw->nw_key.ip_pfx));
    }
    return buf;
}


}    // namespace hal
