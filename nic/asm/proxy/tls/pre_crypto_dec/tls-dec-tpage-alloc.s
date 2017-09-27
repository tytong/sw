/*
 *	Implements the tnmdr ring page read of the TxDMA P4+ tls  pipeline
 */

#include "tls-phv.h"
#include "tls-shared-state.h"
#include "tls-macros.h"
#include "tls-table.h"
#include "ingress.h"
#include "INGRESS_p.h"

 /* d is the data returned by lookup result */
struct d_struct {
	opage			 : DESC_ADDRESS_WIDTH ;
    pad              : 448                ;
};

/* Readonly Parsed packet header info for the current packet */
struct tx_table_s2_t3_k k             ;
struct phv_ p;
struct d_struct d;

%%
	
	    .align
tls_dec_tpage_alloc_process:

        CAPRI_CLEAR_TABLE2_VALID

	    phvwr		p.to_s5_opage, d.opage
	    phvwr		p.to_s6_opage, d.opage
        phvwri      p.to_s7_tnmpr_alloc, 1
	    nop.e
	    nop

