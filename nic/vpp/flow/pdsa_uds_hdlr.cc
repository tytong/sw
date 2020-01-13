//
//  {C} Copyright 2019 Pensando Systems Inc. All rights reserved.
//

#include "pdsa_uds_hdlr.h"
#include "nic/vpp/infra/ipc/uds_internal.h"
#include <gen/proto/types.pb.h>
#include <gen/proto/debug.pb.h>
#include <gen/proto/session.pb.h>
#include <nic/sdk/include/sdk/table.hpp>
#include <gen/p4gen/p4/include/ftl.h>
#include <lib/table/ftl/ftl_base.hpp>

static void
ftlv4_entry_iter_cb(sdk::table::sdk_table_api_params_t *params)
{
    ipv4_flow_hash_entry_t *hwentry = (ipv4_flow_hash_entry_t *) params->entry;
    int *fd = (int *)params->cbdata;

    if (hwentry->get_entry_valid()) {
        pds::FlowMsg flow_msg = pds::FlowMsg();
        pds::Flow *flow = flow_msg.mutable_flowentry();
        
        flow->mutable_key()->set_srcport(hwentry->get_key_metadata_sport());
        flow->mutable_key()->set_dstport(hwentry->get_key_metadata_dport());
        flow->mutable_key()->set_ipproto(hwentry->get_key_metadata_proto());
        flow->mutable_key()->mutable_srcaddr()->set_af(types::IPAF::IP_AF_INET);
        flow->mutable_key()->mutable_srcaddr()->set_v4addr(
                hwentry->get_key_metadata_ipv4_src());
        flow->mutable_key()->mutable_dstaddr()->set_af(types::IPAF::IP_AF_INET);
        flow->mutable_key()->mutable_dstaddr()->set_v4addr(
                hwentry->get_key_metadata_ipv4_dst());
        flow->mutable_key()->set_ipproto(hwentry->get_key_metadata_proto());

        flow->set_sessionidx(hwentry->get_session_index());
        flow->set_flowrole(hwentry->get_flow_role());
        // Epoch is not present in apollo pipeline so commenting out for now
        // flow->set_epoch(hwentry->get_epoch());

        flow_msg.set_flowentrycount(1);
        flow_msg.SerializeToFileDescriptor(*fd);
    }
}

static void
ftlv6_entry_iter_cb(sdk::table::sdk_table_api_params_t *params)
{
    flow_hash_entry_t *hwentry = (flow_hash_entry_t *) params->entry;
    int *fd = (int *)params->cbdata;

    if (hwentry->get_entry_valid()) {
        pds::FlowMsg flow_msg = pds::FlowMsg();
        pds::Flow *flow = flow_msg.mutable_flowentry();

        flow->mutable_key()->set_srcport(hwentry->get_key_metadata_sport());
        flow->mutable_key()->set_dstport(hwentry->get_key_metadata_dport());
        flow->mutable_key()->set_ipproto(hwentry->get_key_metadata_proto());
        flow->mutable_key()->mutable_srcaddr()->set_af(
                types::IPAF::IP_AF_INET6);
        hwentry->get_key_metadata_src(
                (uint8_t *)flow->mutable_key()->mutable_srcaddr());
        flow->mutable_key()->mutable_dstaddr()->set_af(
                types::IPAF::IP_AF_INET6);
        hwentry->get_key_metadata_dst(
                (uint8_t *)flow->mutable_key()->mutable_dstaddr());
        flow->mutable_key()->set_ipproto(hwentry->get_key_metadata_proto());

        flow->set_sessionidx(hwentry->get_session_index());
        flow->set_flowrole(hwentry->get_flow_role());
        // Epoch is not present in apollo pipeline so commenting out for now
        // flow->set_epoch(hwentry->get_epoch());

        flow_msg.set_flowentrycount(1);
        flow_msg.SerializeToFileDescriptor(*fd);
    }
}

// Callback to dump flow entries via UDS
static void
vpp_uds_flow_dump(int fd) 
{
    sdk::table::sdk_table_api_params_t params = {0};
    sdk::table::ftl_base *table4 = 
        (sdk::table::ftl_base *)pds_flow_get_table4();
    sdk::table::ftl_base *table6 =
        (sdk::table::ftl_base *)pds_flow_get_table6();

    // Iterate over v4 table and then v6
    params.itercb = ftlv4_entry_iter_cb;
    params.cbdata = &fd;
    params.force_hwread = false;
    params.entry_size = ipv4_flow_hash_entry_t::entry_size();
    table4->iterate(&params);

    params.itercb = ftlv6_entry_iter_cb;
    params.cbdata = &fd;
    params.force_hwread = false;
    params.entry_size = flow_hash_entry_t::entry_size();
    table6->iterate(&params);

    // Send 0 entries to indicate end of dump
    pds::FlowMsg flow_msg = pds::FlowMsg();
    flow_msg.set_flowentrycount(0);
    flow_msg.SerializeToFileDescriptor(fd);
}

// initializes callbacks for flow dump
void
pds_flow_dump_init(void) 
{
    vpp_uds_register_cb(VPP_UDS_FLOW_DUMP, vpp_uds_flow_dump);
}