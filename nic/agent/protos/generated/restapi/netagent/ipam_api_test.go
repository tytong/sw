// Code generated by protoc-gen-grpc-pensando DO NOT EDIT.

/*
Package netproto is a auto generated package.
Input file: ipam.proto
*/
package restapi_test

import (
	"testing"

	"github.com/pensando/sw/api"
	"github.com/pensando/sw/nic/agent/protos/netproto"
	"github.com/pensando/sw/venice/utils/netutils"
	. "github.com/pensando/sw/venice/utils/testutils"
)

func TestIPAMPolicyList(t *testing.T) {
	t.Parallel()
	var ok bool
	var ipampolicyList []*netproto.IPAMPolicy

	err := netutils.HTTPGet("http://"+agentRestURL+"/api/ipampolicys/", &ipampolicyList)

	AssertOk(t, err, "Error getting ipampolicys from the REST Server")
	for _, o := range ipampolicyList {
		if o.Name == "preCreatedIPAMPolicy" {
			ok = true
			break
		}
	}
	if !ok {
		t.Errorf("Could not find preCreatedIPAMPolicy in Response: %v", ipampolicyList)
	}

}

func TestIPAMPolicyCreateErr(t *testing.T) {
	t.Parallel()
	var resp Response
	badPostData := netproto.IPAMPolicy{
		TypeMeta: api.TypeMeta{Kind: "IPAMPolicy"},
		ObjectMeta: api.ObjectMeta{
			Name: "",
		},
	}

	Assert(t, err != nil, "Expected test to error out with 500. It passed instead")
}

func TestIPAMPolicyDeleteErr(t *testing.T) {
	t.Parallel()
	var resp Response
	badDelData := netproto.IPAMPolicy{
		TypeMeta: api.TypeMeta{Kind: "IPAMPolicy"},
		ObjectMeta: api.ObjectMeta{Tenant: "default",
			Namespace: "default",
			Name:      "badObject"},
	}

	Assert(t, err != nil, "Expected test to error out with 500. It passed instead")
}

func TestIPAMPolicyUpdateErr(t *testing.T) {
	t.Parallel()
	var resp Response
	badDelData := netproto.IPAMPolicy{
		TypeMeta: api.TypeMeta{Kind: "IPAMPolicy"},
		ObjectMeta: api.ObjectMeta{Tenant: "default",
			Namespace: "default",
			Name:      "badObject"},
	}

	Assert(t, err != nil, "Expected test to error out with 500. It passed instead")
}