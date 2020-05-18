// {C} Copyright 2020 Pensando Systems Inc. All rights reserved

#include "gen/proto/types.pb.h"
#include "platform/elba/elba_barco_rings.hpp"
#include "third-party/asic/elba/model/elb_top/elb_top_csr.h"
#include "platform/elba/elba_barco_res.hpp"
#include "platform/elba/elba_barco_crypto.hpp"
#include "platform/elba/elba_common.hpp"
#include "platform/elba/elba_hbm_rw.hpp"
#include "asic/rw/asicrw.hpp"
#include "asic/cmn/asic_hbm.hpp"

namespace sdk {
namespace platform {
namespace elba {

sdk_ret_t elba_barco_asym_init(elba_barco_ring_t *barco_ring);
bool elba_barco_asym_poller(elba_barco_ring_t *barco_ring, uint32_t req_tag);
sdk_ret_t elba_barco_asym_queue_request(struct elba_barco_ring_s *barco_ring,
                                        void *req, uint32_t *req_tag, bool);

sdk_ret_t elba_barco_sym0_ring0_init(elba_barco_ring_t *barco_ring);
sdk_ret_t elba_barco_sym1_ring0_init(elba_barco_ring_t *barco_ring);

bool elba_barco_sym0_ring0_poller(elba_barco_ring_t *barco_ring,
                                  uint32_t req_tag);
bool elba_barco_sym1_ring0_poller(elba_barco_ring_t *barco_ring,
                                  uint32_t req_tag);

sdk_ret_t elba_barco_sym0_ring0_queue_request(struct elba_barco_ring_s *barco_ring,
                                              void *req, uint32_t *req_tag,
                                              bool);
sdk_ret_t elba_barco_sym1_ring0_queue_request(struct elba_barco_ring_s *barco_ring,
                                       void *req, uint32_t *req_tag, bool);

sdk_ret_t elba_barco_sym2_ring0_init(elba_barco_ring_t *barco_ring);
bool elba_barco_sym2_ring0_poller(elba_barco_ring_t *barco_ring,
                                  uint32_t req_tag);
sdk_ret_t elba_barco_sym2_ring0_queue_request(struct elba_barco_ring_s *barco_ring, void *req,
                                              uint32_t *req_tag, bool schedule_barco);

sdk_ret_t elba_barco_sym3_ring0_init(elba_barco_ring_t *barco_ring);
bool elba_barco_sym3_ring0_poller(elba_barco_ring_t *barco_ring, uint32_t req_tag);
sdk_ret_t elba_barco_sym3_ring0_queue_request(struct elba_barco_ring_s *barco_ring,
                                              void *req, uint32_t *req_tag,
                                              bool schedule_barco);

sdk_ret_t elba_barco_mpp_queue_request(struct elba_barco_ring_s *barco_ring,
                                       void *req, uint32_t *req_tag, bool);

sdk_ret_t elba_barco_mpp0_init(elba_barco_ring_t *barco_ring);
sdk_ret_t elba_barco_mpp0_queue_request(struct elba_barco_ring_s *barco_ring,
                                        void *req, uint32_t *req_tag, bool);
bool elba_barco_mpp0_poller(elba_barco_ring_t *barco_ring, uint32_t req_tag);

sdk_ret_t elba_barco_mpp1_init(elba_barco_ring_t *barco_ring);
sdk_ret_t elba_barco_mpp2_init(elba_barco_ring_t *barco_ring);
sdk_ret_t elba_barco_mpp3_init(elba_barco_ring_t *barco_ring);

sdk_ret_t elba_barco_cp_init(elba_barco_ring_t *barco_ring);
bool elba_barco_cp_poller(elba_barco_ring_t *barco_ring, uint32_t req_tag);

sdk_ret_t elba_barco_cp_hot_init(elba_barco_ring_t *barco_ring);
bool elba_barco_cp_hot_poller(elba_barco_ring_t *barco_ring, uint32_t req_tag);

sdk_ret_t elba_barco_dc_init(elba_barco_ring_t *barco_ring);
bool elba_barco_dc_poller(elba_barco_ring_t *barco_ring, uint32_t req_tag);

sdk_ret_t elba_barco_dc_hot_init(elba_barco_ring_t *barco_ring);
bool elba_barco_dc_hot_poller(elba_barco_ring_t *barco_ring, uint32_t req_tag);

static elba_barco_ring_t  barco_rings[] = {
    {   // BARCO_RING_ASYM
        BARCO_RING_ASYM_STR,
        ASIC_HBM_REG_BARCO_RING_ASYM,
        0,
        32,
        1024,
        32,
        0,
        0,
        0,
        0,
        sizeof(uint32_t),
        0,
        0,
        0,
        true,   // run-time modified based on platform
        false,
        elba_barco_asym_init,
        elba_barco_asym_poller,
        elba_barco_asym_queue_request,
    },
    {   // BARCO_RING_SYM0_RING0
        BARCO_RING_SYM0_RING0_STR,
        ASIC_HBM_REG_BARCO_RING_GCM0,
        0,
        BARCO_CRYPTO_DESC_ALIGN_BYTES,
        1024,
        BARCO_CRYPTO_DESC_SZ,
        0,
        0,
        0,
        0,
        sizeof(uint32_t),
        sizeof(uint32_t),
        0,
        0,
        true,   // run-time modified based on platform
        false,
        elba_barco_sym0_ring0_init,
        elba_barco_sym0_ring0_poller,
        elba_barco_sym0_ring0_queue_request,
    },
    {   // BARCO_RING_SYM1_RING0
        BARCO_RING_SYM1_RING0_STR,
        ASIC_HBM_REG_BARCO_RING_GCM1,
        0,
        BARCO_CRYPTO_DESC_ALIGN_BYTES,
        1024,
        BARCO_CRYPTO_DESC_SZ,
        0,
        0,
        0,
        0,
        sizeof(uint32_t),
        sizeof(uint32_t),
        0,
        0,
        true,   // run-time modified based on platform
        false,
        elba_barco_sym1_ring0_init,
        elba_barco_sym1_ring0_poller,
        elba_barco_sym1_ring0_queue_request,
    },
    {   // BARCO_RING_XTS0
        BARCO_RING_XTS0_STR,
        ASIC_HBM_REG_BARCO_RING_XTS0,
        0,
        BARCO_CRYPTO_DESC_ALIGN_BYTES,
        1024,
        BARCO_CRYPTO_DESC_SZ,
        0,
        0,
        0,
        0,
        sizeof(uint32_t),
        sizeof(uint32_t),
        0,
        0,
        true,   // run-time modified based on platform
        false,
        elba_barco_sym2_ring0_init,
        elba_barco_sym2_ring0_poller,
        NULL,   // elba_barco_sym2_ring0_queue_request,
    },
    {   // BARCO_RING_XTS1
        BARCO_RING_XTS1_STR,
        ASIC_HBM_REG_BARCO_RING_XTS1,
        0,
        BARCO_CRYPTO_DESC_ALIGN_BYTES,
        1024,
        BARCO_CRYPTO_DESC_SZ,
        0,
        0,
        0,
        0,
        sizeof(uint32_t),
        sizeof(uint32_t),
        0,
        0,
        true,   // run-time modified based on platform
        false,
        elba_barco_sym3_ring0_init,
        elba_barco_sym3_ring0_poller,
        NULL, // elba_barco_sym3_ring0_queue_request,
    },
    {   // BARCO_RING_MPP0
        BARCO_RING_MPP0_STR,
        ASIC_HBM_REG_BARCO_RING_MPP0,
        0,
        128,
        1024,
        128,
        0,
        0,
        0,
        0,
        sizeof(uint32_t),
        0,
        0,
        0,
        true,   // run-time modified based on platform
        false,
        elba_barco_mpp0_init,
        elba_barco_mpp0_poller,
        elba_barco_mpp0_queue_request,
    },
    {   // BARCO_RING_MPP1
        BARCO_RING_MPP1_STR,
        ASIC_HBM_REG_BARCO_RING_MPP1,
        0,
        128,
        1024,
        128,
        0,
        0,
        0,
        0,
        sizeof(uint32_t),
        0,
        0,
        0,
        true,   // run-time modified based on platform
        false,
        elba_barco_mpp1_init,
        NULL,
        NULL,
    },
    {   // BARCO_RING_MPP2
        BARCO_RING_MPP2_STR,
        ASIC_HBM_REG_BARCO_RING_MPP2,
        0,
        128,
        1024,
        128,
        0,
        0,
        0,
        0,
        sizeof(uint32_t),
        0,
        0,
        0,
        true,   // run-time modified based on platform
        false,
        elba_barco_mpp2_init,
        NULL,
        NULL,
    },
    {   // BARCO_RING_MPP3
        BARCO_RING_MPP3_STR,
        ASIC_HBM_REG_BARCO_RING_MPP3,
        0,
        128,
        1024,
        128,
        0,
        0,
        0,
        0,
        sizeof(uint32_t),
        0,
        0,
        0,
        true,   // run-time modified based on platform
        false,
        elba_barco_mpp3_init,
        NULL,
        NULL,
    },
    {   // BARCO_RING_MPP4
    },
    {   // BARCO_RING_MPP5
    },
    {   // BARCO_RING_MPP6
    },
    {   // BARCO_RING_MPP7
    },
    {   // BARCO_RING_CP
        BARCO_RING_CP_STR,
        ASIC_HBM_REG_BARCO_RING_CP,
        0,
        64,
        BARCO_CRYPTO_CP_RING_SIZE,
        64,
        0,
        0,
        0,
        0,
        sizeof(uint32_t),
        sizeof(uint32_t),
        0,
        0,
        true,   // run-time modified based on platform
        true,   //   "        "
        elba_barco_cp_init,
        elba_barco_cp_poller,
        NULL,
    },
    {   // BARCO_RING_CP_HOT
        BARCO_RING_CP_HOT_STR,
        ASIC_HBM_REG_BARCO_RING_CP_HOT,
        0,
        64,
        BARCO_CRYPTO_CP_HOT_RING_SIZE,
        64,
        0,
        0,
        0,
        0,
        0,
        sizeof(uint32_t),
        0,
        0,
        true,   // run-time modified based on platform
        true,   //   "        "
        elba_barco_cp_hot_init,
        elba_barco_cp_hot_poller,
        NULL,
    },
    {   // BARCO_RING_DC
        BARCO_RING_DC_STR,
        ASIC_HBM_REG_BARCO_RING_DC,
        0,
        64,
        BARCO_CRYPTO_DC_RING_SIZE,
        64,
        0,
        0,
        0,
        0,
        sizeof(uint32_t),
        sizeof(uint32_t),
        0,
        0,
        true,   // run-time modified based on platform
        true,   //   "        "
        elba_barco_dc_init,
        elba_barco_dc_poller,
        NULL,
    },
    {   // BARCO_RING_DC_HOT
        BARCO_RING_DC_HOT_STR,
        ASIC_HBM_REG_BARCO_RING_DC_HOT,
        0,
        64,
        BARCO_CRYPTO_DC_HOT_RING_SIZE,
        64,
        0,
        0,
        0,
        0,
        0,
        sizeof(uint32_t),
        0,
        0,
        true,   // run-time modified based on platform
        true,   //   "        "
        elba_barco_dc_hot_init,
        elba_barco_dc_hot_poller,
        NULL,
    },
};

sdk_ret_t
elba_barco_ring_common_init (elba_barco_ring_t *barco_ring)
{
    uint64_t                            ring_base = 0;
    uint32_t                            ring_size = 0;

    ring_base = get_mem_addr(barco_ring->hbm_region);
    if (ring_base == INVALID_MEM_ADDRESS) {
        SDK_TRACE_ERR("Failed to retrieve Barco Ring memory region for %s",
                      barco_ring->ring_name);
        return SDK_RET_ERR;
    }

    if (ring_base & (barco_ring->ring_alignment - 1)) {
        SDK_TRACE_ERR("Failed to retrieve aligned Barco Ring memory region for %s",
                      barco_ring->ring_name);
        return SDK_RET_ERR;
    }

    ring_size = get_mem_size_kb(barco_ring->hbm_region) * 1024;
    if (ring_size < (uint32_t)(barco_ring->ring_size * barco_ring->descriptor_size)) {
        SDK_TRACE_ERR("Not enough memory for Barco Ring memory region %s",
                      barco_ring->ring_name);
        return SDK_RET_ERR;
    }

    barco_ring->ring_base = ring_base;

    return SDK_RET_OK;
}

sdk_ret_t
elba_barco_asym_key_array_init (void)
{
    elb_top_csr_t &cap0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    elb_hese_csr_t &hese = cap0.md.hese;
    sdk_ret_t ret = SDK_RET_OK;
    uint64_t asym_key_array_base;
    uint32_t asym_key_array_key_count;

    ret = elba_barco_res_region_get(CRYPTO_BARCO_RES_ASYM_KEY_DESCR,
                                    &asym_key_array_base);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR("Could not retrieve the Asym Crypto Key Descr region");
        return ret;
    }

    ret = elba_barco_res_obj_count_get(CRYPTO_BARCO_RES_ASYM_KEY_DESCR,
                                       &asym_key_array_key_count);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR("Could not retrieve the Asym Crypto Key Descr count");
        return ret;
    }

    hese.dhs_crypto_ctl.pk0_key_array_base_w0.fld(asym_key_array_base &
                                                  0xffffffff);
    hese.dhs_crypto_ctl.pk0_key_array_base_w0.write();
    hese.dhs_crypto_ctl.pk0_key_array_base_w1.fld(asym_key_array_base >> 32);
    hese.dhs_crypto_ctl.pk0_key_array_base_w1.write();

    hese.dhs_crypto_ctl.pk0_key_array_size.fld(asym_key_array_key_count);
    hese.dhs_crypto_ctl.pk0_key_array_size.write();

    /*
     * Enable the read/write cache for AXI memory accesses for Barco descriptors, data-read,
     * data-write and status/opaque-tag/doorbell writes.
     */
    hese.dhs_crypto_ctl.pk0_axi_desc.fld(0x3c0);
    hese.dhs_crypto_ctl.pk0_axi_desc.write();
    hese.dhs_crypto_ctl.pk0_axi_data_read.fld(0x3c0);
    hese.dhs_crypto_ctl.pk0_axi_data_read.write();
    hese.dhs_crypto_ctl.pk0_axi_data_write.fld(0x3c0);
    hese.dhs_crypto_ctl.pk0_axi_data_write.write();
    hese.dhs_crypto_ctl.pk0_axi_status.fld(0x3c0);
    hese.dhs_crypto_ctl.pk0_axi_status.write();

    SDK_TRACE_DEBUG("Barco Asym Key Descriptor Array of count %u setup @ 0x%lx",
                    asym_key_array_key_count, asym_key_array_base);

    return ret;
}

sdk_ret_t
elba_barco_asym_init (elba_barco_ring_t *barco_ring)
{
    elb_top_csr_t &cap0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    elb_hens_csr_t &hens = cap0.md.hens;
    sdk_ret_t ret = SDK_RET_OK;

    ret = elba_barco_ring_common_init(barco_ring);
    if (ret != SDK_RET_OK) {
        return ret;
    }

    /* Reset Asym ring */
    hens.dhs_crypto_ctl.pk0_soft_rst.fld(0xffffffff);
    hens.dhs_crypto_ctl.pk0_soft_rst.write();
    /* Bring out of reset */
    hens.dhs_crypto_ctl.pk0_soft_rst.fld(0);
    hens.dhs_crypto_ctl.pk0_soft_rst.write();

    hens.dhs_crypto_ctl.pk0_ring_base_w0.fld((uint32_t)(barco_ring->ring_base &
                                                        0xffffffff));
    hens.dhs_crypto_ctl.pk0_ring_base_w0.write();
    hens.dhs_crypto_ctl.pk0_ring_base_w1.fld((uint32_t)(barco_ring->ring_base >> 32));
    hens.dhs_crypto_ctl.pk0_ring_base_w1.write();

    hens.dhs_crypto_ctl.pk0_ring_size.fld(barco_ring->ring_size);
    hens.dhs_crypto_ctl.pk0_ring_size.write();

    hens.dhs_crypto_ctl.pk0_ci_addr_w0.fld((uint32_t)(barco_ring->opaque_tag_addr & 0xffffffff));
    hens.dhs_crypto_ctl.pk0_ci_addr_w0.write();
    hens.dhs_crypto_ctl.pk0_ci_addr_w1.fld((uint32_t)(barco_ring->opaque_tag_addr >> 32));
    hens.dhs_crypto_ctl.pk0_ci_addr_w1.write();

    hens.dhs_crypto_ctl.pk0_producer_idx.fld(barco_ring->producer_idx);
    hens.dhs_crypto_ctl.pk0_producer_idx.write();

    // CI is read-only
    //hens.dhs_crypto_ctl.pk0_consumer_idx.fld(barco_ring->consumer_idx);
    //hens.dhs_crypto_ctl.pk0_consumer_idx.write();

    SDK_TRACE_DEBUG("Barco ring \"%s\" base setup @ 0x%lx, descriptor count %u",
                    barco_ring->ring_name, barco_ring->ring_base,
                    barco_ring->ring_size);

    return elba_barco_asym_key_array_init();
}

bool
elba_barco_asym_poller (elba_barco_ring_t *barco_ring, uint32_t req_tag)
{
    bool                                ret = FALSE;
    uint32_t                            curr_opaque_tag = 0;

    /* The opaque tag address is used as the CI address for the Asym ring */
    if (sdk::asic::asic_mem_read(barco_ring->opaque_tag_addr,
                                 (uint8_t*)&curr_opaque_tag,
                                 sizeof(curr_opaque_tag))) {
        SDK_TRACE_ERR("Poll:%s: Failed to retrieve current opaque tag value @ 0x%lx",
                      barco_ring->ring_name,
                      (uint64_t) barco_ring->opaque_tag_addr);
        return FALSE;
    }
    else {
        if ((int32_t(curr_opaque_tag - req_tag)) > 0) {
            SDK_TRACE_DEBUG("Poll:%s: Check for req: 0x%x: Retrieved opaque tag addr: 0x%lx value: 0x%x",
                            barco_ring->ring_name, req_tag,
                            barco_ring->opaque_tag_addr, curr_opaque_tag);

            ret = TRUE;
        }
    }

    return ret;
}

sdk_ret_t
elba_barco_asym_queue_request (struct elba_barco_ring_s *barco_ring,
                               void *req, uint32_t *req_tag,
                               bool schedule_barco)
{
    elb_top_csr_t &cap0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    elb_hens_csr_t &hens = cap0.md.hens;
    uint64_t slot_addr = 0;
    sdk_ret_t ret = SDK_RET_OK;

    slot_addr = barco_ring->ring_base + (barco_ring->producer_idx *
                                         barco_ring->descriptor_size);

    if (sdk::asic::asic_mem_write(slot_addr, (uint8_t*)req,
                                  barco_ring->descriptor_size)) {
        SDK_TRACE_ERR("Failed to write descriptor entry for %s  @ 0x%lx",
                      barco_ring->ring_name,
                      (uint64_t) slot_addr);
        ret = SDK_RET_INVALID_ARG;
    }
    else {
        *req_tag = barco_ring->producer_idx;
        barco_ring->producer_idx = (barco_ring->producer_idx + 1) &
            (barco_ring->ring_size - 1);

        /* Barco doorbell */
        hens.dhs_crypto_ctl.pk0_producer_idx.fld(barco_ring->producer_idx);
        hens.dhs_crypto_ctl.pk0_producer_idx.write();
    }

    return ret;
}

/*
 * Only encryption works on xts0 and decryption on xts1
 */
sdk_ret_t
elba_barco_xts0_key_array_init (void)
{
    elb_top_csr_t &cap0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    elb_hese_csr_t &hese = cap0.md.hese;
    sdk_ret_t ret = SDK_RET_OK;
    uint64_t key_array_base;
    uint32_t key_array_key_count;
    char  key_desc_array[] = ELBA_BARCO_KEY_DESC;
    uint32_t region_sz = 0;

    // Currently sharing the same key descriptor array as GCM
    // Eventually all symmetric protocols will share one large key array
    key_array_base = get_mem_addr(key_desc_array);
    SDK_ASSERT(key_array_base != INVALID_MEM_ADDRESS);
    /* All regions in hbm_mem.json are in multiples of 1kb and hence should already be aligned to 16byte
     * but confirm
     */
    assert((key_array_base & (BARCO_CRYPTO_KEY_DESC_ALIGN_BYTES - 1)) == 0);
    region_sz = get_mem_size_kb(key_desc_array) * 1024;
    key_array_key_count = region_sz / BARCO_CRYPTO_KEY_DESC_SZ;
    /* Sanity check that we have enough memory to support the keys scale needed */
    assert(key_array_key_count >= CRYPTO_KEY_COUNT_MAX);

    hese.dhs_crypto_ctl.sym2_key_array_base_w0.fld(key_array_base & 0xffffffff);
    hese.dhs_crypto_ctl.sym2_key_array_base_w0.write();
    hese.dhs_crypto_ctl.sym2_key_array_base_w1.fld(key_array_base >> 32);
    hese.dhs_crypto_ctl.sym2_key_array_base_w1.write();

    hese.dhs_crypto_ctl.sym2_key_array_size.fld(key_array_key_count);
    hese.dhs_crypto_ctl.sym2_key_array_size.write();

    /*
     * Enable the read/write cache for AXI memory accesses for Barco descriptors, data-read,
     * data-write and status/opaque-tag/doorbell writes.
     */
    hese.dhs_crypto_ctl.sym2_axi_desc.fld(0x3c0);
    hese.dhs_crypto_ctl.sym2_axi_desc.write();
    hese.dhs_crypto_ctl.sym2_axi_data_read.fld(0x3c0);
    hese.dhs_crypto_ctl.sym2_axi_data_read.write();
    hese.dhs_crypto_ctl.sym2_axi_data_write.fld(0x3c0);
    hese.dhs_crypto_ctl.sym2_axi_data_write.write();
    hese.dhs_crypto_ctl.sym2_axi_status.fld(0x3c0);
    hese.dhs_crypto_ctl.sym2_axi_status.write();

    SDK_TRACE_DEBUG("Barco xts0/sym2 Key Descriptor Array of count %u setup @ 0x0x%lx",
                    key_array_key_count, key_array_base);

    return ret;
}

sdk_ret_t
elba_barco_sym2_ring0_init (elba_barco_ring_t *barco_ring)
{
    elb_top_csr_t &cap0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    elb_hens_csr_t &hens = cap0.md.hens;
    sdk_ret_t ret = SDK_RET_OK;
    uint32_t sym2_ring0_pi, sym2_ring0_ci;

    ret = elba_barco_ring_common_init(barco_ring);
    if (ret != SDK_RET_OK) {
        return ret;
    }

    /* Reset SYM2 ring */
    hens.dhs_crypto_ctl.sym2_soft_rst.fld(0xffffffff);
    hens.dhs_crypto_ctl.sym2_soft_rst.write();
    /* Bring out of reset */
    hens.dhs_crypto_ctl.sym2_soft_rst.fld(0);
    hens.dhs_crypto_ctl.sym2_soft_rst.write();

    hens.dhs_crypto_ctl.sym2_ring0_base_w0.fld((uint32_t)(barco_ring->ring_base & 0xffffffff));
    hens.dhs_crypto_ctl.sym2_ring0_base_w0.write();
    hens.dhs_crypto_ctl.sym2_ring0_base_w1.fld((uint32_t)(barco_ring->ring_base >> 32));
    hens.dhs_crypto_ctl.sym2_ring0_base_w1.write();

    hens.dhs_crypto_ctl.sym2_ring0_size.fld(barco_ring->ring_size);
    hens.dhs_crypto_ctl.sym2_ring0_size.write();

    sym2_ring0_pi = hens.dhs_crypto_ctl.sym2_ring0_producer_idx.fld().convert_to<uint32_t>();
    sym2_ring0_ci = hens.dhs_crypto_ctl.sym2_ring0_consumer_idx.fld().convert_to<uint32_t>();

    assert((sym2_ring0_pi== 0) && (sym2_ring0_ci == 0));

    SDK_TRACE_DEBUG("Barco ring \"%s\" base setup @ 0x%lx, descriptor count %u",
                    barco_ring->ring_name, barco_ring->ring_base,
                    barco_ring->ring_size);

    return elba_barco_xts0_key_array_init();
}

bool
elba_barco_sym2_ring0_poller (elba_barco_ring_t *barco_ring, uint32_t req_tag)
{
    /* TBD */
    return FALSE;
}

sdk_ret_t
elba_barco_sym2_ring0_queue_request (struct elba_barco_ring_s *barco_ring,
                                     void *req, uint32_t *req_tag,
                                     bool schedule_barco)
{
    return SDK_RET_OK;
}

sdk_ret_t
elba_barco_xts1_key_array_init (void)
{
    elb_top_csr_t &cap0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    elb_hese_csr_t &hese = cap0.md.hese;
    sdk_ret_t ret = SDK_RET_OK;
    uint64_t key_array_base;
    uint32_t key_array_key_count;
    char key_desc_array[] = ELBA_BARCO_KEY_DESC;
    uint32_t region_sz = 0;

    // Currently sharing the same key descriptor array as GCM
    // Eventually all symmetric protocols will share one large key array
    key_array_base = get_mem_addr(key_desc_array);
    SDK_ASSERT(key_array_base != INVALID_MEM_ADDRESS);
    /* All regions in hbm_mem.json are in multiples of 1kb and hence should already be aligned to 16byte
     * but confirm
     */
    assert((key_array_base & (BARCO_CRYPTO_KEY_DESC_ALIGN_BYTES - 1)) == 0);
    region_sz = get_mem_size_kb(key_desc_array) * 1024;
    key_array_key_count = region_sz / BARCO_CRYPTO_KEY_DESC_SZ;
    /* Sanity check that we have enough memory to support the keys scale needed */
    assert(key_array_key_count >= CRYPTO_KEY_COUNT_MAX);

    hese.dhs_crypto_ctl.sym3_key_array_base_w0.fld(key_array_base & 0xffffffff);
    hese.dhs_crypto_ctl.sym3_key_array_base_w0.write();
    hese.dhs_crypto_ctl.sym3_key_array_base_w1.fld(key_array_base >> 32);
    hese.dhs_crypto_ctl.sym3_key_array_base_w1.write();

    hese.dhs_crypto_ctl.sym3_key_array_size.fld(key_array_key_count);
    hese.dhs_crypto_ctl.sym3_key_array_size.write();

    /*
     * Enable the read/write cache for AXI memory accesses for Barco descriptors, data-read,
     * data-write and status/opaque-tag/doorbell writes.
     */
    hese.dhs_crypto_ctl.sym3_axi_desc.fld(0x3c0);
    hese.dhs_crypto_ctl.sym3_axi_desc.write();
    hese.dhs_crypto_ctl.sym3_axi_data_read.fld(0x3c0);
    hese.dhs_crypto_ctl.sym3_axi_data_read.write();
    hese.dhs_crypto_ctl.sym3_axi_data_write.fld(0x3c0);
    hese.dhs_crypto_ctl.sym3_axi_data_write.write();
    hese.dhs_crypto_ctl.sym3_axi_status.fld(0x3c0);
    hese.dhs_crypto_ctl.sym3_axi_status.write();

    SDK_TRACE_DEBUG("Barco xts1/sym3 Key Descriptor Array of count %u setup @ 0x0x%lx",
            key_array_key_count, key_array_base);

    return ret;
}

sdk_ret_t
elba_barco_sym3_ring0_init (elba_barco_ring_t *barco_ring)
{
    elb_top_csr_t &cap0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    elb_hens_csr_t &hens = cap0.md.hens;
    sdk_ret_t ret = SDK_RET_OK;
    uint32_t  sym3_ring0_pi, sym3_ring0_ci;

    ret = elba_barco_ring_common_init(barco_ring);
    if (ret != SDK_RET_OK) {
        return ret;
    }

    /* Reset SYM3 ring */
    hens.dhs_crypto_ctl.sym3_soft_rst.fld(0xffffffff);
    hens.dhs_crypto_ctl.sym3_soft_rst.write();
    /* Bring out of reset */
    hens.dhs_crypto_ctl.sym3_soft_rst.fld(0);
    hens.dhs_crypto_ctl.sym3_soft_rst.write();

    hens.dhs_crypto_ctl.sym3_ring0_base_w0.fld((uint32_t)(barco_ring->ring_base & 0xffffffff));
    hens.dhs_crypto_ctl.sym3_ring0_base_w0.write();
    hens.dhs_crypto_ctl.sym3_ring0_base_w1.fld((uint32_t)(barco_ring->ring_base >> 32));
    hens.dhs_crypto_ctl.sym3_ring0_base_w1.write();

    hens.dhs_crypto_ctl.sym3_ring0_size.fld(barco_ring->ring_size);
    hens.dhs_crypto_ctl.sym3_ring0_size.write();

    sym3_ring0_pi = hens.dhs_crypto_ctl.sym3_ring0_producer_idx.fld().convert_to<uint32_t>();
    sym3_ring0_ci = hens.dhs_crypto_ctl.sym3_ring0_consumer_idx.fld().convert_to<uint32_t>();

    assert((sym3_ring0_pi== 0) && (sym3_ring0_ci == 0));

    SDK_TRACE_DEBUG("Barco ring \"%s\" base setup @ 0x%lx, descriptor count %u",
                    barco_ring->ring_name, barco_ring->ring_base,
                    barco_ring->ring_size);

    return elba_barco_xts1_key_array_init();
}

bool
elba_barco_sym3_ring0_poller (elba_barco_ring_t *barco_ring, uint32_t req_tag)
{
    /* TBD */
    return FALSE;
}

sdk_ret_t
elba_barco_sym3_ring0_queue_request (struct elba_barco_ring_s *barco_ring,
                                     void *req, uint32_t *req_tag,
                                     bool schedule_barco)
{
    return SDK_RET_OK;
}

sdk_ret_t
elba_barco_mpp0_key_array_init (void)
{
    elb_top_csr_t &cap0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    elb_mpse_csr_t &mpse = cap0.mp.mpse;
    sdk_ret_t ret = SDK_RET_OK;
    uint64_t  key_array_base;
    uint32_t  key_array_key_count;
    char      key_desc_array[] = ELBA_BARCO_KEY_DESC;
    uint32_t  region_sz = 0;

    // Currently sharing the same key descriptor array as GCM
    // Eventually all symmetric protocols will share one large key array
    key_array_base = get_mem_addr(key_desc_array);
    SDK_ASSERT(key_array_base != INVALID_MEM_ADDRESS);
    /* All regions in hbm_mem.json are in multiples of 1kb and hence should already be aligned to 16byte
     * but confirm
     */
    assert((key_array_base & (BARCO_CRYPTO_KEY_DESC_ALIGN_BYTES - 1)) == 0);
    region_sz = get_mem_size_kb(key_desc_array) * 1024;
    key_array_key_count = region_sz / BARCO_CRYPTO_KEY_DESC_SZ;

    mpse.dhs_crypto_ctl.mpp0_key_array_base_w0.fld(key_array_base & 0xffffffff);
    mpse.dhs_crypto_ctl.mpp0_key_array_base_w0.write();
    mpse.dhs_crypto_ctl.mpp0_key_array_base_w1.fld(key_array_base >> 32);
    mpse.dhs_crypto_ctl.mpp0_key_array_base_w1.write();

    mpse.dhs_crypto_ctl.mpp0_key_array_size.fld(key_array_key_count);
    mpse.dhs_crypto_ctl.mpp0_key_array_size.write();

    /*
     * Enable the read/write cache for AXI memory accesses for Barco descriptors, data-read,
     * data-write and status/opaque-tag/doorbell writes.
     */
    mpse.dhs_crypto_ctl.mpp0_axi_desc.fld(0x3c0);
    mpse.dhs_crypto_ctl.mpp0_axi_desc.write();
    mpse.dhs_crypto_ctl.mpp0_axi_data_read.fld(0x3c0);
    mpse.dhs_crypto_ctl.mpp0_axi_data_read.write();
    mpse.dhs_crypto_ctl.mpp0_axi_data_write.fld(0x3c0);
    mpse.dhs_crypto_ctl.mpp0_axi_data_write.write();
    mpse.dhs_crypto_ctl.mpp0_axi_status.fld(0x3c0);
    mpse.dhs_crypto_ctl.mpp0_axi_status.write();

    SDK_TRACE_DEBUG("Barco MPP0 Key Descriptor Array of count %u setup @ 0x%lx",
                    key_array_key_count, key_array_base);

    return ret;
}

sdk_ret_t
elba_barco_mpp0_init (elba_barco_ring_t *barco_ring)
{
    elb_top_csr_t &cap0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    elb_mpns_csr_t &mpns = cap0.mp.mpns;
    sdk_ret_t ret = SDK_RET_OK;
    uint32_t mpp0_pi, mpp0_ci;

    ret = elba_barco_ring_common_init(barco_ring);
    if (ret != SDK_RET_OK) {
        return ret;
    }

    /* Reset MPP0 ring */
    mpns.dhs_crypto_ctl.mpp0_soft_rst.fld(0xffffffff);
    mpns.dhs_crypto_ctl.mpp0_soft_rst.write();
    /* Bring out of reset */
    mpns.dhs_crypto_ctl.mpp0_soft_rst.fld(0);
    mpns.dhs_crypto_ctl.mpp0_soft_rst.write();

    mpns.dhs_crypto_ctl.mpp0_ring_base_w0.fld((uint32_t)(barco_ring->ring_base & 0xffffffff));
    mpns.dhs_crypto_ctl.mpp0_ring_base_w0.write();
    mpns.dhs_crypto_ctl.mpp0_ring_base_w1.fld((uint32_t)(barco_ring->ring_base >> 32));
    mpns.dhs_crypto_ctl.mpp0_ring_base_w1.write();

    mpns.dhs_crypto_ctl.mpp0_ring_size.fld(barco_ring->ring_size);
    mpns.dhs_crypto_ctl.mpp0_ring_size.write();

    mpns.dhs_crypto_ctl.mpp0_producer_idx.fld(barco_ring->producer_idx);
    mpns.dhs_crypto_ctl.mpp0_producer_idx.write();

    // CI is read-only
    //mpns.dhs_crypto_ctl.mpp0_consumer_idx.fld(barco_ring->consumer_idx);
    //mpns.dhs_crypto_ctl.mpp0_consumer_idx.write();

    SDK_TRACE_DEBUG("Barco ring \"%s\" base setup @ 0x%lx, descriptor count %u",
                    barco_ring->ring_name, barco_ring->ring_base,
                    barco_ring->ring_size);

    mpp0_pi = mpns.dhs_crypto_ctl.mpp0_producer_idx.fld().convert_to<uint32_t>();
    mpp0_ci = mpns.dhs_crypto_ctl.mpp0_consumer_idx.fld().convert_to<uint32_t>();

    assert((mpp0_pi== 0) && (mpp0_ci == 0));

    return elba_barco_mpp0_key_array_init();
}

bool
elba_barco_mpp0_poller(elba_barco_ring_t *barco_ring, uint32_t req_tag)
{
    bool                                ret = FALSE;
    uint32_t                            curr_opaque_tag = 0;

    if (sdk::asic::asic_mem_read(barco_ring->opaque_tag_addr,
                                 (uint8_t*)&curr_opaque_tag,
                                 sizeof(curr_opaque_tag))) {
        SDK_TRACE_ERR("Poll:%s: Failed to retrieve current opaque tag value @ 0x%lx",
                      barco_ring->ring_name,
                      (uint64_t) barco_ring->opaque_tag_addr);
        return FALSE;
    }
    else {
        SDK_TRACE_DEBUG("Poll:%s: Retrievd opaque tag value: %u",
                        barco_ring->ring_name, curr_opaque_tag);
        /* TODO: Handle wraparounds */
        if (curr_opaque_tag >= req_tag)
            ret = TRUE;
    }

    return ret;

#if 0
    elb_top_csr_t &                     cap0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    elb_mpns_csr_t &                    mpns = cap0.mp.mpns;

    /* TBD - use Doorbell address in req descriptor  to track request completions */
    if (mpns.dhs_crypto_ctl.mpp0_consumer_idx.fld() != barco_ring->consumer_idx) {
        /* New responses available */
        return TRUE;
    }

    return FALSE;
#endif
}

sdk_ret_t
elba_barco_mpp0_queue_request (struct elba_barco_ring_s *barco_ring, void *req,
                               uint32_t *req_tag, bool schedule_barco)
{
    elb_top_csr_t &cap0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    elb_mpns_csr_t &mpns = cap0.mp.mpns;
    uint64_t slot_addr = 0;
    sdk_ret_t ret = SDK_RET_OK;
    barco_symm_req_descriptor_t *sym_req_descr = NULL;

    /*
     * We'll use the opaque-tag address to track response from
     * barco.
     */
    sym_req_descr = (barco_symm_req_descriptor_t*) req;

#if 0
    sym_req_descr->opaque_tag_value = barco_ring->opaqe_tag_value;
    sym_req_descr->opaque_tag_wr_en = 1;
    sym_req_descr->opaque_tag_address = barco_ring->opaque_tag_addr;
#endif

    /*
     * Use doorbell-address in the symm request descriptor to
     * track response from barco.
     */
    sym_req_descr->doorbell_addr = barco_ring->opaque_tag_addr;
    sym_req_descr->doorbell_data = barco_ring->opaqe_tag_value;

    slot_addr = barco_ring->ring_base + (barco_ring->producer_idx *
                                         barco_ring->descriptor_size);

    if (sdk::asic::asic_mem_write(slot_addr, (uint8_t*)req,
                                  barco_ring->descriptor_size)) {
        SDK_TRACE_ERR("Failed to write MPP Req descriptor entry for %s  @ 0x%lx",
                      barco_ring->ring_name, (uint64_t) slot_addr);
        ret = SDK_RET_INVALID_ARG;
    }
    else {
        barco_ring->producer_idx = (barco_ring->producer_idx + 1) &
            (barco_ring->ring_size - 1);

        /* Barco doorbell */
        mpns.dhs_crypto_ctl.mpp0_producer_idx.fld(barco_ring->producer_idx);
        mpns.dhs_crypto_ctl.mpp0_producer_idx.write();

        *req_tag = barco_ring->opaqe_tag_value++;
    }

    return ret;
}

sdk_ret_t
elba_barco_mpp1_key_array_init (void)
{
    elb_top_csr_t &cap0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    elb_mpse_csr_t &mpse = cap0.mp.mpse;
    sdk_ret_t ret = SDK_RET_OK;
    uint64_t key_array_base;
    uint32_t key_array_key_count;
    char key_desc_array[] = ELBA_BARCO_KEY_DESC;
    uint32_t region_sz = 0;

    // Currently sharing the same key descriptor array as GCM
    // Eventually all symmetric protocols will share one large key array
    key_array_base = get_mem_addr(key_desc_array);
    SDK_ASSERT(key_array_base != INVALID_MEM_ADDRESS);
    /* All regions in hbm_mem.json are in multiples of 1kb and hence should already be aligned to 16byte
     * but confirm
     */
    assert((key_array_base & (BARCO_CRYPTO_KEY_DESC_ALIGN_BYTES - 1)) == 0);
    region_sz = get_mem_size_kb(key_desc_array) * 1024;
    key_array_key_count = region_sz / BARCO_CRYPTO_KEY_DESC_SZ;

    mpse.dhs_crypto_ctl.mpp1_key_array_base_w0.fld(key_array_base & 0xffffffff);
    mpse.dhs_crypto_ctl.mpp1_key_array_base_w0.write();
    mpse.dhs_crypto_ctl.mpp1_key_array_base_w1.fld(key_array_base >> 32);
    mpse.dhs_crypto_ctl.mpp1_key_array_base_w1.write();

    mpse.dhs_crypto_ctl.mpp1_key_array_size.fld(key_array_key_count);
    mpse.dhs_crypto_ctl.mpp1_key_array_size.write();

    /*
     * Enable the read/write cache for AXI memory accesses for Barco descriptors, data-read,
     * data-write and status/opaque-tag/doorbell writes.
     */
    mpse.dhs_crypto_ctl.mpp1_axi_desc.fld(0x3c0);
    mpse.dhs_crypto_ctl.mpp1_axi_desc.write();
    mpse.dhs_crypto_ctl.mpp1_axi_data_read.fld(0x3c0);
    mpse.dhs_crypto_ctl.mpp1_axi_data_read.write();
    mpse.dhs_crypto_ctl.mpp1_axi_data_write.fld(0x3c0);
    mpse.dhs_crypto_ctl.mpp1_axi_data_write.write();
    mpse.dhs_crypto_ctl.mpp1_axi_status.fld(0x3c0);
    mpse.dhs_crypto_ctl.mpp1_axi_status.write();

    SDK_TRACE_DEBUG("Barco MPP1 Key Descriptor Array of count %u setup @ 0x%lx",
                    key_array_key_count, key_array_base);

    return ret;
}

sdk_ret_t
elba_barco_mpp1_init(elba_barco_ring_t *barco_ring)
{
    elb_top_csr_t &cap0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    elb_mpns_csr_t &mpns = cap0.mp.mpns;
    sdk_ret_t ret = SDK_RET_OK;
    uint32_t mpp1_pi, mpp1_ci;

    ret = elba_barco_ring_common_init(barco_ring);
    if (ret != SDK_RET_OK) {
        return ret;
    }

    /* Reset MPP1 ring */
    mpns.dhs_crypto_ctl.mpp1_soft_rst.fld(0xffffffff);
    mpns.dhs_crypto_ctl.mpp1_soft_rst.write();
    /* Bring out of reset */
    mpns.dhs_crypto_ctl.mpp1_soft_rst.fld(0);
    mpns.dhs_crypto_ctl.mpp1_soft_rst.write();

    mpns.dhs_crypto_ctl.mpp1_ring_base_w0.fld((uint32_t)(barco_ring->ring_base & 0xffffffff));
    mpns.dhs_crypto_ctl.mpp1_ring_base_w0.write();
    mpns.dhs_crypto_ctl.mpp1_ring_base_w1.fld((uint32_t)(barco_ring->ring_base >> 32));
    mpns.dhs_crypto_ctl.mpp1_ring_base_w1.write();

    mpns.dhs_crypto_ctl.mpp1_ring_size.fld(barco_ring->ring_size);
    mpns.dhs_crypto_ctl.mpp1_ring_size.write();

    mpns.dhs_crypto_ctl.mpp1_producer_idx.fld(barco_ring->producer_idx);
    mpns.dhs_crypto_ctl.mpp1_producer_idx.write();

    // CI is read-only
    //mpns.dhs_crypto_ctl.mpp1_consumer_idx.fld(barco_ring->consumer_idx);
    //mpns.dhs_crypto_ctl.mpp1_consumer_idx.write();

    SDK_TRACE_DEBUG("Barco ring \"%s\" base setup @ 0x%lx, descriptor count %u",
                    barco_ring->ring_name, barco_ring->ring_base,
                    barco_ring->ring_size);

    mpp1_pi = mpns.dhs_crypto_ctl.mpp1_producer_idx.fld().convert_to<uint32_t>();
    mpp1_ci = mpns.dhs_crypto_ctl.mpp1_consumer_idx.fld().convert_to<uint32_t>();

    assert((mpp1_pi== 0) && (mpp1_ci == 0));

    return elba_barco_mpp1_key_array_init();
}

sdk_ret_t
elba_barco_mpp2_key_array_init (void)
{
    elb_top_csr_t &cap0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    elb_mpse_csr_t &mpse = cap0.mp.mpse;
    sdk_ret_t ret = SDK_RET_OK;
    uint64_t key_array_base;
    uint32_t key_array_key_count;
    char key_desc_array[] = ELBA_BARCO_KEY_DESC;
    uint32_t region_sz = 0;

    // Currently sharing the same key descriptor array as GCM
    // Eventually all symmetric protocols will share one large key array
    key_array_base = get_mem_addr(key_desc_array);
    SDK_ASSERT(key_array_base != INVALID_MEM_ADDRESS);
    /* All regions in hbm_mem.json are in multiples of 1kb and hence should already be aligned to 16byte
     * but confirm
     */
    assert((key_array_base & (BARCO_CRYPTO_KEY_DESC_ALIGN_BYTES - 1)) == 0);
    region_sz = get_mem_size_kb(key_desc_array) * 1024;
    key_array_key_count = region_sz / BARCO_CRYPTO_KEY_DESC_SZ;

    mpse.dhs_crypto_ctl.mpp2_key_array_base_w0.fld(key_array_base & 0xffffffff);
    mpse.dhs_crypto_ctl.mpp2_key_array_base_w0.write();
    mpse.dhs_crypto_ctl.mpp2_key_array_base_w1.fld(key_array_base >> 32);
    mpse.dhs_crypto_ctl.mpp2_key_array_base_w1.write();

    mpse.dhs_crypto_ctl.mpp2_key_array_size.fld(key_array_key_count);
    mpse.dhs_crypto_ctl.mpp2_key_array_size.write();

    /*
     * Enable the read/write cache for AXI memory accesses for Barco descriptors, data-read,
     * data-write and status/opaque-tag/doorbell writes.
     */
    mpse.dhs_crypto_ctl.mpp2_axi_desc.fld(0x3c0);
    mpse.dhs_crypto_ctl.mpp2_axi_desc.write();
    mpse.dhs_crypto_ctl.mpp2_axi_data_read.fld(0x3c0);
    mpse.dhs_crypto_ctl.mpp2_axi_data_read.write();
    mpse.dhs_crypto_ctl.mpp2_axi_data_write.fld(0x3c0);
    mpse.dhs_crypto_ctl.mpp2_axi_data_write.write();
    mpse.dhs_crypto_ctl.mpp2_axi_status.fld(0x3c0);
    mpse.dhs_crypto_ctl.mpp2_axi_status.write();

    SDK_TRACE_DEBUG("Barco MPP2 Key Descriptor Array of count %u setup @ 0x%lx",
                    key_array_key_count, key_array_base);

    return ret;
}

sdk_ret_t
elba_barco_mpp2_init (elba_barco_ring_t *barco_ring)
{
    elb_top_csr_t &cap0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    elb_mpns_csr_t &mpns = cap0.mp.mpns;
    sdk_ret_t ret = SDK_RET_OK;
    uint32_t mpp2_pi, mpp2_ci;

    ret = elba_barco_ring_common_init(barco_ring);
    if (ret != SDK_RET_OK) {
        return ret;
    }

    /* Reset MPP2 ring */
    mpns.dhs_crypto_ctl.mpp2_soft_rst.fld(0xffffffff);
    mpns.dhs_crypto_ctl.mpp2_soft_rst.write();
    /* Bring out of reset */
    mpns.dhs_crypto_ctl.mpp2_soft_rst.fld(0);
    mpns.dhs_crypto_ctl.mpp2_soft_rst.write();

    mpns.dhs_crypto_ctl.mpp2_ring_base_w0.fld((uint32_t)(barco_ring->ring_base & 0xffffffff));
    mpns.dhs_crypto_ctl.mpp2_ring_base_w0.write();
    mpns.dhs_crypto_ctl.mpp2_ring_base_w1.fld((uint32_t)(barco_ring->ring_base >> 32));
    mpns.dhs_crypto_ctl.mpp2_ring_base_w1.write();

    mpns.dhs_crypto_ctl.mpp2_ring_size.fld(barco_ring->ring_size);
    mpns.dhs_crypto_ctl.mpp2_ring_size.write();

    mpns.dhs_crypto_ctl.mpp2_producer_idx.fld(barco_ring->producer_idx);
    mpns.dhs_crypto_ctl.mpp2_producer_idx.write();

    // CI is read-only
    //mpns.dhs_crypto_ctl.mpp2_consumer_idx.fld(barco_ring->consumer_idx);
    //mpns.dhs_crypto_ctl.mpp2_consumer_idx.write();

    SDK_TRACE_DEBUG("Barco ring \"%s\" base setup @ 0x%lx, descriptor count %u",
                    barco_ring->ring_name, barco_ring->ring_base,
                    barco_ring->ring_size);

    mpp2_pi = mpns.dhs_crypto_ctl.mpp2_producer_idx.fld().convert_to<uint32_t>();
    mpp2_ci = mpns.dhs_crypto_ctl.mpp2_consumer_idx.fld().convert_to<uint32_t>();

    assert((mpp2_pi== 0) && (mpp2_ci == 0));

    return elba_barco_mpp2_key_array_init();
}

sdk_ret_t
elba_barco_mpp3_key_array_init (void)
{
    elb_top_csr_t &cap0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    elb_mpse_csr_t &mpse = cap0.mp.mpse;
    sdk_ret_t ret = SDK_RET_OK;
    uint64_t key_array_base;
    uint32_t key_array_key_count;
    char key_desc_array[] = ELBA_BARCO_KEY_DESC;
    uint32_t region_sz = 0;

    // Currently sharing the same key descriptor array as GCM
    // Eventually all symmetric protocols will share one large key array
    key_array_base = get_mem_addr(key_desc_array);
    SDK_ASSERT(key_array_base != INVALID_MEM_ADDRESS);
    /* All regions in hbm_mem.json are in multiples of 1kb and hence should already be aligned to 16byte
     * but confirm
     */
    assert((key_array_base & (BARCO_CRYPTO_KEY_DESC_ALIGN_BYTES - 1)) == 0);
    region_sz = get_mem_size_kb(key_desc_array) * 1024;
    key_array_key_count = region_sz / BARCO_CRYPTO_KEY_DESC_SZ;

    mpse.dhs_crypto_ctl.mpp3_key_array_base_w0.fld(key_array_base & 0xffffffff);
    mpse.dhs_crypto_ctl.mpp3_key_array_base_w0.write();
    mpse.dhs_crypto_ctl.mpp3_key_array_base_w1.fld(key_array_base >> 32);
    mpse.dhs_crypto_ctl.mpp3_key_array_base_w1.write();

    mpse.dhs_crypto_ctl.mpp3_key_array_size.fld(key_array_key_count);
    mpse.dhs_crypto_ctl.mpp3_key_array_size.write();

    /*
     * Enable the read/write cache for AXI memory accesses for Barco descriptors, data-read,
     * data-write and status/opaque-tag/doorbell writes.
     */
    mpse.dhs_crypto_ctl.mpp3_axi_desc.fld(0x3c0);
    mpse.dhs_crypto_ctl.mpp3_axi_desc.write();
    mpse.dhs_crypto_ctl.mpp3_axi_data_read.fld(0x3c0);
    mpse.dhs_crypto_ctl.mpp3_axi_data_read.write();
    mpse.dhs_crypto_ctl.mpp3_axi_data_write.fld(0x3c0);
    mpse.dhs_crypto_ctl.mpp3_axi_data_write.write();
    mpse.dhs_crypto_ctl.mpp3_axi_status.fld(0x3c0);
    mpse.dhs_crypto_ctl.mpp3_axi_status.write();

    SDK_TRACE_DEBUG("Barco MPP3 Key Descriptor Array of count %u setup @ 0x%lx",
                    key_array_key_count, key_array_base);

    return ret;
}

sdk_ret_t
elba_barco_mpp3_init (elba_barco_ring_t *barco_ring)
{
    elb_top_csr_t &cap0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    elb_mpns_csr_t &mpns = cap0.mp.mpns;
    sdk_ret_t ret = SDK_RET_OK;
    uint32_t mpp3_pi, mpp3_ci;

    ret = elba_barco_ring_common_init(barco_ring);
    if (ret != SDK_RET_OK) {
        return ret;
    }

    /* Reset MPP3 ring */
    mpns.dhs_crypto_ctl.mpp3_soft_rst.fld(0xffffffff);
    mpns.dhs_crypto_ctl.mpp3_soft_rst.write();
    /* Bring out of reset */
    mpns.dhs_crypto_ctl.mpp3_soft_rst.fld(0);
    mpns.dhs_crypto_ctl.mpp3_soft_rst.write();

    mpns.dhs_crypto_ctl.mpp3_ring_base_w0.fld((uint32_t)(barco_ring->ring_base & 0xffffffff));
    mpns.dhs_crypto_ctl.mpp3_ring_base_w0.write();
    mpns.dhs_crypto_ctl.mpp3_ring_base_w1.fld((uint32_t)(barco_ring->ring_base >> 32));
    mpns.dhs_crypto_ctl.mpp3_ring_base_w1.write();

    mpns.dhs_crypto_ctl.mpp3_ring_size.fld(barco_ring->ring_size);
    mpns.dhs_crypto_ctl.mpp3_ring_size.write();

    mpns.dhs_crypto_ctl.mpp3_producer_idx.fld(barco_ring->producer_idx);
    mpns.dhs_crypto_ctl.mpp3_producer_idx.write();

    // CI is read-only
    //mpns.dhs_crypto_ctl.mpp3_consumer_idx.fld(barco_ring->consumer_idx);
    //mpns.dhs_crypto_ctl.mpp3_consumer_idx.write();

    SDK_TRACE_DEBUG("Barco ring \"%s\" base setup @ 0x%lx, descriptor count %u",
                    barco_ring->ring_name, barco_ring->ring_base,
                    barco_ring->ring_size);

    mpp3_pi = mpns.dhs_crypto_ctl.mpp3_producer_idx.fld().convert_to<uint32_t>();
    mpp3_ci = mpns.dhs_crypto_ctl.mpp3_consumer_idx.fld().convert_to<uint32_t>();

    assert((mpp3_pi== 0) && (mpp3_ci == 0));

    return elba_barco_mpp3_key_array_init();
}

sdk_ret_t
elba_barco_sym0_key_array_init (void)
{
    elb_top_csr_t &cap0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    elb_hese_csr_t &hese = cap0.md.hese;
    sdk_ret_t ret = SDK_RET_OK;
    uint64_t key_array_base;
    uint32_t key_array_key_count;
    char key_desc_array[] = ELBA_BARCO_KEY_DESC;
    uint32_t region_sz = 0;

    // Currently sharing the same key descriptor array as GCM
    // Eventually all symmetric protocols will share one large key array
    key_array_base = get_mem_addr(key_desc_array);
    SDK_ASSERT(key_array_base != INVALID_MEM_ADDRESS);
    /* All regions in hbm_mem.json are in multiples of 1kb and hence should already be aligned to 16byte
     * but confirm
     */
    assert((key_array_base & (BARCO_CRYPTO_KEY_DESC_ALIGN_BYTES - 1)) == 0);
    region_sz = get_mem_size_kb(key_desc_array) * 1024;
    key_array_key_count = region_sz / BARCO_CRYPTO_KEY_DESC_SZ;
    /* Sanity check that we have enough memory to support the keys scale needed */
    assert(key_array_key_count >= CRYPTO_KEY_COUNT_MAX);

    hese.dhs_crypto_ctl.sym0_key_array_base_w0.fld(key_array_base & 0xffffffff);
    hese.dhs_crypto_ctl.sym0_key_array_base_w0.write();
    hese.dhs_crypto_ctl.sym0_key_array_base_w1.fld(key_array_base >> 32);
    hese.dhs_crypto_ctl.sym0_key_array_base_w1.write();

    hese.dhs_crypto_ctl.sym0_key_array_size.fld(key_array_key_count);
    hese.dhs_crypto_ctl.sym0_key_array_size.write();

    /*
     * Enable the read/write cache for AXI memory accesses for Barco descriptors, data-read,
     * data-write and status/opaque-tag/doorbell writes.
     */
    hese.dhs_crypto_ctl.sym0_axi_desc.fld(0x3c0);
    hese.dhs_crypto_ctl.sym0_axi_desc.write();
    hese.dhs_crypto_ctl.sym0_axi_data_read.fld(0x3c0);
    hese.dhs_crypto_ctl.sym0_axi_data_read.write();
    hese.dhs_crypto_ctl.sym0_axi_data_write.fld(0x3c0);
    hese.dhs_crypto_ctl.sym0_axi_data_write.write();
    hese.dhs_crypto_ctl.sym0_axi_status.fld(0x3c0);
    hese.dhs_crypto_ctl.sym0_axi_status.write();

    SDK_TRACE_DEBUG("Barco sym0 Key Descriptor Array of count %u setup @ 0x0x%lx",
                    key_array_key_count, key_array_base);

    return ret;
}

sdk_ret_t
elba_barco_sym0_ring0_init (elba_barco_ring_t *barco_ring)
{

    elb_top_csr_t &cap0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    elb_hens_csr_t &hens = cap0.md.hens;
    sdk_ret_t ret = SDK_RET_OK;
    uint32_t sym0_ring0_pi, sym0_ring0_ci;


    ret = elba_barco_ring_common_init(barco_ring);
    if (ret != SDK_RET_OK) {
        return ret;
    }

    /* Reset SYM0 ring */
    hens.dhs_crypto_ctl.sym0_soft_rst.fld(0xffffffff);
    hens.dhs_crypto_ctl.sym0_soft_rst.write();
    /* Bring out of reset */
    hens.dhs_crypto_ctl.sym0_soft_rst.fld(0);
    hens.dhs_crypto_ctl.sym0_soft_rst.write();

    hens.dhs_crypto_ctl.sym0_ring0_base_w0.fld((uint32_t)(barco_ring->ring_base & 0xffffffff));
    hens.dhs_crypto_ctl.sym0_ring0_base_w0.write();
    hens.dhs_crypto_ctl.sym0_ring0_base_w1.fld((uint32_t)(barco_ring->ring_base >> 32));
    hens.dhs_crypto_ctl.sym0_ring0_base_w1.write();

    hens.dhs_crypto_ctl.sym0_ring0_size.fld(barco_ring->ring_size);
    hens.dhs_crypto_ctl.sym0_ring0_size.write();

    sym0_ring0_pi =
        hens.dhs_crypto_ctl.sym0_ring0_producer_idx.fld().convert_to<uint32_t>();
    sym0_ring0_ci =
        hens.dhs_crypto_ctl.sym0_ring0_consumer_idx.fld().convert_to<uint32_t>();

    assert((sym0_ring0_pi== 0) && (sym0_ring0_ci == 0));

    return elba_barco_sym0_key_array_init();
}

bool
elba_barco_sym0_ring0_poller (elba_barco_ring_t *barco_ring, uint32_t req_tag)
{
    bool                                ret = FALSE;
    uint32_t                            curr_opaque_tag = 0;

    if (sdk::asic::asic_mem_read(barco_ring->opaque_tag_addr,
                                 (uint8_t*)&curr_opaque_tag,
                                 sizeof(curr_opaque_tag))) {
        SDK_TRACE_ERR("Poll:%s: Failed to retrieve current opaque tag value @ 0x%lx",
                barco_ring->ring_name, (uint64_t) barco_ring->opaque_tag_addr);
        return FALSE;
    }
    else {
        SDK_TRACE_DEBUG("Poll:%s: Retrievd opaque tag value: %d",
                        barco_ring->ring_name, curr_opaque_tag);
        /* TODO: Handle wraparounds */
        if (curr_opaque_tag >= req_tag)
            ret = TRUE;
    }

    return ret;

#if 0
    elb_top_csr_t &                     cap0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    elb_hens_csr_t &                    hens = cap0.md.hens;

    /* TBD - use Doorbell address in req descriptor  to track request completions */
    if (hens.dhs_crypto_ctl.gcm0_consumer_idx.fld() != barco_ring->consumer_idx) {
        /* New responses available */
        return TRUE;
    }

    return FALSE;
#endif
}

sdk_ret_t
elba_barco_sym0_ring0_queue_request (struct elba_barco_ring_s *barco_ring,
                                     void *req,
                                     uint32_t *req_tag, bool schedule_barco)
{
    elb_top_csr_t &cap0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    elb_hens_csr_t &hens = cap0.md.hens;
    uint64_t slot_addr = 0;
    sdk_ret_t ret = SDK_RET_OK;
    barco_symm_req_descriptor_t *sym_req_descr = NULL;

    /*
     * We'll use the opaque-tag address to track response from
     * barco.
     */
    sym_req_descr = (barco_symm_req_descriptor_t*) req;

    sym_req_descr->opaque_tag_value = barco_ring->opaqe_tag_value;
    sym_req_descr->opaque_tag_wr_en = 1;

#if 1
    /*
     * Use doorbell-address in the symm request descriptor to
     * track response from barco.
     */
    sym_req_descr->doorbell_addr = barco_ring->opaque_tag_addr;
    sym_req_descr->doorbell_data = barco_ring->opaqe_tag_value;
#endif

    slot_addr = barco_ring->ring_base + (barco_ring->producer_idx *
                                         barco_ring->descriptor_size);

    if (sdk::asic::asic_mem_write(slot_addr, (uint8_t*)req,
                                  barco_ring->descriptor_size)) {
        SDK_TRACE_ERR("Failed to write SYM0-RING0 Req descriptor entry for %s  @ 0x%lx",
                      barco_ring->ring_name, (uint64_t) slot_addr);
        ret = SDK_RET_INVALID_ARG;
    }

    barco_ring->producer_idx = (barco_ring->producer_idx + 1) &
        (barco_ring->ring_size - 1);

    if (schedule_barco) {

        /* Barco doorbell */
        hens.dhs_crypto_ctl.sym0_ring0_producer_idx.fld(barco_ring->producer_idx);
        hens.dhs_crypto_ctl.sym0_ring0_producer_idx.write();
    }
    *req_tag = barco_ring->opaqe_tag_value++;

    return ret;
}

sdk_ret_t
elba_barco_sym1_key_array_init (void)
{
    elb_top_csr_t &cap0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    elb_hese_csr_t &hese = cap0.md.hese;
    sdk_ret_t ret = SDK_RET_OK;
    uint64_t key_array_base;
    uint32_t key_array_key_count;
    char key_desc_array[] = ELBA_BARCO_KEY_DESC;
    uint32_t region_sz = 0;

    // Currently sharing the same key descriptor array as GCM
    // Eventually all symmetric protocols will share one large key array
    key_array_base = get_mem_addr(key_desc_array);
    SDK_ASSERT(key_array_base != INVALID_MEM_ADDRESS);
    /* All regions in hbm_mem.json are in multiples of 1kb and hence should already be aligned to 16byte
     * but confirm
     */
    assert((key_array_base & (BARCO_CRYPTO_KEY_DESC_ALIGN_BYTES - 1)) == 0);
    region_sz = get_mem_size_kb(key_desc_array) * 1024;
    key_array_key_count = region_sz / BARCO_CRYPTO_KEY_DESC_SZ;
    /* Sanity check that we have enough memory to support the keys scale needed */
    assert(key_array_key_count >= CRYPTO_KEY_COUNT_MAX);

#if 0
    hese.dhs_crypto_ctl.gcm1_key_array_base_w0.fld(key_array_base & 0xffffffff);
    hese.dhs_crypto_ctl.gcm1_key_array_base_w0.write();
    hese.dhs_crypto_ctl.gcm1_key_array_base_w1.fld(key_array_base >> 32);
    hese.dhs_crypto_ctl.gcm1_key_array_base_w1.write();

    hese.dhs_crypto_ctl.gcm1_key_array_size.fld(key_array_key_count);
    hese.dhs_crypto_ctl.gcm1_key_array_size.write();

    /*
     * Enable the read/write cache for AXI memory accesses for Barco descriptors, data-read,
     * data-write and status/opaque-tag/doorbell writes.
     */
    hese.dhs_crypto_ctl.gcm1_axi_desc.fld(0x3c0);
    hese.dhs_crypto_ctl.gcm1_axi_desc.write();
    hese.dhs_crypto_ctl.gcm1_axi_data_read.fld(0x3c0);
    hese.dhs_crypto_ctl.gcm1_axi_data_read.write();
    hese.dhs_crypto_ctl.gcm1_axi_data_write.fld(0x3c0);
    hese.dhs_crypto_ctl.gcm1_axi_data_write.write();
    hese.dhs_crypto_ctl.gcm1_axi_status.fld(0x3c0);
    hese.dhs_crypto_ctl.gcm1_axi_status.write();
#else
    hese.dhs_crypto_ctl.sym1_key_array_base_w0.fld(key_array_base & 0xffffffff);
    hese.dhs_crypto_ctl.sym1_key_array_base_w0.write();
    hese.dhs_crypto_ctl.sym1_key_array_base_w1.fld(key_array_base >> 32);
    hese.dhs_crypto_ctl.sym1_key_array_base_w1.write();

    hese.dhs_crypto_ctl.sym1_key_array_size.fld(key_array_key_count);
    hese.dhs_crypto_ctl.sym1_key_array_size.write();

    /*
     * Enable the read/write cache for AXI memory accesses for Barco descriptors, data-read,
     * data-write and status/opaque-tag/doorbell writes.
     */
    hese.dhs_crypto_ctl.sym1_axi_desc.fld(0x3c0);
    hese.dhs_crypto_ctl.sym1_axi_desc.write();
    hese.dhs_crypto_ctl.sym1_axi_data_read.fld(0x3c0);
    hese.dhs_crypto_ctl.sym1_axi_data_read.write();
    hese.dhs_crypto_ctl.sym1_axi_data_write.fld(0x3c0);
    hese.dhs_crypto_ctl.sym1_axi_data_write.write();
    hese.dhs_crypto_ctl.sym1_axi_status.fld(0x3c0);
    hese.dhs_crypto_ctl.sym1_axi_status.write();
#endif

    SDK_TRACE_DEBUG("Barco gcm1 Key Descriptor Array of count %u setup @ 0x%lx",
                    key_array_key_count, key_array_base);

    return ret;
}

sdk_ret_t
elba_barco_sym1_ring0_init (elba_barco_ring_t *barco_ring)
{
    elb_top_csr_t &cap0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    elb_hens_csr_t &hens = cap0.md.hens;
    sdk_ret_t ret = SDK_RET_OK;
    uint32_t sym1_ring0_pi, sym1_ring0_ci;

    ret = elba_barco_ring_common_init(barco_ring);
    if (ret != SDK_RET_OK) {
        return ret;
    }

#if 0
    /* Reset gcm1 ring */
    hens.dhs_crypto_ctl.gcm1_soft_rst.fld(0xffffffff);
    hens.dhs_crypto_ctl.gcm1_soft_rst.write();
    /* Bring out of reset */
    hens.dhs_crypto_ctl.gcm1_soft_rst.fld(0);
    hens.dhs_crypto_ctl.gcm1_soft_rst.write();

    hens.dhs_crypto_ctl.gcm1_ring_base_w0.fld((uint32_t)(barco_ring->ring_base & 0xffffffff));
    hens.dhs_crypto_ctl.gcm1_ring_base_w0.write();
    hens.dhs_crypto_ctl.gcm1_ring_base_w1.fld((uint32_t)(barco_ring->ring_base >> 32));
    hens.dhs_crypto_ctl.gcm1_ring_base_w1.write();

    hens.dhs_crypto_ctl.gcm1_ring_size.fld(barco_ring->ring_size);
    hens.dhs_crypto_ctl.gcm1_ring_size.write();

    hens.dhs_crypto_ctl.gcm1_opa_tag_addr_w0.fld((uint32_t)(barco_ring->opaque_tag_addr & 0xffffffff));
    hens.dhs_crypto_ctl.gcm1_opa_tag_addr_w0.write();
    hens.dhs_crypto_ctl.gcm1_opa_tag_addr_w1.fld((uint32_t)(barco_ring->opaque_tag_addr >> 32));
    hens.dhs_crypto_ctl.gcm1_opa_tag_addr_w1.write();

    gcm1_pi = hens.dhs_crypto_ctl.gcm1_producer_idx.fld().convert_to<uint32_t>();
    gcm1_ci = hens.dhs_crypto_ctl.gcm1_consumer_idx.fld().convert_to<uint32_t>();

    assert((gcm1_pi == 0) && (gcm1_ci == 0));
#else
    /* Reset SYM0 ring */
    hens.dhs_crypto_ctl.sym1_soft_rst.fld(0xffffffff);
    hens.dhs_crypto_ctl.sym1_soft_rst.write();
    /* Bring out of reset */
    hens.dhs_crypto_ctl.sym1_soft_rst.fld(0);
    hens.dhs_crypto_ctl.sym1_soft_rst.write();

    hens.dhs_crypto_ctl.sym1_ring0_base_w0.fld((uint32_t)(barco_ring->ring_base & 0xffffffff));
    hens.dhs_crypto_ctl.sym1_ring0_base_w0.write();
    hens.dhs_crypto_ctl.sym1_ring0_base_w1.fld((uint32_t)(barco_ring->ring_base >> 32));
    hens.dhs_crypto_ctl.sym1_ring0_base_w1.write();

    hens.dhs_crypto_ctl.sym1_ring0_size.fld(barco_ring->ring_size);
    hens.dhs_crypto_ctl.sym1_ring0_size.write();

    sym1_ring0_pi = hens.dhs_crypto_ctl.sym1_ring0_producer_idx.fld().convert_to<uint32_t>();
    sym1_ring0_ci = hens.dhs_crypto_ctl.sym1_ring0_consumer_idx.fld().convert_to<uint32_t>();

    assert((sym1_ring0_pi== 0) && (sym1_ring0_ci == 0));
#endif

    return elba_barco_sym1_key_array_init();
}

sdk_ret_t
elba_barco_cp_init (elba_barco_ring_t *barco_ring)
{
    elb_top_csr_t &cap0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    elb_hens_csr_t &hens = cap0.md.hens;
    sdk_ret_t ret = SDK_RET_OK;
    uint32_t reg_lo;
    uint32_t reg_hi;

    ret = elba_barco_ring_common_init(barco_ring);
    if (ret != SDK_RET_OK) {
        return ret;
    }

    /*
     * Soft reset entire compression block;
     * also set the header version
     */
    hens.dhs_crypto_ctl.cp_cfg_glb.read();
    reg_lo = (hens.dhs_crypto_ctl.cp_cfg_glb.fld().convert_to<uint32_t>() &
              ~BARCO_CRYPTO_CP_CFG_GLB_HDR_VER_MASK)    |
             BARCO_CRYPTO_CP_CFG_GLB_SOFT_RESET         |
             BARCO_CRYPTO_CP_CFG_GLB_HDR_VER;
    hens.dhs_crypto_ctl.cp_cfg_glb.fld(reg_lo);
    hens.dhs_crypto_ctl.cp_cfg_glb.write();

    /*
     * Bring out of reset
     */
    hens.dhs_crypto_ctl.cp_cfg_glb.fld(reg_lo &
                                       ~BARCO_CRYPTO_CP_CFG_GLB_SOFT_RESET);
    hens.dhs_crypto_ctl.cp_cfg_glb.write();

    hens.dhs_crypto_ctl.cp_cfg_q1_base_adr_w0.fld((uint32_t)(barco_ring->ring_base & 0xffffffff));
    hens.dhs_crypto_ctl.cp_cfg_q1_base_adr_w0.write();
    hens.dhs_crypto_ctl.cp_cfg_q1_base_adr_w1.fld((uint32_t)(barco_ring->ring_base >> 32));
    hens.dhs_crypto_ctl.cp_cfg_q1_base_adr_w1.write();

    /*
     * Enable all 16 cp engines
     */
    hens.dhs_crypto_ctl.cp_cfg_ueng_w0.read();
    reg_lo = hens.dhs_crypto_ctl.cp_cfg_ueng_w0.fld().convert_to<uint32_t>()    |
             BARCO_CRYPTO_CP_UENG_LO_EN_ALL;
    hens.dhs_crypto_ctl.cp_cfg_ueng_w1.read();
    reg_hi = (hens.dhs_crypto_ctl.cp_cfg_ueng_w1.fld().convert_to<uint32_t>()   &
              ~BARCO_CRYPTO_CP_UENG_HI_HMEM_FILL_ZERO) |
             (BARCO_CRYPTO_CP_UENG_HI_SHA_DATA_UNCOMP  | BARCO_CRYPTO_CP_UENG_HI_CSUM_ON_UNCOMP |
              BARCO_CRYPTO_CP_UENG_HI_INTEG_APP_STATUS);
    hens.dhs_crypto_ctl.cp_cfg_ueng_w0.fld(reg_lo);
    hens.dhs_crypto_ctl.cp_cfg_ueng_w0.write();
    hens.dhs_crypto_ctl.cp_cfg_ueng_w1.fld(reg_hi);
    hens.dhs_crypto_ctl.cp_cfg_ueng_w1.write();

    /*
     * Enable warm queue
     */
    hens.dhs_crypto_ctl.cp_cfg_dist.read();
    reg_lo = (hens.dhs_crypto_ctl.cp_cfg_dist.fld().convert_to<uint32_t>()  &
              ~BARCO_CRYPTO_CP_DIST_DESC_Q_SIZE(BARCO_CRYPTO_CP_DIST_DESC_Q_SIZE_MASK)) |
             BARCO_CRYPTO_CP_DIST_DESC_Q_EN                                             |
             BARCO_CRYPTO_CP_DIST_DESC_Q_SIZE(BARCO_CRYPTO_CP_RING_SIZE);
    hens.dhs_crypto_ctl.cp_cfg_dist.fld(reg_lo);
    hens.dhs_crypto_ctl.cp_cfg_dist.write();

    hens.dhs_crypto_ctl.cp_cfg_host_opaque_tag_adr_w0.fld((uint32_t)(barco_ring->opaque_tag_addr & 0xffffffff));
    hens.dhs_crypto_ctl.cp_cfg_host_opaque_tag_adr_w0.write();
    hens.dhs_crypto_ctl.cp_cfg_host_opaque_tag_adr_w1.fld((uint32_t)(barco_ring->opaque_tag_addr >> 32));
    hens.dhs_crypto_ctl.cp_cfg_host_opaque_tag_adr_w1.write();
    hens.dhs_crypto_ctl.cp_cfg_host_opaque_tag_data.fld(barco_ring->opaqe_tag_value);
    hens.dhs_crypto_ctl.cp_cfg_host_opaque_tag_data.write();

    hens.dhs_crypto_ctl.cp_cfg_q1_pd_idx.fld(barco_ring->producer_idx);
    hens.dhs_crypto_ctl.cp_cfg_q1_pd_idx.write();

    SDK_TRACE_DEBUG("Barco compression ring \"%s\" base setup @ 0x%lx, descriptor count %u",
                    barco_ring->ring_name, barco_ring->ring_base,
                    barco_ring->ring_size);

    return SDK_RET_OK;
}

bool
elba_barco_cp_poller (elba_barco_ring_t *barco_ring, uint32_t req_tag)
{
    /* TBD */
    return FALSE;
}

sdk_ret_t
elba_barco_cp_hot_init (elba_barco_ring_t *barco_ring)
{
    elb_top_csr_t &cap0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    elb_hens_csr_t &hens = cap0.md.hens;
    sdk_ret_t ret = SDK_RET_OK;
    uint32_t reg_lo;

    ret = elba_barco_ring_common_init(barco_ring);
    if (ret != SDK_RET_OK) {
        return ret;
    }

    hens.dhs_crypto_ctl.cp_cfg_q0_base_adr_w0.fld((uint32_t)(barco_ring->ring_base & 0xffffffff));
    hens.dhs_crypto_ctl.cp_cfg_q0_base_adr_w0.write();
    hens.dhs_crypto_ctl.cp_cfg_q0_base_adr_w1.fld((uint32_t)(barco_ring->ring_base >> 32));
    hens.dhs_crypto_ctl.cp_cfg_q0_base_adr_w1.write();

    /*
     * Enable hot queue
     */
    hens.dhs_crypto_ctl.cp_cfg_dist.read();
    reg_lo = (hens.dhs_crypto_ctl.cp_cfg_dist.fld().convert_to<uint32_t>()  &
              ~BARCO_CRYPTO_CP_DIST_DESC_HOTQ_SIZE(BARCO_CRYPTO_CP_DIST_DESC_HOTQ_SIZE_MASK))   |
             BARCO_CRYPTO_CP_DIST_DESC_HOTQ_EN                                                  |
             BARCO_CRYPTO_CP_DIST_DESC_HOTQ_SIZE(BARCO_CRYPTO_CP_HOT_RING_SIZE);
    hens.dhs_crypto_ctl.cp_cfg_dist.fld(reg_lo);
    hens.dhs_crypto_ctl.cp_cfg_dist.write();

    hens.dhs_crypto_ctl.cp_cfg_q0_pd_idx.fld(barco_ring->producer_idx);
    hens.dhs_crypto_ctl.cp_cfg_q0_pd_idx.write();

    SDK_TRACE_DEBUG("Barco compression hot ring \"%s\" base setup @ 0x%lx, descriptor count %u",
                    barco_ring->ring_name, barco_ring->ring_base,
                    barco_ring->ring_size);

    return SDK_RET_OK;
}

bool
elba_barco_cp_hot_poller (elba_barco_ring_t *barco_ring, uint32_t req_tag)
{
    /* TBD */
    return FALSE;
}

sdk_ret_t
elba_barco_dc_init (elba_barco_ring_t *barco_ring)
{
    elb_top_csr_t &cap0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    elb_hens_csr_t &hens = cap0.md.hens;
    sdk_ret_t ret = SDK_RET_OK;
    uint32_t reg_lo;
    uint32_t reg_hi;

    ret = elba_barco_ring_common_init(barco_ring);
    if (ret != SDK_RET_OK) {
        return ret;
    }

    /*
     * Soft reset entire decompression block;
     * also set the header version
     */
    hens.dhs_crypto_ctl.dc_cfg_glb.read();
    reg_lo = (hens.dhs_crypto_ctl.dc_cfg_glb.fld().convert_to<uint32_t>() &
              ~BARCO_CRYPTO_DC_CFG_GLB_HDR_VER_MASK)    |
             BARCO_CRYPTO_DC_CFG_GLB_SOFT_RESET         |
             BARCO_CRYPTO_DC_CFG_GLB_HDR_VER;
    hens.dhs_crypto_ctl.dc_cfg_glb.fld(reg_lo);
    hens.dhs_crypto_ctl.dc_cfg_glb.write();

    /*
     * Bring out of reset
     */
    hens.dhs_crypto_ctl.dc_cfg_glb.fld(reg_lo &
                                       ~BARCO_CRYPTO_DC_CFG_GLB_SOFT_RESET);
    hens.dhs_crypto_ctl.dc_cfg_glb.write();

    hens.dhs_crypto_ctl.dc_cfg_q1_base_adr_w0.fld((uint32_t)(barco_ring->ring_base & 0xffffffff));
    hens.dhs_crypto_ctl.dc_cfg_q1_base_adr_w0.write();
    hens.dhs_crypto_ctl.dc_cfg_q1_base_adr_w1.fld((uint32_t)(barco_ring->ring_base >> 32));
    hens.dhs_crypto_ctl.dc_cfg_q1_base_adr_w1.write();

    /*
     * Enable all 2 dc engines
     */
    hens.dhs_crypto_ctl.dc_cfg_ueng_w0.read();
    reg_lo = hens.dhs_crypto_ctl.dc_cfg_ueng_w0.fld().convert_to<uint32_t>() |
             BARCO_CRYPTO_DC_UENG_LO_EN_ALL;
    hens.dhs_crypto_ctl.dc_cfg_ueng_w1.read();
    reg_hi = (hens.dhs_crypto_ctl.dc_cfg_ueng_w1.fld().convert_to<uint32_t>() &
              ~BARCO_CRYPTO_DC_UENG_HI_HMEM_FILL_ZERO) |
             (BARCO_CRYPTO_DC_UENG_HI_SHA_DATA_UNCOMP  |
              BARCO_CRYPTO_DC_UENG_HI_CSUM_ON_UNCOMP |
              BARCO_CRYPTO_DC_UENG_HI_INTEG_APP_STATUS);
    hens.dhs_crypto_ctl.dc_cfg_ueng_w0.fld(reg_lo);
    hens.dhs_crypto_ctl.dc_cfg_ueng_w0.write();
    hens.dhs_crypto_ctl.dc_cfg_ueng_w1.fld(reg_hi);
    hens.dhs_crypto_ctl.dc_cfg_ueng_w1.write();

    /*
     * Enable warm queue
     */
    hens.dhs_crypto_ctl.dc_cfg_dist.read();
    reg_lo = (hens.dhs_crypto_ctl.dc_cfg_dist.fld().convert_to<uint32_t>()  &
              ~BARCO_CRYPTO_DC_DIST_DESC_Q_SIZE(BARCO_CRYPTO_DC_DIST_DESC_Q_SIZE_MASK))   |
             BARCO_CRYPTO_DC_DIST_DESC_Q_EN                                               |
             BARCO_CRYPTO_DC_DIST_DESC_Q_SIZE(BARCO_CRYPTO_DC_RING_SIZE);
    hens.dhs_crypto_ctl.dc_cfg_dist.fld(reg_lo);
    hens.dhs_crypto_ctl.dc_cfg_dist.write();

    hens.dhs_crypto_ctl.dc_cfg_host_opaque_tag_adr_w0.fld((uint32_t)(barco_ring->opaque_tag_addr & 0xffffffff));
    hens.dhs_crypto_ctl.dc_cfg_host_opaque_tag_adr_w0.write();
    hens.dhs_crypto_ctl.dc_cfg_host_opaque_tag_adr_w1.fld((uint32_t)(barco_ring->opaque_tag_addr >> 32));
    hens.dhs_crypto_ctl.dc_cfg_host_opaque_tag_adr_w1.write();
    hens.dhs_crypto_ctl.dc_cfg_host_opaque_tag_data.fld(barco_ring->opaqe_tag_value);
    hens.dhs_crypto_ctl.dc_cfg_host_opaque_tag_data.write();

    hens.dhs_crypto_ctl.dc_cfg_q1_pd_idx.fld(barco_ring->producer_idx);
    hens.dhs_crypto_ctl.dc_cfg_q1_pd_idx.write();

    SDK_TRACE_DEBUG("Barco decompression ring \"%s\" base setup @ 0x%lx, descriptor count %u",
                    barco_ring->ring_name, barco_ring->ring_base,
                    barco_ring->ring_size);

    return SDK_RET_OK;
}

bool
elba_barco_dc_poller (elba_barco_ring_t *barco_ring, uint32_t req_tag)
{
    /* TBD */
    return FALSE;
}

sdk_ret_t
elba_barco_dc_hot_init (elba_barco_ring_t *barco_ring)
{
    elb_top_csr_t &cap0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    elb_hens_csr_t &hens = cap0.md.hens;
    sdk_ret_t ret = SDK_RET_OK;
    uint32_t reg_lo;

    ret = elba_barco_ring_common_init(barco_ring);
    if (ret != SDK_RET_OK) {
        return ret;
    }

    hens.dhs_crypto_ctl.dc_cfg_q0_base_adr_w0.fld((uint32_t)(barco_ring->ring_base & 0xffffffff));
    hens.dhs_crypto_ctl.dc_cfg_q0_base_adr_w0.write();
    hens.dhs_crypto_ctl.dc_cfg_q0_base_adr_w1.fld((uint32_t)(barco_ring->ring_base >> 32));
    hens.dhs_crypto_ctl.dc_cfg_q0_base_adr_w1.write();

    /*
     * Enable hot queue
     */
    hens.dhs_crypto_ctl.dc_cfg_dist.read();
    reg_lo = (hens.dhs_crypto_ctl.dc_cfg_dist.fld().convert_to<uint32_t>()  &
             ~BARCO_CRYPTO_DC_DIST_DESC_HOTQ_SIZE(BARCO_CRYPTO_DC_DIST_DESC_HOTQ_SIZE_MASK))    |
             BARCO_CRYPTO_DC_DIST_DESC_HOTQ_EN                                                  |
             BARCO_CRYPTO_DC_DIST_DESC_HOTQ_SIZE(BARCO_CRYPTO_DC_HOT_RING_SIZE);
    hens.dhs_crypto_ctl.dc_cfg_dist.fld(reg_lo);
    hens.dhs_crypto_ctl.dc_cfg_dist.write();

    hens.dhs_crypto_ctl.dc_cfg_q0_pd_idx.fld(barco_ring->producer_idx);
    hens.dhs_crypto_ctl.dc_cfg_q0_pd_idx.write();

    SDK_TRACE_DEBUG("Barco decompression hot ring \"%s\" base setup @ 0x%lx, descriptor count %u",
                    barco_ring->ring_name, barco_ring->ring_base,
                    barco_ring->ring_size);

    return SDK_RET_OK;
}

bool
elba_barco_dc_hot_poller (elba_barco_ring_t *barco_ring, uint32_t req_tag)
{
    /* TBD */
    return FALSE;
}

bool
elba_barco_sym1_ring0_poller (elba_barco_ring_t *barco_ring, uint32_t req_tag)
{
    bool                                ret = FALSE;
    uint32_t                            curr_opaque_tag = 0;

    if (sdk::asic::asic_mem_read(barco_ring->opaque_tag_addr,
                                 (uint8_t*)&curr_opaque_tag,
                                 sizeof(curr_opaque_tag))) {
        SDK_TRACE_ERR("Poll:%s: Failed to retrieve current opaque tag value @ 0x%lx",
                      barco_ring->ring_name,
                      (uint64_t) barco_ring->opaque_tag_addr);
        return FALSE;
    }
    else {
        SDK_TRACE_DEBUG("Poll:%s: Retrievd opaque tag value: %d",
                        barco_ring->ring_name, curr_opaque_tag);
        /* TODO: Handle wraparounds */
        if (curr_opaque_tag >= req_tag)
            ret = TRUE;
    }

    return ret;

#if 0
    elb_top_csr_t &                     cap0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    elb_hens_csr_t &                    hens = cap0.md.hens;

    /* TBD - use Doorbell address in req descriptor  to track request completions */
    if (hens.dhs_crypto_ctl.gcm1_consumer_idx.fld() != barco_ring->consumer_idx) {
        /* New responses available */
        return TRUE;
    }

    return FALSE;
#endif
}

sdk_ret_t
elba_barco_sym1_ring0_queue_request (struct elba_barco_ring_s *barco_ring,
                                     void *req,
                                     uint32_t *req_tag, bool schedule_barco)
{
    uint64_t slot_addr = 0;
    sdk_ret_t ret = SDK_RET_OK;
    barco_symm_req_descriptor_t *sym_req_descr = NULL;

    /*
     * We'll use the opaque-tag address to track response from
     * barco.
     */
    sym_req_descr = (barco_symm_req_descriptor_t*) req;

    sym_req_descr->opaque_tag_value = barco_ring->opaqe_tag_value;
    sym_req_descr->opaque_tag_wr_en = 1;

    /*
     * Use doorbell-address in the symm request descriptor to
     * track response from barco.
     */
    sym_req_descr->doorbell_addr = barco_ring->opaque_tag_addr;
    sym_req_descr->doorbell_data = barco_ring->opaqe_tag_value;

    slot_addr = barco_ring->ring_base + (barco_ring->producer_idx *
                                         barco_ring->descriptor_size);

    if (sdk::asic::asic_mem_write(slot_addr, (uint8_t*)req,
                                  barco_ring->descriptor_size)) {
        SDK_TRACE_ERR("Failed to write SYM1-RING0 Req descriptor entry for %s @ 0x%lx",
                      barco_ring->ring_name, (uint64_t) slot_addr);
        ret = SDK_RET_INVALID_ARG;
    }

    barco_ring->producer_idx = (barco_ring->producer_idx + 1) &
        (barco_ring->ring_size - 1);

    if (schedule_barco) {
        /* Barco doorbell */
#if 0
        elb_top_csr_t &cap0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
        elb_hens_csr_t &hens = cap0.md.hens;

        hens.dhs_crypto_ctl.gcm1_producer_idx.fld(barco_ring->producer_idx);
        hens.dhs_crypto_ctl.gcm1_producer_idx.write();
#endif
    }
    *req_tag = barco_ring->opaqe_tag_value++;

    return ret;
}

bool
elba_barco_ring_poll (barco_rings_t barco_ring_type, uint32_t req_tag)
{
    elba_barco_ring_t          *barco_ring;

    barco_ring = &barco_rings[barco_ring_type];
    if (barco_ring->poller) {
        return barco_ring->poller(barco_ring, req_tag);
    }

    return FALSE;
}

sdk_ret_t
elba_barco_ring_queue_request (barco_rings_t barco_ring_type,
                               void *req, uint32_t *req_tag,
                               bool schedule_barco)
{
    elba_barco_ring_t          *barco_ring;

    /* TODO:
     *  - Locking when queue is shared across multiple CPUs
     *  - Queue full check in the common API
     */
    barco_ring = &barco_rings[barco_ring_type];
    return barco_ring->queue_request(barco_ring, req, req_tag, schedule_barco);
}

sdk_ret_t
elba_barco_ring_consume (barco_rings_t barco_ring_type)
{
    elba_barco_ring_t          *barco_ring;

    barco_ring = &barco_rings[barco_ring_type];
    barco_ring->consumer_idx = (barco_ring->consumer_idx + 1) &
        (barco_ring->ring_size - 1);
    return SDK_RET_OK;
}

sdk_ret_t
elba_barco_rings_init (platform_type_t platform)
{
    uint16_t        idx;
    uint64_t        opa_tag_addr = 0;
    uint32_t        opa_tag_def_val = 0;
    uint64_t        shadow_pndx_addr = 0;
    uint64_t        shadow_pndx_def_val = 0;
    uint64_t        shadow_pndx_total = 0;
    uint32_t        shadow_pndx_size = 0;
    sdk_ret_t       ret = SDK_RET_OK;

    shadow_pndx_total = CRYPTO_BARCO_RES_HBM_MEM_512B_SIZE;
    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_HBM_MEM_512B,
                               NULL, &shadow_pndx_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR("Failed to allocate shadow pndx storage for rings");
        return ret;
    }
    for (idx = 0; idx < BARCO_RING_MAX; idx++) {
        barco_rings[idx].producer_idx = 0;
        barco_rings[idx].consumer_idx = 0;

        if (barco_rings[idx].ring_size != 0) {
            barco_rings[idx].opaqe_tag_value = 0x1;
            /* FIXME: 512 byte is an overkill, on the real target use storage in the ring structure */
            ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_HBM_MEM_512B,
                                       NULL, &opa_tag_addr);
            if (ret != SDK_RET_OK) {
                SDK_TRACE_ERR("Failed to allocate opaque tag storage for ring %s",
                              barco_rings[idx].ring_name);
                return ret;
            }
            SDK_TRACE_DEBUG("Ring: %s: Allocated opaque tag @ 0x%lx",
                            barco_rings[idx].ring_name, opa_tag_addr);
            if(sdk::asic::asic_mem_write(opa_tag_addr,
                                         (uint8_t *)&opa_tag_def_val,
                                         sizeof(opa_tag_def_val))) {
                SDK_TRACE_ERR("Ring: %s: Failed to initialized opaque tag @ 0x%lx",
                    barco_rings[idx].ring_name, opa_tag_addr);
                return SDK_RET_HW_PROGRAM_ERR;
            }
            SDK_TRACE_DEBUG("Ring: %s: initialized opaque tag to 0 @ 0x%lx",
                barco_rings[idx].ring_name, opa_tag_addr);

            barco_rings[idx].opaque_tag_addr = opa_tag_addr;
        }
        shadow_pndx_size = barco_rings[idx].shadow_pndx_size;
        if (shadow_pndx_size) {
            assert(shadow_pndx_size <= sizeof(shadow_pndx_def_val));
            if (shadow_pndx_total < shadow_pndx_size) {
                SDK_TRACE_ERR("Out of shadow pndx storage for ring %s",
                              barco_rings[idx].ring_name);
                return SDK_RET_HW_PROGRAM_ERR;
            }
            barco_rings[idx].shadow_pndx_addr = shadow_pndx_addr;
            if(sdk::asic::asic_mem_write(shadow_pndx_addr,
                                         (uint8_t *)&shadow_pndx_def_val,
                                         shadow_pndx_size)) {
                SDK_TRACE_ERR("Ring: %s: Failed to initialize shadow pndx @ 0x%lx",
                              barco_rings[idx].ring_name, shadow_pndx_addr);
                return SDK_RET_HW_PROGRAM_ERR;
            }
            SDK_TRACE_DEBUG("Ring: %s: initialized shadow pndx to 0 @ 0x%lx",
                            barco_rings[idx].ring_name, shadow_pndx_addr);
            shadow_pndx_addr += shadow_pndx_size;
            shadow_pndx_total -= shadow_pndx_size;
        }

        /* Model does not support sw_reset */
        if (barco_rings[idx].sw_reset_capable) {
            if ((platform != platform_type_t::PLATFORM_TYPE_HAPS) &&
                (platform != platform_type_t::PLATFORM_TYPE_HW)) {
                barco_rings[idx].sw_reset_capable = false;
            }
        }
        if (barco_rings[idx].init) {
            ret = barco_rings[idx].init(&barco_rings[idx]);
            if (ret != SDK_RET_OK) {
                SDK_TRACE_ERR("Failed to initialize Barco Ring %s",
                              barco_rings[idx].ring_name);
                return ret;
            }
        }

    }
    return ret;
}

sdk_ret_t
get_opaque_tag_addr (barco_rings_t ring_type, uint64_t* addr)
{
    *addr = barco_rings[ring_type].opaque_tag_addr;
    return SDK_RET_OK;
}

sdk_ret_t
elba_barco_get_meta_config_info (barco_rings_t ring_type,
                                 uint64_t* shadow_pndx_addr,
                                 uint32_t *shadow_pndx_size,
                                 uint32_t *desc_size,
                                 uint32_t *opaque_tag_size)
{
    *shadow_pndx_addr = barco_rings[ring_type].shadow_pndx_addr;
    *desc_size = barco_rings[ring_type].descriptor_size;
    *shadow_pndx_size = barco_rings[ring_type].shadow_pndx_size;
    *opaque_tag_size = barco_rings[ring_type].opaque_tag_size;
    return SDK_RET_OK;
}

sdk_ret_t
elba_barco_get_meta_config_info (barco_rings_t ring_type,
                                 barco_ring_meta_config_t *meta)
{
    meta->shadow_pndx_addr = barco_rings[ring_type].shadow_pndx_addr;
    meta->desc_size = barco_rings[ring_type].descriptor_size;
    meta->pndx_size = barco_rings[ring_type].shadow_pndx_size;
    meta->opaque_tag_size = barco_rings[ring_type].opaque_tag_size;
    meta->ring_base = barco_rings[ring_type].ring_base;
    meta->producer_idx_addr = barco_rings[ring_type].producer_idx_addr;
    meta->opaque_tag_addr = barco_rings[ring_type].opaque_tag_addr;
    meta->ring_size = barco_rings[ring_type].ring_size;
    return SDK_RET_OK;
}

sdk_ret_t
elba_barco_get_capabilities (barco_rings_t ring_type,
                             bool *sw_reset_capable, bool *sw_enable_capable)
{
    *sw_reset_capable = barco_rings[ring_type].sw_reset_capable;
    *sw_enable_capable = barco_rings[ring_type].sw_enable_capable;
    return SDK_RET_OK;
}

sdk_ret_t
elba_barco_asym_req_descr_get (uint32_t slot_index,
                               barco_asym_descr_t *asym_req_descr)
{
  /* To be implemented */
  return SDK_RET_OK;
}

sdk_ret_t
elba_barco_symm_req_descr_get (barco_rings_t ring_type,
                               uint32_t slot_index,
                               barco_symm_descr_t *symm_req_descr)
{
    barco_symm_req_descriptor_t *req_descr;
    elba_barco_ring_t          *barco_ring;
    uint64_t                    slot_addr;

    barco_ring = &barco_rings[ring_type];
    if (!barco_ring->ring_size || !barco_ring->ring_base)
        return(SDK_RET_OK);

    uint8_t  value[barco_ring->descriptor_size];
    uint32_t index = (slot_index % barco_ring->ring_size);
    slot_addr = barco_ring->ring_base + (index * barco_ring->descriptor_size);
    SDK_TRACE_DEBUG("%s@0x%x: Ring base 0x%lx, slot_addr 0x%lx, read size 0x%x",
                    barco_ring->ring_name, barco_ring->ring_size,
                    barco_ring->ring_base, (uint64_t) slot_addr,
                    barco_ring->descriptor_size);

    if (sdk::asic::asic_mem_read(slot_addr, value, sizeof(value))) {
        SDK_TRACE_ERR("%s@0x%lx: Failed to read Symmetric request descriptor entry",
                      barco_ring->ring_name, (uint64_t) slot_addr);
        return SDK_RET_INVALID_ARG;
    }

    req_descr = (barco_symm_req_descriptor_t *) value;

    /* Fields already in Little-endian */
    symm_req_descr->ilist_addr = req_descr->input_list_addr;
    symm_req_descr->olist_addr = req_descr->output_list_addr;
    symm_req_descr->command = req_descr->command;
    symm_req_descr->key_desc_index = req_descr->key_descr_idx;
    symm_req_descr->iv_addr = req_descr->iv_address;
    symm_req_descr->status_addr = req_descr->status_addr;
    symm_req_descr->doorbell_addr = req_descr->doorbell_addr;
    symm_req_descr->doorbell_data = req_descr->doorbell_data;
    symm_req_descr->header_size = req_descr->header_size;
    symm_req_descr->second_key_desc_index = req_descr->second_key_descr_idx;

    /* IV is not directly located in the ring, hence dereference it */

    if (req_descr->iv_address) {
        if(sdk::asic::asic_mem_read(req_descr->iv_address,
                   (uint8_t*)&symm_req_descr->salt,
                   sizeof(symm_req_descr->salt))) {
           SDK_TRACE_ERR("%s@0x%lx: Failed to read the Salt information from HBM",
                         barco_ring->ring_name,
                         (uint64_t) req_descr->iv_address);
        }
        if(sdk::asic::asic_mem_read(req_descr->iv_address + 4,
                   (uint8_t*)&symm_req_descr->explicit_iv,
                   sizeof(symm_req_descr->explicit_iv))) {
            SDK_TRACE_ERR("%s@0x%lx: Failed to read the explicit IV information from HBM",
                          barco_ring->ring_name,
                          (uint64_t) (req_descr->iv_address + 4));
        }
    }
    if (req_descr->status_addr) {
        if(sdk::asic::asic_mem_read(req_descr->status_addr,
                   (uint8_t*)&symm_req_descr->barco_status,
               sizeof(symm_req_descr->barco_status))) {
           SDK_TRACE_ERR("%s@0x%lx: Failed to read the Barco Status information from HBM",
                         barco_ring->ring_name,
                         (uint64_t) req_descr->status_addr);
        }
    }
    return SDK_RET_OK;
}

sdk_ret_t
elba_barco_ring_meta_get (barco_rings_t ring_type, uint32_t *pi, uint32_t *ci)
{
    elb_top_csr_t &cap0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    elb_mpns_csr_t &mpns = cap0.mp.mpns;
    elb_hens_csr_t &hens = cap0.md.hens;

    /*
     * Read the PI/CI registers for the corresponding barco ring.
     */
    switch(ring_type) {
    case BARCO_RING_GCM0:
        hens.dhs_crypto_ctl.sym0_ring0_producer_idx.read();
        *pi = hens.dhs_crypto_ctl.sym0_ring0_producer_idx.fld().convert_to<uint32_t>();
        hens.dhs_crypto_ctl.sym0_ring0_consumer_idx.read();
        *ci = hens.dhs_crypto_ctl.sym0_ring0_consumer_idx.fld().convert_to<uint32_t>();
        break;
    case BARCO_RING_GCM1:
        hens.dhs_crypto_ctl.sym1_ring0_producer_idx.read();
        *pi = hens.dhs_crypto_ctl.sym1_ring0_producer_idx.fld().convert_to<uint32_t>();
        hens.dhs_crypto_ctl.sym1_ring0_consumer_idx.read();
        *ci = hens.dhs_crypto_ctl.sym1_ring0_consumer_idx.fld().convert_to<uint32_t>();
        break;
    case BARCO_RING_MPP1:
      mpns.dhs_crypto_ctl.mpp1_producer_idx.read();
      *pi = mpns.dhs_crypto_ctl.mpp1_producer_idx.fld().convert_to<uint32_t>();
      mpns.dhs_crypto_ctl.mpp1_consumer_idx.read();
      *ci = mpns.dhs_crypto_ctl.mpp1_consumer_idx.fld().convert_to<uint32_t>();
      break;
    case BARCO_RING_MPP3:
      mpns.dhs_crypto_ctl.mpp3_producer_idx.read();
      *pi = mpns.dhs_crypto_ctl.mpp3_producer_idx.fld().convert_to<uint32_t>();
      mpns.dhs_crypto_ctl.mpp3_consumer_idx.read();
      *ci = mpns.dhs_crypto_ctl.mpp3_consumer_idx.fld().convert_to<uint32_t>();
      break;
    case BARCO_RING_MPP0:
      mpns.dhs_crypto_ctl.mpp0_producer_idx.read();
      *pi = mpns.dhs_crypto_ctl.mpp0_producer_idx.fld().convert_to<uint32_t>();
      mpns.dhs_crypto_ctl.mpp0_consumer_idx.read();
      *ci = mpns.dhs_crypto_ctl.mpp0_consumer_idx.fld().convert_to<uint32_t>();
      break;
    case BARCO_RING_MPP2:
      mpns.dhs_crypto_ctl.mpp2_producer_idx.read();
      *pi = mpns.dhs_crypto_ctl.mpp2_producer_idx.fld().convert_to<uint32_t>();
      mpns.dhs_crypto_ctl.mpp2_consumer_idx.read();
      *ci = mpns.dhs_crypto_ctl.mpp2_consumer_idx.fld().convert_to<uint32_t>();
      break;
    default:
      SDK_TRACE_ERR("%s: Ring type not supported yet",
                    barco_rings[ring_type].ring_name);
      return SDK_RET_INVALID_ARG;
      break;
    }
    SDK_TRACE_DEBUG("%s: PI 0x%x, CI 0x%x",
                    barco_rings[ring_type].ring_name, *pi, *ci);
    return SDK_RET_OK;
}

}    // namespace elba
}    // namespace platform
}    // namespace sdk
