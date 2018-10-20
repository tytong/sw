// {C} Copyright 2017 Pensando Systems Inc. All rights reserved.

package state

import (
	"sync"
	"time"

	"net"
	"net/http"

	"github.com/pensando/sw/api"
	cmd "github.com/pensando/sw/api/generated/cluster"
	"github.com/pensando/sw/nic/agent/nmd/protos"
	roprotos "github.com/pensando/sw/venice/ctrler/rollout/rpcserver/protos"
	"github.com/pensando/sw/venice/utils/certsproxy"
	"github.com/pensando/sw/venice/utils/emstore"
	"github.com/pensando/sw/venice/utils/rpckit/tlsproviders"
)

// NMD is the Naples management daemon instance object
type NMD struct {
	sync.Mutex     // Lock for NMD object
	sync.WaitGroup // Wait group

	store    emstore.Emstore // Embedded DB
	nodeUUID string          // Node's UUID
	macAddr  string          // Primary MAC addr
	cmd      CmdAPI          // CMD API object
	platform PlatformAPI     // Platform Agent object
	rollout  RolloutCtrlAPI  // Rollout API Object
	upgmgr   UpgMgrAPI       // Upgrade Manager API

	config nmd.Naples    // Naples config received via REST
	nic    *cmd.SmartNIC // SmartNIC object

	stopNICReg     chan bool     // channel to stop NIC registration
	nicRegInterval time.Duration // time interval between nic registration in seconds
	isRegOngoing   bool          // status of ongoing nic registration task

	stopNICUpd     chan bool     // channel to stop NIC update
	nicUpdInterval time.Duration // time interval between nic updates in seconds
	isUpdOngoing   bool          // status of ongoing nic update task

	listenURL        string       // URL where http server is listening
	listener         net.Listener // socket listener
	httpServer       *http.Server // HTTP server
	isRestSrvRunning bool         // status of the REST server

	certsListenURL string                            // URL where local processes can request certificates
	remoteCertsURL string                            // URL where local process cert request are forwarder
	certsProxy     *certsproxy.CertsProxy            // the CertsProxy instance
	tlsProvider    *tlsproviders.KeyMgrBasedProvider // TLS provider holding cluster keys
	// Rollout related stuff
	completedOps  map[roprotos.SmartNICOpSpec]bool // the ops that were requested by spec and got completed
	inProgressOps *roprotos.SmartNICOpSpec         // the ops thats currently in progress
	pendingOps    []roprotos.SmartNICOpSpec        // the ops that will be executed after the current op is completed
	opStatus      []roprotos.SmartNICOpStatus
	objectMeta    api.ObjectMeta
}

// NaplesConfigResp is response to NaplesConfig request nmd.Naples
type NaplesConfigResp struct {
	ErrorMsg string
}

// Setters and Getters for NMD attributes
// Note: Locks are in place for these setter and getters
// to prevent data races reported by go race detector.

// GetRegStatus returns the current status of NIC registration task
func (n *NMD) GetRegStatus() bool {
	n.Lock()
	defer n.Unlock()
	return n.isRegOngoing
}

func (n *NMD) setRegStatus(value bool) {
	n.Lock()
	defer n.Unlock()
	n.isRegOngoing = value
}

// GetUpdStatus returns the current running status of NIC update task
func (n *NMD) GetUpdStatus() bool {
	n.Lock()
	defer n.Unlock()
	return n.isUpdOngoing
}

func (n *NMD) setUpdStatus(value bool) {
	n.Lock()
	defer n.Unlock()
	n.isUpdOngoing = value
}

// GetRestServerStatus returns the current running status of REST server
func (n *NMD) GetRestServerStatus() bool {
	n.Lock()
	defer n.Unlock()
	return n.isRestSrvRunning
}

func (n *NMD) setRestServerStatus(value bool) {
	n.Lock()
	defer n.Unlock()
	n.isRestSrvRunning = value
}

// GetConfigMode returns the configured Naples Mode
func (n *NMD) GetConfigMode() nmd.MgmtMode {
	n.Lock()
	defer n.Unlock()
	return n.config.Spec.Mode
}

func (n *NMD) setNaplesConfig(cfg nmd.Naples) {
	n.Lock()
	defer n.Unlock()

	n.config = cfg
}

// GetNaplesConfig returns the current naples config received via REST
func (n *NMD) GetNaplesConfig() nmd.Naples {
	n.Lock()
	defer n.Unlock()

	return n.config
}

// SetSmartNIC intializes the smartNIC object
func (n *NMD) SetSmartNIC(nic *cmd.SmartNIC) error {
	n.Lock()
	defer n.Unlock()

	n.nic = nic
	return nil
}

// GetSmartNIC returns the smartNIC object
func (n *NMD) GetSmartNIC() (*cmd.SmartNIC, error) {
	n.Lock()
	defer n.Unlock()

	return n.nic, nil
}

// GetListenURL returns the listen URL of the http server
func (n *NMD) GetListenURL() string {
	n.Lock()
	defer n.Unlock()

	if n.listener != nil {
		return n.listener.Addr().String()
	}
	return ""
}
