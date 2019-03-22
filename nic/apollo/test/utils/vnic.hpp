//
// {C} Copyright 2019 Pensando Systems Inc. All rights reserved
//
//----------------------------------------------------------------------------
///
/// \file
/// This file contains the vnic test utility routines
///
//----------------------------------------------------------------------------

#ifndef __TEST_UTILS_VNIC_HPP__
#define __TEST_UTILS_VNIC_HPP__

#include "nic/sdk/include/sdk/base.hpp"
#include "nic/apollo/api/include/pds_vnic.hpp"

namespace api_test {

typedef struct vnic_stepper_seed_s {
    uint32_t id;
    uint16_t vlan_tag;
    uint32_t mpls_slot;
    uint64_t mac_u64;
} vnic_stepper_seed_t;

/// VNIC test utility class
class vnic_util {
public:
    pds_vcn_id_t vcn_id;           ///< VCN id
    pds_subnet_id_t sub_id;        ///< Subnet id
    pds_vnic_id_t vnic_id;         ///< VNIC id
    uint16_t vlan_tag;             ///< VLAN tag
    pds_slot_id_t mpls_slot;       ///< MPLS slot
    std::string vnic_mac;          ///< VNIC mac
    mac_addr_t mac_addr;
    uint64_t mac_u64;
    pds_rsc_pool_id_t rsc_pool_id; ///< Resource pool id
    bool src_dst_check;            ///< Source destination check

    /// \brief Constructor
    vnic_util();

    /// \brief Parameterized constructor
    vnic_util(uint32_t vnic_id, uint16_t vlan_tag, uint32_t mpls_slot,
              uint64_t mac_u64);
    /// \brief Parameterized constructor
    vnic_util(pds_vcn_id_t vcn_id, pds_vnic_id_t vnic_id,
              pds_subnet_id_t sub_id = -1, std::string vnic_mac = "",
              bool src_dst_check = false);

    /// \brief Destructor
    ~vnic_util();

    /// \brief Create VNIC
    ///
    /// \returns #SDK_RET_OK on success, failure status code on error
    sdk_ret_t create(void);

    /// \brief Read VNIC info
    ///
    /// \returns #SDK_RET_OK on success, failure status code on error
    sdk_ret_t read(pds_vnic_info_t *info, bool compare_spec = true);

    /// \brief Update VNIC configuration
    ///
    /// \returns #SDK_RET_OK on success, failure status code on error
    sdk_ret_t update(void);

    /// \brief Delete VNIC
    ///
    /// \returns #SDK_RET_OK on success, failure status code on error
    sdk_ret_t del(void);

    /// \brief Create many VNICs for the given subnet and VCN
    ///
    /// This increments the vnic_id, vlan_tag, and mpls_slot.
    ///
    /// \param num_vnics Number of vnics to be created
    ///
    /// \returns #SDK_RET_OK on success, failure status code on error
    static sdk_ret_t many_create(vnic_stepper_seed_t key, uint32_t num_vnics);

    /// \brief Read many VNICs for the given subnet and VCN
    ///
    /// This increments the vnic_id, vlan_tag, and mpls_slot.
    ///
    /// \param num_vnics Number of vnics to be read
    ///
    /// \returns #SDK_RET_OK on success, failure status code on error
    static sdk_ret_t many_read(pds_vnic_key_t start_key, uint32_t num_vnics,
                        sdk::sdk_ret_t expected_result = sdk::SDK_RET_OK);

    /// \brief Delete many VNICs for the given subnet and VCN
    ///
    /// This increments the vnic_id, vlan_tag, and mpls_slot.
    ///
    /// \param num_vnics Number of vnics to be deleted
    ///
    /// \returns #SDK_RET_OK on success, failure status code on error
    static sdk_ret_t many_delete(pds_vnic_key_t start_key, uint32_t num_vnics);
};

} // namespace api_test

#endif // __TEST_UTILS_VNIC_HPP__
