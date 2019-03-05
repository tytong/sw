//-----------------------------------------------------------------------------
// {C} Copyright 2019 Pensando Systems Inc. All rights reserved
//-----------------------------------------------------------------------------
#include <string>
#include <memory>
#include <iostream>
#include <grpc++/grpc++.h>
#include "devapi_iris_types.hpp"
#include "hal_grpc.hpp"
#include "devapi_mem.hpp"
#include "devapi_object.hpp"
#include "platform/src/lib/nicmgr/include/logger.hpp"

namespace iris {

hal_grpc *hal_grpc::hal_ = NULL;

hal_grpc *
hal_grpc::factory()
{
    sdk_ret_t ret = SDK_RET_OK;
    void *mem     = NULL;
    hal_grpc *hal = NULL;

    mem = (hal_grpc *)DEVAPI_CALLOC(DEVAPI_MEM_ALLOC_DEVAPI_HAL_GRPC,
                                    sizeof(hal_grpc));
    if (mem) {
        hal = new (mem) hal_grpc();
        ret = hal->init_();
        if (ret != SDK_RET_OK) {
            hal->~hal_grpc();
            DEVAPI_FREE(DEVAPI_MEM_ALLOC_DEVAPI_HAL_GRPC, mem);
            hal = NULL;
        }
    }
    return hal;
}

sdk_ret_t
hal_grpc::init_()
{
    sdk_ret_t ret = SDK_RET_OK;

    hal_grpc::hal_ = this;

    ret = connect_hal();
    if (ret != SDK_RET_OK) {
        NIC_LOG_DEBUG("Unable to connect to hal: ret: {}", ret);
        goto end;
    }
    // Enables all objects to do gRPC calls
    devapi_object::populate_hal_grpc();
end:
    return ret;
}

sdk_ret_t
hal_grpc::connect_hal()
{
    std::string svc_url;

    if (getenv("HAL_SOCK_PATH")) {
        svc_url = std::string("unix:") + std::getenv("HAL_SOCK_PATH") + "halsock";
    } else if (getenv("HAL_GRPC_PORT")) {
        svc_url = std::string("localhost:") + getenv("HAL_GRPC_PORT");
    } else {
        svc_url = std::string("localhost:50054");
    }

    channel = grpc::CreateChannel(svc_url, grpc::InsecureChannelCredentials());

    // cout << "[INFO] Waiting for HAL to be ready ..." << endl;
    NIC_LOG_DEBUG("Waiting for HAL to be ready at: {}", svc_url);
    auto state = channel->GetState(true);
    while (state != GRPC_CHANNEL_READY) {
        // Wait for State change or deadline
        channel->WaitForStateChange(state, gpr_time_from_seconds(1, GPR_TIMESPAN));
        state = channel->GetState(true);
        // cout << "[INFO] Connecting to HAL, channel status = " << channel->GetState(true) << endl;
    }

    vrf_stub_ = vrf::Vrf::NewStub(channel);
    interface_stub_ = intf::Interface::NewStub(channel);
    endpoint_stub_ = endpoint::Endpoint::NewStub(channel);
    l2segment_stub_ = l2segment::L2Segment::NewStub(channel);
    multicast_stub_ = multicast::Multicast::NewStub(channel);
    rdma_stub_ = rdma::Rdma::NewStub(channel);
    qos_stub_ = qos::QOS::NewStub(channel);

    return SDK_RET_OK;
}

void
hal_grpc::destroy(hal_grpc *hal)
{
    hal->~hal_grpc();
    DEVAPI_FREE(DEVAPI_MEM_ALLOC_DEVAPI_HAL_GRPC, hal);
}

hal_grpc *
hal_grpc::get_hal_grpc()
{
    return hal_;
}

#define SET_TIMEOUT()                                                       \
    std::chrono::system_clock::time_point deadline =                        \
        std::chrono::system_clock::now() + seconds(HAL_GRPC_API_TIMEOUT);   \
    context.set_deadline(deadline);

#define HAL_CREATE_API(obj_api, obj_class, pkg)                             \
    Status                                                                  \
    hal_grpc::obj_api ## _create (obj_class ## RequestMsg& req_msg,    \
                                       obj_class ## ResponseMsg& rsp_msg)   \
    {                                                                       \
        grpc::ClientContext         context;                                \
        grpc::Status                status;                                 \
        SET_TIMEOUT();                                                      \
        status = pkg ## _stub_->obj_class ## Create(&context, req_msg, &rsp_msg);   \
        return status;                                                      \
    }

#define HAL_UPDATE_API(obj_api, obj_class, pkg)                             \
    Status                                                                  \
    hal_grpc::obj_api ## _update (obj_class ## RequestMsg& req_msg,    \
                                       obj_class ## ResponseMsg& rsp_msg)   \
    {                                                                       \
        grpc::ClientContext         context;                                \
        grpc::Status                status;                                 \
        SET_TIMEOUT();                                                      \
        status = pkg ## _stub_->obj_class ## Update(&context, req_msg, &rsp_msg);   \
        return status;                                                      \
    }

#define HAL_DELETE_API(obj_api, obj_class, pkg)                             \
    Status                                                                  \
    hal_grpc::obj_api ## _delete (obj_class ## DeleteRequestMsg& req_msg,    \
                                       obj_class ## DeleteResponseMsg& rsp_msg)   \
    {                                                                       \
        grpc::ClientContext         context;                                \
        grpc::Status                status;                                 \
        SET_TIMEOUT();                                                      \
        status = pkg ## _stub_->obj_class ## Delete(&context, req_msg, &rsp_msg);   \
        return status;                                                      \
    }

#define HAL_GET_API(obj_api, obj_class, pkg)                                \
    Status                                                                  \
    hal_grpc::obj_api ## _get (obj_class ## GetRequestMsg& req_msg,    \
                                    obj_class ## GetResponseMsg& rsp_msg)   \
    {                                                                       \
        grpc::ClientContext         context;                                \
        grpc::Status                status;                                 \
        SET_TIMEOUT();                                                      \
        status = pkg ## _stub_->obj_class ## Get(&context, req_msg, &rsp_msg);   \
        return status;                                                      \
    }

//-----------------------------------------------------------------------------
// Vrf Create to HAL
//-----------------------------------------------------------------------------
Status
hal_grpc::vrf_create (VrfRequestMsg& req_msg, VrfResponseMsg& rsp_msg)
{
    grpc::ClientContext         context;
    grpc::Status                status;

    SET_TIMEOUT();
    status = vrf_stub_->VrfCreate(&context, req_msg, &rsp_msg);

    return status;
}

//-----------------------------------------------------------------------------
// Vrf Delete to HAL
//-----------------------------------------------------------------------------
Status
hal_grpc::vrf_delete (VrfDeleteRequestMsg& req_msg,
                           VrfDeleteResponseMsg& rsp_msg)
{
    grpc::ClientContext         context;
    grpc::Status                status;

    SET_TIMEOUT();
    status = vrf_stub_->VrfDelete(&context, req_msg, &rsp_msg);

    return status;
}

//-----------------------------------------------------------------------------
// Vrf Update to HAL
//-----------------------------------------------------------------------------
Status
hal_grpc::vrf_update (VrfRequestMsg& req_msg, VrfResponseMsg& rsp_msg)
{
    grpc::ClientContext         context;
    grpc::Status                status;

    SET_TIMEOUT();
    status = vrf_stub_->VrfUpdate(&context, req_msg, &rsp_msg);

    return status;
}

//-----------------------------------------------------------------------------
// Vrf Get to HAL
//-----------------------------------------------------------------------------
Status
hal_grpc::vrf_get (VrfGetRequestMsg& req_msg,
                        VrfGetResponseMsg& rsp_msg)
{
    grpc::ClientContext         context;
    grpc::Status                status;

    SET_TIMEOUT();
    status = vrf_stub_->VrfGet(&context, req_msg, &rsp_msg);

    return status;
}

// HalL2Segment Calls
HAL_CREATE_API(l2segment, L2Segment, l2segment);
HAL_UPDATE_API(l2segment, L2Segment, l2segment);
HAL_DELETE_API(l2segment, L2Segment, l2segment);
HAL_GET_API(l2segment, L2Segment, l2segment);

// HalEndpoint Calls
HAL_CREATE_API(endpoint, Endpoint, endpoint);
HAL_DELETE_API(endpoint, Endpoint, endpoint);
HAL_GET_API(endpoint, Endpoint, endpoint);

// TODO: Cleanup once EP's Update request change
// HAL_DELETE_API(endpoint, HalEndpoint);
//-----------------------------------------------------------------------------
// HalEndpoint Update to HAL
//-----------------------------------------------------------------------------
Status
hal_grpc::endpoint_update (EndpointUpdateRequestMsg& req_msg,
                                EndpointUpdateResponseMsg& rsp_msg)
{
    grpc::ClientContext         context;
    grpc::Status                status;

    SET_TIMEOUT();
    status = endpoint_stub_->EndpointUpdate(&context, req_msg, &rsp_msg);

    return status;
}

// Interface Calls
HAL_CREATE_API(interface, Interface, interface);
HAL_UPDATE_API(interface, Interface, interface);
HAL_DELETE_API(interface, Interface, interface);
HAL_GET_API(interface, Interface, interface);

// Lif Calls
HAL_CREATE_API(lif, Lif, interface);
HAL_UPDATE_API(lif, Lif, interface);
HAL_DELETE_API(lif, Lif, interface);
HAL_GET_API(lif, Lif, interface);

// HalMulticast Calls
HAL_CREATE_API(multicast, MulticastEntry, multicast);
HAL_UPDATE_API(multicast, MulticastEntry, multicast);
HAL_DELETE_API(multicast, MulticastEntry, multicast);
HAL_GET_API(multicast, MulticastEntry, multicast);

// Filter Calls
HAL_CREATE_API(filter, Filter, endpoint);
HAL_DELETE_API(filter, Filter, endpoint);
HAL_GET_API(filter, Filter, endpoint);

// Qos Calls
HAL_GET_API(qos_class, QosClass, qos);

} // namespace iris
