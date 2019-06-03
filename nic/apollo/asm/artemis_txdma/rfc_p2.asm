#include "../../p4/include/artemis_sacl_defines.h"
#include "artemis_txdma.h"
#include "INGRESS_p.h"
#include "INGRESS_rfc_p2_k.h"

struct phv_                p;
struct rfc_p2_k_           k;

%%

rfc_p2:
    /* Compute the index into the classid array */
    mod        r7, k.txdma_control_rfc_index, 51
    mul        r7, r7, 10
    /* Access the classid at the index */
    tblrdp     r7, r7, 0, 10
    /* Load sacl base addr to r1 */
    add        r1, r0, k.rx_to_tx_hdr_sacl_base_addr
    /* Is this the first pass? */
    seq        c1, k.capri_p4_intr_recirc_count, 0
    /* If so, add SACL_P3_1_TABLE_OFFSET to sacl base address. */
    addi.c1    r1, r1, SACL_P3_1_TABLE_OFFSET
    /* Else, add SACL_P3_3_TABLE_OFFSET to sacl base address. */
    addi.!c1   r1, r1, SACL_P3_3_TABLE_OFFSET
    /* P3 table index = ((p1_classid << 10) | p2_classid). */
    add        r2, r7, k.txdma_control_rfc_p1_classid, 10
    /* Write P3 table index to PHV */
    phvwr      p.txdma_control_rfc_index, r2
    /* Compute the byte offset for P3 table index */
    div        r2, r2, 51
    mul        r2, r2, 64
    /* Add the byte offset to table base */
    add.e      r1, r1, r2
    /* Write the address back to phv for P3 lookup */
    phvwr      p.txdma_control_rfc_table_addr, r1

/*****************************************************************************/
/* error function                                                            */
/*****************************************************************************/
.align
.assert $ < ASM_INSTRUCTION_OFFSET_MAX
rfc_p2_error:
    phvwr.e         p.capri_intr_drop, 1
    nop
