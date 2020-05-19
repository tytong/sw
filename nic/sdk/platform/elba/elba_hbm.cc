// {C} Copyright 2018 Pensando Systems Inc. All rights reserved

#include <unistd.h>
#include <iostream>
#include "nic/sdk/include/sdk/base.hpp"
#include "nic/sdk/asic/rw/asicrw.hpp"
#include "platform/elba/elba_hbm_rw.hpp"
#include "platform/elba/elba_common.hpp"
#include "platform/elba/elba_state.hpp"
#include "third-party/asic/elba/model/utils/elb_blk_reg_model.h"
#include "third-party/asic/elba/model/elb_pic/elb_pics_csr.h"
#include "third-party/asic/elba/model/elb_wa/elb_wa_csr.h"
#include "third-party/asic/elba/model/elb_pcie/elb_pxb_csr.h"
#include "third-party/asic/elba/model/elb_top/elb_top_csr_defines.h"
#include "third-party/asic/elba/model/elb_top/elb_top_csr.h"
#include "third-party/asic/elba/model/elb_pic/elb_pics_csr.h"
#include "third-party/asic/elba/model/elb_ms/elb_ms_csr.h"
#include "third-party/asic/elba/model/elb_pcie/elb_pxb_csr.h"

namespace sdk {
namespace platform {
namespace elba {

static uint32_t elba_freq = 0;

static inline sdk_ret_t
elb_nx_block_write (uint32_t chip, uint64_t addr, int size,
                    uint32_t *data_in , bool no_zero_time, uint32_t flags)
{
    return sdk::asic::asic_reg_write(addr, data_in, 1,
                                        ASIC_WRITE_MODE_BLOCKING);
}

static inline uint32_t
elb_nx_block_read (uint32_t chip, uint64_t addr, int size,
                   bool no_zero_time, uint32_t flags)
{
    uint32_t data = 0x0;
    if (sdk::asic::asic_reg_read(addr, &data, 1, false /*read_thru*/) !=
                                 SDK_RET_OK) {
        SDK_TRACE_ERR("NX read failed. addr: %lx", addr);
    }

    return data;
}

unsigned int
elb_nx_read_pb_axi_cnt (int rd)
{ // 1=>rd , 0=> wr
    return 0;   /* TBD-ELBA-REBASE: TOT diverged?? */
}

mem_addr_t
get_mem_base (void)
{
    return g_elba_state_pd->mempartition()->base();
}

mem_addr_t
get_mem_offset (const char *reg_name)
{
    return g_elba_state_pd->mempartition()->start_offset(reg_name);
}

mem_addr_t
get_mem_addr (const char *reg_name)
{
    return g_elba_state_pd->mempartition()->start_addr(reg_name);
}

uint32_t
get_mem_size_kb (const char *reg_name)
{
    return (g_elba_state_pd->mempartition()->size(reg_name) >> 10 );
}

mpartition_region_t *
get_hbm_region (char *reg_name)
{
    return g_elba_state_pd->mempartition()->region(reg_name);
}

mpartition_region_t *
get_hbm_region_by_address (uint64_t addr)
{
    return g_elba_state_pd->mempartition()->region_by_address(addr);
}

static sdk_ret_t
elba_hbm_llc_cache_init (asic_cfg_t *cfg)
{
    return SDK_RET_OK;
}

static sdk_ret_t
elba_hbm_p4_cache_init (asic_cfg_t *cfg)
{
    // Enable P4 Ingress (inst_id = 1)
    elb_pics_csr_t & ig_pics_csr =
        ELB_BLK_REG_MODEL_ACCESS(elb_pics_csr_t, 0, 1);
    ig_pics_csr.picc.cfg_cache_global.hash_mode(0);
    ig_pics_csr.picc.cfg_cache_global.write();

    // Enable P4 Egress (inst_id = 2)
    elb_pics_csr_t & eg_pics_csr =
        ELB_BLK_REG_MODEL_ACCESS(elb_pics_csr_t, 0, 2);
    eg_pics_csr.picc.cfg_cache_global.hash_mode(0);
    eg_pics_csr.picc.cfg_cache_global.write();

    return SDK_RET_OK;
}

static sdk_ret_t
elba_hbm_p4plus_cache_init (asic_cfg_t *cfg)
{
    // Enable P4Plus RXDMA (inst_id = 0)
    elb_pics_csr_t & rxdma_pics_csr = ELB_BLK_REG_MODEL_ACCESS(elb_pics_csr_t, 0, 0);
    // rxdma_pics_csr.picc.cfg_cache_global.bypass(global_bypass);
    rxdma_pics_csr.picc.cfg_cache_global.write();

    // Enable P4Plus TXDMA (inst_id = 3)
    elb_pics_csr_t & txdma_pics_csr = ELB_BLK_REG_MODEL_ACCESS(elb_pics_csr_t, 0, 3);
    txdma_pics_csr.picc.cfg_cache_global.hash_mode(0);
    txdma_pics_csr.picc.cfg_cache_global.write();

    return SDK_RET_OK;
}

sdk_ret_t
elba_hbm_cache_init (asic_cfg_t *cfg)
{
    sdk_ret_t   ret;

    ret = elba_hbm_llc_cache_init(cfg);
    assert(ret == SDK_RET_OK);

    ret = elba_hbm_p4_cache_init(cfg);
    assert(ret == SDK_RET_OK);

    ret = elba_hbm_p4plus_cache_init(cfg);
    assert(ret == SDK_RET_OK);

    return SDK_RET_OK;
}

sdk_ret_t
elba_hbm_cache_program_region (mpartition_region_t *reg, uint32_t inst_id,
                               uint32_t filter_idx, bool slave, bool master)
{

   if(!getenv("NO_INVF_FILTER")) {
    elb_pics_csr_t & pics_csr = ELB_BLK_REG_MODEL_ACCESS(elb_pics_csr_t, 0, inst_id);
    pics_csr.p4invf.filter_addr_lo.data[filter_idx].read();
    pics_csr.p4invf.filter_addr_lo.data[filter_idx].value(g_elba_state_pd->mempartition()->addr(reg->start_offset) >> 6); //28 MSB bits only
    pics_csr.p4invf.filter_addr_lo.data[filter_idx].write();

    pics_csr.p4invf.filter_addr_hi.data[filter_idx].read();
    pics_csr.p4invf.filter_addr_hi.data[filter_idx].value((g_elba_state_pd->mempartition()->addr(reg->start_offset) +
                                                 (reg->size)) >> 6);
    pics_csr.p4invf.filter_addr_hi.data[filter_idx].write();
    pics_csr.p4invf.filter_addr_ctl.value[filter_idx].read();
    pics_csr.p4invf.filter_addr_ctl.value[filter_idx].valid(1);
    pics_csr.p4invf.filter_addr_ctl.value[filter_idx].inval_send(1);
    pics_csr.p4invf.filter_addr_ctl.value[filter_idx].inval_receive(1);
    pics_csr.p4invf.filter_addr_ctl.value[filter_idx].use_cache(1);
    pics_csr.p4invf.filter_addr_ctl.value[filter_idx].read_access(1);
    pics_csr.p4invf.filter_addr_ctl.value[filter_idx].write_access(1);
    pics_csr.p4invf.filter_addr_ctl.value[filter_idx].write();
   }

   return SDK_RET_OK;
}

sdk_ret_t
elba_hbm_cache_program_db (mpartition_region_t *reg, uint32_t filter_idx)
{
    elb_wa_csr_t & wa_csr = ELB_BLK_REG_MODEL_ACCESS(elb_wa_csr_t, 0, 0);

    wa_csr.wainvf.filter_addr_lo.data[filter_idx].read();
    wa_csr.wainvf.filter_addr_lo.data[filter_idx].value(g_elba_state_pd->mempartition()->addr(reg->start_offset) >> 6); //28 MSB bits only
    wa_csr.wainvf.filter_addr_lo.data[filter_idx].write();

    wa_csr.wainvf.filter_addr_hi.data[filter_idx].read();
    wa_csr.wainvf.filter_addr_hi.data[filter_idx].value(
        (g_elba_state_pd->mempartition()->addr(reg->start_offset) +
             (reg->size)) >> 6);
    wa_csr.wainvf.filter_addr_hi.data[filter_idx].write();

    wa_csr.wainvf.filter_addr_ctl.value[filter_idx].read();
    wa_csr.wainvf.filter_addr_ctl.value[filter_idx].valid(1);
    wa_csr.wainvf.filter_addr_ctl.value[filter_idx].inval_receive(1);
    wa_csr.wainvf.filter_addr_ctl.value[filter_idx].inval_send(1);
    wa_csr.wainvf.filter_addr_ctl.value[filter_idx].use_cache(1);
    wa_csr.wainvf.filter_addr_ctl.value[filter_idx].read_access(1);
    wa_csr.wainvf.filter_addr_ctl.value[filter_idx].write_access(1);
    wa_csr.wainvf.filter_addr_ctl.value[filter_idx].write();

    return SDK_RET_OK;
}

sdk_ret_t
elba_hbm_cache_program_pcie (mpartition_region_t *reg, uint32_t filter_idx)
{
    elb_pxb_csr_t & pxb_csr = ELB_BLK_REG_MODEL_ACCESS(elb_pxb_csr_t, 0, 0);

    pxb_csr.invf.filter_addr_lo.data[filter_idx].read();
    pxb_csr.invf.filter_addr_lo.data[filter_idx].value(
        g_elba_state_pd->mempartition()->addr(reg->start_offset) >> 6); //28 MSB bits only
    pxb_csr.invf.filter_addr_lo.data[filter_idx].write();

    pxb_csr.invf.filter_addr_hi.data[filter_idx].read();
    pxb_csr.invf.filter_addr_hi.data[filter_idx].value(
        (g_elba_state_pd->mempartition()->addr(reg->start_offset) +
             (reg->size)) >> 6);
    pxb_csr.invf.filter_addr_hi.data[filter_idx].write();

    pxb_csr.invf.filter_addr_ctl.value[filter_idx].read();
    pxb_csr.invf.filter_addr_ctl.value[filter_idx].valid(1);
    pxb_csr.invf.filter_addr_ctl.value[filter_idx].inval_receive(1);
    pxb_csr.invf.filter_addr_ctl.value[filter_idx].inval_send(1);
    pxb_csr.invf.filter_addr_ctl.value[filter_idx].use_cache(1);
    pxb_csr.invf.filter_addr_ctl.value[filter_idx].read_access(1);
    pxb_csr.invf.filter_addr_ctl.value[filter_idx].write_access(1);
    pxb_csr.invf.filter_addr_ctl.value[filter_idx].write();

    return SDK_RET_OK;
}

sdk_ret_t
elba_hbm_cache_regions_init (void)
{
    mpartition_region_t *reg;
    uint32_t            p4ig_filter_idx = 0;
    uint32_t            p4eg_filter_idx = 0;
    uint32_t            p4plus_txdma_filter_idx = 0;
    uint32_t            p4plus_rxdma_filter_idx = 0;
    uint32_t            p4plus_pcie_filter_idx = 0;
    uint32_t            p4plus_db_filter_idx = 0;

    for (int i = 0; i < g_elba_state_pd->mempartition()->num_regions(); i++) {
        reg = g_elba_state_pd->mempartition()->region(i);
        if (is_region_cache_pipe_none(reg)) {
            continue;
        }

        if (is_region_cache_pipe_p4_ig(reg)) {
            SDK_TRACE_DEBUG("Programming %s to P4IG cache(region 1), "
                            "start=%lx size=%lu index=%u", reg->mem_reg_name,
                            g_elba_state_pd->mempartition()->addr(reg->start_offset),
                            reg->size, p4ig_filter_idx);
            elba_hbm_cache_program_region(reg, 1, p4ig_filter_idx, 1, 0);
            p4ig_filter_idx++;
        }

        if (is_region_cache_pipe_p4_eg(reg)) {
            SDK_TRACE_DEBUG("Programming %s to P4EG cache(region 2), "
                            "start=%lx size=%lu index=%u", reg->mem_reg_name,
                            g_elba_state_pd->mempartition()->addr(reg->start_offset),
                            reg->size, p4eg_filter_idx);
            elba_hbm_cache_program_region(reg, 2, p4eg_filter_idx, 1, 0);
            p4eg_filter_idx++;
        }

        if (is_region_cache_pipe_p4plus_txdma(reg)) {
            SDK_TRACE_DEBUG("Programming %s to P4PLUS TXDMA cache(region 3), "
                            "start=%lx size=%lu index=%u", reg->mem_reg_name,
                            g_elba_state_pd->mempartition()->addr(reg->start_offset),
                            reg->size, p4plus_txdma_filter_idx);
            elba_hbm_cache_program_region(reg, 3, p4plus_txdma_filter_idx, 1, 1);
            p4plus_txdma_filter_idx++;
        }

        if (is_region_cache_pipe_p4plus_rxdma(reg)) {
            SDK_TRACE_DEBUG("Programming %s to P4PLUS RXDMA cache(region 0), "
                            "start=%lx size=%lu index=%u", reg->mem_reg_name,
                            g_elba_state_pd->mempartition()->addr(reg->start_offset),
                            reg->size, p4plus_rxdma_filter_idx);
            elba_hbm_cache_program_region(reg, 0, p4plus_rxdma_filter_idx, 1, 1);
            p4plus_rxdma_filter_idx++;
        }

        if (is_region_cache_pipe_p4plus_pciedb(reg)) {
            SDK_TRACE_DEBUG("Programming %s to PCIE, "
                            "start=%lx size=%lu index=%u", reg->mem_reg_name,
                            g_elba_state_pd->mempartition()->addr(reg->start_offset),
                            reg->size, p4plus_pcie_filter_idx);
            elba_hbm_cache_program_pcie(reg, p4plus_pcie_filter_idx);
            p4plus_pcie_filter_idx++;

            SDK_TRACE_DEBUG("Programming %s to Doorbell, "
                            "start=%lx size=%lu index=%u", reg->mem_reg_name,
                            g_elba_state_pd->mempartition()->addr(reg->start_offset),
                            reg->size, p4plus_db_filter_idx);
            elba_hbm_cache_program_db(reg, p4plus_db_filter_idx);
            p4plus_db_filter_idx++;
        }
    }

    return SDK_RET_OK;
}

uint64_t
elba_hbm_timestamp_get (void)
{
    return 0;
}

sdk_ret_t
elba_tpc_bw_mon_rd_get (uint64_t *maxv, uint64_t *avrg)
{
    return SDK_RET_OK;
}

sdk_ret_t
elba_tpc_bw_mon_wr_get (uint64_t *maxv, uint64_t *avrg)
{
    return SDK_RET_OK;
}

sdk_ret_t
elba_rpc_bw_mon_rd_get (uint64_t *maxv, uint64_t *avrg)
{
    return SDK_RET_OK;
}

sdk_ret_t
elba_rpc_bw_mon_wr_get (uint64_t *maxv, uint64_t *avrg)
{
    return SDK_RET_OK;
}

sdk_ret_t
elba_ms_bw_mon_rd_get (uint64_t *maxv, uint64_t *avrg)
{
    return SDK_RET_OK;
}

sdk_ret_t
elba_ms_bw_mon_wr_get (uint64_t *maxv, uint64_t *avrg)
{
    return SDK_RET_OK;
}

sdk_ret_t
elba_pxb_bw_mon_rd_get (uint64_t *maxv, uint64_t *avrg)
{
    return SDK_RET_OK;
}

sdk_ret_t
elba_pxb_bw_mon_wr_get (uint64_t *maxv, uint64_t *avrg)
{
    return SDK_RET_OK;
}

static inline void
populate_hbm_bw (uint64_t max_rd, uint64_t max_wr,
                 uint64_t avg_rd, uint64_t avg_wr,
                 asic_hbm_bw_t *hbm_bw,
                 uint32_t num_bits, uint32_t window_size)
{
    hbm_bw->max.read  = (max_rd * (num_bits * elba_freq)/1000.0f) / window_size;
    hbm_bw->max.write = (max_wr * (num_bits * elba_freq)/1000.0f) / window_size;
    hbm_bw->avg.read  = (avg_rd * (num_bits * elba_freq)/1000.0f) / window_size;
    hbm_bw->avg.write = (avg_wr * (num_bits * elba_freq)/1000.0f) / window_size;

    SDK_TRACE_DEBUG("AVG_RD: %lf, AVG_WR: %lf, "
                    "MAX_RD: %lf, MAX_WR: %lf",
                    hbm_bw->avg.read, hbm_bw->avg.write,
                    hbm_bw->max.read, hbm_bw->max.write);
}

uint32_t
elba_freq_get (void)
{
    uint64_t prev_ts     = 0;
    uint64_t cur_ts      = 0;
    int      delay       = 5000;

    prev_ts = elba_hbm_timestamp_get();
    usleep(delay * 1000);
    cur_ts  = elba_hbm_timestamp_get();

    return ((((cur_ts - prev_ts) * 1.0) / delay) * 1000) / 1000000.0;
}

static inline uint64_t
elba_pb_axi_read_cnt (void)
{
    return 0;
}

static sdk_ret_t
elba_clear_hbm_bw (int val)
{
    return SDK_RET_OK;
}

static sdk_ret_t
elba_set_hbm_bw_window (uint32_t val)
{
    return SDK_RET_OK;
}

sdk_ret_t
elba_hbm_bw (uint32_t samples, uint32_t u_sleep, bool ms_pcie,
             asic_hbm_bw_t *hbm_bw_arr)
{
    uint64_t prev_ts     = 0;
    uint64_t cur_ts      = 0;
    uint64_t clk_diff    = 0;
    uint64_t max_rd      = 0;
    uint64_t avg_rd      = 0;
    uint64_t max_wr      = 0;
    uint64_t avg_wr      = 0;
    uint32_t num_bits    = 64 * 8;
    uint64_t rd_cnt      = 0;
    uint64_t wr_cnt      = 0;
    uint64_t prev_rd_cnt = 0;
    uint64_t prev_wr_cnt = 0;
    int      index       = 0;
    uint64_t cycle_per_nsec = 0;
    uint32_t window_size = 0xfff;
    asic_hbm_bw_t *hbm_bw = NULL;

    if (elba_freq == 0) {
        elba_freq = elba_freq_get();
        elba_set_hbm_bw_window(window_size);
        SDK_TRACE_DEBUG("HBM BW mon window size set to: 0x%x", window_size);
        SDK_TRACE_DEBUG("elba freq: %u", elba_freq);
    }

    elba_clear_hbm_bw(0);
    usleep(1000000);    // 1 sec
    elba_clear_hbm_bw(1);

    prev_ts = elba_hbm_timestamp_get();

    for (uint32_t i = 0; i < samples; ++i) {

        usleep(u_sleep);

        cur_ts = elba_hbm_timestamp_get();

        clk_diff = cur_ts - prev_ts;

        hbm_bw = &hbm_bw_arr[index++];
        hbm_bw->type = asic_block_t::ASIC_BLOCK_TXDMA;
        hbm_bw->clk_diff = clk_diff;
        elba_tpc_bw_mon_rd_get(&max_rd, &avg_rd);
        elba_tpc_bw_mon_wr_get(&max_wr, &avg_wr);
        SDK_TRACE_DEBUG("CLK_DIFF: %lu, TXDMA BW "
                        "AVG_RD: %lu, AVG_WR: %lu, "
                        "MAX_RD: %lu, MAX_WR: %lu",
                        clk_diff,
                        avg_rd, avg_wr,
                        max_rd, max_wr);
        populate_hbm_bw(max_rd, max_wr, avg_rd, avg_wr, hbm_bw, num_bits, window_size);

        hbm_bw = &hbm_bw_arr[index++];
        hbm_bw->type = asic_block_t::ASIC_BLOCK_RXDMA;
        hbm_bw->clk_diff = clk_diff;
        elba_rpc_bw_mon_rd_get(&max_rd, &avg_rd);
        elba_rpc_bw_mon_wr_get(&max_wr, &avg_wr);
        SDK_TRACE_DEBUG("CLK_DIFF: %lu, RXDMA BW "
                        "AVG_RD: %lu, AVG_WR: %lu, "
                        "MAX_RD: %lu, MAX_WR: %lu",
                        clk_diff,
                        avg_rd, avg_wr,
                        max_rd, max_wr);
        populate_hbm_bw(max_rd, max_wr, avg_rd, avg_wr, hbm_bw, num_bits, window_size);

        hbm_bw = &hbm_bw_arr[index++];
        hbm_bw->type = asic_block_t::ASIC_BLOCK_MS;
        hbm_bw->clk_diff = clk_diff;
        elba_ms_bw_mon_rd_get(&max_rd, &avg_rd);
        elba_ms_bw_mon_wr_get(&max_wr, &avg_wr);
        SDK_TRACE_DEBUG("CLK_DIFF: %lu, MS BW "
                        "AVG_RD: %lu, AVG_WR: %lu, "
                        "MAX_RD: %lu, MAX_WR: %lu",
                        clk_diff,
                        avg_rd, avg_wr,
                        max_rd, max_wr);
        populate_hbm_bw(max_rd, max_wr, avg_rd, avg_wr, hbm_bw, num_bits, window_size);

        hbm_bw = &hbm_bw_arr[index++];
        hbm_bw->type = asic_block_t::ASIC_BLOCK_PCIE;
        hbm_bw->clk_diff = clk_diff;
        elba_pxb_bw_mon_rd_get(&max_rd, &avg_rd);
        elba_pxb_bw_mon_wr_get(&max_wr, &avg_wr);
        SDK_TRACE_DEBUG("CLK_DIFF: %lu, PCIE BW "
                        "AVG_RD: %lu, AVG_WR: %lu, "
                        "MAX_RD: %lu, MAX_WR: %lu",
                        clk_diff,
                        avg_rd, avg_wr,
                        max_rd, max_wr);
        populate_hbm_bw(max_rd, max_wr, avg_rd, avg_wr, hbm_bw, num_bits, window_size);

        if (elba_freq != 0) {
            cycle_per_nsec = ((cur_ts - prev_ts) * 1000) / elba_freq;
        }

        hbm_bw = &hbm_bw_arr[index++];
        hbm_bw->type = asic_block_t::ASIC_BLOCK_PACKET_BUFFER;
        hbm_bw->clk_diff = clk_diff;
        rd_cnt = elba_pb_axi_read_cnt();
        wr_cnt = elb_nx_read_pb_axi_cnt(0);

        // avoid arithmetic exceptions
        if (cycle_per_nsec != 0) {
            avg_rd = ((rd_cnt - prev_rd_cnt) * num_bits) / cycle_per_nsec;
            avg_wr = ((wr_cnt - prev_wr_cnt) * num_bits) / cycle_per_nsec;
        }

        hbm_bw->avg.read  = avg_rd;
        hbm_bw->avg.write = avg_wr;
        SDK_TRACE_DEBUG("CLK_DIFF: %lu, PB BW "
                        "AVG_RD: %lu, AVG_WR: %lu",
                        clk_diff,
                        avg_rd, avg_wr);

        prev_rd_cnt = rd_cnt;
        prev_wr_cnt = wr_cnt;
        prev_ts     = cur_ts;
    }

    return SDK_RET_OK;
}

sdk_ret_t
elba_nx_set_llc_counters (uint32_t *data)
{
    return SDK_RET_OK;
}

sdk_ret_t
elba_nx_get_llc_counters (uint32_t *mask, uint32_t *rd_data)
{
    return SDK_RET_OK;
}

sdk_ret_t
elba_nx_setup_llc_counters (uint32_t mask)
{
    //
    //   e9[9] - Retry access
    //   e8[8] - Retry needed
    //   e7[7] - Eviction
    //   e6[6] - Cache maint op
    //   e5[5] - Partial write
    //   e4[4] - Cache miss
    //   e3[3] - Cache hit
    //   e2[2] - Scratchpad access
    //   e1[1] - Cache write
    //   e0[0] - Cache read
    //
    return SDK_RET_OK;
}

}    // namespace elba
}    // namespace platform
}    // namespace sdk
