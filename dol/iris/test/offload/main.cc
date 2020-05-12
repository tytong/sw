
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <execinfo.h>
#include <sys/time.h>
#include "gflags/gflags.h"
#include "hal_if.hpp"
#include "utils.hpp"
#include "tests.hpp"
#include "logger.hpp"
#include "nic/sdk/platform/utils/qstate_mgr.hpp"
#ifdef ELBA
#include "nic/sdk/platform/elba/elba_state.hpp"
#else
#include "nic/sdk/platform/capri/capri_state.hpp"
#endif
#include "nic/sdk/platform/fru/fru.hpp"
#include "crypto_rsa_testvec.hpp"
#include "crypto_ecdsa_testvec.hpp"
#include "crypto_drbg_testvec.hpp"
#include "crypto_sha_testvec.hpp"
#include "crypto_aes_testvec.hpp"
#include "crypto_symm.hpp"
#include "crypto_asym.hpp"
#include "crypto_drbg.hpp"
#include "crypto_sha.hpp"
#include "crypto_aes.hpp"

DEFINE_uint64(hal_port, 50054, "TCP port of the HAL's gRPC server");
DEFINE_string(hal_ip, "localhost", "IP of HAL's gRPC server");
DEFINE_string(test_group, "", "Test group to run");
DEFINE_uint64(poll_interval, 60, "Polling interval in seconds");
DEFINE_uint64(long_poll_interval, 300,
              "Polling interval for longer running tests in seconds");
#ifdef __x86_64__
DEFINE_string(script_dir, "/sw/nic/third-party/nist-cavp",
              "Script directory path)");
#ifdef ELBA
DEFINE_string(engine_path, "/sw/nic/build/x86_64/iris/elba/lib/libtls_pse.so",
              "SSL engine library full path");
#else
DEFINE_string(engine_path, "/sw/nic/build/x86_64/iris/capri/lib/libtls_pse.so",
              "SSL engine library full path");
#endif
DEFINE_string(nicmgr_config_file, "/sw/platform/src/app/nicmgrd/etc/accel.json",
              "nicmgr json configuration filename (full path)");
#else
DEFINE_string(script_dir, "/data/third-party/nist-cavp",
              "Script directory path)");
DEFINE_string(engine_path, "/nic/lib/libtls_pse.so",
              "SSL engine library full path");
DEFINE_string(nicmgr_config_file, "/platform/etc/nicmgrd/accel.json",
              "nicmgr json configuration filename (full path)");
#endif

/*
 * Satisfy extern requirements for sstorage libs
 */
DEFINE_bool(rtl, false,
            "Test being run under RTL");
DEFINE_bool(with_rtl_skipverify, false,
            "Test being run under RTL with --skipverify in effect");
DEFINE_uint64(num_pdma_queues, 3,
              "number of queues for PDMA test (in power of 2)");
DEFINE_uint64(acc_scale_chain_replica, 0,
              "Accelerator chain replication count (in power of 2)");

DEFINE_uint64(acc_scale_submissions, 2,
              "number of accelerator queue submissions for scale testing (in power of 2)");
DEFINE_uint64(acc_scale_iters, 1,
              "Number of iterations for accelerator scale testing (0 = infinite)");
DEFINE_uint64(nvme_scale_iters, 64,
              "Number of iterations for NVME scale testing (0 = infinite)");
bool run_nicmgr_tests;
bool run_pdma_tests;
bool run_rsa_testvectors;
bool run_ecdsa_testvectors;
bool run_drbg_testvectors;
bool run_sha_testvectors;
bool run_aes_testvectors;
uint32_t run_acc_scale_tests_map;

/*
 * Product information, served as header information for
 * testvector response output files.
 */
static vector<string> product_info_vec;

/*
 * test case ID (legacy usage from storage)
 */
size_t tcid;

class test_entry_t
{
public:
    test_entry_t(const string& test_name,
                 std::function<bool(void *)> test_fn,
                 void *test_param) :
        test_name(test_name),
        test_fn(test_fn),
        test_param(test_param),
        test_success(false)
    {
    }

    const string                test_name;
    std::function<bool(void *)> test_fn;
    void                        *test_param;
    bool                        test_success;
};

/*
 * RSA testvectors
 */
typedef struct {
    string                      testvec_fname;
    crypto_rsa::rsa_key_create_type_t key_create_type;
    int                         pad_mode;
} rsa_vector_entry_t;

const static vector<rsa_vector_entry_t> rsa_testvectors =
{
    {"rsa-testvectors/SigGen15_186-3.req",
     crypto_rsa::RSA_KEY_CREATE_SIGN, RSA_PKCS1_PADDING},

    // For signature verification, we want to produce the ciphered output
    // so the encryption method is used.
    {"rsa-testvectors/SigVer15_186-3.req",
      crypto_rsa::RSA_KEY_CREATE_ENCRYPT, RSA_PKCS1_PADDING},

    // Late add-ons from validation vendor for modulus 4K
    {"rsa-testvectors/SigVer15_186-2.req",
      crypto_rsa::RSA_KEY_CREATE_ENCRYPT, RSA_PKCS1_PADDING},

    // Probabilistic Signature Scheme (PSS)
    {"rsa-testvectors/SigGenPSS_186-3.req",
     crypto_rsa::RSA_KEY_CREATE_SIGN, RSA_PKCS1_PSS_PADDING},

    {"rsa-testvectors/SigVerPSS_186-3.req",
     crypto_rsa::RSA_KEY_CREATE_ENCRYPT, RSA_PKCS1_PSS_PADDING},

    // Late add-ons from validation vendor for modulus 4K
    {"rsa-testvectors/SigVerPSS_186-2.req",
     crypto_rsa::RSA_KEY_CREATE_ENCRYPT, RSA_PKCS1_PSS_PADDING},

#ifdef OPENSSL_WITH_TRUNCATED_SHA_SUPPORT
    {"rsa-testvectors/SigGen15_186-3_TruncatedSHAs.req",
     crypto_rsa::RSA_KEY_CREATE_SIGN, RSA_PKCS1_PADDING},
    {"rsa-testvectors/SigVer15_186-3_TruncatedSHAs.req",
     crypto_rsa::RSA_KEY_CREATE_ENCRYPT, RSA_PKCS1_PADDING},
#endif
};

/*
 * ECDSA testvectors
 */
typedef struct {
    string                      testvec_fname;
    crypto_ecdsa::ecdsa_key_create_type_t key_create_type;
} ecdsa_vector_entry_t;

const static vector<ecdsa_vector_entry_t> ecdsa_testvectors =
{
    {"ecdsa-testvectors/SigGen.req",
     crypto_ecdsa::ECDSA_KEY_CREATE_SIGN},

    {"ecdsa-testvectors/SigGenComponent.req",
     crypto_ecdsa::ECDSA_KEY_CREATE_SIGN_COMPONENT},

    {"ecdsa-testvectors/SigVer.req",
      crypto_ecdsa::ECDSA_KEY_CREATE_VERIFY},
};

/*
 * DRBG testvectors
 */
typedef struct {
    string                      testvec_fname;
} drbg_vector_entry_t;

const static vector<drbg_vector_entry_t> drbg_testvectors =
{
    {"drbg-testvectors/Hash_DRBG.req"},
};

/*
 * SHA testvectors
 */
typedef struct {
    string                          testvec_fname;
    crypto_symm::crypto_symm_type_t crypto_symm_type;
} sha_vector_entry_t;

const static vector<sha_vector_entry_t> sha_testvectors =
{
    {"sha-testvectors/SHA1ShortMsg.req",
     crypto_symm::CRYPTO_SYMM_TYPE_SHA},

    {"sha-testvectors/SHA224ShortMsg.req",
     crypto_symm::CRYPTO_SYMM_TYPE_SHA},

    {"sha-testvectors/SHA256ShortMsg.req",
     crypto_symm::CRYPTO_SYMM_TYPE_SHA},

    {"sha-testvectors/SHA384ShortMsg.req",
     crypto_symm::CRYPTO_SYMM_TYPE_SHA},

    {"sha-testvectors/SHA512ShortMsg.req",
     crypto_symm::CRYPTO_SYMM_TYPE_SHA},

#ifndef __x86_64__

    /*
     * Skip these on model to reduce runtime
     */
    {"sha-testvectors/SHA1LongMsg.req",
     crypto_symm::CRYPTO_SYMM_TYPE_SHA},

    {"sha-testvectors/SHA224LongMsg.req",
     crypto_symm::CRYPTO_SYMM_TYPE_SHA},

    {"sha-testvectors/SHA256LongMsg.req",
     crypto_symm::CRYPTO_SYMM_TYPE_SHA},

    {"sha-testvectors/SHA384LongMsg.req",
     crypto_symm::CRYPTO_SYMM_TYPE_SHA},

    {"sha-testvectors/SHA512LongMsg.req",
     crypto_symm::CRYPTO_SYMM_TYPE_SHA},
#endif

    {"sha-testvectors/SHA1Monte.req",
     crypto_symm::CRYPTO_SYMM_TYPE_SHA},

    {"sha-testvectors/SHA224Monte.req",
     crypto_symm::CRYPTO_SYMM_TYPE_SHA},

    {"sha-testvectors/SHA256Monte.req",
     crypto_symm::CRYPTO_SYMM_TYPE_SHA},

    {"sha-testvectors/SHA384Monte.req",
     crypto_symm::CRYPTO_SYMM_TYPE_SHA},

    {"sha-testvectors/SHA512Monte.req",
     crypto_symm::CRYPTO_SYMM_TYPE_SHA},
};

/*
 * AES testvectors
 */
typedef struct {
    string                          testvec_fname;
    crypto_symm::crypto_symm_type_t crypto_symm_type;
    crypto_aes::crypto_aes_montecarlo_t montecarlo_type;
} aes_vector_entry_t;

const static vector<aes_vector_entry_t> aes_testvectors =
{
    {"aes-testvectors/CBCGFSbox128.req",
     crypto_symm::CRYPTO_SYMM_TYPE_AES_CBC,
     crypto_aes::CRYPTO_AES_NOT_MONTECARLO},

    {"aes-testvectors/CBCGFSbox256.req",
     crypto_symm::CRYPTO_SYMM_TYPE_AES_CBC,
     crypto_aes::CRYPTO_AES_NOT_MONTECARLO},

    {"aes-testvectors/CBCKeySbox128.req",
     crypto_symm::CRYPTO_SYMM_TYPE_AES_CBC,
     crypto_aes::CRYPTO_AES_NOT_MONTECARLO},

    {"aes-testvectors/CBCKeySbox256.req",
     crypto_symm::CRYPTO_SYMM_TYPE_AES_CBC,
     crypto_aes::CRYPTO_AES_NOT_MONTECARLO},

    {"aes-testvectors/CBCVarKey128.req",
     crypto_symm::CRYPTO_SYMM_TYPE_AES_CBC,
     crypto_aes::CRYPTO_AES_NOT_MONTECARLO},

    {"aes-testvectors/CBCVarKey256.req",
     crypto_symm::CRYPTO_SYMM_TYPE_AES_CBC,
     crypto_aes::CRYPTO_AES_NOT_MONTECARLO},

    {"aes-testvectors/CBCVarTxt128.req",
     crypto_symm::CRYPTO_SYMM_TYPE_AES_CBC,
     crypto_aes::CRYPTO_AES_NOT_MONTECARLO},

    {"aes-testvectors/CBCVarTxt256.req",
     crypto_symm::CRYPTO_SYMM_TYPE_AES_CBC,
     crypto_aes::CRYPTO_AES_NOT_MONTECARLO},

    {"aes-testvectors/CBCMMT128.req",
     crypto_symm::CRYPTO_SYMM_TYPE_AES_CBC,
     crypto_aes::CRYPTO_AES_NOT_MONTECARLO},

    {"aes-testvectors/CBCMMT256.req",
     crypto_symm::CRYPTO_SYMM_TYPE_AES_CBC,
     crypto_aes::CRYPTO_AES_NOT_MONTECARLO},

    {"aes-testvectors/CBCMCT128.req",
     crypto_symm::CRYPTO_SYMM_TYPE_AES_CBC,
     crypto_aes::CRYPTO_AES_MONTECARLO_CBC},

    {"aes-testvectors/CBCMCT256.req",
     crypto_symm::CRYPTO_SYMM_TYPE_AES_CBC,
     crypto_aes::CRYPTO_AES_MONTECARLO_CBC},
};

/*
 * Runnable test suite
 */
static vector<test_entry_t>     test_suite;

void
sig_handler(int sig)
{
    void        *array[16];
    size_t      size;

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

static int
sdk_trace_cb (uint32_t mod_id, sdk_trace_level_e trace_level,
              const char *format, ...)
{
    char       logbuf[1024];
    va_list    args;

    va_start(args, format);
    vsnprintf(logbuf, sizeof(logbuf), format, args);
    OFFL_LOG_DEBUG("{}", logbuf);
    va_end(args);
    return 0;
}

int
common_setup(void)
{
    sdk::lib::logger::init(sdk_trace_cb);

#ifdef __x86_64__
    assert(sdk::lib::pal_init(platform_type_t::PLATFORM_TYPE_SIM) ==
                sdk::lib::PAL_RET_OK);
    product_info_vec.push_back("Pensando Systems, Inc.");
    product_info_vec.push_back("DOL Simulation");
    product_info_vec.push_back("Implementation: software model");

    if (init_host_mem() < 0) {
        OFFL_FUNC_ERR("Host mem init failed (is model running?)");
        return -1;
    }
    OFFL_FUNC_INFO("Host mem initialized");

#else
    string fru_info;

#if 0 //!defined(APOLLO) && !defined(ARTEMIS)
    assert(sdk::lib::pal_init(platform_type_t::PLATFORM_TYPE_HAPS) ==
                sdk::lib::PAL_RET_OK);
#endif
    fru_info.clear();
    sdk::platform::readfrukey(BOARD_MANUFACTURER_KEY, fru_info);
    product_info_vec.push_back(fru_info);
    fru_info.clear();
    sdk::platform::readfrukey(BOARD_PRODUCTNAME_KEY, fru_info);
    product_info_vec.push_back(fru_info);
    product_info_vec.push_back("Implementation: hardware");
#endif
    product_info_vec.push_back("\n");

    // Initialize hal interface
    hal_if::init_hal_if();
    OFFL_FUNC_INFO("HAL client initialized");

    // Initialize storage hbm memory
    if (utils::hbm_buf_init() < 0) {
        OFFL_FUNC_ERR("HBM buf init failed (is model running?)");
        return -1;
    }
    OFFL_FUNC_INFO("HBM buf initialized");

#ifdef __x86_64__

    // Initialize model client
    if (lib_model_connect() < 0) {
        OFFL_FUNC_ERR("Failed to connect with model (is model running?)");
       return -1;
    }
    OFFL_FUNC_INFO("Model client initialized");
#endif

    crypto_drbg::init();
    crypto_symm::init(FLAGS_engine_path.c_str());
    crypto_asym::init(FLAGS_engine_path.c_str());
    tests::test_generic_eos_ignore();
    return 0;
}

/*
 * Driver for RSA testvectors
 */
static bool
rsa_testvectors_run(void *test_param)
{
    vector<rsa_vector_entry_t>  *testvectors =
                      static_cast<vector<rsa_vector_entry_t> *>(test_param);
    uint32_t        failure_count;

    /*
     * Run the vectors with the specified mem_type
     */
    auto testvectors_run = [](const rsa_vector_entry_t& entry,
                              dp_mem_type_t mem_type) -> bool
    {
        crypto_rsa::rsa_testvec_params_t          testvec_params;
        crypto_rsa::rsa_testvec_pre_push_params_t pre_params;
        crypto_rsa::rsa_testvec_push_params_t     push_params;
        crypto_rsa::rsa_testvec_t                 *rsa_testvec;
        offload_base_params_t                     base_params;
        bool                                      success;

        testvec_params.key_create_type(entry.key_create_type).
                       pad_mode(entry.pad_mode).
                       d_mem_type(mem_type).
                       e_mem_type(mem_type).
                       n_mem_type(mem_type).
                       msg_mem_type(mem_type).
                       sig_mem_type(mem_type).
                       dma_desc_mem_type(mem_type).
                       status_mem_type(mem_type);
        OFFL_LOG_INFO("Running RSA vector {} with mem_type {}",
                      entry.testvec_fname, mem_type);
        rsa_testvec = new crypto_rsa::rsa_testvec_t(
                          testvec_params.base_params(base_params));
        success = rsa_testvec->pre_push(pre_params.scripts_dir(FLAGS_script_dir).
                                                   testvec_fname(entry.testvec_fname).
                                                   rsp_fname_suffix(mem_type == DP_MEM_TYPE_HBM ?
                                                                    "" : "host"));
        if (success) {
            success = rsa_testvec->push(push_params.rsa_ring(crypto_asym::asym_ring));
        }
        if (success) {
            success = rsa_testvec->post_push();
        }
        if (success) {
            success = rsa_testvec->full_verify();
        }
        rsa_testvec->rsp_file_output();
        delete rsa_testvec;
        return success;
    };

    failure_count = 0;
    for (uint32_t i = 0; i < testvectors->size(); i++) {
        const rsa_vector_entry_t& entry = testvectors->at(i);

        /*
         * 1st run with HBM
         */
        if (!testvectors_run(entry, DP_MEM_TYPE_HBM)) {
            failure_count++;
        }

#ifdef __x86_64__

        /*
         * 2nd run with host mem
         */
        if (!testvectors_run(entry, DP_MEM_TYPE_HOST_MEM)) {
            failure_count++;
        }
#endif
    }

    return failure_count == 0;
}

/*
 * Driver for ECDSA testvectors
 */
static bool
ecdsa_testvectors_run(void *test_param)
{
    vector<ecdsa_vector_entry_t>  *testvectors =
                      static_cast<vector<ecdsa_vector_entry_t> *>(test_param);
    uint32_t        failure_count;

    /*
     * Run the vectors with the specified mem_type
     */
    auto testvectors_run = [](const ecdsa_vector_entry_t& entry,
                              dp_mem_type_t mem_type) -> bool
    {
        crypto_ecdsa::ecdsa_testvec_params_t          testvec_params;
        crypto_ecdsa::ecdsa_testvec_pre_push_params_t pre_params;
        crypto_ecdsa::ecdsa_testvec_push_params_t     push_params;
        crypto_ecdsa::ecdsa_testvec_t                 *ecdsa_testvec;
        offload_base_params_t                     base_params;
        bool                                      success;

        testvec_params.key_create_type(entry.key_create_type).
                       q_mem_type(mem_type).
                       d_mem_type(mem_type).
                       k_mem_type(mem_type).
                       msg_mem_type(mem_type).
                       sig_mem_type(mem_type).
                       dma_desc_mem_type(mem_type).
                       status_mem_type(mem_type);
        OFFL_LOG_INFO("Running ECDSA vector {} with mem_type {}",
                      entry.testvec_fname, mem_type);
        ecdsa_testvec = new crypto_ecdsa::ecdsa_testvec_t(
                          testvec_params.base_params(base_params));
        success = ecdsa_testvec->pre_push(pre_params.scripts_dir(FLAGS_script_dir).
                                                     testvec_fname(entry.testvec_fname).
                                                     rsp_fname_suffix(mem_type == DP_MEM_TYPE_HBM ?
                                                                      "" : "host"));
        if (success) {
            success = ecdsa_testvec->push(push_params.ecdsa_ring(crypto_asym::asym_ring));
        }
        if (success) {
            success = ecdsa_testvec->post_push();
        }
        if (success) {
            success = ecdsa_testvec->full_verify();
        }
        ecdsa_testvec->rsp_file_output();
        delete ecdsa_testvec;
        return success;
    };

    failure_count = 0;
    for (uint32_t i = 0; i < testvectors->size(); i++) {
        const ecdsa_vector_entry_t& entry = testvectors->at(i);

        /*
         * 1st run with HBM
         */
        if (!testvectors_run(entry, DP_MEM_TYPE_HBM)) {
            failure_count++;
        }

#ifdef __x86_64__

        /*
         * 2nd run with host mem
         */
        if (!testvectors_run(entry, DP_MEM_TYPE_HOST_MEM)) {
            failure_count++;
        }
#endif
    }

    return failure_count == 0;
}

/*
 * Driver for DRBG testvectors
 */
static bool
drbg_testvectors_run(void *test_param)
{
    vector<drbg_vector_entry_t>  *testvectors =
                       static_cast<vector<drbg_vector_entry_t> *>(test_param);
    uint32_t        failure_count;

    /*
     * Run the vectors with the specified mem_type
     */
    auto testvectors_run = [](const drbg_vector_entry_t& entry,
                              crypto_drbg::drbg_instance_t inst) -> bool
    {
        crypto_drbg::drbg_testvec_params_t        testvec_params;
        crypto_drbg::drbg_testvec_pre_push_params_t pre_params;
        crypto_drbg::drbg_testvec_push_params_t   push_params;
        crypto_drbg::drbg_testvec_t               *drbg_testvec;
        offload_base_params_t                     base_params;
        bool                                      success;

        OFFL_LOG_INFO("Running DRBG vector {}", entry.testvec_fname);
        drbg_testvec = new crypto_drbg::drbg_testvec_t(
                           testvec_params.base_params(base_params));
        success = drbg_testvec->pre_push(pre_params.scripts_dir(FLAGS_script_dir).
                                                    testvec_fname(entry.testvec_fname).
                                                    instance(inst));
        if (success) {
            success = drbg_testvec->push(push_params);
        }
        if (success) {
            success = drbg_testvec->post_push();
        }
        if (success) {
            success = drbg_testvec->full_verify();
        }
        drbg_testvec->rsp_file_output();

        delete drbg_testvec;
        return success;
    };

    failure_count = 0;
    for (uint32_t i = 0; i < testvectors->size(); i++) {
        const drbg_vector_entry_t& entry = testvectors->at(i);

        for (int inst = crypto_drbg::DRBG_INSTANCE0;
             inst < crypto_drbg::DRBG_INSTANCE_MAX;
             inst++) {

            if (!testvectors_run(entry, (crypto_drbg::drbg_instance_t)inst)) {
                failure_count++;
            }
        }
    }

    return failure_count == 0;
}

/*
 * Driver for SHA testvectors
 */
static bool
sha_testvectors_run(void *test_param)
{
    vector<sha_vector_entry_t>  *testvectors =
                      static_cast<vector<sha_vector_entry_t> *>(test_param);
    uint32_t        failure_count;

    /*
     * Run the vectors with the specified mem_type
     */
    auto testvectors_run = [](const sha_vector_entry_t& entry,
                              dp_mem_type_t mem_type) -> bool
    {
        crypto_sha::sha_testvec_params_t          testvec_params;
        crypto_sha::sha_testvec_pre_push_params_t pre_params;
        crypto_sha::sha_testvec_push_params_t     push_params;
        crypto_sha::sha_testvec_t                 *sha_testvec;
        offload_base_params_t                     base_params;
        bool                                      success;

        testvec_params.crypto_symm_type(entry.crypto_symm_type).
                       msg_desc_mem_type(mem_type).
                       msg_digest_mem_type(mem_type).
                       status_mem_type(mem_type).
                       doorbell_mem_type(mem_type);
#ifdef __x86_64__

        /*
         * With model, reduce the scale of Montecarlo tests for faster execution.
         * These results are not to be submitted for validation.
         */
        testvec_params.montecarlo_iters_max(
                       testvec_params.montecarlo_result_epoch());
#endif
        OFFL_LOG_INFO("Running SHA vector {} with mem_type {}",
                      entry.testvec_fname, mem_type);
        sha_testvec = new crypto_sha::sha_testvec_t(
                          testvec_params.base_params(base_params));
        success = sha_testvec->pre_push(pre_params.scripts_dir(FLAGS_script_dir).
                                                   testvec_fname(entry.testvec_fname).
                                                   rsp_fname_suffix(mem_type == DP_MEM_TYPE_HBM ?
                                                                    "" : "host"));
        if (success) {
            success = sha_testvec->push(push_params.sha_ring(crypto_symm::mpp0_ring));
        }
        if (success) {
            success = sha_testvec->post_push();
        }
        if (success) {
            success = sha_testvec->full_verify();
        }
        sha_testvec->rsp_file_output();
        delete sha_testvec;
        return success;
    };

    failure_count = 0;
    for (uint32_t i = 0; i < testvectors->size(); i++) {
        const sha_vector_entry_t& entry = testvectors->at(i);

        /*
         * 1st run with HBM
         */
        if (!testvectors_run(entry, DP_MEM_TYPE_HBM)) {
            failure_count++;
        }

#ifdef __x86_64__

        /*
         * 2nd run with host mem
         */
        if (!testvectors_run(entry, DP_MEM_TYPE_HOST_MEM)) {
            failure_count++;
        }
#endif
    }

    return failure_count == 0;
}

/*
 * Driver for AES testvectors
 */
static bool
aes_testvectors_run(void *test_param)
{
    vector<aes_vector_entry_t>  *testvectors =
                      static_cast<vector<aes_vector_entry_t> *>(test_param);
    uint32_t        failure_count;

    /*
     * Run the vectors with the specified mem_type
     */
    auto testvectors_run = [](const aes_vector_entry_t& entry,
                              dp_mem_type_t mem_type) -> bool
    {
        crypto_aes::aes_testvec_params_t          testvec_params;
        crypto_aes::aes_testvec_pre_push_params_t pre_params;
        crypto_aes::aes_testvec_push_params_t     push_params;
        crypto_aes::aes_testvec_t                 *aes_testvec;
        offload_base_params_t                     base_params;
        bool                                      success;

        testvec_params.crypto_symm_type(entry.crypto_symm_type).
                       montecarlo_type(entry.montecarlo_type).
                       msg_desc_mem_type(mem_type).
                       msg_mem_type(mem_type).
                       key_mem_type(mem_type).
                       iv_mem_type(mem_type).
                       status_mem_type(mem_type).
                       doorbell_mem_type(mem_type);
#ifdef __x86_64__

        /*
         * With model, reduce the scale of Montecarlo tests for faster execution.
         * These results are not to be submitted for validation.
         */
        testvec_params.montecarlo_iters_max(
                       testvec_params.montecarlo_result_epoch());
#endif
        OFFL_LOG_INFO("Running AES vector {} with mem_type {}",
                      entry.testvec_fname, mem_type);
        aes_testvec = new crypto_aes::aes_testvec_t(
                          testvec_params.base_params(base_params));
        success = aes_testvec->pre_push(pre_params.scripts_dir(FLAGS_script_dir).
                                                   testvec_fname(entry.testvec_fname).
                                                   rsp_fname_suffix(mem_type == DP_MEM_TYPE_HBM ?
                                                                    "" : "host"));
        if (success) {
            success = aes_testvec->push(push_params.aes_ring(crypto_symm::mpp0_ring));
        }
        if (success) {
            success = aes_testvec->post_push();
        }
        if (success) {
            success = aes_testvec->full_verify();
        }
        aes_testvec->rsp_file_output();
        delete aes_testvec;
        return success;
    };

    failure_count = 0;
    for (uint32_t i = 0; i < testvectors->size(); i++) {
        const aes_vector_entry_t& entry = testvectors->at(i);

        /*
         * 1st run with HBM
         */
        if (!testvectors_run(entry, DP_MEM_TYPE_HBM)) {
            failure_count++;
        }

#ifdef __x86_64__

        /*
         * 2nd run with host mem
         */
        if (!testvectors_run(entry, DP_MEM_TYPE_HOST_MEM)) {
            failure_count++;
        }
#endif
    }

    return failure_count == 0;
}

int
main(int argc,
     char **argv)
{
    struct timeval      start;
    struct timeval      end;
    bool                overall_success;

    offl::logger::init(true, "info");

    gflags::ParseCommandLineFlags(&argc, &argv, true);
    signal(SIGSEGV, sig_handler);

    std::cout << "Input - hal_port: "        << FLAGS_hal_port
              << "\nInput - hal_ip: "        << FLAGS_hal_ip
              << "\nTest group:     "        << FLAGS_test_group
              << "\nScripts dir:    "        << FLAGS_script_dir
              << "\nOpenssl engine path: "   << FLAGS_engine_path
              << std::endl;

    if (FLAGS_test_group == "") {
        run_rsa_testvectors = true;
        run_ecdsa_testvectors = true;
        run_drbg_testvectors = true;
        run_sha_testvectors = true;
        run_aes_testvectors = true;
    } else if (FLAGS_test_group == "rsa") {
        run_rsa_testvectors = true;
    } else if (FLAGS_test_group == "ecdsa") {
        run_ecdsa_testvectors = true;
    } else if (FLAGS_test_group == "drbg") {
        run_drbg_testvectors = true;
    } else if (FLAGS_test_group == "sha") {
        run_sha_testvectors = true;
    } else if (FLAGS_test_group == "aes") {
        run_aes_testvectors = true;
    }

    if (common_setup() < 0)  {
        OFFL_FUNC_ERR("Failed common setup");
        return 1;
    }

    /*
     * Indicate to model that config is done
     */
    CONFIG_DONE();

    if (run_rsa_testvectors) {
        test_suite.push_back(test_entry_t("FIPS RSA", &rsa_testvectors_run,
                                          (void *)&rsa_testvectors));
    }
    if (run_ecdsa_testvectors) {
        test_suite.push_back(test_entry_t("FIPS ECDSA", &ecdsa_testvectors_run,
                                          (void *)&ecdsa_testvectors));
    }
    if (run_drbg_testvectors) {
        test_suite.push_back(test_entry_t("FIPS DRBG", &drbg_testvectors_run,
                                          (void *)&drbg_testvectors));
    }
    if (run_sha_testvectors) {
        test_suite.push_back(test_entry_t("FIPS SHA", &sha_testvectors_run,
                                          (void *)&sha_testvectors));
    }
    if (run_aes_testvectors) {
        test_suite.push_back(test_entry_t("FIPS AES", &aes_testvectors_run,
                                          (void *)&aes_testvectors));
    }

    OFFL_LOG_INFO("Formed test suite with {} cases", test_suite.size());
    OFFL_LOG_INFO("Running test cases");

    overall_success = true;
    for (tcid = 0; tcid < test_suite.size(); tcid++) {
        test_entry_t& test_entry = test_suite.at(tcid);

        gettimeofday(&start, NULL);
        OFFL_LOG_INFO(" Starting test #: {} name: {}", tcid, test_entry.test_name);

        TESTCASE_BEGIN(tcid, 0);
        test_entry.test_success = test_entry.test_fn(test_entry.test_param);
        TESTCASE_END(tcid, 0);

        gettimeofday(&end, NULL);
        OFFL_LOG_INFO(" Finished test #: {} name: {} status {} time {}",
                      tcid, test_entry.test_name, test_entry.test_success,
                      end.tv_sec - start.tv_sec);
        overall_success &= test_entry.test_success;
    }

    OFFL_LOG_INFO("");
    OFFL_LOG_INFO("Consolidated Test Report");
    OFFL_LOG_INFO("--------------------------------------------------------------");
    OFFL_LOG_INFO("Number\t\tName\t\t\t\tResult\n");
    OFFL_LOG_INFO("--------------------------------------------------------------");

    for (size_t i = 0; i < test_suite.size(); i++) {
        test_entry_t& test_entry = test_suite.at(i);
        OFFL_LOG_INFO("{}\t\t{}\t\t\t{}", i+1, test_entry.test_name,
                      test_entry.test_success ? "Success" : "Failure");
    }

    OFFL_LOG_INFO("");
    OFFL_LOG_INFO("Overall Report: {}",
                  overall_success ? "SUCCESS" : "FAILURE");

    EXIT_SIMULATION();

    if (offl::logger::logger()) {
        offl::logger::logger()->flush();
    }
    fflush(stdout);
    exit(overall_success ? 0 : 1);
}
