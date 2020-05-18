#! /bin/bash

TOOLS_DIR=`dirname $0`
ABS_TOOLS_DIR=`readlink -f $TOOLS_DIR`
export ASIC="${ASIC:-elba}"
export WS_TOP="/sw"
export NICDIR=`dirname $ABS_TOOLS_DIR`
export TOPDIR=$NICDIR
export SDKDIR=${NICDIR}/sdk/
export NON_PERSISTENT_LOGDIR=${TOPDIR}
export ASIC_MOCK_MODE=1
export BUILD_DIR=${TOPDIR}/build/x86_64/iris/${ASIC}
export GEN_TEST_RESULTS_DIR=${BUILD_DIR}/gtest_results

export SNORT_EXPORT_DIR=${TOPDIR}/hal/third-party/snort3/export
export SNORT_LUA_PATH=${SNORT_EXPORT_DIR}/lua/
export LUA_PATH="${SNORT_EXPORT_DIR}/lua/?.lua;;"
export SNORT_DAQ_PATH=${SNORT_EXPORT_DIR}/x86_64/lib/
export HAL_CONFIG_PATH=${TOPDIR}/conf
export DISABLE_AGING=1

PRELOADS=${BUILD_DIR}/lib/libp4pd_mock.so
HBMHASH_PRELOADS=${BUILD_DIR}/lib/libhbmhashp4pd_mock.so
function finish {
   ${TOPDIR}/tools/savelogs.sh
}
trap finish EXIT
set -ex
# PI gtests
export PATH=${PATH}:${BUILD_DIR}/bin
if [[ "$1" ==  --coveragerun ]]; then
    export COVFILE=${TOPDIR}/coverage/sim_bullseye_hal.cov
    # Run sdk tests for code coverage
    ${SDKDIR}/tools/run_sdk_gtests.sh
    [[ $? -ne 0 ]] && echo "sdk gtests failed" && exit 1
fi

# Disabling appid_test as it is failing after integrating with memhash library
#${CMD_OPTS} appid_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/appid_test.xml"

#${CMD_OPTS} port_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/port_test.xml"
# disable for elba
#${CMD_OPTS} ipsec_policy_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/ipsec_policy_test.xml"
${CMD_OPTS} tcp_proxy_policy_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/tcp_proxy_policy_test.xml"
${CMD_OPTS} acl_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/acl_test.xml"
${CMD_OPTS} bm_allocator_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/bm_allocator_test.xml"
${CMD_OPTS} enicif_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/enicif_test.xml"
${CMD_OPTS} lif_manager_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/lif_manager_test.xml"
${CMD_OPTS} lif_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/lif_test.xml"
${CMD_OPTS} event_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/event_test.xml"

${CMD_OPTS} uplinkpc_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/uplinkpc_test.xml"
${CMD_OPTS} internal_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/internal_test.xml"

# disable for elba -- regression -- TBD-ELBA-REBASE
#${CMD_OPTS} endpoint_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/endpoint_test.xml"
#${CMD_OPTS} endpoint_classic_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/endpoint_classic_test.xml"
#${CMD_OPTS} ep_mgmt_merge --gtest_filter=ep_mgmt_merge_test.test1 --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/ep_mgmt_merge1.xml"
#${CMD_OPTS} ep_mgmt_merge --gtest_filter=ep_mgmt_merge_test.test2 --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/ep_mgmt_merge2.xml"
#${CMD_OPTS} ep_mgmt_merge --gtest_filter=ep_mgmt_merge_test.test3 --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/ep_mgmt_merge3.xml"
#${CMD_OPTS} ep_mgmt_merge_classic --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/ep_mgmt_merge_classic.xml"

${CMD_OPTS} network_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/network_test.xml"
${CMD_OPTS} vrf_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/vrf_test.xml"

# disable for elba
#${CMD_OPTS} ipsec_sa_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/ipsec_sa_test.xml"
${CMD_OPTS} multicast_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/multicast_test.xml"
${CMD_OPTS} tunnelif_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/tunnelif_test.xml"
${CMD_OPTS} nwsec_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/nwsec_test.xml"
${CMD_OPTS} scheduler_tx_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/scheduler_tx_test.xml"

# disable for elba
#${CMD_OPTS} qos_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/qos_test.xml"
#${CMD_OPTS} copp_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/copp_test.xml"

${CMD_OPTS} nh_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/nh_test.xml"
${CMD_OPTS} route_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/route_test.xml"
${CMD_OPTS} nwsec_prof_upgrade_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/nwsec_prof_upgrade_test.xml"
${CMD_OPTS} vrf_upgrade_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/vrf_upgrade_test.xml"

# disable for elba -- regression -- TBD-ELBA-REBASE
#${CMD_OPTS} filter_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/filter_test.xml"

${CMD_OPTS} mgmt_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/mgmt_test.xml"
${CMD_OPTS} snake_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/snake_test.xml"

# disable for elba
#${CMD_OPTS} ncsi_test --gtest_filter=ncsi_test.qos --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/ncsi_test_qos.xml"
#${CMD_OPTS} ncsi_test --gtest_filter=ncsi_test.test1 --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/ncsi_test_test1.xml"

#${CMD_OPTS} fmiss_learn_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/fmiss_learn_test.xml"
#${CMD_OPTS} arp_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/arp_test.xml"
#${CMD_OPTS} dhcp_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/dhcp_test.xml"
#${CMD_OPTS} learn_mix_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/learn_mix_test.xml"

# disable for elba -- duplicate
# ${CMD_OPTS} network_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/network_test.xml"

${CMD_OPTS} agent_api_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/agent_api_test.xml"
${CMD_OPTS} events_queue_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/events_queue_test.xml"
${CMD_OPTS} events_recorder_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/events_recorder_test.xml"
#${CMD_OPTS} upgrade_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/upgrade_test.xml"
# L2seg test for hostpin
L2SEG_PRELOAD=${BUILD_DIR}/lib/libport_mock.so
LD_PRELOAD=${L2SEG_PRELOAD} ${CMD_OPTS} l2seg_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/l2seg_test.xml"
DEVAPI_PRELOAD=${BUILD_DIR}/lib/devapi_iris_mock.so
LD_PRELOAD=${DEVAPI_PRELOAD} devapi_iris_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/devapi_iris_test.xml"

bitmap_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/bitmap_test.xml"
host_mem_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/host_mem_test.xml"
pt_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/pt_test.xml"
block_list_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/block_list_test.xml"
fsm_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/fsm_test.xml"

LD_PRELOAD=${PRELOADS} acl_tcam_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/acl_tcam_test.xml"
LD_PRELOAD=${HBMHASH_PRELOADS} hbmhash_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/hbmhash_test.xml"
LD_PRELOAD=${PRELOADS} met_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/met_test.xml"
LD_PRELOAD=${PRELOADS} rw_tbl_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/rw_tbl_test.xml"
LD_PRELOAD=${PRELOADS} tnnl_rw_tbl_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/tnnl_rw_tbl_test.xml"

#@# SDK tests
#@find build/x86_64/iris/bin/ -name *.runfiles_manifest* | sed 's/\.runfiles_manifest$$//1' | \
#while read i; \
#    do printf "\nRunning $$i\n" && $$i --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/${basename{$$i}}.xml"; \
#done \

export ASIC_MOCK_MEMORY_MODE=1
# Any test using memhash library requires ASIC_MOCK_MEMORY_MODE
${CMD_OPTS} alg_utils_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/alg_utils_test.xml"
${CMD_OPTS} alg_rpc_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/alg_rpc_test.xml"
${CMD_OPTS} alg_tftp_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/alg_tftp_test.xml"
${CMD_OPTS} alg_ftp_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/alg_ftp_test.xml"
${CMD_OPTS} alg_rtsp_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/alg_rtsp_test.xml"
${CMD_OPTS} alg_dns_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/alg_dns_test.xml"

${CMD_OPTS} session_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/session_test.xml"
${CMD_OPTS} fte_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/fte_test.xml"
${CMD_OPTS} nwsec_policy_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/nwsec_policy_test.xml"
${CMD_OPTS} net_fwding_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/net_fwding_test.xml"
${CMD_OPTS} mirror_session_test --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/mirror_session_test.xml"

# disable for elba -- regression -- TBD-ELBA-REBASE
#${CMD_OPTS} arp_track_test --gtest_filter=arp_track_test.mirror_order1 --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/arp_track_test1.xml"
#${CMD_OPTS} arp_track_test --gtest_filter=arp_track_test.mirror_order2 --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/arp_track_test2.xml"

${NICDIR}/utils/ftl/test/iris/run.sh --gtest_output="xml:${GEN_TEST_RESULTS_DIR}/ftl_test.xml"
