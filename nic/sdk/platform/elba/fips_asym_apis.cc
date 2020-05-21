#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/bn.h>
#include <openssl/err.h>
#include <openssl/engine.h>
#include "include/sdk/base.hpp"
#include "include/sdk/pse_intf.h"
#include "platform/elba/elba_barco_asym_apis.hpp"

namespace sdk {
namespace platform {
namespace elba {

/*
 * FIXME: Ideally we should have a unified initialization of the PSE engine done in one
 * central place and then used by everybody in HAL.
 * Currently the engine is initialized only when TLS proxy is initialized.
 * Hence ths code below...
 */
static ENGINE       *engine = NULL;
BIO                 *bio = NULL;

static int
fips_asym_rsa_sig_gen_param (void *ctx,
                             const PSE_RSA_SIGN_PARAM *param)
{
    return elba_barco_asym_rsa_sig_gen(param->key_size,
                                          param->key_idx,
                                          param->n,
                                          param->d,
                                          param->hash_input,
                                          param->sig_output,
                                          param->async,
                                          param->caller_unique_id);
}

int
fips_asym_rsa_encrypt_param (void *ctx,
                             const PSE_RSA_ENCRYPT_PARAM *param)
{
    return elba_barco_asym_rsa_encrypt(param->key_size,
                                          param->n,
                                          param->e,
                                          param->plain_input,
                                          param->ciphered_output,
                                          param->async,
                                          param->caller_unique_id);
}

int
fips_asym_rsa_decrypt_param (void *ctx,
                             const PSE_RSA_DECRYPT_PARAM *param)
{
    return elba_barco_asym_rsa2k_crt_decrypt(param->key_idx,
                                              param->p,
                                              param->q,
                                              param->dp,
                                              param->dq,
                                              param->qinv,
                                              param->ciphered_input,
                                              param->plain_output,
                                              param->async,
                                              param->caller_unique_id);
}

const static PSE_RSA_OFFLOAD_METHOD     offload_method =
{
    .sign               = fips_asym_rsa_sig_gen_param,
    .encrypt            = fips_asym_rsa_encrypt_param,
    .decrypt            = fips_asym_rsa_decrypt_param,
    .mem_method         = NULL,
    .rand_method        = NULL,
};

static sdk_ret_t
init_ssl(void)
{
    SSL_library_init();

    /*
     * Basic I/O is used to interface with, among other things,
     * Openssl ERR_print facilities.
     */
    bio = BIO_new_fp(stdout, BIO_NOCLOSE);
    return SDK_RET_OK;
}

static sdk_ret_t
init_pse_engine (void)
{
    const char        *eng_path;

    if(engine != NULL) {
        return SDK_RET_OK;
    }

    init_ssl();

    ENGINE_load_dynamic();
    ENGINE_load_builtin_engines();
    ENGINE_register_all_complete();

    engine = ENGINE_by_id("dynamic");
    if(engine == NULL) {
        SDK_TRACE_ERR("Failed to load dynamic engine");
        return SDK_RET_ERR;
    }

#if 0
    if (is_platform_type_hw() || is_platform_type_haps()) {
        eng_path = "/nic/lib/libtls_pse.so";
    } else {
        eng_path = "/sw/nic/build/x86_64/iris/lib/libtls_pse.so";
    }
#else
    eng_path = "/nic/lib/libtls_pse.so";
#endif
    SDK_TRACE_DEBUG("Loading pensando engine from path: %s", eng_path);

    if(!ENGINE_ctrl_cmd_string(engine, "SO_PATH", eng_path, 0)) {
       SDK_TRACE_ERR("SSL: SO_PATH pensando engine load failed!!");
       return SDK_RET_ERR;
    }

    if(!ENGINE_ctrl_cmd_string(engine, "ID", "pse", 0)) {
        SDK_TRACE_ERR("ID failed!!");
        return SDK_RET_ERR;
    }

    if(!ENGINE_ctrl_cmd_string(engine, "LOAD", NULL, 0)) {
        SDK_TRACE_ERR("ENGINE LOAD_ADD failed, err: %s",
            ERR_error_string(ERR_get_error(), NULL));
        return SDK_RET_ERR;
    }
    int ret = ENGINE_init(engine);
    SDK_TRACE_DEBUG("Successfully loaded OpenSSL Engine: %s init result: %d",
                            ENGINE_get_name(engine), ret);

    ENGINE_set_default_EC(engine);
    ENGINE_set_default_RSA(engine);

    return SDK_RET_OK;
}

#if 0
RSA *
rsa_setup_key (unsigned short modulus_len, char *n, char *e, char *d)
{
    int         ret = 0;
    RSA         *r = NULL;
    BIGNUM      *bn_n =  NULL, *bn_e = NULL, *bn_d = NULL;

    bn_n = BN_bin2bn(n, modulus_len, bn_n);
    bn_e = BN_bin2bn(e, modulus_len, bn_e);
    bn_d = BN_bin2bn(d, modulus_len, bn_d);

    r = RSA_new();
    if (!r) {
        return NULL;
    }
    printf("OSSL Ver: %lx\n", OPENSSL_VERSION_NUMBER);
#if OPENSSL_VERSION_NUMBER < 0x10100000L
    r->n = bn_n;
    r->e = bn_e;
    r->d = bn_d;
#else
    ret = RSA_set0_key(r, n, e, d);
    if (ret != 1) {
        printf("Failed to setup RSA key\n");
    }
#endif
    return r;
}

EVP_PKEY *
evp_key_setup(unsigned short modulus_len, char *n, char *e)
{
    RSA         *r;
    EVP_PKEY    *evp_pkey;

    r = rsa_setup_key(modulus_len, n, e, d);
    if (!r) {
        printf("Failed to create RSA key\n");
        return NULL;
    }

    evp_pkey = EVP_PKEY_new();
    if (!evp_pkey) {
        printf("Failed to allocate EVP_PKEY\n");
        return NULL;
    }
    EVP_PKEY_set1_RSA(evp_pkey, r);

    EVP_PKEY_print_public(bio, evp_pkey, 4, NULL);
    EVP_PKEY_print_private(bio, evp_pkey, 4, NULL);

    return evp_pkey;
}
#endif

int
compute_message_digest (const EVP_MD *md, const unsigned char *message,
                        size_t message_len, char *digest,
                        unsigned int *digest_len)
{
    EVP_MD_CTX *mdctx;

    if((mdctx = EVP_MD_CTX_create()) == NULL) {
        SDK_TRACE_ERR("Failed to create EVP_MD_CTX");
        return -1;
    }

    if(1 != EVP_DigestInit_ex(mdctx, md, NULL)) {
        SDK_TRACE_ERR("Failed to initialize EVP_MD_CTX");
        return -1;
    }

    if(1 != EVP_DigestUpdate(mdctx, message, message_len)) {
        SDK_TRACE_ERR("Failed in EVP_DigestUpdate");
        return -1;
    }

    if(1 != EVP_DigestFinal_ex(mdctx, (unsigned char *)digest, digest_len)) {
        SDK_TRACE_ERR("Failed in EVP_DigestFinal_ex");
        return -1;
    }

    EVP_MD_CTX_destroy(mdctx);

    return 0;
}

EVP_PKEY *
rsa_setup_key (ENGINE *engine, uint16_t key_size, int32_t key_idx,
               uint8_t *n, uint8_t *e)
{
    PSE_KEY             pse_key = {0};
    EVP_PKEY            *pkey = NULL;

    pse_key.type = EVP_PKEY_RSA;
    pse_key.u.rsa_key.sign_key_id = key_idx;
    pse_key.u.rsa_key.decrypt_key_id = key_idx;

    // n
    pse_key.u.rsa_key.rsa_n.len = key_size;
    pse_key.u.rsa_key.rsa_n.data = n;

    // e
    pse_key.u.rsa_key.rsa_e.len = key_size;
    pse_key.u.rsa_key.rsa_e.data = e;

    pse_key.u.rsa_key.offload.offload_method = &offload_method;

    pkey = ENGINE_load_private_key(engine,
                                   (const char *)&pse_key,
                                   NULL, NULL);
    if (!pkey) {
        SDK_TRACE_ERR("Failed to setup RSA key");
    }
    return pkey;
}

sdk_ret_t
elba_barco_asym_fips_rsa_sig_gen (uint16_t key_size, int32_t key_idx,
                                   uint8_t *n, uint8_t *e, uint8_t *msg,
                                   uint16_t msg_len, uint8_t *s,
                                   hash_type_t hash_type,
                                   rsa_signature_scheme_t sig_scheme,
                                   bool async_en, const uint8_t *unique_key)
{
    EVP_PKEY_CTX        *ctx = NULL;
    size_t              siglen = 0;
    sdk_ret_t           ret = SDK_RET_OK;
    int                 ossl_ret = 0;
    char                digest[128];
    unsigned int        digest_len = 0;
    EVP_PKEY            *evp_pkey = NULL;
    const EVP_MD        *md = NULL;

    if ((ret = init_pse_engine()) != SDK_RET_OK) {
        SDK_TRACE_ERR("Failed to initialize SSL engine");
        ret = SDK_RET_ERR;
        goto cleanup;
    }

    /* Setup key */
    evp_pkey = rsa_setup_key(engine, key_size, key_idx, n, e);
    if (!evp_pkey) {
        SDK_TRACE_ERR("Failed to setup RSA key");
        ret = SDK_RET_ERR;
        goto cleanup;
    }

    ctx = EVP_PKEY_CTX_new(evp_pkey, NULL);
    if (!ctx) {
        SDK_TRACE_ERR("Failed to allocate EVP_PKEY_CTX");
        ret = SDK_RET_NO_RESOURCE;
        ERR_print_errors(bio);
        goto cleanup;
    }
    if ((ossl_ret = EVP_PKEY_sign_init(ctx)) <= 0) {
        /* Error */
        SDK_TRACE_ERR("Failed to init EVP_PKEY_CTX, ret:%d", ossl_ret);
        ret = SDK_RET_ERR;
        goto cleanup;
    }
    if ((ossl_ret = EVP_PKEY_CTX_set_rsa_padding(ctx,
                                                 RSA_PKCS1_PADDING)) <= 0) {
        /* Error */
        SDK_TRACE_ERR("Failed to setup padding scheme in EVP_PKEY_CTX, ret:%d",
                      ossl_ret);
        ret = SDK_RET_ERR;
        goto cleanup;
    }
    switch (hash_type) {
        case SHA1:
            md = EVP_sha1();
            break;
        case SHA224:
            md = EVP_sha224();
            break;
        case SHA256:
            md = EVP_sha256();
            break;
        case SHA384:
            md = EVP_sha384();
            break;
        case SHA512:
            md = EVP_sha512();
            break;
        default:
            SDK_TRACE_ERR("Unsupported Hash Algo");
            ret = SDK_RET_INVALID_ARG;
            goto cleanup;
    }
    if ((ossl_ret = EVP_PKEY_CTX_set_signature_md(ctx, md)) <= 0) {
        /* Error */
        SDK_TRACE_ERR("Failed to setup hash scheme in EVP_PKEY_CTX, ret:%d",
                      ossl_ret);
        ret = SDK_RET_ERR;
        goto cleanup;
    }

    /* Determine buffer length */
    if ((ossl_ret = EVP_PKEY_sign(ctx, NULL, &siglen, msg, msg_len)) <= 0) {
        /* Error */
        SDK_TRACE_ERR("Failed to determine sig len, ret:%d", ossl_ret);
        ret = SDK_RET_ERR;
        goto cleanup;
    }
    SDK_TRACE_INFO("Sig Len: %ld", siglen);

    ELBA_BARCO_API_PARAM_HEXDUMP("Msg", (char*)msg, msg_len);
    /* Compute the digest of the message */
    if (compute_message_digest(md, msg, msg_len, digest, &digest_len)) {
        SDK_TRACE_ERR("Failed to compute the message digest");
        ret = SDK_RET_ERR;
        goto cleanup;
    }
    SDK_TRACE_INFO("Computed message digest of len: %d", digest_len);
    ELBA_BARCO_API_PARAM_HEXDUMP("Msg Digest", digest, digest_len);

    if ((ossl_ret = EVP_PKEY_sign(ctx, s, &siglen,
                                  (const unsigned char*) digest,
                                  digest_len)) <= 0) {
        /* Error */
        SDK_TRACE_ERR("Failed to generate sig, ret:%d", ossl_ret);
        ret = SDK_RET_ERR;
        goto cleanup;
    }
    ELBA_BARCO_API_PARAM_HEXDUMP("Sig", (char*)s, key_size);

cleanup:
    if (evp_pkey) {
        EVP_PKEY_free(evp_pkey);
    }

    if (ctx) {
        EVP_PKEY_CTX_free(ctx);
    }

    return ret;
}

sdk_ret_t
elba_barco_asym_fips_rsa_sig_verify (uint16_t key_size, uint8_t *n,
                                      uint8_t *e, uint8_t *msg,
                                      uint16_t msg_len, uint8_t *s,
                                      hash_type_t hash_type,
                                      rsa_signature_scheme_t sig_scheme,
                                      bool async_en, const uint8_t *unique_key)
{
    EVP_PKEY_CTX        *ctx = NULL;
    sdk_ret_t           ret = SDK_RET_OK;
    int                 ossl_ret = 0;
    char                digest[128];
    unsigned int        digest_len = 0;
    EVP_PKEY            *evp_pkey = NULL;
    const EVP_MD        *md = NULL;

    if ((ret = init_pse_engine()) != SDK_RET_OK) {
        SDK_TRACE_ERR("Failed to initialize SSL engine");
        ret = SDK_RET_ERR;
        goto cleanup;
    }

    /* Setup key */
    evp_pkey = rsa_setup_key(engine, key_size, -1, n, e);
    if (!evp_pkey) {
        SDK_TRACE_ERR("Failed to setup RSA key");
        ret = SDK_RET_ERR;
        goto cleanup;
    }

    ctx = EVP_PKEY_CTX_new(evp_pkey, NULL);
    if (!ctx) {
        SDK_TRACE_ERR("Failed to allocate EVP_PKEY_CTX");
        ret = SDK_RET_NO_RESOURCE;
        ERR_print_errors(bio);
        goto cleanup;
    }
    if ((ossl_ret = EVP_PKEY_verify_init(ctx)) <= 0) {
        /* Error */
        SDK_TRACE_ERR("Failed to init EVP_PKEY_CTX, ret:%d", ossl_ret);
        ret = SDK_RET_ERR;
        goto cleanup;
    }
    if ((ossl_ret = EVP_PKEY_CTX_set_rsa_padding(ctx,
                                                 RSA_PKCS1_PADDING)) <= 0) {
        /* Error */
        SDK_TRACE_ERR("Failed to setup padding scheme in EVP_PKEY_CTX, ret:%d",
                      ossl_ret);
        ret = SDK_RET_ERR;
        goto cleanup;
    }
    switch (hash_type) {
        case SHA1:
            md = EVP_sha1();
            break;
        case SHA224:
            md = EVP_sha224();
            break;
        case SHA256:
            md = EVP_sha256();
            break;
        case SHA384:
            md = EVP_sha384();
            break;
        case SHA512:
            md = EVP_sha512();
            break;
        default:
            SDK_TRACE_ERR("Unsupported Hash Algo");
            ret = SDK_RET_INVALID_ARG;
            goto cleanup;
    }
    if ((ossl_ret = EVP_PKEY_CTX_set_signature_md(ctx, md)) <= 0) {
        /* Error */
        SDK_TRACE_ERR("Failed to setup hash scheme in EVP_PKEY_CTX, ret:%d",
                      ossl_ret);
        ret = SDK_RET_ERR;
        goto cleanup;
    }

    ELBA_BARCO_API_PARAM_HEXDUMP("Msg", (char*)msg, msg_len);
    /* Compute the digest of the message */
    if (compute_message_digest(md, msg, msg_len, digest, &digest_len)) {
        SDK_TRACE_ERR("Failed to compute the message digest");
        ret = SDK_RET_ERR;
        goto cleanup;
    }
    SDK_TRACE_INFO("Computed message digest of len: %d", digest_len);
    ELBA_BARCO_API_PARAM_HEXDUMP("Msg Digest", digest, digest_len);

    if ((ossl_ret = EVP_PKEY_verify(ctx, s, key_size,
                                    (const unsigned char*) digest,
                                    digest_len)) <= 0) {
        /* Error */
        SDK_TRACE_ERR("Failed to verify sig, ret:%d", ossl_ret);
        ret = SDK_RET_ERR;
        goto cleanup;
    }
    else {
        SDK_TRACE_INFO("EVP_PKEY_verify Succeeded");
    }

cleanup:
    if (evp_pkey) {
        EVP_PKEY_free(evp_pkey);
    }

    if (ctx) {
        EVP_PKEY_CTX_free(ctx);
    }

    return ret;
}

}    // namespace elba
}    // namespace platform
}    // namespace sdk
