// {C} Copyright 2020 Pensando Systems Inc. All rights reserved

#include <unistd.h>
#include <string>
#include <sstream>
#include <ostream>
#include <iomanip>

/* Barco Crypto initialization support */
#include "include/sdk/crypto_apis.hpp"
#include "third-party/asic/elba/model/elb_top/elb_top_csr.h"
#include "platform/elba/elba_barco_crypto.hpp"
#include "platform/elba/elba_barco_sym_apis.hpp"
#include "platform/elba/elba_barco_res.hpp"
#include "lib/indexer/indexer.hpp"
#include "asic/rw/asicrw.hpp"
#include "asic/cmn/asic_hbm.hpp"

using namespace sdk::asic;

namespace sdk {
namespace platform {
namespace elba {

char key_desc_array[] = ELBA_BARCO_KEY_DESC;

uint64_t    key_desc_array_base = 0;

// byte array to hex string for logging
std::string
barco_hex_dump (const uint8_t *buf, size_t sz)
{
    std::ostringstream result;

    for(size_t i = 0; i < sz; i+=8) {
        result << " 0x";
        for (size_t j = i ; j < sz && j < i+8; j++) {
            result << std::setw(2) << std::setfill('0') <<
                std::hex << (int)buf[j];
        }
    }

    return result.str();
}

static uint64_t    key_mem_base = 0;
static uint64_t    key_mem_size = 0;
static char        key_mem[] = ELBA_BARCO_KEY_MEM;
static indexer    *elba_barco_sym_keys_idxr_ = NULL;

sdk_ret_t
elba_barco_sym_key_init (void)
{
    sdk_ret_t           ret = SDK_RET_OK;
    uint32_t            region_sz = 0;

    key_mem_base = asic_get_mem_addr(key_mem);
    region_sz = asic_get_mem_size_kb(key_mem) * 1024;
    key_mem_size = region_sz / CRYPTO_SYM_KEY_SIZE_MAX;
    assert(key_mem_size >= CRYPTO_KEY_COUNT_MAX);

    // Indexer based allocator to manage the crypto session keys
    elba_barco_sym_keys_idxr_ =
        sdk::lib::indexer::factory(CRYPTO_KEY_COUNT_MAX);
    SDK_ASSERT_RETURN((elba_barco_sym_keys_idxr_ != NULL),
                      SDK_RET_NO_RESOURCE);

    return ret;
}

sdk_ret_t
elba_barco_crypto_init (platform_type_t platform)
{
    sdk_ret_t ret = SDK_RET_OK;
    elb_top_csr_t &cap0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    elb_hens_csr_t &hens = cap0.md.hens;

    /* Barco reset */
    hens.cfg_he_ctl.sw_rst(0xff);
    hens.cfg_he_ctl.write();
    hens.cfg_he_ctl.sw_rst(0);
    hens.cfg_he_ctl.write();

    key_desc_array_base = asic_get_mem_addr(key_desc_array);
    if (key_desc_array_base == INVALID_MEM_ADDRESS) {
        /* For non IRIS scenarios, the region may not be defined
         * in that case bail out silently
         */
        return ret;
    }

    /* All regions in hbm_mem.json are in multiples of 1kb and hence should already be aligned to 16byte
     * but confirm
     */
    assert((key_desc_array_base &
            (BARCO_CRYPTO_KEY_DESC_ALIGN_BYTES - 1)) == 0);

    ret = elba_barco_res_allocator_init();
    if (ret != SDK_RET_OK) {
        return ret;
    }

    ret = elba_barco_rings_init(platform);
    if (ret != SDK_RET_OK) {
        return ret;
    }

    /*
     * Initialize the padding pattern table for use with TLS-proxy.
     */
    ret = elba_barco_crypto_init_tls_pad_table();
    if (ret != SDK_RET_OK) {
        return ret;
    }

    ret = elba_barco_sym_key_init();
    if (ret != SDK_RET_OK) {
        return ret;
    }

#if 0
    sdk_ret_t elba_barco_asym_run_tests(void);
    ret = elba_barco_asym_run_tests();
#endif

#if 0
    sdk_ret_t elba_barco_sym_hash_run_tests(void);
    ret = elba_barco_sym_hash_run_tests();
#endif

#if 0
    sdk_ret_t elba_barco_sym_run_aes_gcm_1K_test(void);
    ret = elba_barco_sym_run_aes_gcm_1K_test();
#endif

    /*
     * Initialize the barco DRBG random number generator.
     */
    if (platform != platform_type_t::PLATFORM_TYPE_HAPS) {
        /* HAPS does not contain the DRBG block */
        elba_barco_init_drbg();
    }

    return ret;
}

sdk_ret_t
elba_barco_init_key (uint32_t key_idx, uint64_t key_addr)
{
    elba_barco_key_desc_t  key_desc;
    uint64_t                key_desc_addr;

    key_desc_addr = key_desc_array_base + (key_idx * BARCO_CRYPTO_KEY_DESC_SZ);
    memset(&key_desc, 0, sizeof(elba_barco_key_desc_t));
    key_desc.key_address = key_addr;
    if (sdk::asic::asic_mem_write(key_desc_addr, (uint8_t*)&key_desc,
                                  sizeof(key_desc))) {
        SDK_TRACE_ERR("Failed to write Barco descriptor @ 0x%lx",
                      (uint64_t) key_desc_addr);
        return SDK_RET_INVALID_ARG;
    }
    return SDK_RET_OK;
}

sdk_ret_t
elba_barco_setup_key (uint32_t key_idx, crypto_key_type_t key_type,
                      uint8_t *key, uint32_t key_size)
{
    elba_barco_key_desc_t  key_desc;
    uint64_t                key_desc_addr = 0;
    uint64_t                key_addr = 0;
    uint32_t                cbkey_type;

    key_desc_addr = key_desc_array_base + (key_idx * BARCO_CRYPTO_KEY_DESC_SZ);
    SDK_TRACE_DEBUG("elba_barco_setup_key: key_desc_addr=0x%lx key_idx=%d",
                    key_desc_addr, key_idx);
    if (sdk::asic::asic_mem_read(key_desc_addr, (uint8_t*)&key_desc,
                                 sizeof(key_desc))) {
        SDK_TRACE_ERR("Failed to read Barco descriptor @ 0x%lx",
                      (uint64_t) key_desc_addr);
        return SDK_RET_INVALID_ARG;
    }

    switch (key_type) {
        case CRYPTO_KEY_TYPE_AES128:
            cbkey_type = ELBA_BARCO_KEYTYPE_AES128;
            break;
        case CRYPTO_KEY_TYPE_AES192:
            cbkey_type = ELBA_BARCO_KEYTYPE_AES192;
            break;
        case CRYPTO_KEY_TYPE_AES256:
            cbkey_type = ELBA_BARCO_KEYTYPE_AES256;
            break;
        case CRYPTO_KEY_TYPE_DES:
            cbkey_type = ELBA_BARCO_KEYTYPE_DES;
            break;
        case CRYPTO_KEY_TYPE_CHACHA20:
            cbkey_type = ELBA_BARCO_KEYTYPE_CHACHA20;
            break;
        case CRYPTO_KEY_TYPE_POLY1305:
            cbkey_type = ELBA_BARCO_KEYTYPE_POY1305;
            break;
        case CRYPTO_KEY_TYPE_HMAC:
            cbkey_type = ELBA_BARCO_KEYTYPE_HMAC(key_size);
            break;
        default:
            return SDK_RET_INVALID_ARG;
    }

    key_desc.key_type = cbkey_type;
    key_addr = key_desc.key_address;
    SDK_TRACE_DEBUG("elba_barco_setup_key key_addr=0x%lx", (uint64_t)key_addr);
    /* Write back key descriptor */
    if (sdk::asic::asic_mem_write(key_desc_addr, (uint8_t*)&key_desc,
                                  sizeof(key_desc))) {
        SDK_TRACE_ERR("Failed to write Barco descriptor @ 0x%lx",
                      (uint64_t) key_desc_addr);
        return SDK_RET_INVALID_ARG;
    }
    SDK_TRACE_DEBUG("elba_barco_setup_key key=%s",
                    barco_hex_dump(key,key_size).c_str());
    /* Write key memory */
    if (sdk::asic::asic_mem_write(key_addr, key, key_size)) {
        SDK_TRACE_ERR("Failed to write key @ 0x%lx", (uint64_t) key_addr);
        return SDK_RET_INVALID_ARG;
    }
    return SDK_RET_OK;
}

sdk_ret_t
elba_barco_read_key (uint32_t key_idx, crypto_key_type_t *key_type,
                     uint8_t *key, uint32_t *key_size)
{
    elba_barco_key_desc_t  key_desc;
    uint64_t                key_desc_addr = 0;
    uint64_t                key_addr = 0;
    uint32_t                cbkey_type;

    key_desc_addr = key_desc_array_base + (key_idx * BARCO_CRYPTO_KEY_DESC_SZ);
    if (sdk::asic::asic_mem_read(key_desc_addr, (uint8_t*)&key_desc,
                                 sizeof(key_desc))) {
        SDK_TRACE_ERR("Failed to read Barco descriptor @ 0x%lx",
                      (uint64_t) key_desc_addr);
        return SDK_RET_INVALID_ARG;
    }

    cbkey_type = key_desc.key_type;
    SDK_TRACE_DEBUG("cbkey_type: %u", cbkey_type);
    switch (cbkey_type) {
        case ELBA_BARCO_KEYTYPE_AES128:
            *key_type = CRYPTO_KEY_TYPE_AES128;
            *key_size = 16;
            break;
        case ELBA_BARCO_KEYTYPE_AES192:
            *key_type = CRYPTO_KEY_TYPE_AES192;
            *key_size = 24;
            break;
        case ELBA_BARCO_KEYTYPE_AES256:
            *key_type = CRYPTO_KEY_TYPE_AES256;
            *key_size = 32;
            break;
        case ELBA_BARCO_KEYTYPE_DES:
            *key_type = CRYPTO_KEY_TYPE_DES;
            *key_size = 16; /* FIXME */
            break;
        case ELBA_BARCO_KEYTYPE_CHACHA20:
            *key_type = CRYPTO_KEY_TYPE_CHACHA20;
            *key_size = 16; /* FIXME */
            break;
        case ELBA_BARCO_KEYTYPE_POY1305:
            *key_type = CRYPTO_KEY_TYPE_POLY1305;
            *key_size = 16; /* FIXME */
            break;
        default:
            if (cbkey_type & ELBA_BARCO_KEYTYPE_HMAC_TYPE) {
                *key_type = CRYPTO_KEY_TYPE_HMAC;
                *key_size = (cbkey_type & ELBA_BARCO_KEYTYPE_HMAC_LEN_MASK);
            }
            else {
                return SDK_RET_INVALID_ARG;
            }
            break;
    }

    key_addr = key_desc.key_address;
    if (sdk::asic::asic_mem_read(key_addr, key, *key_size)) {
        SDK_TRACE_ERR("Failed to read key @ 0x%lx", (uint64_t) key_addr);
        return SDK_RET_INVALID_ARG;
    }

    return SDK_RET_OK;
}

sdk_ret_t
elba_barco_sym_free_key (int32_t key_idx)
{
    sdk_ret_t ret = SDK_RET_OK;
    indexer::status is = indexer::SUCCESS;

    if (!elba_barco_sym_keys_idxr_->is_index_allocated(key_idx)) {
        SDK_TRACE_ERR("SessKey: Freeing non-allocated key: %d", key_idx);
        return SDK_RET_INVALID_ARG;
    }

    is = elba_barco_sym_keys_idxr_->free(key_idx);
    if (is != indexer::SUCCESS) {
        SDK_TRACE_ERR("SessKey: Failed to free key memory: %d", key_idx);
        return SDK_RET_INVALID_ARG;
    }

    SDK_TRACE_DEBUG("SessKey:%s: Freed key memory @ index: %d",
                    __FUNCTION__, key_idx);
    return ret;
}

sdk_ret_t
elba_barco_crypto_init_tls_pad_table (void)
{
    uint8_t  tls_pad_bytes[asic_get_mem_size_kb(ASIC_HBM_REG_TLS_PROXY_PAD_TABLE) * 1024],
        i, j;
    uint64_t tls_pad_base_addr = 0;

    SDK_TRACE_DEBUG("Initializing TLS-proxy Pad Bytes table of size 0x%lx",
                    sizeof(tls_pad_bytes));
    bzero(tls_pad_bytes, sizeof(tls_pad_bytes));

    /*
     * Block-size 16.
     * Pad bytes pattern:
     *   - 16 contiguous blocks with 1st block all 0s, 2nd block all 1s etc.
     */
    for (i = 0; i < 16; i++) {
        for (j = 0; j < 16; j++) {
            tls_pad_bytes[16*(i) + j] = i;
        }
    }

    tls_pad_base_addr = asic_get_mem_addr(ASIC_HBM_REG_TLS_PROXY_PAD_TABLE);
    if (tls_pad_base_addr != INVALID_MEM_ADDRESS) {
        sdk::asic::asic_mem_write(tls_pad_base_addr, tls_pad_bytes,
                                  sizeof(tls_pad_bytes));
    }
    return SDK_RET_OK;
}

sdk_ret_t
elba_barco_asym_alloc_key (int32_t *key_idx)
{
    uint64_t key_desc;
    sdk_ret_t ret = SDK_RET_OK;

    ret = elba_barco_res_alloc(CRYPTO_BARCO_RES_ASYM_KEY_DESCR, key_idx,
                                &key_desc);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR("SessKey: Failed to allocate key memory");
        *key_idx = -1;
        return SDK_RET_NO_RESOURCE;
    }
    return ret;
}

sdk_ret_t
elba_barco_sym_alloc_key (int32_t *key_idx)
{
    sdk_ret_t ret = SDK_RET_OK;
    indexer::status is = indexer::SUCCESS;
    uint64_t key_addr = 0;

    is = elba_barco_sym_keys_idxr_->alloc((uint32_t *)key_idx);
    if (is != indexer::SUCCESS) {
        SDK_TRACE_ERR("SessKey: Failed to allocate key memory");
        *key_idx = -1;
        return SDK_RET_NO_RESOURCE;
    }
    /* Setup the key descriptor with the corresponding key memory
    *  Currently statically carved and associated
    */
    key_addr = key_mem_base + (*key_idx * CRYPTO_SYM_KEY_SIZE_MAX);
    elba_barco_init_key(*key_idx, key_addr);
    SDK_TRACE_DEBUG("SessKey:%s: Allocated key memory @ index: %d",
                    __FUNCTION__, *key_idx);
    return ret;
}

sdk_ret_t
elba_barco_sym_alloc_key_withid (int32_t key_idx, bool allow_dup_alloc)
{
    sdk_ret_t           ret = SDK_RET_OK;
    indexer::status     is = indexer::SUCCESS;
    uint64_t            key_addr = 0;

    is = elba_barco_sym_keys_idxr_->alloc_withid(key_idx);
    if (is != indexer::SUCCESS) {
        SDK_TRACE_ERR("SessKey: Failed to allocate key memory");
        return SDK_RET_NO_RESOURCE;
    }
    /* Setup the key descriptor with the corresponding key memory
    *  Currently statically carved and associated
    */
    key_addr = key_mem_base + (key_idx * CRYPTO_SYM_KEY_SIZE_MAX);
    elba_barco_init_key(key_idx, key_addr);
    SDK_TRACE_DEBUG("SessKey:%s: Allocated key memory @ index: %d",
                    __FUNCTION__, key_idx);
    return ret;
}

sdk_ret_t elba_barco_asym_free_key(int32_t key_idx)
{
    sdk_ret_t ret = SDK_RET_OK;

    /* TODO: Also free up the DMA descriptor and corresponding memory regions
     * if any referenced by the key descriptor
     */

    ret = elba_barco_res_free_by_id(CRYPTO_BARCO_RES_ASYM_KEY_DESCR, key_idx);
    if (ret != SDK_RET_OK) {
        SDK_TRACE_ERR("AsymKey: Failed to free key memory: %d", key_idx);
        return SDK_RET_INVALID_ARG;
    }

    return ret;
}

sdk_ret_t
elba_barco_asym_read_key (int32_t key_idx, elba_barco_asym_key_desc_t *key)
{
    sdk_ret_t ret = SDK_RET_OK;
    uint64_t key_desc_addr = 0;

    ret = elba_barco_res_get_by_id(CRYPTO_BARCO_RES_ASYM_KEY_DESCR, key_idx,
                                    &key_desc_addr);
    if (ret  != SDK_RET_OK) {
        SDK_TRACE_ERR("AsymKey Read: Failed to retrieve the address from key index: %d",
                      key_idx);
        return ret;
    }

    if (asic_mem_read(key_desc_addr, (uint8_t*)&key, sizeof(*key))) {
        SDK_TRACE_ERR("Failed to read Barco Asym key descriptor from 0x%lx",
                      (uint64_t) key_desc_addr);
        return SDK_RET_INVALID_ARG;
    }

    return ret;
}

sdk_ret_t
elba_barco_asym_write_key(int32_t key_idx, elba_barco_asym_key_desc_t *key)
{
    sdk_ret_t ret = SDK_RET_OK;
    uint64_t key_desc_addr = 0;

    ret = elba_barco_res_get_by_id(CRYPTO_BARCO_RES_ASYM_KEY_DESCR, key_idx,
                                    &key_desc_addr);
    if (ret  != SDK_RET_OK) {
        SDK_TRACE_ERR("AsymKey Write: Failed to retrieve the address from key index: %d",
                      key_idx);
        return ret;
    }

    if (asic_mem_write(key_desc_addr, (uint8_t*)key, sizeof(*key))) {
        SDK_TRACE_ERR("Failed to write Barco Asym key descriptor @ 0x%lx",
                      (uint64_t) key_desc_addr);
        return SDK_RET_INVALID_ARG;
    }
    SDK_TRACE_DEBUG("AsymKey Write: Setup key @ 0x%lx", key_desc_addr);

    return ret;
}

}    // namespace elba
}    // namespace platform
}    // namespace sdk
