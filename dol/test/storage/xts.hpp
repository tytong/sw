#ifndef _XTS_HPP_
#define _XTS_HPP_

#include <stdint.h>

#define CAPRI_BARCO_MD_HENS_REG_BASE                    (0x6580000)

#define CAPRI_BARCO_MD_HENS_REG_XTS_PRODUCER_IDX       (CAPRI_BARCO_MD_HENS_REG_BASE + 0x10c)
#define CAPRI_BARCO_MD_HENS_REG_XTS_CONSUMER_IDX       (CAPRI_BARCO_MD_HENS_REG_BASE + 0x180)

namespace xts {

typedef struct xts_aol_ {
  uint64_t a0;
  uint32_t o0;
  uint32_t l0;
  uint64_t a1;
  uint32_t o1;
  uint32_t l1;
  uint64_t a2;
  uint32_t o2;
  uint32_t l2;
  uint64_t next;
  uint64_t resv;
} __attribute__((packed)) xts_aol_t;

typedef struct xts_cmd_ {
  uint32_t enable_crc :1,
           bypass_aes :1,
           unused1 :18,
           is_decrypt :1,
           unused2 :3,
           token3 :4,
           token4 :4;
} __attribute__((packed)) xts_cmd_t;

#define XTS_REQ_DESC_PADDING 44

typedef struct xts_desc_ {
  uint64_t     in_aol;
  uint64_t     out_aol;
  xts_cmd_t    cmd;
  uint32_t     key_desc_idx;
  uint64_t     iv_addr;
  uint64_t     auth_tag;
  uint32_t     hdr;
  uint64_t     status;
  uint32_t     opaque_tag;
  uint32_t     opaque_tag_en :1,
               resv :31;
  uint16_t     sector_size;
  uint16_t     app_tag;
  uint32_t     sector_num;
  uint64_t     db_addr;
  uint64_t     db_data;
  char         resv_pad[XTS_REQ_DESC_PADDING];
} __attribute__((packed)) xts_desc_t;



}  // namespace xts

#endif   // _XTS_HPP_
