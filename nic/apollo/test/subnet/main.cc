//
// {C} Copyright 2019 Pensando Systems Inc. All rights reserved
//
//----------------------------------------------------------------------------
///
/// \file
/// This file contains all subnet test cases
///
//----------------------------------------------------------------------------

#include "nic/apollo/api/include/pds_batch.hpp"
#include "nic/apollo/test/utils/base.hpp"
#include "nic/apollo/test/utils/subnet.hpp"
#include "nic/apollo/test/utils/vcn.hpp"
#include <getopt.h>
#include <gtest/gtest.h>
#include <stdio.h>

using std::cerr;
using std::cout;
using std::endl;

namespace api_test {

// GLobals
char *g_cfg_file = NULL;
int g_batch_epoch = 1;

//----------------------------------------------------------------------------
// Subnet test class
//----------------------------------------------------------------------------

class subnet : public ::pds_test_base {
protected:
    subnet() {}
    virtual ~subnet() {}
    virtual void SetUp() {}
    virtual void TearDown() {}
    static void SetUpTestCase() {
        pds_test_base::SetUpTestCase(g_cfg_file, false);

        pds_batch_params_t batch_params = {0};
        vcn_util vcn_obj(1, "10.0.0.0/8");

        batch_params.epoch = g_batch_epoch++;
        ASSERT_TRUE(pds_batch_start(&batch_params) == sdk::SDK_RET_OK);
        ASSERT_TRUE(vcn_obj.create() == sdk::SDK_RET_OK);
        ASSERT_TRUE(pds_batch_commit() == sdk::SDK_RET_OK);
    }
};

//----------------------------------------------------------------------------
// Subnet test cases implementation
//----------------------------------------------------------------------------

/// \defgroup SUBNET_TEST
/// @{

/// \brief Create and delete maximum subnets in the same batch
/// The operation should be de-duped by framework and is a NO-OP
/// from hardware perspective
/// [ Create SetMax, Delete SetMax ] - Read
TEST_F(subnet, subnet_workflow_1) {
    pds_batch_params_t batch_params = {0};
    pds_subnet_key_t key = {};
    std::string subnet_start_addr = "10.0.0.0/16";
    uint32_t num_subnets = 10;
    pds_vcn_key_t vcn_key = {};

    // Trigger
    key.id = 1;
    vcn_key.id = 1;
    batch_params.epoch = ++g_batch_epoch;
    ASSERT_TRUE(pds_batch_start(&batch_params) == sdk::SDK_RET_OK);
    ASSERT_TRUE(subnet_util::many_create(key, vcn_key, subnet_start_addr,
                                         num_subnets) == sdk::SDK_RET_OK);
    ASSERT_TRUE(subnet_util::many_delete(key, num_subnets) == sdk::SDK_RET_OK);
    ASSERT_TRUE(pds_batch_commit() == sdk::SDK_RET_OK);

    ASSERT_TRUE(subnet_util::many_read(
        key, num_subnets, sdk::SDK_RET_ENTRY_NOT_FOUND) == sdk::SDK_RET_OK);
}

/// \brief Create, delete and create max subnets in the same batch
/// Create and delete should be de-deduped by framework and subsequent create
/// should result in successful creation
/// [ Create SetMax - Delete SetMax - Create SetMax ] - Read
TEST_F(subnet, subnet_workflow_2) {
    pds_batch_params_t batch_params = {0};
    pds_subnet_key_t key = {};
    pds_vcn_key_t vcn_key = {};
    std::string start_addr = "10.0.0.0/16";
    uint32_t num_subnets = 1024;

    // Trigger
    key.id = 1;
    vcn_key.id = 1;
    batch_params.epoch = ++g_batch_epoch;
    ASSERT_TRUE(pds_batch_start(&batch_params) == sdk::SDK_RET_OK);
    ASSERT_TRUE(subnet_util::many_create(key, vcn_key, start_addr,
                                         num_subnets) == sdk::SDK_RET_OK);
    ASSERT_TRUE(subnet_util::many_delete(key, num_subnets) == sdk::SDK_RET_OK);
    ASSERT_TRUE(subnet_util::many_create(key, vcn_key, start_addr,
                                         num_subnets) == sdk::SDK_RET_OK);
    ASSERT_TRUE(pds_batch_commit() == sdk::SDK_RET_OK);

    ASSERT_TRUE(subnet_util::many_read(
        key, num_subnets, sdk::SDK_RET_OK) == sdk::SDK_RET_OK);

    // Cleanup
    batch_params.epoch = ++g_batch_epoch;
    ASSERT_TRUE(pds_batch_start(&batch_params) == sdk::SDK_RET_OK);
    ASSERT_TRUE(subnet_util::many_delete(key, num_subnets) == sdk::SDK_RET_OK);
    ASSERT_TRUE(pds_batch_commit() == sdk::SDK_RET_OK);
}

/// \brief Create Set1, Set2, Delete Set1, Create Set3 in same batch
/// The set1 subnet should be de-duped and set2 and set3 should be programmed
/// in the hardware
/// [ Create Set1, Set2 - Delete Set1 - Create Set3 ] - Read
TEST_F(subnet, subnet_workflow_3) {
    // [ Create Set1, Set2 - Delete Set1 - Create Set3 ] - Read
    pds_batch_params_t batch_params = {0};
    pds_subnet_key_t key1 = {}, key2 = {}, key3 = {};
    std::string subnet_start_addr1 = "10.0.0.0/16";
    std::string subnet_start_addr2 = "30.0.0.0/16";
    std::string subnet_start_addr3 = "60.0.0.0/16";
    uint32_t num_subnets = 20;
    pds_vcn_key_t vcn_key = {};

    // Trigger
    key1.id = 10;
    key2.id = 40;
    key3.id = 70;
    vcn_key.id = 1;
    batch_params.epoch = ++g_batch_epoch;
    ASSERT_TRUE(pds_batch_start(&batch_params) == sdk::SDK_RET_OK);
    ASSERT_TRUE(subnet_util::many_create(key1, vcn_key, subnet_start_addr1,
                                         num_subnets) == sdk::SDK_RET_OK);
    ASSERT_TRUE(subnet_util::many_create(key2, vcn_key, subnet_start_addr2,
                                         num_subnets) == sdk::SDK_RET_OK);
    ASSERT_TRUE(subnet_util::many_delete(key1, num_subnets) == sdk::SDK_RET_OK);
    ASSERT_TRUE(subnet_util::many_create(key3, vcn_key, subnet_start_addr3,
                                         num_subnets) == sdk::SDK_RET_OK);
    ASSERT_TRUE(pds_batch_commit() == sdk::SDK_RET_OK);

    ASSERT_TRUE(subnet_util::many_read(
        key1, num_subnets, sdk::SDK_RET_ENTRY_NOT_FOUND) == sdk::SDK_RET_OK);
    ASSERT_TRUE(subnet_util::many_read(
        key2, num_subnets, sdk::SDK_RET_OK) == sdk::SDK_RET_OK);
    ASSERT_TRUE(subnet_util::many_read(
        key3, num_subnets, sdk::SDK_RET_OK) == sdk::SDK_RET_OK);

    // Cleanup
    batch_params.epoch = ++g_batch_epoch;
    ASSERT_TRUE(pds_batch_start(&batch_params) == sdk::SDK_RET_OK);
    ASSERT_TRUE(subnet_util::many_delete(key2, num_subnets) == sdk::SDK_RET_OK);
    ASSERT_TRUE(subnet_util::many_delete(key3, num_subnets) == sdk::SDK_RET_OK);
    ASSERT_TRUE(pds_batch_commit() == sdk::SDK_RET_OK);

    ASSERT_TRUE(subnet_util::many_read(key2, num_subnets,
                                       sdk::SDK_RET_ENTRY_NOT_FOUND) ==
                sdk::SDK_RET_OK);
    ASSERT_TRUE(subnet_util::many_read(key3, num_subnets,
                                       sdk::SDK_RET_ENTRY_NOT_FOUND) ==
                sdk::SDK_RET_OK);
}

/// \brief Create and delete max subnets in two batches
/// The hardware should create and delete VCN correctly. Validate using reads
/// at each batch end
/// [ Create SetMax ] - Read - [ Delete SetMax ] - Read
TEST_F(subnet, subnet_workflow_4) {
    pds_batch_params_t batch_params = {0};
    pds_subnet_key_t key = {};
    std::string start_addr = "10.0.0.0/16";
    uint32_t num_subnets = 1024;
    pds_vcn_key_t vcn_key = {};

    // Trigger
    key.id = 1;
    vcn_key.id = 1;
    batch_params.epoch = ++g_batch_epoch;
    ASSERT_TRUE(pds_batch_start(&batch_params) == sdk::SDK_RET_OK);
    ASSERT_TRUE(subnet_util::many_create(key, vcn_key, start_addr,
                                         num_subnets) == sdk::SDK_RET_OK);
    ASSERT_TRUE(pds_batch_commit() == sdk::SDK_RET_OK);

    ASSERT_TRUE(subnet_util::many_read(
        key, num_subnets, sdk::SDK_RET_OK) == sdk::SDK_RET_OK);

    batch_params.epoch = ++g_batch_epoch;
    ASSERT_TRUE(pds_batch_start(&batch_params) == sdk::SDK_RET_OK);
    ASSERT_TRUE(subnet_util::many_delete(key, num_subnets) == sdk::SDK_RET_OK);
    ASSERT_TRUE(pds_batch_commit() == sdk::SDK_RET_OK);

    ASSERT_TRUE(subnet_util::many_read(
        key, num_subnets, sdk::SDK_RET_ENTRY_NOT_FOUND) == sdk::SDK_RET_OK);
}

/// \brief Create and delete mix and match of subnets in two batches
/// [ Create Set1, Set2 ] - Read - [Delete Set1 - Create Set3 ] - Read
TEST_F(subnet, subnet_workflow_5) {
    pds_batch_params_t batch_params = {0};
    pds_subnet_key_t key1 = {}, key2 = {}, key3 = {};
    std::string subnet_start_addr1 = "10.0.0.0/16";
    std::string subnet_start_addr2 = "30.0.0.0/16";
    std::string subnet_start_addr3 = "60.0.0.0/16";
    uint32_t num_subnets = 20;
    pds_vcn_key_t vcn_key = {};

    // Trigger
    key1.id = 10;
    key2.id = 40;
    key3.id = 70;
    vcn_key.id = 1;
    batch_params.epoch = ++g_batch_epoch;
    ASSERT_TRUE(pds_batch_start(&batch_params) == sdk::SDK_RET_OK);

    ASSERT_TRUE(subnet_util::many_create(key1, vcn_key, subnet_start_addr1,
                                         num_subnets) == sdk::SDK_RET_OK);
    ASSERT_TRUE(subnet_util::many_create(key2, vcn_key, subnet_start_addr2,
                                         num_subnets) == sdk::SDK_RET_OK);
    ASSERT_TRUE(pds_batch_commit() == sdk::SDK_RET_OK);

    ASSERT_TRUE(subnet_util::many_read(
        key1, num_subnets, sdk::SDK_RET_OK) == sdk::SDK_RET_OK);
    ASSERT_TRUE(subnet_util::many_read(
        key2, num_subnets, sdk::SDK_RET_OK) == sdk::SDK_RET_OK);

    batch_params.epoch = ++g_batch_epoch;
    ASSERT_TRUE(pds_batch_start(&batch_params) == sdk::SDK_RET_OK);
    ASSERT_TRUE(subnet_util::many_delete(key1, num_subnets) == sdk::SDK_RET_OK);
    ASSERT_TRUE(subnet_util::many_create(key3, vcn_key, subnet_start_addr3,
                                         num_subnets) == sdk::SDK_RET_OK);
    ASSERT_TRUE(pds_batch_commit() == sdk::SDK_RET_OK);

    ASSERT_TRUE(subnet_util::many_read(
        key1, num_subnets, sdk::SDK_RET_ENTRY_NOT_FOUND) == sdk::SDK_RET_OK);
    ASSERT_TRUE(subnet_util::many_read(
        key2, num_subnets, sdk::SDK_RET_OK) == sdk::SDK_RET_OK);
    ASSERT_TRUE(subnet_util::many_read(
        key3, num_subnets, sdk::SDK_RET_OK) == sdk::SDK_RET_OK);

    // Cleanup
    batch_params.epoch = ++g_batch_epoch;
    ASSERT_TRUE(pds_batch_start(&batch_params) == sdk::SDK_RET_OK);
    ASSERT_TRUE(subnet_util::many_delete(key2, num_subnets) == sdk::SDK_RET_OK);
    ASSERT_TRUE(subnet_util::many_delete(key3, num_subnets) == sdk::SDK_RET_OK);
    ASSERT_TRUE(pds_batch_commit() == sdk::SDK_RET_OK);
}

/// \brief Create maximum number of VCNs in two batches
/// [ Create SetMax ] - [ Create SetMax ] - Read
TEST_F(subnet, subnet_workflow_neg_1) {
    pds_batch_params_t batch_params = {0};
    pds_subnet_key_t key = {};
    std::string start_addr = "10.0.0.0/16";
    uint32_t num_subnets = 1024;
    pds_vcn_key_t vcn_key = {};

    key.id = 1;
    vcn_key.id = 1;
    batch_params.epoch = ++g_batch_epoch;
    ASSERT_TRUE(pds_batch_start(&batch_params) == sdk::SDK_RET_OK);
    ASSERT_TRUE(subnet_util::many_create(key, vcn_key, start_addr,
                                         num_subnets) == sdk::SDK_RET_OK);
    ASSERT_TRUE(pds_batch_commit() == sdk::SDK_RET_OK);
    ASSERT_TRUE(subnet_util::many_read(
        key, num_subnets, sdk::SDK_RET_OK) == sdk::SDK_RET_OK);

    batch_params.epoch = ++g_batch_epoch;
    ASSERT_TRUE(pds_batch_start(&batch_params) == sdk::SDK_RET_OK);
    ASSERT_TRUE(subnet_util::many_create(key, vcn_key, start_addr,
                                         num_subnets) == sdk::SDK_RET_OK);
    ASSERT_TRUE(pds_batch_commit() != sdk::SDK_RET_OK);
    ASSERT_TRUE(pds_batch_abort() == sdk::SDK_RET_OK);

    // @kalyanbade why should this be ENTRY_NOT_FOUND ? seems like 1K subnet
    // still exist right ? also, there is no cleanup of these subnets here, so
    // next test will have impact due to this .. adding cleanup below !!
    // and this assert itself seems to be wrong here and in other gtests
    ASSERT_TRUE(subnet_util::many_read(
        key, num_subnets, sdk::SDK_RET_ENTRY_NOT_FOUND) == sdk::SDK_RET_OK);

    // Cleanup
    batch_params.epoch = ++g_batch_epoch;
    ASSERT_TRUE(pds_batch_start(&batch_params) == sdk::SDK_RET_OK);
    ASSERT_TRUE(subnet_util::many_delete(key, num_subnets) == sdk::SDK_RET_OK);
    ASSERT_TRUE(pds_batch_commit() == sdk::SDK_RET_OK);
}

/// \brief Create more than maximum number of subnets supported.
/// [ Create SetMax+1] - Read
TEST_F(subnet, subnet_workflow_neg_2) {
    pds_batch_params_t batch_params = {0};
    pds_subnet_key_t key = {};
    std::string start_addr = "10.0.0.0/16";
    uint32_t num_subnets = PDS_MAX_SUBNET + 1;
    pds_vcn_key_t vcn_key = {};

    key.id = 1;
    vcn_key.id = 1;
    batch_params.epoch = ++g_batch_epoch;
    ASSERT_TRUE(pds_batch_start(&batch_params) == sdk::SDK_RET_OK);
    ASSERT_TRUE(subnet_util::many_create(key, vcn_key, start_addr,
                                         num_subnets) == sdk::SDK_RET_OK);
    ASSERT_TRUE(pds_batch_commit() != sdk::SDK_RET_OK);
    ASSERT_TRUE(pds_batch_abort() == sdk::SDK_RET_OK);

    ASSERT_TRUE(subnet_util::many_read(
        key, num_subnets, sdk::SDK_RET_ENTRY_NOT_FOUND) == sdk::SDK_RET_OK);
}

/// \brief Read of a non-existing subnet should return entry not found.
/// Read NonEx
TEST_F(subnet, subnet_workflow_neg_3a) {
    // Read NonEx
    pds_subnet_key_t key = {};
    uint32_t num_subnets = 1024;

    // Trigger
    key.id = 1;
    ASSERT_TRUE(subnet_util::many_read(
        key, num_subnets, sdk::SDK_RET_ENTRY_NOT_FOUND) == sdk::SDK_RET_OK);
}

/// \brief Deletion of a non-existing subnets should fail.
/// [Delete NonEx]
TEST_F(subnet, subnet_workflow_neg_3b) {
    pds_batch_params_t batch_params = {0};
    pds_subnet_key_t key = {};
    std::string start_addr = "10.0.0.0/16";
    uint32_t num_subnets = 1024;
    pds_vcn_key_t vcn_key = {};

    key.id = 1;
    vcn_key.id = 1;
    batch_params.epoch = ++g_batch_epoch;
    ASSERT_TRUE(pds_batch_start(&batch_params) == sdk::SDK_RET_OK);
    ASSERT_TRUE(subnet_util::many_delete(key, num_subnets) == sdk::SDK_RET_OK);
    ASSERT_TRUE(pds_batch_commit() != sdk::SDK_RET_OK);
    ASSERT_TRUE(pds_batch_abort() == sdk::SDK_RET_OK);
}

/// \brief Invalid batch shouldn't affect entries of previous batch
/// [ Create Set1 ] - [Delete Set1, Set2 ] - Read
TEST_F(subnet, subnet_workflow_neg_4) {
    pds_batch_params_t batch_params = {0};
    pds_subnet_key_t key1 = {}, key2 = {};
    std::string subnet_start_addr1 = "10.0.0.0/16";
    std::string subnet_start_addr2 = "40.0.0.0/16";
    uint32_t num_subnets = 20;
    pds_vcn_key_t vcn_key = {};

    // Trigger
    vcn_key.id = 1;
    key1.id = 10;
    key2.id = 40;
    batch_params.epoch = ++g_batch_epoch;
    ASSERT_TRUE(pds_batch_start(&batch_params) == sdk::SDK_RET_OK);
    ASSERT_TRUE(subnet_util::many_create(key1, vcn_key, subnet_start_addr1,
                                         num_subnets) == sdk::SDK_RET_OK);
    ASSERT_TRUE(pds_batch_commit() == sdk::SDK_RET_OK);

    ASSERT_TRUE(subnet_util::many_read(
        key1, num_subnets, sdk::SDK_RET_OK) == sdk::SDK_RET_OK);

    batch_params.epoch = ++g_batch_epoch;
    ASSERT_TRUE(pds_batch_start(&batch_params) == sdk::SDK_RET_OK);
    ASSERT_TRUE(subnet_util::many_delete(key1, num_subnets) == sdk::SDK_RET_OK);
    ASSERT_TRUE(subnet_util::many_delete(key2, num_subnets) == sdk::SDK_RET_OK);
    ASSERT_TRUE(pds_batch_commit() != sdk::SDK_RET_OK);
    ASSERT_TRUE(pds_batch_abort() == sdk::SDK_RET_OK);

    ASSERT_TRUE(subnet_util::many_read(
        key1, num_subnets, sdk::SDK_RET_OK) == sdk::SDK_RET_OK);
    ASSERT_TRUE(subnet_util::many_read(
        key2, num_subnets, sdk::SDK_RET_ENTRY_NOT_FOUND) == sdk::SDK_RET_OK);

    // Cleanup
    batch_params.epoch = ++g_batch_epoch;
    ASSERT_TRUE(pds_batch_start(&batch_params) == sdk::SDK_RET_OK);
    ASSERT_TRUE(subnet_util::many_delete(key1, num_subnets) == sdk::SDK_RET_OK);
    ASSERT_TRUE(pds_batch_commit() == sdk::SDK_RET_OK);
}

/// \brief Subnet workflow corner case 4
///
/// [ Create SetCorner ] - Read
TEST_F(subnet, subnet_workflow_corner_case_4) {}

/// @}

}    // namespace api_test

//----------------------------------------------------------------------------
// Entry point
//----------------------------------------------------------------------------

static inline void
subnet_test_usage_print (char **argv)
{
    cout << "Usage : " << argv[0] << " -c <hal.json>" << endl;
}

static void
subnet_test_options_parse (int argc, char **argv)
{
    int oc;
    struct option longopts[] = {{"config", required_argument, NULL, 'c'},
                                {"help", no_argument, NULL, 'h'},
                                {0, 0, 0, 0}};

    while ((oc = getopt_long(argc, argv, ":hc:", longopts, NULL)) != -1) {
        switch (oc) {
        case 'c':
            api_test::g_cfg_file = optarg;
            break;
        default:    // ignore all other options
            break;
        }
    }
}

static inline sdk_ret_t
subnet_test_options_validate (void)
{
    if (!api_test::g_cfg_file) {
        cerr << "HAL config file is not specified" << endl;
        return SDK_RET_ERR;
    }
    return SDK_RET_OK;
}

int
main (int argc, char **argv)
{
    subnet_test_options_parse(argc, argv);
    if (subnet_test_options_validate() != SDK_RET_OK) {
        subnet_test_usage_print(argv);
        exit(1);
    }

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
