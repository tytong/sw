#ifndef __PD_API_C_H__
#define __PD_API_C_H__

#include "nic/sdk/include/sdk/pse_intf.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

int pd_tls_asym_ecdsa_p256_sig_gen(int32_t key_idx, uint8_t *p, uint8_t *n,
                                   uint8_t *xg, uint8_t *yg, uint8_t *a, uint8_t *b, uint8_t *da,
                                   uint8_t *k, uint8_t *h, uint8_t *r, uint8_t *s,
                                   bool async, const uint8_t *unique_key);
int pd_tls_asym_ecdsa_p256_sig_gen_param(void *ctx,
                                         const PSE_EC_SIGN_PARAM *param);
int pd_tls_asym_ecdsa_p256_sig_verify(uint8_t *p, uint8_t *n,
                                      uint8_t *xg, uint8_t *yg, uint8_t *a, uint8_t *b, uint8_t *xq,
                                      uint8_t *yq, uint8_t *r, uint8_t *s, uint8_t *h,
                                      bool async, const uint8_t *unique_key);
int pd_tls_asym_ecdsa_p256_sig_verify_param(void *ctx,
                                            const PSE_EC_VERIFY_PARAM *param);

int pd_tls_asym_rsa2k_encrypt(uint8_t *n, uint8_t *e, uint8_t *m,  uint8_t *c,
                              bool async, const uint8_t *unique_key);

int pd_tls_asym_rsa_encrypt(uint16_t key_size, uint8_t *n, uint8_t *e, uint8_t *m,  uint8_t *c,
                          bool async, const uint8_t *unique_key);
int pd_tls_asym_rsa_encrypt_param(void *ctx,
                                  const PSE_RSA_ENCRYPT_PARAM *param);

int pd_tls_asym_rsa_sig_gen(uint16_t key_size, int32_t key_idx, uint8_t *n, uint8_t *d,
                              uint8_t *h, uint8_t *s,
                              bool async, const uint8_t *unique_key);
int pd_tls_asym_rsa_sig_gen_param(void *ctx,
                                  const PSE_RSA_SIGN_PARAM *param);

int pd_tls_asym_rsa2k_crt_decrypt(int32_t key_idx, uint8_t *p, uint8_t *q, uint8_t *dp,
                                  uint8_t *dq, uint8_t *qinv, uint8_t *c, uint8_t *m,
                                  bool async, const uint8_t *unique_key);
int pd_tls_asym_rsa_decrypt_param(void *ctx,
                                  const PSE_RSA_DECRYPT_PARAM *param);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __PD_API_C_H__ */
