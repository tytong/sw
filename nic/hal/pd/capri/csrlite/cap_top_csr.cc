#include "sdk/asic/capri/csrlite/cap_top_csr.hpp"
#include "sdk/asic/capri/csrlite/cap_top_csr_defines.h"

namespace sdk {
namespace lib {
namespace csrlite {
using namespace std;

class cap_top_csr_helper_t cap_top_csr_helper;

void cap_top_csr_helper_t::init() {

    ppa.ppa[CAP_PPA_CSR_INST_PPA_0].init(CAP_ADDR_BASE_PPA_PPA_0_OFFSET);
    ppa.ppa[CAP_PPA_CSR_INST_PPA_1].init(CAP_ADDR_BASE_PPA_PPA_1_OFFSET);

    sgi.te[CAP_TE_CSR_INST_SGI_0].init(CAP_ADDR_BASE_SGI_TE_0_OFFSET);
    sgi.te[CAP_TE_CSR_INST_SGI_1].init(CAP_ADDR_BASE_SGI_TE_1_OFFSET);
    sgi.te[CAP_TE_CSR_INST_SGI_2].init(CAP_ADDR_BASE_SGI_TE_2_OFFSET);
    sgi.te[CAP_TE_CSR_INST_SGI_3].init(CAP_ADDR_BASE_SGI_TE_3_OFFSET);
    sgi.te[CAP_TE_CSR_INST_SGI_4].init(CAP_ADDR_BASE_SGI_TE_4_OFFSET);
    sgi.te[CAP_TE_CSR_INST_SGI_5].init(CAP_ADDR_BASE_SGI_TE_5_OFFSET);
    sgi.mpu[CAP_MPU_CSR_INST_SGI_0].init(CAP_ADDR_BASE_SGI_MPU_0_OFFSET);
    sgi.mpu[CAP_MPU_CSR_INST_SGI_1].init(CAP_ADDR_BASE_SGI_MPU_1_OFFSET);
    sgi.mpu[CAP_MPU_CSR_INST_SGI_2].init(CAP_ADDR_BASE_SGI_MPU_2_OFFSET);
    sgi.mpu[CAP_MPU_CSR_INST_SGI_3].init(CAP_ADDR_BASE_SGI_MPU_3_OFFSET);
    sgi.mpu[CAP_MPU_CSR_INST_SGI_4].init(CAP_ADDR_BASE_SGI_MPU_4_OFFSET);
    sgi.mpu[CAP_MPU_CSR_INST_SGI_5].init(CAP_ADDR_BASE_SGI_MPU_5_OFFSET);

    sge.te[CAP_TE_CSR_INST_SGE_0].init(CAP_ADDR_BASE_SGE_TE_0_OFFSET);
    sge.te[CAP_TE_CSR_INST_SGE_1].init(CAP_ADDR_BASE_SGE_TE_1_OFFSET);
    sge.te[CAP_TE_CSR_INST_SGE_2].init(CAP_ADDR_BASE_SGE_TE_2_OFFSET);
    sge.te[CAP_TE_CSR_INST_SGE_3].init(CAP_ADDR_BASE_SGE_TE_3_OFFSET);
    sge.te[CAP_TE_CSR_INST_SGE_4].init(CAP_ADDR_BASE_SGE_TE_4_OFFSET);
    sge.te[CAP_TE_CSR_INST_SGE_5].init(CAP_ADDR_BASE_SGE_TE_5_OFFSET);
    sge.mpu[CAP_MPU_CSR_INST_SGE_0].init(CAP_ADDR_BASE_SGE_MPU_0_OFFSET);
    sge.mpu[CAP_MPU_CSR_INST_SGE_1].init(CAP_ADDR_BASE_SGE_MPU_1_OFFSET);
    sge.mpu[CAP_MPU_CSR_INST_SGE_2].init(CAP_ADDR_BASE_SGE_MPU_2_OFFSET);
    sge.mpu[CAP_MPU_CSR_INST_SGE_3].init(CAP_ADDR_BASE_SGE_MPU_3_OFFSET);
    sge.mpu[CAP_MPU_CSR_INST_SGE_4].init(CAP_ADDR_BASE_SGE_MPU_4_OFFSET);
    sge.mpu[CAP_MPU_CSR_INST_SGE_5].init(CAP_ADDR_BASE_SGE_MPU_5_OFFSET);

    pcr.te[CAP_TE_CSR_INST_PCR_0].init(CAP_ADDR_BASE_PCR_TE_0_OFFSET);
    pcr.te[CAP_TE_CSR_INST_PCR_1].init(CAP_ADDR_BASE_PCR_TE_1_OFFSET);
    pcr.te[CAP_TE_CSR_INST_PCR_2].init(CAP_ADDR_BASE_PCR_TE_2_OFFSET);
    pcr.te[CAP_TE_CSR_INST_PCR_3].init(CAP_ADDR_BASE_PCR_TE_3_OFFSET);
    pcr.te[CAP_TE_CSR_INST_PCR_4].init(CAP_ADDR_BASE_PCR_TE_4_OFFSET);
    pcr.te[CAP_TE_CSR_INST_PCR_5].init(CAP_ADDR_BASE_PCR_TE_5_OFFSET);
    pcr.te[CAP_TE_CSR_INST_PCR_6].init(CAP_ADDR_BASE_PCR_TE_6_OFFSET);
    pcr.te[CAP_TE_CSR_INST_PCR_7].init(CAP_ADDR_BASE_PCR_TE_7_OFFSET);
    pcr.mpu[CAP_MPU_CSR_INST_PCR_0].init(CAP_ADDR_BASE_PCR_MPU_0_OFFSET);
    pcr.mpu[CAP_MPU_CSR_INST_PCR_1].init(CAP_ADDR_BASE_PCR_MPU_1_OFFSET);
    pcr.mpu[CAP_MPU_CSR_INST_PCR_2].init(CAP_ADDR_BASE_PCR_MPU_2_OFFSET);
    pcr.mpu[CAP_MPU_CSR_INST_PCR_3].init(CAP_ADDR_BASE_PCR_MPU_3_OFFSET);
    pcr.mpu[CAP_MPU_CSR_INST_PCR_4].init(CAP_ADDR_BASE_PCR_MPU_4_OFFSET);
    pcr.mpu[CAP_MPU_CSR_INST_PCR_5].init(CAP_ADDR_BASE_PCR_MPU_5_OFFSET);
    pcr.mpu[CAP_MPU_CSR_INST_PCR_6].init(CAP_ADDR_BASE_PCR_MPU_6_OFFSET);
    pcr.mpu[CAP_MPU_CSR_INST_PCR_7].init(CAP_ADDR_BASE_PCR_MPU_7_OFFSET);

    pct.te[CAP_TE_CSR_INST_PCT_0].init(CAP_ADDR_BASE_PCT_TE_0_OFFSET);
    pct.te[CAP_TE_CSR_INST_PCT_1].init(CAP_ADDR_BASE_PCT_TE_1_OFFSET);
    pct.te[CAP_TE_CSR_INST_PCT_2].init(CAP_ADDR_BASE_PCT_TE_2_OFFSET);
    pct.te[CAP_TE_CSR_INST_PCT_3].init(CAP_ADDR_BASE_PCT_TE_3_OFFSET);
    pct.te[CAP_TE_CSR_INST_PCT_4].init(CAP_ADDR_BASE_PCT_TE_4_OFFSET);
    pct.te[CAP_TE_CSR_INST_PCT_5].init(CAP_ADDR_BASE_PCT_TE_5_OFFSET);
    pct.te[CAP_TE_CSR_INST_PCT_6].init(CAP_ADDR_BASE_PCT_TE_6_OFFSET);
    pct.te[CAP_TE_CSR_INST_PCT_7].init(CAP_ADDR_BASE_PCT_TE_7_OFFSET);
    pct.mpu[CAP_MPU_CSR_INST_PCT_0].init(CAP_ADDR_BASE_PCT_MPU_0_OFFSET);
    pct.mpu[CAP_MPU_CSR_INST_PCT_1].init(CAP_ADDR_BASE_PCT_MPU_1_OFFSET);
    pct.mpu[CAP_MPU_CSR_INST_PCT_2].init(CAP_ADDR_BASE_PCT_MPU_2_OFFSET);
    pct.mpu[CAP_MPU_CSR_INST_PCT_3].init(CAP_ADDR_BASE_PCT_MPU_3_OFFSET);
    pct.mpu[CAP_MPU_CSR_INST_PCT_4].init(CAP_ADDR_BASE_PCT_MPU_4_OFFSET);
    pct.mpu[CAP_MPU_CSR_INST_PCT_5].init(CAP_ADDR_BASE_PCT_MPU_5_OFFSET);
    pct.mpu[CAP_MPU_CSR_INST_PCT_6].init(CAP_ADDR_BASE_PCT_MPU_6_OFFSET);
    pct.mpu[CAP_MPU_CSR_INST_PCT_7].init(CAP_ADDR_BASE_PCT_MPU_7_OFFSET);

    rpc.pics.init(CAP_ADDR_BASE_RPC_PICS_OFFSET);
    ssi.pics.init(CAP_ADDR_BASE_SSI_PICS_OFFSET);
    sse.pics.init(CAP_ADDR_BASE_SSE_PICS_OFFSET);
    tpc.pics.init(CAP_ADDR_BASE_TPC_PICS_OFFSET);

    tsi.pict.init(CAP_ADDR_BASE_TSI_PICT_OFFSET);
    tse.pict.init(CAP_ADDR_BASE_TSE_PICT_OFFSET);

    mc.mc[CAP_MC_CSR_INST_MC_0].init(CAP_ADDR_BASE_MC_MC_0_OFFSET);
    mc.mc[CAP_MC_CSR_INST_MC_1].init(CAP_ADDR_BASE_MC_MC_1_OFFSET);
    mc.mc[CAP_MC_CSR_INST_MC_2].init(CAP_ADDR_BASE_MC_MC_2_OFFSET);
    mc.mc[CAP_MC_CSR_INST_MC_3].init(CAP_ADDR_BASE_MC_MC_3_OFFSET);
    mc.mc[CAP_MC_CSR_INST_MC_4].init(CAP_ADDR_BASE_MC_MC_4_OFFSET);
    mc.mc[CAP_MC_CSR_INST_MC_5].init(CAP_ADDR_BASE_MC_MC_5_OFFSET);
    mc.mc[CAP_MC_CSR_INST_MC_6].init(CAP_ADDR_BASE_MC_MC_6_OFFSET);
    mc.mc[CAP_MC_CSR_INST_MC_7].init(CAP_ADDR_BASE_MC_MC_7_OFFSET);

    dpr.dpr[CAP_DPR_CSR_INST_DPR_0].init(CAP_ADDR_BASE_DPR_DPR_0_OFFSET);
    dpr.dpr[CAP_DPR_CSR_INST_DPR_1].init(CAP_ADDR_BASE_DPR_DPR_1_OFFSET);

    dpp.dpp[CAP_DPP_CSR_INST_DPP_0].init(CAP_ADDR_BASE_DPP_DPP_0_OFFSET);
    dpp.dpp[CAP_DPP_CSR_INST_DPP_1].init(CAP_ADDR_BASE_DPP_DPP_1_OFFSET);

    mx.mx[CAP_MX_CSR_INST_MX_0].init(CAP_ADDR_BASE_MX_MX_0_OFFSET);
    mx.mx[CAP_MX_CSR_INST_MX_1].init(CAP_ADDR_BASE_MX_MX_1_OFFSET);

    intr.intr.init(CAP_ADDR_BASE_INTR_INTR_OFFSET);
    pxb.pxb.init(CAP_ADDR_BASE_PXB_PXB_OFFSET);
    pr.pr.init(CAP_ADDR_BASE_PR_PR_OFFSET);
    pp.pp.init(CAP_ADDR_BASE_PP_PP_OFFSET);
    pt.pt.init(CAP_ADDR_BASE_PT_PT_OFFSET);
    txs.txs.init(CAP_ADDR_BASE_TXS_TXS_OFFSET);
    pb.pbc.init(CAP_ADDR_BASE_PB_PBC_OFFSET);
    pm.pbm.init(CAP_ADDR_BASE_PM_PBM_OFFSET);
    db.wa.init(CAP_ADDR_BASE_DB_WA_OFFSET);
    bx.bx.init(CAP_ADDR_BASE_BX_BX_OFFSET);

    md.hens.init(CAP_ADDR_BASE_MD_HENS_OFFSET);
    md.hese.init(CAP_ADDR_BASE_MD_HESE_OFFSET);
    sema.sema.init(CAP_ADDR_BASE_SEMA_SEMA_OFFSET);
    mp.mpns.init(CAP_ADDR_BASE_MP_MPNS_OFFSET);
    mp.mpse.init(CAP_ADDR_BASE_MP_MPSE_OFFSET);
    ms.apb.init(CAP_ADDR_BASE_MS_APB_OFFSET);
    ms.ap.init(CAP_ADDR_BASE_MS_AP_OFFSET);
    ms.emmc.init(CAP_ADDR_BASE_MS_EMMC_OFFSET);

//TODO    ms.rbm.init(CAP_ADDR_BASE_MS_RBM_OFFSET);
//TODO    ms.ssram.init(CAP_ADDR_BASE_MS_SSRAM_OFFSET);
//TODO    ms.apx.init(CAP_ADDR_BASE_MS_APX_OFFSET);
//TODO    ms.esecure.init(CAP_ADDR_BASE_MS_ESECURE_OFFSET);
//TODO    ms.ap2.init(CAP_ADDR_BASE_MS_AP2_OFFSET);
//TODO    ms.flash.init(CAP_ADDR_BASE_MS_FLASH_OFFSET);
//TODO    ms.ms.init(CAP_ADDR_BASE_MS_MS_OFFSET);
//TODO    ms.mss.init(CAP_ADDR_BASE_MS_MSS_OFFSET);
}

}
}
}
