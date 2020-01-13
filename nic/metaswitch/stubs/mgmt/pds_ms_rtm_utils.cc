// {C} Copyright 2019 Pensando Systems Inc. All rights reserved
// Purpose: Helper APIs for metaswitch RTM component

#include "nic/metaswitch/stubs/mgmt/pds_ms_mgmt_utils.hpp"
#include "qc0rtmib.h"

namespace pds {
NBB_VOID
rtm_strt_fill_func (CPStaticRouteSpec&      req,
                    AMB_GEN_IPS             *mib_msg,
                    AMB_CIPR_RTM_STATIC_RT  *data,
                    NBB_LONG                row_status)
{
    // Local variables
    NBB_ULONG *oid = (NBB_ULONG *)((NBB_BYTE *)mib_msg + mib_msg->oid_offset);

    // Let the DC-RTM decide the outgoing interface of this route 
    data->if_index                    = 0;
    oid[AMB_QCR_STRT_IF_INDEX_INDEX]  = 0;
    AMB_SET_FIELD_PRESENT (mib_msg,AMB_OID_QCR_STRT_IF_INDEX);
}
} // End of namespace pds

namespace pds_ms {

// Fill rtmEntityTable: AMB_CIPR_RTM_ENTITY 
NBB_VOID 
pds_ms_fill_amb_cipr_rtm (AMB_GEN_IPS *mib_msg, pds_ms_config_t *conf)
{
    // Local variables
    NBB_ULONG           *oid = NULL; 
    AMB_CIPR_RTM_ENTITY *data= NULL;

    NBB_TRC_ENTRY ("pds_ms_fill_amb_cipr_rtm");

    // Get oid and data offset 
    oid     = (NBB_ULONG *)((NBB_BYTE *)mib_msg + mib_msg->oid_offset);
    data    = (AMB_CIPR_RTM_ENTITY *)((NBB_BYTE *)mib_msg + mib_msg->data_offset); 

    // Set all fields absentt
    AMB_SET_ALL_FIELDS_NOT_PRESENT (mib_msg);

    // Set OID len and family
    oid[0] = AMB_QCR_ENT_OID_LEN;
    oid[1] = AMB_FAM_CIPR_RTM_ENTITY;

    // Set all incoming fields
    oid[AMB_QCR_ENT_FTE_INDEX_INDEX]    = conf->entity_index;
    data->fte_index                     = conf->entity_index;
    AMB_SET_FIELD_PRESENT (mib_msg, AMB_OID_QCR_ENT_FTE_INDEX);

    data->row_status = conf->row_status;
    AMB_SET_FIELD_PRESENT (mib_msg, AMB_OID_QCR_ENT_ROW_STATUS);

    if (conf->row_status != AMB_ROW_DESTROY)
    {
        NBB_TRC_FLOW ((NBB_FORMAT "Not destroying DC-RTM: fill in fields"));
        data->admin_stat = conf->admin_status;
        AMB_SET_FIELD_PRESENT (mib_msg, AMB_OID_QCR_ENT_ADMIN_STAT);

        data->addr_family = AMB_INETWK_ADDR_TYPE_IPV4;
        AMB_SET_FIELD_PRESENT (mib_msg, AMB_OID_QCR_ENT_ADDR_FAM);

        data->i3_index = PDS_MS_LIM_ENT_INDEX;
        AMB_SET_FIELD_PRESENT (mib_msg, AMB_OID_QCR_ENT_I3_INDEX);

        data->partner_wait_time = 1000; // millisec
        AMB_SET_FIELD_PRESENT (mib_msg, AMB_OID_QCR_ENT_PRTNR_WAIT_TIME);

        if (conf->entity_index != PDS_MS_RTM_DEF_ENT_INDEX) {
            auto vrf_name = std::to_string(conf->entity_index);
            strcpy((char*)data->vrf_name, vrf_name.c_str());
            data->vrf_name_len = vrf_name.size();
            SDK_TRACE_DEBUG("RTM VRF name = %s", vrf_name.c_str());
            AMB_SET_FIELD_PRESENT (mib_msg, AMB_OID_QCR_ENT_VRF_NAME);
        }
    }

    NBB_TRC_EXIT();
    return;
} 

// Fill rtmMjTable: AMB_CIPR_RTM_MJ
NBB_VOID
pds_ms_fill_amb_cipr_rtm_mj (AMB_GEN_IPS *mib_msg, pds_ms_config_t *conf)
{
    // Local variables
    NBB_ULONG       *oid = NULL; 
    AMB_CIPR_RTM_MJ *data= NULL;

    NBB_TRC_ENTRY ("sms_fill_amb_cipr_rtm_mj");

    // Get oid and data offset 
    oid     = (NBB_ULONG *)((NBB_BYTE *)mib_msg + mib_msg->oid_offset);
    data    = (AMB_CIPR_RTM_MJ *)((NBB_BYTE *)mib_msg + mib_msg->data_offset); 

    // Set all fields absent
    AMB_SET_ALL_FIELDS_NOT_PRESENT (mib_msg);

    // Set OID len and family
    oid[0] = AMB_QRPM_MJ_OID_LEN;
    oid[1] = AMB_FAM_CIPR_RTM_MJ;

    // Set all incoming fields
    oid[AMB_QRPM_MJ_RTM_FTE_INDEX_INDEX]    = conf->entity_index;
    data->rtm_fte_index                     = conf->entity_index;
    AMB_SET_FIELD_PRESENT (mib_msg, AMB_OID_QRPM_MJ_RTM_FTE_INDEX);

    oid[AMB_QRPM_MJ_SLAVE_FTE_ID_INDEX]     = conf->slave_entity_index;
    data->slave_fte_id                      = conf->slave_entity_index;
    AMB_SET_FIELD_PRESENT (mib_msg, AMB_OID_QRPM_MJ_SLAVE_FTE_ID);

    oid[AMB_QRPM_MJ_SLAVE_TYPE_INDEX]       = conf->slave_type;
    data->slave_type                        = conf->slave_type;
    AMB_SET_FIELD_PRESENT (mib_msg, AMB_OID_QRPM_MJ_SLAVE_TYPE);

    data->row_status = conf->row_status;
    AMB_SET_FIELD_PRESENT (mib_msg, AMB_OID_QRPM_MJ_ROW_STATUS);

    if (conf->row_status != AMB_ROW_DESTROY)
    {
        NBB_TRC_FLOW ((NBB_FORMAT "Not DC-RTM: fill in fields"));
        data->admin_status = conf->admin_status;
        AMB_SET_FIELD_PRESENT (mib_msg, AMB_OID_QRPM_MJ_ADMIN_STATUS);

    }

    NBB_TRC_EXIT();
    return;
} 

// Fill rtmRedistTable: AMB_CIPR_RTM_REDIST 
NBB_VOID 
pds_ms_fill_amb_cipr_rtm_redist (AMB_GEN_IPS *mib_msg, pds_ms_config_t *conf)
{
    // Local variables
    NBB_ULONG           *oid = NULL; 
    AMB_CIPR_RTM_REDIST *data= NULL;

    NBB_TRC_ENTRY ("pds_ms_fill_amb_cipr_rtm_redist");

    // Get oid and data offset 
    oid     = (NBB_ULONG *)((NBB_BYTE *)mib_msg + mib_msg->oid_offset);
    data    = (AMB_CIPR_RTM_REDIST *)((NBB_BYTE *)mib_msg + mib_msg->data_offset); 

    // Set all fields absentt
    AMB_SET_ALL_FIELDS_NOT_PRESENT (mib_msg);

    // Set OID len and family
    oid[0] = AMB_QCR_RDS_OID_LEN;
    oid[1] = AMB_FAM_CIPR_RTM_REDIST;

    // Set all incoming fields
    oid[AMB_QCR_RDS_FTE_INDEX_INDEX]    = conf->entity_index;
    data->fte_index                     = conf->entity_index;
    AMB_SET_FIELD_PRESENT (mib_msg, AMB_OID_QCR_RDS_FTE_INDEX);

    oid[AMB_QCR_RDS_ENTRY_ID_INDEX]    = 1;
    data->entry_id                     = 1;
    AMB_SET_FIELD_PRESENT (mib_msg, AMB_OID_QCR_RDS_ENTRY_ID);

    data->row_status = conf->row_status;
    AMB_SET_FIELD_PRESENT (mib_msg, AMB_OID_QCR_RDS_ROW_STATUS);

    if (conf->row_status != AMB_ROW_DESTROY)
    {
        NBB_TRC_FLOW ((NBB_FORMAT "Not destroying RTM Redistribute: fill in fields"));
        data->admin_stat = conf->admin_status;
        AMB_SET_FIELD_PRESENT (mib_msg, AMB_OID_QCR_RDS_ADMIN_STAT);

        data->info_src = ATG_QC_PROT_CONNECTED;
        AMB_SET_FIELD_PRESENT (mib_msg, AMB_OID_QCR_RDS_INFO_SRC);

        data->info_dest = ATG_QC_PROT_BGP;
        AMB_SET_FIELD_PRESENT (mib_msg, AMB_OID_QCR_RDS_INFO_DEST);

        data->redist_flag = AMB_TRUE;
        AMB_SET_FIELD_PRESENT (mib_msg, AMB_OID_QCR_RDS_REDIST_FLAG);
    }

    NBB_TRC_EXIT();
    return;
} 

NBB_VOID
pds_ms_row_update_rtm (pds_ms_config_t *conf)
{
    NBB_TRC_ENTRY ("pds_ms_row_update_rtm");

    // Set params
    conf->oid_len       = AMB_QCR_ENT_OID_LEN;
    conf->data_len      = sizeof (AMB_CIPR_RTM_ENTITY);

    // Convert to row_update and send
    pds_ms_ctm_send_row_update_common (conf, pds_ms_fill_amb_cipr_rtm); 

    NBB_TRC_EXIT();
    return;
}

NBB_VOID
pds_ms_row_update_rtm_mj (pds_ms_config_t *conf)
{
    NBB_TRC_ENTRY ("pds_ms_row_update_rtm_mj");
    
    // Set params
    conf->oid_len               = AMB_QRPM_MJ_OID_LEN;
    conf->data_len              = sizeof (AMB_CIPR_RTM_MJ);

    // Convert to row_update and send
    pds_ms_ctm_send_row_update_common (conf, pds_ms_fill_amb_cipr_rtm_mj); 

    NBB_TRC_EXIT();
    return;
}

NBB_VOID pds_ms_rtm_redis_connected (pds_ms_config_t *conf)
{
    conf->oid_len               = AMB_QCR_RDS_OID_LEN;
    conf->data_len              = sizeof (AMB_CIPR_RTM_REDIST);

    pds_ms_ctm_send_row_update_common (conf, pds_ms_fill_amb_cipr_rtm_redist); 
    return;
}

NBB_VOID
pds_ms_rtm_create (pds_ms_config_t *conf, int entity_index, bool is_default)
{
    NBB_TRC_ENTRY ("pds_ms_rtm_create");

    // rtmEntityTable - Admin Down                                
    conf->entity_index          = entity_index;
    conf->admin_status          = AMB_ADMIN_STATUS_UP;
    pds_ms_row_update_rtm (conf);

    if (is_default) {
        // rtmMjTable - AMB_RTM_ARI_PARTNER_BGP
        conf->slave_entity_index    = PDS_MS_BGP_RM_ENT_INDEX;
        conf->slave_type            = AMB_RTM_ARI_PARTNER_BGP;
        conf->admin_status          = AMB_ADMIN_STATUS_UP;
        pds_ms_row_update_rtm_mj (conf);

        // rtmMjTable -AMB_RTM_ARI_PARTNER_FT 
        conf->slave_entity_index    = PDS_MS_FT_ENT_INDEX;
        conf->slave_type            = AMB_RTM_ARI_PARTNER_FT;
        conf->admin_status          = AMB_ADMIN_STATUS_UP;
        pds_ms_row_update_rtm_mj (conf);

        // rtmMjTable -AMB_RTM_ARI_PARTNER_PSM 
        conf->slave_entity_index    = PDS_MS_PSM_ENT_INDEX;
        conf->slave_type            = AMB_RTM_ARI_PARTNER_PSM;
        conf->admin_status          = AMB_ADMIN_STATUS_UP;
        pds_ms_row_update_rtm_mj (conf);

        // rtm Redistribute connected
        conf->admin_status          = AMB_ADMIN_STATUS_UP;
        pds_ms_rtm_redis_connected (conf);
    } else {
        // Join FTM only for Tenant VRF to configure Type 5 routes to ROPI stub 
        conf->slave_entity_index    = PDS_MS_FTM_ENT_INDEX;
        conf->slave_type            = AMB_RTM_ARI_PARTNER_FTM;
        conf->admin_status          = AMB_ADMIN_STATUS_UP;
        pds_ms_row_update_rtm_mj (conf);
    }

    NBB_TRC_EXIT();
    return;
}
}