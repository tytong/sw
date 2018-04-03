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
    and r1, d.{out_page_ring_index}.dx, 0x3FF 
    sll r1, r1, 3 
    addui r1, r1, hiword(TNMPR_TABLE_BASE)
    addi r1, r1, loword(TNMPR_TABLE_BASE)
    phvwr p.common_te3_phv_table_addr, r1
    phvwri p.{common_te3_phv_table_lock_en...common_te3_phv_table_raw_table_size}, ((1 << 3) | 3)
    phvwri.f p.common_te3_phv_table_pc, esp_ipv4_tunnel_h2n_allocate_output_page_index[33:6] 
    nop.e 
    nop
