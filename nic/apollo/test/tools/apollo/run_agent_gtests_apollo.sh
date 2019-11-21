export NICDIR=`pwd`
export BUILD_DIR=${NICDIR}/build/x86_64/apollo/
export GEN_TEST_RESULTS_DIR=${BUILD_DIR}/gtest_results
export HAL_CONFIG_PATH=${NICDIR}/conf/
export ZMQ_SOC_DIR=${NICDIR}
export PATH=${PATH}:${BUILD_DIR}/bin
export NON_PERSISTENT_LOGDIR=${NICDIR}
unset CAPRI_MOCK_MODE

cleanup () {
    pkill agent
    pkill cap_model
}
cleanup

$NICDIR/apollo/tools/start-agent-sim.sh > agent.log 2>&1 &
$NICDIR/apollo/test/tools/apollo/start-apollo-model.sh &

echo 'Sleeping for 1.5 minutes till agent start listening'
sleep 90
echo 'Continuing'
$GDB apollo_vpc_test -c hal.json \
--gtest_output="xml:${GEN_TEST_RESULTS_DIR}/apollo_vpc_test.xml"
rm -f $NICDIR/conf/pipeline.json
[[ $? -ne 0 ]] && echo "apollo_vpc_test failed!" && exit 1
