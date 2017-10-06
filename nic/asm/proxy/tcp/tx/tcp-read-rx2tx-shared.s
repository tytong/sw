/*
 *      Implements the rx2tx shared state read stage of the TxDMA P4+ pipeline
 */

#include "tcp-constants.h"
#include "tcp-shared-state.h"
#include "tcp-macros.h"
#include "tcp-table.h"
#include "tcp_common.h"
#include "ingress.h"
#include "INGRESS_p.h"
	
struct phv_ p;
struct tcp_tx_read_rx2tx_k k;
struct tcp_tx_read_rx2tx_read_rx2tx_d d;
	
%%
    .align
    .param          tcp_tx_read_rx2tx_shared_extra_stage1_start
    .param          tcp_tx_sesq_read_ci_stage1_start

tcp_tx_read_rx2tx_shared_process:
    phvwr           p.common_phv_fid, k.p4_txdma_intr_qid
    phvwr           p.common_phv_qstate_addr, k.{p4_txdma_intr_qstate_addr_sbit0_ebit1...p4_txdma_intr_qstate_addr_sbit2_ebit33}
#if 0
    /* Trigger any pending timer bookkeeping from rx */
    sne             c1, d.pending_ft_clear, r0
    bcf             [c1], ft_clear
    nop
    sne             c2, d.pending_ft_reset, r0
    bcf             [c2], ft_reset
    nop
ft_clear:
    b               write_phv
    nop
ft_reset:	
    CAPRI_TIMER_START(LIF_TCP, 0, k.p4_txdma_intr_qid, TCP_SCHED_RING_FT, d.rto_deadline)
write_phv:	
#endif
    phvwr           p.common_phv_snd_una, d.snd_una
    phvwr           p.common_phv_rcv_nxt, d.rcv_nxt
    CAPRI_OPERAND_DEBUG(d.snd_wnd)
    CAPRI_OPERAND_DEBUG(d.snd_cwnd)
    phvwr           p.t0_s2s_snd_cwnd, d.snd_cwnd
    phvwr           p.t0_s2s_snd_wnd, d.snd_wnd

    CAPRI_NEXT_TABLE_READ_OFFSET(0, TABLE_LOCK_EN,
                        tcp_tx_read_rx2tx_shared_extra_stage1_start,
                        k.{p4_txdma_intr_qstate_addr_sbit0_ebit1...p4_txdma_intr_qstate_addr_sbit2_ebit33},
                        TCP_TCB_RX2TX_SHARED_EXTRA_OFFSET, TABLE_SIZE_512_BITS)

	.brbegin
	    brpri		r7[4:0], [0,1,2,3,4]
	    nop
	        .brcase 0
	            b tcp_tx_launch_sesq
	            nop
	        .brcase 1
	            b tcp_tx_pending_rx2tx
	            nop
	        .brcase 2
	            b tcp_tx_rx2tx_end // shouldn't happen
	            nop
	        .brcase 3
	            b tcp_tx_rx2tx_end // shouldn't happen
	            nop
	        .brcase 4
	            b tcp_tx_launch_asesq
	            nop
            .brcase 5
	            b tcp_tx_launch_sesq
	            nop
	.brend


tcp_tx_launch_sesq:
    // TODO check pi != ci
    phvwr           p.to_s2_sesq_cidx, d.{ci_0}.hx
    add             r3, d.{sesq_base}.wx, d.{ci_0}.hx, NIC_SESQ_ENTRY_SIZE_SHIFT
    phvwr           p.to_s1_sesq_ci_addr, r3

    phvwri          p.common_phv_pending_sesq, 1
    CAPRI_NEXT_TABLE_READ(1, TABLE_LOCK_DIS, tcp_tx_sesq_read_ci_stage1_start,
                     r3, TABLE_SIZE_64_BITS)
    nop.e
    nop
tcp_tx_launch_asesq:
    smeqb           c1, d.debug_dol_tx, TCP_TX_DDOL_DONT_TX, TCP_TX_DDOL_DONT_TX
    phvwri.c1       p.common_phv_debug_dol_dont_tx, 1
    phvwr           p.to_s2_sesq_cidx, d.{ci_4}.hx
    add             r3, d.{asesq_base}.wx, d.{ci_4}.hx, NIC_SESQ_ENTRY_SIZE_SHIFT
    phvwr           p.to_s1_sesq_ci_addr, r3
    CAPRI_NEXT_TABLE_READ(1, TABLE_LOCK_DIS, tcp_tx_sesq_read_ci_stage1_start,
                     r3, TABLE_SIZE_64_BITS)
    nop.e
    nop

tcp_tx_pending_rx2tx:
    smeqb           c1, d.debug_dol_tx, TCP_TX_DDOL_DONT_SEND_ACK, TCP_TX_DDOL_DONT_SEND_ACK
    phvwri.c1       p.common_phv_debug_dol_dont_send_ack, 1
tcp_tx_rx2tx_end:
    nop.e
    nop
