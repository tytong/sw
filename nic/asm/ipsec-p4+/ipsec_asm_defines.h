#include "capri-macros.h"

#define IPSEC_CB_SHIFT_SIZE 7
#define IPHDR_SHIFT_SIZE 6 

#define IPSEC_CB_IV_OFFSET 30 
#define IPSEC_IP_HDR_OFFSET 64
#define IPSEC_H2N_STATS_CB_OFFSET 192
#define IPSEC_N2H_STATS_CB_OFFSET 192
// 20 (outer-IP) + 16 (ESP-Header) + 2 (L4proto+pad_size) + 16 (ICV Trailer)
#define IPV4_FIXED_TUNNEL_MODE_GROWTH 54 

#define IPSEC_CB_BASE ipsec_cb_base
#define IPSEC_PAD_BYTES_HBM_TABLE_BASE ipsec_pad_table_base
#define IPSEC_IP_HDR_BASE ipsec_ip_hdr_base

#define IPSEC_CB_RING_BASE_SHIFT_SIZE 9
#define IPSEC_CB_RING_ENTRY_SHIFT_SIZE 3
#define IPSEC_DESC_RING_ENTRY_SHIFT_SIZE 3
#define IPSEC_CB_RING_INDEX_MASK 0x3FF

#define IPSEC_BARCO_RING_ENTRY_SHIFT_SIZE 7
#define IPSEC_BARCO_RING_INDEX_MASK 0x3FF
#define IPSEC_DESC_RING_INDEX_MASK 0xFFF
#define IPSEC_PAGE_RING_INDEX_MASK 0xFFF
#define IPSEC_DESC_RING_SIZE  (IPSEC_DESC_RING_INDEX_MASK + 1)

#define INDESC_SEMAPHORE_ADDR   IPSEC_RNMDR_ALLOC_IDX 
#define OUTDESC_SEMAPHORE_ADDR  IPSEC_TNMDR_ALLOC_IDX 
#define INPAGE_SEMAPHORE_ADDR   IPSEC_RNMPR_ALLOC_IDX 
#define OUTPAGE_SEMAPHORE_ADDR  IPSEC_TNMPR_ALLOC_IDX

#define IPSEC_H2N_SEM_CINDEX_OFFSET 1024
#define IPSEC_N2H_SEM_CINDEX_OFFSET 512 

#define IPSEC_N2H_GLOBAL_FLAGS 1

#define IPSEC_N2H_GLOBAL_FLAGS_BAD_SIG 1

#define IPSEC_DESC_FULL_DESC_ADDR 0xFFFFFFFF

#define RING_INDEX_WIDTH 12

#define IPSEC_PAGE_OFFSET 128
#define IPSEC_DEFAULT_IV_SIZE 8
#define IPSEC_DEFAULT_ICV_SIZE 16


#define IPSEC_PER_CB_RING_WIDTH 10
#define IPSEC_BARCO_RING_WIDTH  10 


#define IPSEC_PROTO_IP     4
#define ESP_FIXED_HDR_SIZE 8
#define ESP_FIXED_HDR_SIZE_LI 0x08000000 
#define ETH_FIXED_HDR_SIZE 18
#define ETH_FIXED_HDR_SIZE_NO_VLAN 14
#define UDP_FIXED_HDR_SIZE 8
#define IPV4_HDR_SIZE 20
#define IPV6_HDR_SIZE 40


#define N2H_IN_DESC_OFFSET            0
#define N2H_OUT_DESC_OFFSET           8
#define N2H_CB_RING_OFFSET            16
#define N2H_IN_PAGE_OFFSET            24
#define N2H_BARCO_REQ_OFFSET          32
#define N2H_BARCO_CB_OFFSET           40
#define N2H_S4_IN_PAGE_OFFSET         48
#define N2H_OUT_PAGE_OFFSET           56 
#define N2H_TXDMA1_ENTER_OFFSET       64
#define N2H_TXDMA2_ENTER_OFFSET       72 
#define N2H_TXDMA1_ENTER_DROP_OFFSET  80
#define N2H_DESC_RING_OFFSET          88
#define N2H_ABORT_TXDMA1_DUMMY_OFFSET 96
#define N2H_LOAD_IPSEC_INT_OFFSET     104
#define N2H_TXDMA2_DUMMY_FREE         112
#define N2H_RXDMA_DUMMY_DESC_OFFSET   120
#define N2H_RXDMA_ENTER_OFFSET        128
#define N2H_BAD_BARCO_IN_DESC         136
#define N2H_BAD_BARCO_OUT_DESC        144
#define N2H_TXDMA2_BAD_INDESC_FREE    152
#define N2H_TXDMA2_BAD_OUTDESC_FREE   160 
#define N2H_TXDMA1_SEM_FREE           168 
#define N2H_TXDMA2_SEM_FREE           176 
#define N2H_TXDMA1_BAD_INDESC_FREE    184
#define N2H_TXDMA1_BAD_OUTDESC_FREE   192
#define N2H_RXDMA_CB_RING_FULL_OFFSET 200
#define N2H_TXDMA1_BARCO_RING_FULL_OFFSSET 208


#define N2H_RXDMA_FREEZE_OFFSET       320 
#define N2H_TXDMA1_FREEZE_OFFSET      328 
#define N2H_TXDMA2_FREEZE_OFFSET      336 
 

#define H2N_IN_DESC_OFFSET             0
#define H2N_OUT_DESC_OFFSET            8
#define H2N_CB_RING_OFFSET            16
#define H2N_IN_PAGE_OFFSET            24
#define H2N_BARCO_REQ_OFFSET          32
#define H2N_BARCO_CB_OFFSET           40
#define H2N_PAD_ADDR_OFFSET           48
#define H2N_TAIL_BYTES_OFFSET         56
#define H2N_OUT_PAGE_OFFSET           64
#define H2N_S4_IN_PAGE_OFFSET         72
#define H2N_T3_IN_PAGE_OFFSET         80
#define H2N_T2_IN_PAGE_OFFSET         88 
#define H2N_T0_IN_PAGE_OFFSET         96
#define H2N_BAD_BARCO_ADDR_OFF       104
#define H2N_BARCO_FULL_OFFSET        112
#define H2N_CB_RING_DMA_OFFSET       120
#define H2N_DESC_EXHAUST_OFFSET      128
#define H2N_TXDMA1_ENTER_OFFSET      136 
#define H2N_TXDMA2_ENTER_OFFSET      144 
#define H2N_TXDMA1_BAD_INDESC_FROM_CB 152
#define H2N_RXDMA_DUMMY_DESC_OFFSET  160
#define H2N_RXDMA_ENTER_OFFSET       168
#define H2N_BAD_BARCO_IN_DESC        176
#define H2N_BAD_BARCO_OUT_DESC       184
#define H2N_TXDMA2_BAD_INDESC_FREE   192
#define H2N_TXDMA2_BAD_OUTDESC_FREE  200
#define H2N_TXDMA1_BAD_INDESC_FREE   208 
#define H2N_TXDMA1_BAD_OUTDESC_FREE  216
#define H2N_TXDMA1_SEM_FREE          224
#define H2N_TXDMA2_SEM_FREE          232
#define H2N_TXDMA1_BARCO_RING_FULL   240
#define H2N_RXDMA_CB_RING_FULL_OFFSET 248

#define H2N_RXDMA_FREEZE_OFFSET      320
#define H2N_TXDMA1_FREEZE_OFFSET     328
#define H2N_TXDMA2_FREEZE_OFFSET     336 

#define DOT1Q_ETYPE  0x8100
#define IPV4_ETYPE   0x0800
#define IPV6_ETYPE   0x86dd

#define IPSEC_RXDMA_HW_SW_INTRINSIC_SIZE 42 
#define IPSEC_TXDMA_HW_INTRINSIC_SIZE 31 

#define IPSEC_SALT_HEADROOM 4

#define IPSEC_INT_START_OFFSET  CAPRI_PHV_START_OFFSET(ipsec_int_header_in_desc) 
#define IPSEC_INT_END_OFFSET    CAPRI_PHV_END_OFFSET(ipsec_int_pad_status)
 
#define IPSEC_N2H_INT_START_OFFSET  CAPRI_PHV_START_OFFSET(ipsec_int_header_in_desc) 
#define IPSEC_N2H_INT_END_OFFSET    CAPRI_PHV_END_OFFSET(ipsec_int_header_status_addr)
 
#define IPSEC_IN_DESC_AOL_START CAPRI_PHV_START_OFFSET(barco_desc_in_A0_addr) 
#define IPSEC_IN_DESC_AOL_END   CAPRI_PHV_END_OFFSET(barco_desc_in_L1) 

#define IPSEC_OUT_DESC_AOL_START CAPRI_PHV_START_OFFSET(barco_desc_out_A0_addr) 
#define IPSEC_OUT_DESC_AOL_END   CAPRI_PHV_END_OFFSET(barco_desc_out_L1)

#define IPSEC_DESC_ZERO_CONTENT_START CAPRI_PHV_START_OFFSET(barco_zero_A2_addr)
#define IPSEC_DESC_ZERO_CONTENT_END  CAPRI_PHV_END_OFFSET(barco_zero_rsvd)

  
#define IPSEC_PHV_RXDMA_DB_DATA_START CAPRI_PHV_START_OFFSET(db_data_pid) 
#define IPSEC_PHV_RXDMA_DB_DATA_END   CAPRI_PHV_END_OFFSET(db_data_index) 


#define IPSEC_TXDMA1_BARCO_REQ_PHV_OFFSET_START CAPRI_PHV_START_OFFSET(barco_req_input_list_address) 
#define IPSEC_TXDMA1_BARCO_REQ_PHV_OFFSET_END   CAPRI_PHV_END_OFFSET(barco_req_secondary_key_index)

#define IPSEC_TXDMA1_BARCO_CB_REQ_PHV_OFFSET_START CAPRI_PHV_START_OFFSET(barco_req_input_list_address) 
#define IPSEC_TXDMA1_BARCO_CB_REQ_PHV_OFFSET_END   CAPRI_PHV_END_OFFSET(barco_req_input_list_address)
 
#define IPSEC_PHV_TXDMA1_DB_DATA_START CAPRI_PHV_START_OFFSET(db_data_pid) 
#define IPSEC_PHV_TXDMA1_DB_DATA_END   CAPRI_PHV_END_OFFSET(db_data_index)

#define IPSEC_BARCO_DOORBELL_OFFSET_START CAPRI_PHV_START_OFFSET(barco_dbell_pi)
#define IPSEC_BARCO_DOORBELL_OFFSET_END CAPRI_PHV_END_OFFSET(barco_dbell_pi)
 

#define IPSEC_TAIL_2_BYTES_PHV_START  CAPRI_PHV_START_OFFSET(ipsec_int_header_pad_size) 
#define IPSEC_TAIL_2_BYTES_PHV_END    CAPRI_PHV_END_OFFSET(ipsec_int_header_l4_protocol) 

#define IPSEC_IN_DESC_IV_SALT_START CAPRI_PHV_START_OFFSET(ipsec_to_stage3_iv_salt)
#define IPSEC_IN_DESC_IV_SALT_END   CAPRI_PHV_END_OFFSET(ipsec_to_stage3_iv_salt)

#define IPSEC_IN_DESC_IV_START CAPRI_PHV_START_OFFSET(esp_header_iv)
#define IPSEC_IN_DESC_IV_END   CAPRI_PHV_END_OFFSET(esp_header_iv)
#define IPSEC_IN_DESC_IV2_END   CAPRI_PHV_END_OFFSET(esp_header_iv2)

#define IPSEC_H2N_CB_RING_IN_DESC_START CAPRI_PHV_START_OFFSET(ipsec_global_in_desc_addr)
#define IPSEC_H2N_CB_RING_IN_DESC_END CAPRI_PHV_END_OFFSET(ipsec_global_in_desc_addr)

#define IPSEC_CB_RING_IN_DESC_START CAPRI_PHV_START_OFFSET(t0_s2s_in_desc_addr)
#define IPSEC_CB_RING_IN_DESC_END CAPRI_PHV_END_OFFSET(t0_s2s_in_desc_addr)

#define IPSEC_ESP_HDR_PHV_START CAPRI_PHV_START_OFFSET(esp_header_spi)
#define IPSEC_ESP_HDR_PHV_END   CAPRI_PHV_END_OFFSET(esp_header_iv)
#define IPSEC_ESP2_HDR_PHV_END   CAPRI_PHV_END_OFFSET(esp_header_iv2)

#define IPSEC_TXDMA2_APP_HEADER_START CAPRI_PHV_START_OFFSET(p4_txdma_intr_qid)
#define IPSEC_TXDMA2_APP_HEADER_END   CAPRI_PHV_END_OFFSET(p4plus2p4_hdr_vlan_tag)

#define IPSEC_TXDMA2_VRF_VLAN_HEADER_START CAPRI_PHV_START_OFFSET(ipsec_to_stage4_dot1q_etype)
#define IPSEC_TXDMA2_VRF_VLAN_HEADER_END   CAPRI_PHV_END_OFFSET(ipsec_to_stage4_ip_etype)

#define H2N_RXDMA_IPSEC_DMA_COMMANDS_OFFSET (CAPRI_PHV_START_OFFSET(dma_cmd_pkt2mem_dma_cmd_type) / 16) 
#define H2N_TXDMA1_DMA_COMMANDS_OFFSET  (CAPRI_PHV_START_OFFSET(brq_in_desc_zero_dma_cmd_type) / 16) 
#define H2N_TXDMA2_DMA_COMMANDS_OFFSET (CAPRI_PHV_START_OFFSET(intrinsic_app_hdr_dma_cmd_type) / 16) 
 
#define N2H_RXDMA_IPSEC_DMA_COMMANDS_OFFSET (CAPRI_PHV_START_OFFSET(dma_cmd_pkt2mem_dma_cmd_type) / 16) 
#define N2H_TXDMA1_DMA_COMMANDS_OFFSET (CAPRI_PHV_START_OFFSET(brq_in_desc_zero_dma_cmd_type) / 16) 
#define N2H_TXDMA2_DMA_COMMANDS_OFFSET (CAPRI_PHV_START_OFFSET(intrinsic_app_hdr_dma_cmd_type) / 16) 

#define TLS_PROXY_BARCO_GCM0_PI_HBM_TABLE_BASE tls_barco_gcm0_pi_table_base
#define TLS_PROXY_BARCO_GCM1_PI_HBM_TABLE_BASE tls_barco_gcm1_pi_table_base



#define IPSEC_PHV2MEM_CACHE_ENABLE  (1 << 5)
#define IPSEC_PKT2MEM_CACHE_ENABLE  (1 << 6)
#define IPSEC_MEM2MEM_CACHE_ENABLE  (1 << 8)
#define IPSEC_MEM2PKT_CACHE_ENABLE  (1 << 6)


#define BRQ_REQ_SEMAPHORE_ADDR 0x40004040 

#define ESP_BASE_OFFSET 18  

#define BRQ_RING_ENTRY_SIZE_SHIFT 7


#define IPSEC_WIN_REPLAY_MAX_DIFF 63

#define P4PLUS_APPTYPE_IPSEC 4
#define P4PLUS_TO_P4_FLAGS_UPDATE_IP_LEN 2

#define IPSEC_FLAGS_V6_MASK     0x1
#define IPSEC_FLAGS_NATT_MASK   0x2
#define IPSEC_FLAGS_RANDOM_MASK 0x4
#define IPSEC_FLAGS_EXTRA_PAD   0x8
#define IPSEC_ENCAP_VLAN_MASK   0x10
