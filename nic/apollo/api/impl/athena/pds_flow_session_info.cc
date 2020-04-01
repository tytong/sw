//
// {C} Copyright 2019 Pensando Systems Inc. All rights reserved
//
//----------------------------------------------------------------------------
///
/// \file
/// athena flow session info implementation
///
//----------------------------------------------------------------------------

#include "nic/sdk/include/sdk/base.hpp"
#include "nic/sdk/lib/p4/p4_api.hpp"
#include "nic/sdk/lib/p4/p4_utils.hpp"
#include "nic/apollo/core/trace.hpp"
#include "nic/apollo/api/include/athena/pds_flow_session_info.h"
#include "gen/p4gen/athena/include/p4pd.h"
#include "nic/sdk/asic/pd/pd.hpp"
#include "gen/p4gen/p4/include/ftl.h"

#define FLOW_SESSION_INFO_FASTER_DELETE

#ifndef BITS_PER_BYTE
#define BITS_PER_BYTE   8
#endif

using namespace sdk;

extern "C" {

static sdk_ret_t
pds_flow_session_entry_setup (session_info_entry_t *entry,
                              pds_flow_session_data_t *data,
                              uint8_t direction)
{
    uint64_t smac = 0;

    if (!entry) {
        PDS_TRACE_ERR("entry is null");
        return SDK_RET_INVALID_ARG;
    }
    entry->set_skip_flow_log(data->skip_flow_log);
    entry->set_conntrack_id(data->conntrack_id);
    memcpy(&smac, data->host_mac,ETH_ADDR_LEN);
    entry->set_smac(smac);

    if (direction & HOST_TO_SWITCH) { 
        entry->set_h2s_epoch_vnic_value(
                data->host_to_switch_flow_info.epoch_vnic);
        entry->set_h2s_epoch_vnic_id(
                data->host_to_switch_flow_info.epoch_vnic_id);
        entry->set_h2s_epoch_mapping_value(
                data->host_to_switch_flow_info.epoch_mapping);
        entry->set_h2s_epoch_mapping_id(
                data->host_to_switch_flow_info.epoch_mapping_id);
        entry->set_h2s_throttle_bw1_id(
                data->host_to_switch_flow_info.policer_bw1_id);
        entry->set_h2s_throttle_bw2_id(
                data->host_to_switch_flow_info.policer_bw2_id);
        entry->set_h2s_vnic_statistics_id(
                data->host_to_switch_flow_info.vnic_stats_id);
        entry->set_h2s_vnic_statistics_mask(
                *(uint32_t *)data->host_to_switch_flow_info.vnic_stats_mask);
        entry->set_h2s_vnic_histogram_packet_len_id(
                data->host_to_switch_flow_info.vnic_histogram_packet_len_id);
        entry->set_h2s_vnic_histogram_latency_id(
                data->host_to_switch_flow_info.vnic_histogram_latency_id);
        entry->set_h2s_slow_path_tcp_flags_match(
                data->host_to_switch_flow_info.tcp_flags_bitmap);
        entry->set_h2s_session_rewrite_id(
                data->host_to_switch_flow_info.rewrite_id);
        entry->set_h2s_egress_action(
                data->host_to_switch_flow_info.egress_action);
        entry->set_h2s_allowed_flow_state_bitmap(
                data->host_to_switch_flow_info.allowed_flow_state_bitmask);
    } 

    if (direction & SWITCH_TO_HOST) {
        entry->set_s2h_epoch_vnic_value(
                data->switch_to_host_flow_info.epoch_vnic);
        entry->set_s2h_epoch_vnic_id(
                data->switch_to_host_flow_info.epoch_vnic_id);
        entry->set_s2h_epoch_mapping_value(
                data->switch_to_host_flow_info.epoch_mapping);
        entry->set_s2h_epoch_mapping_id(
                data->switch_to_host_flow_info.epoch_mapping_id);
        entry->set_s2h_throttle_bw1_id(
                data->switch_to_host_flow_info.policer_bw1_id);
        entry->set_s2h_throttle_bw2_id(
                data->switch_to_host_flow_info.policer_bw2_id);
        entry->set_s2h_vnic_statistics_id(
                data->switch_to_host_flow_info.vnic_stats_id);
        entry->set_s2h_vnic_statistics_mask(
                *(uint32_t *)data->switch_to_host_flow_info.vnic_stats_mask);
        entry->set_s2h_vnic_histogram_packet_len_id(
                data->switch_to_host_flow_info.vnic_histogram_packet_len_id);
        entry->set_s2h_vnic_histogram_latency_id(
                data->switch_to_host_flow_info.vnic_histogram_latency_id);
        entry->set_s2h_slow_path_tcp_flags_match(
                data->switch_to_host_flow_info.tcp_flags_bitmap);
        entry->set_s2h_session_rewrite_id(
                data->switch_to_host_flow_info.rewrite_id);
        entry->set_s2h_egress_action(
                data->switch_to_host_flow_info.egress_action);
        entry->set_s2h_allowed_flow_state_bitmap(
                data->switch_to_host_flow_info.allowed_flow_state_bitmask);
    }
    entry->set_valid_flag(TRUE);

    return SDK_RET_OK;
}
static sdk_ret_t
pds_flow_session_info_spec_fill (pds_flow_session_spec_t *spec,
                                 session_info_entry_t    *entry,
                                 uint8_t direction)
{
    uint32_t   vnic_stats_mask = 0;

    if (!entry || !spec ) {
        PDS_TRACE_ERR("entry (%p) or spec(%p)is null", entry, spec);
        return SDK_RET_INVALID_ARG;
    }

    if (direction & SWITCH_TO_HOST) {
        spec->data.switch_to_host_flow_info.epoch_vnic = 
                entry->get_s2h_epoch_vnic_value();
        spec->data.switch_to_host_flow_info.epoch_vnic_id = 
                entry->get_s2h_epoch_vnic_id();
        spec->data.switch_to_host_flow_info.epoch_mapping = 
                entry->get_s2h_epoch_mapping_value();
        spec->data.switch_to_host_flow_info.epoch_mapping_id  = 
                entry->get_s2h_epoch_mapping_id();
        spec->data.switch_to_host_flow_info.policer_bw1_id = 
                entry->get_s2h_throttle_bw1_id();
        spec->data.switch_to_host_flow_info.policer_bw2_id = 
                entry->get_s2h_throttle_bw2_id();
        spec->data.switch_to_host_flow_info.vnic_stats_id = 
                entry->get_s2h_vnic_statistics_id();
        vnic_stats_mask = entry->get_s2h_vnic_statistics_mask();
        memcpy((char *)spec->data.switch_to_host_flow_info.vnic_stats_mask,  
                (char *)&vnic_stats_mask, PDS_FLOW_STATS_MASK_LEN);
        spec->data.switch_to_host_flow_info.vnic_histogram_packet_len_id = 
                entry->get_s2h_vnic_histogram_packet_len_id();
        spec->data.switch_to_host_flow_info.vnic_histogram_latency_id = 
                entry->get_s2h_vnic_histogram_latency_id();
        spec->data.switch_to_host_flow_info.tcp_flags_bitmap = 
                entry->get_s2h_slow_path_tcp_flags_match();
        spec->data.switch_to_host_flow_info.rewrite_id = 
                entry->get_s2h_session_rewrite_id();
        spec->data.switch_to_host_flow_info.allowed_flow_state_bitmask  = 
                entry->get_s2h_allowed_flow_state_bitmap();
        spec->data.switch_to_host_flow_info.egress_action  = 
                (pds_egress_action_t) entry->get_s2h_egress_action();
    }
    if (direction & HOST_TO_SWITCH) {
        spec->data.host_to_switch_flow_info.epoch_vnic = 
                entry->get_h2s_epoch_vnic_value();
        spec->data.host_to_switch_flow_info.epoch_vnic_id = 
                entry->get_h2s_epoch_vnic_id();
        spec->data.host_to_switch_flow_info.epoch_mapping = 
                entry->get_h2s_epoch_mapping_value();
        spec->data.host_to_switch_flow_info.epoch_mapping_id  = 
                entry->get_h2s_epoch_mapping_id();
        spec->data.host_to_switch_flow_info.policer_bw1_id = 
                entry->get_h2s_throttle_bw1_id();
        spec->data.host_to_switch_flow_info.policer_bw2_id = 
                entry->get_h2s_throttle_bw2_id();
        spec->data.host_to_switch_flow_info.vnic_stats_id = 
                entry->get_h2s_vnic_statistics_id();
        vnic_stats_mask = entry->get_h2s_vnic_statistics_mask();
        memcpy((char *)spec->data.host_to_switch_flow_info.vnic_stats_mask,  
                (char *)&vnic_stats_mask, PDS_FLOW_STATS_MASK_LEN);
        spec->data.host_to_switch_flow_info.vnic_histogram_packet_len_id = 
                entry->get_h2s_vnic_histogram_packet_len_id();
        spec->data.host_to_switch_flow_info.vnic_histogram_latency_id = 
                entry->get_h2s_vnic_histogram_latency_id();
        spec->data.host_to_switch_flow_info.tcp_flags_bitmap = 
                entry->get_h2s_slow_path_tcp_flags_match();
        spec->data.host_to_switch_flow_info.rewrite_id = 
                entry->get_h2s_session_rewrite_id();
        spec->data.host_to_switch_flow_info.allowed_flow_state_bitmask  = 
                entry->get_h2s_allowed_flow_state_bitmap();
        spec->data.host_to_switch_flow_info.egress_action  = 
                (pds_egress_action_t) entry->get_h2s_egress_action();
    }
    return SDK_RET_OK;
}
// Helper function to fill spec with direction specific session info
static void
flow_session_info_spec_fill (pds_flow_session_spec_t *spec,
                             session_info_session_info_t *session_info,
                             uint8_t direction)
{
    if (direction & SWITCH_TO_HOST) {
        spec->data.switch_to_host_flow_info.epoch_vnic =
            session_info->s2h_epoch_vnic_value;
        spec->data.switch_to_host_flow_info.epoch_vnic_id =
            session_info->s2h_epoch_vnic_id;
        spec->data.switch_to_host_flow_info.epoch_mapping =
            session_info->s2h_epoch_mapping_value;
        spec->data.switch_to_host_flow_info.epoch_mapping_id =
            session_info->s2h_epoch_mapping_id;
        spec->data.switch_to_host_flow_info.policer_bw1_id =
            session_info->s2h_throttle_bw1_id;
        spec->data.switch_to_host_flow_info.policer_bw2_id =
            session_info->s2h_throttle_bw2_id;
        spec->data.switch_to_host_flow_info.vnic_stats_id =
            session_info->s2h_vnic_statistics_id;
        // FIXME: Truncated copy, will fix later after P4 changes
        memcpy(spec->data.switch_to_host_flow_info.vnic_stats_mask,
               (uint8_t *)&session_info->s2h_vnic_statistics_mask, 4);
        spec->data.switch_to_host_flow_info.vnic_histogram_packet_len_id =
            session_info->s2h_vnic_histogram_packet_len_id;
        spec->data.switch_to_host_flow_info.vnic_histogram_latency_id =
            session_info->s2h_vnic_histogram_latency_id;
        spec->data.switch_to_host_flow_info.tcp_flags_bitmap =
            session_info->s2h_slow_path_tcp_flags_match;
        spec->data.switch_to_host_flow_info.rewrite_id =
            session_info->s2h_session_rewrite_id;
        spec->data.switch_to_host_flow_info.allowed_flow_state_bitmask =
            session_info->s2h_allowed_flow_state_bitmap;
        spec->data.switch_to_host_flow_info.egress_action =
            (pds_egress_action_t)session_info->s2h_egress_action;
    }
    if (direction & HOST_TO_SWITCH) {
        spec->data.host_to_switch_flow_info.epoch_vnic =
            session_info->h2s_epoch_vnic_value;
        spec->data.host_to_switch_flow_info.epoch_vnic_id =
            session_info->h2s_epoch_vnic_id;
        spec->data.host_to_switch_flow_info.epoch_mapping =
            session_info->h2s_epoch_mapping_value;
        spec->data.host_to_switch_flow_info.epoch_mapping_id =
            session_info->h2s_epoch_mapping_id;
        spec->data.host_to_switch_flow_info.policer_bw1_id =
            session_info->h2s_throttle_bw1_id =
        spec->data.host_to_switch_flow_info.policer_bw2_id =
            session_info->h2s_throttle_bw2_id;
        spec->data.host_to_switch_flow_info.vnic_stats_id = 
            session_info->h2s_vnic_statistics_id;
        // FIXME: Truncated copy, will fix later after P4 changes
        memcpy(spec->data.host_to_switch_flow_info.vnic_stats_mask,
               (uint8_t *)&session_info->h2s_vnic_statistics_mask, 4);
        spec->data.host_to_switch_flow_info.vnic_histogram_packet_len_id =
            session_info->h2s_vnic_histogram_packet_len_id;
        spec->data.host_to_switch_flow_info.vnic_histogram_latency_id =
            session_info->h2s_vnic_histogram_latency_id;
        spec->data.host_to_switch_flow_info.tcp_flags_bitmap =
            session_info->h2s_slow_path_tcp_flags_match;
        spec->data.host_to_switch_flow_info.rewrite_id =
            session_info->h2s_session_rewrite_id;
        spec->data.host_to_switch_flow_info.allowed_flow_state_bitmask =
            session_info->h2s_allowed_flow_state_bitmap;
        spec->data.host_to_switch_flow_info.egress_action =
            (pds_egress_action_t)session_info->h2s_egress_action;
    }
    return;
}

// Helper function to fill actiondata with direction specific session info
static void
flow_session_info_actiondata_fill (session_info_session_info_t *session_info,
                                   pds_flow_session_spec_t *spec,
                                   uint8_t direction)
{
    if (direction & HOST_TO_SWITCH) {
        session_info->h2s_epoch_vnic_value =
            spec->data.host_to_switch_flow_info.epoch_vnic;
        session_info->h2s_epoch_vnic_id =
            spec->data.host_to_switch_flow_info.epoch_vnic_id;
        session_info->h2s_epoch_mapping_value =
            spec->data.host_to_switch_flow_info.epoch_mapping;
        session_info->h2s_epoch_mapping_id =
            spec->data.host_to_switch_flow_info.epoch_mapping_id;
        session_info->h2s_throttle_bw1_id =
            spec->data.host_to_switch_flow_info.policer_bw1_id;
        session_info->h2s_throttle_bw2_id =
            spec->data.host_to_switch_flow_info.policer_bw2_id;
        session_info->h2s_vnic_statistics_id =
            spec->data.host_to_switch_flow_info.vnic_stats_id;
        // FIXME: Truncated copy, will fix later after P4 changes
        memcpy((uint8_t *)&session_info->h2s_vnic_statistics_mask,
               spec->data.host_to_switch_flow_info.vnic_stats_mask, 4);
        session_info->h2s_vnic_histogram_latency_id =
            spec->data.host_to_switch_flow_info.vnic_histogram_latency_id;
        session_info->h2s_vnic_histogram_packet_len_id =
            spec->data.host_to_switch_flow_info.vnic_histogram_packet_len_id;
        session_info->h2s_slow_path_tcp_flags_match =
            spec->data.host_to_switch_flow_info.tcp_flags_bitmap;
        session_info->h2s_session_rewrite_id =
            spec->data.host_to_switch_flow_info.rewrite_id;
        session_info->h2s_allowed_flow_state_bitmap =
            spec->data.host_to_switch_flow_info.allowed_flow_state_bitmask;
        session_info->h2s_egress_action =
            spec->data.host_to_switch_flow_info.egress_action;
    }
    if (direction & SWITCH_TO_HOST) {
        session_info->s2h_epoch_vnic_value =
            spec->data.switch_to_host_flow_info.epoch_vnic;
        session_info->s2h_epoch_vnic_id =
            spec->data.switch_to_host_flow_info.epoch_vnic_id;
        session_info->s2h_epoch_mapping_value =
            spec->data.switch_to_host_flow_info.epoch_mapping;
        session_info->s2h_epoch_mapping_id =
            spec->data.switch_to_host_flow_info.epoch_mapping_id;
        session_info->s2h_throttle_bw1_id =
            spec->data.switch_to_host_flow_info.policer_bw1_id;
        session_info->s2h_throttle_bw2_id =
            spec->data.switch_to_host_flow_info.policer_bw2_id;
        session_info->s2h_vnic_statistics_id =
            spec->data.switch_to_host_flow_info.vnic_stats_id;
        // FIXME: Truncated copy, will fix later after P4 changes
        memcpy((uint8_t *)&session_info->s2h_vnic_statistics_mask,
               spec->data.switch_to_host_flow_info.vnic_stats_mask, 4);
        session_info->s2h_vnic_histogram_latency_id =
            spec->data.switch_to_host_flow_info.vnic_histogram_latency_id;
        session_info->s2h_vnic_histogram_packet_len_id =
            spec->data.switch_to_host_flow_info.vnic_histogram_packet_len_id;
        session_info->s2h_slow_path_tcp_flags_match =
            spec->data.switch_to_host_flow_info.tcp_flags_bitmap;
        session_info->s2h_session_rewrite_id =
            spec->data.switch_to_host_flow_info.rewrite_id;
        session_info->s2h_allowed_flow_state_bitmap =
            spec->data.switch_to_host_flow_info.allowed_flow_state_bitmask;
        session_info->s2h_egress_action =
            spec->data.switch_to_host_flow_info.egress_action;
    }
    return;
}

static sdk_ret_t
pds_flow_session_info_write (pds_flow_session_spec_t *spec, bool update)
{
    p4pd_error_t p4pd_ret;
    uint32_t session_info_id;
    session_info_actiondata_t session_actiondata = { 0 };
    session_info_actiondata_t rd_session_actiondata = { 0 };
    session_info_session_info_t *session_info, *rd_session_info;
    session_info =
        &session_actiondata.action_u.session_info_session_info;

    if (!spec) {
        PDS_TRACE_ERR("spec is null");
        return SDK_RET_INVALID_ARG;
    }
    session_info_id = spec->key.session_info_id;
    if ((session_info_id == 0) ||
        (session_info_id >= PDS_FLOW_SESSION_INFO_ID_MAX)) {
        PDS_TRACE_ERR("session id %u is invalid", session_info_id);
        return SDK_RET_INVALID_ARG;
    }
    if (!((spec->key.direction & HOST_TO_SWITCH) ||
        (spec->key.direction & SWITCH_TO_HOST))) {
        PDS_TRACE_ERR("Direction %u is invalid", spec->key.direction);
        return SDK_RET_INVALID_ARG;
    }

    // For update, check if there is an entry already at the index
    if (update) {
        if (p4pd_global_entry_read(P4TBL_ID_SESSION_INFO, session_info_id,
                                   NULL, NULL, &rd_session_actiondata) !=
                                   P4PD_SUCCESS) {
            PDS_TRACE_ERR("Failed to read session info table at index %u",
                          session_info_id);
            return SDK_RET_HW_READ_ERR;
        }
        rd_session_info =
            &rd_session_actiondata.action_u.session_info_session_info;
        if (!rd_session_info->valid_flag) {
            PDS_TRACE_ERR("No entry in session info table at index %u",
                          session_info_id);
            return SDK_RET_ENTRY_NOT_FOUND;
        }
        // Copy the other already written fields from table entry
        spec->data.conntrack_id = rd_session_info->conntrack_id;
        spec->data.skip_flow_log = rd_session_info->skip_flow_log;
        memcpy(spec->data.host_mac, rd_session_info->smac, ETH_ADDR_LEN);
        if (spec->key.direction & HOST_TO_SWITCH) {
            flow_session_info_spec_fill(spec, rd_session_info, SWITCH_TO_HOST);
            flow_session_info_actiondata_fill(session_info, spec, SWITCH_TO_HOST);
        } else {
            flow_session_info_spec_fill(spec, rd_session_info, HOST_TO_SWITCH);
            flow_session_info_actiondata_fill(session_info, spec, HOST_TO_SWITCH);
        }
    }

    session_actiondata.action_id = SESSION_INFO_SESSION_INFO_ID;
    if (spec->key.direction & HOST_TO_SWITCH)
            flow_session_info_actiondata_fill(session_info, spec, HOST_TO_SWITCH);
    if (spec->key.direction & SWITCH_TO_HOST)
            flow_session_info_actiondata_fill(session_info, spec, SWITCH_TO_HOST);
    session_info->valid_flag = 1;
    session_info->conntrack_id = spec->data.conntrack_id;
    session_info->skip_flow_log = spec->data.skip_flow_log;
    memcpy(session_info->smac, spec->data.host_mac, ETH_ADDR_LEN);
    p4pd_ret = p4pd_global_entry_write(P4TBL_ID_SESSION_INFO,
                                       session_info_id, NULL, NULL,
                                       &session_actiondata);
    if (p4pd_ret != P4PD_SUCCESS) {
        PDS_TRACE_ERR("Failed to write session info table at index %u",
                       session_info_id);
        return SDK_RET_HW_PROGRAM_ERR;
    }

    return SDK_RET_OK;
}

sdk_ret_t
pds_flow_session_info_create (pds_flow_session_spec_t *spec)
{
    sdk_ret_t ret;
    session_info_entry_t entry;

    if (!spec) {
        PDS_TRACE_ERR("spec is null");
        return SDK_RET_INVALID_ARG;
    }

    if ((spec->key.session_info_id == 0) ||
        (spec->key.session_info_id >= PDS_FLOW_SESSION_INFO_ID_MAX)) {
        PDS_TRACE_ERR("session id %u is invalid", spec->key.session_info_id);
        return SDK_RET_INVALID_ARG;
    }
    if (!((spec->key.direction & HOST_TO_SWITCH) ||
        (spec->key.direction & SWITCH_TO_HOST))) {
        PDS_TRACE_ERR("Direction %u is invalid", spec->key.direction);
        return SDK_RET_INVALID_ARG;
    }
    entry.clear();
    if ((ret = pds_flow_session_entry_setup(&entry, 
                                            &spec->data, 
                                            spec->key.direction)) != 
            SDK_RET_OK ) {
        return ret;
    }
    ret =  entry.write(spec->key.session_info_id);
    if (ret != P4PD_SUCCESS) {
        PDS_TRACE_ERR("Failed to write session info table at index %u",
                       spec->key.session_info_id);
        return SDK_RET_HW_PROGRAM_ERR;
    }
    return SDK_RET_OK;
}

sdk_ret_t
pds_flow_session_info_read (pds_flow_session_key_t *key,
                            pds_flow_session_info_t *info)
{
    p4pd_error_t              p4pd_ret;
    uint32_t                  session_info_id;
    session_info_actiondata_t session_actiondata = { 0 };
    session_info_entry_t      entry;
    uint64_t                  smac = 0;

    if (!key || !info) {
        PDS_TRACE_ERR("key or info is null");
        return SDK_RET_INVALID_ARG;
    }
    session_info_id = key->session_info_id;
    if ((session_info_id == 0) ||
        (session_info_id >= PDS_FLOW_SESSION_INFO_ID_MAX)) {
        PDS_TRACE_ERR("session id %u is invalid", session_info_id);
        return SDK_RET_INVALID_ARG;
    }
    if (!((key->direction & HOST_TO_SWITCH) ||
        (key->direction & SWITCH_TO_HOST))) {
        PDS_TRACE_ERR("Direction %u is invalid", key->direction);
        return SDK_RET_INVALID_ARG;
    }
    entry.clear();
    entry.read(session_info_id);
    if (!entry.get_valid_flag()) {

        // Reading an entry to see if it's valid is a normal action
        // so no need to log.
        // PDS_TRACE_ERR("No entry in session info table at index %u",
        //               session_info_id);
        return SDK_RET_ENTRY_NOT_FOUND;
    }

    info->spec.data.conntrack_id = entry.get_conntrack_id();
    info->spec.data.skip_flow_log = entry.get_skip_flow_log();
    info->status.timestamp = entry.get_timestamp();;
    smac = entry.get_smac();
    memcpy(info->spec.data.host_mac, &smac, ETH_ADDR_LEN);
    return pds_flow_session_info_spec_fill(&info->spec, 
                                           &entry, 
                                           key->direction);
}

sdk_ret_t
pds_flow_session_info_update (pds_flow_session_spec_t *spec)
{
    sdk_ret_t                 ret = SDK_RET_OK;
    session_info_entry_t      entry;
    uint32_t                  session_info_id = 0;

    if (!spec) {
        PDS_TRACE_ERR("spec is null");
        return SDK_RET_INVALID_ARG;
    }

    session_info_id = spec->key.session_info_id;
    if ((session_info_id == 0) ||
        (session_info_id >= PDS_FLOW_SESSION_INFO_ID_MAX)) {
        PDS_TRACE_ERR("session id %u is invalid", session_info_id);
        return SDK_RET_INVALID_ARG;
    }
    if (!((spec->key.direction & HOST_TO_SWITCH) ||
        (spec->key.direction & SWITCH_TO_HOST))) {
        PDS_TRACE_ERR("Direction %u is invalid", spec->key.direction);
        return SDK_RET_INVALID_ARG;
    }
    entry.clear();
    entry.read(session_info_id);
    if ((ret = pds_flow_session_entry_setup(&entry, 
                                            &spec->data, 
                                            spec->key.direction)) != 
            SDK_RET_OK ) {
        return ret;
    } 
    return entry.write(spec->key.session_info_id);
}

#ifdef FLOW_SESSION_INFO_FASTER_DELETE
static int
flow_session_info_delete(uint32_t session_info_id,
                         session_info_actiondata_t *null_actiondata)
{
    static p4pd_table_properties_t tbl_ctx;

    if (!tbl_ctx.hbm_layout.entry_width) {
        p4pd_global_table_properties_get(P4TBL_ID_SESSION_INFO, &tbl_ctx);
        if (!tbl_ctx.hbm_layout.entry_width ||
            (sizeof(session_info_actiondata_t) < tbl_ctx.hbm_layout.entry_width)) {
            PDS_TRACE_ERR("Failed entry_width %u or sizeof unpacked "
                          "session_info_actiondata_t %u error",
                          tbl_ctx.hbm_layout.entry_width,
                          (unsigned)sizeof(session_info_actiondata_t));
            return SDK_RET_HW_PROGRAM_ERR;
        }
    }

    // Preference is to write only the few bytes that contain valid_flag
    // but there's no direct asicpd API to do that. The alternative is to
    // break into separate calls to capri_hbm_table_entry_write() and
    // capri_hbm_table_entry_cache_invalidate() but that's too much trouble.

    return sdk::asic::pd::asicpd_hbm_table_entry_write(P4TBL_ID_SESSION_INFO,
                          session_info_id, (uint8_t *)null_actiondata,
                          tbl_ctx.hbm_layout.entry_width * BITS_PER_BYTE);
}
#endif

sdk_ret_t
pds_flow_session_info_delete (pds_flow_session_key_t *key)
{
    p4pd_error_t              p4pd_ret = SDK_RET_OK;
    uint32_t                  session_info_id = 0;
    session_info_entry_t      entry;

    if (!key) {
        PDS_TRACE_ERR("key is null");
        return SDK_RET_INVALID_ARG;
    }
    session_info_id = key->session_info_id;
    if ((session_info_id == 0) ||
        (session_info_id >= PDS_FLOW_SESSION_INFO_ID_MAX)) {
        PDS_TRACE_ERR("session id %u is invalid", session_info_id);
        return SDK_RET_INVALID_ARG;
    }
    if (!((key->direction & HOST_TO_SWITCH) ||
        (key->direction & SWITCH_TO_HOST))) {
        PDS_TRACE_ERR("Direction %u is invalid", key->direction);
        return SDK_RET_INVALID_ARG;
    }
    entry.clear();
    p4pd_ret =  entry.write(session_info_id);
    /*
    TODO: DO we still need this ?
#ifdef FLOW_SESSION_INFO_FASTER_DELETE
    p4pd_ret = flow_session_info_delete(session_info_id, &session_actiondata);
#endif
    */
    if (p4pd_ret != P4PD_SUCCESS) {
        PDS_TRACE_ERR("Failed to delete session info table at index %u",
                      session_info_id);
        return SDK_RET_HW_PROGRAM_ERR;
    }
    return SDK_RET_OK;
}
}

