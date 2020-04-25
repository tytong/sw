// {C} Copyright 2020 Pensando Systems Inc. All rights reserved

#include <map>
#include "asic/cmn/asic_common.hpp"
#include "platform/elba/elba_common.hpp"
#include "lib/p4/p4_api.hpp"
#include "lib/pal/pal.hpp"
#include "platform/pal/include/pal.h"
#include "platform/elba/elba_tbl_rw.hpp"
#include "platform/elba/csrint/csr_init.hpp"
#include "platform/elba/elba_hbm_rw.hpp"
#include "platform/elba/elba_tm_rw.hpp"
#include "platform/elba/elba_txs_scheduler.hpp"
#include "third-party/asic/elba/model/utils/elb_csr_py_if.h"
#include "asic/rw/asicrw.hpp"
#include "asic/asic.hpp"

#include "third-party/asic/elba/model/utils/elb_blk_reg_model.h"
#include "third-party/asic/elba/model/elb_top/elb_top_csr.h"
#include "third-party/asic/elba/model/elb_pic/elb_pict_csr.h"
#include "third-party/asic/elba/model/elb_pic/elb_pics_csr.h"
#include "third-party/asic/elba/model/elb_te/elb_te_csr.h"
#include "third-party/asic/ip/verif/pcpp/cpp_int_helper.h"
#include "third-party/asic/elba/verif/apis/elb_pics_sw_api.h"
#include "third-party/asic/elba/verif/apis/elb_pict_sw_api.h"
#include "third-party/asic/elba/model/elb_top/csr_defines/elb_pics_c_hdr.h"
#include "third-party/asic/elba/design/common/gen/elb_pf_decoders.h"

extern pen_csr_base *get_csr_base_from_path(string);

using namespace std;

namespace sdk {
namespace platform {
namespace elba {

#define ELBA_CALLOC  calloc
#define ELBA_FREE    free

/* Cache invalidate register address and memory mapped addresses */
#define CSR_CACHE_INVAL_INGRESS_REG_ADDR 0x61701000
#define CSR_CACHE_INVAL_EGRESS_REG_ADDR  0x60f01000
#define CSR_CACHE_INVAL_TXDMA_REG_ADDR   0x12701000
#define CSR_CACHE_INVAL_RXDMA_REG_ADDR   0x60701000
#define CSR_CACHE_INVAL_XDMA_REG_ADDR    0x12a01000
#define CSR_CACHE_INVAL_PXB_REG_ADDR     0x200c7000

static uint32_t *csr_cache_inval_ingress_va;
static uint32_t *csr_cache_inval_egress_va;
static uint32_t *csr_cache_inval_txdma_va;
static uint32_t *csr_cache_inval_rxdma_va;

typedef int elba_error_t;

//  Design decisions + Table Update Flow:
//
//  Maintain Shadow memory for SRAM and TCAM Units.
//
//   1.  Shadow memory maintained in ARM memory will be mirror representation
//       of how elba TCAM/SRAM addressing is done. This implies
//       uint8_t ram_row[10 blocks * 16 bytes]. 4k such rows will be created.
//
//   2.  To start with there will be single lock to protect updates to shadow
//       memory (if HAL updates can come from multiple threads)
//       To improve update performance and avoid lock contention, 4K rows
//       can be divided into zones and one lock per zone can be maintained.
//
//   3. Using table property API provided by p4pd_api, start block, start
//      row, and number of buckets (number of table entries within
//      a single row) are obtained.
//
//   4. Using index (at which table entry need to be written to or read from)
//      row#, one or more blocks#, and start word# in first block,
//      end word# in last block are computed.
//
//   5. Read + modify of the relevant blocks and push those blocks to Capri
//
//

//
//
//
//  ELBA SRAM ADDRESSING:
//
//       +=================================================================+
// Row0  | Block0 (128bits) | Block1 (128b) | ...........| Block9 (128b)   |
//       +-----------------------------------------------------------------+
// Row1  | Block0 (128bits) | Block1 (128b) | ...........| Block9 (128b)   |
//       +-----------------------------------------------------------------+
// Row2  | Block0 (128bits) | Block1 (128b) | ...........| Block9 (128b)   |
//       +-----------------------------------------------------------------+
//
//                          :
//                          :
//       +-----------------------------------------------------------------+
// Row   | Block0 (128bits) | Block1 (128b) | ...........| Block9 (128b)   |
// 4K-1  +=================================================================+
//
//
//     1. Any memory writes / reads are done in units of block. To update
//        a table entry that is within one or more blocks, all such memory
//        blocks on a particular Row will need to modified and written back
//        to elba.
//
//     2. Table entry start and end on 16b boundary. Multiple such 16b words
//        are updated or read from when performing table write or read.
//
//

#define ELBA_SRAM_BLOCK_COUNT      (8)
#define ELBA_SRAM_BLOCK_WIDTH      (128) // bits
#define ELBA_SRAM_WORD_WIDTH       (16)  // bits; is also unit of allocation.
#define ELBA_SRAM_WORDS_PER_BLOCK  (8)
#define ELBA_SRAM_ROWS             (0x1000) // 4K

#define ELBA_TCAM_BLOCK_COUNT      (8)
#define ELBA_TCAM_BLOCK_WIDTH      (128) // bits
#define ELBA_TCAM_WORD_WIDTH       (16)  // bits; is also unit of allocation.
#define ELBA_TCAM_WORDS_PER_BLOCK  (8)
#define ELBA_TCAM_ROWS             (0x400) // 1K

typedef struct elba_sram_shadow_mem_ {
    uint8_t zones;          // Using entire memory as one zone.
                            // TBD: carve into multiple zones
                            // to reduce access/update contention

    //pthread_mutex_t mutex; // TBD: when its decided to make HAL thread safe

    // Since writes/read access to SRAM are in done in units of block
    // a three dim array is maintained
    // Since word width is 16bits, uint16_t is used. A table entry starts at 16b
    // boundary
    uint16_t mem[ELBA_SRAM_ROWS][ELBA_SRAM_BLOCK_COUNT][ELBA_SRAM_WORDS_PER_BLOCK];

} elba_sram_shadow_mem_t;

typedef struct elba_tcam_shadow_mem_ {
    uint8_t zones;          // Using entire memory as one zone.
                            // TBD: carve into multiple zones
                            // to reduce access/update contention

    //pthread_mutex_t mutex; // TBD: when its decided to make HAL thread safe
    uint16_t mem_x[ELBA_TCAM_ROWS][ELBA_TCAM_BLOCK_COUNT][ELBA_TCAM_WORDS_PER_BLOCK];
    uint16_t mem_y[ELBA_TCAM_ROWS][ELBA_TCAM_BLOCK_COUNT][ELBA_TCAM_WORDS_PER_BLOCK];

} elba_tcam_shadow_mem_t;


static elba_sram_shadow_mem_t *g_shadow_sram_p4[2];
static elba_tcam_shadow_mem_t *g_shadow_tcam_p4[2];
static elba_sram_shadow_mem_t *g_shadow_sram_rxdma;
static elba_sram_shadow_mem_t *g_shadow_sram_txdma;

static elba_sram_shadow_mem_t *
get_sram_shadow_for_table (uint32_t tableid, int gress)
{
    if ((tableid >= p4pd_tableid_min_get()) &&
        (tableid <= p4pd_tableid_max_get())) {
        return (g_shadow_sram_p4[gress]);
    } else if ((tableid >= p4pd_rxdma_tableid_min_get()) &&
               (tableid <= p4pd_rxdma_tableid_max_get())) {
        return (g_shadow_sram_rxdma);
    } else if ((tableid >= p4pd_txdma_tableid_min_get()) &&
               (tableid <= p4pd_txdma_tableid_max_get())) {
        return (g_shadow_sram_txdma);
    } else {
        SDK_ASSERT(0);
    }
    return NULL;
}

// HBM base address in System memory map; Cached once at the init time
static uint64_t hbm_mem_base_addr;

// Store action pc for every action of the table.
static uint64_t elba_action_asm_base[P4TBL_ID_MAX][P4TBL_MAX_ACTIONS];
static uint64_t elba_action_rxdma_asm_base[P4TBL_ID_MAX][P4TBL_MAX_ACTIONS];
static uint64_t elba_action_txdma_asm_base[P4TBL_ID_MAX][P4TBL_MAX_ACTIONS];
static uint64_t elba_table_rxdma_asm_base[P4TBL_ID_MAX];
static uint64_t elba_table_txdma_asm_base[P4TBL_ID_MAX];

typedef enum elba_tbl_rw_logging_levels_ {
    ELB_TBL_RW_LOG_LEVEL_ALL = 0,
    ELB_TBL_RW_LOG_LEVEL_INFO,
    ELB_TBL_RW_LOG_LEVEL_ERROR,
} elba_tbl_rw_logging_levels;

void
elba_program_table_mpu_pc (int tableid, bool ingress, int stage,
                           int stage_tableid,
                           uint64_t elba_table_asm_err_offset,
                           uint64_t elba_table_asm_base)
{
    // Program table base address into elba TE
    elb_top_csr_t & elb0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    assert(stage_tableid < 16);
    SDK_TRACE_DEBUG("====Stage: %d Tbl_id: %u, Stg_Tbl_id %u, Tbl base: 0x%lx Err pc: 0x%lx==",
                    stage, tableid, stage_tableid,
                    elba_table_asm_base, elba_table_asm_err_offset);
    if (ingress) {
        elb_te_csr_t &te_csr = elb0.sgi.te[stage];
        // Push to HW/Capri from entry_start_block to block
        te_csr.cfg_table_property[stage_tableid].read();
        te_csr.cfg_table_property[stage_tableid]
                .mpu_pc(((elba_table_asm_base) >> 6));
        te_csr.cfg_table_property[stage_tableid].write();
    } else {
        elb_te_csr_t &te_csr = elb0.sge.te[stage];
        // Push to HW/Capri from entry_start_block to block
        te_csr.cfg_table_property[stage_tableid].read();
        te_csr.cfg_table_property[stage_tableid]
                .mpu_pc(((elba_table_asm_base) >> 6));
        te_csr.cfg_table_property[stage_tableid].write();
    }
}

void
elba_program_hbm_table_base_addr (int tableid, int stage_tableid,
                                  char *tablename, int stage, int pipe)
{
    mem_addr_t start_offset;

#ifdef MEM_REGION_RSS_INDIR_TABLE_NAME
    if (strcmp(tablename, MEM_REGION_RSS_INDIR_TABLE_NAME) == 0) {
        return;
    }
#endif

    sdk_ret_t ret;

    ret = sdk::asic::pd::asicpd_set_hbm_table_base_addr(tableid, stage_tableid,
                                                        tablename,
                                                        stage, pipe);
    if (ret != SDK_RET_OK) {
        return;
    }

    assert(stage_tableid < 16);
    start_offset = get_mem_addr(tablename);
    SDK_TRACE_DEBUG("===HBM Tbl Name: %s, Stage: %d, StageTblID: %u, "
                    "Addr: 0x%lx}===",
                    tablename, stage, stage_tableid, start_offset);
    if (start_offset == INVALID_MEM_ADDRESS) {
        return;
    }

    // Program table base address into elba TE
    elb_top_csr_t & elb0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    if (pipe == P4_PIPELINE_INGRESS) {
        elb_te_csr_t &te_csr = elb0.sgi.te[stage];
        te_csr.cfg_table_property[stage_tableid].read();
        te_csr.cfg_table_property[stage_tableid].addr_base(start_offset);
        te_csr.cfg_table_property[stage_tableid].write();
    } else if (pipe == P4_PIPELINE_EGRESS) {
        elb_te_csr_t &te_csr = elb0.sge.te[stage];
        te_csr.cfg_table_property[stage_tableid].read();
        te_csr.cfg_table_property[stage_tableid].addr_base(start_offset);
        te_csr.cfg_table_property[stage_tableid].write();
    } else if (pipe == P4_PIPELINE_RXDMA) {
        elb_te_csr_t &te_csr = elb0.pcr.te[stage];
        te_csr.cfg_table_property[stage_tableid].read();
        te_csr.cfg_table_property[stage_tableid].addr_base(start_offset);
        te_csr.cfg_table_property[stage_tableid].write();
    } else if (pipe == P4_PIPELINE_TXDMA) {
        elb_te_csr_t &te_csr = elb0.pct.te[stage];
        te_csr.cfg_table_property[stage_tableid].read();
        te_csr.cfg_table_property[stage_tableid].addr_base(start_offset);
        te_csr.cfg_table_property[stage_tableid].write();
    } else {
        SDK_ASSERT(0);
    }
}

#define ELBA_P4PLUS_RX_STAGE0_QSTATE_OFFSET_0            0
#define ELBA_P4PLUS_RX_STAGE0_QSTATE_OFFSET_64           64

void
elba_program_p4plus_table_mpu_pc(int tableid, int stage_tbl_id, int stage)
{
    uint64_t pc = 0;
    elb_te_csr_t *te_csr = NULL;
    bool pipe_rxdma = false;

    elb_top_csr_t & elb0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);

    if ((uint32_t)tableid >= p4pd_rxdma_tableid_min_get() &&
        (uint32_t)tableid < p4pd_rxdma_tableid_max_get()) {
        uint32_t lcl_tableid = tableid - p4pd_rxdma_tableid_min_get();
        te_csr = &elb0.pcr.te[stage];
        pc = elba_table_rxdma_asm_base[lcl_tableid];
        pipe_rxdma = true;
    } else if ((uint32_t)tableid >= p4pd_txdma_tableid_min_get() &&
               (uint32_t)tableid < p4pd_txdma_tableid_max_get()) {
        uint32_t lcl_tableid = tableid - p4pd_txdma_tableid_min_get();
        te_csr = &elb0.pct.te[stage];
        pc = elba_table_txdma_asm_base[lcl_tableid];
        pipe_rxdma = false;
    }
    if (pc == 0) {
        return;
    }
    SDK_TRACE_DEBUG("Pipe %s, Stage %d, Tbl_id %u, Stg_Tbl_id %u, "
                    "Tbl base 0x%lx", ((pipe_rxdma) ? "RxDMA" : "TxDMA"),
                    stage, tableid, stage_tbl_id, pc);
    te_csr->cfg_table_property[stage_tbl_id].read();
    te_csr->cfg_table_property[stage_tbl_id].mpu_pc(pc >> 6);
    te_csr->cfg_table_property[stage_tbl_id].write();
}
uint64_t
elba_get_p4plus_table_mpu_pc(int tableid)
{
    uint32_t lcl_tableid;

    if ((uint32_t)tableid >= p4pd_rxdma_tableid_min_get() &&
        (uint32_t)tableid < p4pd_rxdma_tableid_max_get()) {
        lcl_tableid = tableid - p4pd_rxdma_tableid_min_get();
        return elba_table_rxdma_asm_base[lcl_tableid];
    } else if ((uint32_t)tableid >= p4pd_txdma_tableid_min_get() &&
               (uint32_t)tableid < p4pd_txdma_tableid_max_get()) {
        lcl_tableid = tableid - p4pd_txdma_tableid_min_get();
        return elba_table_txdma_asm_base[lcl_tableid];
    }
    return 0;
}

static void
elba_program_p4plus_table_mpu_pc_args (int tbl_id, elb_te_csr_t *te_csr,
                                       uint64_t pc, uint32_t offset)
{
    te_csr->cfg_table_property[tbl_id].read();
    te_csr->cfg_table_property[tbl_id].mpu_pc(pc >> 6);
    te_csr->cfg_table_property[tbl_id].mpu_pc_dyn(1);
    te_csr->cfg_table_property[tbl_id].addr_base(offset);
    te_csr->cfg_table_property[tbl_id].write();
}

#define ELBA_P4PLUS_HANDLE         "p4plus"
#define ELBA_P4PLUS_RXDMA_PROG		"rxdma_stage0.bin"
#define ELBA_P4PLUS_RXDMA_EXT_PROG	"rxdma_stage0_ext.bin"
#define ELBA_P4PLUS_TXDMA_PROG		"txdma_stage0.bin"
#define ELBA_P4PLUS_TXDMA_EXT_PROG	"txdma_stage0_ext.bin"

void
elba_program_p4plus_sram_table_mpu_pc (int tableid, int stage_tbl_id,
                                       int stage)
{
    uint64_t pc = 0;
    elb_te_csr_t *te_csr = NULL;
    bool pipe_rxdma = false;

    elb_top_csr_t &elb0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);

    if ((uint32_t)tableid >= p4pd_rxdma_tableid_min_get() &&
        (uint32_t)tableid < p4pd_rxdma_tableid_max_get()) {
        uint32_t lcl_tableid = tableid - p4pd_rxdma_tableid_min_get();
        te_csr = &elb0.pcr.te[stage];
        pc = elba_table_rxdma_asm_base[lcl_tableid];
        pipe_rxdma = true;
    } else if ((uint32_t)tableid >= p4pd_txdma_tableid_min_get() &&
               (uint32_t)tableid < p4pd_txdma_tableid_max_get()) {
        uint32_t lcl_tableid = tableid - p4pd_txdma_tableid_min_get();
        te_csr = &elb0.pct.te[stage];
        pc = elba_table_txdma_asm_base[lcl_tableid];
        pipe_rxdma = false;
    }
    if (pc == 0) {
        return;
    }
    SDK_TRACE_DEBUG("====Pipe: %s Stage: %d Tbl_id: %u, Stg_Tbl_id %u, "
                    "Tbl base: 0x%lx====", ((pipe_rxdma) ? "RxDMA" : "TxDMA"),
                    stage, tableid, stage_tbl_id, pc);
    te_csr->cfg_table_property[stage_tbl_id].read();
    te_csr->cfg_table_property[stage_tbl_id].mpu_pc(pc >> 6);
    te_csr->cfg_table_property[stage_tbl_id].mpu_pc_dyn(0);
    te_csr->cfg_table_property[stage_tbl_id].addr_base(0);
    te_csr->cfg_table_property[stage_tbl_id].write();
}

sdk_ret_t
elba_p4plus_table_init (platform_type_t platform_type,
                        int stage_apphdr, int stage_tableid_apphdr,
                        int stage_apphdr_ext, int stage_tableid_apphdr_ext,
                        int stage_apphdr_off, int stage_tableid_apphdr_off,
                        int stage_apphdr_ext_off, int stage_tableid_apphdr_ext_off,
                        int stage_txdma_act, int stage_tableid_txdma_act,
                        int stage_txdma_act_ext, int stage_tableid_txdma_act_ext,
                        int stage_sxdma_act, int stage_tableid_sxdma_act)
{
    elb_top_csr_t & elb0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    elb_te_csr_t *te_csr = NULL;
    uint64_t elba_action_p4plus_asm_base;

    // Resolve the p4plus rxdma stage 0 program to its action pc
    if (sdk::p4::p4_program_to_base_addr((char *) ELBA_P4PLUS_HANDLE,
                                   (char *) ELBA_P4PLUS_RXDMA_PROG,
                                   &elba_action_p4plus_asm_base) != 0) {
        SDK_TRACE_ERR("Could not resolve handle %s program %s",
                        (char *) ELBA_P4PLUS_HANDLE,
                        (char *) ELBA_P4PLUS_RXDMA_PROG);
        return SDK_RET_ERR;
    }
    SDK_TRACE_DEBUG("Resolved handle %s program %s to PC 0x%lx",
                    (char *) ELBA_P4PLUS_HANDLE,
                    (char *) ELBA_P4PLUS_RXDMA_PROG,
                    elba_action_p4plus_asm_base);

    // Program app-header table config @(stage, stage_tableid) with the PC
    te_csr = &elb0.pcr.te[stage_apphdr];
    elba_program_p4plus_table_mpu_pc_args(
            stage_tableid_apphdr, te_csr,
            elba_action_p4plus_asm_base,
            ELBA_P4PLUS_RX_STAGE0_QSTATE_OFFSET_0);

    // Program app-header offset 64 table config @(stage, stage_tableid)
    // with the same PC as above
    elba_program_p4plus_table_mpu_pc_args(
            stage_tableid_apphdr_off, te_csr,
            elba_action_p4plus_asm_base,
            ELBA_P4PLUS_RX_STAGE0_QSTATE_OFFSET_64);


    // Resolve the p4plus rxdma stage 0 "ext" program to its action pc
    if (sdk::p4::p4_program_to_base_addr((char *) ELBA_P4PLUS_HANDLE,
                                   (char *) ELBA_P4PLUS_RXDMA_EXT_PROG,
                                   &elba_action_p4plus_asm_base) != 0) {
        SDK_TRACE_DEBUG("Could not resolve handle %s program %s",
                        (char *) ELBA_P4PLUS_HANDLE,
                        (char *) ELBA_P4PLUS_RXDMA_EXT_PROG);
        return SDK_RET_ERR;
    }
    SDK_TRACE_DEBUG("Resolved handle %s program %s to PC 0x%lx",
                    (char *) ELBA_P4PLUS_HANDLE,
                    (char *) ELBA_P4PLUS_RXDMA_EXT_PROG,
                    elba_action_p4plus_asm_base);

    // Program app-header table config @(stage, stage_tableid) with the PC
    te_csr = &elb0.pcr.te[stage_apphdr_ext];
    elba_program_p4plus_table_mpu_pc_args(
            stage_tableid_apphdr_ext, te_csr,
            elba_action_p4plus_asm_base,
            ELBA_P4PLUS_RX_STAGE0_QSTATE_OFFSET_0);

    // Program app-header offset 64 table config @(stage, stage_tableid)
    // with the same PC as above
    elba_program_p4plus_table_mpu_pc_args(
            stage_tableid_apphdr_ext_off, te_csr,
            elba_action_p4plus_asm_base,
            ELBA_P4PLUS_RX_STAGE0_QSTATE_OFFSET_64);

    // Resolve the p4plus txdma stage 0 program to its action pc
    if (sdk::p4::p4_program_to_base_addr((char *) ELBA_P4PLUS_HANDLE,
                                   (char *) ELBA_P4PLUS_TXDMA_PROG,
                                   &elba_action_p4plus_asm_base) != 0) {
        SDK_TRACE_DEBUG("Could not resolve handle %s program %s",
                        (char *) ELBA_P4PLUS_HANDLE,
                        (char *) ELBA_P4PLUS_TXDMA_PROG);
        return SDK_RET_ERR;
    }
    SDK_TRACE_DEBUG("Resolved handle %s program %s to PC 0x%lx",
                    (char *) ELBA_P4PLUS_HANDLE,
                    (char *) ELBA_P4PLUS_TXDMA_PROG,
                    elba_action_p4plus_asm_base);

    // Program table config @(stage, stage_tableid) with the PC
    te_csr = &elb0.pct.te[stage_txdma_act];
    elba_program_p4plus_table_mpu_pc_args(
            stage_tableid_txdma_act, te_csr,
            elba_action_p4plus_asm_base, 0);

    if ((stage_txdma_act == 0) &&
        (platform_type != platform_type_t::PLATFORM_TYPE_SIM)) {
        // TODO: This should 16 as we can process 16 packets per doorbell.
        te_csr->cfg_table_property[stage_tableid_txdma_act].max_bypass_cnt(0x10);
        te_csr->cfg_table_property[stage_tableid_txdma_act].write();
    }

    // Resolve the p4plus txdma stage 0 "ext" program to its action pc
    if (sdk::p4::p4_program_to_base_addr((char *) ELBA_P4PLUS_HANDLE,
                                   (char *) ELBA_P4PLUS_TXDMA_EXT_PROG,
                                   &elba_action_p4plus_asm_base) != 0) {
        SDK_TRACE_DEBUG("Could not resolve handle %s program %s",
                        (char *) ELBA_P4PLUS_HANDLE,
                        (char *) ELBA_P4PLUS_TXDMA_EXT_PROG);
        return SDK_RET_ERR;
    }
    SDK_TRACE_DEBUG("Resolved handle %s program %s to PC 0x%lx",
                    (char *) ELBA_P4PLUS_HANDLE,
                    (char *) ELBA_P4PLUS_TXDMA_EXT_PROG,
                    elba_action_p4plus_asm_base);

    // Program table config @(stage, stage_tableid) with the PC
    te_csr = &elb0.pct.te[stage_txdma_act_ext];
    elba_program_p4plus_table_mpu_pc_args(
            stage_tableid_txdma_act_ext, te_csr,
            elba_action_p4plus_asm_base, 0);

    if ((stage_txdma_act_ext == 0) &&
        (platform_type != platform_type_t::PLATFORM_TYPE_SIM)) {
        // TODO: This should 16 as we can process 16 packets per doorbell.
        te_csr->cfg_table_property[stage_tableid_txdma_act_ext].max_bypass_cnt(0x10);
        te_csr->cfg_table_property[stage_tableid_txdma_act_ext].write();
    }

    // Resolve the p4plus sxdma stage 0 program to its action pc
    if (sdk::p4::p4_program_to_base_addr((char *) ELBA_P4PLUS_HANDLE,
                                   (char *) ELBA_P4PLUS_TXDMA_PROG,
                                   &elba_action_p4plus_asm_base) != 0) {
        SDK_TRACE_DEBUG("Could not resolve handle %s program %s",
                        (char *) ELBA_P4PLUS_HANDLE,
                        (char *) ELBA_P4PLUS_TXDMA_PROG);
        return SDK_RET_ERR;
    }
    SDK_TRACE_DEBUG("SXDMA:Resolved handle %s program %s to PC 0x%lx",
                    (char *) ELBA_P4PLUS_HANDLE,
                    (char *) ELBA_P4PLUS_TXDMA_PROG,
                    elba_action_p4plus_asm_base);

    // Program table config @(stage, stage_tableid) with the PC
    te_csr = &elb0.xg.te[stage_sxdma_act];
    elba_program_p4plus_table_mpu_pc_args(stage_tableid_sxdma_act, te_csr,
                                          elba_action_p4plus_asm_base, 0);

    if ((stage_sxdma_act == 0) &&
        (platform_type != platform_type_t::PLATFORM_TYPE_SIM)) {
        // TODO: This should 16 as we can process 16 packets per doorbell.
        te_csr->cfg_table_property[stage_tableid_sxdma_act].max_bypass_cnt(0x10);
        te_csr->cfg_table_property[stage_tableid_sxdma_act].write();
    }

    return SDK_RET_OK;
}

void
elba_p4plus_recirc_init (void)
{
    elb_top_csr_t &elb0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);

    // RxDMA
    elb0.pr.pr.psp.cfg_profile.read();
    elb0.pr.pr.psp.cfg_profile.recirc_max_enable(1);
    elb0.pr.pr.psp.cfg_profile.recirc_max(7);
    elb0.pr.pr.psp.cfg_profile.write();

    // TxDMA
    elb0.pt.pt.psp.cfg_profile.read();
    elb0.pt.pt.psp.cfg_profile.recirc_max_enable(1);
    elb0.pt.pt.psp.cfg_profile.recirc_max(7);
    elb0.pt.pt.psp.cfg_profile.write();
}

void
elba_timer_init_helper (uint32_t key_lines)
{
    uint64_t timer_key_hbm_base_addr;
    elb_top_csr_t & elb0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    elb_txs_csr_t *txs_csr = &elb0.txs.txs;

    timer_key_hbm_base_addr = get_mem_addr(MEM_REGION_TIMERS_NAME);

    txs_csr->cfg_timer_static.read();
    SDK_TRACE_DEBUG("hbm_base %lx",
                    (uint64_t)txs_csr->cfg_timer_static.hbm_base());
    SDK_TRACE_DEBUG("timer hash depth %u",
                    int(txs_csr->cfg_timer_static.tmr_hsh_depth()));
    SDK_TRACE_DEBUG("timer wheel depth %u",
                    int(txs_csr->cfg_timer_static.tmr_wheel_depth()));

    txs_csr->cfg_timer_static.hbm_base(timer_key_hbm_base_addr);
    txs_csr->cfg_timer_static.tmr_hsh_depth(key_lines - 1);
    txs_csr->cfg_timer_static.tmr_wheel_depth(ELBA_TIMER_WHEEL_DEPTH - 1);
    txs_csr->cfg_timer_static.write();

    txs_csr->cfg_fast_timer_dbell.read();
    txs_csr->cfg_fast_timer_dbell.addr_update(DB_IDX_UPD_PIDX_INC |
                                              DB_SCHED_UPD_EVAL);
    txs_csr->cfg_fast_timer_dbell.write();

    txs_csr->cfg_slow_timer_dbell.read();
    txs_csr->cfg_slow_timer_dbell.addr_update(DB_IDX_UPD_PIDX_INC |
                                              DB_SCHED_UPD_EVAL);
    txs_csr->cfg_slow_timer_dbell.write();

    // TODO:remove
    txs_csr->cfg_timer_static.read();
    SDK_TRACE_DEBUG("hbm_base 0x%lx",
                    (uint64_t)txs_csr->cfg_timer_static.hbm_base());
    SDK_TRACE_DEBUG("timer hash depth %u",
                    int(txs_csr->cfg_timer_static.tmr_hsh_depth()));
    SDK_TRACE_DEBUG("timer wheel depth %u",
                    int(txs_csr->cfg_timer_static.tmr_wheel_depth()));

    SDK_TRACE_DEBUG("Done initializing timer wheel");
}

void
elba_timer_init (void)
{
    elba_timer_init_helper(ELBA_TIMER_NUM_KEY_CACHE_LINES);
}

// This function initializes the stage id register for p4 plus pipelines such that:
//         val0  : 4
//         val1  : 5
//         val2  : 6
//         val3  : 7
//         val4  : 0
//         val5  : 1
//         val6  : 2
//         val7  : 3
//
static void
elba_p4p_stage_id_init (void)
{
}

static inline bool
p4plus_invalidate_cache_aligned (uint64_t addr, uint32_t size_in_bytes,
                                 p4plus_cache_action_t action)
{
    return true;
}

bool
p4plus_invalidate_cache (uint64_t addr, uint32_t size_in_bytes,
                         p4plus_cache_action_t action)
{
    bool ret;

    if ((addr & ~CACHE_LINE_SIZE_MASK) == addr) {
        ret = p4plus_invalidate_cache_aligned(addr, size_in_bytes, action);
    } else {
        int unalign_size = addr & CACHE_LINE_SIZE_MASK;
        ret = p4plus_invalidate_cache_aligned(addr & ~CACHE_LINE_SIZE_MASK,
                                        size_in_bytes + unalign_size,
                                        action);
    }

    return ret;
}

bool
p4plus_invalidate_cache_all (p4plus_cache_action_t action)
{
    return true;
}

void
p4_invalidate_cache (uint64_t addr, uint32_t size_in_bytes,
                     p4pd_table_cache_t cache)
{
    //@@TODO - implement for elba
}

void
elba_deparser_init(int tm_port_ingress, int tm_port_egress)
{
    elb_top_csr_t &elb0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    cpp_int recirc_rw_bm = 0;
    // Ingress deparser is indexed with 1
    elb0.dpr.dpr[1].cfg_global_2.read();
    elb0.dpr.dpr[1].cfg_global_2.increment_recirc_cnt_en(1);
    elb0.dpr.dpr[1].cfg_global_2.drop_max_recirc_cnt(1);
    // Drop after 4 recircs
    elb0.dpr.dpr[1].cfg_global_2.max_recirc_cnt(4);
    elb0.dpr.dpr[1].cfg_global_2.recirc_oport(tm_port_ingress);
    elb0.dpr.dpr[1].cfg_global_2.clear_recirc_bit_en(1);
    recirc_rw_bm |= 1<<tm_port_ingress;
    recirc_rw_bm |= 1<<tm_port_egress;
    elb0.dpr.dpr[1].cfg_global_2.recirc_rw_bm(recirc_rw_bm);
    elb0.dpr.dpr[1].cfg_global_2.write();
    // Egress deparser is indexed with 0
    elb0.dpr.dpr[0].cfg_global_2.read();
    elb0.dpr.dpr[0].cfg_global_2.increment_recirc_cnt_en(0);
    elb0.dpr.dpr[0].cfg_global_2.frame_size_rw_bm(0xffff);
    elb0.dpr.dpr[0].cfg_global_2.write();
}

static void
elba_mpu_icache_invalidate (void)
{
}

//
// Reset tcam memories
//
static void
elba_tcam_memory_init (asic_cfg_t *elba_cfg)
{
    if (!elba_cfg ||
        ((elba_cfg->platform != platform_type_t::PLATFORM_TYPE_HAPS) &&
        (elba_cfg->platform != platform_type_t::PLATFORM_TYPE_HW))) {
        return;
    }

    elb_pict_zero_init_tcam(0, 0, 8);
    elb_pict_zero_init_tcam(0, 1, 4);
}

void
elba_table_rw_cleanup (void)
{
    for (int i = P4_GRESS_INGRESS; i <= P4_GRESS_EGRESS; i++) {
        if (g_shadow_sram_p4[i]) {
            ELBA_FREE(g_shadow_sram_p4[i]);
        }
        g_shadow_sram_p4[i] = NULL;
        if (g_shadow_tcam_p4[i]) {
            ELBA_FREE(g_shadow_tcam_p4[i]);
        }
        g_shadow_tcam_p4[i] = NULL;
    }
}

static sdk_ret_t
elba_p4_shadow_init (void)
{
    for (int i = P4_GRESS_INGRESS; i <= P4_GRESS_EGRESS; i++) {
        g_shadow_sram_p4[i] = (elba_sram_shadow_mem_t*)
                ELBA_CALLOC(1, sizeof(elba_sram_shadow_mem_t));
        g_shadow_tcam_p4[i] = (elba_tcam_shadow_mem_t*)
                ELBA_CALLOC(1, sizeof(elba_tcam_shadow_mem_t));
        if (!g_shadow_sram_p4[i] || !g_shadow_tcam_p4[i]) {
            // TODO: Log error/trace
            elba_table_rw_cleanup();
            return SDK_RET_ERR;
        }
        // Initialize shadow tcam to match all ones. This makes all entries
        // to be treated as inactive
        memset(g_shadow_tcam_p4[i]->mem_x, 0xFF,
               sizeof(g_shadow_tcam_p4[i]->mem_x));
    }

    return SDK_RET_OK;
}

static inline void
elb_pics_zero_init_sram(uint32_t a, uint32_t b, uint32_t c)
{
  elb_pics_zero_init_sram_start(a,b,c);
  elb_pics_zero_init_sram_done(a,b,c);
}

static void
elba_sram_memory_init (asic_cfg_t *elba_cfg)
{
    if (!elba_cfg ||
        ((elba_cfg->platform != platform_type_t::PLATFORM_TYPE_HAPS) &&
        (elba_cfg->platform != platform_type_t::PLATFORM_TYPE_HW))) {
        return;
    }

    elb_pics_zero_init_sram(0, 0, 3);
    elb_pics_zero_init_sram(0, 1, 8);
    elb_pics_zero_init_sram(0, 2, 8);
    elb_pics_zero_init_sram(0, 3, 3);
}

void
elba_p4plus_table_rw_cleanup (void)
{
    if (g_shadow_sram_rxdma) {
        ELBA_FREE(g_shadow_sram_rxdma);
    }
    if (g_shadow_sram_txdma) {
        ELBA_FREE(g_shadow_sram_txdma);
    }
    g_shadow_sram_rxdma = NULL;
    g_shadow_sram_txdma = NULL;
}

static sdk_ret_t
elba_p4plus_shadow_init (void)
{
    g_shadow_sram_rxdma =
        (elba_sram_shadow_mem_t*)ELBA_CALLOC(1, sizeof(elba_sram_shadow_mem_t));
    g_shadow_sram_txdma =
        (elba_sram_shadow_mem_t*)ELBA_CALLOC(1, sizeof(elba_sram_shadow_mem_t));

    if (!g_shadow_sram_rxdma || !g_shadow_sram_txdma) {
        // TODO: Log error/trace
        elba_p4plus_table_rw_cleanup();
        return SDK_RET_ERR;
    }

    return SDK_RET_OK;
}

void
elba_table_csr_cache_inval_init (void)
{
    csr_cache_inval_ingress_va =
        (uint32_t *)sdk::lib::pal_mem_map(CSR_CACHE_INVAL_INGRESS_REG_ADDR,
                                          0x4);
    csr_cache_inval_egress_va =
        (uint32_t *)sdk::lib::pal_mem_map(CSR_CACHE_INVAL_EGRESS_REG_ADDR, 0x4);
    csr_cache_inval_txdma_va =
        (uint32_t *)sdk::lib::pal_mem_map(CSR_CACHE_INVAL_TXDMA_REG_ADDR, 0x4);
    csr_cache_inval_rxdma_va =
        (uint32_t *)sdk::lib::pal_mem_map(CSR_CACHE_INVAL_RXDMA_REG_ADDR, 0x4);
    SDK_TRACE_DEBUG("CSR cache inval ing 0x%lx, egr 0x%lx, txdma 0x%lx, rxdma 0x%lx",
                    (mem_addr_t)csr_cache_inval_ingress_va,
                    (mem_addr_t)csr_cache_inval_egress_va,
                    (mem_addr_t)csr_cache_inval_txdma_va,
                    (mem_addr_t)csr_cache_inval_rxdma_va);
}

sdk_ret_t
elba_table_rw_soft_init (asic_cfg_t *elba_cfg)
{
    int ret;
    /* Initialize the CSR cache invalidate memories */
    elba_table_csr_cache_inval_init();

    ret = elba_p4_shadow_init();
    if (ret != ELBA_OK) {
        return SDK_RET_ERR;
    }

    ret = elba_p4plus_shadow_init();
    if (ret != ELBA_OK) {
        return SDK_RET_ERR;
    }
    return SDK_RET_OK;
}

sdk_ret_t
elba_table_rw_init (asic_cfg_t *elba_cfg)
{
    sdk_ret_t ret;
    // Before making this call, it is expected that
    // in HAL init sequence, p4pd_init() needs to be called before this
    // Create shadow memory and init to zero

    /* Initialize the CSR cache invalidate memories */
    elba_table_csr_cache_inval_init();

    ret = elba_p4_shadow_init();
    if (ret != SDK_RET_OK) {
        return ret;
    }

    ret = elba_p4plus_shadow_init();
    if (ret != SDK_RET_OK) {
        return ret;
    }

    // Initialize stage id registers for p4p
    elba_p4p_stage_id_init();

    hbm_mem_base_addr = get_mem_addr(MEM_REGION_P4_PROGRAM_NAME);

    elba_mpu_icache_invalidate();

    // Initialize tcam memories
    elba_tcam_memory_init(elba_cfg);

    // Initialize sram memories
    elba_sram_memory_init(elba_cfg);

    return SDK_RET_OK;
}

sdk_ret_t
elba_p4plus_table_rw_init (void)
{
    // in HAL init sequence, p4pd_init() needs to be called before this
    elba_p4plus_shadow_init();
    csr_init();

    return (SDK_RET_OK);
}

uint8_t
elba_get_action_pc (uint32_t tableid, uint8_t actionid)
{
    if ((tableid >= p4pd_tableid_min_get()) &&
        (tableid <= p4pd_tableid_max_get())) {
        return ((uint8_t)elba_action_asm_base[tableid][actionid]);
    } else if ((tableid >= p4pd_rxdma_tableid_min_get()) &&
               (tableid <= p4pd_rxdma_tableid_max_get())) {
        uint32_t lcl_tableid = tableid - p4pd_rxdma_tableid_min_get();
        return ((uint8_t)elba_action_rxdma_asm_base[lcl_tableid][actionid]);
    } else if ((tableid >= p4pd_txdma_tableid_min_get()) &&
               (tableid <= p4pd_txdma_tableid_max_get())) {
        uint32_t lcl_tableid = tableid - p4pd_txdma_tableid_min_get();
        return ((uint8_t)elba_action_txdma_asm_base[lcl_tableid][actionid]);
    } else {
        SDK_ASSERT(0);
    }
}

uint8_t
elba_get_action_id (uint32_t tableid, uint8_t actionpc)
{
    if ((tableid >= p4pd_tableid_min_get()) &&
        (tableid <= p4pd_tableid_max_get())) {
        for (int j = 0; j < p4pd_get_max_action_id(tableid); j++) {
            if (elba_action_asm_base[tableid][j] == actionpc) {
                return j;
            }
        }
    } else if ((tableid >= p4pd_rxdma_tableid_min_get()) &&
               (tableid <= p4pd_rxdma_tableid_max_get())) {
        for (int j = 0; j < p4pd_rxdma_get_max_action_id(tableid); j++) {
            uint32_t lcl_tableid = tableid - p4pd_rxdma_tableid_min_get();
            if (elba_action_rxdma_asm_base[lcl_tableid][j] == actionpc) {
                return j;
            }
        }
    } else if ((tableid >= p4pd_txdma_tableid_min_get()) &&
               (tableid <= p4pd_txdma_tableid_max_get())) {
        for (int j = 0; j < p4pd_txdma_get_max_action_id(tableid); j++) {
            uint32_t lcl_tableid = tableid - p4pd_txdma_tableid_min_get();
            if (elba_action_txdma_asm_base[lcl_tableid][j] == actionpc) {
                return j;
            }
        }
    }

    return (0xff);
}

static void
elba_sram_entry_details_get (uint32_t index,
                             int *sram_row, int *entry_start_block,
                             int *entry_end_block, int *entry_start_word,
                             uint16_t top_left_x, uint16_t top_left_y,
                             uint8_t top_left_block, uint16_t btm_right_y,
                             uint8_t num_buckets, uint16_t entry_width)
{
    *sram_row = top_left_y + (index/num_buckets);
    assert(*sram_row <= btm_right_y);
    int tbl_col = index % num_buckets;
    // entry_width is in units of SRAM word  -- 16b

    *entry_start_word = (top_left_x + (tbl_col * entry_width))
                        % ELBA_SRAM_WORDS_PER_BLOCK;
    // Elba 16b word within a 128b block is numbered from right to left.
    //*entry_start_word = (ELBA_SRAM_WORDS_PER_BLOCK - 1) - *entry_start_word;

    *entry_start_block = (top_left_block * ELBA_SRAM_ROWS)
                         + ((((tbl_col * entry_width) + top_left_x)
                           / ELBA_SRAM_WORDS_PER_BLOCK) * ELBA_SRAM_ROWS)
                         + top_left_y + (index/num_buckets);

    *entry_end_block = *entry_start_block + (((entry_width - 1) +
                         (*entry_start_word % ELBA_SRAM_WORDS_PER_BLOCK))
                         / ELBA_SRAM_WORDS_PER_BLOCK) * ELBA_SRAM_ROWS;

}

elb_pics_csr_t *
elba_global_pics_get (uint32_t tableid)
{
    elb_top_csr_t & elb0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);

    if ((tableid >= p4pd_tableid_min_get()) &&
        (tableid <= p4pd_tableid_max_get())) {
        return &elb0.ssi.pics;
    }
    if ((tableid >= p4pd_tableid_min_get()) &&
        (tableid <= p4pd_tableid_max_get())) {
        return &elb0.ssi.pics;
    } else if ((tableid >= p4pd_rxdma_tableid_min_get()) &&
               (tableid <= p4pd_rxdma_tableid_max_get())) {
        return &elb0.rd.pics;
    } else if ((tableid >= p4pd_txdma_tableid_min_get()) &&
               (tableid <= p4pd_txdma_tableid_max_get())) {
        return &elb0.td.pics;
    } else {
        SDK_ASSERT(0);
    }

    return ((elb_pics_csr_t*)nullptr);
}

sdk_ret_t
elba_table_entry_write (uint32_t tableid, uint32_t index, uint8_t  *hwentry,
                        uint8_t  *hwentry_mask, uint16_t hwentry_bit_len,
                        p4_table_mem_layout_t &tbl_info, int gress,
                        bool is_oflow_table, bool ingress,
                        uint32_t ofl_parent_tbl_depth)
{
    // 1. When a Memory line is shared by multiple tables, only tableid's
    //    table entry bits need to be modified in the memory line.
    //    1. read Shadow memory line (entire 128bits in case of SRAM)
    //    2. clear out bits that corresponds to table.
    // 2. Argument hwentry contains byte stream that is already in format that
    //    agrees to Capri.
    //    Bytes read from match-table (SRAM or TCAM) are swizzled before
    //    comparing key bits. Today as per HW team, byte swizzing is
    //    Byte 0 in memory is Byte 63 in KeyMaker (512b keymaker)
    //    Byte 1 in memory is Byte 62 in KeyMaker (512b keymaker)
    //    :
    //    Byte 63 in memory is Byte 0 in KeyMaker (512b keymaker)
    // 3. Write all 128bits back to HW. In case of wide key write back
    //    multiple 128b blocks. When writing back all 128b blocks its
    //    possible to update neighbour table's entry back with same value
    //    as before.
    //

    // Assuming a table entry is contained within a SRAM row...
    // Entry cannot be wider than entire row (10 x 128bits)

    int sram_row, entry_start_block, entry_end_block;
    int entry_start_word;
    elba_sram_shadow_mem_t *shadow_sram;

    shadow_sram = get_sram_shadow_for_table(tableid, gress);

    // In case of overflow TCAM, SRAM associated with the table
    // is folded along with its parent's hash table.
    // Change index to parent table size + index
    if (is_oflow_table) {
        index += ofl_parent_tbl_depth;
    }

    elba_sram_entry_details_get(index,
                                 &sram_row, &entry_start_block,
                                 &entry_end_block, &entry_start_word,
                                 tbl_info.top_left_x,
                                 tbl_info.top_left_y,
                                 tbl_info.top_left_block,
                                 tbl_info.btm_right_y,
                                 tbl_info.num_buckets,
                                 tbl_info.entry_width);
    int tbl_col = index % tbl_info.num_buckets;
    int blk = tbl_info.top_left_block
                 + (((tbl_col * tbl_info.entry_width) + tbl_info.top_left_x) /
                     ELBA_SRAM_WORDS_PER_BLOCK);
    int block = blk;
    int copy_bits = hwentry_bit_len;
    uint16_t *_hwentry = (uint16_t*)hwentry;

    if (hwentry_mask) {
        // If mask is specified, it should encompass the entire macros currently
        if ((entry_start_word != 0) ||
            (tbl_info.entry_width % ELBA_SRAM_WORDS_PER_BLOCK)) {
            SDK_TRACE_ERR("Masked write with entry_start_word %u and width %u "
                          "not supported",
                          entry_start_word, tbl_info.entry_width);
            return SDK_RET_INVALID_ARG;
        }
    }

    for (int j = 0; j < tbl_info.entry_width; j++) {
        if (copy_bits >= 16) {
            shadow_sram->mem[sram_row][block %
                ELBA_SRAM_BLOCK_COUNT][entry_start_word] = *_hwentry;
            _hwentry++;
            copy_bits -= 16;
        } else if (copy_bits) {
            assert(0);
        }
        entry_start_word++;
        if (entry_start_word % ELBA_SRAM_WORDS_PER_BLOCK == 0) {
            // crossed over to next block
            //block += ELBA_SRAM_ROWS;
            block++;
            entry_start_word = 0;
        }
    }

    elb_top_csr_t & elb0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    // Push to HW/Capri from entry_start_block to block
    pu_cpp_int<128> sram_block_data;
    pu_cpp_int<128> sram_block_datamask;
    uint8_t temp[16], tempmask[16];
    for (int i = entry_start_block;
         i <= entry_end_block; i += ELBA_SRAM_ROWS, blk++) {
        //all shadow_sram->mem[sram_row][i] to be pushed to elba..
        uint8_t *s = (uint8_t*)(shadow_sram->mem[sram_row][blk]);
        for (int p = 15; p >= 0; p--) {
            temp[p] = *s; s++;
        }
        elb_pics_csr_t *pics_csr;
        if (ingress) {
            pics_csr = elba_global_pics_get(tableid);
        } else {
            pics_csr = &elb0.sse.pics;
        }
        sram_block_data = 0;
        cpp_int_helper::s_cpp_int_from_array(sram_block_data, 0, 15, temp);

        if (hwentry_mask) {
            uint8_t *m = hwentry_mask +
                (i-entry_start_block)*(ELBA_SRAM_BLOCK_WIDTH>>3) ;
            for (int p = 15; p >= 0; p--) {
                tempmask[p] = *m; m++;
            }

            sram_block_datamask = 0;
            cpp_int_helper::s_cpp_int_from_array(sram_block_datamask,
                                                 0, 15, tempmask);

            pics_csr->dhs_sram_update_addr.entry.address(i);
            pics_csr->dhs_sram_update_addr.entry.write();
            pics_csr->dhs_sram_update_data.entry.data(sram_block_data);
            pics_csr->dhs_sram_update_data.entry.mask(sram_block_datamask);
            pics_csr->dhs_sram_update_data.entry.write();

        } else {
            pics_csr->dhs_sram.entry[i]
                .data((pu_cpp_int<128>)sram_block_data);
            pics_csr->dhs_sram.entry[i].write();
        }
    }

    return SDK_RET_OK;
}

sdk_ret_t
elba_table_entry_read (uint32_t tableid, uint32_t index, uint8_t  *hwentry,
                       uint16_t *hwentry_bit_len,
                        p4_table_mem_layout_t &tbl_info, int gress,
                        bool is_oflow_table, uint32_t ofl_parent_tbl_depth)
{
    //
    // Unswizzing of the bytes into readable format is
    // expected to be done by caller of the API.
    //
    int sram_row, entry_start_block, entry_end_block;
    int entry_start_word;
    // In case of overflow TCAM, SRAM associated with the table
    // is folded along with its parent's hash table.
    // Change index to parent table size + index
    if (is_oflow_table) {
        index += ofl_parent_tbl_depth;
    }
    elba_sram_entry_details_get(index,
                                 &sram_row, &entry_start_block,
                                 &entry_end_block, &entry_start_word,
                                 tbl_info.top_left_x,
                                 tbl_info.top_left_y,
                                 tbl_info.top_left_block,
                                 tbl_info.btm_right_y,
                                 tbl_info.num_buckets,
                                 tbl_info.entry_width);
    int tbl_col = index % tbl_info.num_buckets;
    int blk = tbl_info.top_left_block
                 + (((tbl_col * tbl_info.entry_width) + tbl_info.top_left_x) /
                     ELBA_SRAM_WORDS_PER_BLOCK);
    int block = blk;
    int copy_bits = tbl_info.entry_width_bits;
    uint16_t *_hwentry = (uint16_t*)hwentry;

    elba_sram_shadow_mem_t *shadow_sram;

    shadow_sram = get_sram_shadow_for_table(tableid, gress);

    while(copy_bits) {
        if (copy_bits >= 16) {
            *_hwentry = shadow_sram->mem[sram_row][block %
                ELBA_SRAM_BLOCK_COUNT][entry_start_word];
            _hwentry++;
            copy_bits -= 16;
        } else {
            if (copy_bits > 8) {
                *_hwentry = shadow_sram->mem[sram_row][block %
                    ELBA_SRAM_BLOCK_COUNT][entry_start_word];
            } else {
                *(uint8_t*)_hwentry =
                    shadow_sram->mem[sram_row][block %
                    ELBA_SRAM_BLOCK_COUNT][entry_start_word] >> 8;
            }
            copy_bits = 0;
        }
        entry_start_word++;
        if (entry_start_word % ELBA_SRAM_WORDS_PER_BLOCK == 0) {
            // crossed over to next block
            block++;
            entry_start_word = 0;
        }
    }

    *hwentry_bit_len = tbl_info.entry_width_bits;

    return SDK_RET_OK;
}

sdk_ret_t
elba_table_hw_entry_read (uint32_t tableid, uint32_t index,
                          uint8_t  *hwentry, uint16_t *hwentry_bit_len,
                          p4_table_mem_layout_t &tbl_info, int gress,
                          bool is_oflow_table, bool ingress,
                          uint32_t ofl_parent_tbl_depth)
{
    //
    // Unswizzing of the bytes into readable format is
    // expected to be done by caller of the API.
    //
    int sram_row, entry_start_block, entry_end_block;
    int entry_start_word;
    // In case of overflow TCAM, SRAM associated with the table
    // is folded along with its parent's hash table.
    // Change index to parent table size + index
    if (is_oflow_table) {
        index += ofl_parent_tbl_depth;
    }
    elba_sram_entry_details_get(index,
                                 &sram_row, &entry_start_block,
                                 &entry_end_block, &entry_start_word,
                                 tbl_info.top_left_x,
                                 tbl_info.top_left_y,
                                 tbl_info.top_left_block,
                                 tbl_info.btm_right_y,
                                 tbl_info.num_buckets,
                                 tbl_info.entry_width);
    int copy_bits = tbl_info.entry_width_bits;
    uint8_t *_hwentry = (uint8_t*)hwentry;
    uint8_t  byte, to_copy;
    elb_top_csr_t & elb0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    // read from HW/Capri from entry_start_block to block
    cpp_int sram_block_data;
    uint8_t temp[16];
    for (int i = entry_start_block; (i <= entry_end_block) && (copy_bits > 0);
         i += ELBA_SRAM_ROWS) {
        if (ingress) {
            elb_pics_csr_t *pics_csr = elba_global_pics_get(tableid);
            pics_csr->dhs_sram.entry[i].read();
            sram_block_data = pics_csr->dhs_sram.entry[i].data();
            cpp_int_helper::s_array_from_cpp_int(sram_block_data, 0, 15, temp);
        } else {
            elb_pics_csr_t & pics_csr = elb0.sse.pics;
            pics_csr.dhs_sram.entry[i].read();
            sram_block_data = pics_csr.dhs_sram.entry[i].data();
            cpp_int_helper::s_array_from_cpp_int(sram_block_data, 0, 15, temp);
        }
        for (int p = 15; p >= 8; p--) {
            byte = temp[p];
            temp[p] = temp[15-p];
            temp[15-p] = byte;
        }
        if (entry_start_word) {
            int bits_in_macro = 128 - (entry_start_word * 16);
            to_copy = (copy_bits > bits_in_macro) ? bits_in_macro : copy_bits;
        } else {
            to_copy = (copy_bits > 128) ? 128 : copy_bits;
        }
        uint8_t to_copy_bytes = ((((to_copy - 1) >> 4) + 1) << 4) / 8;
        memcpy(_hwentry, temp + (entry_start_word << 1), to_copy_bytes);
        copy_bits -= to_copy;
        _hwentry += to_copy_bytes;
        entry_start_word = 0;
    }
    *hwentry_bit_len = tbl_info.entry_width_bits;
    return (SDK_RET_OK);
}

//
//  ELBA TCAM ADDRESSING:
//
//       +====================================================================+
// Row0  | V | Block0 (128b value,mask) | .... | V | Block7 (128b value,mask) |
//       +--------------------------------------------------------------------+
// Row1  | V | Block0 (128b value,mask) | .... | V | Block7 (128b value,mask) |
//       +--------------------------------------------------------------------+
//                              :
//                              :
//       +--------------------------------------------------------------------+
// Row   | V | Block0 (128b value,mask) | .... | V | Block7 (128b value,mask) |
// 1K-1  +====================================================================+
//
//
//     1. Any memory writes / reads are done in units of block. To update
//        a table entry that is within one or more blocks, all such memory
//        blocks on a particular Row will need to modified and written back
//        to elba.
//
//     2. Table entry start and end on 16b boundary. Multiple such 16b words
//        are updated or read from when performing table write or read.
//
static void
elba_tcam_entry_details_get (uint32_t index, int *tcam_row,
                             int *entry_start_block, int *entry_end_block,
                             int *entry_start_word, uint16_t top_left_y,
                             uint8_t top_left_block, uint16_t btm_right_y,
                             uint8_t num_buckets, uint16_t entry_width,
                             uint32_t start_index)
{
    *tcam_row = top_left_y + (index/num_buckets);
    assert (*tcam_row <= btm_right_y);
    int tbl_col = index % num_buckets;

    // entry_width is in units of TCAM word  -- 16b */
    // Since every tcam table entry occupies one TCAM block */
    *entry_start_word = start_index % ELBA_TCAM_WORDS_PER_BLOCK;
    // Capri 16b word within a 128b block is numbered from right to left.*/
    // *entry_start_word = (ELBA_TCAM_WORDS_PER_BLOCK - 1) - *entry_start_word;

    // Start block will be column away from top-left because in case of
    // tcam, atmost one entry/column of table can occupy a TCAM block.
    //
    *entry_start_block = ((top_left_block + tbl_col) * ELBA_TCAM_ROWS)
                         + top_left_y
                         + (index/num_buckets);
    *entry_end_block = *entry_start_block + (((entry_width - 1) +
                         (*entry_start_word % ELBA_TCAM_WORDS_PER_BLOCK))
                          / ELBA_TCAM_WORDS_PER_BLOCK) * ELBA_TCAM_ROWS;
}

sdk_ret_t
elba_tcam_table_entry_write (uint32_t tableid, uint32_t index,
                             uint8_t  *trit_x, uint8_t  *trit_y,
                             uint16_t hwentry_bit_len,
                             p4_table_mem_layout_t &tbl_info,
                             int gress, bool ingress)
{
    // 1. When a Memory line is shared by multiple tables, only tableid's
    //     table entry bits need to be modified in the memory line.
    //    1. read Shadow memory line (entire 64bits)
    //    2. clear out bits that corresponds to table
    // 2. Argument trit_x contains key byte stream that is already in format
    //    that agrees to Capri. trit_y contains mask byte stream that is
    //    already in format
    //    Bytes read from match-table (TCAM) are swizzled before
    //    comparing key bits. Today as per HW team, byte swizzing is
    //    Byte 0 in memory is Byte 63 in KeyMaker (512b keymaker)
    //    Byte 1 in memory is Byte 62 in KeyMaker (512b keymaker)
    //    :
    //    Byte 63 in memory is Byte 0 in KeyMaker (512b keymaker)
    // 3. Write all 64bits key,mask back to HW. In case of wide key write back
    //    multiple 64b blocks. When writing back all 64b blocks its
    //    possible to update neighbour table's entry back with same value
    //    as before.
    //
    int tcam_row, entry_start_block, entry_end_block;
    int entry_start_word;

    elba_tcam_entry_details_get(index,
                                 &tcam_row, &entry_start_block,
                                 &entry_end_block, &entry_start_word,
                                 tbl_info.top_left_y,
                                 tbl_info.top_left_block,
                                 tbl_info.btm_right_y,
                                 tbl_info.num_buckets,
                                 tbl_info.entry_width,
                                 tbl_info.start_index);
    int tbl_col = index % tbl_info.num_buckets;
    int blk = tbl_info.top_left_block
                 + ((tbl_col * tbl_info.entry_width) /
                     ELBA_TCAM_WORDS_PER_BLOCK);
    int block = blk;
    int copy_bits = hwentry_bit_len;
    uint16_t *_trit_x = (uint16_t*)trit_x;
    uint16_t *_trit_y = (uint16_t*)trit_y;
    for (int j = 0; j < tbl_info.entry_width; j++) {
        if (copy_bits >= 16) {
            g_shadow_tcam_p4[gress]->mem_x[tcam_row]
                [block % ELBA_TCAM_BLOCK_COUNT][entry_start_word] = *_trit_x;
            g_shadow_tcam_p4[gress]->mem_y[tcam_row]
                [block % ELBA_TCAM_BLOCK_COUNT][entry_start_word] = *_trit_y;
            _trit_x++;
            _trit_y++;
            copy_bits -= 16;
        } else if (copy_bits) {
            assert(0);
        } else {
            // do not match remaining bits from end of entry bits to next 16b
            // aligned word
            g_shadow_tcam_p4[gress]->mem_x[tcam_row]
                [block % ELBA_TCAM_BLOCK_COUNT][entry_start_word] = 0;
            g_shadow_tcam_p4[gress]->mem_y[tcam_row]
                [block % ELBA_TCAM_BLOCK_COUNT][entry_start_word] = 0;
        }
        entry_start_word++;
        if (entry_start_word % ELBA_TCAM_WORDS_PER_BLOCK == 0) {
            // crossed over to next block
            block++;
            entry_start_word = 0;
        }
    }

    elb_top_csr_t & elb0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    // Push to HW/Capri from entry_start_block to block
    pu_cpp_int<128> tcam_block_data_x;
    pu_cpp_int<128> tcam_block_data_y;
    uint8_t temp_x[16];
    uint8_t temp_y[16];
    for (int i = entry_start_block; i <= entry_end_block;
         i += ELBA_TCAM_ROWS, blk++) {
        uint8_t *s = (uint8_t*)(g_shadow_tcam_p4[gress]->mem_x[tcam_row][blk]);
        for (int p = 15; p >= 0; p--) {
            temp_x[p] = *s; s++;
        }
        s = (uint8_t*)(g_shadow_tcam_p4[gress]->mem_y[tcam_row][blk]);
        for (int p = 15; p >= 0; p--) {
            temp_y[p] = *s; s++;
        }
        if (ingress) {
            elb_pict_csr_t & pict_csr = elb0.tsi.pict;
            tcam_block_data_x = 0;
            tcam_block_data_y = 0;
            cpp_int_helper::s_cpp_int_from_array(tcam_block_data_x,
                                                 0, 15, temp_x);
            cpp_int_helper::s_cpp_int_from_array(tcam_block_data_y,
                                                 0, 15, temp_y);
            pict_csr.dhs_tcam_xy.entry[i].x((pu_cpp_int<128>)tcam_block_data_x);
            pict_csr.dhs_tcam_xy.entry[i].y((pu_cpp_int<128>)tcam_block_data_y);
            pict_csr.dhs_tcam_xy.entry[i].valid(1);
            pict_csr.dhs_tcam_xy.entry[i].write();
        } else {
            elb_pict_csr_t & pict_csr = elb0.tse.pict;
            tcam_block_data_x = 0;
            tcam_block_data_y = 0;
            cpp_int_helper::s_cpp_int_from_array(tcam_block_data_x,
                                                 0, 15, temp_x);
            cpp_int_helper::s_cpp_int_from_array(tcam_block_data_y,
                                                 0, 15, temp_y);
            pict_csr.dhs_tcam_xy.entry[i].x((pu_cpp_int<128>)tcam_block_data_x);
            pict_csr.dhs_tcam_xy.entry[i].y((pu_cpp_int<128>)tcam_block_data_y);
            pict_csr.dhs_tcam_xy.entry[i].valid(1);
            pict_csr.dhs_tcam_xy.entry[i].write();
        }
    }

    return SDK_RET_OK;
}

sdk_ret_t
elba_tcam_table_entry_read (uint32_t tableid, uint32_t index,
                            uint8_t  *trit_x, uint8_t  *trit_y,
                            uint16_t *hwentry_bit_len,
                            p4_table_mem_layout_t &tbl_info, int gress)
{
    int tcam_row, entry_start_block, entry_end_block;
    int entry_start_word;

    elba_tcam_entry_details_get(index,
                                &tcam_row, &entry_start_block,
                                &entry_end_block, &entry_start_word,
                                tbl_info.top_left_y,
                                tbl_info.top_left_block,
                                tbl_info.btm_right_y,
                                tbl_info.num_buckets,
                                tbl_info.entry_width,
                                tbl_info.start_index);

    int tbl_col = index % tbl_info.num_buckets;
    int blk = tbl_info.top_left_block
                 + ((tbl_col * tbl_info.entry_width) /
                     ELBA_TCAM_WORDS_PER_BLOCK);
    int block = blk;
    int copy_bits = tbl_info.entry_width_bits;
    int start_word = entry_start_word;
    uint16_t *_trit_x = (uint16_t*)trit_x;
    uint16_t *_trit_y = (uint16_t*)trit_y;
    while(copy_bits) {
        if (copy_bits >= 16) {
            *_trit_x = g_shadow_tcam_p4[gress]->mem_x[tcam_row]
                [block%ELBA_TCAM_BLOCK_COUNT][start_word];
            *_trit_y = g_shadow_tcam_p4[gress]->mem_y[tcam_row]
                [block%ELBA_TCAM_BLOCK_COUNT][start_word];
            _trit_x++;
            _trit_y++;
            copy_bits -= 16;
        } else if (copy_bits) {
            if (copy_bits > 8) {
                *_trit_x = g_shadow_tcam_p4[gress]->mem_x[tcam_row]
                    [block%ELBA_TCAM_BLOCK_COUNT][start_word];
                *_trit_y = g_shadow_tcam_p4[gress]->mem_y[tcam_row]
                    [block%ELBA_TCAM_BLOCK_COUNT][start_word];
            } else {
                *(uint8_t*)_trit_x = g_shadow_tcam_p4[gress]->mem_x[tcam_row]
                    [block%ELBA_TCAM_BLOCK_COUNT][start_word] >> 8;
                *(uint8_t*)_trit_y = g_shadow_tcam_p4[gress]->mem_y[tcam_row]
                    [block%ELBA_TCAM_BLOCK_COUNT][start_word] >> 8;
            }
            copy_bits = 0;
        }
        start_word++;
        if (start_word % ELBA_TCAM_WORDS_PER_BLOCK == 0) {
            // crossed over to next block
            block++;
            start_word = 0;
        }
    }
    *hwentry_bit_len = tbl_info.entry_width_bits;

    return SDK_RET_OK;
}

sdk_ret_t
elba_tcam_table_hw_entry_read (uint32_t tableid, uint32_t index,
                               uint8_t  *trit_x, uint8_t  *trit_y,
                               uint16_t *hwentry_bit_len,
                               p4_table_mem_layout_t &tbl_info, bool ingress)
{
    int tcam_row, entry_start_block, entry_end_block;
    int entry_start_word;

    elba_tcam_entry_details_get(index,
                                 &tcam_row, &entry_start_block,
                                 &entry_end_block, &entry_start_word,
                                 tbl_info.top_left_y,
                                 tbl_info.top_left_block,
                                 tbl_info.btm_right_y,
                                 tbl_info.num_buckets,
                                 tbl_info.entry_width,
                                 tbl_info.start_index);

    int copy_bits = tbl_info.entry_width_bits;
    uint8_t byte, to_copy;
    uint8_t *_trit_x = (uint8_t*)trit_x;
    uint8_t *_trit_y = (uint8_t*)trit_y;

    elb_top_csr_t & elb0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    // Push to HW/Capri from entry_start_block to block
    cpp_int tcam_block_data_x;
    cpp_int tcam_block_data_y;
    uint8_t temp_x[16];
    uint8_t temp_y[16];
    for (int i = entry_start_block; (i <= entry_end_block) && (copy_bits > 0);
         i += ELBA_TCAM_ROWS) {
        if (ingress) {
            elb_pict_csr_t & pict_csr = elb0.tsi.pict;
            pict_csr.dhs_tcam_xy.entry[i].read();
            tcam_block_data_x = pict_csr.dhs_tcam_xy.entry[i].x();
            tcam_block_data_y = pict_csr.dhs_tcam_xy.entry[i].y();
            cpp_int_helper::s_array_from_cpp_int(tcam_block_data_x, 0, 15,
                                                 temp_x);
            cpp_int_helper::s_array_from_cpp_int(tcam_block_data_y, 0, 15,
                                                 temp_y);
        } else {
            elb_pict_csr_t & pict_csr = elb0.tse.pict;
            pict_csr.dhs_tcam_xy.entry[i].read();
            tcam_block_data_x = pict_csr.dhs_tcam_xy.entry[i].x();
            tcam_block_data_y = pict_csr.dhs_tcam_xy.entry[i].y();
            cpp_int_helper::s_array_from_cpp_int(tcam_block_data_x, 0, 15,
                                                 temp_x);
            cpp_int_helper::s_array_from_cpp_int(tcam_block_data_y, 0, 15,
                                                 temp_y);
        }
        for (int p = 15; p >= 8; p--) {
            byte = temp_x[p];
            temp_x[p] = temp_x[15-p];
            temp_x[15-p] = byte;
        }
        for (int p = 15; p >= 8; p--) {
            byte = temp_y[p];
            temp_y[p] = temp_y[15-p];
            temp_y[15-p] = byte;
        }
        if (entry_start_word) {
            int bits_in_macro = 128 - (entry_start_word * 16);
            to_copy = (copy_bits > bits_in_macro) ? bits_in_macro : copy_bits;
        } else {
            to_copy = (copy_bits > 128) ? 128: copy_bits;
        }
        uint8_t to_copy_bytes = ((((to_copy - 1) >> 4) + 1) << 4) / 8;
        memcpy(_trit_x, temp_x + (entry_start_word << 1), to_copy_bytes);
        memcpy(_trit_y, temp_y + (entry_start_word << 1), to_copy_bytes);
        _trit_x += to_copy_bytes;
        _trit_y += to_copy_bytes;
        entry_start_word = 0;
        copy_bits -= to_copy;
    }
    *hwentry_bit_len = tbl_info.entry_width_bits;;

    return (SDK_RET_OK);
}

sdk_ret_t
elba_hbm_table_entry_write (uint32_t tableid, uint32_t index, uint8_t *hwentry,
                            uint16_t entry_size, uint16_t entry_width,
                            p4pd_table_properties_t *tbl_info)
{
    assert((entry_size >> 3) <= entry_width);
    assert(index < tbl_info->tabledepth);
    uint64_t entry_start_addr = (index * entry_width);

    sdk::asic::asic_mem_write(get_mem_addr(tbl_info->tablename) +
                              entry_start_addr, hwentry, (entry_size >> 3));

    return SDK_RET_OK;
}

sdk_ret_t
elba_hbm_table_entry_cache_invalidate (p4pd_table_cache_t cache,
                                       uint64_t entry_addr,
                                       uint16_t entry_width,
                                       mem_addr_t base_mem_pa)
{
    return SDK_RET_INVALID_OP;  /* TBD-ELBA-REBASE: revisit */
}

sdk_ret_t
elba_hbm_table_entry_read (uint32_t tableid, uint32_t index, uint8_t *hwentry,
                            uint16_t *entry_size,
                            p4_table_mem_layout_t &tbl_info,
                            bool read_thru)
{
    assert(index < tbl_info.tabledepth);
    uint64_t entry_start_addr = (index * tbl_info.entry_width);

    sdk::asic::asic_mem_read(get_mem_addr(tbl_info.tablename) +
                             entry_start_addr,
                             hwentry, tbl_info.entry_width);
    *entry_size = tbl_info.entry_width;

    return SDK_RET_OK;
}

sdk_ret_t
elba_table_constant_write (uint64_t val, uint32_t stage,
                           uint32_t stage_tableid, p4pd_pipeline_t pipeline)
{
    elb_te_csr_t *te_csr;
    elb_top_csr_t& elb0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);

    if (pipeline == P4_PIPELINE_INGRESS) {
        te_csr = &elb0.sgi.te[stage];
    } else if (pipeline == P4_PIPELINE_EGRESS) {
        te_csr = &elb0.sge.te[stage];
    } else if (pipeline == P4_PIPELINE_TXDMA) {
        te_csr = &elb0.pct.te[stage];
    } else if (pipeline == P4_PIPELINE_RXDMA) {
        te_csr = &elb0.pcr.te[stage];
    } else {
        return SDK_RET_INVALID_ARG;
    }
    te_csr->cfg_table_mpu_const[stage_tableid].value(val);
    te_csr->cfg_table_mpu_const[stage_tableid].write();
    return SDK_RET_OK;
}

sdk_ret_t
elba_table_constant_read (uint64_t *val, uint32_t stage,
                          int stage_tableid, p4pd_pipeline_t pipeline)
{
    elb_te_csr_t *te_csr;
    elb_top_csr_t& elb0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);

    if (pipeline == P4_PIPELINE_INGRESS) {
        te_csr = &elb0.sgi.te[stage];
    } else if (pipeline == P4_PIPELINE_EGRESS) {
        te_csr = &elb0.sge.te[stage];
    } else if (pipeline == P4_PIPELINE_TXDMA) {
        te_csr = &elb0.pct.te[stage];
    } else if (pipeline == P4_PIPELINE_RXDMA) {
        te_csr = &elb0.pcr.te[stage];
    } else {
        return SDK_RET_INVALID_ARG;
    }
    te_csr->cfg_table_mpu_const[stage_tableid].read();
    *val = te_csr->cfg_table_mpu_const[stage_tableid].
                       value().convert_to<uint64_t>();
    return SDK_RET_OK;
}

void
elba_set_action_asm_base (int tableid, int actionid, uint64_t asm_base)
{
    elba_action_asm_base[tableid][actionid] = asm_base;
    return;
}

void
elba_set_action_rxdma_asm_base (int tableid, int actionid, uint64_t asm_base)
{
    uint32_t lcl_tableid = tableid - p4pd_rxdma_tableid_min_get();
    elba_action_rxdma_asm_base[lcl_tableid][actionid] = asm_base;
    return;
}

void
elba_set_action_txdma_asm_base (int tableid, int actionid, uint64_t asm_base)
{
    uint32_t lcl_tableid = tableid - p4pd_txdma_tableid_min_get();
    elba_action_txdma_asm_base[lcl_tableid][actionid] = asm_base;
    return;
}

void
elba_set_table_rxdma_asm_base (int tableid, uint64_t asm_base)
{
    uint32_t lcl_tableid = tableid - p4pd_rxdma_tableid_min_get();
    elba_table_rxdma_asm_base[lcl_tableid] = asm_base;
    return;
}

void
elba_set_table_txdma_asm_base (int tableid, uint64_t asm_base)
{
    uint32_t lcl_tableid = tableid - p4pd_txdma_tableid_min_get();
    elba_table_txdma_asm_base[lcl_tableid] = asm_base;
    return;
}

vector < tuple < string, string, string >>
asic_csr_dump_reg (char *block_name, bool exclude_mem)
{
    typedef vector< tuple< std::string, std::string, std::string> > reg_data;
    pen_csr_base *objP = get_csr_base_from_path(block_name);
    if (objP == 0) { SDK_TRACE_DEBUG("invalid reg name"); return reg_data(); };
    vector<pen_csr_base *> elb_child_base = objP->get_children(-1);
    reg_data data_tl;
    for (auto itr : elb_child_base) {
        if (itr->get_csr_type() == pen_csr_base::CSR_TYPE_REGISTER) {
            if(itr->get_parent() != nullptr && exclude_mem) {
                if (itr->get_parent()->get_csr_type() ==
                    pen_csr_base::CSR_TYPE_MEMORY) {
                    continue;
                }
            }
            // read name of register
            string name = itr->get_hier_path();
            // read data - same as csr_read()
            itr->read();
            cpp_int data = itr->all();
            stringstream ss;
            ss << hex << "0x" << data;

            // read offset
            uint64_t offset = itr->get_offset();
            stringstream addr;
            addr << hex << "0x" << offset;

            data_tl.push_back( tuple< std::string, string,
                               std::string>(name, addr.str(), ss.str()));
        }
    }
    return data_tl;
}

vector <string>
asic_csr_list_get (string path, int level)
{
    pen_csr_base *objP = get_csr_base_from_path(path);
    vector <string> block_name;
    if (objP == 0) return vector<string>();
    for (auto itr : objP->get_children(level)) {
        block_name.push_back(itr->get_hier_path());
    }

    return block_name;
}

sdk_ret_t
elba_te_enable_capri_mode(void)
{
    elb_top_csr_t &elb0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    unsigned int stage;
    // setting capri mode for all stages in RxDMA pipeline
    for (stage = 0; stage < ELBA_P4PLUS_NUM_STAGES; stage++)
    {
        elb_te_csr_t  &te_csr = elb0.pcr.te[stage];
        te_csr.cfg_global.read();
        te_csr.cfg_global.capri_mode(1);
        te_csr.cfg_global.write();
    }
    // setting capri mode for all stages in TxDMA pipeline
    for (stage = 0; stage < ELBA_P4PLUS_NUM_STAGES; stage++)
    {
        elb_te_csr_t  &te_csr = elb0.pct.te[stage];
        te_csr.cfg_global.read();
        te_csr.cfg_global.capri_mode(1);
        te_csr.cfg_global.write();
    }
    // setting capri mode for all stages in SxDMA pipeline
    for (stage = 0; stage < ELBA_P4PLUS_SXDMA_NUM_STAGES; stage++)
    {
        elb_te_csr_t  &te_csr = elb0.xg.te[stage];
        te_csr.cfg_global.read();
        te_csr.cfg_global.capri_mode(1);
        te_csr.cfg_global.write();
    }
    SDK_TRACE_DEBUG("Elba: Enabled capri mode in TE");

    return SDK_RET_OK;
}

sdk_ret_t
elba_pf_tcam_write (uint8_t entry_idx, pf_tcam_key_fld_pos_t fields,
                    pf_tcam_key_t* tcam_key, pf_tcam_rslt_t* rslt_entry)
{
    if (!tcam_key || !rslt_entry || !entry_idx || (fields == 0)) {
        SDK_TRACE_DEBUG("Invalid input params, returning fail");
        return SDK_RET_INVALID_ARG;
    }
    SDK_TRACE_DEBUG(" in elba_pf_tcam_write \n");
    elb_top_csr_t &elb0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);
    elb_pf_csr_t &pf_csr = elb0.pf.pf;
    pf_rslt_decoder_t pf_rslt_dec;
    pf_lkup_cmn_decoder_t lkup_cmn_data,  lkup_cmn_mask;
    pf_lkup_l2_decoder_t  lkup_l2_data,  lkup_l2_mask;
    pf_lkup_ip_decoder_t  lkup_ip_data,  lkup_ip_mask;
    uint64_t mac_addr;
    bool     lkup_ip = false;
    // init data and mask fields
    lkup_cmn_data.init();
    lkup_cmn_mask.init();
    lkup_l2_data.init();
    lkup_l2_mask.init();
    lkup_ip_data.init();
    lkup_ip_mask.init();
    uint32_t int_fields = (uint32_t)fields;

    // set all mask fields with def-values, all bits set to 1
    lkup_cmn_mask.all(0);
    lkup_cmn_mask.all(lkup_cmn_mask.all()-1);
    lkup_l2_mask.all(0);
    lkup_l2_mask.all(lkup_l2_mask.all()-1);
    lkup_ip_mask.all(0);
    lkup_ip_mask.all(lkup_ip_mask.all()-1);

    pf_rslt_dec.all(0);
    while(fields) {
        for (uint8_t field_pos = 0; field_pos < PF_TCAM_FLD_MAX_FIELDS; field_pos++) {
            if PF_TCAM_FLD_IS_SET(fields, field_pos) {
                switch(field_pos) {
                case PF_TCAM_FLD_MAC_DA:
                    memcpy(&mac_addr, tcam_key->mac_da, 6);
                    mac_addr &= 0xFFFFFFFFFFFF;
                    SDK_TRACE_INFO("setting mac_da in pf_tcam_entry key, :");
                    SDK_TRACE_DEBUG("0x%012lx\n",mac_addr);
                    lkup_cmn_data.mac_da(mac_addr);
                    lkup_cmn_mask.mac_da(0);
                    break;
                case PF_TCAM_FLD_MAC_SA:
                    SDK_TRACE_INFO("setting mac_sa in pf_tcam_entry key, mac:  ");
                    memcpy(&mac_addr, tcam_key->mac_sa, 6);
                    mac_addr &= 0xFFFFFFFFFFFF;
                    SDK_TRACE_INFO("0x%012lx\n",mac_addr);
                    lkup_cmn_data.mac_sa(mac_addr);
                    lkup_cmn_mask.mac_sa(0);
                    break;
                case PF_TCAM_FLD_PORT:
                    SDK_TRACE_INFO("setting port in pf_tcam_entry key for %d", tcam_key->port);
                    lkup_cmn_data.port(tcam_key->port);
                    lkup_cmn_mask.port(0);
                    break;
                case PF_TCAM_FLD_LKUP_IP:
                    SDK_TRACE_INFO("setting lkup_ip in pf_tcam_entry key");
                    lkup_cmn_data.lkup_ip(tcam_key->lkup_ip);
                    lkup_cmn_mask.lkup_ip(0);
                    lkup_ip = true;
                    break;
                case PF_TCAM_FLD_VLAN_ID:
                    SDK_TRACE_INFO("setting vlan_id in pf_tcam_entry key %d\n", tcam_key->vlan_id);
                    lkup_cmn_data.vlanid(tcam_key->vlan_id);
                    lkup_cmn_mask.vlanid(0);
                    lkup_ip = true;
                    break;
                case PF_TCAM_FLD_PROT:
                    SDK_TRACE_INFO("setting protocol in pf_tcam_entry key %d",
                    tcam_key->pf_tcam_l2_ip_key.pf_tcam_ip_key.l4_prot);
                    lkup_ip_data.prot(tcam_key->pf_tcam_l2_ip_key.pf_tcam_ip_key.l4_prot);
                    lkup_ip_mask.prot(0);
                    break;
                case PF_TCAM_FLD_L4_SRC_PORT:
                    SDK_TRACE_INFO("setting l4_src_port in pf_tcam_entry key %d",
                    tcam_key->pf_tcam_l2_ip_key.pf_tcam_ip_key.l4_src_port);
                    lkup_ip_data.l4_sp(tcam_key->pf_tcam_l2_ip_key.pf_tcam_ip_key.l4_src_port);
                    lkup_ip_mask.l4_sp(0);
                    break;
                case PF_TCAM_FLD_L4_DST_PORT:
                    SDK_TRACE_INFO("setting l4_dst_port in pf_tcam_entry key %d",
                    tcam_key->pf_tcam_l2_ip_key.pf_tcam_ip_key.l4_dst_port);
                    lkup_ip_data.l4_dp(tcam_key->pf_tcam_l2_ip_key.pf_tcam_ip_key.l4_dst_port);
                    lkup_ip_mask.l4_dp(0);
                    break;
                case PF_TCAM_FLD_QID_VLD:
                    lkup_cmn_data.qiq_vld(tcam_key->qiq_vld);
                    lkup_cmn_mask.qiq_vld(0);
                    break;
                case PF_TCAM_FLD_QTAG_VLAN_VLD:
                    lkup_cmn_data.qtag_vlan_vld(tcam_key->qtag_vlan_vld);
                    lkup_cmn_mask.qtag_vlan_vld(0);
                    break;
                case PF_TCAM_FLD_QTAG_VLAN:
                    break;
                case PF_TCAM_FLD_COS:
                    lkup_cmn_data.cos(tcam_key->cos);
                    lkup_cmn_mask.cos(0);
                    break;
                case PF_TCAM_FLD_DE:
                    lkup_cmn_data.de(tcam_key->de);
                    lkup_cmn_mask.de(0);
                    break;
                case PF_TCAM_FLD_PYLD_LEN:
                    lkup_cmn_data.pyld_len(tcam_key->pyld_len);
                    lkup_cmn_mask.pyld_len(0);
                    break;
                case PF_TCAM_FLD_ETH_TYPE:
                    lkup_l2_data.etype(tcam_key->pf_tcam_l2_ip_key.pf_tcam_l2_key.eth_type);
                    lkup_l2_mask.etype(0);
                    break;
                case PF_TCAM_FLD_V6:
                    lkup_ip_data.v6(1);
                    lkup_ip_mask.v6(0);
                    break;
                case PF_TCAM_FLD_DSCP:
                    lkup_ip_data.dscp(tcam_key->pf_tcam_l2_ip_key.pf_tcam_ip_key.dscp);
                    lkup_ip_mask.dscp(0);
                    break;
                case PF_TCAM_FLD_ECN:
                    lkup_ip_data.ecn(tcam_key->pf_tcam_l2_ip_key.pf_tcam_ip_key.ecn);
                    lkup_ip_mask.ecn(0);
                    break;
                case PF_TCAM_FLD_OPT:
                    lkup_ip_data.opt(tcam_key->pf_tcam_l2_ip_key.pf_tcam_ip_key.opt);
                    lkup_ip_mask.opt(0);
                    break;
                case PF_TCAM_FLD_MF:
                    lkup_ip_data.mf(tcam_key->pf_tcam_l2_ip_key.pf_tcam_ip_key.mf);
                    lkup_ip_mask.mf(0);
                    break;
                case PF_TCAM_FLD_FRAG_OF:
                    lkup_ip_data.frag_ofst(tcam_key->pf_tcam_l2_ip_key.pf_tcam_ip_key.frag_ofst);
                    lkup_ip_mask.frag_ofst(0);
                    break;
                case PF_TCAM_FLD_FRAG:
                    lkup_ip_data.frag(tcam_key->pf_tcam_l2_ip_key.pf_tcam_ip_key.frag);
                    lkup_ip_mask.frag(0);
                    break;
                default:
                    break;
                }
                PF_TCAM_CLR_FLD(int_fields, field_pos);
                fields = (pf_tcam_key_fld_pos_t)int_fields;
            }
        }
    }
    SDK_TRACE_DEBUG("Entry index %d", entry_idx);
    if (!lkup_ip) {
        lkup_l2_data.cmn.all(lkup_cmn_data.all());
        lkup_l2_mask.cmn.all(lkup_cmn_mask.all());

        lkup_l2_data.set_name(pf_csr.dhs_lookup_xy.entry[entry_idx].get_hier_path()+".key");
        lkup_l2_mask.set_name(pf_csr.dhs_lookup_xy.entry[entry_idx].get_hier_path()+".mask");

        // xy tcam encoding
        pf_csr.dhs_lookup_xy.entry[entry_idx].key( lkup_l2_data.all() & ~lkup_l2_mask.all());
        pf_csr.dhs_lookup_xy.entry[entry_idx].mask(~lkup_l2_data.all() & ~lkup_l2_mask.all());
    } else {
        lkup_ip_data.cmn.all(lkup_cmn_data.all());
        lkup_ip_mask.cmn.all(lkup_cmn_mask.all());

        lkup_ip_data.set_name(pf_csr.dhs_lookup_xy.entry[entry_idx].get_hier_path()+".key");
        lkup_ip_mask.set_name(pf_csr.dhs_lookup_xy.entry[entry_idx].get_hier_path()+".mask");

        // xy tcam encoding
        pf_csr.dhs_lookup_xy.entry[entry_idx].key( lkup_ip_data.all() & ~lkup_ip_mask.all());
        pf_csr.dhs_lookup_xy.entry[entry_idx].mask(~lkup_ip_data.all() & ~lkup_ip_mask.all());
    }
    pf_csr.dhs_lookup_xy.entry[entry_idx].valid(1);
    pf_csr.dhs_lookup_xy.entry[entry_idx].write();
    pf_csr.dhs_lookup_xy.entry[entry_idx].show();

    if (rslt_entry->tc_vld) {
        pf_rslt_dec.tc_vld(rslt_entry->tc_vld);
        pf_rslt_dec.tc(rslt_entry->tc);
    }
    if (rslt_entry->out_pb) {
        SDK_TRACE_INFO("Setting out_pb, port %d", rslt_entry->pb_port);
        pf_rslt_dec.out_pb(rslt_entry->out_pb);
        pf_rslt_dec.pb_port(rslt_entry->pb_port);
    }
    if (rslt_entry->out_mnic) {
        SDK_TRACE_INFO("Setting out_mnic, vlan_op %d, vlan_id %d",
          rslt_entry->mnic_vlan_op, rslt_entry->mnic_vlan_id);
        pf_rslt_dec.out_mnic(rslt_entry->out_mnic);
        pf_rslt_dec.mnic_vlan_op(rslt_entry->mnic_vlan_op);
        pf_rslt_dec.mnic_vlan_id(rslt_entry->mnic_vlan_id);
    }
    if (rslt_entry->out_mx) {
        SDK_TRACE_INFO("Setting out_mx, port %d", rslt_entry->mx_port);
        pf_rslt_dec.out_mx(rslt_entry->out_mx);
        pf_rslt_dec.mx_port(rslt_entry->mx_port);
    }
    if (rslt_entry->out_bx) {
        SDK_TRACE_INFO("Setting out_bx");
        pf_rslt_dec.out_bx(rslt_entry->out_bx);
    }

    pf_rslt_dec.set_name(pf_csr.dhs_lookup_rslt.entry[entry_idx].get_hier_path()+".action");
    pf_csr.dhs_lookup_rslt.entry[entry_idx].action(pf_rslt_dec.all());
    pf_csr.dhs_lookup_rslt.entry[entry_idx].write();
    pf_csr.dhs_lookup_rslt.entry[entry_idx].show();

    return SDK_RET_OK;
}

sdk_ret_t
elba_ipsec_inline_enable (void)
{
    elb_top_csr_t &elb0 = ELB_BLK_REG_MODEL_ACCESS(elb_top_csr_t, 0, 0);

    //
    // is[0] is P4-Egress and is[1] is P4-Ingress crypto module
    // Currently, Inline-ipsec module (ELB-IS) is enabled only in P4I.
    //
    elb_is_csr_t &is_csr = elb0.is.is[1];

    is_csr.cfg_glb.read();
    is_csr.cfg_glb.en(1);
    is_csr.cfg_glb.iv_size(0);
    is_csr.cfg_glb.info_vld_ofst(145);
    is_csr.cfg_glb.info_ofst(176);
    is_csr.cfg_glb.write();

    is_csr = elb0.is.is[0];

    is_csr.cfg_glb.read();
    is_csr.cfg_glb.en(1);
    is_csr.cfg_glb.iv_size(0);
    is_csr.cfg_glb.info_vld_ofst(145);
    is_csr.cfg_glb.info_ofst(176);
    is_csr.cfg_glb.write();

    SDK_TRACE_DEBUG("IPSec Inline enabled globally");
    return SDK_RET_OK;
}

}    // namespace elba
}    // namespace platform
}    // namespace sdk
