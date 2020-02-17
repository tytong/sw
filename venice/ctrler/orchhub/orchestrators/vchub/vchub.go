package vchub

import (
	"context"
	"fmt"
	"net/url"
	"strings"
	"sync"

	"github.com/vmware/govmomi/vim25/mo"
	"github.com/vmware/govmomi/vim25/types"

	"github.com/pensando/sw/api"
	"github.com/pensando/sw/api/generated/orchestration"
	"github.com/pensando/sw/venice/ctrler/orchhub/orchestrators/vchub/defs"
	"github.com/pensando/sw/venice/ctrler/orchhub/orchestrators/vchub/vcprobe"
	"github.com/pensando/sw/venice/ctrler/orchhub/statemgr"
	"github.com/pensando/sw/venice/ctrler/orchhub/utils/pcache"
	"github.com/pensando/sw/venice/utils/kvstore"
	"github.com/pensando/sw/venice/utils/log"
)

const (
	storeQSize = 64
)

// VCHub instance
type VCHub struct {
	*defs.State
	cancel       context.CancelFunc
	vcOpsChannel chan *kvstore.WatchEvent
	vcReadCh     chan defs.Probe2StoreMsg
	vcEventCh    chan defs.Probe2StoreMsg
	pCache       *pcache.PCache
	probe        vcprobe.ProbeInf
	DcMapLock    sync.Mutex
	// TODO: don't use DC display name as key, use ID instead
	DcMap        map[string]*PenDC
	DcID2NameMap map[string]string
	// Opts is options used during creation of this instance
	opts []Option
}

// Option specifies optional values for vchub
type Option func(*VCHub)

// WithScheme sets the scheme for the client to use when connecting to vcenter
func WithScheme(scheme string) Option {
	return func(v *VCHub) {
		vcURL := &url.URL{
			Scheme: scheme,
			Host:   v.OrchConfig.Spec.URI,
			Path:   "/sdk",
		}
		vcURL.User = url.UserPassword(v.OrchConfig.Spec.Credentials.UserName, v.OrchConfig.Spec.Credentials.Password)
		v.State.VcURL = vcURL
	}
}

// LaunchVCHub starts VCHub
func LaunchVCHub(stateMgr *statemgr.Statemgr, config *orchestration.Orchestrator, logger log.Logger, opts ...Option) *VCHub {
	logger.Infof("VCHub instance for %s is starting...", config.GetName())
	vchub := &VCHub{}
	vchub.setupVCHub(stateMgr, config, logger, opts...)
	return vchub
}

func (v *VCHub) setupVCHub(stateMgr *statemgr.Statemgr, config *orchestration.Orchestrator, logger log.Logger, opts ...Option) {
	ctx, cancel := context.WithCancel(context.Background())

	vcURL := &url.URL{
		Scheme: "https",
		Host:   config.Spec.URI,
		Path:   "/sdk",
	}
	vcURL.User = url.UserPassword(config.Spec.Credentials.UserName, config.Spec.Credentials.Password)

	if config.Labels == nil {
		logger.Infof("No DCs specified, handle all DCs in a vcenter")
		config.Labels = map[string]string{}
	}
	forceDCMap := map[string]bool{}
	forceDC, ok := config.Labels["force-dc-names"]
	if ok {
		logger.Infof("Foced DC %s: Only events for this DC will be processed", forceDC)
		forceDCs := strings.Split(forceDC, ",")
		for _, dc := range forceDCs {
			forceDCMap[dc] = true
		}
	}
	state := defs.State{
		VcURL:        vcURL,
		VcID:         config.GetName(),
		Ctx:          ctx,
		Log:          logger.WithContext("submodule", fmt.Sprintf("VCHub-%s", config.GetName())),
		StateMgr:     stateMgr,
		OrchConfig:   config,
		Wg:           &sync.WaitGroup{},
		ForceDCNames: forceDCMap,
	}

	v.State = &state
	v.cancel = cancel
	v.DcMap = map[string]*PenDC{}
	v.DcID2NameMap = map[string]string{}
	v.vcReadCh = make(chan defs.Probe2StoreMsg, storeQSize)
	v.vcEventCh = make(chan defs.Probe2StoreMsg, storeQSize)
	v.opts = opts
	v.setupPCache()

	clusterItems, err := v.StateMgr.Controller().Cluster().List(context.Background(), &api.ListWatchOptions{})
	if err != nil {
		logger.Errorf("Failed to get cluster object, %s", err)
	} else if len(clusterItems) == 0 {
		logger.Errorf("Cluster list returned 0 objects, %s", err)
	} else {
		cluster := clusterItems[0]
		state.ClusterID = defs.CreateClusterID(cluster.Cluster)
	}

	for _, opt := range opts {
		if opt != nil {
			opt(v)
		}
	}
	// Store related go routines
	v.Wg.Add(1)
	go v.startEventsListener()

	// Store must be created before probe for sync to work properly
	v.createProbe(config)

	v.sync()

	v.Wg.Add(1)
	go v.probe.StartWatchers()

	v.DcMapLock.Lock()
	defer v.DcMapLock.Unlock()
	for _, dc := range v.DcMap {
		v.probe.StartEventReceiver([]types.ManagedObjectReference{dc.dcRef})
	}
}

func (v *VCHub) createProbe(config *orchestration.Orchestrator) {
	v.probe = vcprobe.NewVCProbe(v.vcReadCh, v.vcEventCh, v.State)
	v.probe.Start()
}

// Destroy tears down VCHub instance
func (v *VCHub) Destroy(cleanRemote bool) {
	// Teardown probe and store
	v.Log.Infof("Destroying VCHub....")

	// Clearing probe/session state after all routines finish
	// so that a thread in the middle of writing doesn't get a nil client
	if cleanRemote {
		v.Log.Infof("Cleaning up state on VCenter.")
		v.deleteAllDVS()
	}
	v.cancel()
	v.Wg.Wait()

	v.probe.ClearState()

	v.DeleteHosts()
	v.Log.Infof("VCHub Destroyed")
}

// UpdateConfig handles if the Orchestrator config has changed
func (v *VCHub) UpdateConfig(config *orchestration.Orchestrator) {
	// Restart vchub
	v.Log.Infof("VCHub config updated, restarting...")
	v.Destroy(false)
	v.setupVCHub(v.StateMgr, v.OrchConfig, v.Log, v.opts...)
}

// deleteAllDVS cleans up all the PensandoDVS present in the DCs within the VC deployment
func (v *VCHub) deleteAllDVS() {
	v.DcMapLock.Lock()
	defer v.DcMapLock.Unlock()

	for _, dc := range v.DcMap {
		_, ok := v.ForceDCNames[dc.Name]
		if len(v.ForceDCNames) > 0 && !ok {
			log.Infof("Skipping deletion of DVS from %v.", dc.Name)
			continue
		}

		dc.Lock()
		for _, penDVS := range dc.DvsMap {
			err := v.probe.RemovePenDVS(dc.Name, penDVS.DvsName)
			if err != nil {
				log.Errorf("Failed deleting DVS %v in DC %v. Err : %v", penDVS.DvsName, dc.Name, err)
			}
		}
		dc.Unlock()
	}
}

// Sync handles an instance manager request to reqsync the inventory
func (v *VCHub) Sync() {
	v.Log.Debugf("VCHub Sync starting")
	// Bring useg to VCHub struct
	v.Wg.Add(1)
	go func() {
		defer v.Wg.Done()
		v.sync()
	}()

	// Resync the inventory
	// Sync api server state needed for store.
	// v.store.Sync()

	// Gets diffs
	// Pushes deletes/creates as watch events
	// v.probe.Sync()
}

// ListPensandoHosts List only Pensando Hosts from vCenter
func (v *VCHub) ListPensandoHosts(dcRef *types.ManagedObjectReference) []mo.HostSystem {
	hosts := v.probe.ListHosts(dcRef)
	var penHosts []mo.HostSystem
	for _, host := range hosts {
		if !isPensandoHost(host.Config) {
			v.Log.Debugf("Skipping non-Pensando Host %s", host.Name)
			continue
		}
		penHosts = append(penHosts, host)
	}
	return penHosts
}
