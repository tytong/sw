/*****************************************************************************
 * storage_seq.p4: Storage_seq P4+ program that implements sequencer chaining.
 *****************************************************************************/

#include "common/storage_seq_p4_hdr.h"

#include "../common-p4+/common_txdma_dummy.p4"

#define tx_table_s0_t0          s0_tbl
#define tx_table_s1_t0          s1_tbl
#define tx_table_s2_t0          s2_tbl
#define tx_table_s3_t0          s3_tbl

#define tx_table_s1_t1          s1_tbl1
#define tx_table_s3_t1          s3_tbl1
#define tx_table_s3_t2          s3_tbl2
#define tx_table_s3_t3          s3_tbl3

#define tx_table_s4_t0          s4_tbl
#define tx_table_s4_t1          s4_tbl1
#define tx_table_s4_t2          s4_tbl2

#define tx_table_s5_t1          s5_tbl1
#define tx_table_s5_t2          s5_tbl2
#define tx_table_s5_t3          s5_tbl3

#define tx_table_s0_t0_action   seq_q_state_pop

#define tx_table_s1_t0_action   seq_barco_entry_handler
#define tx_table_s1_t0_action1  seq_comp_status_desc0_handler
#define tx_table_s1_t0_action2  seq_xts_status_desc0_handler

#define tx_table_s1_t1_action   seq_comp_status_desc1_handler
#define tx_table_s1_t1_action1  seq_xts_status_desc1_handler

#define tx_table_s2_t0_action   seq_comp_status_handler
#define tx_table_s2_t0_action1  seq_xts_status_handler
#define tx_table_s2_t0_action2  seq_barco_ring_pndx_pre_read0

#define tx_table_s3_t0_action   seq_barco_ring_pndx_read

#define tx_table_s3_t1_action   seq_comp_sgl_pdma_xfer
#define tx_table_s3_t1_action1  seq_xts_sgl_pdma_xfer
#define tx_table_s3_t2_action   seq_comp_sgl_pad_only_xfer
#define tx_table_s3_t2_action1  seq_xts_comp_len_update
#define tx_table_s3_t3_action   seq_comp_aol_pad_handler

#define tx_table_s4_t0_action   seq_barco_chain_action
#define tx_table_s4_t1_action   seq_comp_db_intr_override
#define tx_table_s4_t1_action1  seq_xts_db_intr_override

#define tx_table_s5_t1_action   seq_metrics0_commit
#define tx_table_s5_t2_action   seq_metrics1_commit
#define tx_table_s5_t3_action   seq_metrics2_commit

#include "../common-p4+/common_txdma.p4"


/*****************************************************************************
 * Storage Sequencer PHV layout BEGIN
 * Will be processed by NCC in this order
 *****************************************************************************/

// Global and stage to stage K+I vectors
@pragma pa_header_union ingress common_t0_s2s
metadata seq_kivec0_t seq_kivec0;
@pragma pa_header_union ingress common_global
metadata seq_kivec1_t seq_kivec1;

// Certain macros in storage_p4_hdr.h refer to storage_kivec0/1 by name
// so create alias for them.
@pragma dont_trim
@pragma pa_header_union ingress common_t0_s2s
metadata seq_kivec0_t storage_kivec0;
@pragma dont_trim
@pragma pa_header_union ingress common_global
metadata seq_kivec1_t storage_kivec1;

@pragma pa_header_union ingress common_t0_s2s
metadata seq_kivec4_t seq_kivec4;
@pragma pa_header_union ingress common_global
metadata seq_kivec5_t seq_kivec5;
@pragma pa_header_union ingress common_global
metadata seq_kivec5xts_t seq_kivec5xts;
@pragma pa_header_union ingress to_stage_2
metadata seq_kivec2_t seq_kivec2;
@pragma pa_header_union ingress to_stage_2
metadata seq_kivec2xts_t seq_kivec2xts;
@pragma pa_header_union ingress to_stage_3
metadata seq_kivec3_t seq_kivec3;
@pragma pa_header_union ingress to_stage_3
metadata seq_kivec3xts_t seq_kivec3xts;
@pragma pa_header_union ingress common_t3_s2s
metadata seq_kivec6_t seq_kivec6;
@pragma pa_header_union ingress common_t2_s2s
metadata seq_kivec7xts_t seq_kivec7xts;
@pragma pa_header_union ingress common_t1_s2s
metadata seq_kivec8_t seq_kivec8;
@pragma pa_header_union ingress to_stage_5
metadata seq_kivec9_t seq_kivec9;
@pragma pa_header_union ingress to_stage_4
metadata seq_kivec10_t seq_kivec10;

// Push/Pop doorbells
@pragma dont_trim
metadata storage_doorbell_data_t seq_doorbell_data;

// Interrupt data across PCI bus
@pragma dont_trim
metadata storage_pci_data_t pci_intr_data;

// Barco ring doorbell data
@pragma dont_trim
metadata barco_ring_t barco_doorbell_data;

// Accelerator chaining states
@pragma dont_trim
metadata storage_capri_len32_t last_blk_len;
@pragma dont_trim
metadata storage_capri_len8_t null_byte;
@pragma dont_trim
metadata seq_comp_hdr_t comp_hdr;
@pragma dont_trim
@pragma pa_align 512
metadata barco_sgl_tuple2_pad_t barco_sgl_tuple2_pad;
@pragma dont_trim
@pragma pa_header_union ingress barco_sgl_tuple2_pad
metadata barco_sgl_tuple1_pad_t barco_sgl_tuple1_pad;
@pragma dont_trim
@pragma pa_header_union ingress barco_sgl_tuple2_pad
metadata barco_sgl_tuple0_pad_t barco_sgl_tuple0_pad;

@pragma dont_trim
@pragma pa_header_union ingress barco_sgl_tuple2_pad
metadata barco_sgl_tuple0_len_update_t barco_sgl_tuple0_len_update;
@pragma dont_trim
@pragma pa_header_union ingress barco_sgl_tuple2_pad
metadata barco_sgl_tuple1_len_update_t barco_sgl_tuple1_len_update;
@pragma dont_trim
@pragma pa_header_union ingress barco_sgl_tuple2_pad
metadata barco_sgl_tuple2_len_update_t barco_sgl_tuple2_len_update;

// DMA commands metadata
@pragma dont_trim
@pragma pa_align 512
metadata dma_cmd_phv2mem_t dma_p2m_0;
@pragma dont_trim
@pragma pa_header_union ingress dma_p2m_0
metadata dma_cmd_mem2mem_t dma_m2m_0;

@pragma dont_trim
metadata dma_cmd_phv2mem_t dma_p2m_1;
@pragma dont_trim
@pragma pa_header_union ingress dma_p2m_1
metadata dma_cmd_mem2mem_t dma_m2m_1;

@pragma dont_trim
metadata dma_cmd_phv2mem_t dma_p2m_2;
@pragma dont_trim
@pragma pa_header_union ingress dma_p2m_2
metadata dma_cmd_mem2mem_t dma_m2m_2;

@pragma dont_trim
metadata dma_cmd_phv2mem_t dma_p2m_3;
@pragma dont_trim
@pragma pa_header_union ingress dma_p2m_3
metadata dma_cmd_mem2mem_t dma_m2m_3;

@pragma dont_trim
metadata dma_cmd_phv2mem_t dma_p2m_4;
@pragma dont_trim
@pragma pa_header_union ingress dma_p2m_4
metadata dma_cmd_mem2mem_t dma_m2m_4;

@pragma dont_trim
metadata dma_cmd_phv2mem_t dma_p2m_5;
@pragma dont_trim
@pragma pa_header_union ingress dma_p2m_5
metadata dma_cmd_mem2mem_t dma_m2m_5;

@pragma dont_trim
metadata dma_cmd_phv2mem_t dma_p2m_6;
@pragma dont_trim
@pragma pa_header_union ingress dma_p2m_6
metadata dma_cmd_mem2mem_t dma_m2m_6;

@pragma dont_trim
metadata dma_cmd_phv2mem_t dma_p2m_7;
@pragma dont_trim
@pragma pa_header_union ingress dma_p2m_7
metadata dma_cmd_mem2mem_t dma_m2m_7;

@pragma dont_trim
metadata dma_cmd_phv2mem_t dma_p2m_8;
@pragma dont_trim
@pragma pa_header_union ingress dma_p2m_8
metadata dma_cmd_mem2mem_t dma_m2m_8;

@pragma dont_trim
metadata dma_cmd_phv2mem_t dma_p2m_9;
@pragma dont_trim
@pragma pa_header_union ingress dma_p2m_9
metadata dma_cmd_mem2mem_t dma_m2m_9;

@pragma dont_trim
metadata dma_cmd_phv2mem_t dma_p2m_10;
@pragma dont_trim
@pragma pa_header_union ingress dma_p2m_10
metadata dma_cmd_mem2mem_t dma_m2m_10;

@pragma dont_trim
metadata dma_cmd_phv2mem_t dma_p2m_11;
@pragma dont_trim
@pragma pa_header_union ingress dma_p2m_11
metadata dma_cmd_mem2mem_t dma_m2m_11;

@pragma dont_trim
metadata dma_cmd_phv2mem_t dma_p2m_12;
@pragma dont_trim
@pragma pa_header_union ingress dma_p2m_12
metadata dma_cmd_mem2mem_t dma_m2m_12;

@pragma dont_trim
metadata dma_cmd_phv2mem_t dma_p2m_13;
@pragma dont_trim
@pragma pa_header_union ingress dma_p2m_13
metadata dma_cmd_mem2mem_t dma_m2m_13;

@pragma dont_trim
metadata dma_cmd_phv2mem_t dma_p2m_14;
@pragma dont_trim
@pragma pa_header_union ingress dma_p2m_14
metadata dma_cmd_mem2mem_t dma_m2m_14;

@pragma dont_trim
metadata dma_cmd_phv2mem_t dma_p2m_15;
@pragma dont_trim
@pragma pa_header_union ingress dma_p2m_15
metadata dma_cmd_mem2mem_t dma_m2m_15;

@pragma dont_trim
metadata dma_cmd_phv2mem_t dma_p2m_16;
@pragma dont_trim
@pragma pa_header_union ingress dma_p2m_16
metadata dma_cmd_mem2mem_t dma_m2m_16;

@pragma dont_trim
metadata dma_cmd_phv2mem_t dma_p2m_17;
@pragma dont_trim
@pragma pa_header_union ingress dma_p2m_17
metadata dma_cmd_mem2mem_t dma_m2m_17;

@pragma dont_trim
metadata dma_cmd_phv2mem_t dma_p2m_18;
@pragma dont_trim
@pragma pa_header_union ingress dma_p2m_18
metadata dma_cmd_mem2mem_t dma_m2m_18;

@pragma dont_trim
metadata dma_cmd_phv2mem_t dma_p2m_19;
@pragma dont_trim
@pragma pa_header_union ingress dma_p2m_19
metadata dma_cmd_mem2mem_t dma_m2m_19;


/*****************************************************************************
 * Storage Sequencer PHV layout END
 *****************************************************************************/


// Scratch metadatas to get d-vector generated correctly

@pragma scratch_metadata
metadata seq_q_state_t q_state_scratch;

@pragma scratch_metadata
metadata barco_ring_t barco_ring_scratch;

@pragma scratch_metadata
metadata storage_capri_addr_t doorbell_addr_scratch;

@pragma scratch_metadata
metadata storage_capri_addr_t pci_intr_addr_scratch;

@pragma scratch_metadata
metadata seq_kivec0_t seq_kivec0_scratch;

@pragma scratch_metadata
metadata seq_kivec1_t seq_kivec1_scratch;

@pragma scratch_metadata
metadata seq_kivec2_t seq_kivec2_scratch;

@pragma scratch_metadata
metadata seq_kivec2xts_t seq_kivec2xts_scratch;

@pragma scratch_metadata
metadata seq_kivec3_t seq_kivec3_scratch;

@pragma scratch_metadata
metadata seq_kivec3xts_t seq_kivec3xts_scratch;

@pragma scratch_metadata
metadata seq_kivec4_t seq_kivec4_scratch;

@pragma scratch_metadata
metadata seq_kivec5_t seq_kivec5_scratch;

@pragma scratch_metadata
metadata seq_kivec5xts_t seq_kivec5xts_scratch;

@pragma scratch_metadata
metadata seq_kivec6_t seq_kivec6_scratch;

@pragma scratch_metadata
metadata seq_kivec7xts_t seq_kivec7xts_scratch;

@pragma scratch_metadata
metadata seq_kivec8_t seq_kivec8_scratch;

@pragma scratch_metadata
metadata seq_kivec9_t seq_kivec9_scratch;

@pragma scratch_metadata
metadata seq_kivec10_t seq_kivec10_scratch;

@pragma scratch_metadata
metadata seq_desc_entry_t seq_desc_entry_scratch;

@pragma scratch_metadata
metadata seq_comp_status_desc0_t seq_comp_status_desc0_scratch;

@pragma scratch_metadata
metadata seq_comp_status_desc1_t seq_comp_status_desc1_scratch;

@pragma scratch_metadata
metadata seq_comp_status_t seq_comp_status_scratch;

@pragma scratch_metadata
metadata chain_sgl_pdma_t seq_comp_sgl_scratch;

@pragma scratch_metadata
metadata barco_sgl_t barco_sgl_scratch;

@pragma scratch_metadata
metadata seq_xts_status_desc0_t seq_xts_status_desc0_scratch;

@pragma scratch_metadata
metadata seq_xts_status_desc1_t seq_xts_status_desc1_scratch;

@pragma scratch_metadata
metadata seq_xts_status_t seq_xts_status_scratch;

@pragma scratch_metadata
metadata chain_sgl_pdma_t seq_xts_sgl_scratch;

@pragma scratch_metadata
metadata seq_comp_hdr_t seq_comp_hdr;

@pragma scratch_metadata
metadata barco_aol_t barco_aol_scratch;

@pragma scratch_metadata
metadata cp_desc_t cp_desc_scratch;

@pragma scratch_metadata
metadata seq_q_state_metrics0_t seq_metrics0;

@pragma scratch_metadata
metadata seq_q_state_metrics1_t seq_metrics1;

@pragma scratch_metadata
metadata seq_q_state_metrics2_t seq_metrics2;

/*****************************************************************************
 * Storage Sequencer BEGIN
 *****************************************************************************/

/*****************************************************************************
 * exit: Exit action handler needs to be stubbed out for NCC
 *****************************************************************************/

action exit() {
}


/*****************************************************************************
 *  seq_q_state_pop : Check the queue state and see if there's anything to be
 *                    popped. If so increment the working index and load the
 *                    queue entry.
 *****************************************************************************/
@pragma little_endian p_ndx c_ndx
action seq_q_state_pop(/*pc_offset, */rsvd, cosA, cosB, cos_sel, eval_last,
                       host_rings, total_rings, pid, p_ndx, c_ndx, w_ndx,
                       num_entries, base_addr, entry_size, next_pc,
                       desc1_next_pc, enable, abort, desc1_next_pc_valid, pad) {

  // For D vector generation (type inference). No need to translate this to ASM.
  SEQ_Q_STATE_COPY_STAGE0(q_state_scratch)

  // If queue is empty exit
  if (QUEUE_EMPTY(q_state_scratch)) {
    exit();
  } else {
    // Increment the working consumer index. In ASM this should be a table write.
    QUEUE_POP(q_state_scratch)

    // In ASM, derive these from the K+I for stage 0
    modify_field(seq_kivec1.src_qaddr, 0);
    modify_field(seq_kivec1.src_lif, 0);
    modify_field(seq_kivec1.src_qtype, 0);
    modify_field(seq_kivec1.src_qid, 0);

    // Load the table and program for processing the queue entry in the next stage
    CAPRI_LOAD_TABLE_IDX(common_te0_phv, q_state_scratch.base_addr,
                         q_state_scratch.c_ndx, q_state_scratch.entry_size,
                         q_state_scratch.entry_size, q_state_scratch.next_pc)
  }
}

/*****************************************************************************
 *  seq_barco_entry_handler: Handle sequencer descriptor. Form the
 *                           DMA command to copy the HW descriptor as
 *                           part of the push operation in the next stage.
 *****************************************************************************/

action seq_barco_entry_handler(barco_desc_addr, barco_pndx_addr, barco_pndx_shadow_addr,
			       barco_ring_addr, barco_desc_size, barco_pndx_size,
			       barco_ring_size, batch_mode, rate_limit_src_en,
			       rate_limit_dst_en, rate_limit_en, rsvd0,
			       batch_size, rsvd1,
			       src_data_len, dst_data_len) {

  // Store the K+I vector into scratch to get the K+I generated correctly
  SEQ_KIVEC4_USE(seq_kivec4_scratch, seq_kivec4)

  // For D vector generation (type inference). No need to translate this to ASM.
  modify_field(seq_desc_entry_scratch.barco_desc_addr, barco_desc_addr);
  modify_field(seq_desc_entry_scratch.barco_pndx_addr, barco_pndx_addr);
  modify_field(seq_desc_entry_scratch.barco_pndx_shadow_addr, barco_pndx_shadow_addr);
  modify_field(seq_desc_entry_scratch.barco_ring_addr, barco_ring_addr);
  modify_field(seq_desc_entry_scratch.barco_desc_size, barco_desc_size);
  modify_field(seq_desc_entry_scratch.barco_pndx_size, barco_pndx_size);
  modify_field(seq_desc_entry_scratch.barco_ring_size, barco_ring_size);
  modify_field(seq_desc_entry_scratch.batch_mode, batch_mode);
  modify_field(seq_desc_entry_scratch.rate_limit_src_en, rate_limit_src_en);
  modify_field(seq_desc_entry_scratch.rate_limit_dst_en, rate_limit_dst_en);
  modify_field(seq_desc_entry_scratch.rate_limit_en, rate_limit_en);
  modify_field(seq_desc_entry_scratch.rsvd0, rsvd0);
  modify_field(seq_desc_entry_scratch.batch_size, batch_size);
  modify_field(seq_desc_entry_scratch.rsvd1, rsvd1);
  modify_field(seq_desc_entry_scratch.src_data_len, src_data_len);
  modify_field(seq_desc_entry_scratch.dst_data_len, dst_data_len);

  // Update the K+I vector with the HW descriptor size to be used
  // when calculating the offset for the push operation
  modify_field(seq_kivec4.barco_desc_size, seq_desc_entry_scratch.barco_desc_size);
  modify_field(seq_kivec4.barco_ring_size, seq_desc_entry_scratch.barco_ring_size);

  // Form the doorbell and setup the DMA command to pop the entry by writing
  // w_ndx to c_ndx
  //QUEUE_POP_DOORBELL_UPDATE

  // Setup the DMA command to move the data from source to destination address
  // In ASM, set the host, fence bits etc correctly
  DMA_COMMAND_MEM2MEM_FILL(dma_m2m_1, dma_m2m_2, seq_desc_entry_scratch.barco_desc_addr, 0,
                           0, 0, seq_desc_entry_scratch.barco_desc_size, 0, 0, 0)

  // Setup the doorbell to be rung based on a fence with the previous mem2mem
  // DMA. Form the doorbell DMA command in this stage as opposed the push
  // stage (as is the norm) to avoid carrying the doorbell address in K+I
  // vector.
  modify_field(doorbell_addr_scratch.addr, seq_desc_entry_scratch.barco_pndx_addr);
  DMA_COMMAND_PHV2MEM_FILL(dma_p2m_3,
                           0,
                           PHV_FIELD_OFFSET(qpush_doorbell_data.data),
                           PHV_FIELD_OFFSET(qpush_doorbell_data.data),
                           0, 0, 0, 0)

  // Advance to common stage for launching HW ring pindex read.
  CAPRI_LOAD_TABLE_NO_LKUP(common_te0_phv, seq_barco_ring_pndx_pre_read0_start)
}

/*****************************************************************************
 *  seq_barco_ring_pndx_pre_read0: Prep stage for arriving at a common stage
 *                                 for launching table lock read to get the Barco
 *                                 ring pindex.
 *****************************************************************************/

action seq_barco_ring_pndx_pre_read0() {

  CAPRI_LOAD_TABLE_NO_LKUP(common_te0_phv, seq_barco_ring_pndx_read_start)
}

/*****************************************************************************
 *  seq_barco_ring_pndx_read: Common stage for launching
 *                            table lock read to get the Barco ring pindex.
 *****************************************************************************/

action seq_barco_ring_pndx_read() {

  // Store the K+I vector into scratch to get the K+I generated correctly
  SEQ_KIVEC4_USE(seq_kivec4_scratch, seq_kivec4)

  // Load the Barco ring for the next stage to push the Barco descriptor
  CAPRI_LOAD_TABLE_ADDR(common_te0_phv,
                        seq_kivec4.barco_pndx_shadow_addr,
                        seq_kivec4.barco_pndx_size,
                        seq_barco_chain_action_start)
}

/*****************************************************************************
 *  seq_comp_status_desc0_handler: Handle the compression status descriptor entry in the
 *                         sequencer. This involves:
 *                          1. processing status to see if operation succeeded
 *                          2. breaking up the compressed data into the
 *                             destination SGL provided in the descriptor
 *                         In this stage, load the status entry for next stage
 *                         and save the other fields into I vector.
 *****************************************************************************/

//@pragma little_endian next_db_addr next_db_data status_addr0 status_addr1 intr_addr intr_data status_len
@pragma little_endian intr_data
action seq_comp_status_desc0_handler(next_db_addr, next_db_data,
                                     barco_pndx_addr, barco_pndx_shadow_addr,
                                     barco_desc_size, barco_pndx_size, barco_ring_size,
                                     barco_num_descs,
                                     status_addr0, status_addr1,
                                     intr_addr, intr_data, status_len,
				     status_offset0, status_dma_en,
                                     next_db_en, intr_en,
				     next_db_action_barco_push, rate_limit_src_en,
				     rate_limit_dst_en, rate_limit_en,
				     rsvd0, num_alt_descs, rsvd1) {

  // Store the K+I vector into scratch to get the K+I generated correctly
  SEQ_KIVEC1_USE(seq_kivec1_scratch, seq_kivec1)
  SEQ_KIVEC4_USE(seq_kivec4_scratch, seq_kivec4)
  SEQ_KIVEC5_USE(seq_kivec5_scratch, seq_kivec5)

  // For D vector generation (type inference). No need to translate this to ASM.
  modify_field(seq_comp_status_desc0_scratch.next_db_addr, next_db_addr);
  modify_field(seq_comp_status_desc0_scratch.next_db_data, next_db_data);
  modify_field(seq_comp_status_desc0_scratch.barco_pndx_addr, barco_pndx_addr);
  modify_field(seq_comp_status_desc0_scratch.barco_pndx_shadow_addr, barco_pndx_shadow_addr);
  modify_field(seq_comp_status_desc0_scratch.barco_desc_size, barco_desc_size);
  modify_field(seq_comp_status_desc0_scratch.barco_pndx_size, barco_pndx_size);
  modify_field(seq_comp_status_desc0_scratch.barco_ring_size, barco_ring_size);
  modify_field(seq_comp_status_desc0_scratch.barco_num_descs, barco_num_descs);
  modify_field(seq_comp_status_desc0_scratch.status_addr0, status_addr0);
  modify_field(seq_comp_status_desc0_scratch.status_addr1, status_addr1);
  modify_field(seq_comp_status_desc0_scratch.intr_addr, intr_addr);
  modify_field(seq_comp_status_desc0_scratch.intr_data, intr_data);
  modify_field(seq_comp_status_desc0_scratch.status_len, status_len);
  modify_field(seq_comp_status_desc0_scratch.status_offset0, status_offset0);
  modify_field(seq_comp_status_desc0_scratch.status_dma_en, status_dma_en);
  modify_field(seq_comp_status_desc0_scratch.next_db_en, next_db_en);
  modify_field(seq_comp_status_desc0_scratch.intr_en, intr_en);
  modify_field(seq_comp_status_desc0_scratch.next_db_action_barco_push, next_db_action_barco_push);
  modify_field(seq_comp_status_desc0_scratch.rate_limit_src_en, rate_limit_src_en);
  modify_field(seq_comp_status_desc0_scratch.rate_limit_dst_en, rate_limit_dst_en);
  modify_field(seq_comp_status_desc0_scratch.rate_limit_en, rate_limit_en);
  modify_field(seq_comp_status_desc0_scratch.rsvd0, rsvd0);
  modify_field(seq_comp_status_desc0_scratch.num_alt_descs, num_alt_descs);
  modify_field(seq_comp_status_desc0_scratch.rsvd1, rsvd1);

  // Store the various parts of the descriptor in the K+I vectors for later use
  modify_field(seq_kivec4.barco_ring_addr, seq_comp_status_desc0_scratch.next_db_addr);
  modify_field(seq_kivec4.barco_pndx_shadow_addr, seq_comp_status_desc0_scratch.barco_pndx_shadow_addr);
  modify_field(seq_kivec4.barco_desc_size, seq_comp_status_desc0_scratch.barco_desc_size);
  modify_field(seq_kivec4.barco_pndx_size, seq_comp_status_desc0_scratch.barco_pndx_size);
  modify_field(seq_kivec4.barco_ring_size, seq_comp_status_desc0_scratch.barco_ring_size);
  modify_field(seq_kivec4.barco_num_descs, seq_comp_status_desc0_scratch.barco_num_descs);
  modify_field(seq_kivec5.status_dma_en, seq_comp_status_desc0_scratch.status_dma_en);
  modify_field(seq_kivec5.next_db_en, seq_comp_status_desc0_scratch.next_db_en);
  modify_field(seq_kivec5.intr_en, seq_comp_status_desc0_scratch.intr_en);
  modify_field(seq_kivec5.next_db_action_barco_push, seq_comp_status_desc0_scratch.next_db_action_barco_push);
  modify_field(seq_kivec5.rate_limit_src_en, seq_comp_status_desc0_scratch.rate_limit_src_en);
  modify_field(seq_kivec5.rate_limit_dst_en, seq_comp_status_desc0_scratch.rate_limit_dst_en);
  modify_field(seq_kivec5.rate_limit_en, seq_comp_status_desc0_scratch.rate_limit_en);
  modify_field(seq_kivec10.num_alt_descs, seq_comp_status_desc0_scratch.num_alt_descs);

  // Setup the doorbell to be rung if the doorbell enabled is set.
  // Fence with the SGL mem2mem DMA for ordering.
  if (seq_comp_status_desc0_scratch.next_db_en == 1) {
    // Copy the doorbell addr and data
    modify_field(doorbell_addr_scratch.addr, seq_comp_status_desc0_scratch.next_db_addr);
    modify_field(seq_doorbell_data.data, seq_comp_status_desc0_scratch.next_db_addr);
    DMA_COMMAND_PHV2MEM_FILL(dma_p2m_11,
                             0,
                             PHV_FIELD_OFFSET(seq_doorbell_data.data),
                             PHV_FIELD_OFFSET(seq_doorbell_data.data),
                             0, 0, 0, 0)
  }

  // Fire the interrupt if there is no doorbell to be rung and if the
  // interrupt enabled bit is set. Fence with the SGL mem2mem DMA
  // for ordering.
  if ((seq_comp_status_desc0_scratch.next_db_en ==  0) and
      (seq_comp_status_desc0_scratch.intr_en == 1)) {
    // Copy the doorbell addr and data
    modify_field(pci_intr_addr_scratch.addr, seq_comp_status_desc0_scratch.intr_addr);
    modify_field(pci_intr_data.data, seq_comp_status_desc0_scratch.intr_data);
    DMA_COMMAND_PHV2MEM_FILL(dma_p2m_11,
                             0,
                             PHV_FIELD_OFFSET(pci_intr_data.data),
                             PHV_FIELD_OFFSET(pci_intr_data.data),
                             0, 0, 0, 0)
  }

  // Form the doorbell and setup the DMA command to pop the entry by writing
  // w_ndx to c_ndx
  //QUEUE_POP_DOORBELL_UPDATE

  // Load the address where compression status is stored for processing
  // in the next stage
  CAPRI_LOAD_TABLE_ADDR(common_te0_phv,
                        seq_comp_status_desc0_scratch.status_addr0,
                        STORAGE_DEFAULT_TBL_LOAD_SIZE,
                        seq_comp_status_handler_start)
}

/*****************************************************************************
 *  seq_comp_status_desc1_handler: Part 2 of the comp status decriptor
 *****************************************************************************/

//@pragma little_endian rsvd comp_buf_addr aol_src_vec_addr aol_dst_vec_addr data_len sgl_vec_addr pad_buf_addr alt_buf_addr
@pragma little_endian hdr_version
action seq_comp_status_desc1_handler(comp_buf_addr, aol_src_vec_addr, aol_dst_vec_addr,
                                     sgl_vec_addr, pad_buf_addr, alt_buf_addr,
                                     data_len, hdr_version, rsvd0,
                                     pad_boundary_shift, stop_chain_on_error,
                                     data_len_from_desc, aol_update_en, sgl_update_en,
                                     sgl_sparse_format_en, sgl_pdma_en, sgl_pdma_pad_only,
				     sgl_pdma_alt_src_on_error, desc_vec_push_en,
				     chain_alt_desc_on_error, integ_data0_wr_en,
				     integ_data_null_en, desc_dlen_update_en,
				     hdr_version_wr_en, cp_hdr_update_en,
				     status_len_no_hdr, padding_en,
				     rsvd1, alt_data_len) {

  // Store the K+I vector into scratch to get the K+I generated correctly
  SEQ_KIVEC5_USE(seq_kivec5_scratch, seq_kivec5)

  // For D vector generation (type inference). No need to translate this to ASM.
  modify_field(seq_comp_status_desc1_scratch.comp_buf_addr, comp_buf_addr);
  modify_field(seq_comp_status_desc1_scratch.aol_src_vec_addr, aol_src_vec_addr);
  modify_field(seq_comp_status_desc1_scratch.aol_dst_vec_addr, aol_dst_vec_addr);
  modify_field(seq_comp_status_desc1_scratch.sgl_vec_addr, sgl_vec_addr);
  modify_field(seq_comp_status_desc1_scratch.pad_buf_addr, pad_buf_addr);
  modify_field(seq_comp_status_desc1_scratch.alt_buf_addr, alt_buf_addr);
  modify_field(seq_comp_status_desc1_scratch.data_len, data_len);
  modify_field(seq_comp_status_desc1_scratch.hdr_version, hdr_version);
  modify_field(seq_comp_status_desc1_scratch.rsvd0, rsvd0);
  modify_field(seq_comp_status_desc1_scratch.pad_boundary_shift, pad_boundary_shift);
  modify_field(seq_comp_status_desc1_scratch.stop_chain_on_error, stop_chain_on_error);
  modify_field(seq_comp_status_desc1_scratch.data_len_from_desc, data_len_from_desc);
  modify_field(seq_comp_status_desc1_scratch.aol_update_en, aol_update_en);
  modify_field(seq_comp_status_desc1_scratch.sgl_update_en, sgl_update_en);
  modify_field(seq_comp_status_desc1_scratch.sgl_sparse_format_en, sgl_sparse_format_en);
  modify_field(seq_comp_status_desc1_scratch.sgl_pdma_en, sgl_pdma_en);
  modify_field(seq_comp_status_desc1_scratch.sgl_pdma_pad_only, sgl_pdma_pad_only);
  modify_field(seq_comp_status_desc1_scratch.sgl_pdma_alt_src_on_error, sgl_pdma_alt_src_on_error);
  modify_field(seq_comp_status_desc1_scratch.desc_vec_push_en, desc_vec_push_en);
  modify_field(seq_comp_status_desc1_scratch.chain_alt_desc_on_error, chain_alt_desc_on_error);
  modify_field(seq_comp_status_desc1_scratch.integ_data0_wr_en, integ_data0_wr_en);
  modify_field(seq_comp_status_desc1_scratch.integ_data_null_en, integ_data_null_en);
  modify_field(seq_comp_status_desc1_scratch.desc_dlen_update_en, desc_dlen_update_en);
  modify_field(seq_comp_status_desc1_scratch.hdr_version_wr_en, hdr_version_wr_en);
  modify_field(seq_comp_status_desc1_scratch.cp_hdr_update_en, cp_hdr_update_en);
  modify_field(seq_comp_status_desc1_scratch.status_len_no_hdr, status_len_no_hdr);
  modify_field(seq_comp_status_desc1_scratch.padding_en, padding_en);
  modify_field(seq_comp_status_desc1_scratch.rsvd1, rsvd1);
  modify_field(seq_comp_status_desc1_scratch.alt_data_len, alt_data_len);

  // Store the various parts of the descriptor in the K+I vectors for later use
  modify_field(seq_kivec5.pad_buf_addr, seq_comp_status_desc1_scratch.pad_buf_addr);
  modify_field(seq_kivec5.data_len, seq_comp_status_desc1_scratch.data_len);
  modify_field(seq_kivec4.pad_boundary_shift, seq_comp_status_desc1_scratch.pad_boundary_shift);
  modify_field(seq_kivec3.pad_boundary_shift, seq_comp_status_desc1_scratch.pad_boundary_shift);
  modify_field(seq_kivec5.stop_chain_on_error, seq_comp_status_desc1_scratch.stop_chain_on_error);
  modify_field(seq_kivec5.data_len_from_desc, seq_comp_status_desc1_scratch.data_len_from_desc);
  modify_field(seq_kivec5.aol_update_en, seq_comp_status_desc1_scratch.aol_update_en);
  modify_field(seq_kivec5.sgl_update_en, seq_comp_status_desc1_scratch.sgl_update_en);
  modify_field(seq_kivec5.sgl_sparse_format_en, seq_comp_status_desc1_scratch.sgl_sparse_format_en);
  modify_field(seq_kivec5.sgl_pdma_en, seq_comp_status_desc1_scratch.sgl_pdma_en);
  modify_field(seq_kivec5.sgl_pdma_pad_only, seq_comp_status_desc1_scratch.sgl_pdma_pad_only);
  modify_field(seq_kivec5.sgl_pdma_alt_src_on_error, seq_comp_status_desc1_scratch.sgl_pdma_alt_src_on_error);
  modify_field(seq_kivec5.desc_vec_push_en, seq_comp_status_desc1_scratch.desc_vec_push_en);
  modify_field(seq_kivec5.chain_alt_desc_on_error, seq_comp_status_desc1_scratch.chain_alt_desc_on_error);
  modify_field(seq_kivec5.integ_data0_wr_en, seq_comp_status_desc1_scratch.integ_data0_wr_en);
  modify_field(seq_kivec5.integ_data_null_en, seq_comp_status_desc1_scratch.integ_data_null_en);
  modify_field(seq_kivec5.desc_dlen_update_en, seq_comp_status_desc1_scratch.desc_dlen_update_en);
  modify_field(seq_kivec5.hdr_version_wr_en, seq_comp_status_desc1_scratch.hdr_version_wr_en);
  modify_field(seq_kivec5.cp_hdr_update_en, seq_comp_status_desc1_scratch.cp_hdr_update_en);
  modify_field(seq_kivec5.status_len_no_hdr, seq_comp_status_desc1_scratch.status_len_no_hdr);
  modify_field(seq_kivec5.padding_en, seq_comp_status_desc1_scratch.padding_en);
  modify_field(seq_kivec8.alt_buf_addr, seq_comp_status_desc1_scratch.alt_buf_addr);
  modify_field(seq_kivec5.alt_data_len, seq_comp_status_desc1_scratch.alt_data_len);
}

/*****************************************************************************
 *  seq_comp_status_handler: Store the compression status in K+I vector. Load
 *                           SGL address for next stage to do the PDMA.
 *****************************************************************************/

@pragma little_endian status output_data_len partial_data
action seq_comp_status_handler(status, output_data_len, partial_data,
                               integ_data0, integ_data1) {

  // Store the K+I vector into scratch to get the K+I generated correctly
  SEQ_KIVEC2_USE(seq_kivec2_scratch, seq_kivec2)
  SEQ_KIVEC4_USE(seq_kivec4_scratch, seq_kivec4)
  SEQ_KIVEC5_USE(seq_kivec5_scratch, seq_kivec5)

  // For D vector generation (type inference). No need to translate this to ASM.
  modify_field(seq_comp_status_scratch.status, status);
  modify_field(seq_comp_status_scratch.output_data_len, output_data_len);
  modify_field(seq_comp_status_scratch.partial_data, partial_data);
  modify_field(seq_comp_status_scratch.integ_data0, integ_data0);
  modify_field(seq_comp_status_scratch.integ_data1, integ_data1);

  // Store the data length in the K+I vector for later use if the descriptor
  // has not provided this information
  if (seq_kivec5.data_len_from_desc == 0) {
    modify_field(seq_kivec5.data_len, seq_comp_status_scratch.output_data_len);
  }

  // Load the address where compression destination SGL is stored for
  // processing in the next stage
  CAPRI_LOAD_TABLE_ADDR(common_te1_phv,
                        seq_kivec2.sgl_pdma_dst_addr,
                        STORAGE_DEFAULT_TBL_LOAD_SIZE,
                        seq_comp_sgl_pdma_xfer_start)
}

/*****************************************************************************
 *  seq_barco_chain_action: Push to Barco ring by issuing the mem2mem DMA
 *                          commands and incrementing the p_ndx via ringing the
 *                          doorbell. Assumes that data to be pushed has its
 *                          source in DMA cmd 1 and destination in DMA cmd 2.
 *****************************************************************************/

@pragma little_endian p_ndx
action seq_barco_chain_action(p_ndx) {

  // Store the K+I vector into scratch to get the K+I generated correctly
  SEQ_KIVEC4_USE(seq_kivec4_scratch, seq_kivec4)
  SEQ_KIVEC10_USE(seq_kivec10_scratch, seq_kivec10)

  // For D vector generation (type inference). No need to translate this to ASM.
  modify_field(barco_ring_scratch.p_ndx, p_ndx);

  // Copy the doorbell data to PHV
  modify_field(barco_doorbell_data.p_ndx, barco_ring_scratch.p_ndx);

  // Modify the DMA command 2 to fill the destination address based on p_ndx
  // NOTE: This API in P4 land will not work, but in ASM we can selectively
  // overwrite the fields
  DMA_COMMAND_PHV2MEM_FILL(dma_m2m_10,
                           seq_kivec4.barco_ring_addr +
                           (barco_ring_scratch.p_ndx *
                            seq_kivec4.barco_desc_size),
                           0, 0,
                           0, 0, 0, 0)


  // Doorbell has already been setup

  // Exit the pipeline here
}

/*****************************************************************************
 *  seq_comp_db_intr_override
 *****************************************************************************/

action seq_comp_db_intr_override() {

  // Store the K+I vector into scratch to get the K+I generated correctly
  SEQ_KIVEC5_USE(seq_kivec5_scratch, seq_kivec5)
  SEQ_KIVEC10_USE(seq_kivec10_scratch, seq_kivec10)
}

/*****************************************************************************
 *  seq_comp_sgl_pdma_xfer: Parse the destination SGL and DMA the data from
 *                          comp_buf_addr.
 *****************************************************************************/

@pragma little_endian addr0 addr1 addr2 addr3 len0 len1 len2 len3
action seq_comp_sgl_pdma_xfer(addr0, len0, addr1, len1,
                              addr2, len2, addr3, len3,
                              pad0, pad1) {

  // Store the K+I vector into scratch to get the K+I generated correctly
  SEQ_KIVEC3_USE(seq_kivec3_scratch, seq_kivec3)
  SEQ_KIVEC5_USE(seq_kivec5_scratch, seq_kivec5)
  SEQ_KIVEC8_USE(seq_kivec8_scratch, seq_kivec8)

  // For D vector generation (type inference). No need to translate this to ASM.
  modify_field(seq_comp_sgl_scratch.addr0, addr0);
  modify_field(seq_comp_sgl_scratch.len0, len0);
  modify_field(seq_comp_sgl_scratch.addr1, addr1);
  modify_field(seq_comp_sgl_scratch.len1, len1);
  modify_field(seq_comp_sgl_scratch.addr2, addr2);
  modify_field(seq_comp_sgl_scratch.len2, len2);
  modify_field(seq_comp_sgl_scratch.addr3, addr3);
  modify_field(seq_comp_sgl_scratch.len3, len3);
  modify_field(seq_comp_sgl_scratch.pad0, pad0);
  modify_field(seq_comp_sgl_scratch.pad1, pad1);

  // DMA to SGL 0
  if (seq_kivec5.data_len <= seq_comp_sgl_scratch.len0) {
    DMA_COMMAND_MEM2MEM_FILL(dma_m2m_2, dma_m2m_3,
                             seq_kivec3.comp_buf_addr, 0,
                             seq_comp_sgl_scratch.addr0, 0,
                             seq_kivec5.data_len,
                             0, 0, 0)
    exit();
  } else {
    DMA_COMMAND_MEM2MEM_FILL(dma_m2m_2, dma_m2m_3,
                             seq_kivec3.comp_buf_addr, 0,
                             seq_comp_sgl_scratch.addr0, 0,
                             seq_comp_sgl_scratch.len0,
                             0, 0, 0)
    modify_field(seq_kivec5.data_len,
                 (seq_kivec5.data_len - seq_comp_sgl_scratch.len0));
  }

  // DMA to SGL 1
  if (seq_kivec5.data_len <= seq_comp_sgl_scratch.len1) {
    DMA_COMMAND_MEM2MEM_FILL(dma_m2m_4, dma_m2m_5,
                             seq_kivec3.comp_buf_addr, 0,
                             seq_comp_sgl_scratch.addr1, 0,
                             seq_kivec5.data_len,
                             0, 0, 0)
    exit();
  } else {
    DMA_COMMAND_MEM2MEM_FILL(dma_m2m_4, dma_m2m_5,
                             seq_kivec3.comp_buf_addr, 0,
                             seq_comp_sgl_scratch.addr1, 0,
                             seq_comp_sgl_scratch.len1,
                             0, 0, 0)
    modify_field(seq_kivec5.data_len,
                 (seq_kivec5.data_len - seq_comp_sgl_scratch.len1));
  }

  // DMA to SGL 2
  if (seq_kivec5.data_len <= seq_comp_sgl_scratch.len2) {
    DMA_COMMAND_MEM2MEM_FILL(dma_m2m_6, dma_m2m_7,
                             seq_kivec3.comp_buf_addr, 0,
                             seq_comp_sgl_scratch.addr2, 0,
                             seq_kivec5.data_len,
                             0, 0, 0)
    exit();
  } else {
    DMA_COMMAND_MEM2MEM_FILL(dma_m2m_6, dma_m2m_7,
                             seq_kivec3.comp_buf_addr, 0,
                             seq_comp_sgl_scratch.addr2, 0,
                             seq_comp_sgl_scratch.len2,
                             0, 0, 0)
    modify_field(seq_kivec5.data_len,
                 (seq_kivec5.data_len - seq_comp_sgl_scratch.len2));
  }

  // DMA to SGL 3
  if (seq_kivec5.data_len <= seq_comp_sgl_scratch.len3) {
    DMA_COMMAND_MEM2MEM_FILL(dma_m2m_8, dma_m2m_9,
                             seq_kivec3.comp_buf_addr, 0,
                             seq_comp_sgl_scratch.addr3, 0,
                             seq_kivec5.data_len,
                             0, 0, 0)
    exit();
  } else {
    DMA_COMMAND_MEM2MEM_FILL(dma_m2m_8, dma_m2m_9,
                             seq_kivec3.comp_buf_addr, 0,
                             seq_comp_sgl_scratch.addr3, 0,
                             seq_comp_sgl_scratch.len3,
                             0, 0, 0)
    modify_field(seq_kivec5.data_len,
                 (seq_kivec5.data_len - seq_comp_sgl_scratch.len3));
  }

  // Exit the pipeline here
}


/*****************************************************************************
 *  seq_comp_sgl_pad_only: DMA only padding data to
 *****************************************************************************/

@pragma little_endian addr0 addr1 addr2 len0 len1 len2 link
#ifdef ELBA
action seq_comp_sgl_pad_only_xfer(addr0, rsvd0, len0,
                                  addr1, rsvd1, len1,
                                  addr2, rsvd2, len2,
                                  link, rsvd) {
#else
action seq_comp_sgl_pad_only_xfer(addr0, len0, rsvd0,
                                  addr1, len1, rsvd1,
                                  addr2, len2, rsvd2,
                                  link, rsvd) {
#endif

  // Store the K+I vector into scratch to get the K+I generated correctly
  SEQ_KIVEC3_USE(seq_kivec3_scratch, seq_kivec3)
  SEQ_KIVEC5_USE(seq_kivec5_scratch, seq_kivec5)

  // For D vector generation (type inference). No need to translate this to ASM.
  modify_field(barco_sgl_scratch.addr0, addr0);
  modify_field(barco_sgl_scratch.len0, len0);
  modify_field(barco_sgl_scratch.rsvd0, rsvd0);
  modify_field(barco_sgl_scratch.addr1, addr1);
  modify_field(barco_sgl_scratch.len1, len1);
  modify_field(barco_sgl_scratch.rsvd1, rsvd1);
  modify_field(barco_sgl_scratch.addr2, addr2);
  modify_field(barco_sgl_scratch.len2, len2);
  modify_field(barco_sgl_scratch.rsvd2, rsvd2);
  modify_field(barco_sgl_scratch.link, link);
  modify_field(barco_sgl_scratch.rsvd, rsvd);

  if (seq_kivec3.pad_len > 0) {
    if (seq_kivec3.sgl_tuple_no == 0) {
      DMA_COMMAND_MEM2MEM_FILL(dma_m2m_4, dma_m2m_5,
                               seq_kivec5.pad_buf_addr, 0,
                               barco_sgl_scratch.addr0 +  seq_kivec3.last_blk_len,
                               0, seq_kivec3.pad_len,
                               0, 0, 0)
    }
    if (seq_kivec3.sgl_tuple_no == 1) {
      DMA_COMMAND_MEM2MEM_FILL(dma_m2m_4, dma_m2m_5,
                               seq_kivec5.pad_buf_addr, 0,
                               barco_sgl_scratch.addr1 +  seq_kivec3.last_blk_len,
                               0, seq_kivec3.pad_len,
                               0, 0, 0)
    }
    if (seq_kivec3.sgl_tuple_no == 2) {
      DMA_COMMAND_MEM2MEM_FILL(dma_m2m_4, dma_m2m_5,
                               seq_kivec5.pad_buf_addr, 0,
                               barco_sgl_scratch.addr2 +  seq_kivec3.last_blk_len,
                               0, seq_kivec3.pad_len,
                               0, 0, 0)
    }
  }

  // Exit the pipeline here
}


/*****************************************************************************
 *  seq_comp_aol_pad_handler: apply padding to source AOL and destination AOL
 *****************************************************************************/

action seq_comp_aol_pad_handler() {

  // Store the K+I vector into scratch to get the K+I generated correctly
  SEQ_KIVEC3_USE(seq_kivec3_scratch, seq_kivec3)
  SEQ_KIVEC5_USE(seq_kivec5_scratch, seq_kivec5)
  SEQ_KIVEC6_USE(seq_kivec6_scratch, seq_kivec6)
}

/*****************************************************************************
 *  seq_xts_status_desc0_handler: Handle the XTS status descriptor entry in the
 *                                sequencer.
 *****************************************************************************/

//@pragma little_endian next_db_addr next_db_data status_addr1 status_addr2 intr_addr intr_data status_len
@pragma little_endian intr_data
action seq_xts_status_desc0_handler(next_db_addr, next_db_data,
                                    barco_pndx_addr, barco_pndx_shadow_addr,
                                    barco_desc_size, barco_pndx_size, barco_ring_size,
                                    barco_num_descs,
                                    status_addr0, status_addr1,
                                    intr_addr, intr_data, status_len,
				    status_offset0, status_dma_en,
                                    next_db_en, intr_en,
				    next_db_action_barco_push, rate_limit_src_en,
				    rate_limit_dst_en, rate_limit_en) {

  // Store the K+I vector into scratch to get the K+I generated correctly
  SEQ_KIVEC4_USE(seq_kivec4_scratch, seq_kivec4)
  SEQ_KIVEC5XTS_USE(seq_kivec5xts_scratch, seq_kivec5xts)

  // For D vector generation (type inference). No need to translate this to ASM.
  modify_field(seq_xts_status_desc0_scratch.next_db_addr, next_db_addr);
  modify_field(seq_xts_status_desc0_scratch.next_db_data, next_db_data);
  modify_field(seq_xts_status_desc0_scratch.barco_pndx_addr, barco_pndx_addr);
  modify_field(seq_xts_status_desc0_scratch.barco_pndx_shadow_addr, barco_pndx_shadow_addr);
  modify_field(seq_xts_status_desc0_scratch.barco_desc_size, barco_desc_size);
  modify_field(seq_xts_status_desc0_scratch.barco_pndx_size, barco_pndx_size);
  modify_field(seq_xts_status_desc0_scratch.barco_ring_size, barco_ring_size);
  modify_field(seq_xts_status_desc0_scratch.barco_num_descs, barco_num_descs);
  modify_field(seq_xts_status_desc0_scratch.status_addr0, status_addr0);
  modify_field(seq_xts_status_desc0_scratch.status_addr1, status_addr1);
  modify_field(seq_xts_status_desc0_scratch.intr_addr, intr_addr);
  modify_field(seq_xts_status_desc0_scratch.intr_data, intr_data);
  modify_field(seq_xts_status_desc0_scratch.status_len, status_len);
  modify_field(seq_xts_status_desc0_scratch.status_offset0, status_offset0);
  modify_field(seq_xts_status_desc0_scratch.status_dma_en, status_dma_en);
  modify_field(seq_xts_status_desc0_scratch.next_db_en, next_db_en);
  modify_field(seq_xts_status_desc0_scratch.intr_en, intr_en);
  modify_field(seq_xts_status_desc0_scratch.next_db_action_barco_push, next_db_action_barco_push);
  modify_field(seq_xts_status_desc0_scratch.rate_limit_src_en, rate_limit_src_en);
  modify_field(seq_xts_status_desc0_scratch.rate_limit_dst_en, rate_limit_dst_en);
  modify_field(seq_xts_status_desc0_scratch.rate_limit_en, rate_limit_en);

  // Store the various parts of the descriptor in the K+I vectors for later use
  modify_field(seq_kivec4.barco_ring_addr, seq_xts_status_desc0_scratch.next_db_addr);
  modify_field(seq_kivec4.barco_pndx_shadow_addr, seq_xts_status_desc0_scratch.barco_pndx_shadow_addr);
  modify_field(seq_kivec4.barco_desc_size, seq_xts_status_desc0_scratch.barco_desc_size);
  modify_field(seq_kivec4.barco_pndx_size, seq_xts_status_desc0_scratch.barco_pndx_size);
  modify_field(seq_kivec4.barco_ring_size, seq_xts_status_desc0_scratch.barco_ring_size);
  modify_field(seq_kivec4.barco_num_descs, seq_xts_status_desc0_scratch.barco_num_descs);
  modify_field(seq_kivec7xts.comp_desc_addr, seq_xts_status_desc0_scratch.next_db_data);
  modify_field(seq_kivec5xts.status_dma_en, seq_xts_status_desc0_scratch.status_dma_en);
  modify_field(seq_kivec5xts.next_db_en, seq_xts_status_desc0_scratch.next_db_en);
  modify_field(seq_kivec5xts.intr_en, seq_xts_status_desc0_scratch.intr_en);
  modify_field(seq_kivec5xts.next_db_action_barco_push, seq_xts_status_desc0_scratch.next_db_action_barco_push);
  modify_field(seq_kivec5xts.rate_limit_src_en, seq_xts_status_desc0_scratch.rate_limit_src_en);
  modify_field(seq_kivec5xts.rate_limit_dst_en, seq_xts_status_desc0_scratch.rate_limit_dst_en);
  modify_field(seq_kivec5xts.rate_limit_en, seq_xts_status_desc0_scratch.rate_limit_en);

  // Setup the doorbell to be rung if the doorbell enabled is set.
  // Fence with the SGL mem2mem DMA for ordering.
  if (seq_xts_status_desc0_scratch.next_db_en == 1) {
    // Copy the doorbell addr and data
    modify_field(doorbell_addr_scratch.addr, seq_xts_status_desc0_scratch.next_db_addr);
    modify_field(seq_doorbell_data.data, seq_xts_status_desc0_scratch.next_db_addr);
    DMA_COMMAND_PHV2MEM_FILL(dma_p2m_11,
                             0,
                             PHV_FIELD_OFFSET(seq_doorbell_data.data),
                             PHV_FIELD_OFFSET(seq_doorbell_data.data),
                             0, 0, 0, 0)
  }

  // Fire the interrupt if there is no doorbell to be rung and if the
  // interrupt enabled bit is set. Fence with the SGL mem2mem DMA
  // for ordering.
  if ((seq_xts_status_desc0_scratch.next_db_en ==  0) and
      (seq_xts_status_desc0_scratch.intr_en == 1)) {
    // Copy the doorbell addr and data
    modify_field(pci_intr_addr_scratch.addr, seq_xts_status_desc0_scratch.intr_addr);
    modify_field(pci_intr_data.data, seq_xts_status_desc0_scratch.intr_data);
    DMA_COMMAND_PHV2MEM_FILL(dma_p2m_11,
                             0,
                             PHV_FIELD_OFFSET(pci_intr_data.data),
                             PHV_FIELD_OFFSET(pci_intr_data.data),
                             0, 0, 0, 0)
  }

  // Form the doorbell and setup the DMA command to pop the entry by writing
  // w_ndx to c_ndx
  //QUEUE_POP_DOORBELL_UPDATE

  // Load the address where compression status is stored for processing
  // in the next stage
  CAPRI_LOAD_TABLE_ADDR(common_te0_phv,
                        seq_xts_status_desc0_scratch.status_addr0,
                        STORAGE_TBL_LOAD_SIZE_64_BITS,
                        seq_xts_status_handler_start)
}

/*****************************************************************************
 *  seq_xts_status_desc1_handler: Part 2 of the XTS status decriptor
 *****************************************************************************/

//@pragma little_endian comp_sgl_src_addr sgl_pdma_dst_addr decr_buf_addr
action seq_xts_status_desc1_handler(comp_sgl_src_addr, sgl_pdma_dst_addr, decr_buf_addr,
                                    data_len, blk_boundary_shift, stop_chain_on_error,
                                    comp_len_update_en, comp_sgl_src_en, comp_sgl_src_vec_en,
				    sgl_sparse_format_en, sgl_pdma_en, sgl_pdma_len_from_desc,
				    desc_vec_push_en) {

  // Store the K+I vector into scratch to get the K+I generated correctly
  SEQ_KIVEC5XTS_USE(seq_kivec5xts_scratch, seq_kivec5xts)
  SEQ_KIVEC8_USE(seq_kivec8_scratch, seq_kivec8)

  // For D vector generation (type inference). No need to translate this to ASM.
  modify_field(seq_xts_status_desc1_scratch.comp_sgl_src_addr, comp_sgl_src_addr);
  modify_field(seq_xts_status_desc1_scratch.sgl_pdma_dst_addr, sgl_pdma_dst_addr);
  modify_field(seq_xts_status_desc1_scratch.decr_buf_addr, decr_buf_addr);
  modify_field(seq_xts_status_desc1_scratch.data_len, data_len);
  modify_field(seq_xts_status_desc1_scratch.blk_boundary_shift, blk_boundary_shift);
  modify_field(seq_xts_status_desc1_scratch.stop_chain_on_error, stop_chain_on_error);
  modify_field(seq_xts_status_desc1_scratch.comp_len_update_en, comp_len_update_en);
  modify_field(seq_xts_status_desc1_scratch.comp_sgl_src_en, comp_sgl_src_en);
  modify_field(seq_xts_status_desc1_scratch.comp_sgl_src_vec_en, comp_sgl_src_vec_en);
  modify_field(seq_xts_status_desc1_scratch.sgl_sparse_format_en, sgl_sparse_format_en);
  modify_field(seq_xts_status_desc1_scratch.sgl_pdma_en, sgl_pdma_en);
  modify_field(seq_xts_status_desc1_scratch.sgl_pdma_len_from_desc, sgl_pdma_len_from_desc);
  modify_field(seq_xts_status_desc1_scratch.desc_vec_push_en, desc_vec_push_en);

  // Store the various parts of the descriptor in the K+I vectors for later use
  modify_field(seq_kivec2xts.sgl_pdma_dst_addr, seq_xts_status_desc1_scratch.sgl_pdma_dst_addr);
  modify_field(seq_kivec2xts.decr_buf_addr, seq_xts_status_desc1_scratch.decr_buf_addr);
  modify_field(seq_kivec7xts.comp_sgl_src_addr, seq_xts_status_desc1_scratch.comp_sgl_src_addr);
  modify_field(seq_kivec5xts.data_len, seq_xts_status_desc1_scratch.data_len);
  modify_field(seq_kivec5xts.blk_boundary_shift, seq_xts_status_desc1_scratch.blk_boundary_shift);
  modify_field(seq_kivec5xts.stop_chain_on_error, seq_xts_status_desc1_scratch.stop_chain_on_error);
  modify_field(seq_kivec5xts.comp_len_update_en, seq_xts_status_desc1_scratch.comp_len_update_en);
  modify_field(seq_kivec5xts.comp_sgl_src_en, seq_xts_status_desc1_scratch.comp_sgl_src_en);
  modify_field(seq_kivec5xts.comp_sgl_src_vec_en, seq_xts_status_desc1_scratch.comp_sgl_src_vec_en);
  modify_field(seq_kivec5xts.sgl_sparse_format_en, seq_xts_status_desc1_scratch.sgl_sparse_format_en);
  modify_field(seq_kivec5xts.sgl_pdma_en, seq_xts_status_desc1_scratch.sgl_pdma_en);
  modify_field(seq_kivec5xts.sgl_pdma_len_from_desc, seq_xts_status_desc1_scratch.sgl_pdma_len_from_desc);
  modify_field(seq_kivec5xts.desc_vec_push_en, seq_xts_status_desc1_scratch.desc_vec_push_en);
}

/*****************************************************************************
 *  seq_xts_status_handler: Store the compression status in K+I vector.
 *****************************************************************************/

action seq_xts_status_handler(err) {

  // Store the K+I vector into scratch to get the K+I generated correctly
  SEQ_KIVEC2XTS_USE(seq_kivec2xts_scratch, seq_kivec2xts)
  SEQ_KIVEC5XTS_USE(seq_kivec5xts_scratch, seq_kivec5xts)

  // For D vector generation (type inference). No need to translate this to ASM.
  modify_field(seq_xts_status_scratch.err, err);
}


/*****************************************************************************
 *  seq_xts_sgl_pdma_xfer: Parse the destination SGL and DMA the data from
 *                         decr_buf_addr.
 *****************************************************************************/

@pragma little_endian addr0 addr1 addr2 addr3 len0 len1 len2 len3
action seq_xts_sgl_pdma_xfer(addr0, len0, addr1, len1,
                             addr2, len2, addr3, len3,
                             pad0, pad1) {

  // Store the K+I vector into scratch to get the K+I generated correctly
  SEQ_KIVEC3XTS_USE(seq_kivec3xts_scratch, seq_kivec3xts)
  SEQ_KIVEC5XTS_USE(seq_kivec5xts_scratch, seq_kivec5xts)
  SEQ_KIVEC8_USE(seq_kivec8_scratch, seq_kivec8)

  // For D vector generation (type inference). No need to translate this to ASM.
  modify_field(seq_xts_sgl_scratch.addr0, addr0);
  modify_field(seq_xts_sgl_scratch.len0, len0);
  modify_field(seq_xts_sgl_scratch.addr1, addr1);
  modify_field(seq_xts_sgl_scratch.len1, len1);
  modify_field(seq_xts_sgl_scratch.addr2, addr2);
  modify_field(seq_xts_sgl_scratch.len2, len2);
  modify_field(seq_xts_sgl_scratch.addr3, addr3);
  modify_field(seq_xts_sgl_scratch.len3, len3);
  modify_field(seq_xts_sgl_scratch.pad0, pad0);
  modify_field(seq_xts_sgl_scratch.pad1, pad1);

  // DMA to SGL 0
  if (seq_kivec5xts.data_len <= seq_xts_sgl_scratch.len0) {
    DMA_COMMAND_MEM2MEM_FILL(dma_m2m_6, dma_m2m_7,
                             seq_kivec3xts.decr_buf_addr, 0,
                             seq_xts_sgl_scratch.addr0, 0,
                             seq_kivec5xts.data_len,
                             0, 0, 0)
    exit();
  } else {
    DMA_COMMAND_MEM2MEM_FILL(dma_m2m_6, dma_m2m_7,
                             seq_kivec3xts.decr_buf_addr, 0,
                             seq_xts_sgl_scratch.addr0, 0,
                             seq_xts_sgl_scratch.len0,
                             0, 0, 0)
    modify_field(seq_kivec5xts.data_len,
                 (seq_kivec5xts.data_len - seq_xts_sgl_scratch.len0));
  }

  // DMA to SGL 1
  if (seq_kivec5xts.data_len <= seq_xts_sgl_scratch.len1) {
    DMA_COMMAND_MEM2MEM_FILL(dma_m2m_8, dma_m2m_9,
                             seq_kivec3xts.decr_buf_addr, 0,
                             seq_xts_sgl_scratch.addr1, 0,
                             seq_kivec5xts.data_len,
                             0, 0, 0)
    exit();
  } else {
    DMA_COMMAND_MEM2MEM_FILL(dma_m2m_8, dma_m2m_9,
                             seq_kivec3xts.decr_buf_addr, 0,
                             seq_xts_sgl_scratch.addr1, 0,
                             seq_xts_sgl_scratch.len1,
                             0, 0, 0)
    modify_field(seq_kivec5xts.data_len,
                 (seq_kivec5xts.data_len - seq_xts_sgl_scratch.len1));
  }

  // DMA to SGL 2
  if (seq_kivec5xts.data_len <= seq_xts_sgl_scratch.len2) {
    DMA_COMMAND_MEM2MEM_FILL(dma_m2m_10, dma_m2m_11,
                             seq_kivec3xts.decr_buf_addr, 0,
                             seq_xts_sgl_scratch.addr2, 0,
                             seq_kivec5xts.data_len,
                             0, 0, 0)
    exit();
  } else {
    DMA_COMMAND_MEM2MEM_FILL(dma_m2m_10, dma_m2m_11,
                             seq_kivec3xts.decr_buf_addr, 0,
                             seq_xts_sgl_scratch.addr2, 0,
                             seq_xts_sgl_scratch.len2,
                             0, 0, 0)
    modify_field(seq_kivec5xts.data_len,
                 (seq_kivec5xts.data_len - seq_xts_sgl_scratch.len2));
  }

  // DMA to SGL 3
  if (seq_kivec5xts.data_len <= seq_xts_sgl_scratch.len3) {
    DMA_COMMAND_MEM2MEM_FILL(dma_m2m_12, dma_m2m_13,
                             seq_kivec3xts.decr_buf_addr, 0,
                             seq_xts_sgl_scratch.addr3, 0,
                             seq_kivec5xts.data_len,
                             0, 0, 0)
    exit();
  } else {
    DMA_COMMAND_MEM2MEM_FILL(dma_m2m_12, dma_m2m_13,
                             seq_kivec3xts.decr_buf_addr, 0,
                             seq_xts_sgl_scratch.addr3, 0,
                             seq_xts_sgl_scratch.len3,
                             0, 0, 0)
    modify_field(seq_kivec5xts.data_len,
                 (seq_kivec5xts.data_len - seq_xts_sgl_scratch.len3));
  }

  // Exit the pipeline here
}

/*****************************************************************************
 *  seq_xts_comp_len_update: On XTS completion (presumably a decryption), read
 *                           the compression header from the decrypted data to find
 *                           the actual data length. Use it to update next-in-chain
 *                           compression descriptor and SGL source.
 *****************************************************************************/

@pragma little_endian cksum data_len version
action seq_xts_comp_len_update(cksum, data_len, version) {

  // Store the K+I vector into scratch to get the K+I generated correctly
  SEQ_KIVEC7XTS_USE(seq_kivec7xts_scratch, seq_kivec7xts)
  SEQ_KIVEC5XTS_USE(seq_kivec5xts_scratch, seq_kivec5xts)

  // For D vector generation (type inference). No need to translate this to ASM.
  modify_field(seq_comp_hdr.cksum, cksum);
  modify_field(seq_comp_hdr.data_len, data_len);
  modify_field(seq_comp_hdr.version, version);
}

/*****************************************************************************
 *  seq_xts_db_intr_override
 *****************************************************************************/

action seq_xts_db_intr_override() {

  // Store the K+I vector into scratch to get the K+I generated correctly
  SEQ_KIVEC5XTS_USE(seq_kivec5xts_scratch, seq_kivec5xts)
  SEQ_KIVEC10_USE(seq_kivec10_scratch, seq_kivec10)
}

/*****************************************************************************
 *  seq_metrics0_commit : Update and commit metrics0 to qstate.
 *****************************************************************************/
@pragma little_endian interrupts_raised next_db_rung descs_processed descs_aborted status_pdma_xfers hw_desc_xfers hw_batch_errs hw_op_errs
action seq_metrics0_commit(interrupts_raised, next_db_rung, descs_processed,
                           descs_aborted, status_pdma_xfers, hw_desc_xfers,
			   hw_batch_errs, hw_op_errs) {

  // Store the K+I vector into scratch to get the K+I generated correctly
  SEQ_KIVEC9_USE(seq_kivec9_scratch, seq_kivec9)

  modify_field(seq_metrics0.interrupts_raised, interrupts_raised);
  modify_field(seq_metrics0.next_db_rung, next_db_rung);
  modify_field(seq_metrics0.descs_processed, descs_processed);
  modify_field(seq_metrics0.descs_aborted, descs_aborted);
  modify_field(seq_metrics0.status_pdma_xfers, status_pdma_xfers);
  modify_field(seq_metrics0.hw_desc_xfers, hw_desc_xfers);
  modify_field(seq_metrics0.hw_batch_errs, hw_batch_errs);
  modify_field(seq_metrics0.hw_op_errs, hw_op_errs);
}

/*****************************************************************************
 *  seq_metrics1_commit : Update and commit metrics1 to qstate.
 *****************************************************************************/
@pragma little_endian aol_update_reqs sgl_updatereqs sgl_pdma_xfers sgl_pdma_errs sgl_pad_only_xfers sgl_pad_only_errs alt_descs_taken alt_bufs_taken
action seq_metrics1_commit(aol_update_reqs, sgl_update_reqs, sgl_pdma_xfers,
                           sgl_pdma_errs, sgl_pad_only_xfers, sgl_pad_only_errs,
			   alt_descs_taken, alt_bufs_taken) {

  // Store the K+I vector into scratch to get the K+I generated correctly
  SEQ_KIVEC9_USE(seq_kivec9_scratch, seq_kivec9)

  modify_field(seq_metrics1.aol_update_reqs, aol_update_reqs);
  modify_field(seq_metrics1.sgl_update_reqs, sgl_update_reqs);
  modify_field(seq_metrics1.sgl_pdma_xfers, sgl_pdma_xfers);
  modify_field(seq_metrics1.sgl_pdma_errs, sgl_pdma_errs);
  modify_field(seq_metrics1.sgl_pad_only_xfers, sgl_pad_only_xfers);
  modify_field(seq_metrics1.sgl_pad_only_errs, sgl_pad_only_errs);
  modify_field(seq_metrics1.alt_descs_taken, alt_descs_taken);
  modify_field(seq_metrics1.alt_bufs_taken, alt_bufs_taken);
}

/*****************************************************************************
 *  seq_metrics2_commit : Update and commit metrics2 to qstate.
 *****************************************************************************/
@pragma little_endian len_updates cp_header_updates seq_hw_bytes
action seq_metrics2_commit(len_updates, cp_header_updates, seq_hw_bytes) {

  // Store the K+I vector into scratch to get the K+I generated correctly
  SEQ_KIVEC9_USE(seq_kivec9_scratch, seq_kivec9)

  modify_field(seq_metrics2.len_updates, len_updates);
  modify_field(seq_metrics2.cp_header_updates, cp_header_updates);
  modify_field(seq_metrics2.seq_hw_bytes, seq_hw_bytes);
}

