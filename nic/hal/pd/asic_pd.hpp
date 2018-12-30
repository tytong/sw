// {C} Copyright 2017 Pensando Systems Inc. All rights reserved

#ifndef __HAL_PD_ASIC_RW_HPP__
#define __HAL_PD_ASIC_RW_HPP__

#include <vector>
#include <tuple>
#include "nic/sdk/include/sdk/catalog.hpp"
#include "include/sdk/platform/p4loader/loader.hpp"
#include "nic/include/base.hpp"
#include "nic/sdk/asic/asic.hpp"

using std::vector;
using std::tuple;

namespace hal {
namespace pd {

// asic init
typedef struct pd_asic_init_args_s {
    asic_cfg_t   *cfg;
} __PACK__ pd_asic_init_args_t;

#if 0
//------------------------------------------------------------------------------
// different modes of writing to ASIC
// 1. non-blocking - adds write operation to asicrw thread's work queue &
//                   returns
// 2. blocking     - adds write operation to asicrw thread's work queue & blocks
//                   until the operation is done by asicrw thread
// 3. write-thru   - non-blocking version that bypasses asicrw thread completely
//                   and writes in the caller thread's context
//------------------------------------------------------------------------------
typedef enum asic_write_mode_e {
    ASIC_WRITE_MODE_NON_BLOCKING = 0,
    ASIC_WRITE_MODE_BLOCKING     = 1,
    ASIC_WRITE_MODE_WRITE_THRU   = 2,
} asic_write_mode_t;

//------------------------------------------------------------------------------
// public API for register read operations
//------------------------------------------------------------------------------
hal_ret_t asic_reg_read(uint64_t addr, uint32_t *data, uint32_t num_words = 1,
                        bool read_thru=false);

//------------------------------------------------------------------------------
// public API for memory read operations
//------------------------------------------------------------------------------
hal_ret_t asic_mem_read(uint64_t addr, uint8_t *data, uint32_t len,
                        bool read_thru=false);

//------------------------------------------------------------------------------
// public API for register write operations
// write given data at specified address in the memory
//------------------------------------------------------------------------------
hal_ret_t asic_reg_write(uint64_t addr, uint32_t *data, uint32_t num_words = 1,
                         asic_write_mode_t mode = ASIC_WRITE_MODE_BLOCKING);

//------------------------------------------------------------------------------
// public API for memory write operations
// write given data at specified address in the memory
//------------------------------------------------------------------------------
hal_ret_t asic_mem_write(uint64_t addr, uint8_t *data, uint32_t len,
                         asic_write_mode_t mode = ASIC_WRITE_MODE_BLOCKING);

//------------------------------------------------------------------------------
// public API for ringing doorbells.
//------------------------------------------------------------------------------
hal_ret_t asic_ring_doorbell(uint64_t addr, uint64_t data,
                             asic_write_mode_t mode = ASIC_WRITE_MODE_BLOCKING);
#endif

//------------------------------------------------------------------------------
// public API for saving cpu packet.
//------------------------------------------------------------------------------
hal_ret_t asic_step_cpu_pkt(const uint8_t* pkt, size_t pkt_len);

#if 0
// starting point for asic read-write thread
void *asic_rw_start(void *ctxt);
#endif

// initialize the asic
hal_ret_t asic_init(asic_cfg_t *asic_cfg);
hal_ret_t asic_hbm_parse(asic_cfg_t *asic_cfg);

#if 0
// return TRUE if asic is initialized and ready for read/writes
bool is_asic_rw_ready(void);

// port related config
hal_ret_t asic_port_cfg(uint32_t port_num, uint32_t speed, uint32_t type,
                        uint32_t num_lanes, uint32_t val);
#endif

// check if the current thread is hal-control thread or not
bool is_hal_ctrl_thread(void);

#if 0
// check if this thread is the asic-rw thread or not
bool is_asic_rw_thread(void);

std::string asic_pd_csr_dump(char *csr_str);
std::string asic_csr_dump(char *csr_str);
vector< tuple <std::string, std::string, std::string>> asic_csr_dump_reg(char *block_name, bool exlude_mem);
vector<std::string> asic_csr_list_get(std::string path, int level);
#endif

}    // namespace pd
}    // namespace hal

#endif    // __HAL_PD_ASIC_RW_HPP__

