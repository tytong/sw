//-----------------------------------------------------------------------------
// {C} Copyright 2017 Pensando Systems Inc. All rights reserved
//-----------------------------------------------------------------------------

#include "nic/include/base.h"
#include "nic/hal/src/debug/table.hpp"
#include "nic/include/hal_state.hpp"
#include "nic/include/pd_api.hpp"

using sdk::lib::slab;

namespace hal {

//------------------------------------------------------------------------------
// process a table metadata get
//------------------------------------------------------------------------------
hal_ret_t
table_metadata_get (table::TableMetadataResponseMsg *rsp)
{
    hal_ret_t   ret = HAL_RET_OK;
    hal::pd::pd_table_metadata_get_args_t args;
    pd::pd_func_args_t pd_func_args = {0};

    args.rsp = rsp;

    pd_func_args.pd_table_metadata_get = &args;
    ret = hal::pd::hal_pd_call(hal::pd::PD_FUNC_ID_TABLE_METADATA_GET,
                               &pd_func_args);
    rsp->set_api_status(types::API_STATUS_OK);

    return ret;
}

//------------------------------------------------------------------------------
// process a table get
//------------------------------------------------------------------------------
hal_ret_t
table_get (TableSpec& spec, TableResponse *rsp)
{
    hal_ret_t   ret = HAL_RET_OK;
    hal::pd::pd_table_get_args_t args;
    pd::pd_func_args_t pd_func_args = {0};

    args.spec = &spec;
    args.rsp = rsp;

    pd_func_args.pd_table_get = &args;
    ret = hal::pd::hal_pd_call(hal::pd::PD_FUNC_ID_TABLE_GET, &pd_func_args);
    rsp->set_api_status(types::API_STATUS_OK);


    return ret;
}


}    // namespace hal
