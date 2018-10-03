// Code generated by protoc-gen-grpc-pensando DO NOT EDIT.

/*
Package networkApiServer is a auto generated package.
Input file: network.proto
*/
package network

import (
	"reflect"

	"github.com/pensando/sw/api"
	"github.com/pensando/sw/venice/utils/runtime"
)

var typesMapNetwork = map[string]*api.Struct{

	"network.Network": &api.Struct{
		Kind: "Network", APIGroup: "network", GetTypeFn: func() reflect.Type { return reflect.TypeOf(Network{}) },
		Fields: map[string]api.Field{
			"TypeMeta": api.Field{Name: "TypeMeta", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "", Pointer: false, Slice: false, Map: false, Inline: true, FromInline: false, KeyType: "", Type: "api.TypeMeta"},

			"Kind": api.Field{Name: "Kind", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "kind", Pointer: true, Slice: false, Map: false, Inline: false, FromInline: true, KeyType: "", Type: "TYPE_STRING"},

			"APIVersion": api.Field{Name: "APIVersion", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "api-version", Pointer: true, Slice: false, Map: false, Inline: false, FromInline: true, KeyType: "", Type: "TYPE_STRING"},

			"O": api.Field{Name: "O", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "meta", Pointer: false, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "api.ObjectMeta"},

			"Spec": api.Field{Name: "Spec", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "spec", Pointer: false, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "network.NetworkSpec"},

			"Status": api.Field{Name: "Status", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "status", Pointer: false, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "network.NetworkStatus"},
		},

		CLITags: map[string]api.CLIInfo{
			"allocated-ipv4-addrs": api.CLIInfo{Path: "Status.AllocatedIPv4Addrs", Skip: false, Insert: "", Help: ""},
			"api-version":          api.CLIInfo{Path: "APIVersion", Skip: false, Insert: "", Help: ""},
			"ipv4-gateway":         api.CLIInfo{Path: "Spec.IPv4Gateway", Skip: false, Insert: "", Help: ""},
			"ipv4-subnet":          api.CLIInfo{Path: "Spec.IPv4Subnet", Skip: false, Insert: "", Help: ""},
			"ipv6-gateway":         api.CLIInfo{Path: "Spec.IPv6Gateway", Skip: false, Insert: "", Help: ""},
			"ipv6-subnet":          api.CLIInfo{Path: "Spec.IPv6Subnet", Skip: false, Insert: "", Help: ""},
			"kind":                 api.CLIInfo{Path: "Kind", Skip: false, Insert: "", Help: ""},
			"type":                 api.CLIInfo{Path: "Spec.Type", Skip: false, Insert: "", Help: ""},
			"vlan-id":              api.CLIInfo{Path: "Spec.VlanID", Skip: false, Insert: "", Help: ""},
			"vxlan-vni":            api.CLIInfo{Path: "Spec.VxlanVNI", Skip: false, Insert: "", Help: ""},
			"workloads":            api.CLIInfo{Path: "Status.Workloads", Skip: false, Insert: "", Help: ""},
		},
	},
	"network.NetworkSpec": &api.Struct{
		Kind: "", APIGroup: "", GetTypeFn: func() reflect.Type { return reflect.TypeOf(NetworkSpec{}) },
		Fields: map[string]api.Field{
			"Type": api.Field{Name: "Type", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "type", Pointer: true, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_STRING"},

			"IPv4Subnet": api.Field{Name: "IPv4Subnet", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "ipv4-subnet", Pointer: true, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_STRING"},

			"IPv4Gateway": api.Field{Name: "IPv4Gateway", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "ipv4-gateway", Pointer: true, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_STRING"},

			"IPv6Subnet": api.Field{Name: "IPv6Subnet", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "ipv6-subnet", Pointer: true, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_STRING"},

			"IPv6Gateway": api.Field{Name: "IPv6Gateway", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "ipv6-gateway", Pointer: true, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_STRING"},

			"VlanID": api.Field{Name: "VlanID", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "vlan-id", Pointer: true, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_UINT32"},

			"VxlanVNI": api.Field{Name: "VxlanVNI", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "vxlan-vni", Pointer: true, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_UINT32"},
		},
	},
	"network.NetworkStatus": &api.Struct{
		Kind: "", APIGroup: "", GetTypeFn: func() reflect.Type { return reflect.TypeOf(NetworkStatus{}) },
		Fields: map[string]api.Field{
			"Workloads": api.Field{Name: "Workloads", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "workloads", Pointer: true, Slice: true, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_STRING"},

			"AllocatedIPv4Addrs": api.Field{Name: "AllocatedIPv4Addrs", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "allocated-ipv4-addrs", Pointer: true, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_BYTES"},
		},
	},
}

func init() {
	schema := runtime.GetDefaultScheme()
	schema.AddSchema(typesMapNetwork)
}
