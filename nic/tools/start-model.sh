#!/bin/bash -e
#cd `dirname $0`
#NIC_DIR=$PWD
export ASIC="${ASIC:-capri}"
TOOLS_DIR=`dirname $0`
ABS_TOOLS_DIR=`readlink -f $TOOLS_DIR`
NIC_DIR=`dirname $ABS_TOOLS_DIR`
export ZMQ_SOC_DIR=$NIC_DIR
# export LD_LIBRARY_PATH=$NIC_DIR/model_sim/:$NIC_DIR/model_sim/libs:/home/asic/tools/src/0.25/x86_64/lib64:$LD_LIBRARY_PATH
IRIS_BLD_DIR=$NIC_DIR/build/x86_64/iris/${ASIC}
echo "STARTING MODEL: `date +%x_%H:%M:%S:%N`"
if [ $ASIC == "capri" ]
then
$GDB $IRIS_BLD_DIR/bin/cap_model +PLOG_MAX_QUIT_COUNT=0 +plog=info +model_debug=$IRIS_BLD_DIR/gen/p4gen/p4/dbg_out//model_debug.json 2>&1 | tee $NIC_DIR/model.log
else
$GDB $IRIS_BLD_DIR/bin/elb_model +PLOG_MAX_QUIT_COUNT=0 +plog=info +model_debug=$IRIS_BLD_DIR/gen/p4gen/p4/dbg_out//model_debug.json 2>&1 | tee $NIC_DIR/model.log
fi

##! /bin/bash -e
#TOOLS_DIR=`dirname $0`
#ABS_TOOLS_DIR=`readlink -f $TOOLS_DIR`
#NIC_DIR=`dirname $ABS_TOOLS_DIR`
#export LD_LIBRARY_PATH=$NIC_DIR/model_sim/:$NIC_DIR/model_sim/libs:/home/asic/tools/src/0.25/x86_64/lib64:$LD_LIBRARY_PATH
#$GDB $NIC_DIR/model_sim/build/cap_model +plog=info +model_debug=$NIC_DIR/build/iris/gen/datapath/p4/dbg_out/model_debug.json 2>&1 | tee $NIC_DIR/model.log
