//
// {C} Copyright 2020 Pensando Systems Inc. All rights reserved
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the implementation for csr init func
///
//===----------------------------------------------------------------------===//

#include "platform/csr/asicrw_if.hpp"
#include "third-party/asic/elba/model/utils/elb_csr_py_if.h"
#include "third-party/asic/elba/model/elb_top/elb_top_csr.h"

namespace sdk {
namespace platform {
namespace elba {

void
csr_init (void)
{
    static bool csr_init_done = false;

    if (csr_init_done) {
       return;
    }

    // Register for hal cpu interface
    auto cpu_if = new cpu_hal_if("cpu", "all");
    cpu::access()->add_if("cpu_if", cpu_if);
    cpu::access()->set_cur_if_name("cpu_if");

    // Register at top level for all MRL classes.
    elb_top_csr_t *elb0_ptr = new elb_top_csr_t("elb0");

    elb0_ptr->init(0);
    ELB_BLK_REG_MODEL_REGISTER(elb_top_csr_t, 0, 0, elb0_ptr);
    register_chip_inst("elb0", 0, 0);
    csr_init_done = true;
}

}    // end namespace elba
}    // end namespace platform
}    // end namespace sdk
