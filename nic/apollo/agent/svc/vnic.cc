//------------------------------------------------------------------------------
// {C} Copyright 2019 Pensando Systems Inc. All rights reserved
// -----------------------------------------------------------------------------

#include "nic/apollo/api/include/pds_vnic.hpp"
#include "nic/apollo/agent/core/state.hpp"
#include "nic/apollo/agent/core/vnic.hpp"
#include "nic/apollo/agent/svc/util.hpp"
#include "nic/apollo/agent/svc/vnic.hpp"

// Build VNIC API spec from proto buf spec
static inline void
pds_agent_vnic_api_spec_fill (const pds::VnicSpec &proto_spec,
                              pds_vnic_spec_t *api_spec)
{
    api_spec->vcn.id = proto_spec.pcnid();
    api_spec->subnet.id = proto_spec.subnetid();
    api_spec->key.id = proto_spec.vnicid();
    api_spec->wire_vlan = proto_spec.wirevlan();
    api_spec->fabric_encap = proto_encap_to_pds_encap(proto_spec.encap());
    MAC_UINT64_TO_ADDR(api_spec->mac_addr, proto_spec.macaddress());
    api_spec->rsc_pool_id = proto_spec.resourcepoolid();
    api_spec->src_dst_check = proto_spec.sourceguardenable();
}

Status
VnicSvcImpl::VnicCreate(ServerContext *context, const pds::VnicRequest *proto_req,
                        pds::VnicResponse *proto_rsp) {
    sdk_ret_t ret;
    pds_vnic_key_t key;
    pds_vnic_spec_t *api_spec;

    if (proto_req == NULL) {
        proto_rsp->set_apistatus(types::ApiStatus::API_STATUS_INVALID_ARG);
        return Status::OK;
    }
    for (int i = 0; i < proto_req->request_size(); i ++) {
        api_spec = (pds_vnic_spec_t *)
                    core::agent_state::state()->vnic_slab()->alloc();
        if (api_spec == NULL) {
            proto_rsp->set_apistatus(types::ApiStatus::API_STATUS_OUT_OF_MEM);
            break;
        }
        auto request = proto_req->request(i);
        memset(&key, 0, sizeof(pds_vnic_key_t));
        key.id = request.vnicid();
        pds_agent_vnic_api_spec_fill(request, api_spec);
        ret = core::vnic_create(&key, api_spec);
        proto_rsp->set_apistatus(sdk_ret_to_api_status(ret));
        if (ret != sdk::SDK_RET_OK) {
            break;
        }
    }
    return Status::OK;
}

Status
VnicSvcImpl::VnicDelete(ServerContext *context,
                        const pds::VnicDeleteRequest *proto_req,
                        pds::VnicDeleteResponse *proto_rsp) {
    sdk_ret_t ret;
    pds_vnic_key_t key;

    if (proto_req == NULL) {
        proto_rsp->add_apistatus(types::ApiStatus::API_STATUS_INVALID_ARG);
        return Status::OK;
    }
    for (int i = 0; i < proto_req->vnicid_size(); i++) {
        memset(&key, 0, sizeof(pds_vnic_key_t));
        key.id = proto_req->vnicid(i);
        ret = core::vnic_delete(&key);
        proto_rsp->add_apistatus(sdk_ret_to_api_status(ret));
    }
    return Status::OK;
}
