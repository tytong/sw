#include "nic/p4/common/defines.h"
#include "tcp-constants.h"
#include "tcp-shared-state.h"
#include "tcp-macros.h"
#include "tcp-table.h"
#include "tcp-phv.h"
#include "tcp_common.h"
#include "ingress.h"
#include "INGRESS_p.h"
#include "INGRESS_s1_t0_ooq_tcp_tx_k.h"

struct phv_ p;
struct s1_t0_ooq_tcp_tx_k_ k;
struct s1_t0_ooq_tcp_tx_process_next_descr_addr_d d;

%%
    .align
tcp_ooq_txdma_process_next_descr_addr:
    CAPRI_CLEAR_TABLE_VALID(0)
    CAPRI_OPERAND_DEBUG(d.descr_addr)

tcp_ooq_txdma_dma_cmds:
    phvwri          p.{p4_intr_global_tm_iport...p4_intr_global_tm_oport}, \
                        (TM_PORT_DMA << 4) | TM_PORT_DMA
    phvwri          p.p4_txdma_intr_dma_cmd_ptr, TCP_PHV_OOQ_TXDMA_COMMANDS_START

    phvwr           p.intr_rxdma_qid, k.common_phv_fid
    phvwr           p.intr_rxdma_rx_splitter_offset, \
                    (ASICPD_GLOBAL_INTRINSIC_HDR_SZ + ASICPD_RXDMA_INTRINSIC_HDR_SZ + \
                    P4PLUS_TCP_PROXY_BASE_HDR_SZ + P4PLUS_TCP_PROXY_OOQ_HDR_SZ)
    CAPRI_DMA_CMD_PHV2PKT_SETUP2(intrinsic_dma_cmd, p4_intr_global_tm_iport,
                                p4_intr_packet_len,
                                intr_rxdma_qid, intr_rxdma_rxdma_rsv)

    add             r1, r0, d.descr_addr
    CAPRI_DMA_CMD_MEM2PKT_SETUP_STOP(tcp_app_header_dma_cmd, r1, \
                        P4PLUS_TCP_PROXY_BASE_HDR_SZ + P4PLUS_TCP_PROXY_OOQ_HDR_SZ)

    seq             c1, k.common_phv_all_ooq_done, 1
    b.c1            tcp_ooq_txdma_win_upd_mem2pkt_dma
    nop
    CAPRI_CLEAR_TABLE_VALID(0)
    nop.e
    nop

tcp_ooq_txdma_win_upd_mem2pkt_dma:
    phvwr           p.tcp_app_header_dma_cmd_eop, 0
    CAPRI_DMA_CMD_PHV2PKT_SETUP2(intrinsic2_dma_cmd, p4_intr_global_tm_iport,
                                p4_intr_packet_len,
                                intr_rxdma2_qid, intr_rxdma2_rxdma_rsv)
    phvwr           p.intr_rxdma2_qid, k.common_phv_fid
    phvwr           p.intr_rxdma2_rx_splitter_offset, \
                    (ASICPD_GLOBAL_INTRINSIC_HDR_SZ + ASICPD_RXDMA_INTRINSIC_HDR_SZ + \
                    P4PLUS_TCP_PROXY_BASE_HDR_SZ + 1)
    CAPRI_DMA_CMD_MEM2PKT_SETUP(tcp_app_header2_dma_cmd, r1, \
                        P4PLUS_TCP_PROXY_BASE_HDR_SZ)

    phvwr           p.feedback_type_entry, TCP_TX2RX_FEEDBACK_LAST_OOO_PKT
    CAPRI_DMA_CMD_PHV2PKT_SETUP_STOP(feedback_dma_cmd, feedback_type_entry, feedback_type_entry)
    nop.e
    nop

/* Flow control window update trigger. Application( peer tcp session in case of bypass proxy )
 * has read the data and rang the doorbell. We are in the context of the right TCP session.
 * However an acknodlegement can not be triggered now since the receive window calculation
 * has to be done, and that should be done in rxdma. Setup a pseudo pkt and enqueue it to wakeup
 * rxdma.
 */
    .align
tcp_ooq_txdma_win_upd_phv2pkt_dma:
    CAPRI_CLEAR_TABLE_VALID(0)
    phvwr           p.tcp_app_hdr_from_ooq_txdma, 1
    CAPRI_DMA_CMD_PHV2PKT_SETUP2(intrinsic2_dma_cmd, p4_intr_global_tm_iport,
                                p4_intr_packet_len,
                                intr_rxdma2_qid, intr_rxdma2_rxdma_rsv)
    phvwr           p.intr_rxdma2_qid, k.common_phv_fid
    phvwr           p.intr_rxdma2_rx_splitter_offset, \
                    (ASICPD_GLOBAL_INTRINSIC_HDR_SZ + ASICPD_RXDMA_INTRINSIC_HDR_SZ + \
                    P4PLUS_TCP_PROXY_BASE_HDR_SZ + 1)
    CAPRI_DMA_CMD_PHV2PKT_SETUP(tcp_app_hdr1_dma_cmd, tcp_app_hdr_p4plus_app_id, tcp_app_hdr_prev_echo_ts)


    phvwr           p.feedback_type_entry, TCP_TX2RX_FEEDBACK_WIN_UPD
    CAPRI_DMA_CMD_PHV2PKT_SETUP_STOP(feedback_dma_cmd, feedback_type_entry, feedback_type_entry)
    nop.e
    nop
