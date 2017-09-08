#include "ingress.h"
#include "INGRESS_p.h"
#include "ipsec_asm_defines.h"

struct rx_table_s1_t3_k k;
struct rx_table_s1_t3_allocate_output_page_semaphore_d d;
struct phv_ p;

%%
        .param esp_ipv4_tunnel_h2n_allocate_output_page_index
        .param TNMPR_TABLE_BASE
        .align

esp_ipv4_tunnel_h2n_allocate_output_page_semaphore:
    phvwri p.app_header_table3_valid, 1
    sll r1, d.out_page_ring_index, 3 
    addi r1, r1, TNMPR_TABLE_BASE 
    phvwr p.common_te3_phv_table_addr, r1
    phvwri p.common_te3_phv_table_raw_table_size, 3
    phvwri p.common_te3_phv_table_lock_en, 1
    addi r2, r0, esp_ipv4_tunnel_h2n_allocate_output_page_index
    srl r2, r2, 6
    phvwr p.common_te3_phv_table_pc, r2 
    nop.e 
    nop
