#include "nic/hal/pd/iris/hal_state_pd.hpp"
#include "nic/sdk/asic/cmn/asic_hbm.hpp"
#include "nic/include/pd_api.hpp"
#include "nic/hal/pd/iris/internal/p4plus_pd_api.h"
//#ifdef BARCO    /* TBD-ELBA-REBASE: */
#ifdef ELBA
  #include "nic/sdk/platform/elba/elba_barco_crypto.hpp"
  #include "nic/sdk/platform/elba/elba_barco_res.hpp"
  #include "nic/sdk/platform/elba/elba_barco_sym_apis.hpp"
#else
  #include "nic/sdk/platform/capri/capri_barco_crypto.hpp"
  #include "nic/sdk/platform/capri/capri_barco_res.hpp"
  #include "nic/sdk/platform/capri/capri_barco_sym_apis.hpp"
#endif

#define MAX_IPSEC_PAD_SIZE 256

//#ifndef BARCO    /* TBD-ELBA-REBASE: */
//typedef struct elba_barco_asym_key_desc_s {
//    uint64_t                key_param_list;
//    uint32_t                command_reg;
//    uint32_t                reserved;
//} __PACK__ elba_barco_asym_key_desc_t;
//#endif

namespace hal {
namespace pd {

hal_ret_t
pd_crypto_alloc_key (pd_func_args_t *pd_func_args)
{
    sdk_ret_t              sdk_ret;
    hal_ret_t           ret = HAL_RET_OK;
    pd_crypto_alloc_key_args_t *args = pd_func_args->pd_crypto_alloc_key;
    int32_t             *key_idx = args->key_idx;

#ifdef ELBA
    sdk_ret = elba_barco_sym_alloc_key(key_idx);
#else
    sdk_ret = capri_barco_sym_alloc_key(key_idx);
#endif

    ret = hal_sdk_ret_to_hal_ret(sdk_ret);
    if (ret != HAL_RET_OK) {
        HAL_TRACE_ERR("SessKey: Failed to allocate key");
        return ret;
    }

    HAL_TRACE_DEBUG("SessKey:{}: Allocated key memory @ index: {}",
                    __FUNCTION__, *key_idx);
    return ret;
}

hal_ret_t
pd_crypto_alloc_key_withid (pd_func_args_t *pd_func_args)
{
    sdk_ret_t              sdk_ret;
    hal_ret_t           ret = HAL_RET_OK;
    pd_crypto_alloc_key_withid_args_t *args =
        pd_func_args->pd_crypto_alloc_key_withid;
    int32_t             key_idx = args->key_idx;

    #ifdef ELBA
      sdk_ret = elba_barco_sym_alloc_key_withid(key_idx, args->allow_dup_alloc);
    #else
      sdk_ret = capri_barco_sym_alloc_key_withid(key_idx, args->allow_dup_alloc);
    #endif


    ret = hal_sdk_ret_to_hal_ret(sdk_ret);
    if (ret != HAL_RET_OK) {
        HAL_TRACE_ERR("SessKey: Failed to allocate key by ID: {}", key_idx);
        return ret;
    }
    HAL_TRACE_DEBUG("SessKey:{}: Allocated key memory @ index: {}",
                    __FUNCTION__, key_idx);
    return ret;
}

hal_ret_t
pd_crypto_free_key (pd_func_args_t *pd_func_args)
{
    sdk_ret_t              sdk_ret;
    hal_ret_t           ret = HAL_RET_OK;
    pd_crypto_free_key_args_t *args = pd_func_args->pd_crypto_free_key;
    int32_t             key_idx = args->key_idx;

   #ifdef ELBA
     sdk_ret = elba_barco_sym_free_key(key_idx);
   #else
     sdk_ret = capri_barco_sym_free_key(key_idx);
   #endif

    ret = hal_sdk_ret_to_hal_ret(sdk_ret);
    if (ret != HAL_RET_OK) {
        HAL_TRACE_ERR("SessKey: Failed to free key memory: {}", key_idx);
        return ret;
    }

    HAL_TRACE_DEBUG("SessKey:{}: Freed key memory @ index: {}",
                    __FUNCTION__, key_idx);
    return ret;
}

hal_ret_t
pd_crypto_write_key (pd_func_args_t *pd_func_args)
{
    sdk_ret_t              sdk_ret;
    hal_ret_t           ret = HAL_RET_OK;
    pd_crypto_write_key_args_t *args = pd_func_args->pd_crypto_write_key;
    int32_t key_idx = args->key_idx;
    crypto_key_t *key = args->key;

    #ifdef ELBA
      sdk_ret = elba_barco_setup_key(key_idx, (crypto_key_type_t)key->key_type,
                                    key->key, key->key_size);
    #else
      sdk_ret = capri_barco_setup_key(key_idx, (crypto_key_type_t)key->key_type,
                                    key->key, key->key_size);
    #endif
    ret = hal_sdk_ret_to_hal_ret(sdk_ret);

    return ret;
}

hal_ret_t
pd_crypto_read_key (pd_func_args_t *pd_func_args)
{
    sdk_ret_t              sdk_ret;
    hal_ret_t           ret = HAL_RET_OK;
    pd_crypto_read_key_args_t *args = pd_func_args->pd_crypto_read_key;
    int32_t key_idx = args->key_idx;
    crypto_key_t *key = args->key;

    #ifdef ELBA
      sdk_ret = elba_barco_read_key(key_idx, (crypto_key_type_t*)&key->key_type,
                                   key->key, &key->key_size);
    #else
      sdk_ret = capri_barco_read_key(key_idx, (crypto_key_type_t*)&key->key_type,
                                   key->key, &key->key_size);
    #endif

    ret = hal_sdk_ret_to_hal_ret(sdk_ret);

    return ret;
}

hal_ret_t
pd_crypto_asym_alloc_key (pd_func_args_t *pd_func_args)

{
    sdk_ret_t              sdk_ret;
    pd_crypto_asym_alloc_key_args_t *args =
        pd_func_args->pd_crypto_asym_alloc_key;
    hal_ret_t       ret = HAL_RET_OK;
    int32_t *key_idx = args->key_idx;

    #ifdef ELBA
      sdk_ret = elba_barco_asym_alloc_key(key_idx);
    #else
      sdk_ret = capri_barco_asym_alloc_key(key_idx);
    #endif

    ret = hal_sdk_ret_to_hal_ret(sdk_ret);
    if (ret != HAL_RET_OK) {
        HAL_TRACE_ERR("SessKey: Failed to allocate key memory");
        *key_idx = -1;
        return ret;
    }
    return ret;
}

hal_ret_t
pd_crypto_asym_free_key (pd_func_args_t *pd_func_args)
{
    sdk_ret_t              sdk_ret;
    hal_ret_t           ret = HAL_RET_OK;
    pd_crypto_asym_free_key_args_t *args =
        pd_func_args->pd_crypto_asym_free_key;
    int32_t key_idx = args->key_idx;

    /* TODO: Also free up the DMA descriptor and corresponding memory regions
     * if any referenced by the key descriptor
     */

   #ifdef ELBA
     sdk_ret = elba_barco_asym_free_key(key_idx);
   #else
     sdk_ret = capri_barco_asym_free_key(key_idx);
   #endif

    ret = hal_sdk_ret_to_hal_ret(sdk_ret);
    if (ret != HAL_RET_OK) {
        HAL_TRACE_ERR("AsymKey: Failed to free key memory: {}", key_idx);
        return ret;
    }

    return ret;
}

hal_ret_t
pd_crypto_asym_write_key (pd_func_args_t *pd_func_args)
{
    sdk_ret_t              sdk_ret;
    hal_ret_t              ret = HAL_RET_OK;
    pd_crypto_asym_write_key_args_t *args =
        pd_func_args->pd_crypto_asym_write_key;
#ifdef ELBA
    elba_barco_asym_key_desc_t     key_desc;
#else
    capri_barco_asym_key_desc_t     key_desc;
#endif
    int32_t key_idx = args->key_idx;
    crypto_asym_key_t *key = args->key;

    key_desc.key_param_list = key->key_param_list;
    key_desc.command_reg = key->command_reg;

    #ifdef ELBA
      sdk_ret = elba_barco_asym_write_key(key_idx, &key_desc);
    #else
      sdk_ret = capri_barco_asym_write_key(key_idx, &key_desc);
    #endif

    ret = hal_sdk_ret_to_hal_ret(sdk_ret);
    if (ret != HAL_RET_OK) {
        HAL_TRACE_ERR("Failed to write Barco Asym key descriptor @ {}",
                      key_idx);
        return ret;
    }
    HAL_TRACE_DEBUG("AsymKey Write: Setup key @ {}", key_idx);

    return ret;
}

hal_ret_t
pd_crypto_asym_read_key (pd_func_args_t *pd_func_args)
{
    sdk_ret_t              sdk_ret;
    hal_ret_t              ret = HAL_RET_OK;
    pd_crypto_asym_read_key_args_t *args = pd_func_args->pd_crypto_asym_read_key;
#ifdef ELBA
    elba_barco_asym_key_desc_t     key_desc;
#else
    capri_barco_asym_key_desc_t     key_desc;
#endif
    int32_t key_idx = args->key_idx;
    crypto_asym_key_t *key = args->key;

    #ifdef ELBA
      sdk_ret = elba_barco_asym_read_key(key_idx, &key_desc);
    #else
      sdk_ret = capri_barco_asym_read_key(key_idx, &key_desc);
    #endif

    ret = hal_sdk_ret_to_hal_ret(sdk_ret);
    if (ret != HAL_RET_OK) {
        HAL_TRACE_ERR("Failed to read Barco Asym key descriptor from {}", key_idx);
        return ret;
    }

    key->key_param_list = key_desc.key_param_list;
    key->command_reg = key_desc.command_reg;

    return ret;
}

hal_ret_t
crypto_init_ipsec_pad_table (void)
{
    uint8_t ipsec_pad_bytes[MAX_IPSEC_PAD_SIZE];
    uint64_t ipsec_pad_base_addr = 0;

    HAL_TRACE_DEBUG("Initializing IPSEC Pad Bytes table");
    // Increasing number pattern as per RFC 1-255
    for (int i = 0; i < MAX_IPSEC_PAD_SIZE; i++) {
        ipsec_pad_bytes[i] = i+1;
    }

    ipsec_pad_base_addr = asicpd_get_mem_addr(ASIC_HBM_REG_IPSEC_PAD_TABLE);
    if (ipsec_pad_base_addr != INVALID_MEM_ADDRESS) {
        p4plus_hbm_write(ipsec_pad_base_addr, ipsec_pad_bytes, MAX_IPSEC_PAD_SIZE,
                P4PLUS_CACHE_ACTION_NONE);
    }
    return HAL_RET_OK;
}

/* This is to support the Barco GCM decrypt bug workaround */
hal_ret_t
capri_barco_setup_dummy_ring_desc (void)
{
    sdk_ret_t              sdk_ret;
    hal_ret_t                           ret = HAL_RET_OK;
    pd_crypto_alloc_key_args_t          alloc_key_args;
    pd_func_args_t                      pd_func_args = {0};
    int32_t                             key_idx = -1;
    crypto_key_t                        pd_key;
    pd::pd_crypto_write_key_args_t      write_key_args;
#define CAPRI_BARCO_DUMMY_DEC_KEY_SZ    16
    uint8_t                             key[CAPRI_BARCO_DUMMY_DEC_KEY_SZ];

    /* Allocate key for dummy dec op */
    alloc_key_args.key_idx = &key_idx;
    pd_func_args.pd_crypto_alloc_key = &alloc_key_args;
    ret = pd_crypto_alloc_key(&pd_func_args);

    if ((ret != HAL_RET_OK) || (key_idx < 0)) {
        HAL_TRACE_ERR("Failed to allocate key for dummy decrypt op: {}", ret);
        return ret;
    }

    HAL_TRACE_DEBUG("Allocated key idx ({}) for dummy decrypt op", key_idx);

    /* Setup key for dummy dec op */
    pd_key.key_type = types::CRYPTO_KEY_TYPE_AES128;
    pd_key.key_size = CAPRI_BARCO_DUMMY_DEC_KEY_SZ;
    memset(key, 0, CAPRI_BARCO_DUMMY_DEC_KEY_SZ);
    memcpy((void*)pd_key.key, key, pd_key.key_size);
    write_key_args.key_idx = key_idx;
    write_key_args.key = &pd_key;
    pd_func_args.pd_crypto_write_key = &write_key_args;
    ret = pd_crypto_write_key(&pd_func_args);

    if (ret != HAL_RET_OK) {
        HAL_TRACE_ERR("Failed to setup key for dummy decrypt op: {}", ret);
        return ret;
    }
    HAL_TRACE_DEBUG("Setup AES128 key at idx ({}) for dummy decrypt op",
                    key_idx);

#ifdef BARCO    /* TBD-ELBA-REBASE: */
    sdk_ret = capri_barco_setup_dummy_gcm1_req(key_idx);
#else
    sdk_ret = SDK_RET_OK;
#endif
    ret = hal_sdk_ret_to_hal_ret(sdk_ret);
    if (ret != HAL_RET_OK) {
        HAL_TRACE_ERR("Failed to setup descriptor for dummy decrypt op: {}",
                      ret);
        return ret;
    }
    return ret;
}

hal_ret_t
crypto_pd_init (void)
{
    hal_ret_t           ret = HAL_RET_OK;

    ret = crypto_init_ipsec_pad_table();
    if (ret != HAL_RET_OK) {
        return ret;
    }
#ifdef BARCO    /* TBD-ELBA-REBASE: */
    ret = capri_barco_setup_dummy_ring_desc();
#else
    ret = HAL_RET_OK;
#endif
    if (ret != HAL_RET_OK) {
        return ret;
    }
    return ret;
}


}    // namespace pd
}    // namespace hal
