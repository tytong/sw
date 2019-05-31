//------------------------------------------------------------------------------
// Copyright (c) 2019 Pensando Systems, Inc.
//------------------------------------------------------------------------------

#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <thread>
#include <iostream>
#include <fstream>
#include <math.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <google/protobuf/util/json_util.h>
#include <stdio.h>
#include <getopt.h>
#include <gtest/gtest.h>
#include "nic/sdk/include/sdk/eth.hpp"
#include "nic/sdk/include/sdk/ip.hpp"
#include "nic/apollo/agent/test/scale/utils.hpp"
#include "nic/apollo/agent/test/scale/app.hpp"
#include "nic/apollo/test/scale/test.hpp"
#include "nic/apollo/agent/svc/specs.hpp"

using std::string;

extern std::string  g_svc_endpoint_;

std::unique_ptr<pds::RouteSvc::Stub>             g_route_table_stub_;
std::unique_ptr<pds::MappingSvc::Stub>           g_mapping_stub_;
std::unique_ptr<pds::VnicSvc::Stub>              g_vnic_stub_;
std::unique_ptr<pds::SubnetSvc::Stub>            g_subnet_stub_;
std::unique_ptr<pds::VPCSvc::Stub>               g_vpc_stub_;
std::unique_ptr<pds::TunnelSvc::Stub>            g_tunnel_stub_;
std::unique_ptr<pds::DeviceSvc::Stub>            g_device_stub_;
std::unique_ptr<pds::BatchSvc::Stub>             g_batch_stub_;
std::unique_ptr<pds::SecurityPolicySvc::Stub>    g_policy_stub_;
std::unique_ptr<pds::MirrorSvc::Stub>            g_mirror_stub_;
std::unique_ptr<pds::MeterSvc::Stub>             g_meter_stub_;
std::unique_ptr<pds::TagSvc::Stub>               g_tag_stub_;

RouteTableRequest        g_route_table_req;
SecurityPolicyRequest    g_policy_req;
MappingRequest           g_mapping_req;
VnicRequest              g_vnic_req;
SubnetRequest            g_subnet_req;
VPCRequest               g_vpc_req;
VPCPeerRequest           g_vpc_peer_req;
TunnelRequest            g_tunnel_req;
MirrorSessionRequest     g_mirror_session_req;
MeterRequest             g_meter_req;
TagRequest               g_tag_req;

#define APP_GRPC_BATCH_COUNT    5000

sdk_ret_t
create_route_table_grpc (pds_route_table_spec_t *spec)
{
    ClientContext       context;
    RouteTableResponse  response;
    Status              ret_status;

    populate_route_table_request(&g_route_table_req, spec);
    if ((g_route_table_req.request_size() >= APP_GRPC_BATCH_COUNT) || !spec) {
        ret_status = g_route_table_stub_->RouteTableCreate(&context, g_route_table_req, &response);
        if (!ret_status.ok() || (response.apistatus() != types::API_STATUS_OK)) {
            printf("%s failed!\n", __FUNCTION__);
            return SDK_RET_ERR;
        }
        g_route_table_req.clear_request();
    }

    return SDK_RET_OK;
}

sdk_ret_t
create_policy_grpc (pds_policy_spec_t *spec)
{
    ClientContext             context;
    SecurityPolicyResponse    response;
    Status                    ret_status;

    if (spec != NULL) {
        pds::SecurityPolicySpec *proto_spec = g_policy_req.add_request();
        policy_api_spec_to_proto_spec(spec, proto_spec);
    }
    if ((g_policy_req.request_size() >= APP_GRPC_BATCH_COUNT) || !spec) {
        ret_status = g_policy_stub_->SecurityPolicyCreate(&context,
                                                          g_policy_req,
                                                          &response);
        if (!ret_status.ok() || (response.apistatus() != types::API_STATUS_OK)) {
            printf("%s failed!\n", __FUNCTION__);
            return SDK_RET_ERR;
        }
        g_policy_req.clear_request();
    }

    return SDK_RET_OK;
}

sdk_ret_t
create_local_mapping_grpc (pds_local_mapping_spec_t *spec)
{
    ClientContext   context;
    MappingResponse response;
    Status          ret_status;

    populate_local_mapping_request(&g_mapping_req, spec);
    if ((g_mapping_req.request_size() >= APP_GRPC_BATCH_COUNT) || !spec) {
        ret_status = g_mapping_stub_->MappingCreate(&context, g_mapping_req, &response);
        if (!ret_status.ok() || (response.apistatus() != types::API_STATUS_OK)) {
            printf("%s failed!\n", __FUNCTION__);
            return SDK_RET_ERR;
        }
        g_mapping_req.clear_request();
    }

    return SDK_RET_OK;
}

sdk_ret_t
create_remote_mapping_grpc (pds_remote_mapping_spec_t *spec)
{
    ClientContext   context;
    MappingResponse response;
    Status          ret_status;

    populate_remote_mapping_request(&g_mapping_req, spec);
    if ((g_mapping_req.request_size() >= APP_GRPC_BATCH_COUNT) || !spec) {
        ret_status = g_mapping_stub_->MappingCreate(&context, g_mapping_req, &response);
        if (!ret_status.ok() || (response.apistatus() != types::API_STATUS_OK)) {
            printf("%s failed!\n", __FUNCTION__);
            return SDK_RET_ERR;
        }
        g_mapping_req.clear_request();
    }

    return SDK_RET_OK;
}

sdk_ret_t
create_vnic_grpc (pds_vnic_spec_t *spec)
{
    ClientContext   context;
    VnicResponse    response;
    Status          ret_status;

    if (spec != NULL) {
        pds::VnicSpec *proto_spec = g_vnic_req.add_request();
        vnic_api_spec_to_proto_spec(spec, proto_spec);
    }
    if ((g_vnic_req.request_size() >= APP_GRPC_BATCH_COUNT) || !spec) {
        ret_status = g_vnic_stub_->VnicCreate(&context, g_vnic_req, &response);
        if (!ret_status.ok() || (response.apistatus() != types::API_STATUS_OK)) {
            printf("%s failed!\n", __FUNCTION__);
            return SDK_RET_ERR;
        }
        g_vnic_req.clear_request();
    }

    return SDK_RET_OK;
}

sdk_ret_t
create_subnet_grpc (pds_subnet_spec_t *spec)
{
    ClientContext   context;
    SubnetResponse  response;
    Status          ret_status;

    populate_subnet_request(&g_subnet_req, spec);
    if ((g_subnet_req.request_size() >= APP_GRPC_BATCH_COUNT) || !spec) {
        ret_status = g_subnet_stub_->SubnetCreate(&context, g_subnet_req, &response);
        if (!ret_status.ok() || (response.apistatus() != types::API_STATUS_OK)) {
            printf("%s failed!\n", __FUNCTION__);
            return SDK_RET_ERR;
        }
        g_subnet_req.clear_request();
    }

    return SDK_RET_OK;
}

sdk_ret_t
create_vpc_grpc (pds_vpc_spec_t *spec)
{
    ClientContext   context;
    VPCResponse     response;
    Status          ret_status;

    populate_vpc_request(&g_vpc_req, spec);
    if ((g_vpc_req.request_size() >= APP_GRPC_BATCH_COUNT) || !spec) {
        ret_status = g_vpc_stub_->VPCCreate(&context, g_vpc_req, &response);
        if (!ret_status.ok() || (response.apistatus() != types::API_STATUS_OK)) {
            printf("%s failed!\n", __FUNCTION__);
            return SDK_RET_ERR;
        }
        g_vpc_req.clear_request();
    }

    return SDK_RET_OK;
}

sdk_ret_t
create_vpc_peer_grpc (pds_vpc_peer_spec_t *spec)
{
    ClientContext   context;
    VPCPeerResponse response;
    Status          ret_status;

    if (spec != NULL) {
        pds::VPCPeerSpec *proto_spec = g_vpc_peer_req.add_request();
        vpc_peer_api_spec_to_proto_spec(proto_spec, spec);
    }
    if ((g_vpc_peer_req.request_size() >= APP_GRPC_BATCH_COUNT) || !spec) {
        ret_status = g_vpc_stub_->VPCPeerCreate(&context, g_vpc_peer_req, &response);
        if (!ret_status.ok() || (response.apistatus() != types::API_STATUS_OK)) {
            printf("%s failed!\n", __FUNCTION__);
            return SDK_RET_ERR;
        }
        g_vpc_peer_req.clear_request();
    }

    return SDK_RET_OK;
}

sdk_ret_t
create_tag_grpc (pds_tag_spec_t *spec)
{
    ClientContext context;
    TagResponse   response;
    Status        ret_status;

    if (spec != NULL) {
        pds::TagSpec *proto_spec = g_tag_req.add_request();
        tag_api_spec_to_proto_spec(spec, proto_spec);
    }
    if ((g_tag_req.request_size() >= APP_GRPC_BATCH_COUNT) || !spec) {
        ret_status = g_tag_stub_->TagCreate(&context, g_tag_req, &response);
        if (!ret_status.ok() || (response.apistatus() != types::API_STATUS_OK)) {
            printf("%s failed!\n", __FUNCTION__);
            return SDK_RET_ERR;
        }
        g_tag_req.clear_request();
    }
    return SDK_RET_OK;
}

sdk_ret_t
create_meter_grpc (pds_meter_spec_t *spec)
{
    ClientContext context;
    MeterResponse response;
    Status        ret_status;

    if (spec != NULL) {
        MeterSpec *proto_spec = g_meter_req.add_request();
        meter_api_spec_to_proto_spec(spec, proto_spec);
    }
    if ((g_meter_req.request_size() >= APP_GRPC_BATCH_COUNT) || !spec) {
        ret_status = g_meter_stub_->MeterCreate(&context, g_meter_req, &response);
        if (!ret_status.ok() || (response.apistatus() != types::API_STATUS_OK)) {
            printf("%s failed!\n", __FUNCTION__);
            return SDK_RET_ERR;
        }
        g_meter_req.clear_request();
    }
    return SDK_RET_OK;
}

sdk_ret_t
create_tunnel_grpc (uint32_t id, pds_tep_spec_t *spec)
{
    ClientContext   context;
    TunnelResponse  response;
    Status          ret_status;

    populate_tunnel_request(&g_tunnel_req, id, spec);
    if ((g_tunnel_req.request_size() >= APP_GRPC_BATCH_COUNT) || !spec) {
        ret_status = g_tunnel_stub_->TunnelCreate(&context, g_tunnel_req, &response);
        if (!ret_status.ok() || (response.apistatus() != types::API_STATUS_OK)) {
            printf("%s failed!\n", __FUNCTION__);
            return SDK_RET_ERR;
        }
        g_tunnel_req.clear_request();
    }

    return SDK_RET_OK;
}

sdk_ret_t
create_device_grpc (pds_device_spec_t *spec)
{
    DeviceRequest   request;
    ClientContext   context;
    DeviceResponse  response;
    Status          ret_status;

    populate_device_request(&request, spec);
    ret_status = g_device_stub_->DeviceCreate(&context, request, &response);
    if (!ret_status.ok() || (response.apistatus() != types::API_STATUS_OK)) {
        printf("%s failed!\n", __FUNCTION__);
        return SDK_RET_ERR;
    }

    return SDK_RET_OK;
}

sdk_ret_t
create_mirror_session_grpc (pds_mirror_session_spec_t *spec)
{
    ClientContext            context;
    MirrorSessionResponse    response;
    Status                   status;

    populate_mirror_session_request(&g_mirror_session_req, spec);
    if ((g_mirror_session_req.request_size() >= APP_GRPC_BATCH_COUNT) || !spec) {
        status = g_mirror_stub_->MirrorSessionCreate(&context,
                                                     g_mirror_session_req,
                                                     &response);
        if (!status.ok() || (response.apistatus() != types::API_STATUS_OK)) {
            printf("%s failed!\n", __FUNCTION__);
            return SDK_RET_ERR;
        }
        g_mirror_session_req.clear_request();
    }

    return SDK_RET_OK;
}

sdk_ret_t
batch_start_grpc (int epoch)
{
    BatchSpec           spec;
    ClientContext       start_context;
    Status              ret_status;
    pds_batch_params_t  params;
    BatchStatus         status;

    params.epoch = epoch;
    populate_batch_spec(&spec, &params);

    /* Batch start */
    ret_status = g_batch_stub_->BatchStart(&start_context, spec, &status);
    if (!ret_status.ok()) {
        printf("%s failed!\n", __FUNCTION__);
        return SDK_RET_ERR;
    }

    return SDK_RET_OK;
}


sdk_ret_t
batch_commit_grpc (void)
{
    ClientContext       commit_context;
    types::Empty        empty_spec;
    types::Empty        response;
    Status              ret_status;

    /* Batch commit */
    ret_status = g_batch_stub_->BatchCommit(&commit_context, empty_spec, &response);
    if (!ret_status.ok()) {
        printf("%s: Batch commit failed!\n", __FUNCTION__);
        return SDK_RET_ERR;
    }
    return SDK_RET_OK;
}

sdk_ret_t
test_app_push_configs (void)
{
#if 0
    BatchStatus         status;
    Status              ret_status;
    BatchSpec           spec;
    ClientContext       start_context;
    pds_batch_params_t  params;
#endif
    /* Create objects */
    create_objects();

#if 0
    // TODO: Temp change to push batch start and create flows here
    params.epoch = 0;
    populate_batch_spec(&spec, &params);

    /* Batch start */
    ret_status = g_batch_stub_->BatchStart(&start_context, spec, &status);
    if (!ret_status.ok()) {
        printf("%s failed!\n", __FUNCTION__);
        return SDK_RET_ERR;
    }
#endif

    return SDK_RET_OK;
}

void
test_app_init (void)
{
    grpc_init();
    if (g_svc_endpoint_.empty()) {
        g_svc_endpoint_ = std::string("localhost:9999");
    }
    std::shared_ptr<Channel> channel = grpc::CreateChannel(g_svc_endpoint_,
                               grpc::InsecureChannelCredentials());
    g_route_table_stub_ = pds::RouteSvc::NewStub(channel);
    g_policy_stub_ = pds::SecurityPolicySvc::NewStub(channel);
    g_mapping_stub_ = pds::MappingSvc::NewStub(channel);
    g_vnic_stub_ = pds::VnicSvc::NewStub(channel);
    g_subnet_stub_ = pds::SubnetSvc::NewStub(channel);
    g_vpc_stub_ = pds::VPCSvc::NewStub(channel);
    g_tunnel_stub_ = pds::TunnelSvc::NewStub(channel);
    g_device_stub_ = pds::DeviceSvc::NewStub(channel);
    g_batch_stub_ = pds::BatchSvc::NewStub(channel);
    g_mirror_stub_ = pds::MirrorSvc::NewStub(channel);
    g_meter_stub_ = pds::MeterSvc::NewStub(channel);
    g_tag_stub_ = pds::TagSvc::NewStub(channel);

    return;
}
