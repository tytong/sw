// {C} Copyright 2017 Pensando Systems Inc. All rights reserved

#ifndef __ELBA_HBM_RW_HPP__
#define __ELBA_HBM_RW_HPP__

#include "platform/utils/mpartition.hpp"
#include "p4/loader/loader.hpp"
#include "asic/asic.hpp"

namespace sdk {
namespace platform {
namespace elba {

using sdk::platform::utils::mpartition_region_t;

#define ELBA_RBM_BRIDGE_(x)  (ELB_ADDR_BASE_MS_RBM_OFFSET + NS_SOC_IP_MEMORYMAP_RBM_M_NOC_REGISTERS_BRIDGE_##x##_BYTE_ADDRESS)
#define ELBA_RBM_AGENT_(x)   (ELB_ADDR_BASE_MS_RBM_OFFSET + NS_SOC_IP_MEMORYMAP_RBM_M_NOC_REGISTERS_AGENT_##x##_BYTE_ADDRESS)

#define PAGE_SCRATCH_SIZE               512
#define PAGE_SCRATCH_SIZE_BYTES         8

#define    JUMBO_FRAME_SIZE             9216
#define    ETH_FRAME_SIZE               1536


#define MEM_REGION_MCAST_REPL_NAME "mcast_repl"

mpartition_region_t *get_mem_region(char *name);
uint64_t get_mem_base(void);
uint64_t get_mem_offset(const char *reg_name);
uint64_t get_mem_addr(const char *reg_name);
uint32_t get_mem_size_kb(const char *reg_name);



typedef struct elba_descr_s {
        uint64_t        scratch[8];
        uint64_t        A0;
        uint32_t        O0;
        uint32_t        L0;
        uint64_t        A1;
        uint32_t        O1;
        uint32_t        L1;
        uint64_t        A2;
        uint32_t        O2;
        uint32_t        L2;
        uint64_t        next_addr;
        uint64_t        reserved;
} elba_descr_t;

typedef struct elba_big_page_s {
        uint64_t        scratch[PAGE_SCRATCH_SIZE_BYTES];
        char            data[JUMBO_FRAME_SIZE];
} elba_big_page_t;

typedef struct elba_small_page_s {
        uint64_t        scratch[PAGE_SCRATCH_SIZE_BYTES];
        char            data[ETH_FRAME_SIZE];
} elba_small_page_t;

sdk_ret_t elba_hbm_bw(uint32_t samples, uint32_t u_sleep,
                      bool ms_pcie, asic_hbm_bw_t *hbm_bw_arr);
sdk_ret_t elba_nx_get_llc_counters(uint32_t *mask, uint32_t *rd_data);
sdk_ret_t elba_nx_setup_llc_counters(uint32_t mask);
uint32_t elba_freq_get(void);

sdk_ret_t elba_hbm_cache_init(asic_cfg_t *cfg);
sdk_ret_t elba_hbm_cache_regions_init(void);
mpartition_region_t* get_hbm_region_by_address(uint64_t addr);

}    // namespace elba
}    // namespace platform
}    // namespace sdk

using sdk::platform::elba::get_mem_addr;
using sdk::platform::elba::get_mem_size_kb;

#endif    // __ELBA_HBM_RW_HPP__
