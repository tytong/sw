// {C} Copyright 2020 Pensando Systems Inc. All rights reserved

#ifndef __ELBA_BARCO_RINGS_HPP__
#define __ELBA_BARCO_RINGS_HPP__

#ifndef BARCO_CRYPTO_RINGS_CFG_VAL_ONLY

#include "include/sdk/base.hpp"

namespace sdk {
namespace platform {
namespace elba {

/* FIXME: These definitions used by the storage offload code are a dupliate of
 * what's already defined in capri_barco_rings.hpp. The storage code needs to migrate to
 * the common definitions and the following needs to be removed
 */

typedef struct barco_symm_descr_s {
    uint64_t                ilist_addr;
    uint64_t                olist_addr;
    uint32_t                command;
    uint32_t                key_desc_index;
    uint64_t                iv_addr;
    uint64_t                status_addr;
    uint64_t                doorbell_addr;
    uint64_t                doorbell_data;
    uint32_t                salt;
    uint64_t                explicit_iv;
    uint32_t                barco_status;
    uint32_t                header_size;
    uint32_t                second_key_desc_index;
} barco_symm_descr_t;

typedef struct barco_asym_descr_s {
    uint64_t                ilist_addr;
    uint64_t                olist_addr;
    uint32_t                key_desc_index;
    uint64_t                status_addr;
    uint32_t                opaque_tag_value;
    uint32_t                opaque_tag_wr_en;
    uint32_t                flag_a;
    uint32_t                flag_b;
    uint32_t                barco_status;
} barco_asym_descr_t;

typedef struct barco_ring_meta_config_s {
    uint64_t                ring_base;
    uint64_t                producer_idx_addr;
    uint64_t                shadow_pndx_addr;
    uint64_t                opaque_tag_addr;
    uint32_t                ring_size;
    uint32_t                desc_size;
    uint32_t                pndx_size;
    uint32_t                opaque_tag_size;
} barco_ring_meta_config_t;

typedef enum barco_rings_e {
    BARCO_RING_ASYM,
    BARCO_RING_GCM0,
    BARCO_RING_GCM1,
    BARCO_RING_XTS0,
    BARCO_RING_XTS1,
    BARCO_RING_MPP0,
    BARCO_RING_MPP1,
    BARCO_RING_MPP2,
    BARCO_RING_MPP3,
    BARCO_RING_MPP4,
    BARCO_RING_MPP5,
    BARCO_RING_MPP6,
    BARCO_RING_MPP7,
    BARCO_RING_CP,
    BARCO_RING_CP_HOT,
    BARCO_RING_DC,
    BARCO_RING_DC_HOT,
    BARCO_RING_MAX
} barco_rings_t;

/* Asymmetric engine related definitions */
typedef struct barco_asym_descriptor_s {
    uint64_t                input_list_addr;
    uint64_t                output_list_addr;
    uint64_t                status_addr;
    uint32_t                opaque_tag_value;
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    uint32_t                key_descr_idx:24;
    uint8_t                 opage_tag_wr_en:1;
    uint8_t                 flag_a:1;
    uint8_t                 flag_b:1;
    uint8_t                 rsvd:5;
#else
    uint8_t                 rsvd:5;
    uint8_t                 flag_b:1;
    uint8_t                 flag_a:1;
    uint8_t                 opage_tag_wr_en:1;
    uint32_t                key_descr_idx:24;
#endif
} __attribute__((__packed__)) barco_asym_descriptor_t;

typedef struct barco_asym_dma_descriptor_s {
    uint64_t                address;
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    uint64_t                stop:1;
    uint64_t                rsvd0:1;
    uint64_t                next:62;

    uint32_t                length:28;
    uint32_t                cst_addr:1;
    uint32_t                realign:1;
    uint32_t                discard:1;
    uint32_t                int_en:1;
#else
    uint32_t                int_en:1;
    uint32_t                discard:1;
    uint32_t                realign:1;
    uint32_t                cst_addr:1;
    uint32_t                length:28;

    uint64_t                next:62;
    uint64_t                rsvd0:1;
    uint64_t                stop:1;
#endif
    uint32_t                tag;
} __attribute__((__packed__)) barco_asym_dma_descriptor_t;

typedef struct barco_symm_req_descriptor_s {
    uint64_t                input_list_addr;
    uint64_t                output_list_addr;
    uint32_t                command;
    uint32_t                key_descr_idx;
    uint64_t                iv_address;
    uint64_t                auth_tag_addr;
    uint32_t                header_size;
    uint64_t                status_addr;
    uint64_t                opaque_tag_address;
    uint32_t                opaque_tag_value;
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    uint32_t                opaque_tag_wr_en:1;
    uint32_t                doorbell_32_bit:1;
    uint32_t                rsvd:30;
#else
    uint32_t                rsvd:30;
    uint32_t                doorbell_32_bit:1;
    uint32_t                opaque_tag_wr_en:1;
#endif
    uint64_t                doorbell_addr;
    uint64_t                doorbell_data;
    uint32_t                second_key_descr_idx;
    uint16_t                sector_size;
    uint16_t                application_tag;
    uint32_t                sector_num;
    uint8_t                 reserved[256];
}  __attribute__((__packed__)) barco_symm_req_descriptor_t;

typedef struct barco_sym_msg_descriptor_s {
    uint64_t   A0_addr;
    uint32_t   O0_addr_offset;
    uint32_t   L0_data_length;
    uint64_t   A1_addr;
    uint32_t   O1_addr_offset;
    uint32_t   L1_data_length;
    uint64_t   A2_addr;
    uint32_t   O2_addr_offset;
    uint32_t   L2_data_length;
    uint64_t   next_address;
    uint64_t   reserved;
}  __attribute__((__packed__)) barco_sym_msg_descriptor_t;

/* Barco Asym DMA Descriptor allocator */
sdk_ret_t pd_crypto_asym_dma_descr_alloc(uint64_t *asym_dma_descr);
sdk_ret_t pd_crypto_asym_dma_descr_free(uint64_t asym_dma_descr);
sdk_ret_t pd_crypto_asym_dma_descr_init(void);

typedef sdk_ret_t (*barco_ring_init_t) (struct elba_barco_ring_s *);
typedef bool (*barco_ring_poller_t) (struct elba_barco_ring_s *, uint32_t);
typedef sdk_ret_t (*barco_ring_queue_request) (struct elba_barco_ring_s *,
                                               void *, uint32_t *, bool);
typedef struct elba_barco_ring_s {
    char                ring_name[32];      /*  Friendly name for logging       */
    char                hbm_region[32];     /*  HBM region name                 */
    uint64_t            ring_base;          /*  Address to the ring memory      */
    uint64_t            ring_alignment;     /*  Alignment of the ring elements  */
    uint16_t            ring_size;          /*  number of entries               */
    uint16_t            descriptor_size;    /*  in bytes                        */
    uint16_t            producer_idx;       /*  S/W Write Ptr                   */
    uint16_t            consumer_idx;       /*  S/W Read ptr                    */
    uint64_t            opaque_tag_addr;    /*  Location of opa tag             */
    uint32_t            opaqe_tag_value;    /*  Running counter for the opa tag */
    uint16_t            opaque_tag_size;    /*  size of opa tag                 */
    uint16_t            shadow_pndx_size;   /*  size of shadow pindex, if any   */
    uint64_t            shadow_pndx_addr;   /*  address of shadow pindx         */
    uint64_t            producer_idx_addr;  /*  address of HW pindx register    */
    bool                sw_reset_capable;   /*  support soft ring reset         */
    bool                sw_enable_capable;  /*  support soft ring enable/disable*/
    /* TBD lock/spinlock for ring access */
    barco_ring_init_t   init;               /* Ring initialization fn           */
    barco_ring_poller_t poller;             /* Ring poller fn                   */
    barco_ring_queue_request    queue_request;  /* Request queue support        */

#if 0
    /*
     * User context blocks for asynchronous response handling (TBD).
     */
     elba_barco_user_cb_t user_cb[ELBA_MAX_BARCO_RING_SIZE];
#endif
} elba_barco_ring_t;

//#define BARCO_RING_XTS_STR(id)  "Barco XTS" ## #id
#define BARCO_RING_GCM_STR(id)  "Barco GCM" ## #id
#define BARCO_RING_MPP_BASE_STR "Barco MPP"
#define BARCO_RING_MPP_STR(id)  "Barco MPP" ## #id
#define BARCO_RING_ASYM_STR     "Barco Asym"
#define BARCO_RING_SYM0_RING0_STR   "Barco SYM0_RING0"
#define BARCO_RING_SYM1_RING0_STR   "Barco SYM1_RING0"
#define BARCO_RING_XTS0_STR     "Barco XTS0"
#define BARCO_RING_XTS1_STR     "Barco XTS1"
#define BARCO_RING_MPP0_STR     "Barco MPP0"
#define BARCO_RING_MPP1_STR     "Barco MPP1"
#define BARCO_RING_MPP2_STR     "Barco MPP2"
#define BARCO_RING_MPP3_STR     "Barco MPP3"
#define BARCO_RING_CP_STR       "Barco CP"
#define BARCO_RING_CP_HOT_STR   "Barco CP Hot"
#define BARCO_RING_DC_STR       "Barco DC"
#define BARCO_RING_DC_HOT_STR   "Barco DC Hot"

sdk_ret_t elba_barco_ring_queue_request(barco_rings_t barco_ring_type,
                                        void *req, uint32_t *req_tag, bool);
bool elba_barco_ring_poll(barco_rings_t barco_ring_type, uint32_t req_tag);
sdk_ret_t elba_barco_asym_req_descr_get(uint32_t slot_index,
                                        barco_asym_descr_t *asym_req_descr);
sdk_ret_t elba_barco_symm_req_descr_get(barco_rings_t ring_type,
                                        uint32_t slot_index,
                                        barco_symm_descr_t *symm_req_descr);
sdk_ret_t elba_barco_ring_meta_get(barco_rings_t ring_type,
                                   uint32_t *pi, uint32_t *ci);

typedef int (*barco_response_cb)(void *user_ctx, void *response);

#define ELBA_MAX_BARCO_RING_SIZE 1024

/*
 * User context block for asynchronous response handling.
 */
typedef struct elba_barco_user_cb_t_ {
    void               *user_ctx;     /* Opaque context specific to user */
    barco_response_cb  resp_handler;  /* User callback for response processing */
} elba_barco_user_cb_t;

#if 0
/*
 * Enqueue user request in a shadow ring to track for asynchronous response.
 */
static inline void
elba_barco_req_enqueue (elba_barco_ring_t *barco_ring,
             void *user_ctx, barco_response_cb cb)
{
    barco_ring->user_cb[barco_ring->producer_idx].user_ctx = user_ctx;
    barco_ring->user_cb[barco_ring->producer_idx].resp_handler = cb;
}

/*
 * Handle asynchronous response from barco to dispatch to user.
 */
static inline void
elba_barco_resp_dispatch (elba_barco_ring_t *barco_ring)
{
    barco_response_cb cb;

    cb = barco_ring->user_cb[barco_ring->consumer_idx].resp_handler;
    (*cb)(barco_ring->user_cb[barco_ring->consumer_idx].user_ctx, NULL); // 2nd Param to be fixed

    barco_ring->user_cb[barco_ring->consumer_idx].user_ctx = NULL;
    barco_ring->user_cb[barco_ring->consumer_idx].resp_handler = NULL;
}
#endif

sdk_ret_t get_opaque_tag_addr(barco_rings_t ring_type, uint64_t* addr);

sdk_ret_t elba_barco_get_meta_config_info(barco_rings_t ring_type,
                                          uint64_t* shadow_pndx_addr,
                                          uint32_t *shadow_pndx_size,
                                          uint32_t *desc_size,
                                          uint32_t *opaque_tag_size);

sdk_ret_t elba_barco_get_meta_config_info(barco_rings_t ring_type,
                                           barco_ring_meta_config_t *meta);

sdk_ret_t elba_barco_get_capabilities(barco_rings_t ring_type,
                                      bool *sw_reset_capable,
                                      bool *sw_enable_capable);

} // namespace elba
} // namespace platform
} // namespace sdk

#endif /* BARCO_CRYPTO_RINGS_CFG_VAL_ONLY */

/*
 * Compression/decompression block register values
 */
#define BARCO_CRYPTO_CP_CFG_GLB_SOFT_RESET          (1 << 30)
#define BARCO_CRYPTO_CP_CFG_GLB_HDR_VER             0x00000001
#define BARCO_CRYPTO_CP_CFG_GLB_HDR_VER_MASK        0x0000ffff
#define BARCO_CRYPTO_CP_UENG_LO_EN_ALL              0x0000ffff
#define BARCO_CRYPTO_CP_UENG_HI_SHA_DATA_UNCOMP     (1 << (36 - 32))
#define BARCO_CRYPTO_CP_UENG_HI_CSUM_ON_UNCOMP      (1 << (53 - 32))
#define BARCO_CRYPTO_CP_UENG_HI_HMEM_FILL_ZERO      (1 << (54 - 32))
#define BARCO_CRYPTO_CP_UENG_HI_INTEG_APP_STATUS    (1 << (55 - 32))
#define BARCO_CRYPTO_CP_DIST_DESC_Q_EN              (1 << 0U)
#define BARCO_CRYPTO_CP_DIST_DESC_Q_SIZE_MASK       0xfff
#define BARCO_CRYPTO_CP_DIST_DESC_Q_SIZE_SHFT       2
#define BARCO_CRYPTO_CP_DIST_DESC_Q_SIZE(sz)        \
        (((sz) & BARCO_CRYPTO_CP_DIST_DESC_Q_SIZE_MASK) << BARCO_CRYPTO_CP_DIST_DESC_Q_SIZE_SHFT)
#define BARCO_CRYPTO_CP_DIST_DESC_HOTQ_SIZE_MASK    0xfff
#define BARCO_CRYPTO_CP_DIST_DESC_HOTQ_SIZE_SHFT    14
#define BARCO_CRYPTO_CP_DIST_DESC_HOTQ_EN           (1 << 1)
#define BARCO_CRYPTO_CP_DIST_DESC_HOTQ_SIZE(sz)     \
        (((sz) & BARCO_CRYPTO_CP_DIST_DESC_HOTQ_SIZE_MASK) << BARCO_CRYPTO_CP_DIST_DESC_HOTQ_SIZE_SHFT)

#define BARCO_CRYPTO_DC_CFG_GLB_SOFT_RESET          (1 << 30)
#define BARCO_CRYPTO_DC_CFG_GLB_HDR_VER             0x00000001
#define BARCO_CRYPTO_DC_CFG_GLB_HDR_VER_MASK        0x0000ffff
#define BARCO_CRYPTO_DC_UENG_LO_EN_ALL              0x00000003
#define BARCO_CRYPTO_DC_UENG_HI_SHA_DATA_UNCOMP     (1 << (36 - 32))
#define BARCO_CRYPTO_DC_UENG_HI_CSUM_ON_UNCOMP      (1 << (53 - 32))
#define BARCO_CRYPTO_DC_UENG_HI_HMEM_FILL_ZERO      (1 << (54 - 32))
#define BARCO_CRYPTO_DC_UENG_HI_INTEG_APP_STATUS    (1 << (55 - 32))
#define BARCO_CRYPTO_DC_DIST_DESC_Q_EN              (1 << 0)
#define BARCO_CRYPTO_DC_DIST_DESC_Q_SIZE_MASK       0xfff
#define BARCO_CRYPTO_DC_DIST_DESC_Q_SIZE_SHFT       2
#define BARCO_CRYPTO_DC_DIST_DESC_Q_SIZE(sz)        \
        (((sz) & BARCO_CRYPTO_DC_DIST_DESC_Q_SIZE_MASK) << BARCO_CRYPTO_DC_DIST_DESC_Q_SIZE_SHFT)
#define BARCO_CRYPTO_DC_DIST_DESC_HOTQ_SIZE_MASK    0xfff
#define BARCO_CRYPTO_DC_DIST_DESC_HOTQ_SIZE_SHFT    14
#define BARCO_CRYPTO_DC_DIST_DESC_HOTQ_EN           (1 << 1)
#define BARCO_CRYPTO_DC_DIST_DESC_HOTQ_SIZE(sz)     \
        (((sz) & BARCO_CRYPTO_DC_DIST_DESC_HOTQ_SIZE_MASK) << BARCO_CRYPTO_DC_DIST_DESC_HOTQ_SIZE_SHFT)

#define BARCO_CRYPTO_CP_RING_SIZE                   4096
#define BARCO_CRYPTO_CP_HOT_RING_SIZE               4096
#define BARCO_CRYPTO_DC_RING_SIZE                   4096
#define BARCO_CRYPTO_DC_HOT_RING_SIZE               4096

using sdk::platform::elba::barco_symm_descr_t;
using sdk::platform::elba::barco_asym_descr_t;
using sdk::platform::elba::barco_ring_meta_config_t;
using sdk::platform::elba::barco_rings_t;
using sdk::platform::elba::barco_asym_descriptor_t;
using sdk::platform::elba::barco_asym_dma_descriptor_t;
using sdk::platform::elba::barco_symm_req_descriptor_t;
using sdk::platform::elba::barco_sym_msg_descriptor_t;

#endif // __ELBA_BARCO_RINGS_HPP__
