//
// {C} Copyright 2019 Pensando Systems Inc. All rights reserved
//
//----------------------------------------------------------------------------
///
/// \file
/// pipeline global state maintenance
///
//----------------------------------------------------------------------------

#include "nic/apollo/api/impl/artemis_impl_state.hpp"
#include "gen/p4gen/artemis/include/p4pd.h"
#include "nic/sdk/lib/p4/p4_api.hpp"

namespace api {
namespace impl {

/// \defgroup PDS_ARTEMIS_IMPL_STATE - tep database functionality
/// \ingroup PDS_ARTEMIS
/// \@{

// constructor
artemis_impl_state::artemis_impl_state(pds_state *state) {
#if 0
    p4pd_table_properties_t    tinfo;

    // instantiate P4 tables for bookkeeping
    p4pd_table_properties_get(P4TBL_ID_KEY_NATIVE, &tinfo);
    key_native_tbl_ =
        tcam::factory(tinfo.tablename, P4TBL_ID_KEY_NATIVE,
                      tinfo.tabledepth, tinfo.key_struct_size,
                      tinfo.actiondata_struct_size,
                      false, true, NULL);
    SDK_ASSERT(key_native_tbl_ != NULL);

    p4pd_table_properties_get(P4TBL_ID_KEY_TUNNELED, &tinfo);
    key_tunneled_tbl_ =
        tcam::factory(tinfo.tablename, P4TBL_ID_KEY_TUNNELED,
                      tinfo.tabledepth, tinfo.key_struct_size,
                      tinfo.actiondata_struct_size,
                      false, true, NULL);
    SDK_ASSERT(key_tunneled_tbl_ != NULL);

    p4pd_table_properties_get(P4TBL_ID_P4I_DROP_STATS, &tinfo);
    ingress_drop_stats_tbl_ =
        tcam::factory(tinfo.tablename, P4TBL_ID_P4I_DROP_STATS,
                      tinfo.tabledepth, tinfo.key_struct_size,
                      tinfo.actiondata_struct_size,
                      false, false, NULL);
    SDK_ASSERT(ingress_drop_stats_tbl_ != NULL);

    p4pd_table_properties_get(P4TBL_ID_P4E_DROP_STATS, &tinfo);
    egress_drop_stats_tbl_ =
        tcam::factory(tinfo.tablename, P4TBL_ID_P4E_DROP_STATS,
                      tinfo.tabledepth, tinfo.key_struct_size,
                      tinfo.actiondata_struct_size,
                      false, false, NULL);
    SDK_ASSERT(egress_drop_stats_tbl_ != NULL);

    p4pd_table_properties_get(P4TBL_ID_NACL, &tinfo);
    nacl_tbl_ =
        tcam::factory(tinfo.tablename, P4TBL_ID_NACL, tinfo.tabledepth,
                      tinfo.key_struct_size, tinfo.actiondata_struct_size,
                      false, true, NULL);
     SDK_ASSERT(nacl_tbl_ != NULL);
#endif
}

// destructor
artemis_impl_state::~artemis_impl_state() {
    //tcam::destroy(key_native_tbl_);
    //tcam::destroy(key_tunneled_tbl_);
    //tcam::destroy(ingress_drop_stats_tbl_);
    //tcam::destroy(egress_drop_stats_tbl_);
    //tcam::destroy(nacl_tbl_);
}

/// \@}    // end of PDS_ARTEMIS_IMPL_STATE

}    // namespace impl
}    // namespace api
