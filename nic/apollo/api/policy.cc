/**
 * Copyright (c) 2019 Pensando Systems, Inc.
 *
 * @file    policy.cc
 *
 * @brief   policy handling
 */

#include "nic/sdk/include/sdk/base.hpp"
#include "nic/apollo/core/trace.hpp"
#include "nic/apollo/core/mem.hpp"
#include "nic/apollo/api/policy.hpp"
#include "nic/apollo/api/oci_state.hpp"
#include "nic/apollo/framework/api_ctxt.hpp"
#include "nic/apollo/framework/api_engine.hpp"

namespace api {

/**
 * @defgroup OCI_POLICY - security policy functionality
 * @ingroup OCI_POLICY
 * @{
 */

/**< @brief    constructor */
policy::policy() {
    //SDK_SPINLOCK_INIT(&slock_, PTHREAD_PROCESS_PRIVATE);
    ht_ctxt_.reset();
}

/**
 * @brief    factory method to allocate & initialize a security policy instance
 * @param[in] oci_policy    security policy information
 * @return    new instance of security policy or NULL, in case of error
 */
policy *
policy::factory(oci_policy_t *oci_policy) {
    policy    *new_policy;

    if (oci_policy->policy_type != POLICY_TYPE_FIREWALL) {
        /**< we don't support any other policy type currently */
        return NULL;
    }
    /**< create security policy instance with defaults, if any */
    new_policy = policy_db()->policy_alloc();
    if (new_policy) {
        new (new_policy) policy();
        new_policy->impl_ = impl_base::factory(impl::IMPL_OBJ_ID_SECURITY_POLICY,
                                               oci_policy);
        if (new_policy->impl_ == NULL) {
            policy::destroy(new_policy);
            return NULL;
        }
    }
    return new_policy;
}

/**< @brief    destructor */
policy::~policy() {
    // TODO: fix me
    //SDK_SPINLOCK_DESTROY(&slock_);
}

/**
 * @brief    release all the s/w & h/w resources associated with this object,
 *           if any, and free the memory
 * @param[in] policy     security policy to be freed
 * NOTE: h/w entries themselves should have been cleaned up (by calling
 *       impl->cleanup_hw() before calling this
 */
void
policy::destroy(policy *policy) {
    policy->release_resources();
    if (policy->impl_) {
        impl_base::destroy(impl::IMPL_OBJ_ID_SECURITY_POLICY, policy->impl_);
    }
    policy->~policy();
    policy_db()->policy_free(policy);
}

/**
 * @brief     initialize security policy instance with the given config
 * @param[in] api_ctxt API context carrying the configuration
 * @return    SDK_RET_OK on success, failure status code on error
 */
sdk_ret_t
policy::init_config(api_ctxt_t *api_ctxt) {
    oci_policy_t    *oci_policy;
    
    oci_policy = &api_ctxt->api_params->policy_info;
    memcpy(&this->key_, &oci_policy->key,
           sizeof(oci_policy_key_t));
    return SDK_RET_OK;
}

/**
 * @brief    allocate h/w resources for this object
 * @param[in] orig_obj    old version of the unmodified object
 * @param[in] obj_ctxt    transient state associated with this API
 * @return    SDK_RET_OK on success, failure status code on error
 */
sdk_ret_t
policy::reserve_resources(api_base *orig_obj, obj_ctxt_t *obj_ctxt) {
    return impl_->reserve_resources(this);
}

/**
 * @brief    program all h/w tables relevant to this object except stage 0
 *           table(s), if any
 * @param[in] obj_ctxt    transient state associated with this API
 * @return   SDK_RET_OK on success, failure status code on error
 */
sdk_ret_t
policy::program_config(obj_ctxt_t *obj_ctxt) {
    OCI_TRACE_DEBUG("Programming security policy %u", key_.id);
    return impl_->program_hw(this, obj_ctxt);
}

/**
 * @brief     free h/w resources used by this object, if any
 * @return    SDK_RET_OK on success, failure status code on error
 */
sdk_ret_t
policy::release_resources(void) {
    return impl_->release_resources(this);
}

/**
 * @brief    cleanup all h/w tables relevant to this object except stage 0
 *           table(s), if any, by updating packed entries with latest epoch#
 * @param[in] obj_ctxt    transient state associated with this API
 * @return   SDK_RET_OK on success, failure status code on error
 */
sdk_ret_t
policy::cleanup_config(obj_ctxt_t *obj_ctxt) {
    return impl_->cleanup_hw(this, obj_ctxt);
}

/**
 * @brief    update all h/w tables relevant to this object except stage 0
 *           table(s), if any, by updating packed entries with latest epoch#
 * @param[in] orig_obj    old version of the unmodified object
 * @param[in] obj_ctxt    transient state associated with this API
 * @return   SDK_RET_OK on success, failure status code on error
 */
sdk_ret_t
policy::update_config(api_base *orig_obj, obj_ctxt_t *obj_ctxt) {
    //return impl_->update_hw();
    return sdk::SDK_RET_INVALID_OP;
}

/**
 * @brief    activate the epoch in the dataplane by programming stage 0
 *           tables, if any
 * @param[in] epoch       epoch being activated
 * @param[in] api_op      api operation
 * @param[in] obj_ctxt    transient state associated with this API
 * @return   SDK_RET_OK on success, failure status code on error
 */
sdk_ret_t
policy::activate_config(oci_epoch_t epoch, api_op_t api_op,
                            obj_ctxt_t *obj_ctxt) {
    OCI_TRACE_DEBUG("Activating security policy %u config", key_.id);
    return impl_->activate_hw(this, epoch, api_op, obj_ctxt);
}

/**
 * @brief    this method is called on new object that needs to replace the
 *           old version of the object in the DBs
 * @param[in] orig_obj    old version of the object being swapped out
 * @param[in] obj_ctxt    transient state associated with this API
 * @return   SDK_RET_OK on success, failure status code on error
 */
sdk_ret_t
policy::update_db(api_base *orig_obj, obj_ctxt_t *obj_ctxt) {
    return sdk::SDK_RET_INVALID_OP;
}

/**
 * @brief add security policy to database
 * @return   SDK_RET_OK on success, failure status code on error
 */
sdk_ret_t
policy::add_to_db(void) {
    return policy_db()->policy_ht()->insert_with_key(&key_,
                                                           this, &ht_ctxt_);
}

/**
 * @brief delete security policy from database
 * @return   SDK_RET_OK on success, failure status code on error
 */
sdk_ret_t
policy::del_from_db(void) {
    policy_db()->policy_ht()->remove(&key_);
    return SDK_RET_OK;
}

/**
 * @brief    initiate delay deletion of this object
 */
sdk_ret_t
policy::delay_delete(void) {
    return delay_delete_to_slab(OCI_SLAB_ID_POLICY, this);
}
/** @} */    // end of OCI_POLICY

}    // namespace api
