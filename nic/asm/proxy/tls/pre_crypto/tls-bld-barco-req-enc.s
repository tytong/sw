/*
 * 	Construct the barco request in this stage
 */

#include "tls-constants.h"
#include "tls-phv.h"
#include "tls-shared-state.h"
#include "tls-macros.h"
#include "tls-table.h"
#include "tls-sched.h"
#include "ingress.h"
#include "INGRESS_p.h"
	
struct tx_table_s4_t0_k k                  ;
struct phv_ p	;
struct tx_table_s4_t0_tls_bld_brq4_d d	;
	
%%
	    .param      tls_queue_brq_enc_process
#	    .param		BRQ_QPCB_BASE
        
tls_bld_barco_req_enc_process:        

table_read_QUEUE_BRQ:
    /* Fill the barco request in the phv to be DMAed later into BRQ slot */
    phvwr       p.barco_desc_status_address, k.{to_s4_idesc}.dx
    phvwr       p.barco_desc_input_list_address, k.{to_s4_idesc}.dx
    CAPRI_OPERAND_DEBUG(k.to_s4_idesc)

    phvwr       p.barco_desc_output_list_address, k.{to_s4_odesc}.dx
    CAPRI_OPERAND_DEBUG(k.to_s4_odesc)

    phvwr       p.barco_desc_key_desc_index, d.{key_desc_index}.dx
    CAPRI_OPERAND_DEBUG(d.key_desc_index)

    phvwr       p.barco_desc_command_core,  d.command_core
    CAPRI_OPERAND_DEBUG(d.command_core)

    phvwr       p.barco_desc_command_mode, d.command_mode
    CAPRI_OPERAND_DEBUG(d.command_mode)

    phvwr       p.barco_desc_command_op, d.command_op
    CAPRI_OPERAND_DEBUG(d.command_op)
        
    phvwr       p.barco_desc_command_param, d.command_param
    CAPRI_OPERAND_DEBUG(d.command_param)

	/* address will be in r4 */
	CAPRI_RING_DOORBELL_ADDR(0, DB_IDX_UPD_PIDX_INC, DB_SCHED_UPD_SET, 0, LIF_TLS)
    phvwr       p.barco_desc_doorbell_address, r4
    CAPRI_OPERAND_DEBUG(r4)

    
	/* data will be in r3 */
	CAPRI_RING_DOORBELL_DATA(0, k.tls_global_phv_fid, TLS_SCHED_RING_BSQ, 0)
    phvwr       p.barco_desc_doorbell_data, r3.dx
    CAPRI_OPERAND_DEBUG(r3)
        
    addi        r3, r0, BRQ_QPCB_BASE        
    CAPRI_NEXT_TABLE0_READ(k.tls_global_phv_fid, TABLE_LOCK_EN, tls_queue_brq_enc_process,
                           r3, TLS_TCB_TABLE_ENTRY_SIZE_SHFT,
                       	   0, TABLE_SIZE_512_BITS)
	nop.e
	nop
