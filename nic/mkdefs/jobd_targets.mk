# {C} Copyright 2018 Pensando Systems Inc. All rights reserved.
.PHONY: jobd/runner/core_count_check
jobd/runner/core_count_check:
	${NICDIR}/tools/core_count_check.sh

JOBD_PREREQS:= jobd/runner/core_count_check | jobd/package
ifeq (${PKG_PIPELINE},)
  PKG_PIPELINE=${PIPELINE}
  ifeq (${PKG_PIPELINE}, iris)
    PKG_TAR_FILE=build_$(PKG_PIPELINE)_x86_${ASIC}.tar.gz
  else
	PKG_TAR_FILE=build_$(PKG_PIPELINE)_x86.tar.gz
  endif
endif
# jobd uses pre-build package
.PHONY: jobd/package
jobd/package:
	@if [ "x${JOB_ID}" = "x" ] || [ "x${IGNORE_BUILD_PIPELINE}" != "x" ]; then \
		make package; \
	else \
	  if [ -f /sw/${PKG_TAR_FILE} ]; then \
	    tar -zxf /sw/${PKG_TAR_FILE} -C /; \
	  fi \
	fi

.PHONY: ${JOBD_PREREQS}
jobd/codesync: all
	@$(eval DIFFS=`git ls-files --exclude-standard --modified --others`)
	@echo "Found the following uncommitted files, if any"
	@echo $(DIFFS)
	@test -z "$(DIFFS)"

.PHONY: jobd/dol/rdma_ext
jobd/dol/rdma_ext: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --topo rdma --feature rdma_ext,rdma_perf

.PHONY: jobd/dol/rdma
jobd/dol/rdma: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --topo rdma --feature rdma

.PHONY: jobd/dol/fte
jobd/dol/fte: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --topo fte --feature fte --lite

.PHONY: jobd/dol/fte2
jobd/dol/fte2: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --topo fte --feature fte2 --lite --microseg-enable

.PHONY: jobd/dol/ftevxlan
jobd/dol/ftevxlan: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --topo fte --feature ftevxlan --lite

.PHONY: jobd/dol/alg
jobd/dol/alg: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --topo alg --feature alg

.PHONY: jobd/dol/norm
jobd/dol/norm: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --topo norm --feature norm

.PHONY: jobd/dol/eth
jobd/dol/eth: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --topo eth --feature eth --microseg-enable

.PHONY: jobd/dol/acl
jobd/dol/acl: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --topo acl --feature acl --microseg-enable

.PHONY: jobd/dol/proxy
jobd/dol/proxy: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --topo proxy --feature proxy,proxy_fte,proxy_asym1,proxy_asym2

.PHONY: jobd/dol/ipsec
jobd/dol/ipsec: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --topo proxy --feature ipsec --no_error_check

.PHONY: jobd/dol/networking
jobd/dol/networking: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --topo up2up --feature networking

.PHONY: jobd/dol/vxlan
jobd/dol/vxlan: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --topo vxlan --feature vxlan

.PHONY: jobd/dol/mplsudp
jobd/dol/mplsudp: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --topo mpls_udp --feature mpls_udp --classic

.PHONY: jobd/dol/ipsg
jobd/dol/ipsg: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --topo eth --feature ipsg

.PHONY: jobd/dol/firewall
jobd/dol/firewall: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --topo firewall --feature firewall

.PHONY: jobd/dol/pin
jobd/dol/pin: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --topo fte --feature fte2,hostpin --hostpin --lite --microseg-enable
	#${NICDIR}/run.py ${COVERAGE_OPTS} --topo fte --feature fte,fte2,ftevxlan,hpvxlan,hostpin --hostpin --lite

.PHONY: jobd/dol/multicast
jobd/dol/multicast: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --topo multicast --feature multicast --modellogs

.PHONY: jobd/dol/pinl2mc
jobd/dol/pinl2mc: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --topo multicast --feature pinl2mc --hostpin

.PHONY: jobd/dol/l4lb
jobd/dol/l4lb: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --topo l4lb --feature l4lb

.PHONY: jobd/dol/dos
jobd/dol/dos: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --topo dos --feature dos

.PHONY: jobd/dol/recirc
jobd/dol/recirc: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --topo recirc --feature recirc --microseg-enable

.PHONY: jobd/dol/classic
jobd/dol/classic: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --topo classic --feature classic --classic

.PHONY: jobd/dol/classicl2mc
jobd/dol/classicl2mc: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --topo classic --feature classicl2mc --classic

.PHONY: jobd/dol/parser
jobd/dol/parser: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --topo parser --feature parser --modellogs

.PHONY: jobd/dol/telemetry
jobd/dol/telemetry: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --topo telemetry --feature telemetry --microseg-enable

.PHONY: jobd/dol/p4pt
jobd/dol/p4pt: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --modellogs --topo p4pt --feature p4pt

.PHONY: jobd/dol/app_redir
jobd/dol/app_redir: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --topo app_redir --feature app_redir

.PHONY: jobd/dol/basetopo
jobd/dol/basetopo: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --topo base --feature eth_base,rdma_base --lite

.PHONY: jobd/dol/swphv
jobd/dol/swphv: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --topo firewall --feature swphv

.PHONY: jobd/dol/gft
jobd/dol/gft: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --topo gft --feature gft --gft

#.PHONY: jobd/dol/gft/rdma
#jobd/dol/gft/rdma: ${JOBD_PREREQS}
	#${NICDIR}/run.py ${COVERAGE_OPTS} --topo rdma_gft --feature rdma_perf --gft

#.PHONY: jobd/dol/gft/rdma_l2l
#jobd/dol/gft/rdma_l2l: ${JOBD_PREREQS}
	#${NICDIR}/run.py ${COVERAGE_OPTS} --topo rdma_gft --feature rdma_send_only_inline_l2l,rdma_send_l2l --l2l --gft

#.PHONY: jobd/dol/agent/up2up
#jobd/dol/agent/up2up: ${JOBD_PREREQS}
#	${NICDIR}/run.py ${COVERAGE_OPTS} --topo agentup2up --feature agentup2up

.PHONY: jobd/dol/apollo/mplsoudp
jobd/dol/apollo/mplsoudp: ${JOBD_PREREQS}
	${NICDIR}/apollo/tools/rundol.sh --pipeline apollo --topo mplsoudp --feature networking

.PHONY: jobd/dol/apollo/vxlan
jobd/dol/apollo/vxlan: ${JOBD_PREREQS}
	APOLLO_TEST_TEP_ENCAP=1 ${NICDIR}/apollo/tools/rundol.sh --pipeline apollo --topo vxlan --feature networking

.PHONY: jobd/dol/apollo/lpm
jobd/dol/apollo/lpm: ${JOBD_PREREQS}
	${NICDIR}/apollo/tools/rundol.sh --pipeline apollo --topo lpm --feature lpm

.PHONY: jobd/dol/apollo/rfc
jobd/dol/apollo/rfc: ${JOBD_PREREQS}
	${NICDIR}/apollo/tools/rundol.sh --pipeline apollo --topo rfc --feature rfc

.PHONY: jobd/dol/apollo/mirror
jobd/dol/apollo/mirror: ${JOBD_PREREQS}
	#${NICDIR}/apollo/tools/rundol.sh --pipeline apollo --topo mirror --feature mirror

.PHONY: jobd/apollo/pds_scale_test_mock
jobd/apollo/pds_scale_test_mock: ${JOBD_PREREQS}
	${NICDIR}/apollo/test/scale/run_scale_test_mock.sh
	${NICDIR}/apollo/test/scale/runtestapp.sh

.PHONY: jobd/apollo/pds_scale_test_mock_no_batch
jobd/apollo/pds_scale_test_mock_no_batch: ${JOBD_PREREQS}
	export BATCHING_DISABLED=1
	${NICDIR}/apollo/test/scale/run_scale_test_mock.sh
	${NICDIR}/apollo/test/scale/runtestapp.sh
	unset BATCHING_DISABLED

.PHONY: jobd/apollo/aarch64/container
jobd/apollo/aarch64/container:${JOBD_PREREQS}
	# With Agent
	${NICDIR}/apollo/tools/dev-docker/build_dev_docker.sh 1 aarch64 apollo
	rm -rf ${TOPDIR}/apollo_sw
	# Without Agent
	${NICDIR}/apollo/tools/dev-docker/build_dev_docker.sh 0 aarch64 apollo
	rm -rf ${TOPDIR}/apollo_sw

.PHONY: jobd/apollo/x86_64/container
jobd/apollo/x86_64/container:${JOBD_PREREQS}
	# With Agent
	${NICDIR}/apollo/tools/dev-docker/build_dev_docker.sh 1 x86_64 apollo
	rm -rf ${TOPDIR}/apollo_sw
	# Without Agent
	${NICDIR}/apollo/tools/dev-docker/build_dev_docker.sh 0 x86_64 apollo
	rm -rf ${TOPDIR}/apollo_sw

.PHONY: jobd/artemis/pds_scale_test_mock
jobd/artemis/pds_scale_test_mock: ${JOBD_PREREQS}
	${NICDIR}/apollo/test/scale/artemis/run_scale_test_mock.sh
	${NICDIR}/apollo/test/scale/artemis/run_scale_test_mock.sh --cfg artemis/scale_cfg_1vpc.json
	${NICDIR}/apollo/test/scale/artemis/runtestapp.sh

.PHONY: jobd/dol/artemis/vxlan
jobd/dol/artemis/vxlan: ${JOBD_PREREQS}
	${NICDIR}/apollo/tools/rundol.sh --pipeline artemis --topo vxlan --feature networking

.PHONY: jobd/mbt/base
jobd/mbt/base: ${JOBD_PREREQS}
	DISABLE_AGING=1 ${NICDIR}/run.py ${COVERAGE_OPTS} --mbt --mbtrandomseed 6003702

.PHONY: jobd/mbt/networking
jobd/mbt/networking: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --topo=up2up --feature=networking --mbt

.PHONY: jobd/mbt/firewall
jobd/mbt/firewall: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --topo firewall --feature firewall --mbt

.PHONY: jobd/mbt/alg
jobd/mbt/alg: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --topo alg --feature alg --mbt

.PHONY: jobd/gft/gtest
jobd/gft/gtest: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --gft_gtest

.PHONY: jobd/apollo/gtest_1
jobd/apollo/gtest_1: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --apollo_gtest
	${NICDIR}/run.py ${COVERAGE_OPTS} --apollo_scale_test > /dev/null
	${NICDIR}/run.py ${COVERAGE_OPTS} --apollo_scale_vxlan_test > /dev/null
	${NICDIR}/apollo/test/tools/apollo/run_gtests_1.sh ${COVERAGE_OPTS}

.PHONY: jobd/apollo/gtest_2
jobd/apollo/gtest_2: ${JOBD_PREREQS}
	${NICDIR}/apollo/test/tools/apollo/run_gtests_2.sh ${COVERAGE_OPTS}

.PHONY: jobd/apollo/doc
jobd/apollo/doc: ${JOBD_PREREQS}
	${NICDIR}/apollo/tools/gendoc.sh

.PHONY: jobd/artemis/gtest
jobd/artemis/gtest: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --artemis_gtest
	${NICDIR}/apollo/test/tools/artemis/run_gtests.sh ${COVERAGE_OPTS}

.PHONY: jobd/artemis/scale_test_sim
jobd/artemis/scale_test_sim: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --artemis_scale_test > /dev/null

.PHONY: jobd/artemis/ftlite
jobd/artemis/ftlite: ${JOBD_PREREQS}
	${NICDIR}/utils/ftlite/test/run.sh

.PHONY: jobd/storage
jobd/storage: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --storage

.PHONY: jobd/storage/perf
jobd/storage/perf: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --storage --storage_test perf

.PHONY: jobd/storage/nvme
jobd/storage/nvme: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --storage --storage_test nvme_dp

.PHONY: jobd/storage/nvme_perf
jobd/storage/nvme_perf: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --storage --storage_test nvme_dp_scale

.PHONY: jobd/storage/nicmgr
jobd/storage/nicmgr: ${JOBD_PREREQS}
	${NICDIR}/run.py --storage --storage_test nicmgr

.PHONY: jobd/storage/combined
jobd/storage/combined: ${JOBD_PREREQS}
	GRPC_TRACE=tcp GRPC_VERBOSITY=debug ${NICDIR}/run.py ${COVERAGE_OPTS} --storage --storage_test unit --feature rdma --topo rdma --combined --test RDMA_TX_SEND_ONLY --testcase 1

.PHONY: jobd/hal_test_fips/rsa-siggen15-0
jobd/hal_test_fips/rsa-siggen15-0: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --hal_test_fips --hal_test_fips_runargs "fips-rsa-siggen15 rsa-testvectors/SigGen15_186-3.txt"

.PHONY: jobd/offload
jobd/offload: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --offload

.PHONY: jobd/athena_app/session_aging_sim_simple
jobd/athena_app/session_aging_sim_simple: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --athena_app --athena_app_runargs '-d /sw/nic/apollo/test/athena_app/scripts -t session_aging_sim_simple.txt \-\-no-fte-flow-prog'

.PHONY: jobd/athena_app/session_aging_sim_medium
jobd/athena_app/session_aging_sim_medium: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --athena_app --athena_app_runargs '-d /sw/nic/apollo/test/athena_app/scripts -t session_aging_sim_medium.txt \-\-no-fte-flow-prog'

.PHONY: jobd/athena_app/conntrack_aging_sim_simple
jobd/athena_app/conntrack_aging_sim_simple: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --athena_app --athena_app_runargs '-d /sw/nic/apollo/test/athena_app/scripts -t conntrack_aging_sim_simple.txt \-\-no-fte-flow-prog'

.PHONY: jobd/athena_app/conntrack_aging_sim_medium
jobd/athena_app/conntrack_aging_sim_medium: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --athena_app --athena_app_runargs '-d /sw/nic/apollo/test/athena_app/scripts -t conntrack_aging_sim_medium.txt \-\-no-fte-flow-prog'

.PHONY: jobd/athena_app/policy_sim_gtests
jobd/athena_app/policy_sim_gtests: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --athena_app --disable_hugepages --athena_app_runargs '-m gtest -j /sw/nic/conf/athena/policy_gtest_8k.json'
	#${NICDIR}/run.py ${COVERAGE_OPTS} --athena_app --disable_hugepages --athena_app_runargs '-j /sw/nic/conf/athena/32vnic_scale.json'
	#${NICDIR}/run.py ${COVERAGE_OPTS} --athena_app --disable_hugepages --athena_app_runargs '-j /sw/nic/conf/athena/64vnic_scale.json'
	#${NICDIR}/run.py ${COVERAGE_OPTS} --athena_app --disable_hugepages --athena_app_runargs '-j /sw/nic/conf/athena/128vnic_scale.json'
	#${NICDIR}/run.py ${COVERAGE_OPTS} --athena_app --disable_hugepages --athena_app_runargs '-j /sw/nic/conf/athena/256vnic_scale.json'

.PHONY: jobd/athena/gtests_sim
jobd/athena/gtests_sim: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --athena_gtests

.PHONY: jobd/configtest
jobd/configtest: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --configtest

.PHONY: jobd/dol/e2e/tlsproxy
jobd/dol/e2e/tlsproxy: ${JOBD_PREREQS}
	${SUDO_OPT} ${NICDIR}/run.py ${COVERAGE_OPTS} --topo proxy --feature proxy --config-only --e2e-tls-dol

.PHONY: jobd/dol/e2e/v6tlsproxy
jobd/dol/e2e/v6tlsproxy: ${JOBD_PREREQS}
	${SUDO_OPT} ${NICDIR}/run.py ${COVERAGE_OPTS} --topo proxy --feature proxy --config-only --v6-e2e-tls-dol

.PHONY: jobd/dol/e2e/eplearn
jobd/dol/e2e/eplearn: ${JOBD_PREREQS}
	${SUDO_OPT} ${NICDIR}/run.py ${COVERAGE_OPTS} --topo e2e_eplearn --feature e2e_learn --e2e-mode dol-auto --enable-aging --cfgjson conf/dol.conf

.PHONY: jobd/dol/e2e/alg
jobd/dol/e2e/alg: ${JOBD_PREREQS}
	${SUDO_OPT} ${NICDIR}/run.py ${COVERAGE_OPTS} --topo alg --feature e2e --sub alg --e2e-mode dol-auto --cfgjson conf/dol.conf

.PHONY: jobd/dol/e2e/l7
jobd/dol/e2e/l7: ${JOBD_PREREQS}
	${SUDO_OPT} ${NICDIR}/run.py ${COVERAGE_OPTS} --topo proxy --feature proxy --config-only --e2e-l7-dol

.PHONY: jobd/e2e/naples-sim-image
jobd/e2e/naples-sim-image: ${JOBD_PREREQS}
	cd ${NICDIR}/.. && ${MAKE} -C nic/upgrade_manager/export/upggosdk
	go build -ldflags="-s -w" -o ${NICDIR}/build/x86_64/${PIPELINE}/penctl  github.com/pensando/sw/penctl
	cd ${NICDIR}/.. && python2 nic/tools/package/package.py --pipeline ${PIPELINE}${FLAVOR} --target sim --no-strip
	${NICDIR}/tools/release.sh ${PIPELINE}${FLAVOR}

.PHONY: jobd/e2e/naples-sim
jobd/e2e/naples-sim: ${JOBD_PREREQS} jobd/e2e/naples-sim-image
	${SUDO_OPT} ${NICDIR}/tools/validate-naples-docker.sh

.PHONY: jobd/make/sdk
jobd/make/sdk:
	${SUDO_OPT} mkdir -p /sdk
	${SUDO_OPT} mount --bind ${SDKDIR} /sdk
	${MAKE} -C /sdk
	${SDKDIR}/tools/run_sdk_gtests.sh
	${MAKE} -C /sdk ARCH=aarch64
	${SUDO_OPT} umount /sdk

.PHONY: jobd/make/nic
jobd/make/nic:
	${MAKE} PIPELINE=iris
	${MAKE} PIPELINE=iris PLATFORM=hw ARCH=aarch64
	${MAKE} PIPELINE=gft
	${MAKE} PIPELINE=gft PLATFORM=hw ARCH=aarch64
	${MAKE} PIPELINE=apollo
	${MAKE} PIPELINE=apollo PLATFORM=hw ARCH=aarch64

.PHONY: jobd/agent
jobd/agent: ${JOBD_PREREQS}
	cd ${NICDIR}/.. && ${MAKE} -C nic/sysmgr/proto
	cd ${NICDIR}/.. && ${MAKE} -C nic/agent/dscagent
	cd ${NICDIR}/.. && python2 nic/tools/package/package.py --pipeline ${PIPELINE}${FLAVOR} --target sim --no-strip
	${NICDIR}/tools/release.sh ${PIPELINE}${FLAVOR}
	cd ${NICDIR}/.. go test -tags iris ./nic/agent/dscagent/...
	# TODO uncomment the sanities once these are fixed
	#${NICDIR}/e2etests/go/agent/scripts/golden-sanity.sh

.PHONY: jobd/platform/drivers
jobd/platform/drivers:
	${MAKE} -j1 -C ${TOPDIR}/platform/drivers/linux build

.PHONY: jobd/platform/pciemgrd
jobd/platform/pciemgrd: ${JOBD_PREREQS}
	make -C ../platform/src/third-party/app/pciutils
	make BUILD_ARCH=x86_64 -C ../platform/src/app/pciemgrd/tests test

.PHONY: jobd/halctl
jobd/halctl: ${JOBD_PREREQS}
	cd ${GOPATH}/src/github.com/pensando/sw/nic/agent/cmd/halctl && go test

.PHONY: jobd/gtests
jobd/gtests: ${JOBD_PREREQS}
	${NICDIR}/tools/run_gtests.sh ${COVERAGE_OPTS}
	${NICDIR}/tools/trace_valid.py

.PHONY: jobd/elba_gtests
jobd/elba_gtests: ${JOBD_PREREQS}
	${NICDIR}/tools/run_elba_gtests.sh ${COVERAGE_OPTS}
	${NICDIR}/tools/trace_valid.py

.PHONY: jobd/gtests/platform
jobd/gtests/platform: ${JOBD_PREREQS}
	${NICDIR}/tools/trace_valid.py ${NICDIR}/../platform
	./run.py ${COVERAGE_OPTS} --nicmgr_gtest --classic
	./run.py ${COVERAGE_OPTS} --nicmgr_gtest

.PHONY: jobd/gtests-valgrind
jobd/gtests-valgrind: ${JOBD_PREREQS}
	${NICDIR}/tools/run_gtests_valgrind.sh ${COVERAGE_OPTS}
	${NICDIR}/tools/trace_valid.py
	${NICDIR}/tools/trace_valid.py ${NICDIR}/../platform

.PHONY: jobd/upgrade_manager/gtests
jobd/upgrade_manager/gtests: ${JOBD_PREREQS}
	${NICDIR}/upgrade_manager/tools/run_upgrade_manager_gtests.sh

.PHONY: jobd/filter/gtest
jobd/filter/gtest: ${JOBD_PREREQS}
	${MAKE} -j1 -C ${TOPDIR}/platform/src/lib/hal_api
	./run.py ${COVERAGE_OPTS} --filter_gtest --classic
	./run.py ${COVERAGE_OPTS} --filter_gtest

.PHONY: jobd/nicmgr/gtest
jobd/nicmgr/gtest: ${JOBD_PREREQS}
	./run.py --nicmgr_gtest

.PHONY: jobd/nicmgr/gtest_classic
jobd/nicmgr/gtest_classic: ${JOBD_PREREQS}
	./run.py --nicmgr_gtest --classic

.PHONY: jobd/firmware
jobd/firmware:
	${MAKE} PLATFORM=hw ARCH=aarch64 firmware && make PLATFORM=hw ARCH=aarch64 package-drivers
	cd .. && make -C iota

.PHONY: jobd/venice-image
jobd/venice-image:
	${MAKE} -C ${TOPDIR} venice-image

.PHONY: jobd/iota/venice-bm
jobd/iota/venice-bm:jobd/firmware jobd/venice-image
	${MAKE} -j 1 -C ${GOPATH}/src/github.com/pensando/sw/iota
	cd ${IOTADIR} && ./iota.py  --testsuite hostpin_venice

.PHONY: jobd/iota/venice-sim
jobd/iota/venice-sim:jobd/naples-sim jobd/venice-image
	${MAKE} -j 1 -C ${GOPATH}/src/github.com/pensando/sw/iota
	cd ${IOTADIR} && ./iota.py --testsuite venice --skip-firmware-upgrade

.PHONY: jobd/apollo/gtests-valgrind
jobd/apollo/gtests-valgrind: ${JOBD_PREREQS}
	${NICDIR}/apollo/test/tools/apollo/run_gtests_valgrind.sh ${COVERAGE_OPTS}

.PHONY: jobd/artemis/gtests-valgrind
jobd/artemis/gtests-valgrind: ${JOBD_PREREQS}
	${NICDIR}/apollo/test/tools/artemis/run_gtests_valgrind.sh ${COVERAGE_OPTS}

.PHONY: jobd/apulu/gtests-valgrind
jobd/apulu/gtests-valgrind: ${JOBD_PREREQS}
	${NICDIR}/apollo/test/tools/apulu/run_gtests_valgrind.sh ${COVERAGE_OPTS}

.PHONY: jobd/apulu/pds_scale_test_mock
jobd/apulu/pds_scale_test_mock: ${JOBD_PREREQS}
	${NICDIR}/apollo/test/scale/apulu/run_scale_test_mock.sh
	${NICDIR}/apollo/test/scale/apulu/runtestapp.sh

.PHONY: jobd/apulu/pds_uds_testapp
jobd/apulu/pds_uds_testapp: ${JOBD_PREREQS}
	${NICDIR}/apollo/test/scale/apulu/runudstestapp.sh

.PHONY: jobd/apulu/gtest_1
jobd/apulu/gtest_1: ${JOBD_PREREQS}
	${NICDIR}/run.py ${COVERAGE_OPTS} --apulu_gtest
	${NICDIR}/apollo/test/tools/apulu/run_gtests_1.sh ${COVERAGE_OPTS}

.PHONY: jobd/apulu/gtest_2
jobd/apulu/gtest_2: ${JOBD_PREREQS}
	${NICDIR}/apollo/test/tools/apulu/run_gtests_2.sh ${COVERAGE_OPTS}

.PHONY: jobd/apulu/metaswitch_hals_test
jobd/apulu/metaswitch_hals_test: ${JOBD_PREREQS}
	LD_LIBRARY_PATH=${NICDIR}/third-party/metaswitch/output/x86_64/ ${NICDIR}/build/x86_64/apulu/${ASIC}/out/pds_ms_hals_test_mockapi_gtest/pds_ms_hals_test_mockapi.gtest

.PHONY: jobd/apulu/metaswitch_store_test
jobd/apulu/metaswitch_store_test: ${JOBD_PREREQS}
	LD_LIBRARY_PATH=${NICDIR}/third-party/metaswitch/output/x86_64/ ${NICDIR}/build/x86_64/apulu/${ASIC}/out/pds_ms_store_test_gtest/pds_ms_store_test.gtest

.PHONY: jobd/dol/apulu/vxlan
jobd/dol/apulu/vxlan: ${JOBD_PREREQS}
	${NICDIR}/apollo/tools/rundol.sh --pipeline apulu --topo hostvxlan --feature networking

.PHONY: jobd/dol/apulu/l2
jobd/dol/apulu/l2: ${JOBD_PREREQS}
	${NICDIR}/apollo/tools/rundol.sh --pipeline apulu --topo l2_hostvxlan --feature networking --dry

.PHONY: jobd/dol/apulu/rfc_ingress
jobd/dol/apulu/rfc_ingress: ${JOBD_PREREQS}
	${NICDIR}/apollo/tools/rundol.sh --pipeline apulu --feature rfc --topo rfc_disjoint --sub ipv4_ingress_disjoint
	${NICDIR}/apollo/tools/rundol.sh --pipeline apulu --feature rfc --topo rfc_adjacent --sub ipv4_ingress_adjacent
	${NICDIR}/apollo/tools/rundol.sh --pipeline apulu --feature rfc --topo rfc_overlap_1 --sub ipv4_ingress_overlap_1
	${NICDIR}/apollo/tools/rundol.sh --pipeline apulu --feature rfc --topo rfc_overlap_2 --sub ipv4_ingress_overlap_2
	${NICDIR}/apollo/tools/rundol.sh --pipeline apulu --feature rfc --topo rfc_overlap_3 --sub ipv4_ingress_overlap_3
	${NICDIR}/apollo/tools/rundol.sh --pipeline apulu --feature rfc --topo rfc_negative_1 --sub ipv4_ingress_negative_1
	${NICDIR}/apollo/tools/rundol.sh --pipeline apulu --feature rfc --topo rfc_negative_2 --sub ipv4_ingress_negative_2
	${NICDIR}/apollo/tools/rundol.sh --pipeline apulu --feature rfc --topo rfc_empty --sub ipv4_ingress_empty
	${NICDIR}/apollo/tools/rundol.sh --pipeline apulu --feature rfc --topo rfc_negative_no_match --sub ipv4_ingress_no_match
	${NICDIR}/apollo/tools/rundol.sh --pipeline apulu --feature rfc --topo rfc_any_deny --sub ipv4_ingress_overlap_2

.PHONY: jobd/dol/apulu/rfc_egress
jobd/dol/apulu/rfc_egress: ${JOBD_PREREQS}
	${NICDIR}/apollo/tools/rundol.sh --pipeline apulu --feature rfc --topo rfc_disjoint --sub ipv4_egress_disjoint
	${NICDIR}/apollo/tools/rundol.sh --pipeline apulu --feature rfc --topo rfc_adjacent --sub ipv4_egress_adjacent
	${NICDIR}/apollo/tools/rundol.sh --pipeline apulu --feature rfc --topo rfc_overlap_1 --sub ipv4_egress_overlap_1
	${NICDIR}/apollo/tools/rundol.sh --pipeline apulu --feature rfc --topo rfc_overlap_2 --sub ipv4_egress_overlap_2
	${NICDIR}/apollo/tools/rundol.sh --pipeline apulu --feature rfc --topo rfc_overlap_3 --sub ipv4_egress_overlap_3
	${NICDIR}/apollo/tools/rundol.sh --pipeline apulu --feature rfc --topo rfc_negative_1 --sub ipv4_egress_negative_1
	${NICDIR}/apollo/tools/rundol.sh --pipeline apulu --feature rfc --topo rfc_negative_2 --sub ipv4_egress_negative_2
	${NICDIR}/apollo/tools/rundol.sh --pipeline apulu --feature rfc --topo rfc_empty --sub ipv4_egress_empty
	${NICDIR}/apollo/tools/rundol.sh --pipeline apulu --feature rfc --topo rfc_negative_no_match --sub ipv4_egress_no_match

.PHONY: jobd/dol/apulu/rfc_tags
jobd/dol/apulu/rfc_tags: ${JOBD_PREREQS}
	${NICDIR}/apollo/tools/rundol.sh --pipeline apulu --feature rfc --topo rfc_tags_pfx_range --sub ipv4_ingress_tag_pfx_range,ipv4_egress_tag_pfx_range
	${NICDIR}/apollo/tools/rundol.sh --pipeline apulu --feature rfc --topo rfc_tags_crud --sub ltag_crud,rtag_crud

.PHONY: jobd/dol/apulu/lpm
jobd/dol/apulu/lpm: ${JOBD_PREREQS}
	${NICDIR}/apollo/tools/rundol.sh --pipeline apulu --topo lpm_adjacent --feature lpm --sub v4_adjacent
	${NICDIR}/apollo/tools/rundol.sh --pipeline apulu --topo lpm_overlap --feature lpm --sub v4_overlap
	${NICDIR}/apollo/tools/rundol.sh --pipeline apulu --topo lpm_full_overlap --feature lpm --sub v4_full_overlap
	#priority based jobs
	${NICDIR}/apollo/tools/rundol.sh --pipeline apulu --topo lpm_adjacent_priority --feature lpm --sub v4_adjacent
	${NICDIR}/apollo/tools/rundol.sh --pipeline apulu --topo lpm_overlap_priority --feature lpm --sub v4_overlap
	${NICDIR}/apollo/tools/rundol.sh --pipeline apulu --topo lpm_full_overlap_priority --feature lpm --sub v4_full_overlap

.PHONY: jobd/dol/apulu/learn
jobd/dol/apulu/learn: ${JOBD_PREREQS}
	${NICDIR}/apollo/tools/rundol.sh --pipeline apulu --topo learn --feature learn

.PHONY: jobd/dol/apulu/operd
jobd/dol/apulu/operd: ${JOBD_PREREQS}
	${NICDIR}/apollo/tools/rundol.sh --pipeline apulu --topo hostvxlan --feature operd

.PHONY: jobd/athena/gtests
jobd/athena/gtests: ${JOBD_PREREQS}
	${NICDIR}/apollo/test/tools/athena/run_gtests.sh ${COVERAGE_OPTS}

.PHONY: jobd/athena_app/make
jobd/athena_app/make: ${JOBD_PREREQS}
	${NICDIR}/apollo/test/athena_app/build_athena_app.sh

.PHONY: jobd/apulu/upgrade
jobd/apulu/upgrade: ${JOBD_PREREQS}
	${NICDIR}/apollo/test/tools/apulu/run_upgrade_gtests.sh ${COVERAGE_OPTS}

# Elba DOLs
.PHONY: jobd/dol/elba_classic
jobd/dol/elba_classic: ${JOBD_PREREQS}
	${NICDIR}/tools/sw-env elba iris
	ASIC=elba ${NICDIR}/run.py ${COVERAGE_OPTS} --topo classic --feature classic --classic

.PHONY: jobd/dol/elba_eth
jobd/dol/elba_eth: ${JOBD_PREREQS}
	${NICDIR}/tools/sw-env elba iris
	ASIC=elba ${NICDIR}/run.py ${COVERAGE_OPTS} --topo eth --feature eth --microseg-enable

.PHONY: jobd/dol/elba_storage
jobd/dol/elba_storage: ${JOBD_PREREQS}
	${NICDIR}/tools/sw-env elba iris
	${NICDIR}/run.py ${COVERAGE_OPTS} --storage

.PHONY: jobd/dol/elba_storage/perf
jobd/dol/elba_storage/perf: ${JOBD_PREREQS}
	${NICDIR}/tools/sw-env elba iris
	${NICDIR}/run.py ${COVERAGE_OPTS} --storage --storage_test perf

.PHONY: jobd/dol/elba_storage/nvme
jobd/elba_storage/nvme: ${JOBD_PREREQS}
	${NICDIR}/tools/sw-env elba iris
	${NICDIR}/run.py ${COVERAGE_OPTS} --storage --storage_test nvme_dp
