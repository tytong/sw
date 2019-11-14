//
// {C} Copyright 2019 Pensando Systems Inc. All rights reserved
//
//----------------------------------------------------------------------------

#include "nic/sdk/include/sdk/ip.hpp"
#include "nic/apollo/test/utils/policy.hpp"
#include "nic/apollo/test/utils/utils.hpp"

namespace api_test {

#define MAX_RANGE_RULES_V6 5
#define MAX_RANGE_RULES_V4 20

//----------------------------------------------------------------------------
// Policy feeder class routines
//----------------------------------------------------------------------------

void
policy_feeder::init(pds_policy_key_t key,
                    uint16_t stateful_rules,
                    rule_dir_t direction,
                    policy_type_t type,
                    uint8_t af,
                    std::string cidr_str,
                    uint32_t num_policy) {
    uint32_t max_rules = ((af == IP_AF_IPV4) ?
                            PDS_MAX_RULES_PER_IPV4_SECURITY_POLICY :
                            PDS_MAX_RULES_PER_IPV6_SECURITY_POLICY);
    memset(&this->spec, 0, sizeof(pds_policy_spec_t));
    this->spec.key = key;
    this->spec.num_rules = max_rules;
    this->stateful_rules = stateful_rules;
    this->spec.direction = direction;
    this->spec.policy_type = type;
    this->spec.af = af;
    this->spec.rules = NULL;
    this->cidr_str = cidr_str;
    num_obj = num_policy;
}

void
policy_feeder::iter_next(int width) {
    spec.key.id += width;
    cur_iter_pos++;
}

void
policy_feeder::key_build(pds_policy_key_t *key) const {
    memset(key, 0, sizeof(pds_policy_key_t));
    key->id = this->spec.key.id;
}

void
create_rules(std::string cidr_str, uint16_t num_rules,
             rule_t **rules, uint16_t stateful_rules)
{
    ip_prefix_t ip_pfx;
    uint16_t    num_range_rules;

    *rules =
        (rule_t *)SDK_CALLOC(PDS_MEM_ALLOC_SECURITY_POLICY,
                             num_rules * sizeof(rule_t));
    extract_ip_pfx((char *)cidr_str.c_str(), &ip_pfx);

    if (apulu()) {
        num_range_rules = (ip_pfx.addr.af == IP_AF_IPV6)?
                           MAX_RANGE_RULES_V6: MAX_RANGE_RULES_V4;
        for (uint32_t i = 0; i < num_rules; i++) {
            (*rules)[i].match.l4_match.sport_range.port_lo = 0;
            (*rules)[i].match.l4_match.sport_range.port_hi = 65535;
            (*rules)[i].match.l4_match.dport_range.port_lo = 0;
            (*rules)[i].match.l4_match.dport_range.port_hi = 65535;
            (*rules)[i].match.l3_match.ip_proto = IP_PROTO_TCP;
            // create few as range match rules and rest as prefix
            if (num_range_rules) {
                (*rules)[i].match.l3_match.src_match_type = IP_MATCH_RANGE;
                (*rules)[i].match.l3_match.dst_match_type = IP_MATCH_RANGE;
                (*rules)[i].match.l3_match.src_ip_range.af = ip_pfx.addr.af;
                memcpy(&(*rules)[i].match.l3_match.src_ip_range.ip_lo,
                       &ip_pfx.addr.addr, sizeof(ipvx_addr_t));
                memcpy(&(*rules)[i].match.l3_match.dst_ip_range.ip_lo,
                       &ip_pfx.addr.addr, sizeof(ipvx_addr_t));
                increment_ip_addr(&ip_pfx.addr, 2);
                memcpy(&(*rules)[i].match.l3_match.src_ip_range.ip_hi,
                       &ip_pfx.addr.addr, sizeof(ipvx_addr_t));
                memcpy(&(*rules)[i].match.l3_match.dst_ip_range.ip_hi,
                       &ip_pfx.addr.addr, sizeof(ipvx_addr_t));
                increment_ip_addr(&ip_pfx.addr);
                (*rules)[i].action_data.fw_action.action
                                = SECURITY_RULE_ACTION_DENY;
                num_range_rules--;
            } else {
                (*rules)[i].match.l3_match.src_match_type = IP_MATCH_PREFIX;
                (*rules)[i].match.l3_match.dst_match_type = IP_MATCH_PREFIX;
                memcpy(&(*rules)[i].match.l3_match.src_ip_pfx,
                       &ip_pfx, sizeof(ip_prefix_t));
                // using same ip as dst ip just for testing
                memcpy(&(*rules)[i].match.l3_match.dst_ip_pfx,
                       &ip_pfx, sizeof(ip_prefix_t));
                increment_ip_addr(&ip_pfx.addr);
                (*rules)[i].action_data.fw_action.action
                                = SECURITY_RULE_ACTION_ALLOW;
            }
            cidr_str = ippfx2str(&ip_pfx);
        }
    } else {
        for (uint32_t i = 0; i < num_rules; i++) {
            if (stateful_rules) {
                (*rules)[i].stateful = true;
                (*rules)[i].match.l4_match.sport_range.port_lo = 0;
                (*rules)[i].match.l4_match.sport_range.port_hi = 65535;
                (*rules)[i].match.l4_match.dport_range.port_lo = 0;
                (*rules)[i].match.l4_match.dport_range.port_hi = 65535;
                (*rules)[i].match.l3_match.ip_proto = IP_PROTO_TCP;
                stateful_rules--;
            } else {
                (*rules)[i].stateful = false;
                (*rules)[i].match.l4_match.icmp_type = 1;
                (*rules)[i].match.l4_match.icmp_code = 1;
                (*rules)[i].match.l3_match.ip_proto = IP_PROTO_ICMP;
            }
            memcpy(&(*rules)[i].match.l3_match.src_ip_pfx,
                   &ip_pfx, sizeof(ip_prefix_t));
            increment_ip_addr(&ip_pfx.addr);
            cidr_str = ippfx2str(&ip_pfx);
            (*rules)[i].action_data.fw_action.action = SECURITY_RULE_ACTION_ALLOW;
        }
    }
}

void
policy_feeder::spec_build(pds_policy_spec_t *spec) const {
    memcpy(spec, &this->spec, sizeof(pds_policy_spec_t));
    create_rules(this->cidr_str, this->spec.num_rules, (rule_t**)&(spec->rules),
                 this->stateful_rules);
}

bool
policy_feeder::key_compare(const pds_policy_key_t *key) const {
    return (memcmp(key, &this->spec.key, sizeof(pds_policy_key_t)) == 0);
}

bool
policy_feeder::spec_compare(const pds_policy_spec_t *spec) const {
    if (spec->direction != this->spec.direction)
        return false;
    if (spec->af != this->spec.af)
        return false;

    return true;
}

//----------------------------------------------------------------------------
// Misc routines
//----------------------------------------------------------------------------

// do not modify these sample values as rest of system is sync with these
static policy_feeder k_pol_feeder;

void sample_policy_setup(pds_batch_ctxt_t bctxt) {
    pds_policy_key_t pol_key = {.id = 1};

    // setup and teardown parameters should be in sync
    k_pol_feeder.init(pol_key, 512, RULE_DIR_INGRESS, POLICY_TYPE_FIREWALL,
                      IP_AF_IPV4, "10.0.0.1/16", 5);
    many_create(bctxt, k_pol_feeder);

    pol_key.id += 5;
    k_pol_feeder.init(pol_key, 512, RULE_DIR_INGRESS, POLICY_TYPE_FIREWALL,
                      IP_AF_IPV6, "2001::1/64", 5);
    many_create(bctxt, k_pol_feeder);

    pol_key.id += 5;
    k_pol_feeder.init(pol_key, 512, RULE_DIR_EGRESS, POLICY_TYPE_FIREWALL,
                      IP_AF_IPV4, "20.0.0.1/16", 5);
    many_create(bctxt, k_pol_feeder);

    pol_key.id += 5;
    k_pol_feeder.init(pol_key, 512, RULE_DIR_EGRESS, POLICY_TYPE_FIREWALL,
                      IP_AF_IPV6, "3001::1/64", 5);
    many_create(bctxt, k_pol_feeder);
}

void sample_policy_teardown(pds_batch_ctxt_t bctxt) {
    pds_policy_key_t pol_key = {.id = 1};

    // this feeder base values doesn't matter in case of deletes
    k_pol_feeder.init(pol_key, 512, RULE_DIR_INGRESS, POLICY_TYPE_FIREWALL,
                      IP_AF_IPV4, "10.0.0.1/16", 5);
    many_delete(bctxt, k_pol_feeder);

    pol_key.id += 5;
    k_pol_feeder.init(pol_key, 512, RULE_DIR_INGRESS, POLICY_TYPE_FIREWALL,
                      IP_AF_IPV6, "2001::1/64", 5);
    many_delete(bctxt, k_pol_feeder);

    pol_key.id += 5;
    k_pol_feeder.init(pol_key, 512, RULE_DIR_EGRESS, POLICY_TYPE_FIREWALL,
                      IP_AF_IPV4, "20.0.0.1/16", 5);
    many_delete(bctxt, k_pol_feeder);

    pol_key.id += 5;
    k_pol_feeder.init(pol_key, 512, RULE_DIR_EGRESS, POLICY_TYPE_FIREWALL,
                      IP_AF_IPV6, "3001::1/64", 5);
    many_delete(bctxt, k_pol_feeder);
}

}    // namespace api_test
