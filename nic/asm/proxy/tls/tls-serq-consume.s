/*
 * 	Doorbell write to clear the sched bit for the SERQ having
 *      finished the consumption processing.
 */

#include "tls-constants.h"
#include "tls-phv.h"
#include "tls-shared-state.h"
#include "tls-macros.h"
#include "tls-table.h"
#include "tls-sched.h"
#include "ingress.h"
#include "INGRESS_p.h"
	

/* SERQ consumer index */
        
struct tx_table_s2_t3_k k                  ;
struct phv_ p	;

	
%%
	
tls_serq_consume_process_start:
	/* SERQ_cidx got incremented due to the auto-inc read address used */
	/* address will be in r4 */
	CAPRI_RING_DOORBELL_ADDR(0, DB_IDX_UPD_CIDX_SET, DB_SCHED_UPD_EVAL, 0, LIF_TLS)
	add		r1, k.tls_global_phv_fid, r0
	/* data will be in r3 */
	CAPRI_RING_DOORBELL_DATA(0, r1, TLS_SCHED_RING_SERQ, k.to_s2_serq_ci)
	memwr.d		r4, r3
	nop.e
	nop
