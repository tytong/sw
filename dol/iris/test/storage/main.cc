#include <asm/byteorder.h>
#include <stdio.h>
#include <strings.h>
#include <string>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <vector>
#include <iostream>

#include "gflags/gflags.h"

#include "dol/iris/test/storage/hal_if.hpp"
#include "dol/iris/test/storage/qstate_if.hpp"
#include "dol/iris/test/storage/utils.hpp"
#include "dol/iris/test/storage/tests.hpp"
#include "dol/iris/test/storage/rdma.hpp"
#include "dol/iris/test/storage/queues.hpp"
#include "dol/iris/test/storage/nvme_dp.hpp"
#include "dol/iris/test/storage/nvme_dp_tests.hpp"
#include "dol/iris/test/storage/compression_test.hpp"
#include "dol/iris/test/storage/acc_scale_tests.hpp"
#include "nic/sdk/platform/utils/qstate_mgr.hpp"
#ifdef ELBA
#include "nic/sdk/platform/elba/elba_state.hpp"
#else
#include "nic/sdk/platform/capri/capri_state.hpp"
#endif

namespace queues {
void queues_shutdown();
}

DEFINE_uint64(hal_port, 50054, "TCP port of the HAL's gRPC server");
DEFINE_string(hal_ip, "localhost", "IP of HAL's gRPC server");
DEFINE_string(test_group, "", "Test group to run");
DEFINE_string(acc_scale_test, "", "Accelerator test(s) to run");
DEFINE_uint64(poll_interval, 60, "Polling interval in seconds");
DEFINE_uint64(long_poll_interval, 300,
              "Polling interval for longer running tests in seconds");
DEFINE_uint64(num_pdma_queues, 3,
              "number of queues for PDMA test (in power of 2)");

// Replication each accelerator chains this many times
DEFINE_uint64(acc_scale_chain_replica, 0,
              "Accelerator chain replication count (in power of 2)");

// Number of accelerator submissions in each chain for scale testing
DEFINE_uint64(acc_scale_submissions, 2,
              "number of accelerator queue submissions for scale testing (in power of 2)");

// Block size for accelerator scale testing
DEFINE_uint64(acc_scale_blk_size, 13,
              "Block size for accelerator scale testing (in power of 2)");
DEFINE_validator(acc_scale_blk_size, &tests::acc_scale_tests_blk_size_validate);

// Number of iterations for accelerator scale testing
DEFINE_uint64(acc_scale_iters, 1,
              "Number of iterations for accelerator scale testing (0 = infinite)");

// Verification method for accelerator scale testing
DEFINE_string(acc_scale_verify_method, "full",
              "Per-iteration verification method for accelerator scale testing: fast, or full");

DEFINE_bool(rtl, false,
            "Test being run under RTL");
DEFINE_bool(with_rtl_skipverify, false,
            "Test being run under RTL with --skipverify in effect");

// Number of iterations for NVME scale testing
DEFINE_uint64(nvme_scale_iters, 64,
              "Number of iterations for NVME scale testing (0 = infinite)");

DEFINE_bool(combined, false,
            "Combined run of storage and network tests");

DEFINE_string(nicmgr_config_file, "/sw/platform/src/app/nicmgrd/etc/accel.json",
              "nicmgr json configuration filename (full path)");

bool run_nvme_dp_tests = false;
bool run_nvme_dp_scale_tests = false;
bool run_nvme_dp_scale_perf = false;
bool run_unit_tests = false;
bool run_nvme_tests = false;
bool run_nvme_be_tests = false;
bool run_nvme_wrr_tests = false;
bool run_local_e2e_tests = false;
bool run_comp_tests = false;
bool run_comp_seq_tests = false;
bool run_xts_tests = false;
bool run_rdma_tests = false;
bool run_rdma_lif_override_tests = false;
bool run_rdma_xts_tests = false;
bool run_pdma_tests = false;
bool run_xts_perf_tests = false;
bool run_comp_perf_tests = false;
bool run_noc_perf_tests = false;
bool run_rdma_perf_tests = false;
bool run_rtl_sanity = false;
bool run_nicmgr_tests = false;
uint32_t run_acc_scale_tests_map = ACC_SCALE_TEST_NONE;

static vector<string> product_info_vec;

std::vector<tests::TestEntry> test_suite;

std::vector<tests::TestEntry> nvme_dp_tests = {
  {&tests::test_run_nvme_dp_write_cmd, "NVME Datapath write command", false},
  {&tests::test_run_nvme_dp_read_cmd, "NVME Datapath read command", false},
  {&tests::test_run_nvme_dp_e2e_test, "NVME Datapath E2E test", false},
};

std::vector<tests::TestEntry> nvme_dp_scale_tests = {
  {&tests::test_run_nvme_dp_write_scale, "NVME Datapath write scale", false},
  {&tests::test_run_nvme_dp_read_scale, "NVME Datapath read scale", false},
  {&tests::test_run_nvme_dp_e2e_scale, "NVME Datapath E2E scale", false},
};

std::vector<tests::TestEntry> nvme_dp_scale_perf = {
  {&tests::test_run_nvme_dp_write_perf, "NVME Datapath write performance", false},
  {&tests::test_run_nvme_dp_read_perf, "NVME Datapath read performance", false},
};

std::vector<tests::TestEntry> unit_tests = {
  {&tests::test_run_nvme_pvm_admin_cmd, "NVME->PVM Admin Cmd", false},
};

std::vector<tests::TestEntry> nvme_tests = {
  {&tests::test_run_nvme_pvm_read_cmd, "NVME->PVM Read Cmd", false},
  {&tests::test_run_nvme_pvm_write_cmd, "NVME->PVM Write Cmd", false},
  {&tests::test_run_nvme_pvm_hashing1, "NVME->PVM Hashing 1", false},
  {&tests::test_run_nvme_pvm_hashing2, "NVME->PVM Hashing 2", false},
  {&tests::test_run_pvm_nvme_admin_status, "PVM->NVME Admin Status", false},
  {&tests::test_run_pvm_nvme_read_status, "PVM->NVME Read Status", false},
  {&tests::test_run_pvm_nvme_write_status, "PVM->NVME Write Status", false},
};

std::vector<tests::TestEntry> nvme_be_tests = {
  {&tests::test_run_r2n_read_cmd, "R2N -> SSD Read Cmd", false},
  {&tests::test_run_r2n_write_cmd, "R2N -> SSD Write Cmd", false},
  {&tests::test_run_r2n_ssd_pri1, "R2N -> SSD Pri Cmd 1", false},
  {&tests::test_run_r2n_ssd_pri2, "R2N -> SSD Pri Cmd 2", false},
  {&tests::test_run_r2n_ssd_pri3, "R2N -> SSD Pri Cmd 3", false},
  {&tests::test_run_r2n_ssd_pri4, "R2N -> SSD Pri Cmd 4", false},
  {&tests::test_run_nvme_read_comp1, "PVM Local Read Comp 1", false},
  {&tests::test_run_nvme_write_comp1, "PVM Local Write Comp 1", false},
  {&tests::test_run_nvme_read_comp2, "PVM Local Read Comp 2", false},
  {&tests::test_run_nvme_write_comp2, "PVM Local Write Comp 2", false},
  {&tests::test_run_nvme_read_comp3, "PVM Local Read Comp 3", false},
  {&tests::test_run_nvme_write_comp3, "PVM Local Write Comp 3", false},
};

std::vector<tests::TestEntry> nvme_wrr_tests = {
  {&tests::test_run_nvme_be_wrr1, "NVME Backend WRR 1", false},
  {&tests::test_run_nvme_be_wrr2, "NVME Backend WRR 2", false},
  {&tests::test_run_nvme_be_wrr3, "NVME Backend WRR 3", false},
  {&tests::test_run_nvme_be_wrr4, "NVME Backend WRR 4", false},
  {&tests::test_run_nvme_be_wrr5, "NVME Backend WRR 5", false},
  {&tests::test_run_nvme_be_wrr6, "NVME Backend WRR 6", false},
};

std::vector<tests::TestEntry> local_e2e_tests = {
  {&tests::test_run_nvme_local_e2e1, "NVME Local Tgt E2E 1", false},
  {&tests::test_run_nvme_local_e2e2, "NVME Local Tgt E2E 2", false},
  {&tests::test_run_nvme_local_e2e3, "NVME Local Tgt E2E 3", false},
  {&tests::test_run_seq_write1, "Seq Local Tgt Write 1", false},
  {&tests::test_run_seq_write2, "Seq Local Tgt Write 2", false},
  {&tests::test_run_seq_write3, "Seq Local Tgt Write 3", false},
  {&tests::test_run_seq_write4, "Seq Local Tgt Write 4", false},
  {&tests::test_run_seq_read1, "Seq Local Tgt Read 1", false},
  {&tests::test_run_seq_read2, "Seq Local Tgt Read 2", false},
  {&tests::test_run_seq_read3, "Seq Local Tgt Read 3", false},
  {&tests::test_run_seq_read4, "Seq Local Tgt Read 4", false},
  {&tests::test_run_seq_e2e1, "Seq Local Tgt E2E 1", false},
  {&tests::test_run_seq_e2e2, "Seq Local Tgt E2E 2", false},
  {&tests::test_run_seq_e2e3, "Seq Local Tgt E2E 3", false},
  {&tests::test_run_seq_e2e4, "Seq Local Tgt E2E 4", false},
  {&tests::test_seq_e2e_xts_r2n1, "PDMA->XTS->R2N", false},

  // Last in series
  {&tests::xts_resync, "XTS rings resync", false},
};

std::vector<tests::TestEntry> comp_tests = {
  {&tests::compress_flat_64K_buf, "Compress Host->Host flat 64K buf", false},
  {&tests::decompress_to_flat_64K_buf, "Decompress Host->Host to flat 64K buf", false},
  {&tests::compress_odd_size_buf, "Compress Host->Host to flat odd size buf", false},
  {&tests::decompress_odd_size_buf, "Decompress Host->Host to flat odd size buf", false},
  {&tests::compress_host_sgl_to_host_sgl, "Compress Host->Host using SGLs", false},
  {&tests::decompress_host_sgl_to_host_sgl, "Decompress Host->Host using SGLs", false},
  {&tests::compress_flat_64K_buf_in_hbm, "Compress HBM->HBM flat 64K buf", false},
  {&tests::decompress_to_flat_64K_buf_in_hbm, "Decompress HBM->HBM to flat 64K buf", false},
  {&tests::compress_same_src_and_dst, "Compress with same src and dst", false},
  {&tests::compress_output_through_sequencer, "Compress and pull data from HBM through sequencer", false},
  {&tests::compress_dualq_flat_4K_buf, "Compress Host-Host flat 4K buf on hot and cold queues", false},
  {&tests::compress_dualq_flat_4K_buf_in_hbm, "Compress HBM-HBM flat 4K buf on hot and cold queues", false},
  {&tests::compress_output_encrypt_app_min_size, "Compress->XTS encrypt chaining: app min block size", false},
  {&tests::compress_output_encrypt_app_nominal_size, "Compress->XTS encrypt chaining: app nominal block size", false},
  {&tests::compress_output_encrypt_app_max_size, "Compress->XTS encrypt chaining: app max block size", false},
  {&tests::compress_clear_insert_header, "Compress without 'insert_header' bit set (HBM->HBM flat 64K buf)", false},
  {&tests::decompress_clear_header_present, "Decompress without 'header_present' bit set (HBM->HBM flat 64K buf)", false},
  // Enable when model is fixed.
  //{&tests::verify_integrity_for_gt64K, "Verify integrity calc for data size > 64K", false},

  // Last in series
  {&tests::compression_resync, "Compression rings resync", false},
  {&tests::xts_resync, "XTS rings resync", false},
};

std::vector<tests::TestEntry> comp_seq_tests = {
  {&tests::seq_compress_flat_64K_buf, "Sequencer Compress Host->Host flat 64K buf", false},
  {&tests::seq_decompress_to_flat_64K_buf, "Sequencer Decompress Host->Host to flat 64K buf", false},
  {&tests::seq_compress_host_sgl_to_host_sgl, "Sequencer Compress Host->Host using SGLs", false},
  {&tests::seq_decompress_host_sgl_to_host_sgl, "Sequencer Decompress Host->Host using SGLs", false},
  {&tests::seq_compress_flat_64K_buf_in_hbm, "Sequencer Compress HBM->HBM flat 64K buf", false},
  {&tests::seq_decompress_to_flat_64K_buf_in_hbm, "Sequencer Decompress HBM->HBM to flat 64K buf", false},
  {&tests::seq_compress_output_through_sequencer, "Sequencer Compress and pull data from HBM through sequencer", false},
  {&tests::seq_compress_dualq_flat_4K_buf, "Sequencer Compress Host-Host flat 4K buf on hot and cold queues", false},
  {&tests::seq_compress_dualq_flat_4K_buf_in_hbm, "Sequencer Compress HBM-HBM flat 4K buf on hot and cold queues", false},
  {&tests::seq_compress_same_src_and_dst, "Sequencer Compress with same src and dst", false},
  {&tests::seq_compress_output_encrypt_app_min_size, "Sequencer Compress->XTS encrypt chaining: app min block size", false},
  {&tests::seq_decrypt_output_decompress_len_update_none, "Sequencer XTS decrypt->Decompress chaining: len update none", false},
  {&tests::seq_compress_output_encrypt_app_nominal_size, "Sequencer Compress->XTS encrypt chaining: app nominal block size", false},
  {&tests::seq_decrypt_output_decompress_len_update_flat, "Sequencer XTS decrypt->Decompress chaining: len update flat", false},
  {&tests::seq_compress_output_encrypt_app_test_size, "Sequencer Compress->XTS encrypt chaining: app test block size", false},
  {&tests::seq_decrypt_output_decompress_len_update_sgl_src, "Sequencer XTS decrypt->Decompress chaining: len update SGL", false},
  {&tests::seq_compress_output_encrypt_app_max_size, "Sequencer Compress->XTS encrypt chaining: app max block size", false},
  {&tests::seq_decrypt_output_decompress_len_update_sgl_src_vec, "Sequencer XTS decrypt->Decompress chaining: len update SGL vector", false},
  {&tests::seq_compress_output_encrypt_force_comp_buf2_bypass, "Sequencer Compress->XTS encrypt chaining: force pad-only xfer", false},
  {&tests::seq_compress_output_hash_app_max_size, "Sequencer Compress->hash chaining: app max block size", false},
  {&tests::seq_chksum_decompress_last_app_blk, "Sequencer Checksum-decompress chaining: app max block size", false},
  {&tests::seq_compress_output_hash_app_test_size, "Sequencer Compress->hash chaining: app test block size", false},
  {&tests::seq_chksum_decompress_last_app_blk, "Sequencer Checksum-decompress chaining: app test block size", false},
  {&tests::seq_compress_output_hash_app_nominal_size, "Sequencer Compress->hash chaining: app nominal block size", false},
  {&tests::seq_chksum_decompress_last_app_blk, "Sequencer Checksum-decompress chaining: app nominal block size", false},

  // Last in series
  {&tests::compression_resync, "Compression rings resync", false},
  {&tests::xts_resync, "XTS rings resync", false},
};

std::vector<tests::TestEntry> comp_perf_tests = {
  {&tests::compress_flat_64K_buf, "Compress Host->Host flat 64K buf", false},
  {&tests::compress_flat_64K_buf_in_hbm, "Compress HBM->HBM flat 64K buf", false},
  {&tests::max_data_rate, "Test max data rate", false},
  {&tests::seq_max_data_rate, "Sequencer Test max data rate", false},

  // At EOS, RTL currently reports any CP/DC error status it has encountered
  // regardless of whether errors were intentionally induced, such as from the
  // tests below. Hence, these tests are moved here to exclude them from RTL sanity.
  {&tests::seq_compress_output_encrypt_force_uncomp_encrypt, "Sequencer Compress->XTS encrypt chaining: force encrypt of uncomp data", false},
  {&tests::seq_decrypt_output_decompress_len_update_sgl_src, "Sequencer XTS decrypt->Decompress chaining: len update SGL", false},

  // Last in series
  {&tests::compression_resync, "Compression rings resync", false},
  {&tests::xts_resync, "XTS rings resync", false},
};

std::vector<tests::TestEntry> acc_scale_tests = {
    {&tests::acc_scale_tests_push, "Accelerator chaining scale tests", false},

    // Last in series
    {&tests::compression_resync, "Compression rings resync", false},
    {&tests::xts_resync, "XTS rings resync", false},
};

std::vector<tests::TestEntry> rdma_tests = {
  {&tests::test_run_rdma_e2e_write, "E2E write over RDMA", false},
  {&tests::test_run_rdma_e2e_read, "E2E read over RDMA", false},
};

std::vector<tests::TestEntry> rdma_xts_tests = {
  {&tests::test_run_rdma_e2e_xts_write1, "E2E write over RDMA with XTS", false},
  {&tests::test_run_rdma_e2e_xts_read1, "E2E read over RDMA with XTS", false},

  // Last in series
  {&tests::xts_resync, "XTS rings resync", false},
};

std::vector<tests::TestEntry> rdma_lif_override_tests = {
  {&tests::test_run_rdma_e2e_write, "E2E write (pre LIF override)", false},
  {&tests::test_run_rdma_lif_override, "E2E read LIF override", false},
};

std::vector<tests::TestEntry> pdma_tests = {
  {&tests::test_run_seq_pdma_multi_xfers, "PDMA multiple transfers", false},
};

std::vector<tests::TestEntry> noc_perf_tests = {
  {&tests::xts_multi_blk_noc_stress_from_hbm_hw_chain, "NOC Perf with buffers on hbm HW chain", false},
  {&tests::xts_multi_blk_noc_stress_from_host_hw_chain, "NOC Perf with buffers on host HW chain", false},
  //{&tests::xts_multi_blk_noc_stress_from_hbm, "NOC Perf with buffers on hbm", false},
  //{&tests::xts_multi_blk_noc_stress_from_host, "NOC Perf with buffers on host", false},

  // Last in series
  {&tests::xts_resync, "XTS rings resync", false},
};

std::vector<tests::TestEntry> rdma_perf_tests = {
  {&tests::test_run_perf_rdma_e2e_write, "Perf e2e rdma write", false},
  {&tests::test_run_perf_rdma_e2e_write, "Perf e2e rdma write", false},
};

void sig_handler(int sig) {
  void *array[16];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 16);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(1);
}

/*
 * Return certain flags for use by utils module so it does not
 * have to link in tests.cc
 */
uint64_t
poll_interval(void)
{
    return FLAGS_poll_interval;
}

uint64_t
long_poll_interval(void)
{
    return FLAGS_long_poll_interval;
}

const vector<string>&
product_info_vec_get(void)
{
    return product_info_vec;
}


int common_setup() {
#ifdef __x86_64__
    assert(sdk::lib::pal_init(platform_type_t::PLATFORM_TYPE_SIM) ==
                sdk::lib::PAL_RET_OK);
    product_info_vec.push_back("Pensando Systems, Inc.");
    product_info_vec.push_back("DOL Simulation");
    product_info_vec.push_back("Implementation: software model");

    // Initialize host memory
    int ret;
    if(FLAGS_combined) {
      ret = init_host_mem_bhalf();
    } else {
      ret = init_host_mem();
    }
    if (ret < 0) {
      OFFL_FUNC_ERR("Host mem init failed (is model running?)");
      return -1;
    }
    OFFL_FUNC_INFO("Host mem initialized\n");

#else
    string fru_info;

#if 0 //!defined(APOLLO) && !defined(ARTEMIS)
    assert(sdk::lib::pal_init(platform_type_t::PLATFORM_TYPE_HAPS) ==
                sdk::lib::PAL_RET_OK);
#endif
    fru_info.clear()
    sdk::platform::readfrukey(BOARD_MANUFACTURER_KEY, fru_info);
    product_info_vec.push_back(fru_info);
    fru_info.clear()
    sdk::platform::readfrukey(BOARD_PRODUCTNAME_KEY, fru_info);
    product_info_vec.push_back(fru_info);
    product_info_vec.push_back("Implementation: hardware");
#endif
    product_info_vec.push_back("\n");

  // Initialize hal interface
  hal_if::init_hal_if();
  OFFL_FUNC_INFO("HAL client initialized\n");

  // Initialize storage hbm memory
  if (utils::hbm_buf_init() < 0) {
    OFFL_FUNC_ERR("HBM buf init failed (is model running?)");
    return -1;
  }
  OFFL_FUNC_INFO("HBM buf initialized\n");

  // Initialize model client
  if (lib_model_connect() < 0) {
    OFFL_FUNC_ERR("Failed to connect with model (is model running?)");
    return -1;
  }
  OFFL_FUNC_INFO("Model client initialized\n");

  return 0;
}

// Vector of supported accelerator scale test names

struct acc_scale_test_name_t {
  const std::string name;
  uint32_t          map_bit;
};
std::vector<acc_scale_test_name_t> acc_scale_test_names = {
    {"compress-encrypt", ACC_SCALE_TEST_COMP_ENCRYPT},
    {"decrypt-decompress", ACC_SCALE_TEST_DECRYPT_DECOMP},
    {"compress-hash", ACC_SCALE_TEST_COMP_HASH},
    {"checksum-decompress", ACC_SCALE_TEST_CHKSUM_DECOMP},
    {"encrypt-only", ACC_SCALE_TEST_ENCRYPT_ONLY},
};

void acc_scale_test_help_print(void)
{
    printf("Available accelerator scale tests: ");
    for (uint32_t i = 0; i < acc_scale_test_names.size(); i++) {
        printf("%s%s", acc_scale_test_names[i].name.c_str(),
               i == (acc_scale_test_names.size() - 1) ?
               "\n" : ", ");
    }
}

// Parse and convert the argument string into a bitmap of recognized tests
int acc_scale_test_str_parse(std::string& acc_scale_test_str)
{
    std::string elem;
    size_t      cur_pos, sep_pos, elem_len;
    uint32_t    i;

    cur_pos = 0;
    while (cur_pos != std::string::npos) {
        sep_pos = acc_scale_test_str.find_first_of(", \t", cur_pos);

        if (sep_pos == std::string::npos) {
            sep_pos = acc_scale_test_str.length();
        }

        elem_len = sep_pos - cur_pos;
        if (elem_len == 0) {
            break;
        }

        elem.assign(acc_scale_test_str.substr(cur_pos, elem_len));
        if (elem == "help") {
            acc_scale_test_help_print();

        } else {
            for (i = 0; i < acc_scale_test_names.size(); i++) {
                if (elem == acc_scale_test_names[i].name) {
                    run_acc_scale_tests_map |= acc_scale_test_names[i].map_bit;
                    break;
                }
            }

            if (i == acc_scale_test_names.size()) {
                acc_scale_test_help_print();
                printf("Usage: unrecognized acc_scale_test %s\n", elem.c_str());
                return -1;
            }
        }

        cur_pos = acc_scale_test_str.find_first_not_of(", \t", sep_pos);
    }

    return 0;
}

size_t tcid = 0;
int main(int argc, char**argv) {
  offl::logger::init(true);

  gflags::ParseCommandLineFlags(&argc, &argv, true);
  signal(SIGSEGV, sig_handler);

  std::cout << "Input - hal_port: "   << FLAGS_hal_port
            << "\nInput - hal_ip: "   << FLAGS_hal_ip
            << "\nTest group: "       << FLAGS_test_group
            << "\nPolling interval: " << FLAGS_poll_interval
            << "\n# PDMA queues (power of 2): " << FLAGS_num_pdma_queues
            << "\n# Accelerator chain replications (power of 2): " << FLAGS_acc_scale_chain_replica
            << "\n# Accelerator queue submissions (power of 2): " << FLAGS_acc_scale_submissions
            << "\nBlock size for accelerator scale testing (in power of 2): " << FLAGS_acc_scale_blk_size
            << "\n# Iterations for accelerator scale testing (0 = infinite): " << FLAGS_acc_scale_iters
            << "\nVerification method for accelerator scale testing: " << FLAGS_acc_scale_verify_method
            << "\nAccelerator scale tests: " << FLAGS_acc_scale_test
            << "\nRTL: " << FLAGS_rtl
            << "\nWith RTL --skipverify in effect: " << FLAGS_with_rtl_skipverify
            << "\nWith combined --combined in effect: " << FLAGS_combined
            << "\nnicmgr json config file: " << FLAGS_nicmgr_config_file
            << std::endl;

  // Set the test group based on flags. Default is to allow all.
  if (FLAGS_test_group == "") {
      run_unit_tests = true;
      run_nvme_tests = true;
      run_nvme_be_tests = true;
      run_nvme_wrr_tests = true;
      run_local_e2e_tests = true;
      run_comp_tests = true;
      run_comp_seq_tests = true;
      run_xts_tests = true;
      run_rdma_tests = false;
      run_rdma_lif_override_tests = false;
      run_rdma_xts_tests = false;
      run_xts_perf_tests = false;
      run_comp_perf_tests = false;
      run_pdma_tests = true;
      run_acc_scale_tests_map = ACC_SCALE_TEST_ALL;
  } else if (FLAGS_test_group == "rtl_sanity") {
      run_unit_tests = true;
      run_nvme_tests = true;
      run_nvme_be_tests = true;
      run_nvme_wrr_tests = false;		// Never enable this for RTL sanity
      run_local_e2e_tests = true;
      run_comp_tests = true;
      run_comp_seq_tests = true;
      run_xts_tests = true;
      run_rdma_tests = false;
      run_rdma_xts_tests = false;		// Enable after s/w debugging
      run_rdma_lif_override_tests = false;	// Enable after h/w model changes
      run_xts_perf_tests = false;		// Never enable this for RTL sanity
      run_comp_perf_tests = false;		// Never enable this for RTL sanity
      run_pdma_tests = false;			// Never enable this for RTL sanity
      run_acc_scale_tests_map = ACC_SCALE_TEST_ALL;
      run_rtl_sanity = true;
  } else if (FLAGS_test_group == "unit") {
      run_unit_tests = true;
  } else if (FLAGS_test_group == "nvme") {
      run_nvme_tests = true;
  } else if (FLAGS_test_group == "nvme_dp") {
      run_nvme_dp_tests = false;
  } else if (FLAGS_test_group == "nvme_dp_scale") {
      run_nvme_dp_scale_tests = false;
  } else if (FLAGS_test_group == "nvme_dp_scale_perf") {
      run_nvme_dp_scale_perf = false;
  } else if (FLAGS_test_group == "nvme_be") {
      run_nvme_be_tests = true;
      run_nvme_wrr_tests = true;
  } else if (FLAGS_test_group == "local_e2e") {
      run_local_e2e_tests = true;
  } else if (FLAGS_test_group == "comp") {
      run_comp_tests = true;
      run_comp_seq_tests = true;
  } else if (FLAGS_test_group == "xts") {
      run_xts_tests = true;
  } else if (FLAGS_test_group == "rdma") {
      run_rdma_tests = false;
      run_rdma_xts_tests = false;
      run_rdma_lif_override_tests = false;
  } else if (FLAGS_test_group == "xts_perf") {
      run_xts_perf_tests = true;
  } else if (FLAGS_test_group == "comp_perf") {
      run_comp_perf_tests = true;
  } else if (FLAGS_test_group == "pdma") {
      run_pdma_tests = true;
  } else if (FLAGS_test_group == "noc_perf") {
      run_noc_perf_tests = true;
  } else if (FLAGS_test_group == "rdma_perf") {
      run_rdma_perf_tests = false;
  } else if (FLAGS_test_group == "acc_scale") {
      if (FLAGS_acc_scale_test.empty()) {
          run_acc_scale_tests_map |= ACC_SCALE_TEST_ALL;
      } else {
          if (acc_scale_test_str_parse(FLAGS_acc_scale_test)) {
              return -1;
          }
      }
  } else if (FLAGS_test_group == "perf") {
      run_xts_perf_tests = true;
      run_noc_perf_tests = true;
      run_comp_perf_tests = true;
      run_rdma_perf_tests = false;
  } else if (FLAGS_test_group == "nicmgr") {

      // Limit to a few selected tests, particularly we want to
      // exclude any tests that require RDMA since the l2segments,
      // endpoints, etc., created for RDMA might cause conflict
      // with similar resources created by nicmgr.
      run_acc_scale_tests_map = ACC_SCALE_TEST_ALL;
      run_nicmgr_tests = true;
  } else {
    printf("Usage: ./storage_test [--hal_port <xxx>] "
           "[--test_group unit|nvme|nvme_be|local_e2e|comp|xts|rdma|pdma|acc_scale|rtl_sanity|perf|nvme_dp|nvme_dp_scale|nvme_dp_scale_perf] "
           " [--poll_interval <yyy>] \n");
    return -1;
  }

  if((run_nvme_dp_tests || run_nvme_dp_scale_tests || run_nvme_dp_scale_perf) && FLAGS_combined) {
    printf("ERROR: Cannot run NVMe DP Tests in combined sanity mode\n");
    return -1;
  }

  printf("Starting configuration: run_acc_scale_tests_map 0x%x\n",
         run_acc_scale_tests_map);
  if (common_setup() < 0)  {
    printf("Common setup failed\n");
    return 1;
  }
  printf("Commmon configuration completed \n");

  if (run_nvme_dp_tests || run_nvme_dp_scale_tests || run_nvme_dp_scale_perf) {
    if (nvme_dp::test_setup() < 0) {
      printf("Storage NVME DP test setup failed\n");
      return 1;
    }
  } else {
    if (tests::test_setup() < 0) {
      printf("Storage test setup failed\n");
      return 1;
    }
  }
  printf("Base configuration completed \n");

  if (tests::alloc_buffers() < 0) {
    printf("Storage test buffer allocation failed\n");
    return 1;
  }
  printf("Storage test buffer completed\n");

  printf("Going to init compression\n");
  tests::compression_init();
  printf("Compression configuration completed \n");

  printf("Going to init XTS\n");
  tests::xts_init();
  printf("XTS configuration completed \n");

  if (FLAGS_combined) {
    printf("RDMA configuration, NVME datapath initialization skipped - running in combined sanity mode\n");
  } else {
    if (rdma_init((run_nvme_dp_tests || run_nvme_dp_scale_tests || run_nvme_dp_scale_perf)) < 0) {
      printf("RDMA Setup failed\n");
      return 1;
    }
    printf("RDMA configuration completed \n");

    if (run_nvme_dp_tests || run_nvme_dp_scale_tests || run_nvme_dp_scale_perf) {
      if (nvme_dp::config() < 0) {
        printf("Storage NVME DP config failed\n");
        return 1;
      }
      printf("Storage NVME DP config succeded \n");
    }
  }

  tests::test_generic_eos_ignore();

  // Indicate to model that config is done
  CONFIG_DONE();

  printf("Forming test suite based on configuration\n");
  // Add unit tests
  if (run_unit_tests || run_nvme_tests) {
    for (size_t i = 0; i < unit_tests.size(); i++) {
      test_suite.push_back(unit_tests[i]);
    }
    printf("Added unit tests \n");
  }

  // Add NVME tests
  if (run_nvme_tests) {
    for (size_t i = 0; i < nvme_tests.size(); i++) {
      test_suite.push_back(nvme_tests[i]);
    }
    printf("Added nvme tests \n");
  }

  // Add nvme_dp tests
  if (run_nvme_dp_tests) {
    for (size_t i = 0; i < nvme_dp_tests.size(); i++) {
      test_suite.push_back(nvme_dp_tests[i]);
    }
    printf("Added nvme_dp tests \n");
  }

  // Add nvme_dp scale tests
  if (run_nvme_dp_scale_tests) {
    for (size_t i = 0; i < nvme_dp_scale_tests.size(); i++) {
      test_suite.push_back(nvme_dp_scale_tests[i]);
    }
    printf("Added nvme_dp scale tests \n");
  }

  // Add nvme_dp scale perf tests
  if (run_nvme_dp_scale_perf) {
    for (size_t i = 0; i < nvme_dp_scale_perf.size(); i++) {
      test_suite.push_back(nvme_dp_scale_perf[i]);
    }
    printf("Added nvme_dp scale perf tests \n");
  }

  // Add nvme_be tests
  if (run_nvme_be_tests) {
    for (size_t i = 0; i < nvme_be_tests.size(); i++) {
      test_suite.push_back(nvme_be_tests[i]);
    }
    printf("Added nvme_be tests \n");
  }

  // Add nvme_wrr tests
  if (run_nvme_wrr_tests) {
    for (size_t i = 0; i < nvme_wrr_tests.size(); i++) {
      test_suite.push_back(nvme_wrr_tests[i]);
    }
    printf("Added nvme_wrr tests \n");
  }

  // Add local_e2e tests
  if (run_local_e2e_tests) {
    for (size_t i = 0; i < local_e2e_tests.size(); i++) {
      test_suite.push_back(local_e2e_tests[i]);
    }
    printf("Added local_e2e tests \n");
  }

  // Add comp tests
  if (run_comp_tests) {
    for (size_t i = 0; i < comp_tests.size(); i++) {
      test_suite.push_back(comp_tests[i]);
    }
    printf("Added comp tests \n");
  }

  // Add comp_seq tests
  if (run_comp_seq_tests) {
    for (size_t i = 0; i < comp_seq_tests.size(); i++) {
      test_suite.push_back(comp_seq_tests[i]);
    }
    printf("Added comp_seq tests \n");
  }

  // Add comp_perf tests
  if (run_comp_perf_tests) {
    for (size_t i = 0; i < comp_perf_tests.size(); i++) {
      test_suite.push_back(comp_perf_tests[i]);
    }
    printf("Added comp perf tests \n");
  }

  // Add xts tests
  if (run_xts_tests) {
    tests::add_xts_tests(test_suite);
    printf("Added XTS tests \n");
  }

  // Add xts perf tests
  if (run_xts_perf_tests) {
    tests::add_xts_perf_tests(test_suite);
    printf("Added XTS perf tests \n");
  }

  // Add rdma tests
  if (run_rdma_tests) {
    for (size_t i = 0; i < rdma_tests.size(); i++) {
      test_suite.push_back(rdma_tests[i]);
    }
    printf("Added RDMA tests \n");
  }

  // Add rdma xts tests
  if (run_rdma_xts_tests) {
    for (size_t i = 0; i < rdma_xts_tests.size(); i++) {
      test_suite.push_back(rdma_xts_tests[i]);
    }
    printf("Added RDMA XTS tests \n");
  }

  // Add rdma lif override tests
  if (run_rdma_lif_override_tests) {
    for (size_t i = 0; i < rdma_lif_override_tests.size(); i++) {
      test_suite.push_back(rdma_lif_override_tests[i]);
    }
    printf("Added RDMA LIF override tests \n");
  }

  // Add pdma tests
  if (run_pdma_tests) {
    for (size_t i = 0; i < pdma_tests.size(); i++) {
      test_suite.push_back(pdma_tests[i]);
    }
    printf("Added PDMA tests \n");
  }

  // Add NOC perf tests
  if (run_noc_perf_tests) {
    for (size_t i = 0; i < noc_perf_tests.size(); i++) {
      test_suite.push_back(noc_perf_tests[i]);
    }
    printf("Added NOC Perf tests \n");
  }

  // Add RDMA perf tests
  if (run_rdma_perf_tests) {
    for (size_t i = 0; i < rdma_perf_tests.size(); i++) {
      test_suite.push_back(rdma_perf_tests[i]);
    }
    printf("Added rdma Perf tests \n");
  }

  // Add accelerator scale tests
  if (run_acc_scale_tests_map) {
    for (size_t i = 0; i < acc_scale_tests.size(); i++) {
      test_suite.push_back(acc_scale_tests[i]);
    }
    printf("Added accelerator chaining scale tests \n");
  }

  printf("Formed test suite with %d cases \n", (int) test_suite.size());

  printf("Running test cases \n");
  for (tcid = 0; tcid < test_suite.size(); tcid++) {
    struct timeval start, end;
    gettimeofday(&start, NULL);
    printf(" Starting test #: %d name: %s \n", (int) tcid, test_suite[tcid].test_name.c_str());

    TESTCASE_BEGIN(tcid, 0);
    if (test_suite[tcid].test_fn() < 0)
      test_suite[tcid].test_succeded = false;
    else
      test_suite[tcid].test_succeded = true;
    TESTCASE_END(tcid, 0);

    gettimeofday(&end, NULL);
    printf(" Finished test #: %d name: %s status %d time %d \n", (int) tcid, test_suite[tcid].test_name.c_str(),
           test_suite[tcid].test_succeded, (int) (end.tv_sec - start.tv_sec));
  }

  if (run_rtl_sanity) {
    rdma_shutdown();
  }
  printf("Test case run complete, shutting down queues \n");
  queues::queues_shutdown();

  printf("\nConsolidated Test Report \n");
  printf("--------------------------------------------------------------\n");
  printf("Number\t\tName\t\t\tResult\n");
  printf("--------------------------------------------------------------\n");

  int rc = 0;
  for (size_t i = 0; i < test_suite.size(); i++) {
    printf("%lu\t", i+1);
    printf("%s\t\t\t\t", test_suite[i].test_name.c_str());
    printf("%s\n", test_suite[i].test_succeded ? "Success" : "Failure");
    if (!test_suite[i].test_succeded) rc = 1;
  }
  if (rc != 0) {
    printf("\nOverall Report: FAILURE \n");
  } else {
    printf("\nOverall Report: SUCCESS \n");
  }
  if (offl::logger::logger()) {
      offl::logger::logger()->flush();
  }
  fflush(stdout);
  if (rc != 0) return rc;
  printf("exiting successfully \n");
  fflush(stdout);
  exit(0);
}
