
#include "INGRESS_p.h"
#include "ingress.h"
#include "defines.h"

struct phv_ p;
struct tx_table_s1_t0_k k;
struct tx_table_s1_t0_eth_tx_packet_d d;

%%

.align
eth_tx_packet:

#if 1
    add         r7, r0, d.{addr}.dx
    add         r7, r0, d.{len}.hx
#endif

    // Set intrinsics
    phvwri      p.p4_intr_global_tm_iport, 9
    phvwri      p.p4_intr_global_tm_oport, 11
    phvwri      p.p4_intr_global_tm_oq, 0

    // Setup DMA CMD PTR
    phvwri      p.p4_txdma_intr_dma_cmd_ptr, ETH_DMA_CMD_PTR

    // End of pipeline - Make sure no more tables will be launched
    phvwri      p.{app_header_table0_valid...app_header_table3_valid}, 0

    // DMA cap_phv_intr_global_t (18B)
    phvwri      p.dma_cmd0_dma_cmd_type, CAPRI_DMA_COMMAND_PHV_TO_PKT
    phvwri      p.dma_cmd0_dma_cmd_phv_start_addr, 0
    phvwri      p.dma_cmd0_dma_cmd_phv_end_addr, 16
    phvwri      p.dma_cmd0_dma_pkt_eop, 0
    phvwri      p.dma_cmd0_dma_cmd_eop, 0

    // DMA p4plus_to_p4_header_t (14B)
    phvwri      p.dma_cmd1_dma_cmd_type, CAPRI_DMA_COMMAND_PHV_TO_PKT
    phvwri      p.dma_cmd1_dma_cmd_phv_start_addr, 32
    phvwri      p.dma_cmd1_dma_cmd_phv_end_addr, 45
    phvwri      p.dma_cmd1_dma_pkt_eop, 0
    phvwri      p.dma_cmd1_dma_cmd_eop, 0

    // DMA packet from Host Memory
    phvwri      p.dma_cmd2_dma_cmd_type, CAPRI_DMA_COMMAND_MEM_TO_PKT
    phvwri      p.dma_cmd2_dma_cmd_eop, 0
    phvwri      p.dma_cmd2_dma_pkt_eop, 1
    phvwri      p.dma_cmd2_dma_cmd_host_addr, 1
    phvwri      p.dma_cmd2_dma_cmd_cache, 0
    phvwr       p.dma_cmd2_dma_cmd_addr, d.{addr}.dx
    phvwr       p.dma_cmd2_dma_cmd_size, d.{len}.hx
    phvwri      p.dma_cmd2_dma_cmd_use_override_lif, 0
    phvwri      p.dma_cmd2_dma_cmd_override_lif, 0

    //DMA CQ descriptor to Host Memory
    phvwri      p.dma_cmd3_dma_cmd_type, CAPRI_DMA_COMMAND_PHV_TO_MEM
    phvwri      p.dma_cmd3_dma_cmd_eop, 1
    phvwri      p.dma_cmd3_dma_cmd_host_addr, 1
    phvwri      p.dma_cmd3_dma_cmd_cache, 0
    phvwr       p.dma_cmd3_dma_cmd_addr, k.eth_tx_to_s1_cq_desc_addr
    phvwri      p.dma_cmd3_dma_cmd_wr_fence, 0
    phvwri      p.dma_cmd3_dma_cmd_phv_start_addr, CAPRI_PHV_START_OFFSET(eth_tx_cq_desc_completion_index)
    phvwri      p.dma_cmd3_dma_cmd_phv_end_addr, CAPRI_PHV_END_OFFSET(eth_tx_cq_desc_color)
    phvwri      p.dma_cmd3_dma_cmd_use_override_lif, 0
    phvwri      p.dma_cmd3_dma_cmd_override_lif, 0
    phvwri      p.dma_cmd3_dma_cmd_barrier, 0
    phvwri      p.dma_cmd3_dma_cmd_pcie_msg, 0
    phvwri      p.dma_cmd3_dma_cmd_round, 0
    nop.e
    nop
