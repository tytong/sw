#include "capri.h"
#include "aq_rx.h"
#include "eqcb.h"
#include "common_phv.h"
#include "capri-macros.h"

struct aq_rx_phv_t p;
struct aq_rx_s7_t1_k k;
struct eqcb_t d;

#define EQWQE_P r1
#define DMA_CMD_BASE r4

#define IN_P t1_s2s_cqcb_to_eq_info

#define K_ASYNC_EQ CAPRI_KEY_FIELD(IN_P, async_eq)

#define PHV_EQWQE_START eqwqe.qid 
#define PHV_EQWQE_END   eqwqe.color
#define PHV_ASYNC_EQWQE_START async_eqwqe.qid
#define PHV_ASYNC_EQWQE_END   async_eqwqe.color

#define PHV_EQ_INT_ASSERT_DATA_BEGIN int_assert_data
#define PHV_EQ_INT_ASSERT_DATA_END int_assert_data
#define PHV_ASYNC_EQ_INT_ASSERT_DATA_BEGIN async_int_assert_data
#define PHV_ASYNC_EQ_INT_ASSERT_DATA_END   async_int_assert_data

%%

   .param RDMA_EQ_INTR_TABLE_BASE

.align
rdma_aq_rx_eqcb_process:

    seq             c1, EQ_P_INDEX, 0 //BD Slot
    // flip the color if cq is wrap around
    tblmincri.c1    EQ_COLOR, 1, 1     

    sll             r1, EQ_P_INDEX, d.log_wqe_size
    add             EQWQE_P, d.eqe_base_addr, r1

    bbeq            K_ASYNC_EQ, 1, async_eq
    // increment p_index
    tblmincri       EQ_P_INDEX, d.log_num_wqes, 1 // Branch Delay Slot

completion_eq:
    phvwr           p.eqwqe.color, EQ_COLOR
    DMA_CMD_STATIC_BASE_GET(DMA_CMD_BASE, AQ_RX_DMA_CMD_START_FLIT_ID, AQ_RX_DMA_CMD_EQ)
    DMA_PHV2MEM_SETUP(DMA_CMD_BASE, c1, PHV_EQWQE_START, PHV_EQWQE_END, EQWQE_P)

    //Writing Interrupt unconditionally... if needed, add a flag for this purpose
    DMA_CMD_STATIC_BASE_GET(DMA_CMD_BASE, AQ_RX_DMA_CMD_START_FLIT_ID, AQ_RX_DMA_CMD_EQ_INT)
    phvwri          p.int_assert_data, CAPRI_INT_ASSERT_DATA
    DMA_PHV2MEM_SETUP(DMA_CMD_BASE, c1, PHV_EQ_INT_ASSERT_DATA_BEGIN, PHV_EQ_INT_ASSERT_DATA_END, d.int_assert_addr)

    DMA_SET_WR_FENCE(DMA_CMD_PHV2MEM_T, DMA_CMD_BASE)
    DMA_SET_END_OF_CMDS_E(DMA_CMD_PHV2MEM_T, DMA_CMD_BASE)
    CAPRI_SET_TABLE_1_VALID(0)

async_eq:
    phvwr           p.async_eqwqe.color, EQ_COLOR
    DMA_CMD_STATIC_BASE_GET(DMA_CMD_BASE, AQ_RX_DMA_CMD_START_FLIT_ID, AQ_RX_DMA_CMD_ASYNC_EQ)
    DMA_PHV2MEM_SETUP(DMA_CMD_BASE, c1, PHV_ASYNC_EQWQE_START, PHV_ASYNC_EQWQE_END, EQWQE_P)

    DMA_CMD_STATIC_BASE_GET(DMA_CMD_BASE, AQ_RX_DMA_CMD_START_FLIT_ID, AQ_RX_DMA_CMD_ASYNC_EQ_INT)
    //Writing Interrupt unconditionally... if needed, add a flag for this purpose
    phvwri          p.async_int_assert_data, CAPRI_INT_ASSERT_DATA
    DMA_PHV2MEM_SETUP(DMA_CMD_BASE, c1, PHV_ASYNC_EQ_INT_ASSERT_DATA_BEGIN, PHV_ASYNC_EQ_INT_ASSERT_DATA_END, d.int_assert_addr)
 
    DMA_SET_WR_FENCE(DMA_CMD_PHV2MEM_T, DMA_CMD_BASE)
    DMA_SET_END_OF_CMDS_E(DMA_CMD_PHV2MEM_T, DMA_CMD_BASE)
    CAPRI_SET_TABLE_0_VALID(0)
