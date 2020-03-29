// {C} Copyright 2020 Pensando Systems Inc. All rights reserved
//
//----------------------------------------------------------------------------
///
/// \file
/// Implementation for core upgrade data structures, methods, and APIs
///
//----------------------------------------------------------------------------


#include <iostream>
#include <ev.h>
#include <boost/unordered_map.hpp>
#include <boost/container/vector.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include "nic/sdk/include/sdk/base.hpp"
#include "nic/apollo/upgrade/core/stage.hpp"
#include "nic/apollo/upgrade/core/service.hpp"
#include "nic/apollo/upgrade/core/idl.hpp"
#include "nic/apollo/upgrade/core/fsm.hpp"
#include "nic/apollo/upgrade/include/upgrade.hpp"

namespace upg
{

void           token_parse(std::string& text,
                           std::vector<std::string>& results);
ev_tstamp      str_to_timeout(const std::string& timeout);
upg_stage_t    name_to_stage_id(const std::string stage);
std::string    svc_sequence_to_str(const svc_sequence_list svcs);
std::string    transition_to_str(const transition_stages& transitions);
std::string    script_to_str(const upg_scripts& scripts);
upg_scripts    str_to_scripts(std::string scripts);
svc_rsp_code_t svc_rsp_code(const upg_status_t id);
void           dump(const transition_stages& transitions);
void           dump(const upg_scripts& scripts);
void           dump(upg_stage& stage);
void           dump(upg_stages_map& stages);
void           dump(const upg_svc_map& svcs);
void           dump(const svc_sequence_list& svcs);
void           dump(const stage_map& tran);
void           dump(const fsm& fsm);

}
