// Accelerator hash to decompression chaining DOLs.
#include <math.h>
#include "compression.hpp"
#include "compression_test.hpp"
#include "tests.hpp"
#include "utils.hpp"
#include "queues.hpp"

#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <stdio.h>
#include <byteswap.h>
#include "chksum_decomp_chain.hpp"

namespace tests {


/*
 * Constructor
 */
chksum_decomp_chain_t::chksum_decomp_chain_t(chksum_decomp_chain_params_t params) :
    comp_hash_chain(nullptr),
    app_max_size(params.app_max_size_),
    app_blk_size(0),
    app_hash_size(kCompAppHashBlkSize),
    num_hash_blks(0),
    caller_comp_buf(nullptr),
    caller_chksum_status_vec(nullptr),
    caller_chksum_opaque_vec(nullptr),
    caller_decomp_status_buf(nullptr),
    caller_decomp_opaque_buf(nullptr),
    caller_opaque_data(0),
    chksum_queue(nullptr),
    decomp_queue(nullptr),
    push_type(COMP_QUEUE_PUSH_SEQUENCER),
    destructor_free_buffers(params.destructor_free_buffers_),
    suppress_info_log(params.suppress_info_log_),
    success(false)
{
    uncomp_buf = new dp_mem_t(1, app_max_size,
                              DP_MEM_ALIGN_PAGE, params.uncomp_mem_type_);
    /*
     * Allocate enough chksum status for the worst case
     */
    max_hash_blks = COMP_HASH_CHAIN_MAX_HASH_BLKS(app_max_size, sizeof(cp_hdr_t),
                                                  app_hash_size);
    /*
     * Allocate enough chksum status for the worst case; used in the event
     * caller does not supply their own status buffers.
     */
    chksum_status_vec = new dp_mem_t(max_hash_blks, CP_STATUS_PAD_ALIGNED_SIZE,
                                     DP_MEM_ALIGN_SPEC, DP_MEM_TYPE_HOST_MEM,
                                     kMinHostMemAllocSize);
}


/*
 * Destructor
 */
chksum_decomp_chain_t::~chksum_decomp_chain_t()
{
    // Only free buffers on successful completion; otherwise,
    // HW/P4+ might still be trying to access them.
    
    printf("%s success %u destructor_free_buffers %u\n",
           __FUNCTION__, success, destructor_free_buffers);
    if (success && destructor_free_buffers) {
        delete uncomp_buf;
        delete chksum_status_vec;
        chksum_desc_vec.clear();
    }
}


/*
 * Execute any extra pre-push initialization
 */
void
chksum_decomp_chain_t::pre_push(chksum_decomp_chain_pre_push_params_t params)
{
    caller_chksum_status_vec = params.caller_chksum_status_vec_;
    caller_chksum_opaque_vec = params.caller_chksum_opaque_vec_;
    caller_decomp_status_buf = params.caller_decomp_status_buf_;
    caller_decomp_opaque_buf = params.caller_decomp_opaque_buf_;
    caller_opaque_data = params.caller_opaque_data_;
}

/*
 * Initiate the test
 */
int 
chksum_decomp_chain_t::push(chksum_decomp_chain_push_params_t params)
{
    comp_queue_push_t   curr_push_type;
    int                 actual_hash_blks;
    uint32_t            block_no;

    /*
     * Validate number of hash blocks required
     */
    comp_hash_chain = params.comp_hash_chain_;
    actual_hash_blks = comp_hash_chain->actual_hash_blks_get(
                                               COMP_HASH_CHAIN_NON_BLOCKING_RETRIEVE);
    if ((actual_hash_blks <= 0) || (actual_hash_blks > (int)max_hash_blks)) {
        printf("%s invalid actual_hash_blks %d in relation to max_hash_blks %u\n",
               __FUNCTION__, actual_hash_blks, max_hash_blks);
        assert((actual_hash_blks > 0) || (actual_hash_blks <= (int)max_hash_blks));
        return -1;
    }

    // validate app_blk_size
    app_blk_size = comp_hash_chain->app_blk_size_get();
    if (!app_blk_size || (app_blk_size > app_max_size)) {
        printf("%s app_blk_size %u too small or exceeds app_max_size %u\n",
               __FUNCTION__, app_blk_size, app_max_size);
        assert(app_blk_size && (app_blk_size <= app_max_size));
        return -1;
    }

    // validate app_hash_size
    app_hash_size = comp_hash_chain->app_hash_size_get();
    if (!app_hash_size || (app_hash_size > app_blk_size) ||
        ((app_hash_size < kCompAppMinSize))) {
        printf("%s invalid app_hash_size %u in relation to app_blk_size %u "
               "and kCompAppMinSize %u\n", __FUNCTION__, app_hash_size,
               app_blk_size, kCompAppMinSize);
        assert(app_hash_size && (app_hash_size <= app_blk_size) &&
               (app_hash_size >= kCompAppMinSize));
        return -1;
    }

    if (!suppress_info_log) {
        printf("Starting testcase chksum_decomp_chain app_blk_size %u app_hash_size %u "
               "actual_hash_blks %d push_type %d seq_chksum_qid %u seq_comp_qid %u\n",
               app_blk_size, app_hash_size, actual_hash_blks, params.push_type_,
               params.seq_chksum_qid_, params.seq_decomp_qid_);
    }

    /*
     * Ensure caller supplied enough (optional) opaque buffers
     */
    num_hash_blks = (uint32_t)actual_hash_blks;
    if (caller_chksum_opaque_vec && 
        (caller_chksum_opaque_vec->num_lines_get() < num_hash_blks)) {
        printf("%s number of chksum opaque buffers %u is less than num_hash_blks %u\n",
               __FUNCTION__, caller_chksum_opaque_vec->num_lines_get(),
               num_hash_blks);
        assert(caller_chksum_opaque_vec->num_lines_get() >= num_hash_blks);
        return -1;
    }

    /*
     * Ensure caller supplied enough (optional) status buffers;
     * Substitute with our own if caller didn't supply any.
     */
    if (!caller_chksum_status_vec) {
        caller_chksum_status_vec = chksum_status_vec;
    }
    if (caller_chksum_status_vec && 
        (caller_chksum_status_vec->num_lines_get() < num_hash_blks)) {
        printf("%s number of chksum status buffers %u is less than num_hash_blks %u\n",
               __FUNCTION__, caller_chksum_status_vec->num_lines_get(),
               num_hash_blks);
        assert(caller_chksum_status_vec->num_lines_get() >= num_hash_blks);
        return -1;
    }

    /*
     * Partially overwrite destination buffer to prevent left over
     * data from a previous run
     */
    uncomp_buf->fragment_find(0, 64)->fill_thru(0xff);

    chksum_queue = params.chksum_queue_;
    decomp_queue = params.decomp_queue_;
    caller_comp_buf = comp_hash_chain->comp_buf2_get();

    chksum_desc_vec.clear();
    success = false;

    /*
     * Note that chksum-decomp is not a true chain operation, i.e., both
     * operations can and will be initiated at the same time. The reasons
     * are as follows:
     *   1) HW does not do checksum verification, particularly when
     *      the checksum was created from integrity data. What HW does is,
     *      given a source, it re-generates new integrity data with which
     *      SW can use to verify against the old value. Besides, this
     *      re-generation potentially consists of multiple operations, one
     *      per hash block. Each of these can finish in any order and it is 
     *      not feasible for P4+ to determine when all the operations have been
     *      completed.
     *   2) The initiation of decompression does not have to depend on the
     *      result of checksum verification. Both can and should occur, and
     *      it is up to SW driver/app to work with the results however it wants. 
     */
    /*
     * With multi checksum blocks, use deferred push if possible.
     */
    curr_push_type = push_type == COMP_QUEUE_PUSH_SEQUENCER ?
                     COMP_QUEUE_PUSH_SEQUENCER_DEFER : push_type;
    for (block_no = 0; block_no < num_hash_blks; block_no++) {
        chksum_queue->push(chksum_setup(block_no), curr_push_type,
                           params.seq_chksum_qid_);
    }

    decomp_queue->push(decomp_setup(), push_type,  params.seq_decomp_qid_);
    return 0;
}


/*
 * Execute any deferred push()
 */
void
chksum_decomp_chain_t::post_push(void)
{
    chksum_queue->post_push();
    if (decomp_queue != chksum_queue) {
        decomp_queue->post_push();
    }
}


/*
 * Set up descriptor to calculate checksum, i.e., integrity data, for a 
 * given block. The result would be stored in caller_chksum_status_vec.
 */
cp_desc_t& 
chksum_decomp_chain_t::chksum_setup(uint32_t block_no)
{
    cp_desc_t   chksum_desc = {0};

    caller_chksum_status_vec->line_set(block_no);
    caller_chksum_status_vec->clear_thru();

    chksum_desc.cmd_bits.integrity_src = comp_hash_chain->integrity_src_get();
    chksum_desc.cmd_bits.integrity_type = comp_hash_chain->integrity_type_get();

    chksum_desc.src = caller_comp_buf->pa() + (block_no * app_hash_size);
    chksum_desc.datain_len = app_hash_size;
    chksum_desc.threshold_len = app_hash_size;
    chksum_desc.status_addr = caller_chksum_status_vec->pa();
    chksum_desc.status_data = 0xbeb0;

    /*
     * Checksum interrupts are optional, depending on what the application wants
     */
    if (caller_chksum_opaque_vec) {
        caller_chksum_opaque_vec->line_set(block_no);
        caller_chksum_opaque_vec->clear_thru();

        chksum_desc.opaque_tag_addr = caller_chksum_opaque_vec->pa();
        chksum_desc.opaque_tag_data = caller_opaque_data;
        chksum_desc.cmd_bits.opaque_tag_on = 1;
        if (!suppress_info_log) {
            printf("chksum_decomp_chain HW to potentially write checksum "
                   "opaque_tag_addr 0x%lx opaque_tag_data 0x%x\n",
                   chksum_desc.opaque_tag_addr, chksum_desc.opaque_tag_data);
        }
    }

    chksum_desc_vec.push_back(chksum_desc);
    return chksum_desc_vec.back();
}


/*
 * Set up decompression
 */
cp_desc_t&
chksum_decomp_chain_t::decomp_setup(void)
{
    memset(&dc_desc, 0, sizeof(dc_desc));
    caller_decomp_status_buf->clear_thru();
    decompress_cp_desc_template_fill(dc_desc, caller_comp_buf,
                                     uncomp_buf, caller_decomp_status_buf,
                                     comp_hash_chain->cp_output_data_len_get(),
                                     uncomp_buf->line_size_get());
    dc_desc.status_data = 0xb0be;

    /*
     * Decomp interrupts are optional, depending on what the application wants
     */
    if (caller_decomp_opaque_buf) {
        caller_decomp_opaque_buf->clear_thru();
        dc_desc.opaque_tag_addr = caller_decomp_opaque_buf->pa();
        dc_desc.opaque_tag_data = caller_opaque_data;
        dc_desc.cmd_bits.opaque_tag_on = 1;
        if (!suppress_info_log) {
            printf("chksum_decomp_chain HW to potentially write decomp "
                   "opaque_tag_addr 0x%lx opaque_tag_data 0x%x\n", 
                   dc_desc.opaque_tag_addr, dc_desc.opaque_tag_data);
        }
    }

    return dc_desc;
}


/*
 * Test result verification
 */
int 
chksum_decomp_chain_t::verify(void)
{
    dp_mem_t            *exp_hash_status_vec;
    cp_status_sha512_t  *exp_chksum_st;
    cp_status_sha512_t  *actual_chksum_st;
    uint32_t            block_no;

    /*
     * Verify all chksum status
     */
    exp_hash_status_vec = comp_hash_chain->hash_status_vec_get();
    for (block_no = 0; block_no < num_hash_blks; block_no++) {
        caller_chksum_status_vec->line_set(block_no);
        exp_hash_status_vec->line_set(block_no);

        if (!comp_status_poll(caller_chksum_status_vec, suppress_info_log)) {
            printf("ERROR: chksum_decomp_chain block %u checksum status never came\n",
                   block_no);
            return -1;
        }

        if (compress_status_verify(caller_chksum_status_vec, nullptr,
                                   chksum_desc_vec.at(block_no))) {
            printf("ERROR: chksum_decomp_chain checksum block %u status "
                   "verification failed\n", block_no);
            return -1;
        }

        // Verify the generated checksum
        exp_chksum_st = (cp_status_sha512_t *)exp_hash_status_vec->read();
        actual_chksum_st = (cp_status_sha512_t *)caller_chksum_status_vec->read();
        if (!exp_chksum_st->integrity_data ||
            (actual_chksum_st->integrity_data != exp_chksum_st->integrity_data)) {
            printf("ERROR: chksum_decomp_chain block %u exp_chksum is zero or "
                   "value 0x%lx mismatches with actual_chksum 0x%lx\n", block_no, 
                   exp_chksum_st->integrity_data, actual_chksum_st->integrity_data);
            return -1;
        }
    }

    /*
     * Verify decompression status
     */
    if (!comp_status_poll(caller_decomp_status_buf, suppress_info_log)) {
        printf("ERROR: chksum_decomp_chain decompression status never came\n");
        return -1;
    }

    if (decompress_status_verify(caller_decomp_status_buf, dc_desc,
                                 app_blk_size)) {
        printf("ERROR: chksum_decomp_chain decompression status "
               "verification failed\n");
        return -1;
    }

    /*
     * Validate decompressed data
     */
    if (test_data_verify_and_dump(comp_hash_chain->uncomp_buf_get()->read(),
                                  uncomp_buf->read_thru(),
                                  app_blk_size)) {
        return -1;
    }

    if (!suppress_info_log) {
        printf("Testcase chksum_decomp_chain passed\n");
    }
    success = true;
    return 0;
}

}  // namespace tests
