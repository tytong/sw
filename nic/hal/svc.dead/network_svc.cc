//------------------------------------------------------------------------------
// network service implementation
//------------------------------------------------------------------------------

#include "nic/include/base.h"
#include "nic/include/trace.hpp"
#include "nic/hal/svc/network_svc.hpp"
#include "nic/hal/src/nw.hpp"

Status
NetworkServiceImpl::NetworkCreate(ServerContext *context,
                                const NetworkRequestMsg *req,
                                NetworkResponseMsg *rsp)
{
    uint32_t           i, nreqs = req->request_size();
    NetworkResponse    *response;

    HAL_TRACE_DEBUG("Rcvd Network Create Request");
    if (nreqs == 0) {
        return Status(grpc::StatusCode::INVALID_ARGUMENT, "Empty Request");
    }

    hal::hal_cfg_db_open(hal::CFG_OP_WRITE);
    for (i = 0; i < nreqs; i++) {
        response = rsp->add_response();
        auto spec = req->request(i);
        hal::network_create(spec, response);
    }
    hal::hal_cfg_db_close();
    return Status::OK;
}

Status
NetworkServiceImpl::NetworkUpdate(ServerContext *context,
                                const NetworkRequestMsg *req,
                                NetworkResponseMsg *rsp)
{
    uint32_t           i, nreqs = req->request_size();
    NetworkResponse    *response;

    HAL_TRACE_DEBUG("Rcvd Network Update Request");
    if (nreqs == 0) {
        return Status(grpc::StatusCode::INVALID_ARGUMENT, "Empty Request");
    }

    hal::hal_cfg_db_open(hal::CFG_OP_WRITE);
    for (i = 0; i < nreqs; i++) {
        response = rsp->add_response();
        auto spec = req->request(i);
        hal::network_update(spec, response);
    }
    hal::hal_cfg_db_close();
    return Status::OK;
}


Status
NetworkServiceImpl::NetworkDelete(ServerContext *context,
                                const NetworkDeleteRequestMsg *req,
                                NetworkDeleteResponseMsg *rsp)
{
    uint32_t     i, nreqs = req->request_size();
    NetworkDeleteResponse    *response;

    HAL_TRACE_DEBUG("Rcvd Network Delete Request");
    if (nreqs == 0) {
        return Status(grpc::StatusCode::INVALID_ARGUMENT, "Empty Request");
    }

    hal::hal_cfg_db_open(hal::CFG_OP_WRITE);
    for (i = 0; i < nreqs; i++) {
        response = rsp->add_response();
        auto spec = req->request(i);
        hal::network_delete(spec, response);
    }
    hal::hal_cfg_db_close();
    return Status::OK;
}

Status
NetworkServiceImpl::NetworkGet(ServerContext *context,
                             const NetworkGetRequestMsg *req,
                             NetworkGetResponseMsg *rsp)
{
    uint32_t             i, nreqs = req->request_size();

    HAL_TRACE_DEBUG("Rcvd Network Get Request");
    if (nreqs == 0) {
        return Status(grpc::StatusCode::INVALID_ARGUMENT, "Empty Request");
    }

    hal::hal_cfg_db_open(hal::CFG_OP_READ);
    for (i = 0; i < nreqs; i++) {
        auto request = req->request(i);
        hal::network_get(request, rsp);
    }
    hal::hal_cfg_db_close();
    return Status::OK;
}
