/*
 *  Implements the RTT stage of the RxDMA P4+ pipeline
 */

#include "tcp-constants.h"
#include "tcp-shared-state.h"
#include "tcp-macros.h"
#include "tcp-table.h"
#include "ingress.h"
#include "INGRESS_p.h"
#include "INGRESS_s4_t0_tcp_rx_k.h"

struct phv_ p;
struct s4_t0_tcp_rx_k_ k;
struct s4_t0_tcp_rx_tcp_cc_d d;


%%
    .param          tcp_rx_fc_stage_start
    .param          tcp_cc_new_reno
    .param          tcp_cc_cubic
    .param          TCP_PROXY_STATS
    .align
tcp_rx_cc_stage_start:
    bbeq            k.common_phv_ooq_tx2rx_pkt, 1, tcp_rx_cc_stage_end
    add r1, d.cc_algo, r0
    .brbegin
        br r1[1:0]
        nop
        .brcase TCP_CC_ALGO_NONE
            // Error
            b tcp_rx_cc_stage_end
            nop
        .brcase TCP_CC_ALGO_NEW_RENO
            j tcp_cc_new_reno
            nop
        .brcase TCP_CC_ALGO_CUBIC
            j tcp_cc_cubic
            nop
        .brcase 3 
            // Error
            b tcp_rx_cc_stage_end
            nop
    .brend

tcp_rx_cc_stage_end:
    seq             c1, d.ip_tos_ecn_received, 1
    seq.!c1         c1, k.common_phv_ip_tos_ecn, 3
    bal.c1          r7, tcp_rx_cc_handle_ip_tos
    phvwr           p.rx2tx_extra_snd_cwnd, d.snd_cwnd
    phvwr           p.rx2tx_extra_t_flags, d.t_flags
    CAPRI_NEXT_TABLE_READ_OFFSET_e(0, TABLE_LOCK_EN,
                        tcp_rx_fc_stage_start,
                        k.common_phv_qstate_addr,
                        TCP_TCB_FC_OFFSET, TABLE_SIZE_512_BITS)
    nop


/******************************************************************************
 * Functions
 *****************************************************************************/
 tcp_rx_cc_handle_ip_tos:
    smeqb           c2, k.common_phv_flags, TCPHDR_CWR, TCPHDR_CWR
    seq             c1, k.common_phv_ip_tos_ecn, 3
    balcf           r6, [c1], tcp_rx_cc_handle_ecn_rcvd
    // Next instr gets executed before the branch in the branch delay slot
    tblwr.c2        d.ip_tos_ecn_received, 0
    seq             c1, d.ip_tos_ecn_received, 1
    jr              r7
    tblor.c1.l      d.t_flags, TCPHDR_ECE

tcp_rx_cc_handle_ecn_rcvd:
    addui           r2, r0, hiword(TCP_PROXY_STATS)
    addi            r2, r2, loword(TCP_PROXY_STATS)
    CAPRI_ATOMIC_STATS_INCR1_NO_CHECK(r2, TCP_PROXY_STATS_RCVD_CE_PKTS, 1)
    jr              r6
    tblwr           d.ip_tos_ecn_received, 1
