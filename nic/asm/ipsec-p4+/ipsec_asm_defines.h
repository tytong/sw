#include "capri-macros.h"

#define IPSEC_CB_SHIFT_SIZE 7
#define IPHDR_SHIFT_SIZE 6 

#define IPSEC_CB_IV_OFFSET 30 
#define IPSEC_IP_HDR_OFFSET 64

#define IPSEC_CB_BASE ipsec_cb_base
#define IPSEC_PAD_BYTES_HBM_TABLE_BASE ipsec_pad_table_base
#define IPSEC_IP_HDR_BASE ipsec_ip_hdr_base

#define IPSEC_CB_RING_BASE_SHIFT_SIZE 9
#define IPSEC_CB_RING_ENTRY_SHIFT_SIZE 3
#define IPSEC_CB_RING_INDEX_MASK 0xFF

#define IPSEC_BARCO_RING_ENTRY_SHIFT_SIZE 7
#define IPSEC_BARCO_RING_INDEX_MASK 0x3FF
#define IPSEC_DESC_RING_INDEX_MASK 0x3FF
#define IPSEC_PAGE_RING_INDEX_MASK 0x3FF

#define INDESC_SEMAPHORE_ADDR   RNMDR_ALLOC_IDX 
#define OUTDESC_SEMAPHORE_ADDR  TNMDR_ALLOC_IDX 
#define INPAGE_SEMAPHORE_ADDR   RNMPR_ALLOC_IDX 
#define OUTPAGE_SEMAPHORE_ADDR  TNMPR_ALLOC_IDX

#define RING_INDEX_WIDTH 16

#define IPSEC_PER_CB_RING_WIDTH 10
#define IPSEC_BARCO_RING_WIDTH  10 


#define ESP_FIXED_HDR_SIZE 8
#define ESP_FIXED_HDR_SIZE_LI 0x08000000 
#define ETH_FIXED_HDR_SIZE 18
#define ETH_FIXED_HDR_SIZE_NO_VLAN 14
#define UDP_FIXED_HDR_SIZE 8
#define IPV4_HDR_SIZE 20
#define IPV6_HDR_SIZE 40


#define DOT1Q_ETYPE  0x8100
#define IPV4_ETYPE   0x0800
#define IPV6_ETYPE   0x86dd

#define IPSEC_RXDMA_HW_SW_INTRINSIC_SIZE 42 
#define IPSEC_TXDMA_HW_INTRINSIC_SIZE 31 

#define IPSEC_SALT_HEADROOM 4

#define IPSEC_INT_START_OFFSET  CAPRI_PHV_START_OFFSET(ipsec_int_header_in_desc) 
#define IPSEC_INT_END_OFFSET    CAPRI_PHV_END_OFFSET(ipsec_int_header_l4_protocol)
 
#define IPSEC_N2H_INT_START_OFFSET  CAPRI_PHV_START_OFFSET(ipsec_int_header_in_desc) 
#define IPSEC_N2H_INT_END_OFFSET    CAPRI_PHV_END_OFFSET(ipsec_int_header_spi)
 
#define IPSEC_IN_DESC_AOL_START CAPRI_PHV_START_OFFSET(barco_desc_in_A0_addr) 
#define IPSEC_IN_DESC_AOL_END   CAPRI_PHV_END_OFFSET(barco_desc_in_L1) 

#define IPSEC_OUT_DESC_AOL_START CAPRI_PHV_START_OFFSET(barco_desc_out_A0_addr) 
#define IPSEC_OUT_DESC_AOL_END   CAPRI_PHV_END_OFFSET(barco_desc_out_L1)

#define IPSEC_DESC_ZERO_CONTENT_START CAPRI_PHV_START_OFFSET(barco_zero_A2_addr)
#define IPSEC_DESC_ZERO_CONTENT_END  CAPRI_PHV_END_OFFSET(barco_zero_rsvd)

  
#define IPSEC_PHV_RXDMA_DB_DATA_START CAPRI_PHV_START_OFFSET(db_data_pid) 
#define IPSEC_PHV_RXDMA_DB_DATA_END   CAPRI_PHV_END_OFFSET(db_data_index) 


#define IPSEC_TXDMA1_BARCO_REQ_PHV_OFFSET_START CAPRI_PHV_START_OFFSET(barco_req_input_list_address) 
#define IPSEC_TXDMA1_BARCO_REQ_PHV_OFFSET_END   CAPRI_PHV_END_OFFSET(barco_req_doorbell_data)

 
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


#define BRQ_REQ_SEMAPHORE_ADDR 0x40004040 

#define ESP_BASE_OFFSET 18  

#define BRQ_RING_ENTRY_SIZE_SHIFT 7


#define IPSEC_WIN_REPLAY_MAX_DIFF 63

#define ARM_CPU_LIF    1003
#define LIF_IPSEC_ESP  1004
#define LIF_IPSEC_AH   1005

#define P4PLUS_APPTYPE_IPSEC 4
#define P4PLUS_TO_P4_FLAGS_UPDATE_IP_LEN 2

#define IPSEC_FLAGS_V6_MASK     0x1
#define IPSEC_FLAGS_NATT_MASK   0x2
#define IPSEC_FLAGS_RANDOM_MASK 0x4
#define IPSEC_FLAGS_EXTRA_PAD   0x8
#define IPSEC_ENCAP_VLAN_MASK   0x10
