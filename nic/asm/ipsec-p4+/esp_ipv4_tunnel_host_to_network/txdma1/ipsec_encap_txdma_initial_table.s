#include "INGRESS_p.h"
#include "ingress.h"
#include "ipsec_defines.h"

struct tx_table_s0_t0_k k;
struct tx_table_s0_t0_ipsec_encap_txdma_initial_table_d d;
struct phv_ p;

%%
        .param ipsec_encap_txdma_deque_head_desc
        .param ipsec_encap_txdma_load_head_desc_int_header
        .param ipsec_get_barco_req_index_ptr
        .align
ipsec_encap_txdma_initial_table:
    phvwr p.p4_intr_global_lif, k.{p4_intr_global_lif_sbit0_ebit2...p4_intr_global_lif_sbit3_ebit10}
    phvwr p.p4_intr_global_tm_iq, k.p4_intr_global_tm_iq
    phvwr p.p4_txdma_intr_qtype, k.p4_txdma_intr_qtype
    phvwr p.p4_txdma_intr_qid, k.p4_txdma_intr_qid
    phvwr p.p4_txdma_intr_qstate_addr, k.{p4_txdma_intr_qstate_addr_sbit0_ebit1...p4_txdma_intr_qstate_addr_sbit2_ebit33}

    add r1, r0, d.ipsec_cb_index
    sll r1, r1, IPSEC_CB_SIZE_SHIFT
    addi r1, r1, IPSEC_CB_BASE 
    phvwr p.txdma1_global_ipsec_cb_addr, r1 
    phvwr p.txdma1_global_in_desc_addr, d.head_desc_addr
    
    phvwr p.barco_req_brq_barco_enc_cmd, d.barco_enc_cmd
    addi r2, r1, IPSEC_CB_IV_OFFSET
    phvwr p.barco_req_brq_iv_addr, r2
    phvwr p.barco_req_brq_key_index, d.key_index
    
    phvwri p.app_header_table0_valid, 1 
    phvwri p.common_te0_phv_table_lock_en, 1 
    phvwri p.common_te0_phv_table_pc, ipsec_encap_txdma_deque_head_desc
    phvwri p.common_te0_phv_table_raw_table_size, 6
    add r1, r0, d.head_desc_addr
    addi r1, r1, 64
    phvwr p.common_te0_phv_table_addr, r1

    phvwri p.app_header_table1_valid, 1 
    phvwri p.common_te1_phv_table_lock_en, 1 
    phvwri p.common_te1_phv_table_pc, ipsec_encap_txdma_load_head_desc_int_header 
    phvwri p.common_te1_phv_table_raw_table_size, 6
    phvwr p.common_te1_phv_table_addr, d.head_desc_addr

    phvwri p.app_header_table2_valid, 1 
    phvwri p.common_te2_phv_table_lock_en, 1 
    phvwri p.common_te2_phv_table_pc, ipsec_get_barco_req_index_ptr 
    phvwri p.common_te2_phv_table_raw_table_size, 3
    phvwr p.common_te2_phv_table_addr, d.head_desc_addr

    



