#include "INGRESS_p.h"
#include "ingress.h"
#include "ipsec_asm_defines.h"

struct tx_table_s1_t1_k k;
struct tx_table_s1_t1_esp_v4_tunnel_n2h_allocate_barco_req_pindex_d d;
struct phv_ p;

%%
       .param BRQ_BASE
        .align
esp_v4_tunnel_n2h_txdma1_allocate_barco_req_pindex:
    phvwri p.app_header_table1_valid, 0
    add r1, r0, d.barco_pindex
    andi r1, r1, 0x3F
    sll r1, r1, BRQ_RING_ENTRY_SIZE_SHIFT
    addi r1, r1, BRQ_BASE
    phvwr p.ipsec_to_stage2_barco_req_addr, r1
    phvwr p.ipsec_to_stage3_barco_req_addr, r1
    nop.e
    nop 
