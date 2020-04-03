//
// {C} Copyright 2020 Pensando Systems Inc. All rights reserved
//
//----------------------------------------------------------------------------
#include <stdio.h>
#include <unistd.h>
#include <grpc++/grpc++.h>
#include "nic/sdk/include/sdk/base.hpp"
#include "gen/proto/upgrade.grpc.pb.h"
#include "nic/apollo/include/globals.hpp"

using std::string;
using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

static std::string g_upg_svc_endpoint_;
static std::unique_ptr<pds::UpgSvc::Stub>  g_upg_svc_stub_;


static sdk_ret_t
send_upg_grpc (void)
{
    ClientContext       context;
    pds::UpgRequest     req;
    pds::UpgResponse    rsp;
    Status              ret_status;
    pds::UpgSpec        *spec = req.mutable_request();

    spec->set_reqtype(pds::UpgReqType::UPG_REQ_START);
    spec->set_mode(pds::UpgMode::UPG_MODE_GRACEFUL);

    ret_status = g_upg_svc_stub_->UpgradeRequest(&context, req, &rsp);
    printf("Upgrade response status ok %u, rsp %u, rspmsg %s\n", ret_status.ok(),
           rsp.status(),
           rsp.mutable_statusmsg()->empty() ? "success" :
           rsp.mutable_statusmsg()->c_str());
    if (!ret_status.ok()) {
        printf("%s failed!\n", __FUNCTION__);
        return SDK_RET_ERR;
    }
    return SDK_RET_OK;
}

static void
svc_init (void)
{
    grpc_init();
    if (g_upg_svc_endpoint_.empty()) {
        g_upg_svc_endpoint_ = std::string("localhost:") +
            std::to_string(PDS_GRPC_PORT_UPGMGR);
    }
    std::shared_ptr<Channel> channel =
        grpc::CreateChannel(g_upg_svc_endpoint_,
                            grpc::InsecureChannelCredentials());
    g_upg_svc_stub_ = pds::UpgSvc::NewStub(channel);

}

int
main (int argc, char **argv)
{
    svc_init();
    send_upg_grpc();
}
