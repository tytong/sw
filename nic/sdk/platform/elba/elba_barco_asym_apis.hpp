// {C} Copyright 2020 Pensando Systems Inc. All rights reserved

#ifndef __ELBA_BARCO_ASYM_APIS_HPP__
#define __ELBA_BARCO_ASYM_APIS_HPP__

namespace sdk {
namespace platform {
namespace elba {

typedef enum hash_type_e {
    SHA1,
    SHA224,
    SHA256,
    SHA384,
    SHA512
} hash_type_t;

typedef enum rsa_signature_scheme_e {
    RSASSA_PSS,
    RSASSA_PKCS1_v1_5
} rsa_signature_scheme_t;

//#define _API_PARAM_DEBUG_

#ifdef _API_PARAM_DEBUG_

#define MAX_LINE_SZ 128
static inline void
elba_hex_dump_trace (char *label, char *buf, uint16_t len)
{
    char            line[MAX_LINE_SZ];
    char            *lineptr;
    uint16_t        idx = 0;
    uint16_t        lineoffset = 0;

    lineptr = &line[0];
    HAL_TRACE_DEBUG("{}:", label);
    for (idx = 0; idx < len; idx++) {

        lineoffset += snprintf(lineptr + lineoffset,
                               (MAX_LINE_SZ - lineoffset - 1),
                               "%02hhx ", buf[idx]);

        if (((idx + 1) % 16) == 0) {
            HAL_TRACE_DEBUG("{}", line);
            lineoffset = 0;
        }
    }
    if (lineoffset) {
        HAL_TRACE_DEBUG("{}", line);
    }
}

#define ELBA_BARCO_API_PARAM_HEXDUMP(label, buf, len) elba_hex_dump_trace(label, buf, len)

#else

#define ELBA_BARCO_API_PARAM_HEXDUMP(label, buf, len)

#endif /* _API_PARAM_DEBUG_ */

sdk_ret_t elba_barco_asym_ecc_point_mul_p256(uint8_t *p, uint8_t *n,
                                             uint8_t *xg, uint8_t *yg,
                                             uint8_t *a, uint8_t *b,
                                             uint8_t *x1, uint8_t *y1,
                                             uint8_t *k, uint8_t *x3,
                                             uint8_t *y3);

sdk_ret_t elba_barco_asym_ecc_point_mul(uint16_t key_size,
                                         uint8_t *p, uint8_t *n,
                                         uint8_t *xg, uint8_t *yg,
                                         uint8_t *a, uint8_t *b,
                                         uint8_t *x1, uint8_t *y1,
                                         uint8_t *k,
                                         uint8_t *x3, uint8_t *y3);

sdk_ret_t elba_barco_asym_ecdsa_p256_setup_priv_key(uint8_t *p, uint8_t *n,
                                                    uint8_t *xg, uint8_t *yg,
                                                    uint8_t *a, uint8_t *b,
                                                    uint8_t *da,
                                                    int32_t *key_idx);

sdk_ret_t elba_barco_asym_ecdsa_p256_sig_gen(int32_t key_idx, uint8_t *p,
                                             uint8_t *n,
                                             uint8_t *xg, uint8_t *yg,
                                             uint8_t *a, uint8_t *b,
                                             uint8_t *da,
                                             uint8_t *k, uint8_t *h, uint8_t *r,
                                             uint8_t *s, bool async_en,
                                             const uint8_t *unique_key);

sdk_ret_t elba_barco_asym_ecdsa_p256_sig_verify(uint8_t *p, uint8_t *n,
                                                uint8_t *xg, uint8_t *yg,
                                                uint8_t *a, uint8_t *b,
                                                uint8_t *xq, uint8_t *yq,
                                                uint8_t *r, uint8_t *s,
                                                uint8_t *h, bool async_en,
                                                const uint8_t *unique_key);

sdk_ret_t elba_barco_asym_rsa2k_setup_sig_gen_priv_key(uint8_t *n, uint8_t *d,
                                                       int32_t *key_idx);

sdk_ret_t elba_barco_asym_rsa_setup_priv_key(uint16_t key_size, uint8_t *n,
                                              uint8_t *d, int32_t* key_idx);

sdk_ret_t elba_barco_asym_rsa2k_crt_setup_decrypt_priv_key(uint8_t *p,
                                                           uint8_t *q,
                                                           uint8_t *dp,
                                                           uint8_t *dq,
                                                           uint8_t *qinv,
                                                           int32_t* key_idx);

sdk_ret_t elba_barco_asym_rsa2k_encrypt(uint8_t *n, uint8_t *e,
                                        uint8_t *m,  uint8_t *c,
                                        bool async_en,
                                        const uint8_t *unique_key);

sdk_ret_t elba_barco_asym_rsa_encrypt(uint16_t key_size, uint8_t *n,
                                       uint8_t *e, uint8_t *m,  uint8_t *c,
                                       bool async_en,
                                       const uint8_t *unique_key);

sdk_ret_t elba_barco_asym_rsa2k_decrypt(uint8_t *n, uint8_t *d,
                                        uint8_t *c,  uint8_t *m);

sdk_ret_t elba_barco_asym_rsa2k_crt_decrypt(int32_t key_idx, uint8_t *p,
                                            uint8_t *q,
                                            uint8_t *dp, uint8_t *dq,
                                            uint8_t *qinv, uint8_t *c,
                                            uint8_t *m, bool async_en,
                                            const uint8_t *unique_key);

sdk_ret_t elba_barco_asym_rsa2k_sig_gen(int32_t key_idx, uint8_t *n,
                                        uint8_t *d, uint8_t *h, uint8_t *s,
                                        bool async_en,
                                        const uint8_t *unique_key);

sdk_ret_t elba_barco_asym_rsa_sig_gen(uint16_t key_size, int32_t key_idx,
                                       uint8_t *n, uint8_t *d,
                                       uint8_t *h, uint8_t *s,
                                       bool async_en,
                                       const uint8_t *unique_key);

sdk_ret_t elba_barco_asym_fips_rsa_sig_gen(uint16_t key_size, int32_t key_idx,
                                            uint8_t *n, uint8_t *e,
                                            uint8_t *msg, uint16_t msg_len,
                                            uint8_t *s, hash_type_t hash_type,
                                            rsa_signature_scheme_t sig_scheme,
                                            bool async_en,
                                            const uint8_t *unique_key);

sdk_ret_t elba_barco_asym_rsa2k_sig_verify(uint8_t *n, uint8_t *e,
                                           uint8_t *h, uint8_t *s);

sdk_ret_t elba_barco_asym_fips_rsa_sig_verify(uint16_t key_size,
                                               uint8_t *n, uint8_t *e,
                                               uint8_t *msg, uint16_t msg_len,
                                               uint8_t *s,
                                               hash_type_t hash_type,
                                               rsa_signature_scheme_t sig_scheme,
                                               bool async_en,
                                               const uint8_t *unique_key);

sdk_ret_t elba_barco_asym_add_pend_req(uint32_t hw_id, uint32_t sw_id);

sdk_ret_t elba_barco_asym_poll_pend_req(uint32_t batch_size, uint32_t* id_count,
                                        uint32_t *ids);

} // namespace elba
} // namespace platform
} // namespace sdk

#endif  //   __ELBA_BARCO_ASYM_APIS_HPP__
