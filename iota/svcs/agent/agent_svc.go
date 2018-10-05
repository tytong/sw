package agent

import (
	"fmt"
	"io"
	"os"

	log "github.com/sirupsen/logrus"

	iota "github.com/pensando/sw/iota/protos/gogen"
	"github.com/pensando/sw/iota/svcs/common"
)

const (
	agentDir       = "/tmp/iota-agent"
	nodeAddTimeout = 300
)

//IotaNode interface
type IotaNode interface {
	Init(*iota.Node) (*iota.Node, error)

	// DeleteNode, remove the personaltiy set
	Destroy(*iota.Node) (*iota.Node, error)

	// AddWorkloads brings up a workload type on a given node
	AddWorkload(*iota.Workload) (*iota.Workload, error)

	// DeleteWorkloads deletes a given workload
	DeleteWorkload(*iota.Workload) (*iota.Workload, error)

	// Trigger invokes the workload's trigger. It could be ping, start client/server etc..
	Trigger(*iota.TriggerMsg) (*iota.TriggerMsg, error)

	// CheckClusterHealth returns the cluster health
	CheckHealth(*iota.NodeHealth) (*iota.NodeHealth, error)

	//Type
	NodeType() iota.PersonalityType

	//Type
	NodeName() string
}

//Base implementations for all node
type iotaNode struct {
	name   string
	logger *log.Logger // Logger for each entity.
}

func (s *iotaNode) log(msg interface{}) {

	if s.logger == nil {
		s.logger = log.New()
		file, err := os.OpenFile(agentDir+"/"+s.name+".log", os.O_CREATE|os.O_WRONLY|os.O_APPEND, 0666)
		if err != nil {
			log.Fatalln("Failed to open log file", "file.txt", ":", err)
		}
		s.logger.Out = io.MultiWriter(file, os.Stdout)
	}
	s.logger.Println(msg)
}

func (s *iotaNode) NodeName() string {
	return s.name
}

func (s *iotaNode) Destroy(*iota.Node) (*iota.Node, error) {
	s.logger.Printf("Doing Node clean up.")
	return &iota.Node{NodeStatus: &iota.IotaAPIResponse{ApiStatus: iota.APIResponseType_API_STATUS_OK}}, nil
}

// IOTAAgentListenURL is the default URL for IOTA Agent
var IOTAAgentListenURL = fmt.Sprintf(":%d", common.IotaAgentPort)

// StartIOTAAgent starts IOTA Agent
func StartIOTAAgent(stubMode *bool) {
	agentSvc, err := common.CreateNewGRPCServer("IOTA Agent", IOTAAgentListenURL)
	if err != nil {
		log.Errorf("Could not start IOTA Agent. Err: %v", err)
	}
	// Change this to NewAgentService when ready to integrate
	if !*stubMode {
		agentHandler := NewAgentService()
		iota.RegisterIotaAgentApiServer(agentSvc.Srv, agentHandler)
	} else {
		agentHandler := NewAgentStubService()
		iota.RegisterIotaAgentApiServer(agentSvc.Srv, agentHandler)
	}
	agentSvc.Start()
}
