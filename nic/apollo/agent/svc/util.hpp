//------------------------------------------------------------------------------
// {C} Copyright 2019 Pensando Systems Inc. All rights reserved
// -----------------------------------------------------------------------------

#ifndef __AGENT_SVC_UTIL_HPP__
#define __AGENT_SVC_UTIL_HPP__

#include "nic/sdk/include/sdk/base.hpp"
#include "nic/sdk/include/sdk/ip.hpp"
#include "nic/apollo/api/include/pds.hpp"
#include "nic/apollo/agent/trace.hpp"
#include "gen/proto/types.pb.h"

//----------------------------------------------------------------------------
// convert IP address spec in proto to ip_addr
//----------------------------------------------------------------------------
static inline void
ipaddr_proto_spec_to_api_spec (ip_addr_t *out_ipaddr,
                               const types::IPAddress &in_ipaddr)
{
    memset(out_ipaddr, 0, sizeof(ip_addr_t));
    if (in_ipaddr.af() == types::IP_AF_INET) {
        out_ipaddr->af = IP_AF_IPV4;
        out_ipaddr->addr.v4_addr = in_ipaddr.v4addr();
    } else if (in_ipaddr.af() == types::IP_AF_INET6) {
        out_ipaddr->af = IP_AF_IPV6;
        memcpy(out_ipaddr->addr.v6_addr.addr8,
               in_ipaddr.v6addr().c_str(),
               IP6_ADDR8_LEN);
    } else {
        return;
    }

    return;
}

static inline sdk_ret_t
ippfx_proto_spec_to_api_spec (ip_prefix_t *ip_pfx,
                              const types::IPPrefix& in_ippfx)
{
    ip_pfx->len = in_ippfx.len();
    if (((in_ippfx.addr().af() == types::IP_AF_INET) &&
             (ip_pfx->len > 32)) ||
        ((in_ippfx.addr().af() == types::IP_AF_INET6) &&
             (ip_pfx->len > 128))) {
        return sdk::SDK_RET_INVALID_ARG;
    } else {
        ipaddr_proto_spec_to_api_spec(&ip_pfx->addr, in_ippfx.addr());
    }
    return sdk::SDK_RET_OK;
}

static inline sdk_ret_t
ipv4pfx_proto_spec_to_api_spec (ipv4_prefix_t *ip_pfx,
                                const types::IPPrefix& in_ippfx)
{
    ip_pfx->len = in_ippfx.len();
    if ((in_ippfx.addr().af() == types::IP_AF_INET) &&
             (ip_pfx->len > 32)) {
        return sdk::SDK_RET_INVALID_ARG;
    } else {
        ip_pfx->v4_addr = in_ippfx.addr().v4addr();
    }
    return sdk::SDK_RET_OK;
}

static inline sdk_ret_t
iprange_proto_spec_to_api_spec (ipvx_range_t *ip_range,
                                const types::AddressRange& in_iprange)
{
    if (in_iprange.has_ipv4range()) {
        ip_range->af = IP_AF_IPV4;
        ip_range->ip_lo.v4_addr = in_iprange.ipv4range().low().v4addr();
        ip_range->ip_hi.v4_addr = in_iprange.ipv4range().high().v4addr();
    } else if (in_iprange.has_ipv6range()) {
        ip_range->af = IP_AF_IPV6;
        memcpy(ip_range->ip_lo.v6_addr.addr8,
               in_iprange.ipv6range().low().v6addr().c_str(),
               IP6_ADDR8_LEN);
        memcpy(ip_range->ip_hi.v6_addr.addr8,
               in_iprange.ipv6range().high().v6addr().c_str(),
               IP6_ADDR8_LEN);
    } else {
        return sdk::SDK_RET_INVALID_ARG;
    }
    return sdk::SDK_RET_OK;
}
//----------------------------------------------------------------------------
// convert ip_addr_t to IP address proto spec
//----------------------------------------------------------------------------
static inline void
ipaddr_api_spec_to_proto_spec (types::IPAddress *out_ipaddr,
                               const ip_addr_t *in_ipaddr)
{
    if (in_ipaddr->af == IP_AF_IPV4) {
        out_ipaddr->set_af(types::IP_AF_INET);
        out_ipaddr->set_v4addr(in_ipaddr->addr.v4_addr);
    } else if (in_ipaddr->af == IP_AF_IPV6) {
        out_ipaddr->set_af(types::IP_AF_INET6);
        out_ipaddr->set_v6addr(
                    std::string((const char *)&in_ipaddr->addr.v6_addr.addr8,
                                IP6_ADDR8_LEN));
    }
    return;
}

static inline void
ipv4addr_api_spec_to_proto_spec (types::IPAddress *out_ipaddr,
                                 const ipv4_addr_t *in_ipaddr)
{
    out_ipaddr->set_af(types::IP_AF_INET);
    out_ipaddr->set_v4addr(*in_ipaddr);
}

static inline void
ipv6addr_api_spec_to_proto_spec (types::IPAddress *out_ipaddr,
                                 const ipv6_addr_t *in_ipaddr)
{
    out_ipaddr->set_af(types::IP_AF_INET6);
    out_ipaddr->set_v6addr((const char *)&(in_ipaddr->addr8),
                           IP6_ADDR8_LEN);
}

static inline sdk_ret_t
ippfx_api_spec_to_proto_spec (types::IPPrefix *out_ippfx,
                              const ip_prefix_t *in_ippfx)
{
    out_ippfx->set_len(in_ippfx->len);
    ipaddr_api_spec_to_proto_spec(out_ippfx->mutable_addr(), &in_ippfx->addr);
    return sdk::SDK_RET_OK;
}

static inline sdk_ret_t
ipv4pfx_api_spec_to_proto_spec (types::IPPrefix *out_ippfx,
                                const ipv4_prefix_t *in_ippfx)
{
    auto out_addr = out_ippfx->mutable_addr();
    out_ippfx->set_len(in_ippfx->len);
    out_addr->set_af(types::IP_AF_INET);
    out_addr->set_v4addr(in_ippfx->v4_addr);
    return sdk::SDK_RET_OK;
}

static inline sdk_ret_t
iprange_api_spec_to_proto_spec (types::AddressRange *out_iprange,
                                const ipvx_range_t *in_iprange)
{
    switch (in_iprange->af) {
    case IP_AF_IPV4:
        {
            auto out_range = out_iprange->mutable_ipv4range();
            ipv4addr_api_spec_to_proto_spec(out_range->mutable_low(),
                                            &in_iprange->ip_lo.v4_addr);
            ipv4addr_api_spec_to_proto_spec(out_range->mutable_high(),
                                            &in_iprange->ip_hi.v4_addr);
        }
        break;
    case IP_AF_IPV6:
        {
            auto out_range = out_iprange->mutable_ipv4range();
            ipv6addr_api_spec_to_proto_spec(out_range->mutable_low(),
                                            &in_iprange->ip_lo.v6_addr);
            ipv6addr_api_spec_to_proto_spec(out_range->mutable_high(),
                                            &in_iprange->ip_hi.v6_addr);
        }
        break;
    default:
        return SDK_RET_INVALID_ARG;
    }
    return sdk::SDK_RET_OK;
}

static inline sdk_ret_t
pds_encap_to_proto_encap (types::Encap *proto_encap,
                          const pds_encap_t *pds_encap)
{
    switch (pds_encap->type) {
    case PDS_ENCAP_TYPE_NONE:
        proto_encap->set_type(types::ENCAP_TYPE_NONE);
        break;

    case PDS_ENCAP_TYPE_DOT1Q:
        proto_encap->set_type(types::ENCAP_TYPE_DOT1Q);
        proto_encap->mutable_value()->set_vlanid(pds_encap->val.vlan_tag);
        break;

    case PDS_ENCAP_TYPE_QINQ:
        proto_encap->set_type(types::ENCAP_TYPE_QINQ);
        proto_encap->mutable_value()->mutable_qinqtag()->
            set_ctag(pds_encap->val.qinq_tag.c_tag);
        proto_encap->mutable_value()->mutable_qinqtag()->
            set_stag(pds_encap->val.qinq_tag.s_tag);
        break;

    case PDS_ENCAP_TYPE_MPLSoUDP:
        proto_encap->set_type(types::ENCAP_TYPE_MPLSoUDP);
        proto_encap->mutable_value()->set_mplstag(pds_encap->val.mpls_tag);
        break;

    case PDS_ENCAP_TYPE_VXLAN:
        proto_encap->set_type(types::ENCAP_TYPE_VXLAN);
        proto_encap->mutable_value()->set_vnid(pds_encap->val.vnid);
        break;

    default:
        return SDK_RET_INVALID_ARG;
    }

    return SDK_RET_OK;
}

static inline pds_encap_t
proto_encap_to_pds_encap (types::Encap encap)
{
    pds_encap_t    pds_encap;

    memset(&pds_encap, 0, sizeof(pds_encap));
    switch (encap.type()) {
    case types::ENCAP_TYPE_NONE:
        pds_encap.type = PDS_ENCAP_TYPE_NONE;
        break;

    case types::ENCAP_TYPE_DOT1Q:
        pds_encap.type = PDS_ENCAP_TYPE_DOT1Q;
        pds_encap.val.vlan_tag = encap.value().vlanid();
        break;

    case types::ENCAP_TYPE_QINQ:
        pds_encap.type = PDS_ENCAP_TYPE_QINQ;
        pds_encap.val.qinq_tag.c_tag = encap.value().qinqtag().ctag();
        pds_encap.val.qinq_tag.s_tag = encap.value().qinqtag().stag();
        break;

    case types::ENCAP_TYPE_MPLSoUDP:
        pds_encap.type = PDS_ENCAP_TYPE_MPLSoUDP;
        pds_encap.val.mpls_tag = encap.value().mplstag();
        break;

    case types::ENCAP_TYPE_VXLAN:
        pds_encap.type = PDS_ENCAP_TYPE_VXLAN;
        pds_encap.val.vnid = encap.value().vnid();
        break;
    default:
        break;
    }
    return pds_encap;
}

static inline types::ApiStatus
sdk_ret_to_api_status (sdk_ret_t ret)
{
    switch (ret) {
    case sdk::SDK_RET_OK:
        return types::ApiStatus::API_STATUS_OK;

    case sdk::SDK_RET_OOM:
        return types::ApiStatus::API_STATUS_OUT_OF_MEM;

    case sdk::SDK_RET_INVALID_ARG:
        return types::ApiStatus::API_STATUS_INVALID_ARG;

    case sdk::SDK_RET_ENTRY_NOT_FOUND:
        return types::ApiStatus::API_STATUS_NOT_FOUND;

    case sdk::SDK_RET_ENTRY_EXISTS:
        return types::ApiStatus::API_STATUS_EXISTS_ALREADY;

    case sdk::SDK_RET_NO_RESOURCE:
        return types::ApiStatus::API_STATUS_OUT_OF_RESOURCE;

    default:
        return types::ApiStatus::API_STATUS_ERR;
    }
}

static inline sdk_ret_t
pds_af_proto_spec_to_api_spec (uint8_t *af, const types::IPAF &addrfamily)
{
    if (addrfamily == types::IP_AF_INET) {
        *af = IP_AF_IPV4;
    } else if (addrfamily == types::IP_AF_INET6) {
        *af = IP_AF_IPV6;
    } else {
        PDS_TRACE_ERR("IP_AF_NONE passed in proto");
        return SDK_RET_INVALID_ARG;
    }
    return SDK_RET_OK;
}

static inline types::IPAF
pds_af_api_spec_to_proto_spec (uint8_t af)
{
    if (af == IP_AF_IPV4) {
        return types::IP_AF_INET;
    } else if (af == IP_AF_IPV6) {
        return types::IP_AF_INET6;
    } else {
        return types::IP_AF_NONE;
    }
}

#endif    // __AGENT_SVC_UTIL_HPP__
