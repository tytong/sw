// Compression DOLs.
#include <math.h>
#include "compression.hpp"
#include "compression_test.hpp"
#include "tests.hpp"
#include "utils.hpp"
#include "queues.hpp"
#include "xts.hpp"
#include "decrypt_decomp_chain.hpp"
#include "comp_encrypt_chain.hpp"
#include "comp_hash_chain.hpp"
#include "chksum_decomp_chain.hpp"
#include "storage_seq_p4pd.hpp"
#ifdef ELBA
#include "third-party/asic/elba/model/elb_top/elb_top_csr_defines.h"
#include "third-party/asic/elba/model/elb_top/csr_defines/elb_hens_c_hdr.h"
#else
#include "third-party/asic/capri/model/cap_top/cap_top_csr_defines.h"
#include "third-party/asic/capri/model/cap_he/readonly/cap_hens_csr_define.h"
#endif

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
#include "gflags/gflags.h"

namespace tests {

static uint64_t cp_cfg_glob;
static uint64_t cp_cfg_dist;
static uint64_t cp_cfg_ueng;
static uint64_t cp_cfg_q_base;
static uint64_t cp_cfg_hotq_base;
static uint64_t cp_cfg_q_pd_idx;
static uint64_t cp_cfg_hotq_pd_idx;
static uint64_t cp_cfg_host;
static uint64_t cp_cfg_host_opaque_tag_addr;
static uint64_t dc_cfg_glob;
static uint64_t dc_cfg_dist;
static uint64_t dc_cfg_ueng;
static uint64_t dc_cfg_q_base;
static uint64_t dc_cfg_hotq_base;
static uint64_t dc_cfg_q_pd_idx;
static uint64_t dc_cfg_hotq_pd_idx;
static uint64_t dc_cfg_host;
static uint64_t dc_cfg_host_opaque_tag_addr;

#ifdef ELBA
static uint64_t tmp_db_addr;
#endif

#ifdef ELBA
static void init_cpdc_registers(void) {
    cp_cfg_glob = ELB_ADDR_BASE_MD_HENS_OFFSET +
        ELB_HENS_CSR_DHS_CRYPTO_CTL_CP_CFG_GLB_BYTE_ADDRESS;

    cp_cfg_dist = ELB_ADDR_BASE_MD_HENS_OFFSET +
        ELB_HENS_CSR_DHS_CRYPTO_CTL_CP_CFG_DIST_BYTE_ADDRESS;

    cp_cfg_ueng = ELB_ADDR_BASE_MD_HENS_OFFSET +
        ELB_HENS_CSR_DHS_CRYPTO_CTL_CP_CFG_UENG_W0_BYTE_ADDRESS;

    cp_cfg_q_base = ELB_ADDR_BASE_MD_HENS_OFFSET +
        ELB_HENS_CSR_DHS_CRYPTO_CTL_CP_CFG_Q1_BASE_ADR_W0_BYTE_ADDRESS;

    cp_cfg_hotq_base = ELB_ADDR_BASE_MD_HENS_OFFSET +
        ELB_HENS_CSR_DHS_CRYPTO_CTL_CP_CFG_Q0_BASE_ADR_W0_BYTE_ADDRESS;

    cp_cfg_q_pd_idx = ELB_ADDR_BASE_MD_HENS_OFFSET +
        ELB_HENS_CSR_DHS_CRYPTO_CTL_CP_CFG_Q1_PD_IDX_BYTE_ADDRESS;

    cp_cfg_hotq_pd_idx = ELB_ADDR_BASE_MD_HENS_OFFSET +
        ELB_HENS_CSR_DHS_CRYPTO_CTL_CP_CFG_Q0_PD_IDX_BYTE_ADDRESS;

    cp_cfg_host = ELB_ADDR_BASE_MD_HENS_OFFSET +
        ELB_HENS_CSR_DHS_CRYPTO_CTL_CP_CFG_HOST_BYTE_ADDRESS;

    cp_cfg_host_opaque_tag_addr = ELB_ADDR_BASE_MD_HENS_OFFSET +
        ELB_HENS_CSR_DHS_CRYPTO_CTL_CP_CFG_HOST_OPAQUE_TAG_ADR_W0_BYTE_ADDRESS;

    dc_cfg_glob = ELB_ADDR_BASE_MD_HENS_OFFSET +
        ELB_HENS_CSR_DHS_CRYPTO_CTL_DC_CFG_GLB_BYTE_ADDRESS;

    dc_cfg_dist = ELB_ADDR_BASE_MD_HENS_OFFSET +
        ELB_HENS_CSR_DHS_CRYPTO_CTL_DC_CFG_DIST_BYTE_ADDRESS;

    dc_cfg_ueng = ELB_ADDR_BASE_MD_HENS_OFFSET +
        ELB_HENS_CSR_DHS_CRYPTO_CTL_DC_CFG_UENG_W0_BYTE_ADDRESS;

    dc_cfg_q_base = ELB_ADDR_BASE_MD_HENS_OFFSET +
        ELB_HENS_CSR_DHS_CRYPTO_CTL_DC_CFG_Q1_BASE_ADR_W0_BYTE_ADDRESS;

    dc_cfg_hotq_base = ELB_ADDR_BASE_MD_HENS_OFFSET +
        ELB_HENS_CSR_DHS_CRYPTO_CTL_DC_CFG_Q0_BASE_ADR_W0_BYTE_ADDRESS;

    dc_cfg_q_pd_idx = ELB_ADDR_BASE_MD_HENS_OFFSET +
        ELB_HENS_CSR_DHS_CRYPTO_CTL_DC_CFG_Q1_PD_IDX_BYTE_ADDRESS;

    dc_cfg_hotq_pd_idx = ELB_ADDR_BASE_MD_HENS_OFFSET +
        ELB_HENS_CSR_DHS_CRYPTO_CTL_DC_CFG_Q0_PD_IDX_BYTE_ADDRESS;

    dc_cfg_host = ELB_ADDR_BASE_MD_HENS_OFFSET +
        ELB_HENS_CSR_DHS_CRYPTO_CTL_DC_CFG_HOST_BYTE_ADDRESS;

    dc_cfg_host_opaque_tag_addr = ELB_ADDR_BASE_MD_HENS_OFFSET +
        ELB_HENS_CSR_DHS_CRYPTO_CTL_DC_CFG_HOST_OPAQUE_TAG_ADR_W0_BYTE_ADDRESS;
}
#else
static void init_cpdc_registers(void) {
    cp_cfg_glob = CAP_ADDR_BASE_MD_HENS_OFFSET +
        CAP_HENS_CSR_DHS_CRYPTO_CTL_CP_CFG_GLB_BYTE_ADDRESS;

    cp_cfg_dist = CAP_ADDR_BASE_MD_HENS_OFFSET +
        CAP_HENS_CSR_DHS_CRYPTO_CTL_CP_CFG_DIST_BYTE_ADDRESS;

    cp_cfg_ueng = CAP_ADDR_BASE_MD_HENS_OFFSET +
        CAP_HENS_CSR_DHS_CRYPTO_CTL_CP_CFG_UENG_W0_BYTE_ADDRESS;

    cp_cfg_q_base = CAP_ADDR_BASE_MD_HENS_OFFSET +
        CAP_HENS_CSR_DHS_CRYPTO_CTL_CP_CFG_Q_BASE_ADR_W0_BYTE_ADDRESS;

    cp_cfg_hotq_base = CAP_ADDR_BASE_MD_HENS_OFFSET +
        CAP_HENS_CSR_DHS_CRYPTO_CTL_CP_CFG_HOTQ_BASE_ADR_W0_BYTE_ADDRESS;

    cp_cfg_q_pd_idx = CAP_ADDR_BASE_MD_HENS_OFFSET +
        CAP_HENS_CSR_DHS_CRYPTO_CTL_CP_CFG_Q_PD_IDX_BYTE_ADDRESS;

    cp_cfg_hotq_pd_idx = CAP_ADDR_BASE_MD_HENS_OFFSET +
        CAP_HENS_CSR_DHS_CRYPTO_CTL_CP_CFG_HOTQ_PD_IDX_BYTE_ADDRESS;

    cp_cfg_host = CAP_ADDR_BASE_MD_HENS_OFFSET +
        CAP_HENS_CSR_DHS_CRYPTO_CTL_CP_CFG_HOST_BYTE_ADDRESS;

    cp_cfg_host_opaque_tag_addr = CAP_ADDR_BASE_MD_HENS_OFFSET +
        CAP_HENS_CSR_DHS_CRYPTO_CTL_CP_CFG_HOST_OPAQUE_TAG_ADR_W0_BYTE_ADDRESS;

    dc_cfg_glob = CAP_ADDR_BASE_MD_HENS_OFFSET +
        CAP_HENS_CSR_DHS_CRYPTO_CTL_DC_CFG_GLB_BYTE_ADDRESS;

    dc_cfg_dist = CAP_ADDR_BASE_MD_HENS_OFFSET +
        CAP_HENS_CSR_DHS_CRYPTO_CTL_DC_CFG_DIST_BYTE_ADDRESS;

    dc_cfg_ueng = CAP_ADDR_BASE_MD_HENS_OFFSET +
        CAP_HENS_CSR_DHS_CRYPTO_CTL_DC_CFG_UENG_W0_BYTE_ADDRESS;

    dc_cfg_q_base = CAP_ADDR_BASE_MD_HENS_OFFSET +
        CAP_HENS_CSR_DHS_CRYPTO_CTL_DC_CFG_Q_BASE_ADR_W0_BYTE_ADDRESS;

    dc_cfg_hotq_base = CAP_ADDR_BASE_MD_HENS_OFFSET +
        CAP_HENS_CSR_DHS_CRYPTO_CTL_DC_CFG_HOTQ_BASE_ADR_W0_BYTE_ADDRESS;

    dc_cfg_q_pd_idx = CAP_ADDR_BASE_MD_HENS_OFFSET +
        CAP_HENS_CSR_DHS_CRYPTO_CTL_DC_CFG_Q_PD_IDX_BYTE_ADDRESS;

    dc_cfg_hotq_pd_idx = CAP_ADDR_BASE_MD_HENS_OFFSET +
        CAP_HENS_CSR_DHS_CRYPTO_CTL_DC_CFG_HOTQ_PD_IDX_BYTE_ADDRESS;

    dc_cfg_host = CAP_ADDR_BASE_MD_HENS_OFFSET +
        CAP_HENS_CSR_DHS_CRYPTO_CTL_DC_CFG_HOST_BYTE_ADDRESS;

    dc_cfg_host_opaque_tag_addr = CAP_ADDR_BASE_MD_HENS_OFFSET +
        CAP_HENS_CSR_DHS_CRYPTO_CTL_DC_CFG_HOST_OPAQUE_TAG_ADR_W0_BYTE_ADDRESS;
}
#endif

#define dump_csr_32(r)                          \
    do {                                        \
        uint32_t lo_dummy ;                     \
        lo_dummy = READ_REG32(r);               \
        printf("=== %d: %s() %s addr: 0x%lx lo: 0x%x\n",    \
                __LINE__, __func__, #r, r, lo_dummy);\
    } while (0)

#define dump_csr_64(r)                          \
    do {                                        \
        uint32_t lo_dummy, hi_dummy;            \
        lo_dummy = READ_REG32(r);               \
        hi_dummy = READ_REG32(r+4);             \
        printf("=== %d: %s() %s addr: 0x%lx hi: 0x%x lo: 0x%x\n",           \
                __LINE__, __func__, #r, r, hi_dummy, lo_dummy);\
    } while (0)

static void dump_cpdc_registers(void) {
    dump_csr_32(cp_cfg_glob);
    dump_csr_32(cp_cfg_dist);
    dump_csr_64(cp_cfg_ueng);
    dump_csr_64(cp_cfg_q_base);
    dump_csr_64(cp_cfg_hotq_base);
    dump_csr_32(cp_cfg_q_pd_idx);
    dump_csr_32(cp_cfg_hotq_pd_idx);
    dump_csr_32(cp_cfg_host);
    dump_csr_64(cp_cfg_host_opaque_tag_addr);

    dump_csr_32(dc_cfg_glob);
    dump_csr_32(dc_cfg_dist);
    dump_csr_64(dc_cfg_ueng);
    dump_csr_64(dc_cfg_q_base);
    dump_csr_64(dc_cfg_hotq_base);
    dump_csr_32(dc_cfg_q_pd_idx);
    dump_csr_32(dc_cfg_hotq_pd_idx);
    dump_csr_32(dc_cfg_host);
    dump_csr_64(dc_cfg_host_opaque_tag_addr);
}

// compression/decompression blocks initialized by HAL
// or by this DOL module.
static const bool comp_inited_by_hal = true;
const static uint32_t kMaxSubqEntries = 4096;

acc_ring_t *cp_ring;
acc_ring_t *dc_ring;

acc_ring_t *cp_hot_ring;
acc_ring_t *dc_hot_ring;

// These constants equate to the number of
// hardware compression/decompression engines.
#define MAX_CP_REQ  16
#define MAX_DC_REQ  2

// Sample data generated during test.
static const uint8_t all_zeros[kCompEngineMaxSize] = {0};
static constexpr uint32_t kUncompressedDataSize = kCompEngineMaxSize;
static uint8_t uncompressed_data[kUncompressedDataSize];
static constexpr uint32_t kCompressedBufSize = kCompEngineMaxSize - 4096;
static uint8_t compressed_data_buf[kCompressedBufSize];
static uint16_t compressed_data_size;  // Calculated at run-time;
static uint16_t last_cp_output_data_len;

static uint16_t kCPVersion = 0x1234;

static dp_mem_t *uncompressed_buf;
static dp_mem_t *uncompressed_host_buf;
static dp_mem_t *compressed_buf;
static dp_mem_t *compressed_host_buf;

static dp_mem_t *status_buf;
static dp_mem_t *status_buf2;
static dp_mem_t *status_host_buf;
static dp_mem_t *status_host_buf2;
static dp_mem_t *opaque_host_buf;
static dp_mem_t *hash_status_host_vec;
static dp_mem_t *hash_opaque_host_vec;
static dp_mem_t *decomp_status_host;

static dp_mem_t *host_sgl1;
static dp_mem_t *host_sgl2;
static dp_mem_t *host_sgl3;
static dp_mem_t *host_sgl4;
static dp_mem_t *seq_sgl;

static dp_mem_t *xts_status_host_vec;

static comp_encrypt_chain_t   *comp_encrypt_chain;
static comp_hash_chain_t      *comp_hash_chain;
static decrypt_decomp_chain_t *decrypt_decomp_chain;
static chksum_decomp_chain_t  *chksum_decomp_chain;

dp_mem_t *comp_pad_buf;

// Forward declaration with default param values
int run_cp_test(cp_desc_t& desc,
                dp_mem_t *dst_buf,
                dp_mem_t *status,
                acc_ring_push_t push_type = ACC_RING_PUSH_HW_DIRECT,
                uint32_t seq_comp_qid = 0);
int run_dc_test(cp_desc_t& desc,
                dp_mem_t *status,
                uint32_t exp_output_data_len,
                acc_ring_push_t push_type = ACC_RING_PUSH_HW_DIRECT,
                uint32_t seq_comp_qid = 0);

/*
 * Format a packed SGL vector where each tuple block length is of a fixed
 * size. The packed format will result in more effort in P4+ for work such
 * as compression padding, in terms of computation and DMA resources required
 * to determine which tuples to adjust and pad, and which tuples to clear.
 *
 * The more prefered format would have been the sparse SGL which, just prior
 * to Capri1 tape out, was found to fail in stress tests and was declared
 * as not-supported by the ASIC team.
 */
void
comp_sgl_packed_fill(dp_mem_t *comp_sgl_vec,
                     dp_mem_t *comp_buf,
                     uint32_t blk_size)
{
    cp_sgl_t    *comp_sgl;
    uint64_t    comp_buf_addr;
    uint32_t    comp_buf_size;
    uint32_t    block_no = 0;
    uint32_t    save_curr_line;

    comp_buf_addr = comp_buf->pa();
    comp_buf_size = comp_buf->line_size_get();
    save_curr_line = comp_sgl_vec->line_get();

    while (comp_buf_size) {
        assert(comp_sgl_vec->num_lines_get() >= (block_no + 1));
        comp_sgl_vec->line_set(block_no++);
        comp_sgl_vec->clear();

        comp_sgl = (cp_sgl_t *)comp_sgl_vec->read();
        comp_sgl->len0 = std::min(comp_buf_size, blk_size);
        comp_sgl->addr0 = comp_buf_addr;

        comp_buf_addr += comp_sgl->len0;
        comp_buf_size -= comp_sgl->len0;
        comp_sgl->len1 = std::min(comp_buf_size, blk_size);
        if (comp_sgl->len1 == 0) {
            comp_sgl_vec->write_thru();
            break;
        }
        comp_sgl->addr1 = comp_buf_addr;

        comp_buf_addr += comp_sgl->len1;
        comp_buf_size -= comp_sgl->len1;
        comp_sgl->len2 = std::min(comp_buf_size, blk_size);
        if (comp_sgl->len2 == 0) {
            comp_sgl_vec->write_thru();
            break;
        }
        comp_sgl->addr2 = comp_buf_addr;

        comp_buf_addr += comp_sgl->len2;
        comp_buf_size -= comp_sgl->len2;
        if (comp_buf_size) {
            comp_sgl->link = comp_sgl_vec->pa() + comp_sgl_vec->line_size_get();
        }
        comp_sgl_vec->write_thru();
    }
    comp_sgl_vec->line_set(save_curr_line);
}

/*
 * Print debug trace info for an SGL vector
 */
void
comp_sgl_trace(const char *comp_sgl_name,
               dp_mem_t *comp_sgl_vec,
               uint32_t max_blks,
               bool honor_link)
{
    cp_sgl_t        *comp_sgl;
    uint32_t        block_no;
    uint32_t        num_blks;
    uint32_t        save_curr_line;

    save_curr_line = comp_sgl_vec->line_get();
    num_blks = std::min(comp_sgl_vec->num_lines_get(), max_blks);

    for (block_no = 0; block_no < num_blks; block_no++) {
        comp_sgl_vec->line_set(block_no);
        comp_sgl = (cp_sgl_t *)comp_sgl_vec->read_thru();
        printf("%s 0x%lx block %u addr0 0x%lx len0 %u addr1 0x%lx len1 %u "
               "addr2 0x%lx len2 %u link 0x%lx\n", comp_sgl_name,
               comp_sgl_vec->pa(), block_no,
               comp_sgl->addr0, comp_sgl->len0, comp_sgl->addr1, comp_sgl->len1,
               comp_sgl->addr2, comp_sgl->len2, comp_sgl->link);

        if (honor_link && !comp_sgl->link) {
            break;
        }
    }
    comp_sgl_vec->line_set(save_curr_line);
}

/*
 * Format a packed chain SGL for PDMA purposes. Note that TxDMA mem2mem has
 * a transfer limit of 14 bits (16K - 1) so each SGL addr/len must be
 * within this limit.
 */
void
chain_sgl_pdma_packed_fill(dp_mem_t *seq_sgl_pdma,
                           dp_mem_t *dst_buf)
{
    chain_sgl_pdma_t    *sgl_pdma_entry;
    uint64_t            dst_buf_addr;
    uint32_t            dst_buf_size;
    uint32_t            pdma_size;

    dst_buf_addr = dst_buf->pa();
    dst_buf_size = dst_buf->line_size_get();

    seq_sgl_pdma->clear();
    sgl_pdma_entry = (chain_sgl_pdma_t *)seq_sgl_pdma->read();
    for (uint32_t i = 0; i < ARRAYSIZE(sgl_pdma_entry->tuple); i++) {
        pdma_size = dst_buf_size > kMaxMem2MemSize ?
                    kMaxMem2MemSize : dst_buf_size;
        sgl_pdma_entry->tuple[i].addr = dst_buf_addr;
        sgl_pdma_entry->tuple[i].len = pdma_size;

        dst_buf_addr += pdma_size;
        dst_buf_size -= pdma_size;
    }

    assert(dst_buf_size == 0);
    seq_sgl_pdma->write_thru();
}

bool
comp_status_poll(dp_mem_t *status,
                 const cp_desc_t& desc,
                 bool suppress_log)
{
  uint32_t poll_factor;
  uint32_t datain_len;

  auto func = [status, suppress_log] () -> int {
    cp_status_sha512_t *s = (cp_status_sha512_t *)status->read_thru();
    if (s->valid) {
      if (!suppress_log) {
          printf("Got status %llx\n", *((unsigned long long *)s));
      }
      return 0;
    }
    return 1;
  };

  datain_len = desc.datain_len == 0 ?
               kCompEngineMaxSize : desc.datain_len;
  poll_factor = (datain_len + kCompAppMinSize - 1) / kCompAppMinSize;
  tests::Poller poll(FLAGS_long_poll_interval * poll_factor);
  if (poll(func) == 0)
    return true;
  return false;
}

#define LOG_CHECK_PRINTF(fmt, ...)  \
    if (log_error) {printf(fmt, __VA_ARGS__);}

int
compress_status_verify(dp_mem_t *status,
                       dp_mem_t *dst_buf,
                       const cp_desc_t& desc,
                       bool log_error,
                       uint32_t expected_status)
{
    cp_status_sha512_t *st = (cp_status_sha512_t *)status->read_thru();

    if (!st->valid) {
        LOG_CHECK_PRINTF("%s ERROR: Status valid bit not set\n", __func__);
        return -1;
    }
    if (st->err == expected_status) {
        if (expected_status != CP_STATUS_SUCCESS) {
            LOG_CHECK_PRINTF("%s Got expected status 0x%x\n",
                             __func__, st->err);
            // No further checks needed
            return 0;
        }

    } else {
        LOG_CHECK_PRINTF("%s ERROR: status err 0x%x is unexpected\n",
                         __func__, st->err);
        return -1;
    }
    if (st->partial_data != desc.status_data) {
        LOG_CHECK_PRINTF("%s ERROR: partial data mismatch, expected 0x%x, "
                         "received 0x%x\n", __func__, desc.status_data,
                         st->partial_data);
        return -1;
    }
    if (desc.cmd_bits.comp_decomp_en && desc.cmd_bits.insert_header) {
        dp_mem_t *hdr_buf = dst_buf->fragment_find(0, sizeof(cp_hdr_t));
        cp_hdr_t *hdr = (cp_hdr_t *)hdr_buf->read_thru();

        if (hdr->version != kCPVersion) {
            LOG_CHECK_PRINTF("%s Header version mismatch, expected 0x%x, "
                             "received 0x%x\n", __func__, kCPVersion, hdr->version);
            return -1;
        }
        if (hdr->cksum == 0) {
            LOG_CHECK_PRINTF("%s ERROR: Header with zero cksum\n", __func__);
            return -1;
        }
        uint32_t datain_len = desc.datain_len == 0 ?
                              kCompEngineMaxSize : desc.datain_len;
        if ((hdr->data_len == 0) ||
            (hdr->data_len > (dst_buf->line_size_get() - sizeof(cp_hdr_t))) ||
            (hdr->data_len > datain_len)) {

            LOG_CHECK_PRINTF("%s ERROR: Invalid data_len 0x%x\n",
                             __func__, hdr->data_len);
            return -1;
        }
        if (st->output_data_len != (hdr->data_len + sizeof(cp_hdr_t))) {
            LOG_CHECK_PRINTF("%s ERROR: output_data_len %u mismatches hdr size %d\n",
                             __func__, st->output_data_len,
                             (int)(hdr->data_len + sizeof(cp_hdr_t)));
            return -1;
        }
    }
    if (desc.cmd_bits.sha_en) {
        int sha_size = desc.cmd_bits.sha_type ? 32 : 64;
        if (memcmp(st->sha512, all_zeros, sha_size) == 0) {
            LOG_CHECK_PRINTF("%s ERROR: Sha is all zero\n", __func__);
            return -1;
        }
    }
    if (st->integrity_data == 0) {
        LOG_CHECK_PRINTF("%s Integrity is all zero\n", __func__);
        return -1;
    }
    return 0;
}

int
decompress_status_verify(dp_mem_t *status,
                         const cp_desc_t& desc,
                         uint32_t exp_output_data_len,
                         bool log_error)
{
    cp_status_sha512_t *st = (cp_status_sha512_t *)status->read_thru();

    if (!st->valid) {
        LOG_CHECK_PRINTF("%s ERROR: Status valid bit not set\n", __func__);
        return -1;
    }
    if (st->err) {
        LOG_CHECK_PRINTF("%s ERROR: status err 0x%x is unexpected\n",
                         __func__, st->err);
        return -1;
    }
    if (st->partial_data != desc.status_data) {
        LOG_CHECK_PRINTF("%s ERROR: partial data mismatch, expected 0x%x, "
                         "received 0x%x\n",__func__, desc.status_data,
                         st->partial_data);
        return -1;
    }
    if (st->output_data_len != exp_output_data_len) {
        LOG_CHECK_PRINTF("%s ERROR: output data len mismatch, expected %u, "
                         "received %u\n", __func__, exp_output_data_len,
                         st->output_data_len);
        return -1;
    }
    return 0;
}

uint32_t
comp_status_output_data_len_get(dp_mem_t *status)
{
    cp_status_sha512_t *st = (cp_status_sha512_t *)status->read_thru();
    return st->output_data_len;
}


uint64_t
comp_status_integrity_data_get(dp_mem_t *status)
{
    cp_status_sha512_t *st = (cp_status_sha512_t *)status->read_thru();
    return st->integrity_data;
}


// Calculate comp sequencer status producer index
// then fill the next available seq_status_desc with the
// given chaining parameters.
int
seq_comp_status_desc_fill(chain_params_comp_t& chain_params)
{
    STORAGE_SEQ_CS_DESC0_DEFINE(desc0_action) = {0};
    STORAGE_SEQ_CS_DESC1_DEFINE(desc1_action) = {0};
    dp_mem_t    *seq_status_desc;

    seq_status_desc = queues::seq_sq_consume_entry(chain_params.seq_spec.seq_status_q,
                                                   &chain_params.seq_spec.ret_seq_status_index);
    seq_status_desc->clear();

    // desc bytes 0-63
    if (chain_params.next_db_action_barco_push) {
        STORAGE_SEQ_CS_DESC0_ARRAY_SET(desc0_action, next_db_addr,
                                       chain_params.push_spec.barco_ring_addr);
        STORAGE_SEQ_CS_DESC0_ARRAY_SET(desc0_action, next_db_data,
                                       chain_params.push_spec.barco_desc_addr);
        STORAGE_SEQ_CS_DESC0_ARRAY_SET(desc0_action, barco_pndx_addr,
                                       chain_params.push_spec.barco_pndx_addr);
        STORAGE_SEQ_CS_DESC0_ARRAY_SET(desc0_action, barco_pndx_shadow_addr,
                                       chain_params.push_spec.barco_pndx_shadow_addr);

        STORAGE_SEQ_CS_DESC0_SCALAR_SET(desc0_action, barco_desc_size,
                                        chain_params.push_spec.barco_desc_size);
        STORAGE_SEQ_CS_DESC0_SCALAR_SET(desc0_action, barco_pndx_size,
                                        chain_params.push_spec.barco_pndx_size);
        STORAGE_SEQ_CS_DESC0_SCALAR_SET(desc0_action, barco_ring_size,
                                        chain_params.push_spec.barco_ring_size);
        STORAGE_SEQ_CS_DESC0_SCALAR_SET(desc0_action, barco_num_descs,
                                        chain_params.push_spec.barco_num_descs);

    } else {
        STORAGE_SEQ_CS_DESC0_ARRAY_SET(desc0_action, next_db_addr,
                                       chain_params.db_spec.next_doorbell_addr);
        STORAGE_SEQ_CS_DESC0_ARRAY_SET(desc0_action, next_db_data,
                                       chain_params.db_spec.next_doorbell_data);
    }

    STORAGE_SEQ_CS_DESC0_ARRAY_SET(desc0_action, status_addr0,
                                   chain_params.status_addr0);
    STORAGE_SEQ_CS_DESC0_ARRAY_SET(desc0_action, status_addr1,
                                   chain_params.status_addr1);
    STORAGE_SEQ_CS_DESC0_ARRAY_SET(desc0_action, intr_addr,
                                   chain_params.intr_addr);
    STORAGE_SEQ_CS_DESC0_SCALAR_SET(desc0_action, intr_data,
                                    chain_params.intr_data);
    STORAGE_SEQ_CS_DESC0_SCALAR_SET(desc0_action, status_len,
                                    chain_params.status_len);
    STORAGE_SEQ_CS_DESC0_SCALAR_SET(desc0_action, status_offset0,
                                    chain_params.status_offset0);
    STORAGE_SEQ_CS_DESC0_SCALAR_SET(desc0_action, status_dma_en,
                                    chain_params.status_dma_en);
    STORAGE_SEQ_CS_DESC0_SCALAR_SET(desc0_action, next_db_en,
                                    chain_params.next_doorbell_en);
    STORAGE_SEQ_CS_DESC0_SCALAR_SET(desc0_action, intr_en,
                                    chain_params.intr_en);
    STORAGE_SEQ_CS_DESC0_SCALAR_SET(desc0_action, next_db_action_barco_push,
                                    chain_params.next_db_action_barco_push);
    STORAGE_SEQ_CS_DESC0_SCALAR_SET(desc0_action, num_alt_descs,
                                    chain_params.num_alt_descs);
    /*
     * There's really no rate limiting in model but we'll enable all
     * possible rate limiting modes to exercise P4+ code.
     */
    STORAGE_SEQ_CS_DESC0_SCALAR_SET(desc0_action, rate_limit_en, 1);
    STORAGE_SEQ_CS_DESC0_SCALAR_SET(desc0_action, rate_limit_src_en, 1);
    STORAGE_SEQ_CS_DESC0_SCALAR_SET(desc0_action, rate_limit_dst_en, 1);
    STORAGE_SEQ_CS_DESC0_PACK(seq_status_desc->read(), desc0_action);


    // desc bytes 64-127
    STORAGE_SEQ_CS_DESC1_ARRAY_SET(desc1_action, comp_buf_addr,
                                   chain_params.comp_buf_addr);
    STORAGE_SEQ_CS_DESC1_ARRAY_SET(desc1_action, aol_src_vec_addr,
                                   chain_params.aol_src_vec_addr);
    STORAGE_SEQ_CS_DESC1_ARRAY_SET(desc1_action, aol_dst_vec_addr,
                                   chain_params.aol_dst_vec_addr);
    STORAGE_SEQ_CS_DESC1_ARRAY_SET(desc1_action, sgl_vec_addr,
                                   chain_params.sgl_vec_addr);
    STORAGE_SEQ_CS_DESC1_ARRAY_SET(desc1_action, pad_buf_addr,
                                   chain_params.pad_buf_addr);
    STORAGE_SEQ_CS_DESC1_ARRAY_SET(desc1_action, alt_buf_addr,
                                   chain_params.alt_buf_addr);
    STORAGE_SEQ_CS_DESC1_SCALAR_SET(desc1_action, data_len,
                                    chain_params.data_len);
    STORAGE_SEQ_CS_DESC1_SCALAR_SET(desc1_action, alt_data_len,
                                    chain_params.alt_data_len);
    STORAGE_SEQ_CS_DESC1_SCALAR_SET(desc1_action, pad_boundary_shift,
                                    chain_params.pad_boundary_shift);
    STORAGE_SEQ_CS_DESC1_SCALAR_SET(desc1_action, stop_chain_on_error,
                                    chain_params.stop_chain_on_error);
    STORAGE_SEQ_CS_DESC1_SCALAR_SET(desc1_action, data_len_from_desc,
                                    chain_params.data_len_from_desc);
    STORAGE_SEQ_CS_DESC1_SCALAR_SET(desc1_action, aol_update_en,
                                    chain_params.aol_update_en);
    STORAGE_SEQ_CS_DESC1_SCALAR_SET(desc1_action, sgl_update_en,
                                    chain_params.sgl_update_en);
    STORAGE_SEQ_CS_DESC1_SCALAR_SET(desc1_action, sgl_sparse_format_en,
                                    chain_params.sgl_sparse_format_en);
    STORAGE_SEQ_CS_DESC1_SCALAR_SET(desc1_action, sgl_pdma_en,
                                    chain_params.sgl_pdma_en);
    STORAGE_SEQ_CS_DESC1_SCALAR_SET(desc1_action, sgl_pdma_pad_only,
                                    chain_params.sgl_pdma_pad_only);
    STORAGE_SEQ_CS_DESC1_SCALAR_SET(desc1_action, sgl_pdma_alt_src_on_error,
                                    chain_params.sgl_pdma_alt_src_on_error);
    STORAGE_SEQ_CS_DESC1_SCALAR_SET(desc1_action, desc_vec_push_en,
                                    chain_params.desc_vec_push_en);
    STORAGE_SEQ_CS_DESC1_SCALAR_SET(desc1_action, chain_alt_desc_on_error,
                                    chain_params.chain_alt_desc_on_error);
    STORAGE_SEQ_CS_DESC1_SCALAR_SET(desc1_action, integ_data0_wr_en,
                                    chain_params.integ_data0_wr_en);
    STORAGE_SEQ_CS_DESC1_SCALAR_SET(desc1_action, integ_data_null_en,
                                    chain_params.integ_data_null_en);
    STORAGE_SEQ_CS_DESC1_SCALAR_SET(desc1_action, desc_dlen_update_en,
                                    chain_params.desc_dlen_update_en);
    STORAGE_SEQ_CS_DESC1_SCALAR_SET(desc1_action, hdr_version,
                                    chain_params.hdr_version);
    STORAGE_SEQ_CS_DESC1_SCALAR_SET(desc1_action, hdr_version_wr_en,
                                    chain_params.hdr_version_wr_en);
    STORAGE_SEQ_CS_DESC1_SCALAR_SET(desc1_action, cp_hdr_update_en,
                                    chain_params.cp_hdr_update_en);
    STORAGE_SEQ_CS_DESC1_SCALAR_SET(desc1_action, status_len_no_hdr,
                                    chain_params.status_len_no_hdr);
    STORAGE_SEQ_CS_DESC1_SCALAR_SET(desc1_action, padding_en,
                                    chain_params.padding_en);
    STORAGE_SEQ_CS_DESC1_PACK(seq_status_desc->read() +
                              STORAGE_SEQ_P4PD_TABLE_BYTE_WIDTH_DFLT, desc1_action);

    seq_status_desc->write_thru();

    // Form the doorbell to be returned by the API
    queues::get_capri_doorbell(queues::get_seq_lif(), SQ_TYPE,
                               chain_params.seq_spec.seq_status_q, 0,
                               chain_params.seq_spec.ret_seq_status_index,
                               &chain_params.seq_spec.ret_doorbell_addr,
                               &chain_params.seq_spec.ret_doorbell_data);
    return 0;
}

uint64_t
queue_mem_pa_get(uint64_t reg_addr)
{
    uint32_t lo_val, hi_val;

    lo_val = READ_REG32(reg_addr);
    hi_val = READ_REG32(reg_addr + 4);
    return ((uint64_t)hi_val << 32) | lo_val;
}

void
compression_buf_init()
{
    uint32_t    max_hash_blks;

    uncompressed_buf = new dp_mem_t(1, kUncompressedDataSize,
                                    DP_MEM_ALIGN_PAGE);
    uncompressed_host_buf = new dp_mem_t(1, kUncompressedDataSize,
                                         DP_MEM_ALIGN_PAGE, DP_MEM_TYPE_HOST_MEM);
    // size of compressed buffers accounts any for compression failure,
    // i.e., must be as large as uncompressed buffer plus cp_hdr_t
    compressed_buf = new dp_mem_t(1, kUncompressedDataSize + sizeof(cp_hdr_t),
                                  DP_MEM_ALIGN_PAGE);
    compressed_host_buf = new dp_mem_t(1, kUncompressedDataSize + sizeof(cp_hdr_t),
                                       DP_MEM_ALIGN_PAGE, DP_MEM_TYPE_HOST_MEM);
    status_buf = new dp_mem_t(1, sizeof(cp_status_sha512_t),
                              DP_MEM_ALIGN_SPEC, DP_MEM_TYPE_HBM,
                              kMinHostMemAllocSize);
    status_buf2 = new dp_mem_t(1, sizeof(cp_status_sha512_t),
                               DP_MEM_ALIGN_SPEC, DP_MEM_TYPE_HBM,
                               kMinHostMemAllocSize);
    status_host_buf = new dp_mem_t(1, sizeof(cp_status_sha512_t),
                                   DP_MEM_ALIGN_SPEC, DP_MEM_TYPE_HOST_MEM,
                                   kMinHostMemAllocSize);
    status_host_buf2 = new dp_mem_t(1, sizeof(cp_status_sha512_t),
                                    DP_MEM_ALIGN_SPEC, DP_MEM_TYPE_HOST_MEM,
                                    kMinHostMemAllocSize);
    opaque_host_buf = new dp_mem_t(1, sizeof(uint64_t),
                                   DP_MEM_ALIGN_SPEC, DP_MEM_TYPE_HOST_MEM,
                                   kMinHostMemAllocSize);
    host_sgl1 = new dp_mem_t(1, sizeof(cp_sgl_t),
                             DP_MEM_ALIGN_SPEC, DP_MEM_TYPE_HOST_MEM,
                             sizeof(cp_sgl_t));
    host_sgl2 = new dp_mem_t(1, sizeof(cp_sgl_t),
                             DP_MEM_ALIGN_SPEC, DP_MEM_TYPE_HOST_MEM,
                             sizeof(cp_sgl_t));
    host_sgl3 = new dp_mem_t(1, sizeof(cp_sgl_t),
                             DP_MEM_ALIGN_SPEC, DP_MEM_TYPE_HOST_MEM,
                             sizeof(cp_sgl_t));
    host_sgl4 = new dp_mem_t(1, sizeof(cp_sgl_t),
                             DP_MEM_ALIGN_SPEC, DP_MEM_TYPE_HOST_MEM,
                             sizeof(cp_sgl_t));
    seq_sgl = new dp_mem_t(1, sizeof(chain_sgl_pdma_t),
                           DP_MEM_ALIGN_SPEC, DP_MEM_TYPE_HBM,
                           sizeof(chain_sgl_pdma_t));

    comp_pad_buf = new dp_mem_t(1, 4096, DP_MEM_ALIGN_PAGE);

    // Pre-fill input buffers.
    uint64_t *p64 = (uint64_t *)uncompressed_data;
    for (uint64_t i = 0; i < (kUncompressedDataSize/sizeof(uint64_t)); i++)
      p64[i] = i;

    memcpy(uncompressed_buf->read(), uncompressed_data,
           uncompressed_buf->line_size_get());
    uncompressed_buf->write_thru();
    memcpy(uncompressed_host_buf->read(), uncompressed_data,
          uncompressed_host_buf->line_size_get());
    uncompressed_host_buf->write_thru();

    // Create and initialize compression->XTS-encrypt chaining
    comp_encrypt_chain_params_t cec_ctor;
    comp_encrypt_chain =
         new comp_encrypt_chain_t(cec_ctor.app_max_size(kCompAppMaxSize).
                                           app_enc_size(kCompAppHashBlkSize).
                                           uncomp_mem_type(DP_MEM_TYPE_HOST_MEM).
                                           comp_mem_type1(DP_MEM_TYPE_HBM).
                                           comp_mem_type2(DP_MEM_TYPE_HOST_MEM).
                                           comp_status_mem_type1(DP_MEM_TYPE_HBM).
                                           comp_status_mem_type2(DP_MEM_TYPE_HOST_MEM).
                                           encrypt_mem_type(DP_MEM_TYPE_HOST_MEM).
                                           destructor_free_buffers(true));
    max_hash_blks = COMP_MAX_HASH_BLKS(kCompAppMaxSize, kCompAppHashBlkSize);
    xts_status_host_vec = new dp_mem_t(max_hash_blks, sizeof(xts::xts_status_t),
                                       DP_MEM_ALIGN_SPEC, DP_MEM_TYPE_HOST_MEM,
                                       sizeof(xts::xts_status_t));
    comp_encrypt_chain_pre_push_params_t cec_pre_push;
    comp_encrypt_chain->pre_push(cec_pre_push.caller_comp_pad_buf(comp_pad_buf).
                                              caller_xts_status_vec(xts_status_host_vec).
                                              caller_xts_opaque_vec(nullptr).
                                              caller_xts_opaque_data(0));

    // Create and initialize XTS-decrypt->decompression chaining
    decrypt_decomp_chain_params_t ddc_ctor;
    decrypt_decomp_chain =
         new decrypt_decomp_chain_t(ddc_ctor.app_max_size(kCompAppMaxSize).
                                             app_enc_size(kCompAppHashBlkSize).
                                             uncomp_mem_type(DP_MEM_TYPE_HOST_MEM).
                                             xts_status_mem_type1(DP_MEM_TYPE_HBM).
                                             xts_status_mem_type2(DP_MEM_TYPE_HOST_MEM).
                                             decrypt_mem_type1(DP_MEM_TYPE_HBM).
                                             decrypt_mem_type2(DP_MEM_TYPE_HOST_MEM).
                                             destructor_free_buffers(true));
    decrypt_decomp_chain_pre_push_params_t ddc_pre_push;
    ddc_pre_push.caller_comp_status_buf(
        test_mem_type_workaround(DP_MEM_TYPE_HOST_MEM) == DP_MEM_TYPE_HOST_MEM ?
        status_host_buf : status_buf);
    decrypt_decomp_chain->pre_push(ddc_pre_push.caller_comp_opaque_buf(nullptr).
                                                caller_comp_opaque_data(0));
    // Create and initialize compression->hash chaining
    comp_hash_chain_params_t chc_ctor;
    comp_hash_chain =
         new comp_hash_chain_t(chc_ctor.app_max_size(kCompAppMaxSize).
                                        uncomp_mem_type(DP_MEM_TYPE_HOST_MEM).
                                        comp_mem_type1(DP_MEM_TYPE_HBM).
                                        comp_mem_type2(DP_MEM_TYPE_HOST_MEM).
                                        comp_status_mem_type1(DP_MEM_TYPE_HBM).
                                        comp_status_mem_type2(DP_MEM_TYPE_HOST_MEM).
                                        destructor_free_buffers(true));
    hash_status_host_vec = new dp_mem_t(max_hash_blks, CP_STATUS_PAD_ALIGNED_SIZE,
                                        DP_MEM_ALIGN_SPEC,
                                        test_mem_type_workaround(DP_MEM_TYPE_HOST_MEM),
                                        kMinHostMemAllocSize);
    hash_opaque_host_vec = new dp_mem_t(max_hash_blks, sizeof(uint64_t),
                                        DP_MEM_ALIGN_SPEC,
                                        test_mem_type_workaround(DP_MEM_TYPE_HOST_MEM),
                                        kMinHostMemAllocSize);
    comp_hash_chain_pre_push_params_t chc_pre_push;
    comp_hash_chain->pre_push(chc_pre_push.caller_comp_pad_buf(comp_pad_buf).
                                           caller_hash_status_vec(hash_status_host_vec).
                                           caller_hash_opaque_vec(hash_opaque_host_vec).
                                           caller_hash_opaque_data(kCompHashIntrData));
    // Create and initialize checksum-decompression chaining
    chksum_decomp_chain_params_t cdc_ctor;
    chksum_decomp_chain =
         new chksum_decomp_chain_t(cdc_ctor.app_max_size(kCompAppMaxSize).
                                            uncomp_mem_type(DP_MEM_TYPE_HOST_MEM).
                                            destructor_free_buffers(true));
    decomp_status_host = new dp_mem_t(1, sizeof(cp_status_sha512_t),
                                      DP_MEM_ALIGN_SPEC, DP_MEM_TYPE_HOST_MEM,
                                      kMinHostMemAllocSize);
    chksum_decomp_chain_pre_push_params_t cdc_pre_push;
    chksum_decomp_chain->pre_push(cdc_pre_push.caller_decomp_status_buf(decomp_status_host));

#ifdef ELBA
    int rv = utils::hbm_addr_alloc(sizeof(uint64_t), &tmp_db_addr);
    if (rv < 0) {
      printf("Error can't allocate tmp_db_addr for CP/DC in HBM\n");
      tmp_db_addr = 0;
     }
#endif

}

static acc_ring_t *
compression_ring_create(const char *ring_name,
                        accel_ring_id_t ring_id,
                        uint64_t cfg_csr_pa,
                        uint64_t cfg_opaque_csr_pa,
                        uint64_t ring_pndx_pa,
                        uint32_t ring_size)
{
    accel_ring_t    *nicmgr_accel_ring;
    uint64_t        ring_base_pa;
    uint64_t        ring_opaque_tag_pa = 0;
    uint64_t        ring_shadow_pndx_pa = 0;
    uint32_t        desc_size = sizeof(cp_desc_t);
    uint32_t        pi_size = sizeof(uint32_t);
    uint32_t        opaque_tag_size = sizeof(uint32_t);

    /*
     * If working with nicmgr, use the info discovered during the identify phase
     */
    if (run_nicmgr_tests) {
        nicmgr_accel_ring = &queues::nicmgr_accel_ring_tbl[ring_id];
        ring_size = nicmgr_accel_ring->ring_size;
        ring_base_pa = nicmgr_accel_ring->ring_base_pa;
        ring_pndx_pa = nicmgr_accel_ring->ring_pndx_pa;
        ring_shadow_pndx_pa = nicmgr_accel_ring->ring_shadow_pndx_pa;
        ring_opaque_tag_pa = nicmgr_accel_ring->ring_opaque_tag_pa;
        desc_size = nicmgr_accel_ring->ring_desc_size;
        pi_size = nicmgr_accel_ring->ring_pndx_size;
        opaque_tag_size = nicmgr_accel_ring->ring_opaque_tag_size;

    } else {
        ring_base_pa = queue_mem_pa_get(cfg_csr_pa);
        if (cfg_opaque_csr_pa) {
            ring_opaque_tag_pa = queue_mem_pa_get(cfg_opaque_csr_pa);
        }
    }

    printf("%s ring_size %u desc_size %u pi_size %u opaque_tag_size %u\n",
           __FUNCTION__, ring_size, desc_size, pi_size, opaque_tag_size);

    return new acc_ring_t(ring_name, ring_pndx_pa, ring_shadow_pndx_pa,
                          ring_size, desc_size, ring_base_pa, pi_size,
                          ring_opaque_tag_pa, opaque_tag_size);
}

void
compression_init()
{
  uint32_t cp_ring_size = kMaxSubqEntries;
  uint32_t cp_hot_ring_size = kMaxSubqEntries;
  uint32_t dc_ring_size = kMaxSubqEntries;
  uint32_t dc_hot_ring_size = kMaxSubqEntries;
  uint32_t lo_reg, hi_reg;

  init_cpdc_registers();
  dump_cpdc_registers();

  lo_reg = READ_REG32(cp_cfg_glob);
  if (comp_inited_by_hal) {
      kCPVersion = lo_reg & 0xffff;
      printf("Comp version is 0x%x\n", kCPVersion);

      lo_reg = READ_REG32(cp_cfg_dist);
      cp_ring_size = (lo_reg >> 2) & 0xfff;
      cp_hot_ring_size = (lo_reg >> 14) & 0xfff;
      lo_reg = READ_REG32(dc_cfg_dist);
      dc_ring_size = (lo_reg >> 2) & 0xfff;
      dc_hot_ring_size = (lo_reg >> 14) & 0xfff;
      if (!cp_ring_size) cp_ring_size = kMaxSubqEntries;
      if (!cp_hot_ring_size) cp_hot_ring_size = kMaxSubqEntries;
      if (!dc_ring_size) dc_ring_size = kMaxSubqEntries;
      if (!dc_hot_ring_size) dc_hot_ring_size = kMaxSubqEntries;
  }

  cp_ring = compression_ring_create("cp_ring", ACCEL_RING_CP, cp_cfg_q_base,
                                    cp_cfg_host_opaque_tag_addr, cp_cfg_q_pd_idx,
                                    cp_ring_size);
  cp_hot_ring = compression_ring_create("cp_hot_ring", ACCEL_RING_CP_HOT,
                                        cp_cfg_hotq_base, 0,
                                        cp_cfg_hotq_pd_idx, cp_hot_ring_size);
  dc_ring = compression_ring_create("dc_ring", ACCEL_RING_DC, dc_cfg_q_base,
                                    dc_cfg_host_opaque_tag_addr,
                                    dc_cfg_q_pd_idx, dc_ring_size);
  dc_hot_ring = compression_ring_create("dc_hot_ring", ACCEL_RING_DC_HOT,
                                        dc_cfg_hotq_base, 0,
                                        dc_cfg_hotq_pd_idx, dc_hot_ring_size);
  compression_buf_init();

  if (!comp_inited_by_hal) {

      // Write cp queue base.
      WRITE_REG32(cp_cfg_glob, (lo_reg & 0xFFFF0000u) | kCPVersion);
      WRITE_REG32(cp_cfg_q_base, cp_ring->ring_base_mem_pa_get() & 0xFFFFFFFFu);
      WRITE_REG32(cp_cfg_q_base + 4, (cp_ring->ring_base_mem_pa_get() >> 32) & 0xFFFFFFFFu);

      WRITE_REG32(cp_cfg_hotq_base, cp_hot_ring->ring_base_mem_pa_get() & 0xFFFFFFFFu);
      WRITE_REG32(cp_cfg_hotq_base + 4, (cp_hot_ring->ring_base_mem_pa_get() >> 32) & 0xFFFFFFFFu);

      // Write dc queue base.
      lo_reg = READ_REG32(dc_cfg_glob);
      WRITE_REG32(dc_cfg_glob, (lo_reg & 0xFFFF0000u) | kCPVersion);
      WRITE_REG32(dc_cfg_q_base, dc_ring->ring_base_mem_pa_get() & 0xFFFFFFFFu);
      WRITE_REG32(dc_cfg_q_base + 4, (dc_ring->ring_base_mem_pa_get() >> 32) & 0xFFFFFFFFu);

      WRITE_REG32(dc_cfg_hotq_base, dc_hot_ring->ring_base_mem_pa_get() & 0xFFFFFFFFu);
      WRITE_REG32(dc_cfg_hotq_base + 4, (dc_hot_ring->ring_base_mem_pa_get() >> 32) & 0xFFFFFFFFu);

      // Enable all 16 cp engines.
      lo_reg = READ_REG32(cp_cfg_ueng);
      hi_reg = READ_REG32(cp_cfg_ueng+4);
      lo_reg |= 0xFFFF;
      hi_reg &= ~(1u << (54 - 32));
      hi_reg &= ~(1u << (53 - 32));
      hi_reg |= (1u << (36 - 32)) |
                (1u << (55 - 32));
      WRITE_REG32(cp_cfg_ueng, lo_reg);
      WRITE_REG32(cp_cfg_ueng+4, hi_reg);
      // Enable both DC engines.
      lo_reg = READ_REG32(dc_cfg_ueng);
      hi_reg = READ_REG32(dc_cfg_ueng+4);
      lo_reg |= 0x3;
      hi_reg &= ~(1u << (54 - 32));
      hi_reg &= ~(1u << (53 - 32));
      hi_reg |= (1u << (36 - 32)) |
                (1u << (55 - 32));
      WRITE_REG32(dc_cfg_ueng, lo_reg);
      WRITE_REG32(dc_cfg_ueng+4, hi_reg);

      // Enable all queues ...
      lo_reg = READ_REG32(cp_cfg_dist);
#ifdef ELBA
      lo_reg |= 0xFF;
#else
      lo_reg |= 1;
#endif
      WRITE_REG32(cp_cfg_dist, lo_reg);
      lo_reg = READ_REG32(dc_cfg_dist);
#ifdef ELBA
      lo_reg |= 0xFF;
#else
      lo_reg |= 1;
#endif
      WRITE_REG32(dc_cfg_dist, lo_reg);
  }

  dump_cpdc_registers();
  printf("Compression init done\n");
}

/*
 * This function may be invoked at the end of any series of tests to resync
 * shadow-to-PI for all rings.
 */
int
compression_resync()
{
    cp_ring->resync();
    cp_hot_ring->resync();
    dc_ring->resync();
    dc_hot_ring->resync();
    return 0;
}

int run_cp_test(cp_desc_t& desc,
                dp_mem_t *dst_buf,
                dp_mem_t *status,
                acc_ring_push_t push_type,
                uint32_t seq_comp_qid)
{
    status->clear_thru();
    cp_ring->push((const void *)&desc, push_type, seq_comp_qid);
    if (!comp_status_poll(status, desc)) {
      printf("ERROR: status never came\n");
      return -1;
    }

    if (compress_status_verify(status, dst_buf, desc) == 0) {
        last_cp_output_data_len = comp_status_output_data_len_get(status);
        printf("Last output data len = %u\n", last_cp_output_data_len);
        return 0;
    }
    return -1;
}

int run_dc_test(cp_desc_t& desc,
                dp_mem_t *status,
                uint32_t exp_output_data_len,
                acc_ring_push_t push_type,
                uint32_t seq_comp_qid)
{
    status->clear_thru();
    dc_ring->push((const void *)&desc, push_type, seq_comp_qid);
    if (!comp_status_poll(status, desc)) {
      printf("ERROR: status never came\n");
      return -1;
    }
    return decompress_status_verify(status, desc, exp_output_data_len);
}

void compress_cp_desc_template_fill(cp_desc_t &d,
                                    dp_mem_t *src_buf,
                                    dp_mem_t *dst_buf,
                                    dp_mem_t *status_buf,
                                    dp_mem_t *invalidate_hdr_buf,
                                    uint32_t src_len)
{
    memset(&d, 0, sizeof(d));
    d.cmd_bits.comp_decomp_en = 1;
    d.cmd_bits.insert_header = 1;
    d.src = src_buf->pa();
    d.dst = dst_buf->pa();
    d.status_addr = status_buf->pa();
    d.datain_len = src_len == kCompEngineMaxSize ? 0 : src_len;
    d.threshold_len = src_len - sizeof(cp_hdr_t);
    d.status_data = 0x1234;

    if (invalidate_hdr_buf) {
        invalidate_hdr_buf->fragment_find(0, sizeof(cp_hdr_t))->clear_thru();
    }
}

void decompress_cp_desc_template_fill(cp_desc_t &d,
                                      dp_mem_t *src_buf,
                                      dp_mem_t *dst_buf,
                                      dp_mem_t *status_buf,
                                      uint32_t src_len,
                                      uint32_t threshold_len)
{
    memset(&d, 0, sizeof(d));
    d.cmd_bits.comp_decomp_en = 1;
    d.cmd_bits.header_present = 1;
    d.src = src_buf->pa();
    d.dst = dst_buf->pa();
    d.status_addr = status_buf->pa();
    d.datain_len = src_len == kCompEngineMaxSize ? 0 : src_len;
    d.threshold_len = threshold_len == kCompEngineMaxSize ? 0 : threshold_len;
    d.status_data = 0x3456;
    #ifdef ELBA
      d.doorbell_addr = tmp_db_addr;
      d.cmd_bits.doorbell_on = 1;
    #endif
}

int _compress_flat_64K_buf(acc_ring_push_t push_type,
                           uint32_t seq_comp_qid) {
  cp_desc_t d;

  printf("Starting testcase %s push_type %d seq_comp_qid %u\n",
         __func__, push_type, seq_comp_qid);
  compress_cp_desc_template_fill(d, uncompressed_host_buf, compressed_host_buf,
                                 status_host_buf, compressed_host_buf,
                                 kUncompressedDataSize);
#ifdef ELBA
  d.doorbell_addr = tmp_db_addr;
  d.cmd_bits.doorbell_on = 1;
#endif

  if (run_cp_test(d, compressed_host_buf, status_host_buf,
                  push_type, seq_comp_qid) < 0) {
    printf("Testcase %s failed\n", __func__);
    return -1;
  }
  printf("Testcase %s passed\n", __func__);
  // Save compressed data.
  cp_hdr_t *hdr = (cp_hdr_t *)compressed_host_buf->read_thru();
  memcpy(compressed_data_buf, hdr, hdr->data_len + sizeof(cp_hdr_t));
  compressed_data_size = hdr->data_len + sizeof(cp_hdr_t);
  return 0;
}

int compress_flat_64K_buf() {
  return _compress_flat_64K_buf(ACC_RING_PUSH_HW_DIRECT, 0);
}

int seq_compress_flat_64K_buf() {
  return _compress_flat_64K_buf(ACC_RING_PUSH_SEQUENCER,
                                queues::get_seq_comp_sq(0));
}

int _compress_same_src_and_dst(acc_ring_push_t push_type,
                               uint32_t seq_comp_qid) {
  cp_desc_t d;

  printf("Starting testcase %s push_type %d seq_comp_qid %u\n",
         __func__, push_type, seq_comp_qid);

  memcpy(compressed_buf->read(), uncompressed_data, kCompAppMinSize);
  compressed_buf->write_thru();
  compress_cp_desc_template_fill(d, compressed_buf, compressed_buf,
                                 status_buf, nullptr, kCompAppMinSize);
#ifdef ELBA
  d.doorbell_addr = tmp_db_addr;
  d.cmd_bits.doorbell_on = 1;
#endif

  if (run_cp_test(d, compressed_buf, status_buf,
                  push_type, seq_comp_qid) < 0) {
    printf("Testcase %s failed\n", __func__);
    return -1;
  }
  printf("Testcase %s passed\n", __func__);
  return 0;
}

int compress_same_src_and_dst() {
    return _compress_same_src_and_dst(ACC_RING_PUSH_HW_DIRECT, 0);
}

int seq_compress_same_src_and_dst() {
    return _compress_same_src_and_dst(ACC_RING_PUSH_SEQUENCER,
                                      queues::get_seq_comp_sq(0));
}

int _decompress_to_flat_64K_buf(acc_ring_push_t push_type,
                                uint32_t seq_comp_qid) {
  cp_desc_t d;

  printf("Starting testcase %s push_type %d seq_comp_qid %u\n",
         __func__, push_type, seq_comp_qid);

  uncompressed_host_buf->clear_thru();
  memcpy(compressed_host_buf->read(), compressed_data_buf, compressed_data_size);
  compressed_host_buf->write_thru();

  decompress_cp_desc_template_fill(d, compressed_host_buf, uncompressed_host_buf,
                                   status_host_buf, compressed_data_size,
                                   kUncompressedDataSize);
#ifdef ELBA
  d.doorbell_addr = tmp_db_addr;
  d.cmd_bits.doorbell_on = 1;
#endif
  if (run_dc_test(d, status_host_buf, 0,
                  push_type, seq_comp_qid) < 0) {
    printf("Testcase %s failed\n", __func__);
    return -1;
  }
  // Verify data buf
  if (test_data_verify_and_dump(uncompressed_data,
                                uncompressed_host_buf->read_thru(),
                                kUncompressedDataSize)) {
      printf("Testcase %s failed\n", __func__);
      return -1;
  }
  printf("Testcase %s passed\n", __func__);
  return 0;
}

int decompress_to_flat_64K_buf() {
    return _decompress_to_flat_64K_buf(ACC_RING_PUSH_HW_DIRECT, 0);
}

int seq_decompress_to_flat_64K_buf() {
    return _decompress_to_flat_64K_buf(ACC_RING_PUSH_SEQUENCER,
                                       queues::get_seq_comp_sq(0));
}

int compress_odd_size_buf() {
  cp_desc_t d;

  printf("Starting testcase %s\n", __func__);
  compress_cp_desc_template_fill(d, uncompressed_host_buf, compressed_host_buf,
                                 status_host_buf, compressed_host_buf, 567);
#ifdef ELBA
  d.doorbell_addr = tmp_db_addr;
  d.cmd_bits.doorbell_on = 1;
#endif
  if (run_cp_test(d, compressed_host_buf, status_host_buf) < 0) {
    printf("Testcase %s failed\n", __func__);
    return -1;
  }
  printf("Testcase %s passed\n", __func__);
  return 0;
}

int decompress_odd_size_buf() {
  cp_desc_t d;

  printf("Starting testcase %s\n", __func__);
  decompress_cp_desc_template_fill(d, compressed_host_buf, uncompressed_host_buf,
                                   status_host_buf, last_cp_output_data_len, 567);
#ifdef ELBA
  d.doorbell_addr = tmp_db_addr;
  d.cmd_bits.doorbell_on = 1;
#endif
  if (run_dc_test(d, status_host_buf, 567) < 0) {
    printf("Testcase %s failed\n", __func__);
    return -1;
  }
  // Verify data buf
  if (test_data_verify_and_dump(uncompressed_data,
                                uncompressed_host_buf->read_thru(),
                                567)) {
      printf("Testcase %s failed\n", __func__);
      return -1;
  }
  printf("Testcase %s passed\n", __func__);
  return 0;
}

int _compress_host_sgl_to_host_sgl(acc_ring_push_t push_type,
                                   uint32_t seq_comp_qid) {
  cp_desc_t d;

  // Prepare source SGLs
  host_sgl1->clear();
  host_sgl2->clear();
  cp_sgl_t *cp_sgl1 = (cp_sgl_t *)host_sgl1->read();
  cp_sgl_t *cp_sgl2 = (cp_sgl_t *)host_sgl2->read();

  cp_sgl1->len0 = cp_sgl1->len1 = cp_sgl1->len2 = 1000;
  cp_sgl1->addr0 = uncompressed_host_buf->pa() + 0;
  cp_sgl1->addr1 = uncompressed_host_buf->pa() + 1000;
  cp_sgl1->addr2 = uncompressed_host_buf->pa() + 2000;
  cp_sgl1->link = host_sgl2->pa();
  cp_sgl2->len0 = cp_sgl2->len1 = cp_sgl2->len2 = 1000;
  cp_sgl2->addr0 = uncompressed_host_buf->pa() + 3000;
  cp_sgl2->addr1 = uncompressed_host_buf->pa() + 4000;
  cp_sgl2->addr2 = uncompressed_host_buf->pa() + 5000;
  host_sgl1->write_thru();
  host_sgl2->write_thru();

  // Prepare destination SGLs
  host_sgl3->clear();
  host_sgl4->clear();
  cp_sgl_t *cp_sgl3 = (cp_sgl_t *)host_sgl3->read();
  cp_sgl_t *cp_sgl4 = (cp_sgl_t *)host_sgl4->read();
  cp_sgl3->len0 = cp_sgl3->len1 = cp_sgl3->len2 = 1024;
  cp_sgl3->addr0 = compressed_host_buf->pa() + 0;
  cp_sgl3->addr1 = compressed_host_buf->pa() + 1024;
  cp_sgl3->addr2 = compressed_host_buf->pa() + 2048;
  cp_sgl3->link = host_sgl4->pa();
  cp_sgl4->len0 = cp_sgl4->len1 = cp_sgl4->len2 = 1024;
  cp_sgl4->addr0 = compressed_host_buf->pa() + (3 * 1024);
  cp_sgl4->addr1 = compressed_host_buf->pa() + 4096;
  cp_sgl4->addr2 = compressed_host_buf->pa() + (5 * 1024);
  host_sgl3->write_thru();
  host_sgl4->write_thru();

  printf("Starting testcase %s push_type %d seq_comp_qid %u\n",
         __func__, push_type, seq_comp_qid);
  compress_cp_desc_template_fill(d, host_sgl1, host_sgl3, status_host_buf,
                                 compressed_host_buf, 6000);
#ifdef ELBA
  d.doorbell_addr = tmp_db_addr;
  d.cmd_bits.doorbell_on = 1;
#endif
  d.cmd_bits.src_is_list = 1;
  d.cmd_bits.dst_is_list = 1;
  if (run_cp_test(d, compressed_host_buf, status_host_buf,
                  push_type, seq_comp_qid) < 0) {
    printf("Testcase %s failed\n", __func__);
    return -1;
  }
  printf("Testcase %s passed\n", __func__);
  return 0;
}

int compress_host_sgl_to_host_sgl() {
    return _compress_host_sgl_to_host_sgl(ACC_RING_PUSH_HW_DIRECT, 0);
}

int seq_compress_host_sgl_to_host_sgl() {
    return _compress_host_sgl_to_host_sgl(ACC_RING_PUSH_SEQUENCER,
                                          queues::get_seq_comp_sq(0));
}

int _decompress_host_sgl_to_host_sgl(acc_ring_push_t push_type,
                                     uint32_t seq_comp_qid) {
  cp_desc_t d;

  // clear some initial area.
  uncompressed_host_buf->fragment_find(0, 64)->clear_thru();

  // Prepare source SGLs
  host_sgl1->clear();
  cp_sgl_t *cp_sgl1 = (cp_sgl_t *)host_sgl1->read();

  cp_sgl1->len0 = last_cp_output_data_len >> 1;
  cp_sgl1->len1 = last_cp_output_data_len - cp_sgl1->len0;
  cp_sgl1->addr0 = compressed_host_buf->pa() + 0;
  cp_sgl1->addr1 = compressed_host_buf->pa() + cp_sgl1->len0;
  host_sgl1->write_thru();

  // Prepare destination SGLs
  host_sgl3->clear();
  host_sgl4->clear();
  cp_sgl_t *cp_sgl3 = (cp_sgl_t *)host_sgl3->read();
  cp_sgl_t *cp_sgl4 = (cp_sgl_t *)host_sgl4->read();

  cp_sgl3->len0 = cp_sgl3->len1 = cp_sgl3->len2 = 1024;
  cp_sgl3->addr0 = uncompressed_host_buf->pa() + 0;
  cp_sgl3->addr1 = uncompressed_host_buf->pa() + 1024;
  cp_sgl3->addr2 = uncompressed_host_buf->pa() + 2048;
  cp_sgl3->link = host_sgl4->pa();
  cp_sgl4->len0 = cp_sgl4->len1 = cp_sgl4->len2 = 1024;
  cp_sgl4->addr0 = uncompressed_host_buf->pa() + (3 * 1024);
  cp_sgl4->addr1 = uncompressed_host_buf->pa() + 4096;
  cp_sgl4->addr2 = uncompressed_host_buf->pa() + (5 * 1024);
  host_sgl3->write_thru();
  host_sgl4->write_thru();

  printf("Starting testcase %s push_type %d seq_comp_qid %u\n",
         __func__, push_type, seq_comp_qid);
  decompress_cp_desc_template_fill(d, host_sgl1, host_sgl3, status_host_buf,
                                   last_cp_output_data_len, 6000);
#ifdef ELBA
  d.doorbell_addr = tmp_db_addr;
  d.cmd_bits.doorbell_on = 1;
#endif
  d.cmd_bits.src_is_list = 1;
  d.cmd_bits.dst_is_list = 1;
  if (run_dc_test(d, status_host_buf, 6000, push_type, seq_comp_qid) < 0) {
    printf("Testcase %s failed\n", __func__);
    return -1;
  }
  if (test_data_verify_and_dump(uncompressed_data,
                                uncompressed_host_buf->read_thru(),
                                6000)) {
      printf("Testcase %s failed\n", __func__);
      return -1;
  }
  printf("Testcase %s passed\n", __func__);
  return 0;
}

int decompress_host_sgl_to_host_sgl() {
    return _decompress_host_sgl_to_host_sgl(ACC_RING_PUSH_HW_DIRECT, 0);
}

int seq_decompress_host_sgl_to_host_sgl() {
    return _decompress_host_sgl_to_host_sgl(ACC_RING_PUSH_SEQUENCER,
                                            queues::get_seq_comp_sq(0));
}

int _compress_flat_64K_buf_in_hbm(acc_ring_push_t push_type,
                                  uint32_t seq_comp_qid) {
  cp_desc_t d;

  printf("Starting testcase %s push_type %d seq_comp_qid %u\n",
          __func__, push_type, seq_comp_qid);
  compress_cp_desc_template_fill(d, uncompressed_buf, compressed_buf,
                                 status_buf, compressed_buf, kUncompressedDataSize);
#ifdef ELBA
  d.doorbell_addr = tmp_db_addr;
  d.cmd_bits.doorbell_on = 1;
#endif
  if (run_cp_test(d, compressed_buf, status_buf, push_type, seq_comp_qid) < 0) {
    printf("Testcase %s failed\n", __func__);
    return -1;
  }
  printf("Testcase %s passed\n", __func__);
  return 0;
}

int compress_flat_64K_buf_in_hbm() {
    return _compress_flat_64K_buf_in_hbm(ACC_RING_PUSH_HW_DIRECT, 0);
}

int seq_compress_flat_64K_buf_in_hbm() {
    return _compress_flat_64K_buf_in_hbm(ACC_RING_PUSH_SEQUENCER,
                                         queues::get_seq_comp_sq(0));
}

int _decompress_to_flat_64K_buf_in_hbm(acc_ring_push_t push_type,
                                       uint32_t seq_comp_qid) {

  cp_desc_t d;

  // clear some initial area.
  uncompressed_buf->fragment_find(0, 64)->clear_thru();

  printf("Starting testcase %s push_type %d seq_comp_qid %u\n",
          __func__, push_type, seq_comp_qid);
  decompress_cp_desc_template_fill(d, compressed_buf, uncompressed_buf,
                                   status_buf, compressed_data_size,
                                   kUncompressedDataSize);
#ifdef ELBA
  d.doorbell_addr = tmp_db_addr;
  d.cmd_bits.doorbell_on = 1;
#endif
  if (run_dc_test(d, status_buf, 0, push_type, seq_comp_qid) < 0) {
    printf("Testcase %s failed\n", __func__);
    return -1;
  }
  printf("Testcase %s passed\n", __func__);
  return 0;
}

int decompress_to_flat_64K_buf_in_hbm() {
    return _decompress_to_flat_64K_buf_in_hbm(ACC_RING_PUSH_HW_DIRECT, 0);
}

int seq_decompress_to_flat_64K_buf_in_hbm() {
    return _decompress_to_flat_64K_buf_in_hbm(ACC_RING_PUSH_SEQUENCER,
                                              queues::get_seq_comp_sq(0));
}

// Route the compressed output through sequencer to handle output block
// boundry issues of compression engine.
int _compress_output_through_sequencer(acc_ring_push_t push_type,
                                       uint32_t seq_comp_qid) {
  cp_desc_t d;

  printf("Starting testcase %s push_type %d seq_comp_qid %u\n",
         __func__, push_type, seq_comp_qid);
  compress_cp_desc_template_fill(d, uncompressed_host_buf, compressed_buf,
                                 status_buf, compressed_buf,
                                 kUncompressedDataSize);
  //d.cmd_bits.doorbell_on = 1;  //already done below
  // Prepare an SGL PDMA for the sequencer to output data.
  compressed_host_buf->fragment_find(0, 64)->clear_thru();
  seq_sgl->clear();
  chain_sgl_pdma_t *ssgl = (chain_sgl_pdma_t *)seq_sgl->read();

  ssgl->tuple[0].addr = compressed_host_buf->pa();
  ssgl->tuple[0].len = 13199;
  ssgl->tuple[1].addr = compressed_host_buf->pa() +
                        ssgl->tuple[0].len;
  ssgl->tuple[1].len = 9537;
  ssgl->tuple[2].addr = compressed_host_buf->pa() +
                        ssgl->tuple[0].len + ssgl->tuple[1].len;
  ssgl->tuple[2].len = 10123;
  ssgl->tuple[3].addr = compressed_host_buf->pa() +
                        ssgl->tuple[0].len + ssgl->tuple[1].len +
                        ssgl->tuple[2].len;
  ssgl->tuple[3].len = kMaxMem2MemSize;
  seq_sgl->write_thru();

  chain_params_comp_t chain_params = {0};
  status_host_buf->clear_thru();
  chain_params.status_addr0 = status_buf->pa();
  chain_params.status_addr1 = status_host_buf->pa();
  chain_params.comp_buf_addr = d.dst;
  chain_params.aol_dst_vec_addr = seq_sgl->pa();
  chain_params.sgl_pdma_en = 1;
  chain_params.intr_addr = opaque_host_buf->pa();
  chain_params.intr_data = kCompSeqIntrData;

  // Clear the area where interrupt from sequencer is going to come.
  opaque_host_buf->clear_thru();
  chain_params.status_len = status_buf->line_size_get();
  chain_params.status_dma_en = 1;
  chain_params.intr_en = 1;
  chain_params.seq_spec.seq_status_q = queues::get_seq_comp_status_sq(0);
  if (seq_comp_status_desc_fill(chain_params) != 0) {
    printf("%s seq_comp_status_desc_fill failed\n", __FUNCTION__);
    return -1;
  }
  d.doorbell_addr = chain_params.seq_spec.ret_doorbell_addr;
  d.doorbell_data = chain_params.seq_spec.ret_doorbell_data;
  d.cmd_bits.doorbell_on = 1;

  // Verify(wait for) that the status makes it to HBM.
  // We could directly poll for sequencer but this is just additional verification.
  if (run_cp_test(d, compressed_buf, status_buf, push_type, seq_comp_qid) < 0) {
    printf("Testcase %s failed\n", __func__);
    return -1;
  }
  // Now poll for sequencer interrupt.
  auto seq_intr_poll_func = [] () -> int {
    uint32_t *p = (uint32_t *)opaque_host_buf->read_thru();
    if (*p == kCompSeqIntrData)
      return 0;
    return 1;
  };
  tests::Poller intr_poll;
  if (intr_poll(seq_intr_poll_func) != 0) {
    printf("ERROR: Interrupt from sequencer never came.\n");
  }
  if (test_data_verify_and_dump(compressed_data_buf,
                                compressed_host_buf->read_thru(),
                                last_cp_output_data_len)) {
      printf("Testcase %s failed\n", __func__);
      return -1;
  }
  // Status verification done.

  printf("Testcase %s passed\n", __func__);
  return 0;
}

int compress_output_through_sequencer() {
    return _compress_output_through_sequencer(ACC_RING_PUSH_HW_DIRECT, 0);
}

int seq_compress_output_through_sequencer() {
    return _compress_output_through_sequencer(ACC_RING_PUSH_SEQUENCER,
                                              queues::get_seq_comp_sq(0));
}

// Verify integrity of >64K buffer
int verify_integrity_for_gt64K() {
  cp_desc_t d;
  printf("Starting testcase %s\n", __func__);

  compress_cp_desc_template_fill(d, uncompressed_host_buf, compressed_host_buf,
                                 status_host_buf, nullptr, kCompAppMinSize);
#ifdef ELBA
  d.doorbell_addr = tmp_db_addr;
  d.cmd_bits.doorbell_on = 1;
#endif
  // Give 68K length
  d.extended_len = 1;
  d.status_data = 0x7654;
  if (run_cp_test(d, compressed_host_buf, status_host_buf) < 0) {
    printf("Testcase %s failed\n", __func__);
    return -1;
  }
  cp_status_sha512_t *st2 = (cp_status_sha512_t *)status_host_buf->read();
  uint64_t old_integiry = st2->integrity_data;

  // Now repeat the test using only 4K data i.e. higher 16 bits are zero.
  compress_cp_desc_template_fill(d, uncompressed_host_buf, compressed_host_buf,
                                 status_host_buf, nullptr, kCompAppMinSize);
#ifdef ELBA
  d.doorbell_addr = tmp_db_addr;
  d.cmd_bits.doorbell_on = 1;
#endif
  d.status_data = 0x5454;
  if (run_cp_test(d, compressed_host_buf, status_host_buf) < 0) {
    printf("Testcase %s failed\n", __func__);
    return -1;
  }
  st2 = (cp_status_sha512_t *)status_host_buf->read();
  if (st2->integrity_data == old_integiry) {
    printf("ERROR: Same integrity data for different data sizes\n");
    return -1;
  }

  printf("Testcase %s passed\n", __func__);
  return 0;
}

int _max_data_rate(acc_ring_push_t push_type,
                   uint32_t seq_comp_qid_cp,
                   uint32_t seq_comp_qid_dc)
{
  cp_desc_t         comp_cp_desc[MAX_CP_REQ];
  cp_desc_t         decomp_cp_desc[MAX_DC_REQ];
  cp_desc_t         *d;
  uint32_t          source_compressed_len;

  // Generate source compressed data for use by decompression below
  dp_mem_t *partial_buf = uncompressed_buf->fragment_find(0, kCompAppNominalSize);
  memcpy(partial_buf->read(), uncompressed_data, kCompAppNominalSize);
  partial_buf->write_thru();
  compress_cp_desc_template_fill(comp_cp_desc[0], partial_buf, compressed_buf,
                                 status_buf, compressed_buf, kCompAppNominalSize);
  #ifdef ELBA
    comp_cp_desc[0].doorbell_addr = tmp_db_addr;
    comp_cp_desc[0].cmd_bits.doorbell_on = 1;
  #endif
  if (run_cp_test(comp_cp_desc[0], compressed_buf, status_buf) < 0) {
    printf("%s source compressed data generation failed\n", __func__);
    return -1;
  }
  source_compressed_len = last_cp_output_data_len;

  printf("Starting testcase %s push_type %d seq_comp_qid_cp %u "
         "seq_comp_qid_dc %u\n", __func__, push_type,
         seq_comp_qid_cp, seq_comp_qid_dc);

  uint32_t i;
  dp_mem_t *max_cp_uncompressed_buf = new dp_mem_t(MAX_CP_REQ, kCompAppNominalSize,
                                                   DP_MEM_ALIGN_PAGE);
  dp_mem_t *max_cp_compressed_buf = new dp_mem_t(MAX_CP_REQ, kCompAppNominalSize,
                                                 DP_MEM_ALIGN_PAGE);
  dp_mem_t *max_cp_status_buf = new dp_mem_t(MAX_CP_REQ, CP_STATUS_PAD_ALIGNED_SIZE,
                                             DP_MEM_ALIGN_SPEC, DP_MEM_TYPE_HBM,
                                             kMinHostMemAllocSize);
  dp_mem_t *max_dc_uncompressed_buf = new dp_mem_t(MAX_DC_REQ, kCompAppNominalSize,
                                                   DP_MEM_ALIGN_PAGE);
  dp_mem_t *max_dc_status_buf = new dp_mem_t(MAX_DC_REQ, CP_STATUS_PAD_ALIGNED_SIZE,
                                             DP_MEM_ALIGN_SPEC, DP_MEM_TYPE_HBM,
                                             kMinHostMemAllocSize);
  // allocate opaque tags as a byte stream for easy memcmp
  dp_mem_t *max_cp_opaque_host_buf = new dp_mem_t(1, MAX_CP_REQ * sizeof(uint64_t),
                                                  DP_MEM_ALIGN_SPEC, DP_MEM_TYPE_HOST_MEM,
                                                  kMinHostMemAllocSize);
  dp_mem_t *exp_opaque_data_buf = new dp_mem_t(1, MAX_CP_REQ * sizeof(uint64_t),
                                               DP_MEM_ALIGN_SPEC, DP_MEM_TYPE_HOST_MEM,
                                               kMinHostMemAllocSize);
  dp_mem_t *max_dc_opaque_host_buf = new dp_mem_t(1, MAX_DC_REQ * sizeof(uint64_t),
                                                  DP_MEM_ALIGN_SPEC, DP_MEM_TYPE_HOST_MEM,
                                                  kMinHostMemAllocSize);
  uint32_t exp_opaque_data = 0xa5a5a5a5;

  // Note that RTL expects each opaque tag as uint64_t and writes
  // exp_opaque_data to the first 4 bytes, followed by 0 in the next 4 bytes
  for (i = 0; i < MAX_CP_REQ; i++) {
      memcpy(exp_opaque_data_buf->read() + (i * sizeof(uint64_t)),
             &exp_opaque_data, sizeof(uint32_t));
  }
  exp_opaque_data_buf->write_thru();

  // allocate and fill descriptors to load compression engine with requests
  for (i = 0, d = &comp_cp_desc[0];
       i < MAX_CP_REQ;
       i++, d++) {

    max_cp_uncompressed_buf->line_set(i);
    max_cp_compressed_buf->line_set(i);
    max_cp_status_buf->line_set(i);
    compress_cp_desc_template_fill(*d, max_cp_uncompressed_buf,
                                   max_cp_compressed_buf, max_cp_status_buf,
                                   nullptr, kCompAppNominalSize);
#ifdef ELBA
    d->doorbell_addr = tmp_db_addr;
    d->cmd_bits.doorbell_on = 1;
#endif
    d->status_data += i;
    d->cmd_bits.opaque_tag_on = 1;
    d->opaque_tag_addr = max_cp_opaque_host_buf->pa() + (i * sizeof(uint64_t));
    d->opaque_tag_data = exp_opaque_data;
    cp_ring->push((const void *)d, push_type, seq_comp_qid_cp);
  }

  // Ring the doorbell after loading decompression engine with requests
  for (i = 0, d = &decomp_cp_desc[0];
       i < MAX_DC_REQ;
       i++, d++) {

    // decompress from data obtained from sourced compressed data
    // created earlier
    max_dc_uncompressed_buf->line_set(i);
    max_dc_status_buf->line_set(i);
    decompress_cp_desc_template_fill(*d, compressed_buf, max_dc_uncompressed_buf,
                                     max_dc_status_buf, source_compressed_len,
                                     kCompAppNominalSize);
#ifdef ELBA
    d->doorbell_addr = tmp_db_addr;
    d->cmd_bits.doorbell_on = 1;
#endif
    d->status_data += i;
    d->cmd_bits.opaque_tag_on = 1;
    d->opaque_tag_addr = max_dc_opaque_host_buf->pa() + (i * sizeof(uint64_t));
    d->opaque_tag_data = exp_opaque_data;
    dc_ring->push((const void *)d, push_type, seq_comp_qid_dc);
  }

  // Now ring doorbells
  cp_ring->post_push();
  dc_ring->post_push();

  // Wait for all the interrupts
  auto func = [max_cp_opaque_host_buf, max_dc_opaque_host_buf,
               exp_opaque_data_buf,
               max_cp_status_buf, max_dc_status_buf,
               max_cp_compressed_buf,
               comp_cp_desc, decomp_cp_desc] () -> int {

      const cp_desc_t   *d;
      uint32_t          i;

      if (memcmp(max_cp_opaque_host_buf->read_thru(),
                 exp_opaque_data_buf->read_thru(),
                 max_cp_opaque_host_buf->line_size_get())) {
          return -1;
      }
      if (memcmp(max_dc_opaque_host_buf->read_thru(),
                 exp_opaque_data_buf->read_thru(),
                 max_dc_opaque_host_buf->line_size_get())) {
          return -1;
      }

      for (i = 0, d = &comp_cp_desc[0];
           i < MAX_CP_REQ;
           i++, d++) {

          max_cp_status_buf->line_set(i);
          max_cp_compressed_buf->line_set(i);
          if (compress_status_verify(max_cp_status_buf,
                                     max_cp_compressed_buf, *d, false)) {
              return -1;
          }
      }

      for (i = 0, d = &decomp_cp_desc[0];
           i < MAX_DC_REQ;
           i++, d++) {

          max_dc_status_buf->line_set(i);
          if (decompress_status_verify(max_dc_status_buf, *d,
                                       kCompAppNominalSize, false)) {
              return -1;
          }
      }

      return 0;
  };

  tests::Poller poll(FLAGS_long_poll_interval);
  if (poll(func) == 0) {
    printf("Testcase %s passed\n", __func__);

    // Only free buffers on successful completion; otherwise,
    // HW/P4+ might still be accessing the buffers

    delete max_cp_uncompressed_buf;
    delete max_cp_compressed_buf;
    delete max_cp_status_buf;
    delete max_dc_uncompressed_buf;
    delete max_dc_status_buf;
    delete max_cp_opaque_host_buf;
    delete exp_opaque_data_buf;
    delete max_dc_opaque_host_buf;

    return 0;
  }

  if (memcmp(max_cp_opaque_host_buf->read_thru(),
             exp_opaque_data_buf->read_thru(),
             max_cp_opaque_host_buf->line_size_get())) {
      printf("Compression requests did not complete\n");
  }
  if (memcmp(max_dc_opaque_host_buf->read_thru(),
             exp_opaque_data_buf->read_thru(),
             max_dc_opaque_host_buf->line_size_get())) {
      printf("Decompression requests did not complete\n");
  }
  printf("ERROR: Timed out waiting for all the commands to complete\n");
  return -1;


}

int max_data_rate() {
    return _max_data_rate(ACC_RING_PUSH_HW_DIRECT_BATCH, 0, 0);
}

int seq_max_data_rate() {
    return _max_data_rate(ACC_RING_PUSH_SEQUENCER_BATCH,
                          queues::get_seq_comp_sq(0),
                          queues::get_seq_comp_sq(1));
}

static int cp_dualq_flat_4K_buf(dp_mem_t *comp_buf,
                                dp_mem_t *uncomp_buf,
                                dp_mem_t *status_buf1,
                                dp_mem_t *status_buf2,
                                acc_ring_push_t push_type,
                                uint32_t seq_comp_qid_cp,
                                uint32_t seq_comp_qid_hotq) {
  cp_desc_t lq_desc;
  cp_desc_t hq_desc;

  // Setup compression descriptor for low prirority queue
  compress_cp_desc_template_fill(lq_desc, uncomp_buf, comp_buf,
                                 status_buf1, comp_buf, kCompAppMinSize);
#ifdef ELBA
  lq_desc.doorbell_addr = tmp_db_addr;
  lq_desc.cmd_bits.doorbell_on = 1;
#endif
  // Setup compression descriptor for high prirority queue
  dp_mem_t *hq_uncomp_buf = uncomp_buf->fragment_find(kCompAppMinSize,
                                                      kCompAppMinSize);
  dp_mem_t *hq_comp_buf = comp_buf->fragment_find(kCompAppMinSize,
                                                  kCompAppMinSize);
  compress_cp_desc_template_fill(hq_desc, hq_uncomp_buf, hq_comp_buf,
                                 status_buf2, hq_comp_buf, kCompAppMinSize);
#ifdef ELBA
  hq_desc.doorbell_addr = tmp_db_addr;
  hq_desc.cmd_bits.doorbell_on = 1;
#endif
  hq_desc.status_data = lq_desc.status_data * 2;

  // Initialize status for both the requests
  status_buf1->clear_thru();
  status_buf2->clear_thru();

  // Add descriptor for both high and low priority queues
  cp_ring->push((const void *)&lq_desc, push_type, seq_comp_qid_cp);

  // Dont ring the doorbell yet
  cp_hot_ring->push((const void *)&hq_desc, push_type, seq_comp_qid_hotq);

  // Now ring door bells for both high and low queues
  cp_ring->post_push();
  cp_hot_ring->post_push();

  // Check status update to both the descriptors
  auto func = [status_buf1, status_buf2,
               comp_buf, hq_comp_buf,
               lq_desc, hq_desc] () -> int {
    if (compress_status_verify(status_buf1, comp_buf, lq_desc, false)) {
      return -1;
    }

    if (compress_status_verify(status_buf2, hq_comp_buf, hq_desc, false)) {
      return -1;
    }

    return 0;
  };

  tests::Poller poll(FLAGS_long_poll_interval);
  if (poll(func) == 0) {
    printf("Testcase %s passed\n", __func__);
    return 0;
  }

  if (compress_status_verify(status_buf1, comp_buf, lq_desc)) {
    printf("Compression request in low queue verification failed\n");
  }

  if (compress_status_verify(status_buf2, hq_comp_buf, hq_desc)) {
    printf("Compression request in high/hot queue verification failed\n");
  }

  return -1;
}

int compress_dualq_flat_4K_buf() {
  printf("Starting testcase %s\n", __func__);

  int rc = cp_dualq_flat_4K_buf(compressed_host_buf, uncompressed_host_buf,
                                status_host_buf, status_host_buf2,
                                ACC_RING_PUSH_HW_DIRECT_BATCH, 0, 0);
  if (rc == 0)
    printf("Testcase %s passed\n", __func__);
  else
    printf("Testcase %s failed\n", __func__);

  return rc;
}

int seq_compress_dualq_flat_4K_buf() {
  printf("Starting testcase %s\n", __func__);

  int rc = cp_dualq_flat_4K_buf(compressed_host_buf, uncompressed_host_buf,
                                status_host_buf, status_host_buf2,
                                ACC_RING_PUSH_SEQUENCER_BATCH,
                                queues::get_seq_comp_sq(0),
                                queues::get_seq_comp_sq(1));
  if (rc == 0)
    printf("Testcase %s passed\n", __func__);
  else
    printf("Testcase %s failed\n", __func__);

  return rc;
}

int compress_dualq_flat_4K_buf_in_hbm() {
  printf("Starting testcase %s\n", __func__);

  int rc = cp_dualq_flat_4K_buf(compressed_buf, uncompressed_buf,
                                status_buf, status_buf2,
                                ACC_RING_PUSH_HW_DIRECT_BATCH, 0, 0);
  if (rc == 0)
    printf("Testcase %s passed\n", __func__);
  else
    printf("Testcase %s failed\n", __func__);

  return rc;
}

int seq_compress_dualq_flat_4K_buf_in_hbm() {
  printf("Starting testcase %s\n", __func__);

  int rc = cp_dualq_flat_4K_buf(compressed_buf, uncompressed_buf,
                                status_buf, status_buf2,
                                ACC_RING_PUSH_SEQUENCER_BATCH,
                                queues::get_seq_comp_sq(0),
                                queues::get_seq_comp_sq(1));
  if (rc == 0)
    printf("Testcase %s passed\n", __func__);
  else
    printf("Testcase %s failed\n", __func__);

  return rc;
}

// Accelerator compression to XTS-encrypt chaining DOLs.
int compress_output_encrypt_app_min_size() {
    comp_encrypt_chain_push_params_t    params;
    comp_encrypt_chain->push(params.app_blk_size(kCompAppMinSize).
                                    comp_ring(cp_ring).
                                    push_type(ACC_RING_PUSH_HW_DIRECT).
                                    seq_comp_qid(0).
                                    seq_comp_status_qid(queues::get_seq_comp_status_sq(0)).
                                    seq_xts_status_qid(queues::get_seq_xts_status_sq(0)));
    comp_encrypt_chain->post_push();
    return comp_encrypt_chain->full_verify();
}

int seq_compress_output_encrypt_app_min_size() {
    comp_encrypt_chain_push_params_t    params;
    comp_encrypt_chain->push(params.app_blk_size(kCompAppMinSize).
                                    comp_ring(cp_ring).
                                    push_type(ACC_RING_PUSH_SEQUENCER).
                                    seq_comp_qid(queues::get_seq_comp_sq(0)).
                                    seq_comp_status_qid(queues::get_seq_comp_status_sq(0)).
                                    seq_xts_status_qid(queues::get_seq_xts_status_sq(0)));
    comp_encrypt_chain->post_push();
    return comp_encrypt_chain->full_verify();
}

int compress_output_encrypt_app_max_size() {
    comp_encrypt_chain_push_params_t    params;
    comp_encrypt_chain->push(params.app_blk_size(kCompAppMaxSize).
                                    comp_ring(cp_ring).
                                    push_type(ACC_RING_PUSH_HW_DIRECT).
                                    seq_comp_qid(0).
                                    seq_comp_status_qid(queues::get_seq_comp_status_sq(0)).
                                    seq_xts_status_qid(queues::get_seq_xts_status_sq(0)));
    comp_encrypt_chain->post_push();
    return comp_encrypt_chain->full_verify();
}

int seq_compress_output_encrypt_app_max_size() {
    comp_encrypt_chain_push_params_t    params;
    comp_encrypt_chain->push(params.enc_dec_blk_type(XTS_ENC_DEC_PER_HASH_BLK).
                                    app_blk_size(kCompAppMaxSize).
                                    comp_ring(cp_ring).
                                    push_type(ACC_RING_PUSH_SEQUENCER).
                                    seq_comp_qid(queues::get_seq_comp_sq(0)).
                                    seq_comp_status_qid(queues::get_seq_comp_status_sq(0)).
                                    seq_xts_status_qid(queues::get_seq_xts_status_sq(0)));
    comp_encrypt_chain->post_push();
    return comp_encrypt_chain->full_verify();
}

int compress_output_encrypt_app_nominal_size() {
    comp_encrypt_chain_push_params_t    params;
    comp_encrypt_chain->push(params.app_blk_size(kCompAppNominalSize).
                                    comp_ring(cp_ring).
                                    push_type(ACC_RING_PUSH_HW_DIRECT).
                                    seq_comp_qid(0).
                                    seq_comp_status_qid(queues::get_seq_comp_status_sq(0)).
                                    seq_xts_status_qid(queues::get_seq_xts_status_sq(0)));
    comp_encrypt_chain->post_push();
    return comp_encrypt_chain->full_verify();
}

int seq_compress_output_encrypt_app_nominal_size() {
    comp_encrypt_chain_push_params_t    params;
    comp_encrypt_chain->push(params.enc_dec_blk_type(XTS_ENC_DEC_PER_HASH_BLK).
                                    app_blk_size(kCompAppNominalSize).
                                    comp_ring(cp_ring).
                                    push_type(ACC_RING_PUSH_SEQUENCER).
                                    seq_comp_qid(queues::get_seq_comp_sq(0)).
                                    seq_comp_status_qid(queues::get_seq_comp_status_sq(0)).
                                    seq_xts_status_qid(queues::get_seq_xts_status_sq(0)));
    comp_encrypt_chain->post_push();
    return comp_encrypt_chain->full_verify();
}

int seq_compress_output_encrypt_app_test_size() {
    comp_encrypt_chain_push_params_t    params;
    comp_encrypt_chain->push(params.enc_dec_blk_type(XTS_ENC_DEC_PER_HASH_BLK).
                                    app_blk_size(kCompAppTestSize).
                                    comp_ring(cp_ring).
                                    push_type(ACC_RING_PUSH_SEQUENCER).
                                    seq_comp_qid(queues::get_seq_comp_sq(0)).
                                    seq_comp_status_qid(queues::get_seq_comp_status_sq(0)).
                                    seq_xts_status_qid(queues::get_seq_xts_status_sq(0)));
    comp_encrypt_chain->post_push();
    return comp_encrypt_chain->full_verify();
}

int seq_compress_output_encrypt_force_comp_buf2_bypass() {
    comp_encrypt_chain_push_params_t    params;
    comp_encrypt_chain->push(params.enc_dec_blk_type(XTS_ENC_DEC_PER_HASH_BLK).
                                    app_blk_size(kCompAppTestSize).
                                    comp_ring(cp_ring).
                                    push_type(ACC_RING_PUSH_SEQUENCER).
                                    seq_comp_qid(queues::get_seq_comp_sq(0)).
                                    seq_comp_status_qid(queues::get_seq_comp_status_sq(0)).
                                    seq_xts_status_qid(queues::get_seq_xts_status_sq(0)).
                                    force_comp_buf2_bypass(true));
    comp_encrypt_chain->post_push();
    return comp_encrypt_chain->full_verify();
}

int seq_compress_output_encrypt_force_uncomp_encrypt() {
    comp_encrypt_chain_push_params_t    params;
    comp_encrypt_chain->push(params.enc_dec_blk_type(XTS_ENC_DEC_PER_HASH_BLK).
                                    app_blk_size(kCompAppTestSize).
                                    comp_ring(cp_ring).
                                    push_type(ACC_RING_PUSH_SEQUENCER).
                                    seq_comp_qid(queues::get_seq_comp_sq(0)).
                                    seq_comp_status_qid(queues::get_seq_comp_status_sq(0)).
                                    seq_xts_status_qid(queues::get_seq_xts_status_sq(0)).
                                    force_uncomp_encrypt(true));
    comp_encrypt_chain->post_push();
    return comp_encrypt_chain->full_verify();
}

int _compress_clear_insert_header(acc_ring_push_t push_type,
                                  uint32_t seq_comp_qid) {
  cp_desc_t d;

  printf("Starting testcase %s push_type %d seq_comp_qid %u\n",
          __func__, push_type, seq_comp_qid);

  // flat 64K buf in hbm
  compress_cp_desc_template_fill(d, uncompressed_buf, compressed_buf,
                                 status_buf, compressed_buf,
                                 kUncompressedDataSize);
#ifdef ELBA
  d.doorbell_addr = tmp_db_addr;
  d.cmd_bits.doorbell_on = 1;
#endif
  // reset the bit to not to insert the header
  d.cmd_bits.insert_header = 0;

  if (run_cp_test(d, compressed_buf, status_buf, push_type, seq_comp_qid) < 0) {
    printf("Testcase %s failed\n", __func__);
    return -1;
  }
  printf("Testcase %s passed\n", __func__);
  return 0;
}

int compress_clear_insert_header() {
    return _compress_clear_insert_header(ACC_RING_PUSH_HW_DIRECT, 0);
}

int _decompress_clear_header_present(acc_ring_push_t push_type,
                                       int32_t seq_comp_qid) {

  cp_desc_t d;

  // clear some initial area.
  uncompressed_buf->fragment_find(0, 64)->clear_thru();

  printf("Starting testcase %s push_type %d seq_comp_qid %u\n",
          __func__, push_type, seq_comp_qid);

  // flat 64K buf in hbm
  decompress_cp_desc_template_fill(d, compressed_buf, uncompressed_buf,
                                   status_buf, last_cp_output_data_len,
                                   kUncompressedDataSize);
#ifdef ELBA
  d.doorbell_addr = tmp_db_addr;
  d.cmd_bits.doorbell_on = 1;
#endif
  // reset the bits to invalidate the verification against header
  d.cmd_bits.header_present = 0;
  d.cmd_bits.cksum_verify_en = 0;

  if (run_dc_test(d, status_buf, 0, push_type, seq_comp_qid) < 0) {
    printf("Testcase %s failed\n", __func__);
    return -1;
  }

  // Verify data buf
  if (test_data_verify_and_dump(uncompressed_data,
                                uncompressed_buf->read_thru(),
                                kUncompressedDataSize)) {
      printf("Testcase %s failed\n", __func__);
      return -1;
  }

  printf("Testcase %s passed\n", __func__);
  return 0;
}

int decompress_clear_header_present() {
    return _decompress_clear_header_present(ACC_RING_PUSH_HW_DIRECT, 0);
}

// Accelerator XTS-decrypt to decompression chaining DOLs.
int seq_decrypt_output_decompress_len_update_none() {

    // Execute decrypt-decompression on the last compress-pad-encrypted block,
    // i.e., the block size is whatever was last compressed and padded.
    //
    // Execute the operations with no decomp_len_update.
    //
    // Use the xts_ctx default mode of XTS sequencer queue, with chaining
    // to decompression initiated from P4+ handling of XTS status sequencer.
    decrypt_decomp_chain_push_params_t  params;
    decrypt_decomp_chain->push(params.comp_encrypt_chain(comp_encrypt_chain).
                                      decomp_ring(dc_ring).
                                      seq_xts_qid(queues::get_seq_xts_sq(0)).
                                      seq_xts_status_qid(queues::get_seq_xts_status_sq(0)).
                                      seq_comp_status_qid(queues::get_seq_comp_status_sq(0)).
                                      decrypt_decomp_len_update(DECRYPT_DECOMP_LEN_UPDATE_NONE));
    decrypt_decomp_chain->post_push();
    return decrypt_decomp_chain->full_verify();
}

int seq_decrypt_output_decompress_len_update_flat() {

    // Execute decrypt-decompression with DECRYPT_DECOMP_LEN_UPDATE_FLAT.
    decrypt_decomp_chain_push_params_t  params;
    decrypt_decomp_chain->push(params.comp_encrypt_chain(comp_encrypt_chain).
                                      decomp_ring(dc_ring).
                                      seq_xts_qid(queues::get_seq_xts_sq(0)).
                                      seq_xts_status_qid(queues::get_seq_xts_status_sq(0)).
                                      seq_comp_status_qid(queues::get_seq_comp_status_sq(0)).
                                      decrypt_decomp_len_update(DECRYPT_DECOMP_LEN_UPDATE_FLAT));
    decrypt_decomp_chain->post_push();
    return decrypt_decomp_chain->full_verify();
}

int seq_decrypt_output_decompress_len_update_sgl_src() {

    // Execute decrypt-decompression with DECRYPT_DECOMP_LEN_UPDATE_SGL_SRC.
    decrypt_decomp_chain_push_params_t  params;
    decrypt_decomp_chain->push(params.comp_encrypt_chain(comp_encrypt_chain).
                                      decomp_ring(dc_ring).
                                      seq_xts_qid(queues::get_seq_xts_sq(0)).
                                      seq_xts_status_qid(queues::get_seq_xts_status_sq(0)).
                                      seq_comp_status_qid(queues::get_seq_comp_status_sq(0)).
                                      decrypt_decomp_len_update(DECRYPT_DECOMP_LEN_UPDATE_SGL_SRC));
    decrypt_decomp_chain->post_push();
    return decrypt_decomp_chain->full_verify();
}

int seq_decrypt_output_decompress_len_update_sgl_src_vec() {

    // Execute decrypt-decompression with DECRYPT_DECOMP_LEN_UPDATE_SGL_SRC_VEC.
    decrypt_decomp_chain_push_params_t  params;
    decrypt_decomp_chain->push(params.comp_encrypt_chain(comp_encrypt_chain).
                                      decomp_ring(dc_ring).
                                      seq_xts_qid(queues::get_seq_xts_sq(0)).
                                      seq_xts_status_qid(queues::get_seq_xts_status_sq(0)).
                                      seq_comp_status_qid(queues::get_seq_comp_status_sq(0)).
                                      decrypt_decomp_len_update(DECRYPT_DECOMP_LEN_UPDATE_SGL_SRC_VEC));
    decrypt_decomp_chain->post_push();
    return decrypt_decomp_chain->full_verify();
}

int seq_compress_output_hash_app_max_size() {
    comp_hash_chain_push_params_t   params;

    // Compression and hash both using cp_ring
    comp_hash_chain->push(params.app_blk_size(kCompAppMaxSize).
                                 app_hash_size(kCompAppHashBlkSize).
                                 integrity_type(COMP_INTEGRITY_M_ADLER32).
                                 comp_ring(cp_ring).
                                 hash_ring(cp_ring).
                                 push_type(ACC_RING_PUSH_SEQUENCER).
                                 seq_comp_qid(queues::get_seq_comp_sq(0)).
                                 seq_comp_status_qid(queues::get_seq_comp_status_sq(0)));
    comp_hash_chain->post_push();
    return comp_hash_chain->full_verify();
}

int seq_compress_output_hash_app_test_size() {
    comp_hash_chain_push_params_t   params;

    // Note: cp_ring being used for compression and cp_hot_ring for hashing
    comp_hash_chain->push(params.app_blk_size(kCompAppTestSize).
                                 app_hash_size(kCompAppHashBlkSize).
                                 comp_ring(cp_ring).
                                 hash_ring(cp_hot_ring).
                                 push_type(ACC_RING_PUSH_SEQUENCER).
                                 seq_comp_qid(queues::get_seq_comp_sq(0)).
                                 seq_comp_status_qid(queues::get_seq_comp_status_sq(0)));
    comp_hash_chain->post_push();
    return comp_hash_chain->full_verify();
}

int seq_compress_output_hash_app_nominal_size() {
    comp_hash_chain_push_params_t   params;

    // Note: cp_hot_ring being used for compression and cp_ring for hashing
    comp_hash_chain->push(params.app_blk_size(kCompAppNominalSize).
                                 app_hash_size(kCompAppHashBlkSize).
                                 integrity_type(COMP_INTEGRITY_M_ADLER32).
                                 comp_ring(cp_hot_ring).
                                 hash_ring(cp_ring).
                                 push_type(ACC_RING_PUSH_SEQUENCER).
                                 seq_comp_qid(queues::get_seq_comp_sq(0)).
                                 seq_comp_status_qid(queues::get_seq_comp_status_sq(0)));
    comp_hash_chain->post_push();
    return comp_hash_chain->full_verify();
}

// Accelerator checksum to decompression chaining DOLs.
int seq_chksum_decompress_last_app_blk() {

    // Execute checksum-decompression on the last compress-hash block,
    // i.e., the block size is whatever was last compressed.
    chksum_decomp_chain_push_params_t  params;
    chksum_decomp_chain->push(params.comp_hash_chain(comp_hash_chain).
                                     chksum_ring(dc_hot_ring).
                                     decomp_ring(dc_ring).
                                     push_type(ACC_RING_PUSH_SEQUENCER).
                                     seq_chksum_qid(queues::get_seq_comp_sq(0)).
                                     seq_decomp_qid(queues::get_seq_comp_sq(1)));
    chksum_decomp_chain->post_push();
    return chksum_decomp_chain->full_verify();
}

}  // namespace tests
