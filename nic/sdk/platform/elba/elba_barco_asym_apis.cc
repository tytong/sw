// {C} Copyright 2020 Pensando Systems Inc. All rights reserved

#include "platform/elba/elba_barco_crypto.hpp"
#include "platform/elba/elba_barco_res.hpp"
#include "platform/elba/elba_barco_rings.hpp"
#include "platform/elba/elba_barco_asym_apis.hpp"
#include "asic/rw/asicrw.hpp"
#include <openssl/async.h>

namespace sdk {
namespace platform {
namespace elba {

sdk_ret_t
elba_barco_asym_poll_pend_req (uint32_t batch_size, uint32_t* id_count,
                               uint32_t *ids)
{
    dllist_ctxt_t          *entry = NULL;
    crypto_pend_req_t      *req = NULL;

    *id_count = 0;

    if(dllist_empty(&g_pend_req_list)) {
        return SDK_RET_OK;
    }

    dllist_for_each(entry, &g_pend_req_list) {
        req = dllist_entry(entry, crypto_pend_req_t, list_ctxt);
        SDK_TRACE_DEBUG("Checking status for req: %d", req->hw_id);
        if(elba_barco_ring_poll(BARCO_RING_ASYM, req->hw_id) != TRUE) {
            return SDK_RET_OK;
        }
        ids[*id_count] = req->sw_id;
        SDK_TRACE_DEBUG("Request completed for count: %d hw-id: %d sw-id: %d id: %d",
                        *id_count, req->hw_id, req->sw_id, ids[*id_count]);
        elba_barco_del_pend_req_from_db(req);
        if(++(*id_count) >= batch_size)
            break;
    }
    return SDK_RET_OK;
}

sdk_ret_t
elba_barco_asym_add_pend_req (uint32_t hw_id, uint32_t sw_id)
{
    return elba_barco_add_pend_req_to_db(hw_id, sw_id);
}

void
elba_barco_wait_for_resp (uint32_t req_tag, bool async_en,
                          const uint8_t *unique_key)
{
    ASYNC_JOB           *job = NULL;
    ASYNC_WAIT_CTX      *wait_ctx = NULL;
    OSSL_ASYNC_FD       hw_id;

    if(async_en && ((job = ASYNC_get_current_job()) != NULL)) {
        SDK_TRACE_DEBUG("Async: Pausing existing job to wait for resp..");
        if((wait_ctx = ASYNC_get_wait_ctx(job)) == NULL) {
            SDK_TRACE_ERR("Failed to get wait_ctx");
            return;
        }

        hw_id = req_tag;
        if(ASYNC_WAIT_CTX_set_wait_fd(wait_ctx, unique_key,
                                      hw_id, NULL, NULL) == 0) {
            SDK_TRACE_ERR("Failed to set fd to wait_ctx");
            return;
        }

        ASYNC_pause_job();
        SDK_TRACE_DEBUG("Async: Job resumed..");
    } else {
        SDK_TRACE_DEBUG("Poll: Waiting for resp");
        /* Poll for completion */
        while (elba_barco_ring_poll(BARCO_RING_ASYM, req_tag) != TRUE) {
            //SDK_TRACE_DEBUG("Waiting for Barco completion...");
        }
    }
    return;
}

sdk_ret_t
elba_barco_asym_ecc_point_mul_p256 (uint8_t *p, uint8_t *n,
                                    uint8_t *xg, uint8_t *yg,
                                    uint8_t *a, uint8_t *b,
                                    uint8_t *x1, uint8_t *y1, uint8_t *k,
                                    uint8_t *x3, uint8_t *y3)
{
    sdk_ret_t                   ret = SDK_RET_OK;
    uint64_t                    ilist_dma_descr_addr = 0,
                                olist_dma_descr_addr = 0;
    uint64_t                    ilist_mem_addr = 0,
                                olist_mem_addr = 0, curr_ptr = 0;
    barco_asym_descriptor_t     asym_req_descr;
    barco_asym_dma_descriptor_t ilist_dma_descr, olist_dma_descr;
    int32_t                     ecc_pm_p256_key_idx = -1;
    elba_barco_asym_key_desc_t  asym_key;
    uint32_t                    req_tag = 0;

    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"p", (char *)p, 32);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"a", (char *)a, 32);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"b", (char *)b, 32);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"n", (char *)n, 32);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"xg", (char *)xg, 32);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"yg", (char *)yg, 32);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"x1", (char *)x1, 32);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"y1", (char *)y1, 32);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"k", (char *)k, 32);

    ret = elba_barco_asym_alloc_key(&ecc_pm_p256_key_idx);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR("ECC Point Mul P256: Failed to allocate key descriptor");
        goto cleanup;
    }
    SDK_TRACE_DEBUG("ECC Point Mul P256: Allocated Key Descr @ %d",
                    ecc_pm_p256_key_idx);

    asym_key.key_param_list = 0; /* Barco does not use key space for ECC Point MUL for now */
    asym_key.command_reg = (ELBA_BARCO_ASYM_CMD_SWAP_BYTES |
                            ELBA_BARCO_ASYM_CMD_SIZE_OF_OP(32) |
                            ELBA_BARCO_ASYM_CMD_ECC_POINT_MUL);

    ret = elba_barco_asym_write_key(ecc_pm_p256_key_idx, &asym_key);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR("ECC Point Mul P256: Failed to write key: %d",
                      ecc_pm_p256_key_idx);
        goto cleanup;
    }
    SDK_TRACE_DEBUG("ECC Point Mul P256: Setup key @ %d",
                    ecc_pm_p256_key_idx);

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                               NULL, &ilist_dma_descr_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR("ECC Point Mul P256: Failed to allocate memory for ilist DMA Descr");
        goto cleanup;
    }
    SDK_TRACE_DEBUG("ECC Point Mul P256: Allocated memory for ilist DMA Descr @ 0x%lx",
                    ilist_dma_descr_addr);

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                               NULL, &olist_dma_descr_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR("ECC Point Mul P256: Failed to allocate memory for olist DMA Descr");
        goto cleanup;
    }
    SDK_TRACE_DEBUG("ECC Point Mul P256: Allocated memory for olist DMA Descr @ 0x%lx",
                    olist_dma_descr_addr);

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_HBM_MEM_512B,
                               NULL, &ilist_mem_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR("ECC Point Mul P256: Failed to allocate memory for ilist content");
        goto cleanup;
    }
    SDK_TRACE_DEBUG("ECC Point Mul P256: Allocated memory for input mem @ 0x%lx",
                    ilist_mem_addr);

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_HBM_MEM_512B,
                               NULL, &olist_mem_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR("ECC Point Mul P256: Failed to allocate memory for olist content");
        goto cleanup;
    }
    SDK_TRACE_DEBUG("ECC Point Mul P256: Allocated memory for output mem @ 0x%lx",
                    olist_mem_addr);

    /* Copy the input to the ilist memory */
    curr_ptr = ilist_mem_addr;

    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)p, 32)) {
        SDK_TRACE_ERR("ECC Point Mul P256: Failed to write ECC param p into ilist memory @ 0x%lx",
                      (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    curr_ptr += 32;

    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)n, 32)) {
        SDK_TRACE_ERR("ECC Point Mul P256: Failed to write ECC param n into ilist memory @ 0x%lx",
                      (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    curr_ptr += 32;

    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)a, 32)) {
        SDK_TRACE_ERR("ECC Point Mul P256: Failed to write ECC param a into ilist memory @ 0x%lx",
                      (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    curr_ptr += 32;

    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)b, 32)) {
        SDK_TRACE_ERR("ECC Point Mul P256: Failed to write ECC param b into ilist memory @ 0x%lx",
                      (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    curr_ptr += 32;

    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)x1, 32)) {
        SDK_TRACE_ERR("ECC Point Mul P256: Failed to write ECC param x1 into ilist memory @ 0x%lx",
                      (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    curr_ptr += 32;

    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)y1, 32)) {
        SDK_TRACE_ERR("ECC Point Mul P256: Failed to write ECC param y1 into ilist memory @ 0x%lx",
                      (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    curr_ptr += 32;

    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)k, 32)) {
        SDK_TRACE_ERR("ECC Point Mul P256: Failed to write ECC param k into ilist memory @ 0x%lx",
                      (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    curr_ptr += 32;

    /* Setup ilist DMA descriptor */
    ilist_dma_descr.address = ilist_mem_addr;
    ilist_dma_descr.stop = 1;
    ilist_dma_descr.rsvd0 = 1;
    ilist_dma_descr.next = 0;
    ilist_dma_descr.int_en = 0;
    ilist_dma_descr.discard = 0;
    ilist_dma_descr.realign = 1;
    ilist_dma_descr.cst_addr = 0;
    ilist_dma_descr.length = (curr_ptr - ilist_mem_addr);
    if (sdk::asic::asic_mem_write(ilist_dma_descr_addr,
                                  (uint8_t*) &ilist_dma_descr,
                                  sizeof(ilist_dma_descr))) {
        SDK_TRACE_ERR("ECC Point Mul P256: Failed to write ilist DMA Descr @ 0x%lx",
                      (uint64_t) ilist_dma_descr_addr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }

    /* Setup olist DMA descriptor */
    olist_dma_descr.address = olist_mem_addr;
    olist_dma_descr.stop = 1;
    olist_dma_descr.rsvd0 = 1;
    olist_dma_descr.next = 0;
    olist_dma_descr.int_en = 0;
    olist_dma_descr.discard = 0;
    olist_dma_descr.realign = 1;
    olist_dma_descr.cst_addr = 0;
    olist_dma_descr.length = (2 * 32 ); /* Outputs: x3, y3 */
    if (sdk::asic::asic_mem_write(olist_dma_descr_addr,
                                  (uint8_t*) &olist_dma_descr,
                                  sizeof(olist_dma_descr))) {
        SDK_TRACE_ERR("ECC Point Mul P256: Failed to write olist DMA Descr @ 0x%lx",
                      (uint64_t) olist_dma_descr_addr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }

    /* Setup Asymmetric Request Descriptor */
    asym_req_descr.input_list_addr = ilist_dma_descr_addr;
    asym_req_descr.output_list_addr = olist_dma_descr_addr;
    asym_req_descr.key_descr_idx = ecc_pm_p256_key_idx;
    asym_req_descr.status_addr = curr_ptr;
    asym_req_descr.opaque_tag_value = 0;
    asym_req_descr.opage_tag_wr_en = 0;
    asym_req_descr.flag_a = 0;
    asym_req_descr.flag_b = 0;

    ret = elba_barco_ring_queue_request(BARCO_RING_ASYM,
                                        (void *) &asym_req_descr,
                                        &req_tag, true);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR("ECC Point Mul P256: Failed to enqueue request");
        ret = SDK_RET_ERR;
        goto cleanup;
    }
    /* Poll for completion */
    while (elba_barco_ring_poll(BARCO_RING_ASYM, req_tag) != TRUE) {
        //SDK_TRACE_DEBUG("ECC Point Mul P256: Waiting for Barco completion...");
    }

    /* Copy out the results */
    if (sdk::asic::asic_mem_read(olist_mem_addr, (uint8_t*)x3, 32)) {
        SDK_TRACE_ERR("ECC Point Mul P256: Failed to read x3 output from memory @ 0x%lx",
                      (uint64_t) olist_mem_addr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    if (sdk::asic::asic_mem_read(olist_mem_addr + 32, (uint8_t*)y3, 32)) {
        SDK_TRACE_ERR("ECC Point Mul P256: Failed to read y3 output from memory @ 0x%lx",
                      (uint64_t) (olist_mem_addr + 32));
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }

    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"x3", (char *)x3, 32);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"y3", (char *)y3, 32);

cleanup:
    if (olist_mem_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_HBM_MEM_512B, olist_mem_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR("ECC Point Mul P256: Failed to free memory for olist content:0x%lx",
                          olist_mem_addr);
        }
    }

    if (ilist_mem_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_HBM_MEM_512B, ilist_mem_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR("ECC Point Mul P256: Failed to free memory for ilist content:0x%lx",
                          ilist_mem_addr);
        }
    }

    if (olist_dma_descr_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_ASYM_DMA_DESCR, olist_dma_descr_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR("ECC Point Mul P256: Failed to free memory for olist DMA Descr: 0x%lx",
                          olist_dma_descr_addr);
        }
    }

    if (ilist_dma_descr_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_ASYM_DMA_DESCR, ilist_dma_descr_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR("ECC Point Mul P256: Failed to free memory for ilist DMA Descr: 0x%lx",
                          ilist_dma_descr_addr);
        }
    }

    if (ecc_pm_p256_key_idx != -1) {
        ret = elba_barco_asym_free_key(ecc_pm_p256_key_idx);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR("ECC Point Mul P256: Failed to free key descriptor");
        }
    }

    return ret;
}

#define ECC_POINT_MUL_WR_PARAM(param)                                                       \
do {                                                                                        \
    if (pad_size) {                                                                         \
        if (sdk::asic::asic_mem_write(curr_ptr, zero_array, pad_size)) {                    \
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME                                              \
                    " Failed to write pad for ECC param " #param " into ilist memory @ 0x%lx", (uint64_t) curr_ptr); \
            ret = SDK_RET_INVALID_ARG;                                                      \
            goto cleanup;                                                                   \
        }                                                                                   \
        curr_ptr += pad_size;                                                               \
    }                                                                                       \
    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)param, key_size)) {                   \
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME                                                  \
                " Failed to write ECC param " #param " into ilist memory @ 0x%lx", (uint64_t) curr_ptr); \
        ret = SDK_RET_INVALID_ARG;                                                          \
        goto cleanup;                                                                       \
    }                                                                                       \
    curr_ptr += key_size;                                                                   \
} while (0)

static uint8_t zero_array[64] = {0};

sdk_ret_t
elba_barco_asym_ecc_point_mul (uint16_t key_size, uint8_t *p, uint8_t *n,
                                uint8_t *xg, uint8_t *yg,
                                uint8_t *a, uint8_t *b,
                                uint8_t *x1, uint8_t *y1,
                                uint8_t *k, uint8_t *x3, uint8_t *y3)
{
    sdk_ret_t                   ret = SDK_RET_OK;
    uint64_t                    ilist_dma_descr_addr1 = 0,
                                olist_dma_descr_addr = 0;
    uint64_t                    ilist_dma_descr_addr2 = 0;
    uint64_t                    ilist_mem_addr1 = 0,
                                olist_mem_addr = 0,
                                curr_ptr = 0;
    uint64_t                    ilist_mem_addr2 = 0;
    barco_asym_descriptor_t     asym_req_descr;
    barco_asym_dma_descriptor_t ilist_dma_descr, olist_dma_descr;
    int32_t                     ecc_pm_key_idx = -1;
    elba_barco_asym_key_desc_t asym_key;
    uint32_t                    req_tag = 0;
    uint32_t                    status = 0;
    uint16_t                    operand_size = 0, pad_size = 0;

    if (key_size <= 32) {
        operand_size = 32;
    }
    else if (key_size <= 64) {
        operand_size = 64;
    }
    else if (key_size <= 128) {
        operand_size = 128;
    }
    pad_size = operand_size - key_size;

#undef ELBA_BARCO_API_NAME
#define ELBA_BARCO_API_NAME "ECC Point Mul: "

    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"p", (char *)p, key_size);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"a", (char *)a, key_size);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"b", (char *)b, key_size);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"n", (char *)n, key_size);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"xg", (char *)xg, key_size);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"yg", (char *)yg, key_size);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"x1", (char *)x1, key_size);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"y1", (char *)y1, key_size);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"k", (char *)k, key_size);

    ret = elba_barco_asym_alloc_key(&ecc_pm_key_idx);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate key descriptor");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated Key Descr @ %d",
                    ecc_pm_key_idx);

    asym_key.key_param_list = 0; /* Barco does not use key space for ECC Point MUL for now */
    asym_key.command_reg = (ELBA_BARCO_ASYM_CMD_SWAP_BYTES |
                            ELBA_BARCO_ASYM_CMD_SIZE_OF_OP(key_size) |
                            ELBA_BARCO_ASYM_CMD_ECC_POINT_MUL);

    ret = elba_barco_asym_write_key(ecc_pm_key_idx, &asym_key);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write key: %d",
                      ecc_pm_key_idx);
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Setup key @ %d", ecc_pm_key_idx);


    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
            NULL, &ilist_dma_descr_addr1);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME " Failed to allocate memory for ilist DMA Descr1");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME " Allocated memory for ilist DMA Descr1 @ 0x%lx",
                    ilist_dma_descr_addr1);
    if (operand_size > 64) {
        ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                NULL, &ilist_dma_descr_addr2);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME " Failed to allocate memory for ilist DMA Descr2");
            goto cleanup;
        }
        SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME " Allocated memory for ilist DMA Descr2 @ 0x%lx",
                        ilist_dma_descr_addr2);
    }

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
            NULL, &olist_dma_descr_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME " Failed to allocate memory for olist DMA Descr");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME " Allocated memory for olist DMA Descr @ 0x%lx",
                    olist_dma_descr_addr);

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_HBM_MEM_512B,
            NULL, &ilist_mem_addr1);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME " Failed to allocate memory for ilist1 content");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME " Allocated memory for input mem1 @ 0x%lx",
                    ilist_mem_addr1);

    if (operand_size > 64) {
        ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_HBM_MEM_512B,
                NULL, &ilist_mem_addr2);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME " Failed to allocate memory for ilist2 content");
            goto cleanup;
        }
        SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME " Allocated memory for input mem2 @ 0x%lx",
                        ilist_mem_addr2);
    }

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_HBM_MEM_512B,
            NULL, &olist_mem_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME " Failed to allocate memory for olist content");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME " Allocated memory for output mem @ 0x%lx",
                    olist_mem_addr);

    /* Copy the input to the ilist memory */
    curr_ptr = ilist_mem_addr1;

    ECC_POINT_MUL_WR_PARAM(p);

    ECC_POINT_MUL_WR_PARAM(n);

    ECC_POINT_MUL_WR_PARAM(a);

    ECC_POINT_MUL_WR_PARAM(b);

    if (operand_size > 64) {
        /* Setup ilist DMA descriptor */
        ilist_dma_descr.address = ilist_mem_addr1;
        ilist_dma_descr.stop = 0;
        ilist_dma_descr.rsvd0 = 1;
        ilist_dma_descr.next = (ilist_dma_descr_addr2 >> 2);
        ilist_dma_descr.int_en = 0;
        ilist_dma_descr.discard = 0;
        ilist_dma_descr.realign = 1;
        ilist_dma_descr.cst_addr = 0;
        ilist_dma_descr.length = (curr_ptr - ilist_mem_addr1);
        if (sdk::asic::asic_mem_write(ilist_dma_descr_addr1,
                                      (uint8_t*)&ilist_dma_descr,
                                      sizeof(ilist_dma_descr))) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME " Failed to write ilist DMA Descr1 @ 0x%lx",
                          (uint64_t) ilist_dma_descr_addr1);
            ret = SDK_RET_INVALID_ARG;
            goto cleanup;
        }
        curr_ptr = ilist_mem_addr2;
    }

    ECC_POINT_MUL_WR_PARAM(x1);

    ECC_POINT_MUL_WR_PARAM(y1);

    ECC_POINT_MUL_WR_PARAM(k);

    if (operand_size > 64) {
        /* Setup ilist DMA descriptor */
        ilist_dma_descr.address = ilist_mem_addr2;
        ilist_dma_descr.stop = 1;
        ilist_dma_descr.rsvd0 = 1;
        ilist_dma_descr.next = 0;
        ilist_dma_descr.int_en = 0;
        ilist_dma_descr.discard = 0;
        ilist_dma_descr.realign = 1;
        ilist_dma_descr.cst_addr = 0;
        ilist_dma_descr.length = (curr_ptr - ilist_mem_addr2);
        if (sdk::asic::asic_mem_write(ilist_dma_descr_addr2,
                                      (uint8_t*)&ilist_dma_descr,
                                      sizeof(ilist_dma_descr))) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME " Failed to write ilist DMA Descr2 @ 0x%lx",
                          (uint64_t) ilist_dma_descr_addr2);
            ret = SDK_RET_INVALID_ARG;
            goto cleanup;
        }
    } else {
        /* Setup ilist DMA descriptor */
        ilist_dma_descr.address = ilist_mem_addr1;
        ilist_dma_descr.stop = 1;
        ilist_dma_descr.rsvd0 = 1;
        ilist_dma_descr.next = 0;
        ilist_dma_descr.int_en = 0;
        ilist_dma_descr.discard = 0;
        ilist_dma_descr.realign = 1;
        ilist_dma_descr.cst_addr = 0;
        ilist_dma_descr.length = (curr_ptr - ilist_mem_addr1);
        if (sdk::asic::asic_mem_write(ilist_dma_descr_addr1,
                                      (uint8_t*)&ilist_dma_descr,
                                      sizeof(ilist_dma_descr))) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME " Failed to write ilist DMA Descr1 @ 0x%lx",
                          (uint64_t) ilist_dma_descr_addr1);
            ret = SDK_RET_INVALID_ARG;
            goto cleanup;
        }
    }

    /* Setup olist DMA descriptor */
    olist_dma_descr.address = olist_mem_addr;
    olist_dma_descr.stop = 1;
    olist_dma_descr.rsvd0 = 1;
    olist_dma_descr.next = 0;
    olist_dma_descr.int_en = 0;
    olist_dma_descr.discard = 0;
    olist_dma_descr.realign = 1;
    olist_dma_descr.cst_addr = 0;
    olist_dma_descr.length = (2 * operand_size); /* Outputs: x3, y3 */
    if (sdk::asic::asic_mem_write(olist_dma_descr_addr,
                                  (uint8_t*)&olist_dma_descr,
                                  sizeof(olist_dma_descr))) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME " Failed to write olist DMA Descr @ 0x%lx",
                      (uint64_t) olist_dma_descr_addr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }

    /* Setup Asymmetric Request Descriptor */
    asym_req_descr.input_list_addr = ilist_dma_descr_addr1;
    asym_req_descr.output_list_addr = olist_dma_descr_addr;
    asym_req_descr.key_descr_idx = ecc_pm_key_idx;
    asym_req_descr.status_addr = curr_ptr;
    asym_req_descr.opaque_tag_value = 0;
    asym_req_descr.opage_tag_wr_en = 0;
    asym_req_descr.flag_a = 0;
    asym_req_descr.flag_b = 0;

    ret = elba_barco_ring_queue_request(BARCO_RING_ASYM,
                                         (void *)&asym_req_descr, &req_tag,
                                         true);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME " Failed to enqueue request");
        ret = SDK_RET_ERR;
        goto cleanup;
    }

    /* Poll for completion */
    while (elba_barco_ring_poll(BARCO_RING_ASYM, req_tag) != TRUE) {
        //SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME " Waiting for Barco completion...");
    }

    if (sdk::asic::asic_mem_read(asym_req_descr.status_addr,
                                 (uint8_t*)&status, sizeof(status))) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to retrieve operation status @ 0x%lx",
                      (uint64_t) asym_req_descr.status_addr);
        ret = SDK_RET_ERR;
        goto cleanup;
    }
    if (status != 0) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Operation failed with status 0x%lx",
                      status);
        ret = SDK_RET_ERR;
        goto cleanup;
    }

    /* Copy out the results */
    if (sdk::asic::asic_mem_read(olist_mem_addr + pad_size,
                                 (uint8_t*)x3, key_size)) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME " Failed to read x3 output from memory @ 0x%lx",
                      (uint64_t) olist_mem_addr + pad_size);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    if (sdk::asic::asic_mem_read(olist_mem_addr + operand_size + pad_size,
                                 (uint8_t*)y3, key_size)) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME " Failed to read y3 output from memory @ 0x%lx",
                      (uint64_t) (olist_mem_addr + operand_size + pad_size));
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }

    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"x3", (char *)x3, key_size);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"y3", (char *)y3, key_size);

cleanup:
    if (olist_mem_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_HBM_MEM_512B,
                                   olist_mem_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME " Failed to free memory for olist content:0x%lx",
                    olist_mem_addr);
        }
    }

    if (ilist_mem_addr2) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_HBM_MEM_512B,
                                   ilist_mem_addr2);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME " Failed to free memory for ilist2 content:0x%lx",
                    ilist_mem_addr2);
        }
    }

    if (ilist_mem_addr1) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_HBM_MEM_512B,
                                   ilist_mem_addr1);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME " Failed to free memory for ilist1 content:0x%lx",
                          ilist_mem_addr1);
        }
    }

    if (olist_dma_descr_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                                   olist_dma_descr_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME " Failed to free memory for olist DMA Descr: 0x%lx",
                          olist_dma_descr_addr);
        }
    }

    if (ilist_dma_descr_addr2) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                                   ilist_dma_descr_addr2);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME " Failed to free memory for ilist DMA Descr2: 0x%lx",
                          ilist_dma_descr_addr2);
        }
    }

    if (ilist_dma_descr_addr1) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                                   ilist_dma_descr_addr1);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME " Failed to free memory for ilist DMA Descr1: 0x%lx",
                          ilist_dma_descr_addr1);
        }
    }

    if (ecc_pm_key_idx != -1) {
        ret = elba_barco_asym_free_key(ecc_pm_key_idx);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME " Failed to free key descriptor");
        }
    }

    return ret;
}

sdk_ret_t
elba_barco_asym_ecdsa_p256_setup_priv_key (uint8_t *p, uint8_t *n,
                                           uint8_t *xg, uint8_t *yg,
                                           uint8_t *a, uint8_t *b,
                                           uint8_t *da, int32_t *key_idx)
{
    sdk_ret_t                   ret = SDK_RET_OK;
    uint64_t                    key_dma_descr_addr = 0;
    uint64_t                    curr_ptr = 0;
    uint64_t                    key_param_addr = 0;
    barco_asym_dma_descriptor_t key_dma_descr;
    elba_barco_asym_key_desc_t  asym_key;

#undef ELBA_BARCO_API_NAME
#define ELBA_BARCO_API_NAME "ECDSA Setup Key: "

    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"p", (char *)p, 32);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"a", (char *)a, 32);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"b", (char *)b, 32);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"n", (char *)n, 32);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"xg", (char *)xg, 32);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"yg", (char *)yg, 32);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"da", (char *)da, 32);

    *key_idx = -1;

    /* Setup params in the key memory */
    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_HBM_MEM_512B,
                               NULL, &key_param_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for key param");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for key param @ 0x%lx",
                    key_param_addr);

    curr_ptr = key_param_addr;

    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)p, 32)) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write ECC param p into key memory @ 0x%lx",
                      (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    curr_ptr += 32;

    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)n, 32)) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write ECC param n into key memory @ 0x%lx",
                      (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    curr_ptr += 32;

    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)xg, 32)) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write ECC param xg into key memory @ 0x%lx",
                      (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    curr_ptr += 32;

    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)yg, 32)) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write ECC param yg into key memory @ 0x%lx",
                      (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    curr_ptr += 32;

    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)a, 32)) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write ECC param a into key memory @ 0x%lx",
                      (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    curr_ptr += 32;

    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)b, 32)) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write ECC param b into key memory @ 0x%lx",
                      (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    curr_ptr += 32;

    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)da, 32)) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write ECC param da into key memory @ 0x%lx",
                      (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    curr_ptr += 32;

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                               NULL, &key_dma_descr_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for key DMA Descr");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for ilist DMA Descr @ 0x%lx", key_dma_descr_addr);

    /* Setup key DMA descriptor */
    key_dma_descr.address = key_param_addr;
    key_dma_descr.stop = 1;
    key_dma_descr.rsvd0 = 1;
    key_dma_descr.next = 0;
    key_dma_descr.int_en = 0;
    key_dma_descr.discard = 0;
    key_dma_descr.realign = 1;
    key_dma_descr.cst_addr = 0;
    key_dma_descr.length = (curr_ptr - key_param_addr);
    if (sdk::asic::asic_mem_write(key_dma_descr_addr, (uint8_t*) &key_dma_descr,
                                  sizeof(key_dma_descr))) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write key DMA Descr @ 0x%lx",
                (uint64_t) key_dma_descr_addr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }

    ret = elba_barco_asym_alloc_key(key_idx);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate key descriptor");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated Key Descr @ %d", *key_idx);

    asym_key.key_param_list = key_dma_descr_addr;
    asym_key.command_reg = (ELBA_BARCO_ASYM_CMD_SWAP_BYTES |
                            ELBA_BARCO_ASYM_CMD_SIZE_OF_OP(32) |
                            ELBA_BARCO_ASYM_ECDSA_SIG_GEN);

    ret = elba_barco_asym_write_key(*key_idx, &asym_key);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write key: %d", *key_idx);
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Setup key @ %d", *key_idx);

    return ret;

cleanup:
    if (*key_idx != -1) {
        ret = elba_barco_asym_free_key(*key_idx);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME"Failed to free key descriptor");
        }
    }

    if (key_dma_descr_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                                  key_dma_descr_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for key DMA Descr: 0x%lx",
                          key_dma_descr_addr);
        }
    }

    if (key_param_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_HBM_MEM_512B, key_param_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for key param :0x%lx",
                          key_param_addr);
        }
    }
    return ret;
}

sdk_ret_t
elba_barco_asym_ecdsa_p256_sig_gen (int32_t key_idx, uint8_t *p, uint8_t *n,
                                    uint8_t *xg, uint8_t *yg,
                                    uint8_t *a, uint8_t *b, uint8_t *da,
                                    uint8_t *k, uint8_t *h, uint8_t *r,
                                    uint8_t *s,
                                    bool async_en, const uint8_t *unique_key)
{
    sdk_ret_t                   ret = SDK_RET_OK;
    uint64_t                    ilist_dma_descr_addr = 0,
                                olist_dma_descr_addr = 0;
    uint64_t                    ilist_mem_addr = 0,
                                olist_mem_addr = 0, curr_ptr = 0;
    barco_asym_descriptor_t     asym_req_descr;
    barco_asym_dma_descriptor_t ilist_dma_descr, olist_dma_descr;
    int32_t                     ecc_p256_key_idx = -1;
    uint32_t                    req_tag = 0;
    uint32_t                    status = 0;

#undef ELBA_BARCO_API_NAME
#define ELBA_BARCO_API_NAME "ECDSA Sig Gen: "

    SDK_TRACE_DEBUG("Key_idx: %d", key_idx);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"k", (char *)k, 32);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"h", (char *)h, 32);

    if(key_idx < 0) {
        ret = elba_barco_asym_ecdsa_p256_setup_priv_key(p, n, xg, yg,
                                                        a, b, da,
                                                        &ecc_p256_key_idx);
        if(ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to setup private key");
            goto cleanup;
        }
    } else {
        ecc_p256_key_idx = key_idx;
    }

    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Key @ %d", ecc_p256_key_idx);

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                               NULL, &ilist_dma_descr_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for ilist DMA Descr");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for ilist DMA Descr @ 0x%lx", ilist_dma_descr_addr);

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                               NULL, &olist_dma_descr_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for olist DMA Descr");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for olist DMA Descr @ 0x%lx",
                    olist_dma_descr_addr);

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_HBM_MEM_512B,
                               NULL, &ilist_mem_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for ilist content");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for input mem @ 0x%lx", ilist_mem_addr);

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_HBM_MEM_512B,
                               NULL, &olist_mem_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for olist content");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for output mem @ 0x%lx", olist_mem_addr);

    /* Copy the input to the ilist memory */
    curr_ptr = ilist_mem_addr;

    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)k, 32)) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write ECC param k into ilist memory @ 0x%lx", (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    curr_ptr += 32;

    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)h, 32)) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write ECC param h into ilist memory @ 0x%lx", (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    curr_ptr += 32;

    /* Setup ilist DMA descriptor */
    ilist_dma_descr.address = ilist_mem_addr;
    ilist_dma_descr.stop = 1;
    ilist_dma_descr.rsvd0 = 1;
    ilist_dma_descr.next = 0;
    ilist_dma_descr.int_en = 0;
    ilist_dma_descr.discard = 0;
    ilist_dma_descr.realign = 1;
    ilist_dma_descr.cst_addr = 0;
    ilist_dma_descr.length = (curr_ptr - ilist_mem_addr);
    if (sdk::asic::asic_mem_write(ilist_dma_descr_addr,
                                  (uint8_t*) &ilist_dma_descr,
                                  sizeof(ilist_dma_descr))) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write ilist DMA Descr @ 0x%lx",
                      (uint64_t) ilist_dma_descr_addr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }

    /* Setup olist DMA descriptor */
    olist_dma_descr.address = olist_mem_addr;
    olist_dma_descr.stop = 1;
    olist_dma_descr.rsvd0 = 1;
    olist_dma_descr.next = 0;
    olist_dma_descr.int_en = 0;
    olist_dma_descr.discard = 0;
    olist_dma_descr.realign = 1;
    olist_dma_descr.cst_addr = 0;
    olist_dma_descr.length = (2 * 32 ); /* Outputs: r, s */
    if (sdk::asic::asic_mem_write(olist_dma_descr_addr,
                                  (uint8_t*) &olist_dma_descr,
                                  sizeof(olist_dma_descr))) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write olist DMA Descr @ 0x%lx",
                (uint64_t) olist_dma_descr_addr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }

    /* Setup Asymmetric Request Descriptor */
    asym_req_descr.input_list_addr = ilist_dma_descr_addr;
    asym_req_descr.output_list_addr = olist_dma_descr_addr;
    asym_req_descr.key_descr_idx = ecc_p256_key_idx;
    asym_req_descr.status_addr = curr_ptr;
    asym_req_descr.opaque_tag_value = 0;
    asym_req_descr.opage_tag_wr_en = 0;
    asym_req_descr.flag_a = 0;
    asym_req_descr.flag_b = 0;

    ret = elba_barco_ring_queue_request(BARCO_RING_ASYM,
                                        (void *) &asym_req_descr,
                                        &req_tag, true);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to enqueue request");
        ret = SDK_RET_ERR;
        goto cleanup;
    }

    // Wait for operation to be completed
    elba_barco_wait_for_resp(req_tag, async_en, unique_key);

    if (sdk::asic::asic_mem_read(asym_req_descr.status_addr,
                                 (uint8_t*) &status, sizeof(status))) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to retrieve operation status @ 0x%lx",
                (uint64_t) asym_req_descr.status_addr);
        ret = SDK_RET_ERR;
        goto cleanup;
    }
    if (status != 0) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Operation failed with status %u",
                status);
        ret = SDK_RET_ERR;
        goto cleanup;
    }

    /* Copy out the results */
    if (sdk::asic::asic_mem_read(olist_mem_addr, (uint8_t*)r, 32)) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to read r output from memory @ 0x%lx",
                      (uint64_t) olist_mem_addr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    if (sdk::asic::asic_mem_read(olist_mem_addr + 32, (uint8_t*)s, 32)) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to read s output from memory @ 0x%lx",
                      (uint64_t) (olist_mem_addr + 32));
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }

    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"r", (char *)r, 32);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"s", (char *)s, 32);


cleanup:
    if (olist_mem_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_HBM_MEM_512B,
                                  olist_mem_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for olist content:0x%lx",
                    olist_mem_addr);
        }
    }

    if (ilist_mem_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_HBM_MEM_512B,
                                  ilist_mem_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR("ECC Point Mul P256: Failed to free memory for ilist content:0x%lx",
                    ilist_mem_addr);
        }
    }

    if (olist_dma_descr_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                                  olist_dma_descr_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for olist DMA Descr: 0x%lx",
                    olist_dma_descr_addr);
        }
    }

    if (ilist_dma_descr_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                                  ilist_dma_descr_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for ilist DMA Descr: 0x%lx",
                    ilist_dma_descr_addr);
        }
    }
    if(status)
        ret = SDK_RET_ERR;
    return ret;
}

sdk_ret_t
elba_barco_asym_ecdsa_p256_sig_verify (uint8_t *p, uint8_t *n,
                                       uint8_t *xg, uint8_t *yg,
                                       uint8_t *a, uint8_t *b, uint8_t *xq,
                                       uint8_t *yq, uint8_t *r, uint8_t *s,
                                       uint8_t *h, bool async_en,
                                       const uint8_t *unique_key)
{
    sdk_ret_t                   ret = SDK_RET_OK;
    uint64_t                    ilist_dma_descr_addr = 0;
    uint64_t                    key_dma_descr_addr = 0;
    uint64_t                    ilist_mem_addr = 0,
                                olist_mem_addr = 0, curr_ptr = 0;
    uint64_t                    key_param_addr = 0;
    barco_asym_descriptor_t     asym_req_descr;
    barco_asym_dma_descriptor_t ilist_dma_descr;
    barco_asym_dma_descriptor_t key_dma_descr;
    int32_t                     ecc_p256_key_idx = -1;
    elba_barco_asym_key_desc_t  asym_key;
    uint32_t                    req_tag = 0;
    uint32_t                    status = 0;

#undef ELBA_BARCO_API_NAME
#define ELBA_BARCO_API_NAME "ECDSA Sig Verify: "

    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"p", (char *)p, 32);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"a", (char *)a, 32);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"b", (char *)b, 32);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"n", (char *)n, 32);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"xg", (char *)xg, 32);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"yg", (char *)yg, 32);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"xq", (char *)xq, 32);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"yq", (char *)yq, 32);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"r", (char *)r, 32);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"s", (char *)s, 32);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"h", (char *)h, 32);

    /* Setup params in the key memory */
    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_HBM_MEM_512B,
                               NULL, &key_param_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for key param");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for key param @ 0x%lx", key_param_addr);

    curr_ptr = key_param_addr;

    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)p, 32)) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write ECC param p into key memory @ 0x%lx", (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    curr_ptr += 32;

    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)n, 32)) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write ECC param n into key memory @ 0x%lx", (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    curr_ptr += 32;

    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)xg, 32)) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write ECC param xg into key memory @ 0x%lx", (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    curr_ptr += 32;

    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)yg, 32)) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write ECC param yg into key memory @ 0x%lx", (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    curr_ptr += 32;

    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)a, 32)) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write ECC param a into key memory @ 0x%lx", (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    curr_ptr += 32;

    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)b, 32)) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write ECC param b into key memory @ 0x%lx", (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    curr_ptr += 32;

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                               NULL, &key_dma_descr_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for key DMA Descr");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for key DMA Descr @ 0x%lx", key_dma_descr_addr);

    /* Setup key DMA descriptor */
    key_dma_descr.address = key_param_addr;
    key_dma_descr.stop = 1;
    key_dma_descr.rsvd0 = 1;
    key_dma_descr.next = 0;
    key_dma_descr.int_en = 0;
    key_dma_descr.discard = 0;
    key_dma_descr.realign = 1;
    key_dma_descr.cst_addr = 0;
    key_dma_descr.length = (curr_ptr - key_param_addr);
    if (sdk::asic::asic_mem_write(key_dma_descr_addr, (uint8_t*)&key_dma_descr,
                sizeof(key_dma_descr))) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write key DMA Descr @ 0x%lx",
                (uint64_t) key_dma_descr_addr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }

    ret = elba_barco_asym_alloc_key(&ecc_p256_key_idx);

    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate key descriptor");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated Key Descr @ %d",
                    ecc_p256_key_idx);

    asym_key.key_param_list = key_dma_descr_addr;
    asym_key.command_reg = (ELBA_BARCO_ASYM_CMD_SWAP_BYTES |
                            ELBA_BARCO_ASYM_CMD_SIZE_OF_OP(32) |
                            ELBA_BARCO_ASYM_ECDSA_SIG_VERIFY);

    ret = elba_barco_asym_write_key(ecc_p256_key_idx, &asym_key);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write key: %d",
                      ecc_p256_key_idx);
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Setup key @ %d", ecc_p256_key_idx);

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                               NULL, &ilist_dma_descr_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for ilist DMA Descr");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for ilist DMA Descr @ 0x%lx", ilist_dma_descr_addr);

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_HBM_MEM_512B,
                               NULL, &ilist_mem_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for ilist content");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for input mem @ 0x%lx", ilist_mem_addr);

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_HBM_MEM_512B,
                               NULL, &olist_mem_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for olist content");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for output mem @ 0x%lx", olist_mem_addr);

    /* Copy the input to the ilist memory */
    curr_ptr = ilist_mem_addr;

    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)xq, 32)) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write ECC param xq into key memory @ 0x%lx", (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    curr_ptr += 32;

    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)yq, 32)) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write ECC param yq into key memory @ 0x%lx", (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    curr_ptr += 32;

    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)r, 32)) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write ECC param r into ilist memory @ 0x%lx", (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    curr_ptr += 32;

    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)s, 32)) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write ECC param s into ilist memory @ 0x%lx", (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    curr_ptr += 32;

    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)h, 32)) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write ECC param h into ilist memory @ 0x%lx", (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    curr_ptr += 32;

    /* Setup ilist DMA descriptor */
    ilist_dma_descr.address = ilist_mem_addr;
    ilist_dma_descr.stop = 1;
    ilist_dma_descr.rsvd0 = 1;
    ilist_dma_descr.next = 0;
    ilist_dma_descr.int_en = 0;
    ilist_dma_descr.discard = 0;
    ilist_dma_descr.realign = 1;
    ilist_dma_descr.cst_addr = 0;
    ilist_dma_descr.length = (curr_ptr - ilist_mem_addr);
    if (sdk::asic::asic_mem_write(ilist_dma_descr_addr,
                                  (uint8_t*)&ilist_dma_descr,
                                  sizeof(ilist_dma_descr))) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write ilist DMA Descr @ 0x%lx",
                (uint64_t) ilist_dma_descr_addr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }

    /* Setup Asymmetric Request Descriptor */
    asym_req_descr.input_list_addr = ilist_dma_descr_addr;
    asym_req_descr.output_list_addr = 0;
    asym_req_descr.key_descr_idx = ecc_p256_key_idx;
    asym_req_descr.status_addr = curr_ptr;
    asym_req_descr.opaque_tag_value = 0;
    asym_req_descr.opage_tag_wr_en = 0;
    asym_req_descr.flag_a = 0;
    asym_req_descr.flag_b = 0;

    ret = elba_barco_ring_queue_request(BARCO_RING_ASYM,
                                        (void *)&asym_req_descr,
                                        &req_tag, true);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to enqueue request");
        ret = SDK_RET_ERR;
        goto cleanup;
    }

    // Wait for operation to be completed
    elba_barco_wait_for_resp(req_tag, async_en, unique_key);

    if (sdk::asic::asic_mem_read(asym_req_descr.status_addr,
                                 (uint8_t*)&status, sizeof(status))) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to retrieve operation status @ 0x%lx",
                (uint64_t) asym_req_descr.status_addr);
        ret = SDK_RET_ERR;
        goto cleanup;
    }
    if (status != 0) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Operation failed with status %u",
                status);
        ret = SDK_RET_ERR;
        goto cleanup;
    }
    else {
        SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Signature Verification success");
    }

    /* No output */

cleanup:
    if (olist_mem_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_HBM_MEM_512B,
                                  olist_mem_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for olist content:0x%lx",
                    olist_mem_addr);
        }
    }

    if (ilist_mem_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_HBM_MEM_512B,
                                  ilist_mem_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR("ECC Point Mul P256: Failed to free memory for ilist content:0x%lx",
                    ilist_mem_addr);
        }
    }

    if (ilist_dma_descr_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                                  ilist_dma_descr_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for ilist DMA Descr: 0x%lx",
                          ilist_dma_descr_addr);
        }
    }

    if (ecc_p256_key_idx != -1) {
        ret = elba_barco_asym_free_key(ecc_p256_key_idx);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME"Failed to free key descriptor");
        }
    }

    if (key_dma_descr_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                                  key_dma_descr_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for key DMA Descr: 0x%lx",
                          key_dma_descr_addr);
        }
    }

    if (key_param_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_HBM_MEM_512B,
                                  key_param_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for key param :0x%lx",
                          key_param_addr);
        }
    }

    if (status)
        ret = SDK_RET_ERR;

    return ret;
}

sdk_ret_t
elba_barco_asym_rsa2k_setup_sig_gen_priv_key (uint8_t *n, uint8_t *d,
                                              int32_t *key_idx)
{
    sdk_ret_t                   ret = SDK_RET_OK;
    uint64_t                    key_dma_descr_addr = 0;
    uint64_t                    curr_ptr = 0;
    uint64_t                    key_param_addr = 0;
    barco_asym_dma_descriptor_t key_dma_descr;
    elba_barco_asym_key_desc_t  asym_key;

#undef ELBA_BARCO_API_NAME
#define ELBA_BARCO_API_NAME "RSA 2K Key setup: "

    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"n", (char *)n, 256);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"d", (char *)d, 256);

    *key_idx = -1;

    /* Setup params in the key memory */
    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_HBM_MEM_512B,
                               NULL, &key_param_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for key param");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for key param @ 0x%lx", key_param_addr);

    curr_ptr = key_param_addr;

    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)n, 256)) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write RSA param n into key memory @ 0x%lx", (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    curr_ptr += 256;

    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)d, 256)) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write RSA param d into key memory @ 0x%lx", (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    curr_ptr += 256;

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                               NULL, &key_dma_descr_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for key DMA Descr");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for key DMA Descr @ 0x%lx", key_dma_descr_addr);

    /* Setup key DMA descriptor */
    key_dma_descr.address = key_param_addr;
    key_dma_descr.stop = 1;
    key_dma_descr.rsvd0 = 1;
    key_dma_descr.next = 0;
    key_dma_descr.int_en = 0;
    key_dma_descr.discard = 0;
    key_dma_descr.realign = 1;
    key_dma_descr.cst_addr = 0;
    key_dma_descr.length = (curr_ptr - key_param_addr);
    if (sdk::asic::asic_mem_write(key_dma_descr_addr, (uint8_t*)&key_dma_descr,
                sizeof(key_dma_descr))) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write key DMA Descr @ 0x%lx",
                (uint64_t) key_dma_descr_addr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }

    ret = elba_barco_asym_alloc_key(key_idx);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate key descriptor");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated Key Descr @ %d", *key_idx);

    asym_key.key_param_list = key_dma_descr_addr;
    asym_key.command_reg = (ELBA_BARCO_ASYM_CMD_SWAP_BYTES |
                            ELBA_BARCO_ASYM_CMD_SIZE_OF_OP(256) |
                            ELBA_BARCO_ASYM_CMD_RSA_SIG_GEN);

    ret = elba_barco_asym_write_key(*key_idx, &asym_key);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write key: %d", *key_idx);
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Setup key @ %d", *key_idx);

    return ret;

cleanup:
    if (*key_idx != -1) {
        ret = elba_barco_asym_free_key(*key_idx);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME"Failed to free key descriptor");
        }
    }
    if (key_dma_descr_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                                  key_dma_descr_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for key DMA Descr: 0x%lx",
                    key_dma_descr_addr);
        }
    }

    if (key_param_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_HBM_MEM_512B, key_param_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for key param :0x%lx",
                    key_param_addr);
        }
    }
    return ret;
}

sdk_ret_t
elba_barco_asym_rsa_setup_priv_key (uint16_t key_size, uint8_t *n, uint8_t *d,
                                    int32_t* key_idx)
{
    sdk_ret_t                   ret = SDK_RET_OK;
    uint64_t                    key_dma_descr_addr1 = 0,
                                key_dma_descr_addr2 = 0;
    uint64_t                    curr_ptr = 0;
    uint64_t                    key_param_addr1 = 0, key_param_addr2 = 0;
    barco_asym_dma_descriptor_t key_dma_descr;
    elba_barco_asym_key_desc_t asym_key;

#undef ELBA_BARCO_API_NAME
#define ELBA_BARCO_API_NAME "RSA Key setup: "

    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"n", (char *)n, key_size);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"d", (char *)d, key_size);

    *key_idx = -1;

    /* Setup params in the key memory */
    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_HBM_MEM_512B,
                               NULL, &key_param_addr1);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for key param1");
        goto cleanup;
    }

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_HBM_MEM_512B,
            NULL, &key_param_addr2);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for key param2");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for key param @ 0x%lx, 0x%lx", key_param_addr1, key_param_addr2);

    curr_ptr = key_param_addr1;

    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)n, key_size)) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write RSA param n into key memory @ 0x%lx", (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }

    curr_ptr = key_param_addr2;
    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)d, key_size)) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write RSA param d into key memory @ 0x%lx", (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                               NULL, &key_dma_descr_addr1);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for key DMA Descr1");
        goto cleanup;
    }

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
            NULL, &key_dma_descr_addr2);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for key DMA Descr2");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for key DMA Descr @ 0x%lx, 0x%lx", key_dma_descr_addr1, key_dma_descr_addr2);

    /* Setup key DMA descriptor 1 */
    key_dma_descr.address = key_param_addr1;
    key_dma_descr.stop = 0;
    key_dma_descr.rsvd0 = 1;
    key_dma_descr.next = (key_dma_descr_addr2 >> 2);
    key_dma_descr.int_en = 0;
    key_dma_descr.discard = 0;
    key_dma_descr.realign = 0;
    key_dma_descr.cst_addr = 0;

    key_dma_descr.length = (key_size);
    if (sdk::asic::asic_mem_write(key_dma_descr_addr1,
                                  (uint8_t*)&key_dma_descr,
                                  sizeof(key_dma_descr))) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write key DMA Descr1 @ 0x%lx",
                      (uint64_t) key_dma_descr_addr1);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }

    /* Setup key DMA descriptor 2 */
    key_dma_descr.address = key_param_addr2;
    key_dma_descr.stop = 1;
    key_dma_descr.rsvd0 = 1;
    key_dma_descr.next = 0;
    key_dma_descr.int_en = 0;
    key_dma_descr.discard = 0;
    key_dma_descr.realign = 1;
    key_dma_descr.cst_addr = 0;
    key_dma_descr.length = (key_size);
    if (sdk::asic::asic_mem_write(key_dma_descr_addr2,
                                  (uint8_t*)&key_dma_descr,
                                  sizeof(key_dma_descr))) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write key DMA Descr @ 0x%lx",
                      (uint64_t) key_dma_descr_addr2);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }

    ret = elba_barco_asym_alloc_key(key_idx);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate key descriptor");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated Key Descr @ %d", *key_idx);

    asym_key.key_param_list = key_dma_descr_addr1;
    asym_key.command_reg = (ELBA_BARCO_ASYM_CMD_SWAP_BYTES |
                            ELBA_BARCO_ASYM_CMD_SIZE_OF_OP(key_size) |
                            ELBA_BARCO_ASYM_CMD_RSA_SIG_GEN);

    ret = elba_barco_asym_write_key(*key_idx, &asym_key);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write key: %d", *key_idx);
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Setup key @ %d", *key_idx);

    return ret;

cleanup:
    if (*key_idx != -1) {
        ret = elba_barco_asym_free_key(*key_idx);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME"Failed to free key descriptor");
        }
    }
    if (key_dma_descr_addr2) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_ASYM_DMA_DESCR, key_dma_descr_addr2);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for key DMA Descr: 0x%lx",
                          key_dma_descr_addr2);
        }
    }
    if (key_dma_descr_addr1) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_ASYM_DMA_DESCR, key_dma_descr_addr1);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for key DMA Descr: 0x%lx",
                          key_dma_descr_addr1);
        }
    }

    if (key_param_addr2) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_HBM_MEM_512B, key_param_addr2);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for key param :0x%lx",
                          key_param_addr2);
        }
    }
    if (key_param_addr1) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_HBM_MEM_512B, key_param_addr2);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for key param :0x%lx",
                          key_param_addr1);
        }
    }

    return ret;
}

sdk_ret_t
elba_barco_asym_rsa2k_encrypt (uint8_t *n, uint8_t *e,
                               uint8_t *m,  uint8_t *c, bool async_en,
                               const uint8_t *unique_key)
{
    sdk_ret_t                   ret = SDK_RET_OK;
    uint64_t                    ilist_dma_descr_addr = 0,
                                olist_dma_descr_addr = 0;
    uint64_t                    key_dma_descr_addr = 0;
    uint64_t                    ilist_mem_addr = 0,
                                olist_mem_addr = 0, curr_ptr = 0;
    uint64_t                    key_param_addr = 0;
    barco_asym_descriptor_t     asym_req_descr;
    barco_asym_dma_descriptor_t ilist_dma_descr, olist_dma_descr;
    barco_asym_dma_descriptor_t key_dma_descr;
    int32_t                     ecc_p256_key_idx = -1;
    elba_barco_asym_key_desc_t  asym_key;
    uint32_t                    req_tag = 0;
    uint32_t                    status = 0;

#undef ELBA_BARCO_API_NAME
#define ELBA_BARCO_API_NAME "RSA 2K Encrypt: "

    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"n", (char *)n, 256);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"e", (char *)e, 256);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"m", (char *)m, 256);
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "async: {}", async_en);

    /* Setup params in the key memory */
    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_HBM_MEM_512B,
            NULL, &key_param_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for key param");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for key param @ {:x}", key_param_addr);

    curr_ptr = key_param_addr;

    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)n, 256)) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write RSA param n into key memory @ {:x}", (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    curr_ptr += 256;

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
            NULL, &key_dma_descr_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for key DMA Descr");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for key DMA Descr @ {:x}", key_dma_descr_addr);

    /* Setup key DMA descriptor */
    key_dma_descr.address = key_param_addr;
    key_dma_descr.stop = 1;
    key_dma_descr.rsvd0 = 1;
    key_dma_descr.next = 0;
    key_dma_descr.int_en = 0;
    key_dma_descr.discard = 0;
    key_dma_descr.realign = 1;
    key_dma_descr.cst_addr = 0;
    key_dma_descr.length = (curr_ptr - key_param_addr);
    if (sdk::asic::asic_mem_write(key_dma_descr_addr, (uint8_t*)&key_dma_descr,
                sizeof(key_dma_descr))) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write key DMA Descr @ {:x}",
                (uint64_t) key_dma_descr_addr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }

    ret = elba_barco_asym_alloc_key(&ecc_p256_key_idx);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate key descriptor");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated Key Descr @ %d",
                    ecc_p256_key_idx);

    asym_key.key_param_list = key_dma_descr_addr;
    asym_key.command_reg = (ELBA_BARCO_ASYM_CMD_SWAP_BYTES |
                            ELBA_BARCO_ASYM_CMD_SIZE_OF_OP(256) |
                            ELBA_BARCO_ASYM_CMD_RSA_ENCRYPT);

    ret = elba_barco_asym_write_key(ecc_p256_key_idx, &asym_key);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write key: %d",
                      ecc_p256_key_idx);
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Setup key @ {:x}", ecc_p256_key_idx);


    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                               NULL, &ilist_dma_descr_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for ilist DMA Descr");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for ilist DMA Descr @ {:x}", ilist_dma_descr_addr);

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
            NULL, &olist_dma_descr_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for olist DMA Descr");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for olist DMA Descr @ {:x}", olist_dma_descr_addr);

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_HBM_MEM_512B,
            NULL, &ilist_mem_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for ilist content");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for input mem @ {:x}", ilist_mem_addr);

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_HBM_MEM_512B,
            NULL, &olist_mem_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for olist content");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for output mem @ {:x}", olist_mem_addr);

    /* Copy the input to the ilist memory */
    curr_ptr = ilist_mem_addr;

    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)m, 256)) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write RSA param m into ilist memory @ {:x}", (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    curr_ptr += 256;

    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)e, 256)) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write RSA param e into key memory @ {:x}", (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    curr_ptr += 256;


    /* Setup ilist DMA descriptor */
    ilist_dma_descr.address = ilist_mem_addr;
    ilist_dma_descr.stop = 1;
    ilist_dma_descr.rsvd0 = 1;
    ilist_dma_descr.next = 0;
    ilist_dma_descr.int_en = 0;
    ilist_dma_descr.discard = 0;
    ilist_dma_descr.realign = 1;
    ilist_dma_descr.cst_addr = 0;
    ilist_dma_descr.length = (curr_ptr - ilist_mem_addr);
    if (sdk::asic::asic_mem_write(ilist_dma_descr_addr,
                                  (uint8_t*)&ilist_dma_descr,
                                  sizeof(ilist_dma_descr))) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write ilist DMA Descr @ {:x}",
                (uint64_t) ilist_dma_descr_addr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }

    /* Setup olist DMA descriptor */
    olist_dma_descr.address = olist_mem_addr;
    olist_dma_descr.stop = 1;
    olist_dma_descr.rsvd0 = 1;
    olist_dma_descr.next = 0;
    olist_dma_descr.int_en = 0;
    olist_dma_descr.discard = 0;
    olist_dma_descr.realign = 1;
    olist_dma_descr.cst_addr = 0;
    olist_dma_descr.length = (1 * 256); /* cipher text */
    if (sdk::asic::asic_mem_write(olist_dma_descr_addr,
                                  (uint8_t*)&olist_dma_descr,
                                  sizeof(olist_dma_descr))) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write olist DMA Descr @ {:x}",
                (uint64_t) olist_dma_descr_addr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }

    /* Setup Asymmetric Request Descriptor */
    asym_req_descr.input_list_addr = ilist_dma_descr_addr;
    asym_req_descr.output_list_addr = olist_dma_descr_addr;
    asym_req_descr.key_descr_idx = ecc_p256_key_idx;
    asym_req_descr.status_addr = olist_mem_addr + 256;
    asym_req_descr.opaque_tag_value = 0;
    asym_req_descr.opage_tag_wr_en = 0;
    asym_req_descr.flag_a = 0;
    asym_req_descr.flag_b = 0;

    ret = elba_barco_ring_queue_request(BARCO_RING_ASYM,
                                        (void *)&asym_req_descr,
                                        &req_tag, true);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to enqueue request");
        ret = SDK_RET_ERR;
        goto cleanup;
    }

    // Wait for operation to be completed
    elba_barco_wait_for_resp(req_tag, async_en, unique_key);

    if (sdk::asic::asic_mem_read(asym_req_descr.status_addr,
                                 (uint8_t*)&status, sizeof(status))) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to retrieve operation status @ {:x}",
                (uint64_t) asym_req_descr.status_addr);
        ret = SDK_RET_ERR;
        goto cleanup;
    }
    if (status != 0) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Operation failed with status {:x}",
                status);
        ret = SDK_RET_ERR;
        goto cleanup;
    }
    else {
        /* Copy out the results */
        if (sdk::asic::asic_mem_read(olist_mem_addr, (uint8_t*)c, 256)) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to read output c from memory @ {:x}",
                          (uint64_t) olist_mem_addr);
            ret = SDK_RET_INVALID_ARG;
            goto cleanup;
        }
        ELBA_BARCO_API_PARAM_HEXDUMP((char *)"c", (char *)c, 256);
        SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "RSA Encrypt succeeded");
    }

cleanup:
    if (olist_mem_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_HBM_MEM_512B,
                                  olist_mem_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for olist content:{:x}",
                          olist_mem_addr);
        }
    }

    if (ilist_mem_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_HBM_MEM_512B,
                                  ilist_mem_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR("ECC Point Mul P256: Failed to free memory for ilist content:{:x}",
                    ilist_mem_addr);
        }
    }

    if (olist_dma_descr_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                                  olist_dma_descr_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for olist DMA Descr: {:x}",
                    olist_dma_descr_addr);
        }
    }

    if (ilist_dma_descr_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                                  ilist_dma_descr_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for ilist DMA Descr: {:x}",
                          ilist_dma_descr_addr);
        }
    }

    if (ecc_p256_key_idx != -1) {
        ret = elba_barco_asym_free_key(ecc_p256_key_idx);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME"Failed to free key descriptor");
        }
    }

    if (key_dma_descr_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                                  key_dma_descr_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for key DMA Descr: {:x}",
                          key_dma_descr_addr);
        }
    }

    if (key_param_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_HBM_MEM_512B,
                                  key_param_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for key param :{:x}",
                          key_param_addr);
        }
    }

    if (status)
        ret = SDK_RET_ERR;

    return ret;
}

sdk_ret_t
elba_barco_asym_rsa_encrypt (uint16_t key_size, uint8_t *n, uint8_t *e,
                              uint8_t *m,  uint8_t *c,
                              bool async_en, const uint8_t *unique_key)
{
    sdk_ret_t                   ret = SDK_RET_OK;
    uint64_t                    ilist_dma_descr_addr = 0, olist_dma_descr_addr = 0;
    uint64_t                    key_dma_descr_addr1 = 0, key_dma_descr_addr2 = 0;
    uint64_t                    ilist_mem_addr = 0, olist_mem_addr = 0, status_mem_addr = 0, curr_ptr = 0;
    uint64_t                    key_param_addr1 = 0, key_param_addr2 = 0;
    barco_asym_descriptor_t     asym_req_descr;
    barco_asym_dma_descriptor_t ilist_dma_descr, olist_dma_descr;
    barco_asym_dma_descriptor_t key_dma_descr;
    int32_t                     ecc_p256_key_idx = -1;
    elba_barco_asym_key_desc_t asym_key;
    uint32_t                    req_tag = 0;
    uint32_t                    status = 0;

#undef ELBA_BARCO_API_NAME
#define ELBA_BARCO_API_NAME "RSA Encrypt: "

    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"n", (char *)n, key_size);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"e", (char *)e, key_size);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"m", (char *)m, key_size);
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "async: %d", async_en);

    /* Setup params in the key memory */
    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_HBM_MEM_512B,
            NULL, &key_param_addr1);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for key param1");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for key param1 @ 0x%lx", key_param_addr1);
    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_HBM_MEM_512B,
            NULL, &key_param_addr2);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for key param2");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for key param2 @ 0x%lx", key_param_addr2);

    curr_ptr = key_param_addr1;

    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)n, key_size)) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write RSA param n into key memory @ 0x%lx", (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }

    curr_ptr = key_param_addr2;
    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)e, key_size)) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write RSA param e into key memory @ 0x%lx", (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
            NULL, &key_dma_descr_addr1);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for key DMA Descr1");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for key DMA Descr1 @ 0x%lx", key_dma_descr_addr1);

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
            NULL, &key_dma_descr_addr2);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for key DMA Descr2");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for key DMA Descr2 @ 0x%lx", key_dma_descr_addr2);

    /* Setup key DMA descriptor 1 */
    key_dma_descr.address = key_param_addr1;
    key_dma_descr.stop = 0;
    key_dma_descr.rsvd0 = 1;
    key_dma_descr.next = (key_dma_descr_addr2 >> 2);
    key_dma_descr.int_en = 0;
    key_dma_descr.discard = 0;
    key_dma_descr.realign = 0;
    key_dma_descr.cst_addr = 0;
    key_dma_descr.length = (key_size);
    if (sdk::asic::asic_mem_write(key_dma_descr_addr1, (uint8_t*)&key_dma_descr,
                sizeof(key_dma_descr))) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write key DMA Descr1 @ 0x%lx",
                (uint64_t) key_dma_descr_addr1);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }

    /* Setup key DMA descriptor 2 */
    key_dma_descr.address = key_param_addr2;
    key_dma_descr.stop = 1;
    key_dma_descr.rsvd0 = 1;
    key_dma_descr.next = 0;
    key_dma_descr.int_en = 0;
    key_dma_descr.discard = 0;
    key_dma_descr.realign = 1;
    key_dma_descr.cst_addr = 0;
    key_dma_descr.length = (key_size);
    if (sdk::asic::asic_mem_write(key_dma_descr_addr2, (uint8_t*)&key_dma_descr,
                sizeof(key_dma_descr))) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write key DMA Descr2 @ 0x%lx",
                (uint64_t) key_dma_descr_addr2);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }

    ret = elba_barco_asym_alloc_key(&ecc_p256_key_idx);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate key descriptor");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated Key Descr @ %d", ecc_p256_key_idx);

    asym_key.key_param_list = key_dma_descr_addr1;
    asym_key.command_reg = (ELBA_BARCO_ASYM_CMD_SWAP_BYTES |
                            ELBA_BARCO_ASYM_CMD_SIZE_OF_OP(key_size) |
                            ELBA_BARCO_ASYM_CMD_RSA_ENCRYPT);

    ret = elba_barco_asym_write_key(ecc_p256_key_idx, &asym_key);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write key: %d", ecc_p256_key_idx);
>>>>>>> 6b44818... Port relevant crypto apis from hal pd to asic pd.  Cleanup SDK_TRACE debug statements.
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Setup key @ %d", ecc_p256_key_idx);


    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
            NULL, &ilist_dma_descr_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for ilist DMA Descr");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for ilist DMA Descr @ 0x%lx", ilist_dma_descr_addr);

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                               NULL, &olist_dma_descr_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for olist DMA Descr");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for olist DMA Descr @ 0x%lx", olist_dma_descr_addr);

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_HBM_MEM_512B,
                               NULL, &ilist_mem_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for ilist content");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for input mem @ 0x%lx", ilist_mem_addr);

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_HBM_MEM_512B,
                               NULL, &olist_mem_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for olist content");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for output mem @ 0x%lx",
                    olist_mem_addr);

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_HBM_MEM_512B,
            NULL, &status_mem_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for status output");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for output mem @ 0x%lx",
                    status_mem_addr);

    /* Copy the input to the ilist memory */
    curr_ptr = ilist_mem_addr;

    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)m, key_size)) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write RSA param m into ilist memory @ 0x%lx",
                      (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }

    /* Setup ilist DMA descriptor */
    ilist_dma_descr.address = ilist_mem_addr;
    ilist_dma_descr.stop = 1;
    ilist_dma_descr.rsvd0 = 1;
    ilist_dma_descr.next = 0;
    ilist_dma_descr.int_en = 0;
    ilist_dma_descr.discard = 0;
    ilist_dma_descr.realign = 1;
    ilist_dma_descr.cst_addr = 0;
    ilist_dma_descr.length = (key_size);
    if (sdk::asic::asic_mem_write(ilist_dma_descr_addr,
                                  (uint8_t*)&ilist_dma_descr,
                                  sizeof(ilist_dma_descr))) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write ilist DMA Descr @ 0x%lx",
                      (uint64_t) ilist_dma_descr_addr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }

    /* Setup olist DMA descriptor */
    olist_dma_descr.address = olist_mem_addr;
    olist_dma_descr.stop = 1;
    olist_dma_descr.rsvd0 = 1;
    olist_dma_descr.next = 0;
    olist_dma_descr.int_en = 0;
    olist_dma_descr.discard = 0;
    olist_dma_descr.realign = 1;
    olist_dma_descr.cst_addr = 0;
    olist_dma_descr.length = (key_size); /* cipher text */

    if (sdk::asic::asic_mem_write(olist_dma_descr_addr,
                                  (uint8_t*)&olist_dma_descr,
                                  sizeof(olist_dma_descr))) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write olist DMA Descr @ 0x%lx",
                      (uint64_t) olist_dma_descr_addr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }

    /* Setup Asymmetric Request Descriptor */
    asym_req_descr.input_list_addr = ilist_dma_descr_addr;
    asym_req_descr.output_list_addr = olist_dma_descr_addr;
    asym_req_descr.key_descr_idx = ecc_p256_key_idx;
    asym_req_descr.status_addr = status_mem_addr;
    asym_req_descr.opaque_tag_value = 0;
    asym_req_descr.opage_tag_wr_en = 0;
    asym_req_descr.flag_a = 0;
    asym_req_descr.flag_b = 0;

    ret = elba_barco_ring_queue_request(BARCO_RING_ASYM, (void *)&asym_req_descr, &req_tag, true);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to enqueue request");
        ret = SDK_RET_ERR;
        goto cleanup;
    }

    // Wait for operation to be completed
    elba_barco_wait_for_resp(req_tag, async_en, unique_key);

    if (sdk::asic::asic_mem_read(asym_req_descr.status_addr,
                                 (uint8_t*)&status, sizeof(status))) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to retrieve operation status @ 0x%lx",
                      (uint64_t) asym_req_descr.status_addr);
        ret = SDK_RET_ERR;
        goto cleanup;
    }
    if (status != 0) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Operation failed with status %u",
                      status);
        ret = SDK_RET_ERR;
        goto cleanup;
    }
    else {
        /* Copy out the results */
        if (sdk::asic::asic_mem_read(olist_mem_addr, (uint8_t*)c, key_size)) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to read output c from memory @ 0x%lx",
                          (uint64_t) olist_mem_addr);
            ret = SDK_RET_INVALID_ARG;
            goto cleanup;
        }
        ELBA_BARCO_API_PARAM_HEXDUMP((char *)"c", (char *)c, key_size);
        SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "RSA Encrypt succeeded");
    }

cleanup:

    if (status_mem_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_HBM_MEM_512B,
                                   status_mem_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for status content:0x%lx",
                          status_mem_addr);
        }
    }

    if (olist_mem_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_HBM_MEM_512B,
                                   olist_mem_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for olist content:0x%lx",
                          olist_mem_addr);
        }
    }

    if (ilist_mem_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_HBM_MEM_512B,
                                   ilist_mem_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR("ECC Point Mul P256: Failed to free memory for ilist content:0x%lx",
                          ilist_mem_addr);
        }
    }

    if (olist_dma_descr_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                                   olist_dma_descr_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for olist DMA Descr: 0x%lx",
                          olist_dma_descr_addr);
        }
    }

    if (ilist_dma_descr_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                                   ilist_dma_descr_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for ilist DMA Descr: 0x%lx",
                          ilist_dma_descr_addr);
        }
    }

    if (ecc_p256_key_idx != -1) {
        ret = elba_barco_asym_free_key(ecc_p256_key_idx);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME"Failed to free key descriptor");
        }
    }

    if (key_dma_descr_addr2) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                                   key_dma_descr_addr2);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for key DMA Descr2: 0x%lx",
key_dma_descr_addr2);
        }
    }

    if (key_dma_descr_addr1) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                                   key_dma_descr_addr1);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for key DMA Descr1: 0x%lx",
                          key_dma_descr_addr1);
        }
    }

    if (key_param_addr2) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_HBM_MEM_512B,
                                   key_param_addr2);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for key param2 :0x%lx",
                          key_param_addr2);
        }
    }

    if (key_param_addr1) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_HBM_MEM_512B,
                                   key_param_addr1);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for key param1 :0x%lx",
                          key_param_addr1);
        }
    }

    if (status)
        ret = SDK_RET_ERR;

    return ret;
}

sdk_ret_t
elba_barco_asym_rsa2k_decrypt (uint8_t *n, uint8_t *d, uint8_t *c,  uint8_t *m)
{
    sdk_ret_t                   ret = SDK_RET_OK;
    uint64_t                    ilist_dma_descr_addr = 0,
                                olist_dma_descr_addr = 0;
    uint64_t                    key_dma_descr_addr = 0;
    uint64_t                    ilist_mem_addr = 0,
                                olist_mem_addr = 0, curr_ptr = 0;
    uint64_t                    key_param_addr = 0;
    barco_asym_descriptor_t     asym_req_descr;
    barco_asym_dma_descriptor_t ilist_dma_descr, olist_dma_descr;
    barco_asym_dma_descriptor_t key_dma_descr;
    int32_t                     ecc_p256_key_idx = -1;
    elba_barco_asym_key_desc_t  asym_key;
    uint32_t                    req_tag = 0;
    uint32_t                    status = 0;

#undef ELBA_BARCO_API_NAME
#define ELBA_BARCO_API_NAME "RSA 2K Decrypt: "

    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"n", (char *)n, 256);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"d", (char *)d, 256);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"c", (char *)c, 256);

    /* Setup params in the key memory */
    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_HBM_MEM_512B,
                               NULL, &key_param_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for key param");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for key param @ 0x%lx", key_param_addr);

    curr_ptr = key_param_addr;

    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)n, 256)) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write RSA param n into key memory @ 0x%lx", (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    curr_ptr += 256;

    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)d, 256)) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write RSA param d into key memory @ 0x%lx", (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    curr_ptr += 256;

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                               NULL, &key_dma_descr_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for key DMA Descr");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for key DMA Descr @ 0x%lx", key_dma_descr_addr);

    /* Setup key DMA descriptor */
    key_dma_descr.address = key_param_addr;
    key_dma_descr.stop = 1;
    key_dma_descr.rsvd0 = 1;
    key_dma_descr.next = 0;
    key_dma_descr.int_en = 0;
    key_dma_descr.discard = 0;
    key_dma_descr.realign = 1;
    key_dma_descr.cst_addr = 0;
    key_dma_descr.length = (curr_ptr - key_param_addr);
    if (sdk::asic::asic_mem_write(key_dma_descr_addr, (uint8_t*)&key_dma_descr,
                sizeof(key_dma_descr))) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write key DMA Descr @ 0x%lx",
                (uint64_t) key_dma_descr_addr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }

    ret = elba_barco_asym_alloc_key(&ecc_p256_key_idx);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate key descriptor");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated Key Descr @ %d",
                    ecc_p256_key_idx);

    asym_key.key_param_list = key_dma_descr_addr;
    asym_key.command_reg = (ELBA_BARCO_ASYM_CMD_SWAP_BYTES |
                            ELBA_BARCO_ASYM_CMD_SIZE_OF_OP(256) |
                            ELBA_BARCO_ASYM_CMD_RSA_DECRYPT);

    ret = elba_barco_asym_write_key(ecc_p256_key_idx, &asym_key);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write key: %d",
                      ecc_p256_key_idx);
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Setup key @ %d", ecc_p256_key_idx);

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                               NULL, &ilist_dma_descr_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for ilist DMA Descr");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for ilist DMA Descr @ 0x%lx", ilist_dma_descr_addr);

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                               NULL, &olist_dma_descr_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for olist DMA Descr");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for olist DMA Descr @ 0x%lx", olist_dma_descr_addr);

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_HBM_MEM_512B,
                               NULL, &ilist_mem_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for ilist content");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for input mem @ 0x%lx", ilist_mem_addr);

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_HBM_MEM_512B,
                               NULL, &olist_mem_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for olist content");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for output mem @ 0x%lx", olist_mem_addr);

    /* Copy the input to the ilist memory */
    curr_ptr = ilist_mem_addr;

    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)c, 256)) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write RSA param c into ilist memory @ 0x%lx", (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    curr_ptr += 256;

    /* Setup ilist DMA descriptor */
    ilist_dma_descr.address = ilist_mem_addr;
    ilist_dma_descr.stop = 1;
    ilist_dma_descr.rsvd0 = 1;
    ilist_dma_descr.next = 0;
    ilist_dma_descr.int_en = 0;
    ilist_dma_descr.discard = 0;
    ilist_dma_descr.realign = 1;
    ilist_dma_descr.cst_addr = 0;
    ilist_dma_descr.length = (curr_ptr - ilist_mem_addr);
    if (sdk::asic::asic_mem_write(ilist_dma_descr_addr,
                                  (uint8_t*)&ilist_dma_descr,
                                  sizeof(ilist_dma_descr))) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write ilist DMA Descr @ 0x%lx",
                (uint64_t) ilist_dma_descr_addr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }

    /* Setup olist DMA descriptor */
    olist_dma_descr.address = olist_mem_addr;
    olist_dma_descr.stop = 1;
    olist_dma_descr.rsvd0 = 1;
    olist_dma_descr.next = 0;
    olist_dma_descr.int_en = 0;
    olist_dma_descr.discard = 0;
    olist_dma_descr.realign = 1;
    olist_dma_descr.cst_addr = 0;
    olist_dma_descr.length = (1 * 256); /* plain text */
    if (sdk::asic::asic_mem_write(olist_dma_descr_addr,
                                  (uint8_t*)&olist_dma_descr,
                                  sizeof(olist_dma_descr))) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write olist DMA Descr @ 0x%lx",
                (uint64_t) olist_dma_descr_addr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }

    /* Setup Asymmetric Request Descriptor */
    asym_req_descr.input_list_addr = ilist_dma_descr_addr;
    asym_req_descr.output_list_addr = olist_dma_descr_addr;
    asym_req_descr.key_descr_idx = ecc_p256_key_idx;
    asym_req_descr.status_addr = olist_mem_addr + 256;
    asym_req_descr.opaque_tag_value = 0;
    asym_req_descr.opage_tag_wr_en = 0;
    asym_req_descr.flag_a = 0;
    asym_req_descr.flag_b = 0;

    ret = elba_barco_ring_queue_request(BARCO_RING_ASYM,
                                        (void *)&asym_req_descr,
                                        &req_tag, true);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to enqueue request");
        ret = SDK_RET_ERR;
        goto cleanup;
    }
    /* Poll for completion */
    while (elba_barco_ring_poll(BARCO_RING_ASYM, req_tag) != TRUE) {
        //SDK_TRACE_DEBUG("ECC Point Mul P256: Waiting for Barco completion...");
    }
    if (sdk::asic::asic_mem_read(asym_req_descr.status_addr,
                                 (uint8_t*)&status, sizeof(status))) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to retrieve operation status @ 0x%lx",
                      (uint64_t) asym_req_descr.status_addr);
        ret = SDK_RET_ERR;
        goto cleanup;
    }
    if (status != 0) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Operation failed with status %u",
                      status);
        ret = SDK_RET_ERR;
        goto cleanup;
    }
    else {
        /* Copy out the results */
        if (sdk::asic::asic_mem_read(olist_mem_addr, (uint8_t*)m, 256)) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to read output m from memory @ 0x%lx",
                          (uint64_t) olist_mem_addr);
            ret = SDK_RET_INVALID_ARG;
            goto cleanup;
        }
        ELBA_BARCO_API_PARAM_HEXDUMP((char *)"m", (char *)m, 256);
        SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "RSA Decrypt succeeded");
    }

cleanup:
    if (olist_mem_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_HBM_MEM_512B,
                                  olist_mem_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for olist content:0x%lx",
                          olist_mem_addr);
        }
    }

    if (ilist_mem_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_HBM_MEM_512B,
                                  ilist_mem_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR("ECC Point Mul P256: Failed to free memory for ilist content:0x%lx",
                    ilist_mem_addr);
        }
    }

    if (olist_dma_descr_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                                  olist_dma_descr_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for olist DMA Descr: 0x%lx",
                    olist_dma_descr_addr);
        }
    }

    if (ilist_dma_descr_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                                  ilist_dma_descr_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for ilist DMA Descr: 0x%lx",
                          ilist_dma_descr_addr);
        }
    }

    if (ecc_p256_key_idx != -1) {
        ret = elba_barco_asym_free_key(ecc_p256_key_idx);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME"Failed to free key descriptor");
        }
    }

    if (key_dma_descr_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                                  key_dma_descr_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for key DMA Descr: 0x%lx",
                    key_dma_descr_addr);
        }
    }

    if (key_param_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_HBM_MEM_512B,
                                  key_param_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for key param :0x%lx",
                          key_param_addr);
        }
    }

    if (status)
        ret = SDK_RET_ERR;

    return ret;
}

sdk_ret_t
elba_barco_asym_rsa2k_crt_setup_decrypt_priv_key (uint8_t *p, uint8_t *q,
                                                  uint8_t *dp, uint8_t *dq,
                                                  uint8_t *qinv,
                                                  int32_t* key_idx)
{
    sdk_ret_t                   ret = SDK_RET_OK;
    uint64_t                    key_dma_descr_addr1 = 0,
                                key_dma_descr_addr2 = 0,
                                key_dma_descr_addr3 = 0;
    uint64_t                    curr_ptr = 0;
    uint64_t                    key_param_addr1 = 0, key_param_addr2 = 0,
                                key_param_addr3 = 0;
    barco_asym_dma_descriptor_t key_dma_descr;
    elba_barco_asym_key_desc_t  asym_key;

#undef ELBA_BARCO_API_NAME
#define ELBA_BARCO_API_NAME "RSA 2K CRT Key setup: "

    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"p", (char *)p, 256);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"q", (char *)q, 256);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"dp", (char *)dp, 256);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"dq", (char *)dq, 256);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"qinv", (char *)qinv, 256);

    *key_idx = -1;

    /* Key Param fragment 1 */
    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_HBM_MEM_512B,
                               NULL, &key_param_addr1);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for key param 1");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for key param 1 @ 0x%lx", key_param_addr1);
    /* Key Param fragment 2 */
    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_HBM_MEM_512B,
                               NULL, &key_param_addr2);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for key param 2");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for key param 2 @ 0x%lx", key_param_addr2);
    /* Key Param fragment 3 */
    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_HBM_MEM_512B,
                               NULL, &key_param_addr3);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for key param 3");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for key param 3 @ 0x%lx", key_param_addr3);

    /* Allocate key DMA descriptor 1 */
    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                               NULL, &key_dma_descr_addr1);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for key DMA Descr1");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for key DMA Descr1 @ 0x%lx", key_dma_descr_addr1);

    /* Allocate key DMA descriptor 2 */
    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                               NULL, &key_dma_descr_addr2);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for key DMA Descr2");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for key DMA Descr2 @ 0x%lx", key_dma_descr_addr2);

    /* Allocate key DMA descriptor 3 */
    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                               NULL, &key_dma_descr_addr3);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for key DMA Descr3");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for key DMA Descr3 @ 0x%lx", key_dma_descr_addr3);

    /* Setup params in the key memory 1 */
    curr_ptr = key_param_addr1;

    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)p, 256)) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write RSA param p into key memory @ 0x%lx", (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    curr_ptr += 256;

    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)q, 256)) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write RSA param q into key memory @ 0x%lx", (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    curr_ptr += 256;

    key_dma_descr.address = key_param_addr1;
    key_dma_descr.stop = 0;
    key_dma_descr.rsvd0 = 1;
    key_dma_descr.next = (key_dma_descr_addr2 >> 2);
    key_dma_descr.int_en = 0;
    key_dma_descr.discard = 0;
    key_dma_descr.realign = 0;
    key_dma_descr.cst_addr = 0;
    key_dma_descr.length = (curr_ptr - key_param_addr1);
    if (sdk::asic::asic_mem_write(key_dma_descr_addr1, (uint8_t*)&key_dma_descr,
                                  sizeof(key_dma_descr))) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write key DMA Descr @ 0x%lx",
                      (uint64_t) key_dma_descr_addr1);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }

    /* Setup params in the key memory 2 */
    curr_ptr = key_param_addr2;

    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)dp, 256)) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write RSA param dp into key memory @ 0x%lx",
                      (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    curr_ptr += 256;

    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)dq, 256)) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write RSA param dq into key memory @ 0x%lx",
                      (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    curr_ptr += 256;

    /* Setup key DMA descriptor 2 */
    key_dma_descr.address = key_param_addr2;
    key_dma_descr.stop = 0;
    key_dma_descr.rsvd0 = 1;
    key_dma_descr.next = (key_dma_descr_addr3 >> 2);
    key_dma_descr.int_en = 0;
    key_dma_descr.discard = 0;
    key_dma_descr.realign = 0;
    key_dma_descr.cst_addr = 0;
    key_dma_descr.length = (curr_ptr - key_param_addr2);
    if (sdk::asic::asic_mem_write(key_dma_descr_addr2, (uint8_t*)&key_dma_descr,
                sizeof(key_dma_descr))) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write key DMA Descr @ 0x%lx",
                (uint64_t) key_dma_descr_addr2);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }

    /* Setup params in the key memory 3 */
    curr_ptr = key_param_addr3;

    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)qinv, 256)) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write RSA param qinv into key memory @ 0x%lx", (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    curr_ptr += 256;

    /* Setup key DMA descriptor 3 */
    key_dma_descr.address = key_param_addr3;
    key_dma_descr.stop = 1;
    key_dma_descr.rsvd0 = 1;
    key_dma_descr.next = 0;
    key_dma_descr.int_en = 0;
    key_dma_descr.discard = 0;
    key_dma_descr.realign = 1;
    key_dma_descr.cst_addr = 0;
    key_dma_descr.length = (curr_ptr - key_param_addr3);
    if (sdk::asic::asic_mem_write(key_dma_descr_addr3, (uint8_t*)&key_dma_descr,
                sizeof(key_dma_descr))) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write key DMA Descr @ 0x%lx",
                (uint64_t) key_dma_descr_addr3);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }

    ret = elba_barco_asym_alloc_key(key_idx);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate key descriptor");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated Key Descr @ %d", *key_idx);

    asym_key.key_param_list = key_dma_descr_addr1;
    asym_key.command_reg = (ELBA_BARCO_ASYM_CMD_SWAP_BYTES |
                            ELBA_BARCO_ASYM_CMD_SIZE_OF_OP(256) |
                            ELBA_BARCO_ASYM_CMD_RSA_CRT_DECRYPT);

    ret = elba_barco_asym_write_key(*key_idx, &asym_key);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write key: %d", *key_idx);
        goto cleanup;
    }

    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Setup key @ %d", *key_idx);
    return ret;

cleanup:
    if (*key_idx != -1) {
        ret = elba_barco_asym_free_key(*key_idx);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME"Failed to free key descriptor");
        }
    }

    if (key_dma_descr_addr3) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                                  key_dma_descr_addr3);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for key DMA Descr: 0x%lx",
                    key_dma_descr_addr3);
        }
    }

    if (key_dma_descr_addr2) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                                  key_dma_descr_addr2);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for key DMA Descr: 0x%lx",
                    key_dma_descr_addr2);
        }
    }

    if (key_dma_descr_addr1) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                                  key_dma_descr_addr1);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for key DMA Descr: 0x%lx",
                    key_dma_descr_addr1);
        }
    }

    if (key_param_addr3) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_HBM_MEM_512B,
                                  key_param_addr3);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for key param :0x%lx",
                    key_param_addr3);
        }
    }

    if (key_param_addr2) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_HBM_MEM_512B,
                                  key_param_addr2);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for key param :0x%lx",
                    key_param_addr2);
        }
    }

    if (key_param_addr1) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_HBM_MEM_512B,
                                  key_param_addr1);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for key param :0x%lx",
                    key_param_addr1);
        }
    }

    return ret;
}

sdk_ret_t
elba_barco_asym_rsa2k_crt_decrypt (int32_t key_idx, uint8_t *p, uint8_t *q,
                                   uint8_t *dp, uint8_t *dq,
                                   uint8_t *qinv, uint8_t *c, uint8_t *m,
                                   bool async_en, const uint8_t *unique_key)
{
    sdk_ret_t                   ret = SDK_RET_OK;
    uint64_t                    ilist_dma_descr_addr = 0,
                                olist_dma_descr_addr = 0;
    uint64_t                    ilist_mem_addr = 0,
                                olist_mem_addr = 0, curr_ptr = 0;
    barco_asym_descriptor_t     asym_req_descr;
    barco_asym_dma_descriptor_t ilist_dma_descr, olist_dma_descr;
    int32_t                     ecc_p256_key_idx = -1;
    uint32_t                    req_tag = 0;
    uint32_t                    status = 0;

#undef ELBA_BARCO_API_NAME
#define ELBA_BARCO_API_NAME "RSA 2K CRT Decrypt: "

    SDK_TRACE_DEBUG("key_idx: %d", key_idx);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"c", (char *)c, 256);

    if(key_idx < 0) {
        ELBA_BARCO_API_PARAM_HEXDUMP((char *)"p", (char *)p, 256);
        ELBA_BARCO_API_PARAM_HEXDUMP((char *)"q", (char *)q, 256);
        ELBA_BARCO_API_PARAM_HEXDUMP((char *)"dp", (char *)dp, 256);
        ELBA_BARCO_API_PARAM_HEXDUMP((char *)"dq", (char *)dq, 256);
        ELBA_BARCO_API_PARAM_HEXDUMP((char *)"qinv", (char *)qinv, 256);

        ret = elba_barco_asym_rsa2k_crt_setup_decrypt_priv_key(p, q, dp, dq,
                                                               qinv,
                                                               &ecc_p256_key_idx);
        if(ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to setup private key");
            goto cleanup;
        }
    } else {
        ecc_p256_key_idx = key_idx;
    }

    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "key @ %d", ecc_p256_key_idx);

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                               NULL, &ilist_dma_descr_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for ilist DMA Descr");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for ilist DMA Descr @ 0x%lx", ilist_dma_descr_addr);

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                               NULL, &olist_dma_descr_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for olist DMA Descr");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for olist DMA Descr @ 0x%lx", olist_dma_descr_addr);

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_HBM_MEM_512B,
                               NULL, &ilist_mem_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for ilist content");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for input mem @ 0x%lx", ilist_mem_addr);

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_HBM_MEM_512B,
                               NULL, &olist_mem_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for olist content");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for output mem @ 0x%lx", olist_mem_addr);

    /* Copy the input to the ilist memory */
    curr_ptr = ilist_mem_addr;

    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)c, 256)) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write RSA param c into ilist memory @ 0x%lx", (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    curr_ptr += 256;

    /* Setup ilist DMA descriptor */
    ilist_dma_descr.address = ilist_mem_addr;
    ilist_dma_descr.stop = 1;
    ilist_dma_descr.rsvd0 = 1;
    ilist_dma_descr.next = 0;
    ilist_dma_descr.int_en = 0;
    ilist_dma_descr.discard = 0;
    ilist_dma_descr.realign = 1;
    ilist_dma_descr.cst_addr = 0;
    ilist_dma_descr.length = (curr_ptr - ilist_mem_addr);
    if (sdk::asic::asic_mem_write(ilist_dma_descr_addr,
                                  (uint8_t*)&ilist_dma_descr,
                                  sizeof(ilist_dma_descr))) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write ilist DMA Descr @ 0x%lx",
                (uint64_t) ilist_dma_descr_addr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }

    /* Setup olist DMA descriptor */
    olist_dma_descr.address = olist_mem_addr;
    olist_dma_descr.stop = 1;
    olist_dma_descr.rsvd0 = 1;
    olist_dma_descr.next = 0;
    olist_dma_descr.int_en = 0;
    olist_dma_descr.discard = 0;
    olist_dma_descr.realign = 1;
    olist_dma_descr.cst_addr = 0;
    olist_dma_descr.length = (1 * 256); /* plain text */
    if (sdk::asic::asic_mem_write(olist_dma_descr_addr,
                                  (uint8_t*)&olist_dma_descr,
                                  sizeof(olist_dma_descr))) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write olist DMA Descr @ 0x%lx",
                (uint64_t) olist_dma_descr_addr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }

    /* Setup Asymmetric Request Descriptor */
    asym_req_descr.input_list_addr = ilist_dma_descr_addr;
    asym_req_descr.output_list_addr = olist_dma_descr_addr;
    asym_req_descr.key_descr_idx = ecc_p256_key_idx;
    asym_req_descr.status_addr = olist_mem_addr + 256;
    asym_req_descr.opaque_tag_value = 0;
    asym_req_descr.opage_tag_wr_en = 0;
    asym_req_descr.flag_a = 0;
    asym_req_descr.flag_b = 0;

    ret = elba_barco_ring_queue_request(BARCO_RING_ASYM,
                                        (void *)&asym_req_descr,
                                        &req_tag, true);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to enqueue request");
        ret = SDK_RET_ERR;
        goto cleanup;
    }
    // Wait for operation to be completed
    elba_barco_wait_for_resp(req_tag, async_en, unique_key);

    if (sdk::asic::asic_mem_read(asym_req_descr.status_addr,
                                 (uint8_t*)&status, sizeof(status))) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to retrieve operation status @ 0x%lx",
                (uint64_t) asym_req_descr.status_addr);
        ret = SDK_RET_ERR;
        goto cleanup;
    }
    if (status != 0) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Operation failed with status %u",
                status);
        ret = SDK_RET_ERR;
        goto cleanup;
    }
    else {
        /* Copy out the results */
        if (sdk::asic::asic_mem_read(olist_mem_addr, (uint8_t*)m, 256)) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to read output m from memory @ 0x%lx",
                    (uint64_t) olist_mem_addr);
            ret = SDK_RET_INVALID_ARG;
            goto cleanup;
        }
        ELBA_BARCO_API_PARAM_HEXDUMP((char *)"m", (char *)m, 256);
        SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "RSA CRT Decrypt succeeded");
    }

cleanup:
    if (olist_mem_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_HBM_MEM_512B,
                                  olist_mem_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for olist content:0x%lx",
                    olist_mem_addr);
        }
    }

    if (ilist_mem_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_HBM_MEM_512B,
                                  ilist_mem_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR("ECC Point Mul P256: Failed to free memory for ilist content:0x%lx",
                    ilist_mem_addr);
        }
    }

    if (olist_dma_descr_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                                  olist_dma_descr_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for olist DMA Descr: 0x%lx",
                    olist_dma_descr_addr);
        }
    }

    if (ilist_dma_descr_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                                  ilist_dma_descr_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for ilist DMA Descr: 0x%lx",
                    ilist_dma_descr_addr);
        }
    }

    if (status)
        ret = SDK_RET_ERR;

    return ret;
}

sdk_ret_t
elba_barco_asym_rsa2k_sig_gen (int32_t key_idx, uint8_t *n, uint8_t *d,
                               uint8_t *h, uint8_t *s, bool async_en,
                               const uint8_t *unique_key)
{
    sdk_ret_t                   ret = SDK_RET_OK;
    uint64_t                    ilist_dma_descr_addr = 0,
                                olist_dma_descr_addr = 0;
    uint64_t                    ilist_mem_addr = 0,
                                olist_mem_addr = 0, curr_ptr = 0;
    barco_asym_descriptor_t     asym_req_descr;
    barco_asym_dma_descriptor_t ilist_dma_descr, olist_dma_descr;
    int32_t                     ecc_p256_key_idx = -1;
    uint32_t                    req_tag = 0;
    uint32_t                    status = 0;

#undef ELBA_BARCO_API_NAME
#define ELBA_BARCO_API_NAME "RSA 2K Sig Gen: "
    SDK_TRACE_DEBUG("key_idx: %d", key_idx);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"n", (char *)n, 256);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"h", (char *)h, 256);

    if(key_idx < 0) {
        ELBA_BARCO_API_PARAM_HEXDUMP((char *)"d", (char *)d, 256);
        ret = elba_barco_asym_rsa2k_setup_sig_gen_priv_key(n, d,
                                                           &ecc_p256_key_idx);
        if(ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to setup private key");
            goto cleanup;
        }
    } else {
        ecc_p256_key_idx = key_idx;
    }

    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "key @ %d", ecc_p256_key_idx);

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                               NULL, &ilist_dma_descr_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for ilist DMA Descr");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for ilist DMA Descr @ 0x%lx", ilist_dma_descr_addr);

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                               NULL, &olist_dma_descr_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for olist DMA Descr");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for olist DMA Descr @ 0x%lx", olist_dma_descr_addr);

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_HBM_MEM_512B,
                               NULL, &ilist_mem_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for ilist content");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for input mem @ 0x%lx", ilist_mem_addr);

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_HBM_MEM_512B,
                               NULL, &olist_mem_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for olist content");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for output mem @ 0x%lx", olist_mem_addr);

    /* Copy the input to the ilist memory */
    curr_ptr = ilist_mem_addr;

    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)h, 256)) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write RSA param h into ilist memory @ 0x%lx", (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    curr_ptr += 256;

    /* Setup ilist DMA descriptor */
    ilist_dma_descr.address = ilist_mem_addr;
    ilist_dma_descr.stop = 1;
    ilist_dma_descr.rsvd0 = 1;
    ilist_dma_descr.next = 0;
    ilist_dma_descr.int_en = 0;
    ilist_dma_descr.discard = 0;
    ilist_dma_descr.realign = 1;
    ilist_dma_descr.cst_addr = 0;
    ilist_dma_descr.length = (curr_ptr - ilist_mem_addr);
    if (sdk::asic::asic_mem_write(ilist_dma_descr_addr,
                                  (uint8_t*)&ilist_dma_descr,
                                  sizeof(ilist_dma_descr))) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write ilist DMA Descr @ 0x%lx",
                (uint64_t) ilist_dma_descr_addr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }

    /* Setup olist DMA descriptor */
    olist_dma_descr.address = olist_mem_addr;
    olist_dma_descr.stop = 1;
    olist_dma_descr.rsvd0 = 1;
    olist_dma_descr.next = 0;
    olist_dma_descr.int_en = 0;
    olist_dma_descr.discard = 0;
    olist_dma_descr.realign = 1;
    olist_dma_descr.cst_addr = 0;
    olist_dma_descr.length = (1 * 256); /* sig */
    if (sdk::asic::asic_mem_write(olist_dma_descr_addr,
                                  (uint8_t*)&olist_dma_descr,
                                  sizeof(olist_dma_descr))) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write olist DMA Descr @ 0x%lx",
                (uint64_t) olist_dma_descr_addr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }

    /* Setup Asymmetric Request Descriptor */
    asym_req_descr.input_list_addr = ilist_dma_descr_addr;
    asym_req_descr.output_list_addr = olist_dma_descr_addr;
    asym_req_descr.key_descr_idx = ecc_p256_key_idx;
    asym_req_descr.status_addr = olist_mem_addr + 256;
    asym_req_descr.opaque_tag_value = 0;
    asym_req_descr.opage_tag_wr_en = 0;
    asym_req_descr.flag_a = 0;
    asym_req_descr.flag_b = 0;

    ret = elba_barco_ring_queue_request(BARCO_RING_ASYM,
                                        (void *)&asym_req_descr,
                                        &req_tag, true);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to enqueue request");
        ret = SDK_RET_ERR;
        goto cleanup;
    }

    // Wait for operation to be completed
    elba_barco_wait_for_resp(req_tag, async_en, unique_key);

    if (sdk::asic::asic_mem_read(asym_req_descr.status_addr,
                                 (uint8_t*)&status, sizeof(status))) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to retrieve operation status @ 0x%lx",
                (uint64_t) asym_req_descr.status_addr);
        ret = SDK_RET_ERR;
        goto cleanup;
    }
    if (status != 0) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Operation failed with status %u",
                      status);
        ret = SDK_RET_ERR;
        goto cleanup;
    }
    else {
        /* Copy out the results */
        if (sdk::asic::asic_mem_read(olist_mem_addr, (uint8_t*)s, 256)) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to read output s from memory @ 0x%lx",
                    (uint64_t) olist_mem_addr);
            ret = SDK_RET_INVALID_ARG;
            goto cleanup;
        }
        ELBA_BARCO_API_PARAM_HEXDUMP((char *)"s", (char *)s, 256);
        SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "RSA Sig Gen succeeded");
    }

cleanup:
    if (olist_mem_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_HBM_MEM_512B,
                                  olist_mem_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for olist content:0x%lx",
                    olist_mem_addr);
        }
    }

    if (ilist_mem_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_HBM_MEM_512B,
                                  ilist_mem_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR("ECC Point Mul P256: Failed to free memory for ilist content:0x%lx",
                    ilist_mem_addr);
        }
    }

    if (olist_dma_descr_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                                  olist_dma_descr_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for olist DMA Descr: 0x%lx",
                    olist_dma_descr_addr);
        }
    }

    if (ilist_dma_descr_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                                  ilist_dma_descr_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for ilist DMA Descr: 0x%lx",
                    ilist_dma_descr_addr);
        }
    }

    if (status)
        ret = SDK_RET_ERR;

    return ret;
}

sdk_ret_t
elba_barco_asym_rsa_sig_gen (uint16_t key_size, int32_t key_idx,
                              uint8_t *n, uint8_t *d, uint8_t *h, uint8_t *s,
                              bool async_en, const uint8_t *unique_key)
{
    sdk_ret_t                   ret = SDK_RET_OK;
    uint64_t                    ilist_dma_descr_addr = 0, olist_dma_descr_addr = 0;
    uint64_t                    ilist_mem_addr = 0, olist_mem_addr = 0, curr_ptr = 0, status_mem_addr = 0;
    barco_asym_descriptor_t     asym_req_descr;
    barco_asym_dma_descriptor_t ilist_dma_descr, olist_dma_descr;
    int32_t                     ecc_p256_key_idx = -1;
    uint32_t                    req_tag = 0;
    uint32_t                    status = 0;

#undef ELBA_BARCO_API_NAME
#define ELBA_BARCO_API_NAME "RSA Sig Gen: "
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"n", (char *)n, key_size);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"h", (char *)h, key_size);

    if(key_idx < 0) {
        ELBA_BARCO_API_PARAM_HEXDUMP((char *)"d", (char *)d, key_size);
        ret = elba_barco_asym_rsa_setup_priv_key(key_size, n, d, &ecc_p256_key_idx);
        if(ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to setup private key");
            goto cleanup;
        }
    } else {
        ecc_p256_key_idx = key_idx;
    }

    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "key @ %d", ecc_p256_key_idx);

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
            NULL, &ilist_dma_descr_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for ilist DMA Descr");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for ilist DMA Descr @ 0x%lx",
                    ilist_dma_descr_addr);

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
            NULL, &olist_dma_descr_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for olist DMA Descr");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for olist DMA Descr @ 0x%lx",
                    olist_dma_descr_addr);

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_HBM_MEM_512B,
            NULL, &ilist_mem_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for ilist content");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for input mem @ 0x%lx",
                    ilist_mem_addr);

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_HBM_MEM_512B,
            NULL, &olist_mem_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for olist content");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for output mem @ 0x%lx",
                    olist_mem_addr);

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_HBM_MEM_512B,
            NULL, &status_mem_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for status content");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for status mem @ 0x%lx",
                    status_mem_addr);

    /* Copy the input to the ilist memory */
    curr_ptr = ilist_mem_addr;

    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)h, key_size)) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write RSA param h into ilist memory @ 0x%lx", (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    curr_ptr += key_size;

    /* Setup ilist DMA descriptor */
    ilist_dma_descr.address = ilist_mem_addr;
    ilist_dma_descr.stop = 1;
    ilist_dma_descr.rsvd0 = 1;
    ilist_dma_descr.next = 0;
    ilist_dma_descr.int_en = 0;
    ilist_dma_descr.discard = 0;
    ilist_dma_descr.realign = 1;
    ilist_dma_descr.cst_addr = 0;
    ilist_dma_descr.length = (curr_ptr - ilist_mem_addr);
    if (sdk::asic::asic_mem_write(ilist_dma_descr_addr,
                                  (uint8_t*)&ilist_dma_descr,
                                  sizeof(ilist_dma_descr))) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write ilist DMA Descr @ 0x%lx",
                (uint64_t) ilist_dma_descr_addr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }

    /* Setup olist DMA descriptor */
    olist_dma_descr.address = olist_mem_addr;
    olist_dma_descr.stop = 1;
    olist_dma_descr.rsvd0 = 1;
    olist_dma_descr.next = 0;
    olist_dma_descr.int_en = 0;
    olist_dma_descr.discard = 0;
    olist_dma_descr.realign = 1;
    olist_dma_descr.cst_addr = 0;
    olist_dma_descr.length = (1 * key_size); /* sig */
    if (sdk::asic::asic_mem_write(olist_dma_descr_addr,
                                  (uint8_t*)&olist_dma_descr,
                                  sizeof(olist_dma_descr))) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write olist DMA Descr @ 0x%lx",
                (uint64_t) olist_dma_descr_addr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }

    /* Setup Asymmetric Request Descriptor */
    asym_req_descr.input_list_addr = ilist_dma_descr_addr;
    asym_req_descr.output_list_addr = olist_dma_descr_addr;
    asym_req_descr.key_descr_idx = ecc_p256_key_idx;
    asym_req_descr.status_addr = status_mem_addr;
    asym_req_descr.opaque_tag_value = 0;
    asym_req_descr.opage_tag_wr_en = 0;
    asym_req_descr.flag_a = 0;
    asym_req_descr.flag_b = 0;

    ret = elba_barco_ring_queue_request(BARCO_RING_ASYM, (void *)&asym_req_descr, &req_tag, true);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to enqueue request");
        ret = SDK_RET_ERR;
        goto cleanup;
    }

    // Wait for operation to be completed
    elba_barco_wait_for_resp(req_tag, async_en, unique_key);

    if (sdk::asic::asic_mem_read(asym_req_descr.status_addr,
                                 (uint8_t*)&status, sizeof(status))) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to retrieve operation status @ 0x%lx",
                (uint64_t) asym_req_descr.status_addr);
        ret = SDK_RET_ERR;
        goto cleanup;
    }
    if (status != 0) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Operation failed with status 0x%lx",
                status);
        ret = SDK_RET_ERR;
        goto cleanup;
    }
    else {
        /* Copy out the results */
        if (sdk::asic::asic_mem_read(olist_mem_addr, (uint8_t*)s, key_size)) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to read output s from memory @ 0x%lx",
                    (uint64_t) olist_mem_addr);
            ret = SDK_RET_INVALID_ARG;
            goto cleanup;
        }
        ELBA_BARCO_API_PARAM_HEXDUMP((char *)"s", (char *)s, key_size);
        SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "RSA Sig Gen succeeded");
    }

cleanup:

    if (status_mem_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_HBM_MEM_512B, status_mem_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for status content:0x%lx",
                    status_mem_addr);
        }
    }

    if (olist_mem_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_HBM_MEM_512B, olist_mem_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for olist content:0x%lx",
                    olist_mem_addr);
        }
    }

    if (ilist_mem_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_HBM_MEM_512B, ilist_mem_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for ilist content:0x%lx",
                    ilist_mem_addr);
        }
    }

    if (olist_dma_descr_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_ASYM_DMA_DESCR, olist_dma_descr_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for olist DMA Descr: 0x%lx",
                    olist_dma_descr_addr);
        }
    }

    if (ilist_dma_descr_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_ASYM_DMA_DESCR, ilist_dma_descr_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for ilist DMA Descr: 0x%lx",
                    ilist_dma_descr_addr);
        }
    }

    if (status)
        ret = SDK_RET_ERR;

    return ret;
}

sdk_ret_t
elba_barco_asym_rsa2k_sig_verify (uint8_t *n, uint8_t *e,
                                  uint8_t *h, uint8_t *s)
{
    sdk_ret_t                   ret = SDK_RET_OK;
    uint64_t                    ilist_dma_descr_addr1 = 0,
                                ilist_dma_descr_addr2 = 0;
    uint64_t                    key_dma_descr_addr = 0;
    uint64_t                    ilist_mem_addr1 = 0,
                                ilist_mem_addr2 = 0,
                                olist_mem_addr = 0, curr_ptr = 0;
    uint64_t                    key_param_addr = 0;
    barco_asym_descriptor_t     asym_req_descr;
    barco_asym_dma_descriptor_t ilist_dma_descr;
    barco_asym_dma_descriptor_t key_dma_descr;
    int32_t                     ecc_p256_key_idx = -1;
    elba_barco_asym_key_desc_t  asym_key;
    uint32_t                    req_tag = 0;
    uint32_t                    status = 0;

#undef ELBA_BARCO_API_NAME
#define ELBA_BARCO_API_NAME "RSA 2K Sig Verify: "

    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"n", (char *)n, 256);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"e", (char *)e, 256);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"h", (char *)h, 256);
    ELBA_BARCO_API_PARAM_HEXDUMP((char *)"s", (char *)s, 256);

    /* Setup params in the key memory */
    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_HBM_MEM_512B,
                               NULL, &key_param_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for key param");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for key param @ 0x%lx", key_param_addr);

    curr_ptr = key_param_addr;

    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)n, 256)) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write RSA param n into key memory @ 0x%lx", (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    curr_ptr += 256;

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                               NULL, &key_dma_descr_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for key DMA Descr");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for key DMA Descr @ 0x%lx", key_dma_descr_addr);

    /* Setup key DMA descriptor */
    key_dma_descr.address = key_param_addr;
    key_dma_descr.stop = 1;
    key_dma_descr.rsvd0 = 1;
    key_dma_descr.next = 0;
    key_dma_descr.int_en = 0;
    key_dma_descr.discard = 0;
    key_dma_descr.realign = 1;
    key_dma_descr.cst_addr = 0;
    key_dma_descr.length = (curr_ptr - key_param_addr);
    if (sdk::asic::asic_mem_write(key_dma_descr_addr, (uint8_t*)&key_dma_descr,
                                  sizeof(key_dma_descr))) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write key DMA Descr @ 0x%lx",
                (uint64_t) key_dma_descr_addr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }

    ret = elba_barco_asym_alloc_key(&ecc_p256_key_idx);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate key descriptor");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated Key Descr @ %d",
                    ecc_p256_key_idx);

    asym_key.key_param_list = key_dma_descr_addr;
    asym_key.command_reg = (ELBA_BARCO_ASYM_CMD_SWAP_BYTES |
                            ELBA_BARCO_ASYM_CMD_SIZE_OF_OP(256) |
                            ELBA_BARCO_ASYM_CMD_RSA_SIG_VERIFY);

    ret = elba_barco_asym_write_key(ecc_p256_key_idx, &asym_key);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write key: %d",
                      ecc_p256_key_idx);
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Setup key @ %d", ecc_p256_key_idx);

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                               NULL, &ilist_dma_descr_addr1);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for ilist DMA Descr1");
        goto cleanup;
    }
    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                               NULL, &ilist_dma_descr_addr2);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for ilist DMA Descr2");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for ilist DMA Descr @ 0x%lx & 0x%lx",
            ilist_dma_descr_addr1, ilist_dma_descr_addr2);

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_HBM_MEM_512B,
                               NULL, &ilist_mem_addr1);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for ilist content 1");
        goto cleanup;
    }
    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_HBM_MEM_512B,
                               NULL, &ilist_mem_addr2);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for ilist content 2");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for input mem @ 0x%lx & 0x%lx",
            ilist_mem_addr1, ilist_mem_addr2);

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_HBM_MEM_512B,
                               NULL, &olist_mem_addr);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to allocate memory for olist content");
        goto cleanup;
    }
    SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "Allocated memory for output mem @ 0x%lx", olist_mem_addr);

    /* Copy the input to the ilist memory */
    curr_ptr = ilist_mem_addr1;

    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)e, 256)) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write RSA param e into key memory @ 0x%lx", (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    curr_ptr += 256;


    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)s, 256)) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write RSA param s into ilist memory @ 0x%lx", (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    curr_ptr += 256;

    /* Setup ilist DMA descriptor */
    ilist_dma_descr.address = ilist_mem_addr1;
    ilist_dma_descr.stop = 0;
    ilist_dma_descr.rsvd0 = 1;
    ilist_dma_descr.next = (ilist_dma_descr_addr2 >> 2);
    ilist_dma_descr.int_en = 0;
    ilist_dma_descr.discard = 0;
    ilist_dma_descr.realign = 0;
    ilist_dma_descr.cst_addr = 0;
    ilist_dma_descr.length = (curr_ptr - ilist_mem_addr1);
    if (sdk::asic::asic_mem_write(ilist_dma_descr_addr1,
                                  (uint8_t*)&ilist_dma_descr,
                                  sizeof(ilist_dma_descr))) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write ilist DMA Descr @ 0x%lx",
                (uint64_t) ilist_dma_descr_addr1);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }


    curr_ptr = ilist_mem_addr2;
    if (sdk::asic::asic_mem_write(curr_ptr, (uint8_t*)h, 256)) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write RSA param h into ilist memory @ 0x%lx", (uint64_t) curr_ptr);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }
    curr_ptr += 256;

    ilist_dma_descr.address = ilist_mem_addr2;
    ilist_dma_descr.stop = 1;
    ilist_dma_descr.rsvd0 = 1;
    ilist_dma_descr.next = 0;
    ilist_dma_descr.int_en = 0;
    ilist_dma_descr.discard = 0;
    ilist_dma_descr.realign = 1;
    ilist_dma_descr.cst_addr = 0;
    ilist_dma_descr.length = (curr_ptr - ilist_mem_addr2);
    if (sdk::asic::asic_mem_write(ilist_dma_descr_addr2,
                                  (uint8_t*)&ilist_dma_descr,
                                  sizeof(ilist_dma_descr))) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to write ilist DMA Descr @ 0x%lx",
                (uint64_t) ilist_dma_descr_addr2);
        ret = SDK_RET_INVALID_ARG;
        goto cleanup;
    }

    /* Setup Asymmetric Request Descriptor */
    asym_req_descr.input_list_addr = ilist_dma_descr_addr1;
    asym_req_descr.output_list_addr = 0;
    asym_req_descr.key_descr_idx = ecc_p256_key_idx;
    asym_req_descr.status_addr = olist_mem_addr + 256;
    asym_req_descr.opaque_tag_value = 0;
    asym_req_descr.opage_tag_wr_en = 0;
    asym_req_descr.flag_a = 0;
    asym_req_descr.flag_b = 0;

    ret = elba_barco_ring_queue_request(BARCO_RING_ASYM,
                                        (void *)&asym_req_descr,
                                        &req_tag, true);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to enqueue request");
        ret = SDK_RET_ERR;
        goto cleanup;
    }
    /* Poll for completion */
    while (elba_barco_ring_poll(BARCO_RING_ASYM, req_tag) != TRUE) {
        //SDK_TRACE_DEBUG("ECC Point Mul P256: Waiting for Barco completion...");
    }
    if (sdk::asic::asic_mem_read(asym_req_descr.status_addr,
                                 (uint8_t*)&status, sizeof(status))) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to retrieve operation status @ 0x%lx",
                (uint64_t) asym_req_descr.status_addr);
        ret = SDK_RET_ERR;
        goto cleanup;
    }
    if (status != 0) {
        SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Operation failed with status %u",
                      status);
        ret = SDK_RET_ERR;
        goto cleanup;
    }
    else {
        SDK_TRACE_DEBUG(ELBA_BARCO_API_NAME "RSA Sig Verify succeeded");
    }

cleanup:
    if (olist_mem_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_HBM_MEM_512B,
                                  olist_mem_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for olist content:0x%lx",
                    olist_mem_addr);
        }
    }

    if (ilist_mem_addr2) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_HBM_MEM_512B,
                                  ilist_mem_addr2);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR("ECC Point Mul P256: Failed to free memory for ilist content: 0x%lx",
                    ilist_mem_addr2);
        }
    }

    if (ilist_mem_addr1) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_HBM_MEM_512B,
                                  ilist_mem_addr1);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR("ECC Point Mul P256: Failed to free memory for ilist content:0x%lx",
                          ilist_mem_addr1);
        }
    }

    if (ilist_dma_descr_addr2) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                                  ilist_dma_descr_addr2);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for ilist DMA Descr: 0x%lx",
                          ilist_dma_descr_addr2);
        }
    }

    if (ilist_dma_descr_addr1) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                                  ilist_dma_descr_addr1);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for ilist DMA Descr: 0x%lx",
                          ilist_dma_descr_addr1);
        }
    }

    if (ecc_p256_key_idx != -1) {
        ret = elba_barco_asym_free_key(ecc_p256_key_idx);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME"Failed to free key descriptor");
        }
    }

    if (key_dma_descr_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_ASYM_DMA_DESCR,
                                  key_dma_descr_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for key DMA Descr: 0x%lx",
                          key_dma_descr_addr);
        }
    }

    if (key_param_addr) {
        ret = elba_barco_res_free(CRYPTO_BARCO_RES_HBM_MEM_512B,
                                  key_param_addr);
        if (ret != SDK_RET_OK) {
            SDK_TRACE_ERR(ELBA_BARCO_API_NAME "Failed to free memory for key param :0x%lx",
                          key_param_addr);
        }
    }

    if (status)
        ret = SDK_RET_ERR;

    return ret;
}

}    // namespace elba
}    // namespace platform
}    // namespace sdk
