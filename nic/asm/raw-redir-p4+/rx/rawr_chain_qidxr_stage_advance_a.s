#include "rawr-defines.h"

struct phv_     p;

%%

    .param      rawr_s4_chain_qidxr_stage_advance
    .align

/*
 * Table 3 advance to next stage which is stage 4 to eventually arrive
 * at a pre-agreed upon stage for handling chain pindex atomic update.
 */    
rawr_s3_chain_qidxr_stage_advance:

    CAPRI_CLEAR_TABLE0_VALID

    CAPRI_NEXT_TABLE_READ_NO_TABLE_LKUP(0, rawr_s4_chain_qidxr_stage_advance)
    nop.e
    nop
