#! /bin/bash
cd `dirname $0`
export ASIC_SRC=$NIC_DIR/sdk/asic_repo/asic
export ASIC_GEN=$NIC_DIR/asic_gen
echo "ASIC_SRC = $ASIC_SRC"
echo "ASIC_GEN = $ASIC_GEN"


export ASIC_REPO_DIR=$NIC_DIR/sdk/asic_repo
export ASIC_EXPORT_DIR=$NIC_DIR/sdk/third-party/asic

function export_asic_files () {
    cd $SW_DIR
    for file in `cat nic/tools/asic_sw_combined_sanity/elba_asic_headers.txt`
    do
        destdir=`dirname $file`
        cp $ASIC_REPO_DIR/asic/$file $ASIC_EXPORT_DIR/$destdir/
    done
}

cp -v $ASIC_SRC/elba/model/elb_top/elb_env_base.h $NIC_DIR/sdk/model_sim/include/elb_env_base.h
cp -v $ASIC_SRC/elba/model/elb_top/elb_model_base.h $NIC_DIR/sdk/model_sim/include/elb_model_base.h
cp -v $ASIC_SRC/ip/verif/pcpp/pen_blk_env_base.h $NIC_DIR/sdk/model_sim/include/pen_blk_env_base.h
cp -v $ASIC_SRC/ip/verif/pcpp/cpu_bus_base.h $NIC_DIR/sdk/model_sim/include/cpu_bus_base.h
cp -v $ASIC_SRC/ip/verif/pcpp/cpu_bus_stub.h $NIC_DIR/sdk/model_sim/include/cpu_bus_stub.h
cp -v $ASIC_SRC/ip/verif/pcpp/cpu.h $NIC_DIR/sdk/model_sim/include/cpu.h
cp -v $ASIC_SRC/ip/verif/pcpp/HBM.h $NIC_DIR/sdk/model_sim/include/HBM.h
cp -v $ASIC_SRC/ip/verif/pcpp/HOST_MEM.h $NIC_DIR/sdk/model_sim/include/HOST_MEM.h
cp -v $ASIC_SRC/ip/verif/pcpp/pen_mem.h $NIC_DIR/sdk/model_sim/include/pen_mem.h
cp -v $ASIC_SRC/ip/verif/pcpp/pen_mem_base.h $NIC_DIR/sdk/model_sim/include/pen_mem_base.h
cp -v $ASIC_SRC/ip/verif/pcpp/pknobs.h $NIC_DIR/sdk/model_sim/include/pknobs.h
cp -v $ASIC_SRC/ip/verif/pcpp/LogMsg.h $NIC_DIR/sdk/model_sim/include/LogMsg.h
cp -v $ASIC_SRC/ip/verif/pcpp/common_dpi.h $NIC_DIR/sdk/model_sim/include/common_dpi.h
cp -v $ASIC_SRC/elba/model/utils/elb_blk_env_base.h $NIC_DIR/sdk/model_sim/include/elb_blk_env_base.h
cp -v $ASIC_GEN/elba/verif/common/manifest/libcommon_cc.so $NIC_DIR/sdk/model_sim/libs/elba/libcommon_cc.so
cp -v $ASIC_GEN/elba/model/elb_top/libmodel_cc.so $NIC_DIR/sdk/model_sim/libs/elba/libmodel_cc.so
cp -v $ASIC_GEN/elba/verif/common/manifest/libtop_csr_compile.so $NIC_DIR/sdk/model_sim/libs/elba/libtop_csr_compile.so
cp -v $ASIC_SRC/ip/verif/pensim/gen/lib/libasmsym.a $NIC_DIR/sdk/model_sim/libs/elba/libasmsym.a
cp -v $ASIC_SRC/ip/verif/pensim/gen/lib/libelbsim.a $NIC_DIR/sdk/model_sim/libs/elba/libelbsim.a
cp -v $ASIC_SRC/ip/verif/pensim/gen/lib/libisa.a $NIC_DIR/sdk/model_sim/libs/elba/libisa.a
cp -v $ASIC_SRC/ip/verif/pensim/gen/lib/libelbisa.a $NIC_DIR/sdk/model_sim/libs/elba/libelbisa.a
cp -v $ASIC_SRC/ip/verif/pensim/gen/lib/libmpuobj.a $NIC_DIR/sdk/model_sim/libs/elba/libmpuobj.a

cp -v $ASIC_SRC/elba/model/elb_te/elb_te_csr.json $NIC_DIR/tools/ncc/csr_json/
cp -v $ASIC_SRC/elba/model/elb_ppa/elb_ppa_decoders.json $NIC_DIR/tools/ncc/csr_json/
cp -v $ASIC_SRC/elba/model/elb_ppa/elb_ppa_csr.json $NIC_DIR/tools/ncc/csr_json/
cp -v $ASIC_SRC/elba/model/elb_pic/elb_pict_csr.json $NIC_DIR/tools/ncc/csr_json/
cp -v $ASIC_SRC/elba/model/elb_pic/elb_pics_csr.json $NIC_DIR/tools/ncc/csr_json/
cp -v $ASIC_SRC/elba/model/elb_dpa/elb_dpr_csr.json $NIC_DIR/tools/ncc/csr_json/
cp -v $ASIC_SRC/elba/model/elb_dpa/elb_dpp_csr.json $NIC_DIR/tools/ncc/csr_json/
cp -v $ASIC_SRC/elba/design/common/elb_addr.json $NIC_DIR/tools/ncc/csr_json/

export_asic_files
