/*****************************************************************************
 *  nvme_be_cq_handler: Save the NVME status into PHV. Load the saved R2N WQE
 *                      for the command that was sent to the SSD
 *****************************************************************************/

#include "storage_asm_defines.h"
#include "ingress.h"
#include "INGRESS_p.h"

struct s1_tbl_k k;
struct s1_tbl_nvme_be_cq_handler_d d;
struct phv_ p;

%%
   .param storage_tx_nvme_be_wqe_handler_start

storage_tx_nvme_be_cq_handler_start:

   // Update the queue doorbell to clear the scheduler bit
   QUEUE_POP_DOORBELL_UPDATE

   // Save the R2N WQE to PHV
   phvwr	p.{nvme_be_sta_cspec...nvme_be_sta_status_phase},	\
   		d.{cspec...status_phase}

   // Set the state information for the NVME backend status header
   // TODO: FIXME
   phvwri	p.nvme_be_sta_hdr_time_us, 0
   phvwri	p.nvme_be_sta_hdr_be_status, 0
   phvwri	p.nvme_be_sta_hdr_is_q0, 0
   phvwri	p.nvme_be_sta_hdr_rsvd, 0

   // Store the SSD's c_ndx value for DMA to the NVME backend SQ
   add		r1, r0, d.sq_head
   phvwr	p.ssd_ci_c_ndx, r1.hx

   // Setup the DMA command to push the sq_head to the c_ndx of the SSD
   DMA_PHV2MEM_SETUP(ssd_ci_c_ndx, ssd_ci_c_ndx, 
                     STORAGE_KIVEC1_SSD_CI_ADDR, dma_p2m_2)

   // Obtain the saved command index from the command id in the status
   // and save it in the PHV. Store the result in GPR r6 to pass as input
   // to SSD_CMD_ENTRY_ADDR_CALC
   add		r6, d.cid, r0
   add		r6, r0, r6.hx
   andi		r6, r6, 0xFF
   phvwr	p.r2n_wqe_nvme_cmd_cid, r6
   phvwr	p.storage_kivec0_cmd_index, r6

   // Calculate the table address based on the command index offset into
   // the SSD's list of outstanding commands. Output is stored in GPR r7.
   SSD_CMD_ENTRY_ADDR_CALC

   // Set the table and program address 
   LOAD_TABLE_FOR_ADDR_PARAM(r7, STORAGE_DEFAULT_TBL_LOAD_SIZE,
                             storage_tx_nvme_be_wqe_handler_start)

exit:
   nop.e
   nop
