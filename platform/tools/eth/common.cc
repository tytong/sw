/*
* Copyright (c) 2020, Pensando Systems Inc.
*/

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#ifdef ELBA
#include "elb_top_csr_defines.h"
#include "elb_wa_c_hdr.h"
#include "elb_pics_c_hdr.h"
#else
#include "cap_top_csr_defines.h"
#include "cap_wa_c_hdr.h"
#include "cap_pics_c_hdr.h"
#endif

#include "nic/sdk/lib/pal/pal.hpp"
#include "nic/sdk/lib/device/device.hpp"
#include "platform/utils/mpartition.hpp"

#include "gen/platform/mem_regions.hpp"

#include "impl.hpp"

bool
get_lif_qstate(uint16_t lif, queue_info_t qinfo[QTYPE_MAX])
{
#ifndef ELBA
    uint32_t cnt[4] = {0};
    uint32_t size[QTYPE_MAX] = {0};
    uint32_t length[QTYPE_MAX] = {0};

    sdk::lib::pal_reg_read(CAP_ADDR_BASE_DB_WA_OFFSET +
                               CAP_WA_CSR_DHS_LIF_QSTATE_MAP_BYTE_ADDRESS + (16 * lif),
                           cnt, 4);

    // decode lif qstate table:
    uint8_t valid = CAP_WA_CSR_DHS_LIF_QSTATE_MAP_ENTRY_ENTRY_0_4_VLD_GET(cnt[0]);
    if (!valid) {
        printf("Invalid lif %u\n", lif);
        return false;
    }

    uint64_t base =
        (uint64_t)CAP_WA_CSR_DHS_LIF_QSTATE_MAP_ENTRY_ENTRY_0_4_QSTATE_BASE_GET(cnt[0]);
    // Qstate Size: 3 bit size is qstate size (32B/64B/128B)
    size[0] = CAP_WA_CSR_DHS_LIF_QSTATE_MAP_ENTRY_ENTRY_0_4_SIZE0_GET(cnt[0]);
    size[1] = CAP_WA_CSR_DHS_LIF_QSTATE_MAP_ENTRY_ENTRY_1_4_SIZE1_GET(cnt[1]);
    size[2] = CAP_WA_CSR_DHS_LIF_QSTATE_MAP_ENTRY_ENTRY_1_4_SIZE2_GET(cnt[1]);
    size[3] = CAP_WA_CSR_DHS_LIF_QSTATE_MAP_ENTRY_ENTRY_1_4_SIZE3_GET(cnt[1]);
    size[4] = CAP_WA_CSR_DHS_LIF_QSTATE_MAP_ENTRY_ENTRY_1_4_SIZE4_GET(cnt[1]);
    size[5] = CAP_WA_CSR_DHS_LIF_QSTATE_MAP_ENTRY_ENTRY_2_4_SIZE5_GET(cnt[2]);
    size[6] = CAP_WA_CSR_DHS_LIF_QSTATE_MAP_ENTRY_ENTRY_2_4_SIZE6_GET(cnt[2]);
    size[7] = CAP_WA_CSR_DHS_LIF_QSTATE_MAP_ENTRY_ENTRY_2_4_SIZE7_GET(cnt[2]);
    // log2(number_of_queues)
    length[0] = CAP_WA_CSR_DHS_LIF_QSTATE_MAP_ENTRY_ENTRY_0_4_LENGTH0_GET(cnt[0]);
    length[1] = CAP_WA_CSR_DHS_LIF_QSTATE_MAP_ENTRY_ENTRY_0_4_LENGTH1_0_0_GET(cnt[0]) |
                (CAP_WA_CSR_DHS_LIF_QSTATE_MAP_ENTRY_ENTRY_1_4_LENGTH1_4_1_GET(cnt[1]) << 1);
    length[2] = CAP_WA_CSR_DHS_LIF_QSTATE_MAP_ENTRY_ENTRY_1_4_LENGTH2_GET(cnt[1]);
    length[3] = CAP_WA_CSR_DHS_LIF_QSTATE_MAP_ENTRY_ENTRY_1_4_LENGTH3_GET(cnt[1]);
    length[4] = CAP_WA_CSR_DHS_LIF_QSTATE_MAP_ENTRY_ENTRY_1_4_LENGTH4_GET(cnt[1]);
    length[5] = CAP_WA_CSR_DHS_LIF_QSTATE_MAP_ENTRY_ENTRY_1_4_LENGTH5_0_0_GET(cnt[1]) |
                (CAP_WA_CSR_DHS_LIF_QSTATE_MAP_ENTRY_ENTRY_2_4_LENGTH5_4_1_GET(cnt[2]) << 1);
    length[6] = CAP_WA_CSR_DHS_LIF_QSTATE_MAP_ENTRY_ENTRY_2_4_LENGTH6_GET(cnt[2]);
    length[7] = CAP_WA_CSR_DHS_LIF_QSTATE_MAP_ENTRY_ENTRY_2_4_LENGTH7_GET(cnt[2]);
#else
    uint32_t cnt[8] = {0};
    uint32_t size[QTYPE_MAX] = {0};
    uint32_t length[QTYPE_MAX] = {0};

    sdk::lib::pal_reg_read(ELB_ADDR_BASE_DB_WA_OFFSET +
                               ELB_WA_CSR_DHS_LIF_QSTATE_MAP_BYTE_ADDRESS + (32 * lif),
                           cnt, 8);

    // decode lif qstate table:
    uint8_t valid = ELB_WA_CSR_DHS_LIF_QSTATE_MAP_ENTRY_ENTRY_0_8_VLD_GET(cnt[0]);
    if (!valid) {
        printf("Invalid lif %u\n", lif);
        return false;
    }

    uint64_t base =
        (uint64_t)ELB_WA_CSR_DHS_LIF_QSTATE_MAP_ENTRY_ENTRY_0_8_QSTATE_BASE_GET(cnt[0]);
    // Qstate Size: 3 bit size is qstate size (32B/64B/128B)
    size[0] = (ELB_WA_CSR_DHS_LIF_QSTATE_MAP_ENTRY_ENTRY_1_8_SIZE0_2_1_GET(cnt[1])<<1)|ELB_WA_CSR_DHS_LIF_QSTATE_MAP_ENTRY_ENTRY_0_8_SIZE0_0_0_GET(cnt[0]);
    size[1] = (ELB_WA_CSR_DHS_LIF_QSTATE_MAP_ENTRY_ENTRY_1_8_SIZE1_GET(cnt[1]));
    size[2] = (ELB_WA_CSR_DHS_LIF_QSTATE_MAP_ENTRY_ENTRY_1_8_SIZE2_GET(cnt[1]));
    size[3] = (ELB_WA_CSR_DHS_LIF_QSTATE_MAP_ENTRY_ENTRY_1_8_SIZE3_GET(cnt[1]));
    size[4] = (ELB_WA_CSR_DHS_LIF_QSTATE_MAP_ENTRY_ENTRY_2_8_SIZE4_2_1_GET(cnt[2])<<1)|ELB_WA_CSR_DHS_LIF_QSTATE_MAP_ENTRY_ENTRY_1_8_SIZE4_0_0_GET(cnt[1]);
    size[5] = (ELB_WA_CSR_DHS_LIF_QSTATE_MAP_ENTRY_ENTRY_2_8_SIZE5_GET(cnt[2]));
    size[6] = (ELB_WA_CSR_DHS_LIF_QSTATE_MAP_ENTRY_ENTRY_2_8_SIZE6_GET(cnt[2]));
    size[7] = (ELB_WA_CSR_DHS_LIF_QSTATE_MAP_ENTRY_ENTRY_2_8_SIZE7_GET(cnt[2]));
    // log2(number_of_queues)
    length[0] = ELB_WA_CSR_DHS_LIF_QSTATE_MAP_ENTRY_ENTRY_0_8_LENGTH0_GET(cnt[0]);
    length[1] = ELB_WA_CSR_DHS_LIF_QSTATE_MAP_ENTRY_ENTRY_1_8_LENGTH1_GET(cnt[1]);
    length[2] = ELB_WA_CSR_DHS_LIF_QSTATE_MAP_ENTRY_ENTRY_1_8_LENGTH2_GET(cnt[1]);
    length[3] = ELB_WA_CSR_DHS_LIF_QSTATE_MAP_ENTRY_ENTRY_1_8_LENGTH3_GET(cnt[1]);
    length[4] = ELB_WA_CSR_DHS_LIF_QSTATE_MAP_ENTRY_ENTRY_1_8_LENGTH4_GET(cnt[1]);
    length[5] = ELB_WA_CSR_DHS_LIF_QSTATE_MAP_ENTRY_ENTRY_2_8_LENGTH5_GET(cnt[2]);
    length[6] = ELB_WA_CSR_DHS_LIF_QSTATE_MAP_ENTRY_ENTRY_2_8_LENGTH6_GET(cnt[2]);
    length[7] = ELB_WA_CSR_DHS_LIF_QSTATE_MAP_ENTRY_ENTRY_2_8_LENGTH7_GET(cnt[2]);
#endif

    base = base << 12;

    for (uint8_t qtype = 0; qtype < QTYPE_MAX && size[qtype] != 0; qtype++) {

        qinfo[qtype].base = base;
        qinfo[qtype].size = 32 << size[qtype];
        qinfo[qtype].length = 1 << length[qtype];

        base += qinfo[qtype].size * qinfo[qtype].length;
    }

    return true;
}


std::string
hal_cfg_path()
{
    std::string hal_cfg_path_;
    if (std::getenv("HAL_CONFIG_PATH") == NULL) {
        hal_cfg_path_ = "/nic/conf/";
    } else {
        hal_cfg_path_ = std::string(std::getenv("HAL_CONFIG_PATH"));
    }

    return hal_cfg_path_;
}

std::string
mpart_cfg_path()
{
    std::string mpart_json;
    std::string hal_cfg_path_ = hal_cfg_path();

    // WARNING -- this must be picked based on profile, this is guaranteed to be
    // broken soon
#if defined(APOLLO)
    mpart_json = hal_cfg_path_ + "/apollo/hbm_mem.json";
#elif defined(ARTEMIS)
    mpart_json = hal_cfg_path_ + "/artemis/hbm_mem.json";
#elif defined(APULU)
    mpart_json = hal_cfg_path_ + "/apulu/8g/hbm_mem.json";
#elif defined(ATHENA)
    mpart_json = hal_cfg_path_ + "/athena/4g/hbm_mem.json";
#else
    std::string profile_name;
    sdk::lib::dev_feature_profile_t feature_profile;
    sdk::lib::device *device =
        sdk::lib::device::factory("/sysconfig/config0/device.conf");

    feature_profile = device->get_feature_profile();
    profile_name = std::string(DEV_FEATURE_PROFILE_str(feature_profile));
    profile_name.replace(0, std::string("FEATURE_PROFILE").length(), "");
    std::transform(profile_name.begin(), profile_name.end(),
                   profile_name.begin(), ::tolower);
    mpart_json =  hal_cfg_path_ + "/" +
        "iris" + "/hbm_mem" + profile_name + ".json";
#endif

    return mpart_json;
}

void
qinfo(uint16_t lif)
{
    queue_info_t qinfo[QTYPE_MAX] = {0};

    if (!get_lif_qstate(lif, qinfo)) {
        printf("Failed to get qinfo for lif %u\n", lif);
        return;
    }

    for (int qtype = 0; qtype < 8; qtype++) {
        printf("qtype: %d    base: %12lx    size: %6u    count: %6u\n", qtype,
                qinfo[qtype].base, qinfo[qtype].size, qinfo[qtype].length);
    }
}

uint8_t *
parse_byte_array(char **argv, uint32_t size)
{
    uint8_t *src = (uint8_t *)calloc(1, size);
    if (src == NULL) {
        printf("Failed to srcfer\n");
        return NULL;
    }

    for (uint32_t i = 0; i < size; i++) {
        src[i] = strtoul(argv[i], NULL, 16);
    }

    return src;
}

void
mem_rd(uint64_t addr, uint32_t size)
{
    uint8_t *src = (uint8_t *)sdk::lib::pal_mem_map(addr, size);
    if (src == NULL) {
        printf("Failed to map address\n");
        return;
    }

    for (uint32_t i = 0; i < size / 16; i++) {
        printf("%0lx : ", addr + (i * 16));
        for (uint32_t j = 0; j < 16; j++) {
            printf("%02x ", src[(i * 16) + j]);
        }
        printf("\n");
    }
    printf("\n");

    sdk::lib::pal_mem_unmap((void *)src);
}

void
mem_wr(uint64_t addr, uint32_t size, uint8_t *src)
{
    uint8_t *dst = (uint8_t *)sdk::lib::pal_mem_map(addr, size);
    if (dst == NULL) {
        printf("Failed to map address\n");
        return;
    }

    memcpy(dst, src, size);

    sdk::lib::pal_mem_unmap((void *)dst);
}

void
mem_dump(uint64_t addr, uint32_t size, char *filepath)
{
    uint8_t *src = (uint8_t *)sdk::lib::pal_mem_map(addr, size);
    if (src == NULL) {
        printf("Failed to map address\n");
        return;
    }

    FILE *fp = fopen(filepath, "wb");
    if (fp == NULL) {
        printf("Failed to open file for writing\n");
        return;
    }

    fwrite(src, sizeof(src[0]), size, fp);
    fclose(fp);

    sdk::lib::pal_mem_unmap((void *)src);
}

void
mem_bzero(uint64_t addr, uint32_t size)
{
    uint8_t *dst = (uint8_t *)sdk::lib::pal_mem_map(addr, size);
    if (dst == NULL) {
        printf("Failed to map address\n");
        return;
    }

    memset(dst, 0, size);

    sdk::lib::pal_mem_unmap((void *)dst);
}

void
mem_fill(uint64_t addr, uint32_t size, uint8_t *pattern, uint32_t pattern_sz)
{
    uint8_t *dst = (uint8_t *)sdk::lib::pal_mem_map(addr, size);
    if (dst == NULL) {
        printf("Failed to map address\n");
        return;
    }

    uint32_t fill_sz = 0;
    for (uint32_t i = 0; i < size; i += pattern_sz) {
        fill_sz = min(pattern_sz, size - i);
        memcpy(&dst[i], pattern, fill_sz);
    }

    sdk::lib::pal_mem_unmap((void *)dst);
}

void
mem_find(uint64_t addr, uint32_t size, uint8_t *pattern, uint32_t pattern_sz)
{
    uint8_t *src = (uint8_t *)sdk::lib::pal_mem_map(addr, size);
    if (src == NULL) {
        printf("Failed to map address\n");
        return;
    }

    for (uint32_t i = 0; i < size - pattern_sz; i++) {
        if (!memcmp((void *)&src[i], pattern, pattern_sz)) {
            printf("0x%lx\n", addr + i);
            goto exit;
        }
    }
    printf("Pattern not found in region 0x%lx - 0x%lx\n",
        addr, addr + size);

exit:
    sdk::lib::pal_mem_unmap((void *)src);
}

void
mem_nfind(uint64_t addr, uint32_t size, uint8_t *pattern, uint32_t pattern_sz)
{
    uint8_t *src = (uint8_t *)sdk::lib::pal_mem_map(addr, size);
    if (src == NULL) {
        printf("Failed to map address\n");
        return;
    }

    for (uint32_t i = 0; i < size - pattern_sz; i++) {
        if (memcmp((void *)&src[i], pattern, pattern_sz)) {
            printf("0x%lx\n", addr + i);
            goto exit;
        }
    }
    printf("Pattern not found in region 0x%lx - 0x%lx\n",
        addr, addr + size);

exit:
    sdk::lib::pal_mem_unmap((void *)src);
}
