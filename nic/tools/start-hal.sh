#! /bin/bash -e

export ASIC="${ASIC:-capri}"
TOOLS_DIR=`dirname $0`
ABS_TOOLS_DIR=`readlink -f $TOOLS_DIR`
NIC_DIR=`dirname $ABS_TOOLS_DIR`
SNORT_EXPORT_DIR=$NIC_DIR/hal/third-party/snort3/export
#GDB='gdb --args'
export ZMQ_SOC_DIR=$NIC_DIR
export HAL_CONFIG_PATH=$NIC_DIR/conf/
export SNORT_LUA_PATH=$SNORT_EXPORT_DIR/lua/
export LUA_PATH="$SNORT_EXPORT_DIR/lua/?.lua;;"
export SNORT_DAQ_PATH=$SNORT_EXPORT_DIR/x86_64/lib/

echo "{" > /sw/nic/conf/device.conf
echo "\"forwarding-mode\": \"FORWARDING_MODE_HOSTPIN\"," >> /sw/nic/conf/device.conf
echo "\"feature-profile\": 1," >> /sw/nic/conf/device.conf
echo "\"port-admin-state\": \"PORT_ADMIN_STATE_ENABLE\"," >> /sw/nic/conf/device.conf
echo "\"mgmt-if-mac\": 0" >> /sw/nic/conf/device.conf
echo "}" >> /sw/nic/conf/device.conf

cp $HAL_CONFIG_PATH/hal_classic.ini $HAL_CONFIG_PATH/hal.ini

echo "STARTING HAL: `date +%x_%H:%M:%S:%N`"
IRIS_BLD_DIR=$NIC_DIR/build/x86_64/iris/${ASIC}
$GDB $IRIS_BLD_DIR/bin/hal -c hal.json 2>&1
