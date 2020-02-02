package objects

import (
	"fmt"
	"math/rand"

	"github.com/pensando/sw/api/generated/cluster"
	iota "github.com/pensando/sw/iota/protos/gogen"
	"github.com/pensando/sw/iota/test/venice/iotakit/testbed"
)

// Naples represents a smart-nic
type Node struct {
	name     string
	iotaNode *iota.Node
	testNode *testbed.TestNode
	Nodeuuid string
}

// Naples represents a smart-nic
type Naples struct {
	Node
	SmartNic *cluster.DistributedServiceCard
}

// ThirdPartyNode represents non-naples
type ThirdPartyNode struct {
	Node
}

func NewNaplesNode(name string, node *testbed.TestNode, sn *cluster.DistributedServiceCard) *Naples {

	return &Naples{
		Node: Node{
			testNode: node,
			name:     name,
			iotaNode: node.GetIotaNode(),
			Nodeuuid: sn.Status.PrimaryMAC,
		},
		SmartNic: sn,
	}
}

func NewThirdPartyNode(name string, node *testbed.TestNode) *ThirdPartyNode {

	return &ThirdPartyNode{
		Node: Node{
			testNode: node,
			name:     name,
			iotaNode: node.GetIotaNode(),
		},
	}
}

func (n *Node) IP() string {
	return n.testNode.InstanceParams().NicMgmtIP
}

func (n *Node) Name() string {
	return n.name
}

func (n *Node) NodeName() string {
	return n.iotaNode.Name
}

func (n *Node) GetIotaNode() *iota.Node {
	return n.iotaNode
}

func (n *Node) GetTestNode() *testbed.TestNode {
	return n.testNode
}

func (n *Node) Personality() iota.PersonalityType {
	return n.testNode.Personality
}

// NaplesCollection contains a list of naples nodes
type NaplesCollection struct {
	CollectionCommon
	Nodes     []*Naples
	FakeNodes []*Naples
}

// ThirdPartyCollection contains a list of 3rd party nodes
type ThirdPartyCollection struct {
	CollectionCommon
	Nodes []*ThirdPartyNode
}

// Names retruns names of all naples in the collection
func (npc *NaplesCollection) Names() []string {
	var ret []string
	for _, n := range npc.Nodes {
		ret = append(ret, n.SmartNic.ObjectMeta.Name)
	}

	return ret
}

// Any returns the requested number of naples from collection in random
func (npc *NaplesCollection) Any(num int) *NaplesCollection {
	if npc.HasError() || len(npc.Nodes) <= num {
		return npc
	}

	newNpc := &NaplesCollection{Nodes: []*Naples{}}
	tmpArry := make([]*Naples, len(npc.Nodes))
	copy(tmpArry, npc.Nodes)
	for i := 0; i < num; i++ {
		idx := rand.Intn(len(tmpArry))
		sn := tmpArry[idx]
		tmpArry = append(tmpArry[:idx], tmpArry[idx+1:]...)
		newNpc.Nodes = append(newNpc.Nodes, sn)
	}

	return newNpc
}

// SelectByPercentage returns a collection with the specified napls based on percentage.
func (naples *NaplesCollection) SelectByPercentage(percent int) (*NaplesCollection, error) {
	if percent > 100 {
		return nil, fmt.Errorf("Invalid percentage input %v", percent)
	}

	if naples.err != nil {
		return nil, fmt.Errorf("naples collection error (%s)", naples.err)
	}

	ret := &NaplesCollection{}
	for _, entry := range naples.Nodes {
		ret.Nodes = append(ret.Nodes, entry)
		if (len(ret.Nodes)) >= (len(naples.Nodes)+len(naples.FakeNodes))*percent/100 {
			break
		}
	}

	for _, entry := range naples.FakeNodes {

		if (len(ret.Nodes) + len(ret.FakeNodes)) >= (len(naples.Nodes)+len(naples.FakeNodes))*percent/100 {
			break
		}
		ret.FakeNodes = append(ret.FakeNodes, entry)
	}

	if (len(ret.Nodes) + len(ret.FakeNodes)) == 0 {
		return nil, fmt.Errorf("Did not find hosts matching percentage (%v)", percent)
	}
	return ret, nil
}
