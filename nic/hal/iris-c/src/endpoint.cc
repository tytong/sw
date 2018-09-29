
#include <iostream>
#include <iomanip>

#include "gen/proto/kh.grpc.pb.h"
#include "gen/proto/types.grpc.pb.h"

#include "nic/hal/iris-c/include/endpoint.hpp"
#include "nic/hal/iris-c/include/print.hpp"

using namespace std;

std::map<ep_key_t, Endpoint*> Endpoint::ep_db;

Endpoint *
Endpoint::Factory(L2Segment *l2seg, mac_t mac, Enic *enic)
{
    ep_key_t ep_key(l2seg, mac);

    Endpoint *ep = new Endpoint(l2seg, mac, enic);

    // Store in DB
    ep_db[ep_key] = ep;
    return ep;
}

void
Endpoint::Destroy(Endpoint *ep)
{
    ep_key_t ep_key(ep->GetL2Seg(), ep->GetMac());

    if (ep) {
        ep->~Endpoint();
    }

    // Remove from DB
    ep_db.erase(ep_key);
}

Endpoint *
Endpoint::Lookup(L2Segment *l2seg, mac_t mac)
{
    ep_key_t key(l2seg, mac);

    if (ep_db.find(key) != ep_db.cend()) {
        return ep_db[key];
    } else {
        return NULL;
    }
}

Endpoint::Endpoint(L2Segment *l2seg, mac_t mac, Enic *enic)
{
    grpc::ClientContext             context;
    grpc::Status                    status;

    endpoint::EndpointSpec          *req;
    endpoint::EndpointResponse      rsp;
    endpoint::EndpointRequestMsg    req_msg;
    endpoint::EndpointResponseMsg   rsp_msg;

    HAL_TRACE_DEBUG("EP create: l2seg: {}, mac: {}, enic: {}",
                    l2seg->GetId(), macaddr2str(mac), enic->GetId());

    this->mac = mac;
    this->l2seg = l2seg;
    this->enic = enic;

    req = req_msg.add_request();
    req->mutable_vrf_key_handle()->set_vrf_id(l2seg->GetVrf()->GetId());
    req->mutable_key_or_handle()->mutable_endpoint_key()->mutable_l2_key()->set_mac_address(mac);
    req->mutable_key_or_handle()->mutable_endpoint_key()->mutable_l2_key()->mutable_l2segment_key_handle()->set_l2segment_handle(l2seg->GetHandle());
    req->mutable_endpoint_attrs()->mutable_interface_key_handle()->set_interface_id(enic->GetId());
    status = hal->endpoint_create(req_msg, rsp_msg);
    if (status.ok()) {
        rsp = rsp_msg.response(0);
        if (rsp.api_status() != types::API_STATUS_OK) {
            HAL_TRACE_ERR("Failed to create EP L2seg: {}, Mac: {}. err: {}",
                          l2seg->GetId(), macaddr2str(mac),
                          rsp.api_status());
        } else {
            HAL_TRACE_DEBUG("Created EP L2seg: {}, Mac: {}", l2seg->GetId(), macaddr2str(mac));
            handle = rsp.endpoint_status().endpoint_handle();
        }
    } else {
        HAL_TRACE_ERR("Failed to create EP L2seg: {}, Mac: {}. err: {}, msg: {}", l2seg->GetId(),
                      macaddr2str(mac), status.error_code(), status.error_message());
    }
}

Endpoint::~Endpoint()
{
    grpc::ClientContext                   context;
    grpc::Status                          status;

    endpoint::EndpointDeleteRequest       *req;
    endpoint::EndpointDeleteResponse      rsp;
    endpoint::EndpointDeleteRequestMsg    req_msg;
    endpoint::EndpointDeleteResponseMsg   rsp_msg;

    HAL_TRACE_DEBUG("EP delete: l2seg: {}, mac: {}, enic: {}",
                    l2seg->GetId(), macaddr2str(mac), enic->GetId());

    req = req_msg.add_request();
    req->mutable_vrf_key_handle()->set_vrf_id(l2seg->GetVrf()->GetId());
    req->mutable_key_or_handle()->mutable_endpoint_key()->mutable_l2_key()->set_mac_address(mac);
    req->mutable_key_or_handle()->mutable_endpoint_key()->mutable_l2_key()->mutable_l2segment_key_handle()->set_l2segment_handle(l2seg->GetHandle());

    status = hal->endpoint_delete(req_msg, rsp_msg);
    if (status.ok()) {
        rsp = rsp_msg.response(0);
        if (rsp.api_status() != types::API_STATUS_OK) {
            HAL_TRACE_ERR("Failed to delete EP L2seg: {}, Mac: {}. err: {}", l2seg->GetId(), macaddr2str(mac),
                          rsp.api_status());
        } else {
            HAL_TRACE_DEBUG("Delete EP L2seg: {}, Mac: {}", l2seg->GetId(), macaddr2str(mac));
        }
    } else {
        HAL_TRACE_ERR("Failed to delete EP L2seg: {}, Mac: {}. err: {}, msg: {}", l2seg->GetId(),
                      macaddr2str(mac), status.error_code(), status.error_message());
    }
}

L2Segment *
Endpoint::GetL2Seg()
{
    return l2seg;
}
mac_t
Endpoint::GetMac()
{
    return mac;
}
