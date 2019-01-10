// Code generated by protoc-gen-grpc-pensando DO NOT EDIT.

/*
Package netproto is a auto generated package.
Input file: secprofile.proto
*/
package restapi

import (
	"testing"

	api "github.com/pensando/sw/api"
	"github.com/pensando/sw/nic/agent/netagent/protos/netproto"
	"github.com/pensando/sw/venice/utils/netutils"
	. "github.com/pensando/sw/venice/utils/testutils"
)

func TestSecurityProfileList(t *testing.T) {
	t.Parallel()
	var ok bool
	var securityprofileList []*netproto.SecurityProfile

	err := netutils.HTTPGet("http://"+agentRestURL+"/api/security/profiles/", &securityprofileList)

	AssertOk(t, err, "Error getting securityprofiles from the REST Server")
	for _, o := range securityprofileList {
		if o.Name == "preCreatedSecurityProfile" {
			ok = true
			break
		}
	}
	if !ok {
		t.Errorf("Could not find preCreatedSecurityProfile in Response: %v", securityprofileList)
	}

}

func TestSecurityProfilePost(t *testing.T) {
	t.Parallel()
	var resp Response
	var ok bool
	var securityprofileList []*netproto.SecurityProfile

	postData := netproto.SecurityProfile{
		TypeMeta: api.TypeMeta{Kind: "SecurityProfile"},
		ObjectMeta: api.ObjectMeta{
			Tenant:    "default",
			Namespace: "default",
			Name:      "testPostSecurityProfile",
		},
		Spec: netproto.SecurityProfileSpec{
			Timeouts: &netproto.Timeouts{
				SessionIdle:        "10s",
				TCP:                "1m",
				TCPDrop:            "5s",
				TCPConnectionSetup: "300ms",
				TCPClose:           "1h",
				Drop:               "30s",
				UDP:                "5s",
				UDPDrop:            "1s",
				ICMP:               "100ms",
				ICMPDrop:           "1h10m15s",
			},
		},
	}
	err := netutils.HTTPPost("http://"+agentRestURL+"/api/security/profiles/", &postData, &resp)
	getErr := netutils.HTTPGet("http://"+agentRestURL+"/api/security/profiles/", &securityprofileList)

	AssertOk(t, err, "Error posting securityprofile to REST Server")
	AssertOk(t, getErr, "Error getting securityprofiles from the REST Server")
	for _, o := range securityprofileList {
		if o.Name == "testPostSecurityProfile" {
			ok = true
			break
		}
	}
	if !ok {
		t.Errorf("Could not find testPostSecurityProfile in Response: %v", securityprofileList)
	}

}

func TestSecurityProfileDelete(t *testing.T) {
	t.Parallel()
	var resp Response
	var found bool
	var securityprofileList []*netproto.SecurityProfile

	deleteData := netproto.SecurityProfile{
		TypeMeta: api.TypeMeta{Kind: "SecurityProfile"},
		ObjectMeta: api.ObjectMeta{
			Tenant:    "default",
			Namespace: "default",
			Name:      "testDeleteSecurityProfile",
		},
		Spec: netproto.SecurityProfileSpec{
			Timeouts: &netproto.Timeouts{
				SessionIdle:        "10s",
				TCP:                "1m",
				TCPDrop:            "5s",
				TCPConnectionSetup: "300ms",
				TCPClose:           "1h",
				Drop:               "30s",
				UDP:                "5s",
				UDPDrop:            "1s",
				ICMP:               "100ms",
				ICMPDrop:           "1h10m15s",
			},
		},
	}
	postErr := netutils.HTTPPost("http://"+agentRestURL+"/api/security/profiles/", &deleteData, &resp)
	err := netutils.HTTPDelete("http://"+agentRestURL+"/api/security/profiles/default/default/testDeleteSecurityProfile", &deleteData, &resp)
	getErr := netutils.HTTPGet("http://"+agentRestURL+"/api/security/profiles/", &securityprofileList)

	AssertOk(t, postErr, "Error posting securityprofile to REST Server")
	AssertOk(t, err, "Error deleting securityprofile from REST Server")
	AssertOk(t, getErr, "Error getting securityprofiles from the REST Server")
	for _, o := range securityprofileList {
		if o.Name == "testDeleteSecurityProfile" {
			found = true
			break
		}
	}
	if found {
		t.Errorf("Found testDeleteSecurityProfile in Response after deleting: %v", securityprofileList)
	}

}

func TestSecurityProfileUpdate(t *testing.T) {
	t.Parallel()
	var resp Response
	var securityprofileList []*netproto.SecurityProfile

	var actualSecurityProfileSpec netproto.SecurityProfileSpec
	updatedSecurityProfileSpec := netproto.SecurityProfileSpec{
		Timeouts: &netproto.Timeouts{
			SessionIdle: "20s",
		},
	}
	putData := netproto.SecurityProfile{
		TypeMeta: api.TypeMeta{Kind: "SecurityProfile"},
		ObjectMeta: api.ObjectMeta{
			Tenant:    "default",
			Name:      "preCreatedSecurityProfile",
			Namespace: "default",
		},
		Spec: updatedSecurityProfileSpec,
	}
	err := netutils.HTTPPut("http://"+agentRestURL+"/api/security/profiles/default/default/preCreatedSecurityProfile", &putData, &resp)
	AssertOk(t, err, "Error updating securityprofile to REST Server")

	getErr := netutils.HTTPGet("http://"+agentRestURL+"/api/security/profiles/", &securityprofileList)
	AssertOk(t, getErr, "Error getting securityprofiles from the REST Server")
	for _, o := range securityprofileList {
		if o.Name == "preCreatedSecurityProfile" {
			actualSecurityProfileSpec = o.Spec
			break
		}
	}
	AssertEquals(t, updatedSecurityProfileSpec, actualSecurityProfileSpec, "Could not validate updated spec.")

}

func TestSecurityProfileCreateErr(t *testing.T) {
	t.Parallel()
	var resp Response
	badPostData := netproto.SecurityProfile{
		TypeMeta: api.TypeMeta{Kind: "SecurityProfile"},
		ObjectMeta: api.ObjectMeta{
			Name: "",
		},
	}

	err := netutils.HTTPPost("http://"+agentRestURL+"/api/security/profiles/", &badPostData, &resp)

	Assert(t, err != nil, "Expected test to error out with 500. It passed instead")
}

func TestSecurityProfileDeleteErr(t *testing.T) {
	t.Parallel()
	var resp Response
	badDelData := netproto.SecurityProfile{
		TypeMeta: api.TypeMeta{Kind: "SecurityProfile"},
		ObjectMeta: api.ObjectMeta{Tenant: "default",
			Namespace: "default",
			Name:      "badObject"},
	}

	err := netutils.HTTPDelete("http://"+agentRestURL+"/api/security/profiles/default/default/badObject", &badDelData, &resp)

	Assert(t, err != nil, "Expected test to error out with 500. It passed instead")
}

func TestSecurityProfileUpdateErr(t *testing.T) {
	t.Parallel()
	var resp Response
	badDelData := netproto.SecurityProfile{
		TypeMeta: api.TypeMeta{Kind: "SecurityProfile"},
		ObjectMeta: api.ObjectMeta{Tenant: "default",
			Namespace: "default",
			Name:      "badObject"},
	}

	err := netutils.HTTPPut("http://"+agentRestURL+"/api/security/profiles/default/default/badObject", &badDelData, &resp)

	Assert(t, err != nil, "Expected test to error out with 500. It passed instead")
}
