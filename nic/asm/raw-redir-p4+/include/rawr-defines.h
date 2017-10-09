#ifndef __RAWR_DEFINES_H
#define __RAWR_DEFINES_H

#include "INGRESS_p.h"
#include "ingress.h"
#include "capri-macros.h"
#include "../../../p4/nw/include/defines.h"

#define BITS_PER_BYTE                   8
#define SIZE_IN_BITS(bytes)             ((bytes) * BITS_PER_BYTE)
#define SIZE_IN_BYTES(bits)             ((bits) / BITS_PER_BYTE)


/*
 * Debug flags
 */
#define RAWR_FREE_SEM_PINDEX_FULL_DEBUG 0

 
/*
 * Intrinsic fields
 */
#define CAPRI_INTRINSIC_LIF             k.{p4_intr_global_lif_sbit0_ebit2...p4_intr_global_lif_sbit3_ebit10}
#define CAPRI_INTRINSIC_QTYPE           k.p4_rxdma_intr_qtype
#define CAPRI_INTRINSIC_QID             k.p4_rxdma_intr_qid
#define CAPRI_INTRINSIC_QSTATE_ADDR     k.{p4_rxdma_intr_qstate_addr_sbit0_ebit1...p4_rxdma_intr_qstate_addr_sbit2_ebit33}

#define RAWR_CB_TABLE_ENTRY_SIZE        64 /* in bytes */
#define RAWR_CB_TABLE_ENTRY_SIZE_SHFT   6

#define RAWRPR_ALLOC_IDX                CAPRI_SEM_RNMPR_ALLOC_INF_ADDR
#define RAWRPR_FREE_IDX                 CAPRI_SEM_RNMPR_FREE_INF_ADDR
#define RAWRPR_SMALL_ALLOC_IDX          CAPRI_SEM_RNMPR_SMALL_ALLOC_INF_ADDR
#define RAWRPR_SMALL_FREE_IDX           CAPRI_SEM_RNMPR_SMALL_FREE_INF_ADDR
#define RAWRDR_ALLOC_IDX                CAPRI_SEM_RNMDR_ALLOC_INF_ADDR 
#define RAWRDR_FREE_IDX                 CAPRI_SEM_RNMDR_FREE_INF_ADDR

#define NIC_DESC_ENTRY_0_OFFSET         64         /* &((nic_desc_t *)0)->entry[0]*/

/*
 * p4_to_p4plus app header immediately after rx_splitter_offset
 */
#define RAWR_P4PLUS_APP_HEADER_SZ       P4PLUS_CPU_PKT_SZ

/*
 * From capri_hbm.hpp
 */
#define JUMBO_FRAME_SIZE                9216
#define ETH_FRAME_SIZE                  1536

#define RAWR_PPAGE_SIZE                 JUMBO_FRAME_SIZE
#define RAWR_MPAGE_SIZE                 ETH_FRAME_SIZE

/*
 * Descriptor valid bit
 */ 
#define DESC_VALID_BIT_SHIFT            63

/*
 * When semaphore access is not applicable, service chain queues may be directly
 * accessed from an HBM table. For example, if the next service queue were a
 * per-core ARQ, the following indices would represent core IDs.
 * 
 */
#define CHAIN_QIDXR_PI_0                0
#define CHAIN_QIDXR_PI_1                1
#define CHAIN_QIDXR_PI_2                2

/*
 * Load 64-bit immediate
 */
#define RAWR_IMM64_LOAD(reg, imm64)                                             \
        addui       reg, r0, hiword(imm64);                                     \
        addi        reg, r0, loword(imm64);
    
#define RAWR_IMM64_LOAD_CF(cf, reg, imm64)                                      \
        addui.cf    reg, r0, hiword(imm64);                                     \
        addi.cf     reg, r0, loword(imm64);
        
/*
 * Launch an allocation semaphore pindex fetch/update
 */
#define RAWR_ALLOC_SEM_PINDEX_FETCH_UPDATE(table_no, reg, sem_alloc_idx, stage_entry) \
        addi        reg, r0, sem_alloc_idx;                                     \
        CAPRI_NEXT_TABLE_READ(table_no, TABLE_LOCK_DIS, stage_entry,            \
                              reg, TABLE_SIZE_64_BITS)
 
/*
 * Launch an free semaphore pindex fetch/update
 */
#define RAWR_FREE_SEM_PINDEX_FETCH_UPDATE(table_no, reg, sem_free_idx, stage_entry) \
        addi        reg, r0, sem_free_idx;                                      \
        CAPRI_NEXT_TABLE_READ(table_no, TABLE_LOCK_DIS, stage_entry,            \
                              reg, TABLE_SIZE_64_BITS)
                              
/*
 * Free an HBM element back to its table at a given pindex
 */
#define RAWR_FREE_SEM_PINDEX_ELEM_FREE(table_base, entry_shift_sz, pindex, elem)\
        RAWR_IMM64_LOAD(r3, table_base);                                        \
        add         r4, r0, pindex;                                             \
        sll         r4, r4, entry_shift_sz;                                     \
        add         r3, r3, r4;                                                 \
        memwr.wx    r3, elem.wx;
        

/*
 * Free semaphore pindex full trap
 */
#if RAWR_FREE_SEM_PINDEX_FULL_DEBUG
#define RAWR_RAWR_FREE_SEM_PINDEX_FULL_TRAP(cf)                                 \
        illegal.cf;                                                             \
        nop;
#else
#define RAWR_RAWR_FREE_SEM_PINDEX_FULL_TRAP(cf)                                 \
        nop;
#endif



/*
 * Command structure for DMA_CMD_TYPE_SKIP
 * (structure currently not available in p4/common-p4+/common_headers.p4)
 */
struct dma_cmd_skip_t {
    rsvd                    : 109;
    dma_cmd_skip_to_eop     : 1;
    dma_cmd_size            : 14;
    dma_cmd_eop             : 1;
    dma_cmd_type            : 3;
};

#endif //__RAWR_DEFINES_H

