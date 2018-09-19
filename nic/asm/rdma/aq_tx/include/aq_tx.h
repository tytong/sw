#ifndef __AQ_TX_H
#define __AQ_TX_H
#include "capri.h"
#include "types.h"
#include "sqcb.h"
#include "cqcb.h"
#include "rqcb.h"
#include "INGRESS_p.h"
#include "ingress.h"
#include "common_phv.h"


#define AQ_TX_DMA_CMD_PHV_INTRINSIC 2

#define AQ_TX_MAX_DMA_CMDS        12

#define AQ_TX_DMA_CMD_START_FLIT_ID 9
#define AQ_TX_DMA_CMD_START_FLIT_CMD_ID 0
#define TOTAL_DMA_CMD_BITS 16 * 16 * 8 // (cmds * dma_cmd_size * bits_per_byte) 

#define AQ_TX_DMA_CMD_RDMA_FEEDBACK (AQ_TX_MAX_DMA_CMDS -1)  // This should be at the end
#define AQ_TX_DMA_CMD_CREATE_CQ_CB  (AQ_TX_MAX_DMA_CMDS -2)
#define AQ_TX_DMA_CMD_CREATE_CQ_PT_DST  (AQ_TX_MAX_DMA_CMDS -3)
#define AQ_TX_DMA_CMD_CREATE_CQ_PT_SRC  (AQ_TX_MAX_DMA_CMDS -4)

#define AQ_TX_DMA_CMD_CREATE_QP_CB    (AQ_TX_MAX_DMA_CMDS -2)
#define AQ_TX_DMA_CMD_CREATE_QP_SQPT  (AQ_TX_MAX_DMA_CMDS -3)

#define AQ_TX_DMA_CMD_STATS_DUMP_4 (AQ_TX_MAX_DMA_CMDS - 2)
#define AQ_TX_DMA_CMD_STATS_DUMP_3 (AQ_TX_MAX_DMA_CMDS - 3)
#define AQ_TX_DMA_CMD_STATS_DUMP_2 (AQ_TX_MAX_DMA_CMDS - 4)
#define AQ_TX_DMA_CMD_STATS_DUMP_1 (AQ_TX_MAX_DMA_CMDS - 5)

#define AQ_TX_DMA_CMD_MR_PT_DST (AQ_TX_MAX_DMA_CMDS - 2)
#define AQ_TX_DMA_CMD_MR_PT_SRC (AQ_TX_MAX_DMA_CMDS - 3)
#define AQ_TX_DMA_CMD_MR_KT_UPDATE (AQ_TX_MAX_DMA_CMDS - 4)

#define AQ_TX_CQCB_ADDR_GET(_r, _cqid, _cqcb_base_addr_hi)  \
    CQCB_ADDR_GET(_r, _cqid, _cqcb_base_addr_hi);

#define RDMA_UPDATE_QP_OPER_SET_DEST_QP 0
#define RDMA_UPDATE_QP_OPER_SET_HEADER_TEMPLATE 1
#define RDMA_UPDATE_QP_OPER_SET_Q_KEY 2
#define RDMA_UPDATE_QP_OPER_SET_E_PSN 3
#define RDMA_UPDATE_QP_OPER_SET_TX_PSN 4
#define RDMA_UPDATE_QP_OPER_SET_RD_ATM 5
#define RDMA_UPDATE_QP_OPER_SET_DEST_RD_ATM 6

struct aq_tx_dma_cmds_flit_t {
    dma_cmd0 : 128;
    dma_cmd1 : 128;
    dma_cmd2 : 128;
    dma_cmd3 : 128;
};

// phv 
struct aq_tx_phv_t {
    // dma commands
    /* flit 11 */
    struct aq_tx_dma_cmds_flit_t flit_11;
    
        /* flit 10 */
    struct aq_tx_dma_cmds_flit_t flit_10;
    
        /* flit 9 */
    struct aq_tx_dma_cmds_flit_t flit_9;

        /* flit 8 */
    union {
        struct aq_tx_dma_cmds_flit_t flit_8;
        struct sqcb0_t sqcb0;
    };
     
        /* flit 7 */
    union {
        struct aq_tx_dma_cmds_flit_t flit_7;
        struct sqcb1_t sqcb1;
    };
        
        /* flit 6 */
    union {
        struct aq_tx_dma_cmds_flit_t flit_6;
        struct key_entry_t key;
        struct sqcb2_t sqcb2;
        struct cqcb_t  cqcb;
    };

    /* flit 0-5 */
    union {
        struct {
            struct phv_intr_p4_t p4_intr;            //  5 Bytes
            struct phv_intr_rxdma_t p4_intr_rxdma;   // 10 Bytes
            struct aq_p4_to_p4plus_roce_header_t p4_to_p4plus; // 20 Bytes
            struct rdma_aq_feedback_t rdma_feedback;    // 29 Bytes
            pad4: 512;
            pad3: 512;
            pad2: 512;
            pad1: 512;
            pad0: 512;
        };
        // common tx
        struct phv_ common;
    };
};

#endif //__AQ_TX_H
