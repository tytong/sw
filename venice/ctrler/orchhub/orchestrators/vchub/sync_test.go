package vchub

import (
	"context"
	"fmt"
	"net"
	"net/url"
	"os"
	"sync"
	"testing"
	"time"

	"github.com/vmware/govmomi/vim25/types"

	"github.com/pensando/sw/api"
	"github.com/pensando/sw/api/generated/cluster"
	"github.com/pensando/sw/api/generated/network"
	"github.com/pensando/sw/api/generated/orchestration"
	"github.com/pensando/sw/api/generated/workload"
	"github.com/pensando/sw/venice/ctrler/orchhub/orchestrators/vchub/defs"
	"github.com/pensando/sw/venice/ctrler/orchhub/orchestrators/vchub/sim"
	"github.com/pensando/sw/venice/ctrler/orchhub/orchestrators/vchub/testutils"
	"github.com/pensando/sw/venice/ctrler/orchhub/orchestrators/vchub/vcprobe"
	"github.com/pensando/sw/venice/ctrler/orchhub/orchestrators/vchub/vcprobe/mock"
	"github.com/pensando/sw/venice/ctrler/orchhub/statemgr"
	smmock "github.com/pensando/sw/venice/ctrler/orchhub/statemgr"
	"github.com/pensando/sw/venice/ctrler/orchhub/utils"
	"github.com/pensando/sw/venice/globals"
	"github.com/pensando/sw/venice/utils/log"
	conv "github.com/pensando/sw/venice/utils/strconv"
	. "github.com/pensando/sw/venice/utils/testutils"
)

// Tests creation of internal DC state, creationg of DVS
// and creation/deletion of networks in venice
// trigger respective events in VC
func TestVCSyncPG(t *testing.T) {
	// Stale PGs should be deleted
	// New PGs should be created
	err := testutils.ValidateParams(defaultTestParams)
	if err != nil {
		t.Fatalf("Failed at validating test parameters")
	}

	// SETTING UP LOGGER
	config := log.GetDefaultConfig("sync_test-pg")
	config.LogToStdout = true
	config.Filter = log.AllowAllFilter
	logger := log.SetConfig(config)

	// SETTING UP STATE MANAGER
	sm, _, err := smmock.NewMockStateManager()
	if err != nil {
		t.Fatalf("Failed to create state manager. Err : %v", err)
		return
	}

	// CREATING ORCH CONFIG
	orchConfig := smmock.GetOrchestratorConfig(defaultTestParams.TestHostName, defaultTestParams.TestUser, defaultTestParams.TestPassword)

	err = sm.Controller().Orchestrator().Create(orchConfig)

	// SETTING UP VCSIM
	vcURL := &url.URL{
		Scheme: "http",
		Host:   defaultTestParams.TestHostName,
		Path:   "/sdk",
	}
	vcURL.User = url.UserPassword(defaultTestParams.TestUser, defaultTestParams.TestPassword)

	s, err := sim.NewVcSim(sim.Config{Addr: vcURL.String()})
	AssertOk(t, err, "Failed to create vcsim")
	defer s.Destroy()
	dc1, err := s.AddDC(defaultTestParams.TestDCName)
	AssertOk(t, err, "failed dc create")

	pvlanConfigSpecArray := testutils.GenPVLANConfigSpecArray(defaultTestParams, "add")
	dvsCreateSpec := testutils.GenDVSCreateSpec(defaultTestParams, pvlanConfigSpecArray)
	_, err = dc1.AddDVS(dvsCreateSpec)
	AssertOk(t, err, "failed dvs create")

	orchInfo1 := []*network.OrchestratorInfo{
		{
			Name:      orchConfig.Name,
			Namespace: defaultTestParams.TestDCName,
		},
	}

	// CREATING VENICE NETWORKS
	smmock.CreateNetwork(sm, "default", "pg1", "10.1.1.0/24", "10.1.1.1", 100, nil, orchInfo1)
	smmock.CreateNetwork(sm, "default", "pg2", "10.1.2.0/24", "10.1.1.2", 101, nil, orchInfo1)

	time.Sleep(1 * time.Second)

	// SETTING UP MOCK
	// Real probe that will be used by mock probe when possible
	vchub := setupVCHub(vcURL, sm, orchConfig, logger)
	vcp := vcprobe.NewVCProbe(vchub.vcReadCh, vchub.State)
	mockProbe := mock.NewProbeMock(vcp)
	vchub.probe = mockProbe
	mockProbe.Start()

	defer vchub.Destroy()

	vchub.Sync()

	verifyPg := func(dcPgMap map[string][]string) {
		AssertEventually(t, func() (bool, interface{}) {
			for name, pgNames := range dcPgMap {
				dc := vchub.GetDC(name)
				if dc == nil {
					return false, fmt.Errorf("Failed to find DC %s", name)
				}
				dvs := dc.GetPenDVS(createDVSName(name))
				if dvs == nil {
					return false, fmt.Errorf("Failed to find dvs in DC %s", name)
				}

				for _, pgName := range pgNames {
					pgObj := dvs.GetPenPG(pgName)
					if pgObj == nil {
						return false, fmt.Errorf("Failed to find %s in DC %s", pgName, name)
					}
				}
				dvs.Lock()
				if len(dvs.Pgs) != len(pgNames) {
					err := fmt.Errorf("PG length didn't match: exp %v, actual %v", pgNames, dvs.Pgs)
					dvs.Unlock()
					return false, err
				}
				dvs.Unlock()
			}
			return true, nil
		}, "Failed to find PGs")
	}

	// n1 should only be in defaultDC
	// n2 should be in both
	pg1 := createPGName("pg1")
	pg2 := createPGName("pg2")

	dcPgMap := map[string][]string{
		defaultTestParams.TestDCName: []string{pg1, pg2},
	}

	verifyPg(dcPgMap)

}
func TestVCSyncHost(t *testing.T) {
	// Stale hosts should be deleted
	// New hosts should be created
	err := testutils.ValidateParams(defaultTestParams)
	if err != nil {
		t.Fatalf("Failed at validating test parameters")
	}

	// SETTING UP LOGGER
	config := log.GetDefaultConfig("sync_test-Host")
	config.LogToStdout = true
	config.Filter = log.AllowAllFilter
	logger := log.SetConfig(config)

	// SETTING UP STATE MANAGER
	sm, _, err := smmock.NewMockStateManager()
	if err != nil {
		t.Fatalf("Failed to create state manager. Err : %v", err)
		return
	}

	// CREATING ORCH CONFIG
	orchConfig := smmock.GetOrchestratorConfig(defaultTestParams.TestHostName, defaultTestParams.TestUser, defaultTestParams.TestPassword)

	err = sm.Controller().Orchestrator().Create(orchConfig)

	// SETTING UP VCSIM
	vcURL := &url.URL{
		Scheme: "http",
		Host:   defaultTestParams.TestHostName,
		Path:   "/sdk",
	}
	vcURL.User = url.UserPassword(defaultTestParams.TestUser, defaultTestParams.TestPassword)

	s, err := sim.NewVcSim(sim.Config{Addr: vcURL.String()})
	AssertOk(t, err, "Failed to create vcsim")
	defer s.Destroy()
	dc1, err := s.AddDC(defaultTestParams.TestDCName)
	AssertOk(t, err, "failed dc create")

	hostSystem1, err := dc1.AddHost("host1")
	AssertOk(t, err, "failed host1 create")
	pNicMac := net.HardwareAddr{}
	pNicMac = append(pNicMac, globals.PensandoOUI[0])
	pNicMac = append(pNicMac, globals.PensandoOUI[1])
	pNicMac = append(pNicMac, globals.PensandoOUI[2])
	pNicMac = append(pNicMac, 0xaa)
	pNicMac = append(pNicMac, 0x00)
	pNicMac = append(pNicMac, 0x00)
	// Make it Pensando host
	err = hostSystem1.AddNic("vmnic0", conv.MacString(pNicMac))

	hostSystem2, err := dc1.AddHost("host2")
	AssertOk(t, err, "failed host2 create")
	pNicMac = net.HardwareAddr{}
	pNicMac = append(pNicMac, globals.PensandoOUI[0])
	pNicMac = append(pNicMac, globals.PensandoOUI[1])
	pNicMac = append(pNicMac, globals.PensandoOUI[2])
	pNicMac = append(pNicMac, 0xbb)
	pNicMac = append(pNicMac, 0x00)
	pNicMac = append(pNicMac, 0x00)
	// Make it Pensando host
	err = hostSystem2.AddNic("vmnic0", conv.MacString(pNicMac))

	// CREATING HOSTS
	staleHost := cluster.Host{
		ObjectMeta: api.ObjectMeta{
			Name:      utils.CreateGlobalKey(orchConfig.Name, dc1.Obj.Self.Value, "hostsystem-00000"),
			Namespace: "default",
			// Don't set Tenant as object is not scoped inside Tenant in proto file.
		},
		TypeMeta: api.TypeMeta{
			Kind: "Host",
		},
		Spec: cluster.HostSpec{
			DSCs: []cluster.DistributedServiceCardID{
				cluster.DistributedServiceCardID{
					ID: "test",
				},
			},
		},
	}
	sm.Controller().Host().Create(&staleHost)
	host1 := cluster.Host{
		ObjectMeta: api.ObjectMeta{
			Name:      utils.CreateGlobalKey(orchConfig.Name, dc1.Obj.Self.Value, hostSystem1.Obj.Self.Value),
			Namespace: "default",
			// Don't set Tenant as object is not scoped inside Tenant in proto file.
		},
		TypeMeta: api.TypeMeta{
			Kind: "Host",
		},
		Spec: cluster.HostSpec{
			DSCs: []cluster.DistributedServiceCardID{
				cluster.DistributedServiceCardID{
					ID: "test1",
				},
			},
		},
	}
	sm.Controller().Host().Create(&host1)

	time.Sleep(1 * time.Second)

	// SETTING UP MOCK
	// Real probe that will be used by mock probe when possible
	vchub := setupVCHub(vcURL, sm, orchConfig, logger)
	vcp := vcprobe.NewVCProbe(vchub.vcReadCh, vchub.State)
	mockProbe := mock.NewProbeMock(vcp)
	vchub.probe = mockProbe
	mockProbe.Start()

	defer vchub.Destroy()

	vchub.Sync()

	verifyHosts := func(dcHostMap map[string][]string) {
		AssertEventually(t, func() (bool, interface{}) {
			for name, hostnames := range dcHostMap {
				dc := vchub.GetDC(name)
				if dc == nil {
					return false, fmt.Errorf("Failed to find DC %s", name)
				}
				for _, hostname := range hostnames {
					meta := &api.ObjectMeta{
						Name: hostname,
					}
					host, err := sm.Controller().Host().Find(meta)
					if err != nil {
						return false, fmt.Errorf("Failed to find host %s", hostname)
					}
					if host == nil {
						return false, fmt.Errorf("Returned host was nil for host %s", hostname)
					}
				}
				opts := api.ListWatchOptions{}
				hosts, err := sm.Controller().Host().List(context.Background(), &opts)
				AssertOk(t, err, "failed to get hosts")
				if len(hostnames) != len(hosts) {
					return false, fmt.Errorf("expected %d hosts but only got %d", len(hostnames), len(hosts))
				}
			}
			return true, nil
		}, "Failed to find hosts")
	}

	dcHostMap := map[string][]string{
		defaultTestParams.TestDCName: []string{host1.Name, utils.CreateGlobalKey(orchConfig.Name, dc1.Obj.Self.Value, hostSystem2.Obj.Self.Value)},
	}

	verifyHosts(dcHostMap)

}

func TestVCSyncVM(t *testing.T) {
	// Stale VMs should be deleted
	// New VMs should be assigned Vlans
	// VMs with an already assigned override should keep it
	// VM with a different override than we assigned should be set back to what we have
	err := testutils.ValidateParams(defaultTestParams)
	if err != nil {
		t.Fatalf("Failed at validating test parameters")
	}

	// SETTING UP LOGGER
	config := log.GetDefaultConfig("sync_test-Host")
	config.LogToStdout = true
	config.Filter = log.AllowAllFilter
	logger := log.SetConfig(config)

	// SETTING UP STATE MANAGER
	sm, _, err := smmock.NewMockStateManager()
	if err != nil {
		t.Fatalf("Failed to create state manager. Err : %v", err)
		return
	}

	// CREATING ORCH CONFIG
	orchConfig := smmock.GetOrchestratorConfig(defaultTestParams.TestHostName, defaultTestParams.TestUser, defaultTestParams.TestPassword)

	err = sm.Controller().Orchestrator().Create(orchConfig)

	// SETTING UP VCSIM
	vcURL := &url.URL{
		Scheme: "http",
		Host:   defaultTestParams.TestHostName,
		Path:   "/sdk",
	}
	vcURL.User = url.UserPassword(defaultTestParams.TestUser, defaultTestParams.TestPassword)

	s, err := sim.NewVcSim(sim.Config{Addr: vcURL.String()})
	AssertOk(t, err, "Failed to create vcsim")
	defer s.Destroy()
	dc1, err := s.AddDC(defaultTestParams.TestDCName)
	AssertOk(t, err, "failed dc create")

	hostSystem1, err := dc1.AddHost("host1")
	AssertOk(t, err, "failed host1 create")
	pNicMac := net.HardwareAddr{}
	pNicMac = append(pNicMac, globals.PensandoOUI[0])
	pNicMac = append(pNicMac, globals.PensandoOUI[1])
	pNicMac = append(pNicMac, globals.PensandoOUI[2])
	pNicMac = append(pNicMac, 0xaa)
	pNicMac = append(pNicMac, 0x00)
	pNicMac = append(pNicMac, 0x00)
	// Make it Pensando host
	err = hostSystem1.AddNic("vmnic0", conv.MacString(pNicMac))

	pvlanConfigSpecArray := testutils.GenPVLANConfigSpecArray(defaultTestParams, "add")
	dvsCreateSpec := testutils.GenDVSCreateSpec(defaultTestParams, pvlanConfigSpecArray)
	dvs, err := dc1.AddDVS(dvsCreateSpec)
	AssertOk(t, err, "failed dvs create")

	// SETTING UP MOCK
	// Real probe that will be used by mock probe when possible
	vchub := setupVCHub(vcURL, sm, orchConfig, logger)
	vcp := vcprobe.NewVCProbe(vchub.vcReadCh, vchub.State)
	mockProbe := mock.NewProbeMock(vcp)
	vchub.probe = mockProbe
	mockProbe.Start()

	time.Sleep(3 * time.Second)

	spec := testutils.GenPGConfigSpec(createPGName("pg1"), 2, 3)
	err = mockProbe.AddPenPG(dc1.Obj.Name, dvs.Obj.Name, &spec)
	AssertOk(t, err, "failed to create pg")
	pg1, err := mockProbe.GetPenPG(dc1.Obj.Name, createPGName("pg1"))
	AssertOk(t, err, "failed to get pg")

	defer vchub.Destroy()

	// Setting up VMs
	vmExistingPort := "10"
	vmExistingMac := "aaaa.bbbb.dddd"
	vmExisting, err := dc1.AddVM("vmExisting", "host1", []sim.VNIC{
		sim.VNIC{
			MacAddress:   vmExistingMac,
			PortgroupKey: pg1.Reference().Value,
			PortKey:      vmExistingPort,
		},
	})
	AssertOk(t, err, "Failed to create vmExisting")
	vnNew, err := dc1.AddVM("vnNew", "host1", []sim.VNIC{
		sim.VNIC{
			MacAddress:   "aaaa.bbbb.ddde",
			PortgroupKey: pg1.Reference().Value,
			PortKey:      "11",
		},
	})
	AssertOk(t, err, "Failed to create vnNew")

	// CREATING HOSTS
	host1 := cluster.Host{
		ObjectMeta: api.ObjectMeta{
			Name:      utils.CreateGlobalKey(orchConfig.Name, dc1.Obj.Self.Value, hostSystem1.Obj.Self.Value),
			Namespace: "default",
			// Don't set Tenant as object is not scoped inside Tenant in proto file.
		},
		TypeMeta: api.TypeMeta{
			Kind: "Host",
		},
		Spec: cluster.HostSpec{
			DSCs: []cluster.DistributedServiceCardID{
				cluster.DistributedServiceCardID{
					ID: "test1",
				},
			},
		},
	}
	sm.Controller().Host().Create(&host1)

	// CREATING WORKLOADS
	staleWorkload := workload.Workload{
		ObjectMeta: api.ObjectMeta{
			Name:      utils.CreateGlobalKey(orchConfig.Name, dc1.Obj.Self.Value, "staleWorkload"),
			Namespace: "default",
			Tenant:    "default",
		},
		TypeMeta: api.TypeMeta{
			Kind: "Workload",
		},
		Spec: workload.WorkloadSpec{
			HostName: host1.Name,
			Interfaces: []workload.WorkloadIntfSpec{
				workload.WorkloadIntfSpec{
					MACAddress:   "aaaa.bbbb.cccc",
					MicroSegVlan: 2000,
				},
			},
		},
	}
	sm.Controller().Workload().Create(&staleWorkload)

	workloadExisting := workload.Workload{
		ObjectMeta: api.ObjectMeta{
			Name:      utils.CreateGlobalKey(orchConfig.Name, dc1.Obj.Self.Value, vmExisting.Self.Value),
			Namespace: "default",
			Tenant:    "default",
		},
		TypeMeta: api.TypeMeta{
			Kind: "Workload",
		},
		Spec: workload.WorkloadSpec{
			HostName: host1.Name,
			Interfaces: []workload.WorkloadIntfSpec{
				workload.WorkloadIntfSpec{
					MACAddress:   vmExistingMac,
					MicroSegVlan: 3000,
				},
			},
		},
	}
	sm.Controller().Workload().Create(&workloadExisting)

	portUpdate := vcprobe.PenDVSPortSettings{
		vmExistingPort: &types.VmwareDistributedVirtualSwitchVlanIdSpec{
			VlanId: int32(3000),
		},
	}
	mockProbe.UpdateDVSPortsVlan(dc1.Obj.Name, dvs.Obj.Name, portUpdate)

	orchInfo1 := []*network.OrchestratorInfo{
		{
			Name:      orchConfig.Name,
			Namespace: dc1.Obj.Name,
		},
	}
	smmock.CreateNetwork(sm, "default", "pg1", "10.1.1.0/24", "10.1.1.1", 100, nil, orchInfo1)

	time.Sleep(1 * time.Second)

	vchub.Sync()

	verifyWorkloads := func(dcWorkloadMap map[string][]string) {
		AssertEventually(t, func() (bool, interface{}) {
			for name, wlNames := range dcWorkloadMap {
				dc := vchub.GetDC(name)
				if dc == nil {
					return false, fmt.Errorf("Failed to find DC %s", name)
				}
				for _, wlName := range wlNames {
					meta := &api.ObjectMeta{
						Name:      wlName,
						Tenant:    "default",
						Namespace: "default",
					}
					wl, err := sm.Controller().Workload().Find(meta)
					if err != nil {
						return false, fmt.Errorf("Failed to find workload %s", wlName)
					}
					if wl == nil {
						return false, fmt.Errorf("Returned workload was nil for %s", wlName)
					}
					if len(wl.Spec.Interfaces) == 0 {
						return false, fmt.Errorf("Workload %s had no interfaces", wlName)
					}
					for _, inf := range wl.Spec.Interfaces {
						if inf.ExternalVlan != 100 {
							return false, fmt.Errorf("interface did not have external valn set correctly")
						}
					}
				}
				opts := api.ListWatchOptions{}
				workloadObjs, err := sm.Controller().Workload().List(context.Background(), &opts)
				AssertOk(t, err, "failed to get workloads")
				if len(wlNames) != len(workloadObjs) {
					return false, fmt.Errorf("expected %d workloads but only got %d", len(wlNames), len(workloadObjs))
				}
			}
			return true, nil
		}, "Failed to find hosts")
	}

	dcWorkloadMap := map[string][]string{
		defaultTestParams.TestDCName: []string{workloadExisting.Name, utils.CreateGlobalKey(orchConfig.Name, dc1.Obj.Self.Value, vnNew.Self.Value)},
	}

	verifyWorkloads(dcWorkloadMap)

	dvsPorts, err := mockProbe.GetPenDVSPorts(dc1.Obj.Name, dvs.Obj.Name, &types.DistributedVirtualSwitchPortCriteria{})
	AssertOk(t, err, "Failed to get port info")
	overrides := vchub.extractOverrides(dvsPorts)
	AssertEquals(t, 2, len(overrides), "2 ports should have vlan override, overrideMap: %v", overrides)
	for port, override := range overrides {
		if port == vmExistingPort {
			AssertEquals(t, 3000, override, "vmExisting didn't have override set in workload object")
		}
	}

	// Change one of the overrides
	// Resync should set it back to its correct value
	portUpdate = vcprobe.PenDVSPortSettings{
		vmExistingPort: &types.VmwareDistributedVirtualSwitchVlanIdSpec{
			VlanId: int32(3002),
		},
	}
	mockProbe.UpdateDVSPortsVlan(dc1.Obj.Name, dvs.Obj.Name, portUpdate)

	vchub.Sync()

	AssertEventually(t, func() (bool, interface{}) {
		dvsPorts, err := mockProbe.GetPenDVSPorts(dc1.Obj.Name, dvs.Obj.Name, &types.DistributedVirtualSwitchPortCriteria{})
		AssertOk(t, err, "Failed to get port info")
		overrides := vchub.extractOverrides(dvsPorts)
		if len(overrides) != 2 {
			return false, fmt.Errorf("Expected only 2 overrides, %v", overrides)
		}
		for port, override := range overrides {
			if port == vmExistingPort {
				if override != 3000 {
					return false, fmt.Errorf("Override was %d, not 3000", override)
				}
			}
		}
		return true, nil
	}, "Override was not set back")

}

func TestSyncVmkNics(t *testing.T) {
	err := testutils.ValidateParams(defaultTestParams)
	if err != nil {
		t.Fatalf("Failed at validating test parameters")
	}

	// SETTING UP LOGGER
	config := log.GetDefaultConfig("sync_test")
	config.LogToStdout = true
	config.Filter = log.AllowAllFilter
	logger := log.SetConfig(config)

	// SETTING UP STATE MANAGER
	sm, _, err := smmock.NewMockStateManager()
	if err != nil {
		t.Fatalf("Failed to create state manager. Err : %v", err)
		return
	}

	// CREATING ORCH CONFIG
	orchConfig := smmock.GetOrchestratorConfig(defaultTestParams.TestHostName, defaultTestParams.TestUser, defaultTestParams.TestPassword)

	err = sm.Controller().Orchestrator().Create(orchConfig)

	// SETTING UP VCSIM
	vcURL := &url.URL{
		Scheme: "http",
		Host:   defaultTestParams.TestHostName,
		Path:   "/sdk",
	}
	vcURL.User = url.UserPassword(defaultTestParams.TestUser, defaultTestParams.TestPassword)

	s, err := sim.NewVcSim(sim.Config{Addr: vcURL.String()})
	AssertOk(t, err, "Failed to create vcsim")
	defer s.Destroy()
	// SETTING UP MOCK
	// Real probe that will be used by mock probe when possible
	vchub := setupVCHub(vcURL, sm, orchConfig, logger)
	vcp := vcprobe.NewVCProbe(vchub.vcReadCh, vchub.State)
	mockProbe := mock.NewProbeMock(vcp)
	vchub.probe = mockProbe
	mockProbe.Start()
	defer vchub.Destroy()

	for !mockProbe.IsSessionReady() {
		time.Sleep(1 * time.Second)
	}

	// Add DC
	dc, err := s.AddDC(defaultTestParams.TestDCName)
	AssertOk(t, err, "failed dc create")
	// Add it using vcHub so mockProbe gets the needed info ???
	// This will also create PenDVS
	logger.Infof("Creating PenDC for %s\n", dc.Obj.Reference().Value)
	_, err = vchub.NewPenDC(defaultTestParams.TestDCName)
	// Add DVS
	dvsName := createDVSName(defaultTestParams.TestDCName)
	dvs, ok := dc.GetDVS(dvsName)
	if !ok {
		logger.Info("GetDVS Failed")
		os.Exit(1)
	}
	Assert(t, ok, "failed dvs create")

	orchInfo1 := []*network.OrchestratorInfo{
		{
			Name:      orchConfig.Name,
			Namespace: defaultTestParams.TestDCName,
		},
	}
	// Create one PG for vmkNic
	pgConfigSpec := []types.DVPortgroupConfigSpec{
		types.DVPortgroupConfigSpec{
			Name:     createPGName("vMotion_PG"),
			NumPorts: 8,
			DefaultPortConfig: &types.VMwareDVSPortSetting{
				Vlan: &types.VmwareDistributedVirtualSwitchPvlanSpec{
					PvlanId: int32(100),
				},
			},
		},
	}

	smmock.CreateNetwork(sm, "default", "vMotion_PG", "11.1.1.0/24", "11.1.1.1", 500, nil, orchInfo1)
	// Add PG to mockProbe (this is weird, this should be part of sim)
	// vcHub should provide this function ??
	mockProbe.AddPenPG(defaultTestParams.TestDCName, dvsName, &pgConfigSpec[0])
	pg, err := mockProbe.GetPenPG(defaultTestParams.TestDCName, createPGName("vMotion_PG"))
	AssertOk(t, err, "failed to add portgroup")

	// Create Host
	host, err := dc.AddHost("host1")
	AssertOk(t, err, "failed to add Host to DC")
	err = dvs.AddHost(host)
	AssertOk(t, err, "failed to add Host to DVS")

	pNicMac := net.HardwareAddr{}
	pNicMac = append(pNicMac, globals.PensandoOUI[0])
	pNicMac = append(pNicMac, globals.PensandoOUI[1])
	pNicMac = append(pNicMac, globals.PensandoOUI[2])
	pNicMac = append(pNicMac, 0xbb)
	pNicMac = append(pNicMac, 0x00)
	pNicMac = append(pNicMac, 0x00)
	// Make it Pensando host
	err = host.AddNic("vmnic0", conv.MacString(pNicMac))
	AssertOk(t, err, "failed to add pNic")

	type testEP struct {
		mac  string
		vlan uint32
	}

	type WlMap map[string]map[string]testEP
	testWorkloadMap := WlMap{}
	testNICs := map[string]testEP{}

	// Create vmkNIC
	var spec types.HostVirtualNicSpec
	spec.Mac = "0011.2233.0001"
	var dvPort types.DistributedVirtualSwitchPortConnection
	dvPort.PortgroupKey = pg.Reference().Value
	dvPort.PortKey = "10" // use some port number
	spec.DistributedVirtualPort = &dvPort
	err = host.AddVmkNic(&spec, "vmk1")
	testNICs[spec.Mac] = testEP{
		mac:  spec.Mac,
		vlan: 500,
	}
	AssertOk(t, err, "failed to add vmkNic")

	spec.Mac = "0011.2233.0002"
	var dvPort2 types.DistributedVirtualSwitchPortConnection
	dvPort2.PortgroupKey = pg.Reference().Value
	dvPort2.PortKey = "11" // use some port number
	spec.DistributedVirtualPort = &dvPort2
	err = host.AddVmkNic(&spec, "vmk2")
	AssertOk(t, err, "failed to add vmkNic")
	testNICs[spec.Mac] = testEP{
		mac:  spec.Mac,
		vlan: 500,
	}

	logger.Infof("===== Sync1 =====")
	vchub.Sync()

	wlName := createVmkWorkLoadName(orchInfo1[0].Name, dc.Obj.Self.Value, host.Obj.Self.Value)
	testWorkloadMap[wlName] = testNICs

	// Add Validations
	// Check that workload for the host with its vmknics as EPs is created
	verifyVmkworkloads := func(testWlEPMap WlMap, msg string) {
		AssertEventually(t, func() (bool, interface{}) {
			opts := api.ListWatchOptions{}
			wls, err := sm.Controller().Workload().List(context.Background(), &opts)
			if err != nil {
				return false, nil
			}
			if len(wls) != len(testWlEPMap) {
				return false, nil
			}
			for wlname, testEPs := range testWlEPMap {
				meta := &api.ObjectMeta{
					Name:      wlname,
					Tenant:    "default",
					Namespace: "default",
				}
				wl, err := sm.Controller().Workload().Find(meta)
				if err != nil {
					logger.Infof("Workload not found %s", wlname)
					return false, nil
				}
				for _, ep := range wl.Workload.Spec.Interfaces {
					_, ok := testEPs[ep.MACAddress]
					if !ok {
						logger.Infof("EP not found %s", ep.MACAddress)
						return false, nil
					}
				}
				if len(wl.Workload.Spec.Interfaces) != len(testEPs) {
					return false, nil
				}
			}
			return true, nil
		}, msg, "5s", "1s")
	}
	verifyVmkworkloads(testWorkloadMap, "WL with 2EPs create failed")
	host.RemoveVmkNic("vmk2")
	delete(testNICs, "0011.2233.0002")
	spec.Mac = "0011.2233.0003"
	var dvPort3 types.DistributedVirtualSwitchPortConnection
	dvPort3.PortgroupKey = pg.Reference().Value
	dvPort3.PortKey = "12" // use some port number
	spec.DistributedVirtualPort = &dvPort3
	err = host.AddVmkNic(&spec, "vmk3")
	AssertOk(t, err, "failed to add vmkNic")
	testNICs[spec.Mac] = testEP{
		mac:  spec.Mac,
		vlan: 500,
	}
	logger.Infof("===== Sync2 =====")
	vchub.Sync()
	verifyVmkworkloads(testWorkloadMap, "WL delete 1 EP failed")

	// Delete all vmkNics from a host, WL should be deleted
	time.Sleep(5 * time.Second)

	host.RemoveVmkNic("vmk1")
	host.RemoveVmkNic("vmk3")
	delete(testWorkloadMap, wlName)

	logger.Infof("===== Sync3 =====")
	vchub.Sync()
	verifyVmkworkloads(testWorkloadMap, "WL delete all EPs failed")
}

func setupVCHub(vcURL *url.URL, stateMgr *statemgr.Statemgr, config *orchestration.Orchestrator, logger log.Logger, opts ...Option) *VCHub {
	ctx, cancel := context.WithCancel(context.Background())

	state := defs.State{
		VcURL:      vcURL,
		VcID:       config.GetName(),
		Ctx:        ctx,
		Log:        logger.WithContext("submodule", fmt.Sprintf("VCHub-%s", config.GetName())),
		StateMgr:   stateMgr,
		OrchConfig: config,
		Wg:         &sync.WaitGroup{},
	}
	vchub := &VCHub{}
	vchub.State = &state
	vchub.cancel = cancel
	vchub.DcMap = map[string]*PenDC{}
	vchub.vcReadCh = make(chan defs.Probe2StoreMsg, storeQSize)
	vchub.setupPCache()

	return vchub
}