#include <stdio.h>
#include <gtest/gtest.h>

#ifdef GFT
#include <boost/crc.hpp>
#include "nic/model_sim/include/lib_model_client.h"
#include "nic/hal/pd/capri/capri_config.hpp"
#include "nic/hal/pd/capri/capri_hbm.hpp"
#include "nic/hal/pd/capri/capri_loader.h"
#include "nic/hal/pd/capri/capri_tbl_rw.hpp"
#include "nic/hal/pd/p4pd_api.hpp"
#include "nic/gen/gft/include/gft_p4pd.h"
#include "nic/p4/gft/include/defines.h"

// packets generated by tools/gft_test.py
static uint8_t g_snd_pkt1[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x00, 0xA1, 0xA2,
    0xA3, 0xA4, 0xA5, 0x08, 0x00, 0x45, 0x00, 0x00, 0x8C,
    0x00, 0x01, 0x00, 0x00, 0x40, 0x11, 0x61, 0x59, 0x0B,
    0x01, 0x02, 0x03, 0x0A, 0x01, 0x02, 0x03, 0x00, 0x35,
    0x12, 0xB5, 0x00, 0x78, 0x5A, 0x17, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x11, 0x12, 0x13,
    0x14, 0x15, 0x00, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0x08,
    0x00, 0x45, 0x00, 0x00, 0x5A, 0x00, 0x01, 0x00, 0x00,
    0x40, 0x11, 0x5D, 0x8B, 0x0D, 0x01, 0x02, 0x03, 0x0C,
    0x01, 0x02, 0x03, 0x00, 0x35, 0x12, 0xB5, 0x00, 0x46,
    0x19, 0xDE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x21, 0x22, 0x23, 0x24, 0x25, 0x00, 0xC1,
    0xC2, 0xC3, 0xC4, 0xC5, 0x08, 0x00, 0x45, 0x00, 0x00,
    0x28, 0x00, 0x01, 0x00, 0x00, 0x40, 0x06, 0x59, 0xC8,
    0x0F, 0x01, 0x02, 0x03, 0x0E, 0x01, 0x02, 0x03, 0xAB,
    0xBA, 0xBE, 0xEF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x50, 0x02, 0x20, 0x00, 0x04, 0x31, 0x00,
    0x00};

static uint8_t g_rcv_pkt1[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0xA3, 0x21, 0xA2,
    0xA3, 0xA4, 0xA5, 0x08, 0x00, 0x45, 0x00, 0x00, 0x8C,
    0x00, 0x01, 0x00, 0x00, 0x40, 0x11, 0x61, 0x59, 0x0B,
    0x01, 0x02, 0x03, 0x0A, 0x01, 0x02, 0x03, 0x00, 0x35,
    0x12, 0xB5, 0x00, 0x78, 0x5A, 0x17, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x11, 0x12, 0x13,
    0x14, 0x15, 0x00, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0x08,
    0x00, 0x45, 0x00, 0x00, 0x5A, 0x00, 0x01, 0x00, 0x00,
    0x40, 0x11, 0x5D, 0x8B, 0x0D, 0x01, 0x02, 0x03, 0x0C,
    0x01, 0x02, 0x03, 0x00, 0x35, 0x12, 0xB5, 0x00, 0x46,
    0x19, 0xDE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x21, 0x22, 0x23, 0x24, 0x25, 0x00, 0xC1,
    0xC2, 0xC3, 0xC4, 0xC5, 0x08, 0x00, 0x45, 0x00, 0x00,
    0x28, 0x00, 0x01, 0x00, 0x00, 0x40, 0x06, 0x59, 0xC8,
    0x0F, 0x01, 0x02, 0x03, 0x0E, 0x01, 0x02, 0x03, 0xAB,
    0xBA, 0xBE, 0xEF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x50, 0x02, 0x20, 0x00, 0x04, 0x31, 0x00,
    0x00};

static uint8_t g_snd_pkt2[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x00, 0xA1, 0xA2,
    0xA3, 0xA4, 0xA5, 0x08, 0x00, 0x45, 0x00, 0x00, 0x8C,
    0x00, 0x01, 0x00, 0x00, 0x40, 0x11, 0x61, 0x59, 0x0B,
    0x01, 0x02, 0x03, 0x0A, 0x01, 0x02, 0x03, 0x00, 0x35,
    0x12, 0xB5, 0x00, 0x78, 0x5A, 0x17, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x11, 0x12, 0x13,
    0x14, 0x15, 0x00, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0x08,
    0x00, 0x45, 0x00, 0x00, 0x5A, 0x00, 0x01, 0x00, 0x00,
    0x40, 0x11, 0x5D, 0x8B, 0x0D, 0x01, 0x02, 0x03, 0x0C,
    0x01, 0x02, 0x03, 0x00, 0x35, 0x12, 0xB5, 0x00, 0x46,
    0x19, 0xDE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x21, 0x22, 0x23, 0x24, 0x25, 0x00, 0xC1,
    0xC2, 0xC3, 0xC4, 0xC5, 0x08, 0x00, 0x45, 0x00, 0x00,
    0x28, 0x00, 0x01, 0x00, 0x00, 0x40, 0x06, 0x59, 0xC8,
    0x0F, 0x01, 0x02, 0x03, 0x0E, 0x01, 0x02, 0x03, 0x12,
    0x34, 0x56, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x50, 0x02, 0x20, 0x00, 0x06, 0x2F, 0x00,
    0x00};

static uint8_t g_rcv_pkt2[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0xA3, 0x41, 0xA2,
    0xA3, 0xA4, 0xA5, 0x08, 0x00, 0x45, 0x00, 0x00, 0x8C,
    0x00, 0x01, 0x00, 0x00, 0x40, 0x11, 0x61, 0x59, 0x0B,
    0x01, 0x02, 0x03, 0x0A, 0x01, 0x02, 0x03, 0x00, 0x35,
    0x12, 0xB5, 0x00, 0x78, 0x5A, 0x17, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x11, 0x12, 0x13,
    0x14, 0x15, 0x00, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0x08,
    0x00, 0x45, 0x00, 0x00, 0x5A, 0x00, 0x01, 0x00, 0x00,
    0x40, 0x11, 0x5D, 0x8B, 0x0D, 0x01, 0x02, 0x03, 0x0C,
    0x01, 0x02, 0x03, 0x00, 0x35, 0x12, 0xB5, 0x00, 0x46,
    0x19, 0xDE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x21, 0x22, 0x23, 0x24, 0x25, 0x00, 0xC1,
    0xC2, 0xC3, 0xC4, 0xC5, 0x08, 0x00, 0x45, 0x00, 0x00,
    0x28, 0x00, 0x01, 0x00, 0x00, 0x40, 0x06, 0x59, 0xC8,
    0x0F, 0x01, 0x02, 0x03, 0x0E, 0x01, 0x02, 0x03, 0x12,
    0x34, 0x56, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x50, 0x02, 0x20, 0x00, 0x06, 0x2F, 0x00,
    0x00};

uint64_t g_layer1_dmac  = 0x000102030405ULL;
uint64_t g_layer1_smac  = 0x00A1A2A3A4A5ULL;
uint32_t g_layer1_dip   = 0x0A010203;
uint32_t g_layer1_sip   = 0x0B010203;
uint8_t  g_layer1_proto = IP_PROTO_UDP;
uint64_t g_layer2_dmac  = 0x001112131415ULL;
uint64_t g_layer2_smac  = 0x00B1B2B3B4B5ULL;
uint32_t g_layer2_dip   = 0x0C010203;
uint32_t g_layer2_sip   = 0x0D010203;
uint8_t  g_layer2_proto = IP_PROTO_UDP;
uint64_t g_layer3_dmac  = 0x002122232425ULL;
uint64_t g_layer3_smac  = 0x00C1C2C3C4C5ULL;
uint32_t g_layer3_dip   = 0x0E010203;
uint32_t g_layer3_sip   = 0x0F010203;
uint8_t  g_layer3_proto = IP_PROTO_TCP;
uint16_t g_layer31_sport = 0xABBA;
uint16_t g_layer31_dport = 0xBEEF;
uint16_t g_layer32_sport = 0x1234;
uint16_t g_layer32_dport = 0x5678;
uint32_t g_ohash_idx    = 0xDEAF;

static uint32_t
generate_hash_(void *key, uint32_t key_len, uint32_t crc_init_val) {
    boost::crc_basic<32> *crc_hash;
    uint32_t hash_val = 0x0;

    crc_hash = new boost::crc_basic<32>(0x04C11DB7, crc_init_val,
                                        0x00000000, false, false);
    crc_hash->process_bytes(key, key_len);
    hash_val = crc_hash->checksum();
    delete crc_hash;
    return hash_val;
}

static void
ingress_key1_init() {
    ingress_key1_swkey_t      key;
    ingress_key1_swkey_mask_t mask;
    ingress_key1_actiondata   data;

    memset(&key, 0, sizeof(key));
    memset(&mask, 0, sizeof(mask));
    memset(&data, 0, sizeof(data));

    // key and mask
    key.ethernet_1_valid = 0xFF;
    key.ipv4_1_valid = 0xFF;
    key.udp_1_valid = 0xFF;
    key.tunnel_metadata_tunnel_type_1 = INGRESS_TUNNEL_TYPE_VXLAN;
    key.ethernet_2_valid = 0xFF;
    key.ipv4_2_valid = 0xFF;
    key.udp_2_valid = 0xFF;
    key.tunnel_metadata_tunnel_type_2 = INGRESS_TUNNEL_TYPE_VXLAN;
    key.ethernet_3_valid = 0xFF;
    key.ipv4_3_valid = 0xFF;
    key.tcp_3_valid = 0xFF;
    memset(&mask, 0xFF, sizeof(mask));

    // data
    data.ingress_key1_action_u.ingress_key1_ingress_key1.match_fields =
        (MATCH_ETHERNET_SRC | MATCH_ETHERNET_DST |
         MATCH_IP_SRC | MATCH_IP_DST | MATCH_IP_PROTO);

    // prepare entry and write hardware
    uint32_t hwkey_len = 0;
    uint32_t hwmask_len = 0;
    uint32_t hwdata_len = 0;
    uint8_t  *hwkey = NULL;
    uint8_t  *hwmask = NULL;
    p4pd_gft_hwentry_query(P4_GFT_TBL_ID_INGRESS_KEY1, &hwkey_len, &hwmask_len,
                           &hwdata_len);
    hwkey_len = (hwkey_len >> 3) + ((hwkey_len & 0x7) ? 1 : 0);
    hwmask_len = (hwmask_len >> 3) + ((hwmask_len & 0x7) ? 1 : 0);
    hwdata_len = (hwdata_len >> 3) + ((hwdata_len & 0x7) ? 1 : 0);
    hwkey = new uint8_t[hwkey_len];
    hwmask = new uint8_t[hwmask_len];
    memset(hwkey, 0, hwkey_len);
    memset(hwmask, 0, hwmask_len);
    p4pd_gft_hwkey_hwmask_build(P4_GFT_TBL_ID_INGRESS_KEY1, &key, &mask,
                                hwkey, hwmask);
    p4pd_gft_entry_write(P4_GFT_TBL_ID_INGRESS_KEY1, 0, hwkey, hwmask, &data);
    delete [] hwkey;
    delete [] hwmask;
}

static void
ingress_key2_init() {
    ingress_key2_swkey_t      key;
    ingress_key2_swkey_mask_t mask;
    ingress_key2_actiondata   data;

    memset(&key, 0, sizeof(key));
    memset(&mask, 0, sizeof(mask));
    memset(&data, 0, sizeof(data));

    // key and mask
    key.ethernet_1_valid = 0xFF;
    key.ipv4_1_valid = 0xFF;
    key.udp_1_valid = 0xFF;
    key.tunnel_metadata_tunnel_type_1 = INGRESS_TUNNEL_TYPE_VXLAN;
    key.ethernet_2_valid = 0xFF;
    key.ipv4_2_valid = 0xFF;
    key.udp_2_valid = 0xFF;
    key.tunnel_metadata_tunnel_type_2 = INGRESS_TUNNEL_TYPE_VXLAN;
    key.ethernet_3_valid = 0xFF;
    key.ipv4_3_valid = 0xFF;
    key.tcp_3_valid = 0xFF;
    memset(&mask, 0xFF, sizeof(mask));

    // data
    data.ingress_key2_action_u.ingress_key2_ingress_key2.match_fields =
        (MATCH_ETHERNET_SRC | MATCH_ETHERNET_DST |
         MATCH_IP_SRC | MATCH_IP_DST | MATCH_IP_PROTO);

    // prepare entry and write hardware
    uint32_t hwkey_len = 0;
    uint32_t hwmask_len = 0;
    uint32_t hwdata_len = 0;
    uint8_t  *hwkey = NULL;
    uint8_t  *hwmask = NULL;
    p4pd_gft_hwentry_query(P4_GFT_TBL_ID_INGRESS_KEY2, &hwkey_len, &hwmask_len,
                           &hwdata_len);
    hwkey_len = (hwkey_len >> 3) + ((hwkey_len & 0x7) ? 1 : 0);
    hwmask_len = (hwmask_len >> 3) + ((hwmask_len & 0x7) ? 1 : 0);
    hwdata_len = (hwdata_len >> 3) + ((hwdata_len & 0x7) ? 1 : 0);
    hwkey = new uint8_t[hwkey_len];
    hwmask = new uint8_t[hwmask_len];
    memset(hwkey, 0, hwkey_len);
    memset(hwmask, 0, hwmask_len);
    p4pd_gft_hwkey_hwmask_build(P4_GFT_TBL_ID_INGRESS_KEY2, &key, &mask,
                                hwkey, hwmask);
    p4pd_gft_entry_write(P4_GFT_TBL_ID_INGRESS_KEY2, 0, hwkey, hwmask, &data);
    delete [] hwkey;
    delete [] hwmask;
}

static void
ingress_key3_init() {
    ingress_key3_swkey_t      key;
    ingress_key3_swkey_mask_t mask;
    ingress_key3_actiondata   data;

    memset(&key, 0, sizeof(key));
    memset(&mask, 0, sizeof(mask));
    memset(&data, 0, sizeof(data));

    // key and mask
    key.ethernet_1_valid = 0xFF;
    key.ipv4_1_valid = 0xFF;
    key.udp_1_valid = 0xFF;
    key.tunnel_metadata_tunnel_type_1 = INGRESS_TUNNEL_TYPE_VXLAN;
    key.ethernet_2_valid = 0xFF;
    key.ipv4_2_valid = 0xFF;
    key.udp_2_valid = 0xFF;
    key.tunnel_metadata_tunnel_type_2 = INGRESS_TUNNEL_TYPE_VXLAN;
    key.ethernet_3_valid = 0xFF;
    key.ipv4_3_valid = 0xFF;
    key.tcp_3_valid = 0xFF;
    memset(&mask, 0xFF, sizeof(mask));

    // data
    data.ingress_key3_action_u.ingress_key3_ingress_key3.match_fields =
        (MATCH_ETHERNET_SRC | MATCH_ETHERNET_DST |
         MATCH_IP_SRC | MATCH_IP_DST | MATCH_IP_PROTO);

    // prepare entry and write hardware
    uint32_t hwkey_len = 0;
    uint32_t hwmask_len = 0;
    uint32_t hwdata_len = 0;
    uint8_t  *hwkey = NULL;
    uint8_t  *hwmask = NULL;
    p4pd_gft_hwentry_query(P4_GFT_TBL_ID_INGRESS_KEY3, &hwkey_len, &hwmask_len,
                           &hwdata_len);
    hwkey_len = (hwkey_len >> 3) + ((hwkey_len & 0x7) ? 1 : 0);
    hwmask_len = (hwmask_len >> 3) + ((hwmask_len & 0x7) ? 1 : 0);
    hwdata_len = (hwdata_len >> 3) + ((hwdata_len & 0x7) ? 1 : 0);
    hwkey = new uint8_t[hwkey_len];
    hwmask = new uint8_t[hwmask_len];
    memset(hwkey, 0, hwkey_len);
    memset(hwmask, 0, hwmask_len);
    p4pd_gft_hwkey_hwmask_build(P4_GFT_TBL_ID_INGRESS_KEY3, &key, &mask,
                                hwkey, hwmask);
    p4pd_gft_entry_write(P4_GFT_TBL_ID_INGRESS_KEY3, 0, hwkey, hwmask, &data);
    delete [] hwkey;
    delete [] hwmask;
}

static void
ingress_key4_init() {
    ingress_key4_swkey_t      key;
    ingress_key4_swkey_mask_t mask;
    ingress_key4_actiondata   data;

    memset(&key, 0, sizeof(key));
    memset(&mask, 0, sizeof(mask));
    memset(&data, 0, sizeof(data));

    // key and mask
    key.ethernet_1_valid = 0xFF;
    key.ipv4_1_valid = 0xFF;
    key.udp_1_valid = 0xFF;
    key.tunnel_metadata_tunnel_type_1 = INGRESS_TUNNEL_TYPE_VXLAN;
    key.ethernet_2_valid = 0xFF;
    key.ipv4_2_valid = 0xFF;
    key.udp_2_valid = 0xFF;
    key.tunnel_metadata_tunnel_type_2 = INGRESS_TUNNEL_TYPE_VXLAN;
    key.ethernet_3_valid = 0xFF;
    key.ipv4_3_valid = 0xFF;
    key.tcp_3_valid = 0xFF;
    memset(&mask, 0xFF, sizeof(mask));

    // data
    data.ingress_key4_action_u.ingress_key4_ingress_key4.match_fields =
        (MATCH_TRANSPORT_SRC_PORT_3 | MATCH_TRANSPORT_DST_PORT_3);

    // prepare entry and write hardware
    uint32_t hwkey_len = 0;
    uint32_t hwmask_len = 0;
    uint32_t hwdata_len = 0;
    uint8_t  *hwkey = NULL;
    uint8_t  *hwmask = NULL;
    p4pd_gft_hwentry_query(P4_GFT_TBL_ID_INGRESS_KEY4, &hwkey_len, &hwmask_len,
                           &hwdata_len);
    hwkey_len = (hwkey_len >> 3) + ((hwkey_len & 0x7) ? 1 : 0);
    hwmask_len = (hwmask_len >> 3) + ((hwmask_len & 0x7) ? 1 : 0);
    hwdata_len = (hwdata_len >> 3) + ((hwdata_len & 0x7) ? 1 : 0);
    hwkey = new uint8_t[hwkey_len];
    hwmask = new uint8_t[hwmask_len];
    memset(hwkey, 0, hwkey_len);
    memset(hwmask, 0, hwmask_len);
    p4pd_gft_hwkey_hwmask_build(P4_GFT_TBL_ID_INGRESS_KEY4, &key, &mask,
                                hwkey, hwmask);
    p4pd_gft_entry_write(P4_GFT_TBL_ID_INGRESS_KEY4, 0, hwkey, hwmask, &data);
    delete [] hwkey;
    delete [] hwmask;
}

static void
ingress_key_init() {
    ingress_key1_init();
    ingress_key2_init();
    ingress_key3_init();
    ingress_key4_init();
}

static void
create_vport_entry() {
    ingress_vport_swkey_t      key;
    ingress_vport_swkey_mask_t mask;
    ingress_vport_actiondata   data;

    memset(&key, 0, sizeof(key));
    memset(&mask, 0, sizeof(mask));
    memset(&data, 0, sizeof(data));

    // key
    key.ethernet_1_valid = 0xFF;
    memcpy(key.ethernet_1_dstAddr, &g_layer1_dmac, 6);

    // mask
    mask.ethernet_1_valid_mask = 0xFF;
    memset(mask.ethernet_1_dstAddr_mask, 0xFF, 6);

    // data
    data.ingress_vport_action_u.ingress_vport_ingress_vport.vport = 0x55;

    // prepare entry and write hardware
    uint32_t hwkey_len = 0;
    uint32_t hwmask_len = 0;
    uint32_t hwdata_len = 0;
    uint8_t  *hwkey = NULL;
    uint8_t  *hwmask = NULL;
    p4pd_gft_hwentry_query(P4_GFT_TBL_ID_INGRESS_VPORT, &hwkey_len, &hwmask_len,
                           &hwdata_len);
    hwkey_len = (hwkey_len >> 3) + ((hwkey_len & 0x7) ? 1 : 0);
    hwmask_len = (hwmask_len >> 3) + ((hwmask_len & 0x7) ? 1 : 0);
    hwdata_len = (hwdata_len >> 3) + ((hwdata_len & 0x7) ? 1 : 0);
    hwkey = new uint8_t[hwkey_len];
    hwmask = new uint8_t[hwmask_len];
    memset(hwkey, 0, hwkey_len);
    memset(hwmask, 0, hwmask_len);
    p4pd_gft_hwkey_hwmask_build(P4_GFT_TBL_ID_INGRESS_VPORT, &key, &mask,
                                hwkey, hwmask);
    p4pd_gft_entry_write(P4_GFT_TBL_ID_INGRESS_VPORT, 0, hwkey, hwmask, &data);
    delete [] hwkey;
    delete [] hwmask;
}

static void
create_gft_entry1() {
    gft_hash_swkey_t     key;
    gft_hash_actiondata  data;
    uint32_t             hwkey_len = 0;
    uint32_t             hwdata_len = 0;
    uint32_t             hash = 0;
    uint32_t             hash_len = 0;
    uint32_t             gft_idx = 0;
    uint8_t              *hwkey = NULL;

    memset(&key, 0, sizeof(key));
    memset(&data, 0, sizeof(data));

    // key
    memcpy(key.flow_lkp_metadata_ethernet_dst_1, &g_layer1_dmac, 6);
    memcpy(key.flow_lkp_metadata_ethernet_src_1, &g_layer1_smac, 6);
    memcpy(key.flow_lkp_metadata_ip_dst_1, &g_layer1_dip, 4);
    memcpy(key.flow_lkp_metadata_ip_src_1, &g_layer1_sip, 4);
    key.flow_lkp_metadata_ip_proto_1 = g_layer1_proto;
    memcpy(key.flow_lkp_metadata_ethernet_dst_2, &g_layer2_dmac, 6);
    memcpy(key.flow_lkp_metadata_ethernet_src_2, &g_layer2_smac, 6);
    memcpy(key.flow_lkp_metadata_ip_dst_2, &g_layer2_dip, 4);
    memcpy(key.flow_lkp_metadata_ip_src_2, &g_layer2_sip, 4);
    key.flow_lkp_metadata_ip_proto_2 = g_layer2_proto;
    memcpy(key.flow_lkp_metadata_ethernet_dst_3, &g_layer3_dmac, 6);
    memcpy(key.flow_lkp_metadata_ethernet_src_3, &g_layer3_smac, 6);
    memcpy(key.flow_lkp_metadata_ip_dst_3, &g_layer3_dip, 4);
    memcpy(key.flow_lkp_metadata_ip_src_3, &g_layer3_sip, 4);
    key.flow_lkp_metadata_ip_proto_3 = g_layer3_proto;
    key.flow_lkp_metadata_l4_sport_3 = g_layer31_sport;
    key.flow_lkp_metadata_l4_dport_3 = g_layer31_dport;

    // data
    data.gft_hash_action_u.gft_hash_gft_hash_info.entry_valid = 1;
    data.gft_hash_action_u.gft_hash_gft_hash_info.flow_index = 0xA32;

    // build hardware entry
    p4pd_gft_hwentry_query(P4_GFT_TBL_ID_GFT_HASH, &hwkey_len, NULL,
                           &hwdata_len);
    hwkey_len = (hwkey_len >> 3) + ((hwkey_len & 0x7) ? 1 : 0);
    hwdata_len = (hwdata_len >> 3) + ((hwdata_len & 0x7) ? 1 : 0);
    hash_len = hwkey_len;
    if (hash_len % 64) {
        hash_len +=  (64 - (hwkey_len % 64));
    }
    hwkey = new uint8_t[hash_len];
    memset(hwkey, 0, hash_len);
    p4pd_gft_hwkey_hwmask_build(P4_GFT_TBL_ID_GFT_HASH, &key, NULL,
                                hwkey, NULL);

    // generate hash
    uint32_t crc_init_val = 0;
    for (uint32_t i = 0; i < hash_len; i += 64) {
        hash = generate_hash_(hwkey + i, 64, crc_init_val);
        printf("HASH(%d) : 0x%0x\n", i, hash);
        crc_init_val = hash;
    }
    hash = generate_hash_(hwkey, hash_len, 0);
    gft_idx = hash & 0xFFFFF;
    printf("Final hash : 0x%0x, index : 0x%0x\n", hash, gft_idx);

    p4pd_gft_entry_write(P4_GFT_TBL_ID_GFT_HASH, gft_idx, hwkey, NULL, &data);
    delete [] hwkey;
}

static void
create_gft_entry2() {
    gft_hash_swkey_t     key;
    gft_hash_actiondata  data;
    uint32_t             hwkey_len = 0;
    uint32_t             hwdata_len = 0;
    uint32_t             hash = 0;
    uint32_t             hash_len = 0;
    uint32_t             gft_idx = 0;
    uint8_t              *hwkey = NULL;

    memset(&key, 0, sizeof(key));
    memset(&data, 0, sizeof(data));

    // key
    memcpy(key.flow_lkp_metadata_ethernet_dst_1, &g_layer1_dmac, 6);
    memcpy(key.flow_lkp_metadata_ethernet_src_1, &g_layer1_smac, 6);
    memcpy(key.flow_lkp_metadata_ip_dst_1, &g_layer1_dip, 4);
    memcpy(key.flow_lkp_metadata_ip_src_1, &g_layer1_sip, 4);
    key.flow_lkp_metadata_ip_proto_1 = g_layer1_proto;
    memcpy(key.flow_lkp_metadata_ethernet_dst_2, &g_layer2_dmac, 6);
    memcpy(key.flow_lkp_metadata_ethernet_src_2, &g_layer2_smac, 6);
    memcpy(key.flow_lkp_metadata_ip_dst_2, &g_layer2_dip, 4);
    memcpy(key.flow_lkp_metadata_ip_src_2, &g_layer2_sip, 4);
    key.flow_lkp_metadata_ip_proto_2 = g_layer2_proto;
    memcpy(key.flow_lkp_metadata_ethernet_dst_3, &g_layer3_dmac, 6);
    memcpy(key.flow_lkp_metadata_ethernet_src_3, &g_layer3_smac, 6);
    memcpy(key.flow_lkp_metadata_ip_dst_3, &g_layer3_dip, 4);
    memcpy(key.flow_lkp_metadata_ip_src_3, &g_layer3_sip, 4);
    key.flow_lkp_metadata_ip_proto_3 = g_layer3_proto;
    key.flow_lkp_metadata_l4_sport_3 = g_layer32_sport;
    key.flow_lkp_metadata_l4_dport_3 = g_layer32_dport;

    // data
    data.gft_hash_action_u.gft_hash_gft_hash_info.entry_valid = 1;
    data.gft_hash_action_u.gft_hash_gft_hash_info.flow_index = 0;
    data.gft_hash_action_u.gft_hash_gft_hash_info.hint9 = g_ohash_idx;

    // build hardware entry
    p4pd_gft_hwentry_query(P4_GFT_TBL_ID_GFT_HASH, &hwkey_len, NULL,
                           &hwdata_len);
    hwkey_len = (hwkey_len >> 3) + ((hwkey_len & 0x7) ? 1 : 0);
    hwdata_len = (hwdata_len >> 3) + ((hwdata_len & 0x7) ? 1 : 0);
    hash_len = hwkey_len;
    if (hash_len % 64) {
        hash_len +=  (64 - (hwkey_len % 64));
    }
    hwkey = new uint8_t[hash_len];
    memset(hwkey, 0, hash_len);
    p4pd_gft_hwkey_hwmask_build(P4_GFT_TBL_ID_GFT_HASH, &key, NULL,
                                hwkey, NULL);

    // generate hash
    uint32_t crc_init_val = 0;
    for (uint32_t i = 0; i < hash_len; i += 64) {
        hash = generate_hash_(hwkey + i, 64, crc_init_val);
        printf("HASH(%d) : 0x%0x\n", i, hash);
        crc_init_val = hash;
    }
    hash = generate_hash_(hwkey, hash_len, 0);
    gft_idx = hash & 0xFFFFF;
    printf("Final hash : 0x%0x, index : 0x%0x\n", hash, gft_idx);

    p4pd_gft_entry_write(P4_GFT_TBL_ID_GFT_HASH, gft_idx, hwkey, NULL, &data);
    delete [] hwkey;
}

static void
create_gft_overflow_entry() {
    gft_hash_swkey_t     key;
    gft_hash_actiondata  data;
    uint32_t             hwkey_len = 0;
    uint32_t             hwdata_len = 0;
    uint8_t              *hwkey = NULL;

    memset(&key, 0, sizeof(key));
    memset(&data, 0, sizeof(data));

    // key
    memcpy(key.flow_lkp_metadata_ethernet_dst_1, &g_layer1_dmac, 6);
    memcpy(key.flow_lkp_metadata_ethernet_src_1, &g_layer1_smac, 6);
    memcpy(key.flow_lkp_metadata_ip_dst_1, &g_layer1_dip, 4);
    memcpy(key.flow_lkp_metadata_ip_src_1, &g_layer1_sip, 4);
    key.flow_lkp_metadata_ip_proto_1 = g_layer1_proto;
    memcpy(key.flow_lkp_metadata_ethernet_dst_2, &g_layer2_dmac, 6);
    memcpy(key.flow_lkp_metadata_ethernet_src_2, &g_layer2_smac, 6);
    memcpy(key.flow_lkp_metadata_ip_dst_2, &g_layer2_dip, 4);
    memcpy(key.flow_lkp_metadata_ip_src_2, &g_layer2_sip, 4);
    key.flow_lkp_metadata_ip_proto_2 = g_layer2_proto;
    memcpy(key.flow_lkp_metadata_ethernet_dst_3, &g_layer3_dmac, 6);
    memcpy(key.flow_lkp_metadata_ethernet_src_3, &g_layer3_smac, 6);
    memcpy(key.flow_lkp_metadata_ip_dst_3, &g_layer3_dip, 4);
    memcpy(key.flow_lkp_metadata_ip_src_3, &g_layer3_sip, 4);
    key.flow_lkp_metadata_ip_proto_3 = g_layer3_proto;
    key.flow_lkp_metadata_l4_sport_3 = g_layer32_sport;
    key.flow_lkp_metadata_l4_dport_3 = g_layer32_dport;

    // data
    data.gft_hash_action_u.gft_hash_gft_hash_info.entry_valid = 1;
    data.gft_hash_action_u.gft_hash_gft_hash_info.flow_index = 0xA34;

    // build hardware entry
    p4pd_gft_hwentry_query(P4_GFT_TBL_ID_GFT_HASH_OVERFLOW, &hwkey_len, NULL,
                           &hwdata_len);
    hwkey_len = (hwkey_len >> 3) + ((hwkey_len & 0x7) ? 1 : 0);
    hwdata_len = (hwdata_len >> 3) + ((hwdata_len & 0x7) ? 1 : 0);
    hwkey = new uint8_t[hwkey_len];
    memset(hwkey, 0, hwkey_len);
    p4pd_gft_hwkey_hwmask_build(P4_GFT_TBL_ID_GFT_HASH_OVERFLOW, &key, NULL,
                                hwkey, NULL);

    p4pd_gft_entry_write(P4_GFT_TBL_ID_GFT_HASH_OVERFLOW, g_ohash_idx, hwkey,
                         NULL, &data);
    delete [] hwkey;
}
#endif

class gft_test : public ::testing::Test {
  protected:
    gft_test() {}
    virtual ~gft_test() {}
    virtual void SetUp() {}
    virtual void TearDown() {}
};

TEST_F(gft_test, test1) {
#ifdef GFT
    int ret = 0;
    uint64_t asm_base_addr;
    printf("Connecting to ASIC SIM\n");
    ret = lib_model_connect();
    ASSERT_NE(ret, -1);
    ret = capri_load_config((char *)"obj/gft/pgm_bin");
    ASSERT_NE(ret, -1);
    ret = capri_hbm_parse();
    ASSERT_NE(ret, -1);
    asm_base_addr = (uint64_t)get_start_offset((char *)JP4_PRGM);
    ret = capri_load_mpu_programs("gft", (char *)"obj/gft/asm_bin",
                                  asm_base_addr, NULL, 0);
    ASSERT_NE(ret, -1);
    ret = p4pd_init();
    ASSERT_NE(ret, -1);
    ret = capri_table_rw_init();
    ASSERT_NE(ret, -1);
    ingress_key_init();
    create_vport_entry();
    create_gft_entry1();
    create_gft_entry2();
    create_gft_overflow_entry();

    uint32_t port = 0;
    uint32_t cos = 0;
    std::vector<uint8_t> ipkt;
    std::vector<uint8_t> opkt;
    std::vector<uint8_t> epkt;

    ipkt.resize(sizeof(g_snd_pkt1));
    memcpy(ipkt.data(), g_snd_pkt1, sizeof(g_snd_pkt1));
    epkt.resize(sizeof(g_rcv_pkt1));
    memcpy(epkt.data(), g_rcv_pkt1, sizeof(g_rcv_pkt1));
    std::cout << "Testing wide key without overflow" << std::endl;
    step_network_pkt(ipkt, port);
    get_next_pkt(opkt, port, cos);
    EXPECT_TRUE(opkt == epkt);

    ipkt.resize(sizeof(g_snd_pkt2));
    memcpy(ipkt.data(), g_snd_pkt2, sizeof(g_snd_pkt2));
    epkt.resize(sizeof(g_rcv_pkt2));
    memcpy(epkt.data(), g_rcv_pkt2, sizeof(g_rcv_pkt2));
    std::cout << "Testing wide key with overflow" << std::endl;
    step_network_pkt(ipkt, port);
    get_next_pkt(opkt, port, cos);
    EXPECT_TRUE(opkt == epkt);
#endif
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
