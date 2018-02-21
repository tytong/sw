#include "nic/include/base.h"
#include "nic/include/capri_common.h"
#include <unistd.h>
#include <iostream>
#include "nic/hal/pd/capri/capri_hbm.hpp"
#include "nic/include/hal_mem.hpp"
#include "nic/include/asic_pd.hpp"
#include "boost/foreach.hpp"
#include "boost/optional.hpp"
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/json_parser.hpp"
#include <arpa/inet.h>

namespace pt = boost::property_tree;

static capri_hbm_region_t *hbm_regions_;
static int num_hbm_regions_;

#define HBM_OFFSET(x)       CAPRI_HBM_OFFSET(x)

hal_ret_t
capri_hbm_parse (capri_cfg_t *cfg)
{
    pt::ptree               json_pt;
    std::string             full_path;
    capri_hbm_region_t      *reg;

    // makeup the full file path
    full_path =  cfg->cfg_path + "/" + cfg->pgm_name +
                     "/" + std::string("hbm_mem.json");

    HAL_TRACE_DEBUG("HBM Memory Json: {}", full_path.c_str());

    // make sure cfg file exists
    if (access(full_path.c_str(), R_OK) < 0) {
        HAL_TRACE_ERR("{} not_present/no_read_permissions", full_path.c_str());
        HAL_ASSERT_RETURN(0, HAL_RET_ERR);
    }

    std::ifstream hbm_json(full_path.c_str());

    // Reading from file
    read_json(hbm_json, json_pt);

    boost::optional<pt::ptree&>reg_pt = json_pt.get_child_optional(JKEY_REGIONS);
    if (!reg_pt) {
        return HAL_RET_ERR;
    }

    num_hbm_regions_ = json_pt.get_child(JKEY_REGIONS).size();
    hbm_regions_ = (capri_hbm_region_t *)
        HAL_CALLOC(hal::HAL_MEM_ALLOC_PD, num_hbm_regions_ * sizeof(capri_hbm_region_t));
    if (!hbm_regions_) {
        return HAL_RET_OOM;
    }

    int idx = 0;
    uint64_t offset = 0;
    BOOST_FOREACH(pt::ptree::value_type &p4_tbl, json_pt.get_child(JKEY_REGIONS)) {

        reg = hbm_regions_ + idx;

        std::string reg_name = p4_tbl.second.get<std::string>(JKEY_REGION_NAME);


        strcpy(reg->mem_reg_name, reg_name.c_str());
        reg->size_kb = p4_tbl.second.get<int>(JKEY_SIZE_KB);
        // reg->start_offset = p4_tbl.second.get<int>(JKEY_START_OFF);
        reg->start_offset = offset;

        HAL_TRACE_DEBUG("Region: {0:}, Size_KB: {1:}, Start_Off: 0x{2:x}", 
                reg->mem_reg_name, reg->size_kb, HBM_OFFSET(reg->start_offset));

        offset += reg->size_kb * 1024;
        idx++;
    }

    return HAL_RET_OK;
}

uint64_t
get_hbm_base(void)
{
    return HBM_OFFSET(0);
}

uint64_t
get_hbm_offset(const char *reg_name)
{
    capri_hbm_region_t *reg;

    for (int i = 0; i < num_hbm_regions_; i++) {
        reg = &hbm_regions_[i];
        if (!strcmp(reg->mem_reg_name, reg_name)) {
            return (reg->start_offset);
        }
    }

    return 0;
}

uint64_t
get_start_offset(const char *reg_name)
{
    capri_hbm_region_t      *reg;

    for (int i = 0; i < num_hbm_regions_; i++) {
        reg = &hbm_regions_[i];
        if (!strcmp(reg->mem_reg_name, reg_name)) {
            return (HBM_OFFSET(reg->start_offset));
        }
    }

    return 0;
}

uint32_t
get_size_kb(const char *reg_name)
{
    capri_hbm_region_t      *reg;

    for (int i = 0; i < num_hbm_regions_; i++) {
        reg = &hbm_regions_[i];
        if (!strcmp(reg->mem_reg_name, reg_name)) {
            return reg->size_kb;
        }
    }

    return 0;
}

capri_hbm_region_t *
get_hbm_region(char *reg_name)
{
    capri_hbm_region_t      *reg;

    for (int i = 0; i < num_hbm_regions_; i++) {
        reg = &hbm_regions_[i];
        if (!strcmp(reg->mem_reg_name, reg_name)) {
            return reg;
        }
    }
    return NULL;
}

int32_t
capri_hbm_read_mem(uint64_t addr, uint8_t *buf, uint32_t size)
{
    hal_ret_t rc = hal::pd::asic_mem_read(addr, buf, size);
    return (rc == HAL_RET_OK) ? 0 : -EIO;
}

int32_t
capri_hbm_write_mem(uint64_t addr, uint8_t *buf, uint32_t size)
{
    hal_ret_t rc = hal::pd::asic_mem_write(addr, buf, size, true);
    return (rc == HAL_RET_OK) ? 0 : -EIO;
}
