#include "INGRESS_p.h"
#include "ingress.h"
#include "ipsec_asm_defines.h"

struct tx_table_s3_t1_k k;
struct tx_table_s3_t1_ipsec_encap_txdma2_load_out_desc_d d;
struct phv_ p;

%%
        .param ipsec_build_encap_packet 
        .align
ipsec_encap_txdma2_load_out_desc:
    phvwr p.t0_s2s_out_page_addr, d.addr0
    phvwri p.app_header_table0_valid, 0
    nop.e
 
