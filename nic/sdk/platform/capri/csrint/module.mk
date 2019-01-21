# {C} Copyright 2018 Pensando Systems Inc. All rights reserved
include ${MKDEFS}/pre.mk
MODULE_TARGET   = libsdkcapri_csrint.so
MODULE_DEFS     = ${NIC_CSR_DEFINES}
MODULE_FLAGS    = -DCAPRI_SW ${NIC_CSR_FLAGS}
MODULE_SOLIBS   := sdkcapri_asicrw_if
MODULE_EXCLUDE_FLAGS = -O2
MODULE_INCS     = ${NIC_CSR_INCS} ${MODULE_DIR}
MODULE_SRCS     = ${TOPDIR}/nic/asic/capri/model/cap_top/cap_top_csr.cc \
                  ${TOPDIR}/nic/asic/capri/model/utils/cap_csr_base.cc \
                  ${TOPDIR}/nic/asic/capri/model/utils/mem_access.cc \
                  ${TOPDIR}/nic/asic/ip/verif/pcpp/pen_axi4_slave.cc \
                  ${TOPDIR}/nic/asic/ip/verif/pcpp/cpp_int_helper.cc \
                  ${TOPDIR}/nic/asic/ip/verif/pcpp/common_dpi.cc \
                  ${TOPDIR}/nic/asic/capri/design/common/gen/cap_qstate_decoders.cc \
                  ${TOPDIR}/nic/asic/ip/verif/pcpp/axi_xn_db.cc \
                  ${TOPDIR}/nic/asic/capri/model/cap_ptd/cap_ptd_decoders.cc \
                  ${TOPDIR}/nic/asic/capri/design/common/gen/cap_axi_decoders.cc \
                  ${TOPDIR}/nic/asic/ip/verif/pcpp/pen_csr_base.cc \
                  ${TOPDIR}/nic/asic/capri/model/cap_prd/cap_pr_csr.cc \
                  ${TOPDIR}/nic/asic/capri/model/cap_ptd/cap_pt_csr.cc \
                  ${TOPDIR}/nic/asic/capri/model/cap_psp/cap_psp_csr.cc \
                  ${TOPDIR}/nic/asic/capri/model/cap_ppa/cap_ppa_csr.cc \
                  ${TOPDIR}/nic/asic/capri/model/cap_prd/cap_prd_csr.cc \
                  ${TOPDIR}/nic/asic/capri/model/cap_ptd/cap_ptd_csr.cc \
                  ${TOPDIR}/nic/asic/capri/model/cap_te/cap_te_csr.cc \
                  ${TOPDIR}/nic/asic/capri/model/cap_mpu/cap_mpu_csr.cc \
                  ${TOPDIR}/nic/asic/capri/model/cap_pic/cap_pics_csr.cc \
                  ${TOPDIR}/nic/asic/capri/model/cap_pic/cap_picc_csr.cc \
                  ${TOPDIR}/nic/asic/capri/model/cap_pcie/cap_pxb_csr.cc \
                  ${TOPDIR}/nic/asic/capri/model/cap_pcie/cap_pp_csr.cc \
                  ${TOPDIR}/nic/asic/capri/model/cap_npv/cap_npv_csr.cc \
                  ${TOPDIR}/nic/asic/capri/model/cap_pic/cap_pict_csr.cc \
                  ${TOPDIR}/nic/asic/capri/model/cap_txs/cap_txs_csr.cc \
                  ${TOPDIR}/nic/asic/capri/model/cap_pb/cap_pbc_csr.cc \
                  ${TOPDIR}/nic/asic/capri/model/cap_pb/cap_pbm_csr.cc \
                  ${TOPDIR}/nic/asic/capri/model/cap_pb/cap_pbchbmtx_csr.cc \
                  ${TOPDIR}/nic/asic/capri/model/cap_pb/cap_pbchbmeth_csr.cc \
                  ${TOPDIR}/nic/asic/capri/model/cap_pb/cap_pbchbm_csr.cc \
                  ${TOPDIR}/nic/asic/capri/model/cap_pb/cap_pbc_decoders.cc \
                  ${TOPDIR}/nic/asic/capri/model/cap_wa/cap_wa_csr.cc \
                  ${TOPDIR}/nic/asic/capri/model/cap_ms/cap_ms_csr.cc \
                  ${TOPDIR}/nic/asic/capri/model/cap_ms/cap_msr_csr.cc \
                  ${TOPDIR}/nic/asic/capri/model/cap_ms/cap_msh_csr.cc \
                  ${TOPDIR}/nic/asic/capri/model/cap_ms/cap_mss_csr.cc \
                  ${TOPDIR}/nic/asic/capri/model/cap_em/cap_em_csr.cc \
                  ${TOPDIR}/nic/asic/capri/model/cap_em/cap_emm_csr.cc \
                  ${TOPDIR}/nic/asic/capri/model/cap_em/emmc_SDHOST_Memory_Map.cc \
                  ${TOPDIR}/nic/asic/capri/model/cap_ap/cap_ap_csr.cc \
                  ${TOPDIR}/nic/asic/capri/model/cap_ap/cap_apb_csr.cc \
                  ${TOPDIR}/nic/asic/capri/model/cap_he/readonly/cap_hens_csr.cc \
                  ${TOPDIR}/nic/asic/capri/model/cap_he/readonly/cap_hese_csr.cc \
                  ${TOPDIR}/nic/asic/capri/model/cap_he/readonly/cap_mpns_csr.cc \
                  ${TOPDIR}/nic/asic/capri/model/cap_he/readonly/cap_mpse_csr.cc \
                  ${TOPDIR}/nic/asic/capri/model/cap_pcie/cap_pxp_csr.cc \
                  ${TOPDIR}/nic/asic/capri/model/cap_pcie/cap_pxc_csr.cc \
                  ${TOPDIR}/nic/asic/capri/model/cap_sema/cap_sema_csr.cc \
                  ${TOPDIR}/nic/asic/capri/model/cap_intr/cap_intr_csr.cc \
                  ${TOPDIR}/nic/asic/capri/model/cap_bx/cap_bx_csr.cc \
                  ${TOPDIR}/nic/asic/capri/model/cap_mx/cap_mx_csr.cc \
                  ${TOPDIR}/nic/asic/capri/model/cap_mc/cap_mc_csr.cc \
                  ${TOPDIR}/nic/asic/capri/model/cap_mc/cap_mch_csr.cc \
                  ${TOPDIR}/nic/asic/ip/verif/pcpp/LogMsg.cc \
                  ${TOPDIR}/nic/asic/ip/verif/pcpp/msg_man.cc \
                  ${TOPDIR}/nic/asic/ip/verif/pcpp/msg_stream.cc \
                  ${TOPDIR}/nic/asic/capri/model/utils/cap_csr_py_if.cc \
                  ${TOPDIR}/nic/asic/ip/verif/pcpp/cpu.cc \
                  ${TOPDIR}/nic/asic/ip/verif/pcpp/pknobs.cc \
                  ${TOPDIR}/nic/asic/capri/verif/apis/gen/cap_pb_api.cc \
                  ${TOPDIR}/nic/asic/capri/verif/apis/gen/cap_quiesce_api.cc \
                  ${TOPDIR}/nic/asic/capri/verif/apis/cap_txs_api.cc \
                  ${TOPDIR}/nic/asic/capri/verif/apis/cap_nx_api.cc \
                  ${TOPDIR}/nic/asic/capri/design/common/gen/cap_phv_intr_decoders.cc \
                  ${TOPDIR}/nic/asic/capri/verif/apis/cap_npv_api.cc \
                  ${TOPDIR}/nic/asic/capri/verif/apis/cap_dpa_api.cc \
                  ${TOPDIR}/nic/asic/capri/verif/apis/cap_intr_api.cc \
                  ${TOPDIR}/nic/asic/capri/verif/apis/cap_pics_api.cc \
                  ${TOPDIR}/nic/asic/capri/verif/apis/cap_pict_api.cc \
                  ${TOPDIR}/nic/asic/capri/verif/apis/cap_ppa_api.cc \
                  ${TOPDIR}/nic/asic/capri/verif/apis/cap_prd_api.cc \
                  ${TOPDIR}/nic/asic/capri/verif/apis/cap_psp_api.cc \
                  ${TOPDIR}/nic/asic/capri/verif/apis/cap_ptd_api.cc \
                  ${TOPDIR}/nic/asic/capri/verif/apis/cap_stg_api.cc \
                  ${TOPDIR}/nic/asic/capri/verif/apis/cap_wa_api.cc \
                  ${TOPDIR}/nic/asic/capri/design/common/gen/cap_lif_qstate_decoders.cc \
                  ${TOPDIR}/nic/asic/capri/model/cap_ppa/cap_ppa_decoders.cc \
                  ${TOPDIR}/nic/asic/capri/verif/apis/cap_csr_util_api.cc \
                  ${TOPDIR}/nic/asic/capri/model/cap_pic/cap_pic_decoders.cc \
                  ${TOPDIR}/nic/asic/capri/verif/apis/cap_te_hal_api.cc \
                  ${TOPDIR}/nic/asic/capri/verif/apis/cap_mpu_api.cc \
                  ${TOPDIR}/nic/asic/ip/verif/pcpp/pknobs_reader.cc \
                  ${TOPDIR}/nic/asic/capri/verif/apis/cap_nwl_sbus_api.cc \
                  ${TOPDIR}/nic/asic/capri/verif/apis/cap_sbus_api.cc \
		  ${TOPDIR}/nic/sdk/platform/capri/csrint/csr_init.cc \
                  $(wildcard ${TOPDIR}/nic/asic/capri/model/cap_dpa/*csr.cc) \
                  $(wildcard ${TOPDIR}/nic/asic/capri/model/cap_pb/*port*csr.cc)
include ${MKDEFS}/post.mk
