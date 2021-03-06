/* TLS P4 definitions */

#include "../../common-p4+/common_txdma_dummy.p4"

#define tx_table_s0_t0_action       read_tls_stg0

#define tx_table_s1_t0_action       read_serq_entry

#define tx_table_s2_t0_action       tls_serq_process
#define tx_table_s2_t1_action       read_tnmdr_pidx
#define tx_table_s2_t2_action       read_tnmpr_pidx
#define tx_table_s2_t3_action       tls_serq_consume

#define tx_table_s3_t0_action       tls_read_tls_header
#define tx_table_s3_t1_action       tdesc_alloc
#define tx_table_s3_t2_action       tpage_alloc

#define tx_table_s4_t0_action       tls_read_barco_pi

#define tx_table_s5_t0_action       tls_bld_brq5

#define tx_table_s6_t0_action       tls_queue_brq6
#define tx_table_s6_t1_action       tls_write_arq

#define tx_table_s7_t0_action       tls_pre_crypto_stats7

#include "../../common-p4+/common_txdma.p4"
#include "../../cpu-p4+/cpu_rx_common.p4"
#include "tls_txdma_common.p4"


// d for stage 3 table 1
header_type tdesc_alloc_d_t {
    fields {
        desc                    : 64;
        pad                     : 448;
    }
}

// d for stage 3 table 2
header_type tpage_alloc_d_t {
    fields {
        page                    : 64;
        pad                     : 448;
    }
}

header_type tls_stage_bld_barco_req_d_t {
    fields {
        key_desc_index                  : HBM_ADDRESS_WIDTH;
        command_core                    : 4;
        command_mode                    : 4;
        command_op                      : 4;
        command_param                   : 20;
        // TBD: Total used   : 64 bits, pending: 448
        pad                             : 448;
    }
}
#define STG_BLD_BARCO_REQ_ACTION_PARAMS                                                                 \
key_desc_index, command_core, command_mode, command_op, command_param,idesc, odesc
#

#define GENERATE_STG_BLD_BARCO_REQ_D                                                                    \
    modify_field(tls_bld_barco_req_d.key_desc_index, key_desc_index);                                   \
    modify_field(tls_bld_barco_req_d.command_core, command_core);                                       \
    modify_field(tls_bld_barco_req_d.command_mode, command_mode);                                       \
    modify_field(tls_bld_barco_req_d.command_op, command_op);                                           \
    modify_field(tls_bld_barco_req_d.command_param, command_param);

header_type tls_stage_queue_brq_d_t {
    fields {
        // 8 Bytes intrinsic header
        CAPRI_QSTATE_HEADER_COMMON
        // 4 Bytes BRQ ring
        CAPRI_QSTATE_HEADER_RING(0)

        brq_base                       : ADDRESS_WIDTH;

        // TBD: Total used   : 128 bits, pending: 384
        pad                             : 384;
    }
}

#define STG_QUEUE_BRQ_ACTION_PARAMS                                                                 \
rsvd,cosA,cosB,cos_sel,eval_last,host,total,pid, pi_0,ci_0
#

#define GENERATE_STG_QUEUE_BRQ_D                                                                    \
    modify_field(tls_queue_brq_d.rsvd, rsvd);                                                       \
    modify_field(tls_queue_brq_d.cosA, cosA);                                                       \
    modify_field(tls_queue_brq_d.cosB, cosB);                                                       \
    modify_field(tls_queue_brq_d.cos_sel, cos_sel);                                                 \
    modify_field(tls_queue_brq_d.eval_last, eval_last);                                             \
    modify_field(tls_queue_brq_d.host, host);                                                       \
    modify_field(tls_queue_brq_d.total, total);                                                     \
    modify_field(tls_queue_brq_d.pid, pid);                                                         \
    modify_field(tls_queue_brq_d.pi_0, pi_0);                                                       \
    modify_field(tls_queue_brq_d.ci_0, ci_0);



#define GENERATE_GLOBAL_K                                                                               \
        modify_field(tls_global_phv_scratch.fid, tls_global_phv.fid);                                   \
        modify_field(tls_global_phv_scratch.dec_flow, tls_global_phv.dec_flow);                         \
        modify_field(tls_global_phv_scratch.write_arq, tls_global_phv.write_arq);                       \
        modify_field(tls_global_phv_scratch.do_pre_ccm_dec, tls_global_phv.do_pre_ccm_dec);             \
        modify_field(tls_global_phv_scratch.tls_global_pad0, tls_global_phv.tls_global_pad0);           \
        modify_field(tls_global_phv_scratch.qstate_addr, tls_global_phv.qstate_addr);                   \
        modify_field(tls_global_phv_scratch.tls_hdr_type, tls_global_phv.tls_hdr_type);                 \
        modify_field(tls_global_phv_scratch.tls_hdr_version_major, tls_global_phv.tls_hdr_version_major);\
        modify_field(tls_global_phv_scratch.tls_hdr_version_minor, tls_global_phv.tls_hdr_version_minor);\
        modify_field(tls_global_phv_scratch.tls_hdr_len, tls_global_phv.tls_hdr_len);                   \
        modify_field(tls_global_phv_scratch.next_tls_hdr_offset, tls_global_phv.next_tls_hdr_offset);   \
        modify_field(tls_global_phv_scratch.debug_dol, tls_global_phv.debug_dol);

/* Global PHV definition */
header_type tls_global_phv_t {
    fields {
        fid                             : 16;
        dec_flow                        : 8;
        write_arq                       : 1;
        do_pre_ccm_dec                  : 1;
        tls_global_pad0                 : 6;
        qstate_addr                     : HBM_ADDRESS_WIDTH;
        tls_hdr_type                    : 8;
        tls_hdr_version_major           : 8;
        tls_hdr_version_minor           : 8;
        tls_hdr_len                     : 16;
        next_tls_hdr_offset             : 16;
        debug_dol                       : 8;
    }
}

header_type to_stage_1_phv_t {
    fields {
        serq_ci                         : 16;
        serq_prod_ci_addr               : HBM_ADDRESS_WIDTH;
    }
}

header_type to_stage_2_phv_t {
    fields {
        idesc                           : HBM_ADDRESS_WIDTH;
    }
}

header_type to_stage_5_phv_t {
    fields {
        idesc                           : HBM_ADDRESS_WIDTH;
        odesc                           : HBM_ADDRESS_WIDTH;
        opage                           : HBM_ADDRESS_WIDTH;
        cur_tls_data_len                : 16;
        write_arq                       : 1;
        pad                             : 7;
    }
}

header_type to_stage_6_phv_t {
    fields {
        idesc                           : HBM_ADDRESS_WIDTH;
        odesc                           : HBM_ADDRESS_WIDTH;
        opage                           : HBM_ADDRESS_WIDTH;
        cur_tls_data_len                : 16;
        next_tls_hdr_offset             : 16;
    }
}
 header_type to_stage_7_phv_t {
     fields {
        tnmdpr_alloc                    : 16;
        enc_requests                    : 8;
        dec_requests                    : 8;
        debug_stage0_3_thread           : 16;
        debug_stage4_7_thread           : 16;
     }
 }

#define GENERATE_TO_S6(TO, FROM)                                                        \
        modify_field(TO.idesc, FROM.idesc);                                             \
        modify_field(TO.odesc, FROM.odesc);                                             \
        modify_field(TO.opage, FROM.opage);                                             \
        modify_field(TO.cur_tls_data_len, FROM.cur_tls_data_len);                       \
        modify_field(TO.next_tls_hdr_offset, FROM.next_tls_hdr_offset);


#define GENERATE_S1_S2_T0                                                                   \
        modify_field(s1_s2_t0_scratch.idesc_aol0_addr, s1_s2_t0_phv.idesc_aol0_addr);       \
        modify_field(s1_s2_t0_scratch.idesc_aol0_offset, s1_s2_t0_phv.idesc_aol0_offset);   \
        modify_field(s1_s2_t0_scratch.idesc_aol0_len, s1_s2_t0_phv.idesc_aol0_len);

header_type s1_s2_t0_phv_t {
    fields {
        idesc_aol0_addr                 : ADDRESS_WIDTH;
        idesc_aol0_offset               : 32;
        pad1                            : 24;
        idesc_aol0_len                  : 32;
    }
}
#define GENERATE_S2_S3_T0                                                               \
        modify_field(s2_s3_t0_scratch.idesc_aol0_addr, s2_s3_t0_phv.idesc_aol0_addr);   \
        modify_field(s2_s3_t0_scratch.idesc_aol0_offset, s2_s3_t0_phv.idesc_aol0_offset);   \
        modify_field(s2_s3_t0_scratch.idesc_aol0_len, s2_s3_t0_phv.idesc_aol0_len);

header_type s2_s3_t0_phv_t {
    fields {
        idesc_aol0_addr                 : ADDRESS_WIDTH;
        idesc_aol0_offset               : 32;
        pad1                            : 24;
        idesc_aol0_len                  : 32;
    }
}

//The aad fields below should be same as additional_data_t
header_type s4_s6_t0_phv_t {
    fields {
        aad_seq_num                 : 64;
        aad_type                    : 8;
        aad_version_major           : 8;
        aad_version_minor           : 8;
        aad_length                  : 16;
        sw_barco_pi                 : 16;
    }
}

#define GENERATE_S4_S6_T0  \
        GENERATE_AAD_FIELDS(s4_s6_t0_scratch, s4_s6_t0_phv)    \
        modify_field(s4_s6_t0_scratch.sw_barco_pi, s4_s6_t0_phv.sw_barco_pi);


#define GENERATE_S5_S6_T1                                                               \
        modify_field(s5_s6_t1_s2s_scratch.arq_base, s5_s6_t1_s2s.arq_base);             \
        modify_field(s5_s6_t1_s2s_scratch.arq_opage, s5_s6_t1_s2s.arq_opage);

header_type s5_s6_t1_s2s_phv_t {
    fields {
        pad                             : 16;
        arq_base                        : 32;
        arq_opage                       : 64;

    }
}


/* PHV PI storage */
header_type barco_dbell_t {
    fields {
        pi                                  : 32;
    } 
}

header_type odesc_dma_src_t {
    fields {
        odesc                               : 64;
    }
}


header_type s3_t1_s2s_phv_t {
    fields {
        tnmdr_pidx              : 16;
    }
}

header_type s3_t2_s2s_phv_t {
    fields {
        tnmpr_pidx              : 16;
    }
}

header_type pad_to_dma_cmds_t {
    fields {
        pad                     : 28;
    }
}

header_type ring_entry_pad_t {
    fields {
        pad                     : 192;
    }
}

header_type after_odesc_pad_t {
    fields {
        pad                     : 32;
    }
}

@pragma scratch_metadata
metadata tlscb_0_t tlscb_0_d;

@pragma scratch_metadata
metadata tlscb_1_t tlscb_1_d;

@pragma scratch_metadata
metadata tls_stage_bld_barco_req_d_t tls_bld_barco_req_d;

@pragma scratch_metadata
metadata tls_stage_queue_brq_d_t tls_queue_brq_d;

@pragma scratch_metadata
metadata tls_stage_pre_crypto_stats_d_t tls_pre_crypto_stats_d;

@pragma scratch_metadata
metadata serq_entry_new_t SERQ_ENTRY_NEW_SCRATCH;

@pragma scratch_metadata
metadata tnmdr_pidx_t TNMDR_PIDX_SCRATCH;

@pragma scratch_metadata
metadata tnmpr_pidx_t TNMPR_PIDX_SCRATCH;

@pragma scratch_metadata
metadata pkt_descr_aol_t PKT_DESCR_AOL_SCRATCH;

@pragma scratch_metadata
metadata tls_header_t TLS_HDR_SCRATCH;

@pragma scratch_metadata
metadata barco_channel_pi_ci_t tls_enc_queue_brq_d;

@pragma pa_header_union ingress to_stage_1 to_s1 cpu_hdr1
metadata to_stage_1_phv_t to_s1;
@pragma dont_trim
metadata p4_to_p4plus_cpu_pkt_1_t cpu_hdr1;

@pragma dont_trim
@pragma pa_header_union ingress to_stage_2 to_s2 cpu_hdr2
metadata to_stage_2_phv_t to_s2;
@pragma dont_trim
metadata p4_to_p4plus_cpu_pkt_2_t cpu_hdr2;

@pragma pa_header_union ingress to_stage_3 odesc_dma_src
@pragma dont_trim
metadata odesc_dma_src_t odesc_dma_src;

@pragma pa_header_union ingress to_stage_4 crypto_iv
@pragma dont_trim
metadata crypto_iv_t crypto_iv;

@pragma pa_header_union ingress to_stage_5
metadata to_stage_5_phv_t to_s5;

@pragma pa_header_union ingress to_stage_6
metadata to_stage_6_phv_t to_s6;

@pragma pa_header_union ingress to_stage_7
metadata to_stage_7_phv_t to_s7;

@pragma pa_header_union ingress common_global
metadata tls_global_phv_t tls_global_phv;

@pragma pa_header_union ingress  common_t0_s2s s2_s3_t0_phv s4_s6_t0_phv
metadata s1_s2_t0_phv_t s1_s2_t0_phv;
metadata s2_s3_t0_phv_t s2_s3_t0_phv;
metadata s4_s6_t0_phv_t s4_s6_t0_phv;



@pragma pa_header_union ingress common_t1_s2s s5_s6_t1_s2s
metadata s3_t1_s2s_phv_t s3_t1_s2s;
metadata s5_s6_t1_s2s_phv_t s5_s6_t1_s2s;

@pragma pa_header_union ingress common_t2_s2s
metadata s3_t2_s2s_phv_t s3_t2_s2s;


@pragma dont_trim
metadata pkt_descr_aol_t idesc; 
@pragma dont_trim
metadata pkt_descr_aol_t odesc;
@pragma dont_trim
metadata barco_dbell_t barco_dbell;
@pragma dont_trim
metadata after_odesc_pad_t odesc_pad;
@pragma dont_trim
metadata barco_desc_t barco_desc;
@pragma dont_trim
metadata ring_entry_t ring_entry;
@pragma dont_trim
metadata ccm_header_t ccm_header_with_aad;
@pragma dont_trim
metadata bsq_slot_t bsq_slot;
@pragma dont_trim
metadata pad_to_dma_cmds_t pad_to_dma_cmds;

@pragma dont_trim
metadata dma_cmd_phv2mem_t dma_cmd_aad;
@pragma dont_trim
metadata dma_cmd_phv2mem_t dma_cmd_bsq_slot;
@pragma dont_trim
metadata dma_cmd_phv2mem_t dma_cmd_odesc;
@pragma dont_trim
metadata dma_cmd_phv2mem_t dma_cmd_iv;
@pragma dont_trim
metadata dma_cmd_phv2mem_t dma_cmd_brq_slot;
@pragma dont_trim
metadata dma_cmd_phv2mem_t dma_cmd_idesc;
@pragma dont_trim
metadata dma_cmd_phv2mem_t dma_cmd_idesc_meta;
@pragma dont_trim
metadata dma_cmd_phv2mem_t dma_cmd_dbell;

@pragma scratch_metadata
metadata tls_global_phv_t tls_global_phv_scratch;
@pragma scratch_metadata
metadata to_stage_1_phv_t to_s1_scratch;
@pragma scratch_metadata
metadata to_stage_2_phv_t to_s2_scratch;
@pragma scratch_metadata
metadata barco_dbell_t barco_db_scratch;
@pragma scratch_metadata
metadata to_stage_5_phv_t to_s5_scratch;
@pragma scratch_metadata
metadata to_stage_6_phv_t to_s6_scratch;
@pragma scratch_metadata
metadata to_stage_7_phv_t to_s7_scratch;

@pragma scratch_metadata
metadata s1_s2_t0_phv_t s1_s2_t0_scratch;
@pragma scratch_metadata
metadata s2_s3_t0_phv_t s2_s3_t0_scratch;
@pragma scratch_metadata
metadata s4_s6_t0_phv_t s4_s6_t0_scratch;

@pragma scratch_metadata
metadata s3_t1_s2s_phv_t s3_t1_s2s_scratch;
@pragma scratch_metadata
metadata s5_s6_t1_s2s_phv_t s5_s6_t1_s2s_scratch;
@pragma scratch_metadata
metadata s3_t2_s2s_phv_t s3_t2_s2s_scratch;


@pragma scratch_metadata
metadata tdesc_alloc_d_t tdesc_alloc_d;
@pragma scratch_metadata
metadata tpage_alloc_d_t tpage_alloc_d;
@pragma scratch_metadata
metadata arq_pi_d_t arq_tx_pi_d;

@pragma scratch_metadata
metadata barco_shadow_params_d_t BARCO_SHADOW_SCRATCH;

@pragma scratch_metadata
metadata tlscb_config_aead_t TLSCB_CONFIG_AEAD_SCRATCH;

/* Stage 1 Table 0 action */
action read_serq_entry(SERQ_ENTRY_NEW_ACTION_PARAMS) {

    GENERATE_GLOBAL_K

    /* To Stage 1 fields */
    modify_field(to_s1_scratch.serq_ci, to_s1.serq_ci);
    modify_field(to_s1_scratch.serq_prod_ci_addr, to_s1.serq_prod_ci_addr);

    GENERATE_SERQ_ENTRY_NEW_D
}


/* Stage 2 table 0 action -- not needed anyore, absorbed in read_serq_entry
action tls_read_pkt_descr_aol(PKT_DESCR_AOL_ACTION_PARAMS) {

    GENERATE_GLOBAL_K

    GENERATE_PKT_DESCR_AOL_D
}
*/

/* Stage 2 table 0 action */
action tls_serq_process(TLSCB_CONFIG_AEAD_PARAMS) {


    GENERATE_GLOBAL_K

    GENERATE_S1_S2_T0

    /* To Stage 3 fields */
    modify_field(to_s2_scratch.idesc, to_s2.idesc);

    /* Unionized PHV mem with to_s3 used in later stage as scratch */
    /*modify_field(barco_db_scratch.pi, barco_dbell.pi); */

    /* D vector */
    GENERATE_TLSCB_CONFIG_AEAD

}

/*
 * Stage 2 table 1 action
 */
action read_tnmdr_pidx(TNMDR_PIDX_ACTION_PARAMS) {
    GENERATE_GLOBAL_K

    GENERATE_TNMDR_PIDX_D
}

/*
 * Stage 2 table 2 action
 */
action read_tnmpr_pidx(TNMPR_PIDX_ACTION_PARAMS) {
    GENERATE_GLOBAL_K

    GENERATE_TNMPR_PIDX_D
}

/* Stage 2 table 3 action */
action tls_serq_consume(TLSCB_0_PARAMS_NON_STG0) {


    GENERATE_GLOBAL_K

    /* D vector */
    GENERATE_TLSCB_0_D_NON_STG0

}

/* Stage 3 Table 0 action */
action tls_read_tls_header(TLS_HDR_ACTION_PARAMS) {
    GENERATE_GLOBAL_K

    GENERATE_S2_S3_T0

    GENERATE_TLS_HDR_D
}

/*
 * Stage 3 table 1 action
 */
action tdesc_alloc(desc, pad) {
    // k + i for stage 3 table 1

    // from to_stage 3

    // from ki global
    GENERATE_GLOBAL_K

    // from stage 2 to stage 3
    modify_field(s3_t1_s2s_scratch.tnmdr_pidx, s3_t1_s2s.tnmdr_pidx);

    // d for stage 3 table 1
    modify_field(tdesc_alloc_d.desc, desc);
    modify_field(tdesc_alloc_d.pad, pad);
}

/*
 * Stage 3 table 2 action
 */
action tpage_alloc(page, pad) {
    // k + i for stage 3 table 2

    // from to_stage 3

    // from ki global
    GENERATE_GLOBAL_K

    // from stage 2 to stage 3
    modify_field(s3_t2_s2s_scratch.tnmpr_pidx, s3_t2_s2s.tnmpr_pidx);

    // d for stage 3 table 2
    modify_field(tpage_alloc_d.page, page);
    modify_field(tpage_alloc_d.pad, pad);
}


/* Stage 3 table 3 action */
action tls_stage3(TLSCB_1_PARAMS) {

    GENERATE_GLOBAL_K

    GENERATE_TLSCB_1_D
}


/* Stage 4 Table 0 action */
action tls_read_barco_pi(BARCO_SHADOW_PARAMS) {

    GENERATE_GLOBAL_K

    /* D vector */
    GENERATE_BARCO_SHADOW_PARAMS_D
}

/* Stage 5 action */
action tls_bld_brq5(TLSCB_CONFIG_AEAD_PARAMS) {

    GENERATE_GLOBAL_K

    /* To Stage 5 fields */
    modify_field(to_s5_scratch.idesc, to_s5.idesc);
    modify_field(to_s5_scratch.odesc, to_s5.odesc);

    GENERATE_TLSCB_CONFIG_AEAD
}


/* Stage 6 action */
action tls_queue_brq6(BARCO_CHANNEL_PARAMS) {

    GENERATE_GLOBAL_K

    /* Stage 4 to 6 */
    GENERATE_S4_S6_T0

    /* To Stage 6 fields */
    GENERATE_TO_S6(to_s6_scratch, to_s6)


    GENERATE_BARCO_CHANNEL_D
}

/*
 * Stage 6 table 1 action
 */
action tls_write_arq(ARQ_PI_PARAMS) {

    // k + i for stage 6
    GENERATE_GLOBAL_K

    GENERATE_S5_S6_T1

    // from to_stage 6
    modify_field(to_s6_scratch.idesc, to_s6.idesc);
    modify_field(to_s6_scratch.odesc, to_s6.odesc);
    modify_field(to_s6_scratch.opage, to_s6.opage);
    modify_field(to_s6_scratch.cur_tls_data_len, to_s6.cur_tls_data_len);
    modify_field(to_s6_scratch.next_tls_hdr_offset, to_s6.next_tls_hdr_offset);


    // from stage to stage

    // d for stage 6 table 1
    GENERATE_ARQ_PI_D(arq_tx_pi_d)
}


/* Stage 7 action */
action tls_pre_crypto_stats7(STG_PRE_CRYPTO_STATS_ACTION_PARAMS) {

    GENERATE_GLOBAL_K


    /* To Stage 7 fields */
    modify_field(to_s7_scratch.tnmdpr_alloc, to_s7.tnmdpr_alloc);
    modify_field(to_s7_scratch.enc_requests, to_s7.enc_requests);
    modify_field(to_s7_scratch.dec_requests, to_s7.dec_requests);
    modify_field(to_s7_scratch.debug_stage0_3_thread, to_s7.debug_stage0_3_thread);
    modify_field(to_s7_scratch.debug_stage4_7_thread, to_s7.debug_stage4_7_thread);




    GENERATE_STG_PRE_CRYPTO_STATS_D
}
