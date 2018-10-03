// Code generated by protoc-gen-grpc-pensando DO NOT EDIT.

/*
Package workloadApiServer is a auto generated package.
Input file: workload.proto
*/
package workload

import (
	"reflect"

	"github.com/pensando/sw/api"
	"github.com/pensando/sw/venice/utils/runtime"
)

var typesMapWorkload = map[string]*api.Struct{

	"workload.Workload": &api.Struct{
		Kind: "Workload", APIGroup: "workload", GetTypeFn: func() reflect.Type { return reflect.TypeOf(Workload{}) },
		Fields: map[string]api.Field{
			"TypeMeta": api.Field{Name: "TypeMeta", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "", Pointer: false, Slice: false, Map: false, Inline: true, FromInline: false, KeyType: "", Type: "api.TypeMeta"},

			"Kind": api.Field{Name: "Kind", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "kind", Pointer: true, Slice: false, Map: false, Inline: false, FromInline: true, KeyType: "", Type: "TYPE_STRING"},

			"APIVersion": api.Field{Name: "APIVersion", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "api-version", Pointer: true, Slice: false, Map: false, Inline: false, FromInline: true, KeyType: "", Type: "TYPE_STRING"},

			"O": api.Field{Name: "O", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "meta", Pointer: false, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "api.ObjectMeta"},

			"Spec": api.Field{Name: "Spec", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "spec", Pointer: false, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "workload.WorkloadSpec"},

			"Status": api.Field{Name: "Status", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "status", Pointer: false, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "workload.WorkloadStatus"},
		},

		CLITags: map[string]api.CLIInfo{
			"api-version":    api.CLIInfo{Path: "APIVersion", Skip: false, Insert: "", Help: ""},
			"endpoint":       api.CLIInfo{Path: "Status.Interfaces[].Endpoint", Skip: false, Insert: "", Help: ""},
			"external-vlan":  api.CLIInfo{Path: "Spec.Interfaces[].ExternalVlan", Skip: false, Insert: "", Help: ""},
			"host-name":      api.CLIInfo{Path: "Spec.HostName", Skip: false, Insert: "", Help: ""},
			"ip-addresses":   api.CLIInfo{Path: "Status.Interfaces[].IpAddrs", Skip: false, Insert: "", Help: ""},
			"kind":           api.CLIInfo{Path: "Kind", Skip: false, Insert: "", Help: ""},
			"micro-seg-vlan": api.CLIInfo{Path: "Spec.Interfaces[].MicroSegVlan", Skip: false, Insert: "", Help: ""},
		},
	},
	"workload.WorkloadIntfSpec": &api.Struct{
		Kind: "", APIGroup: "", GetTypeFn: func() reflect.Type { return reflect.TypeOf(WorkloadIntfSpec{}) },
		Fields: map[string]api.Field{
			"MicroSegVlan": api.Field{Name: "MicroSegVlan", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "micro-seg-vlan", Pointer: true, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_UINT32"},

			"ExternalVlan": api.Field{Name: "ExternalVlan", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "external-vlan", Pointer: true, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_UINT32"},
		},
	},
	"workload.WorkloadIntfStatus": &api.Struct{
		Kind: "", APIGroup: "", GetTypeFn: func() reflect.Type { return reflect.TypeOf(WorkloadIntfStatus{}) },
		Fields: map[string]api.Field{
			"IpAddrs": api.Field{Name: "IpAddrs", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "ip-addresses", Pointer: true, Slice: true, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_STRING"},

			"Endpoint": api.Field{Name: "Endpoint", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "endpoint", Pointer: true, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_STRING"},
		},
	},
	"workload.WorkloadSpec": &api.Struct{
		Kind: "", APIGroup: "", GetTypeFn: func() reflect.Type { return reflect.TypeOf(WorkloadSpec{}) },
		Fields: map[string]api.Field{
			"HostName": api.Field{Name: "HostName", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "host-name", Pointer: true, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_STRING"},

			"Interfaces": api.Field{Name: "Interfaces", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "interfaces", Pointer: false, Slice: false, Map: true, Inline: false, FromInline: false, KeyType: "TYPE_STRING", Type: "workload.WorkloadIntfSpec"},
		},
	},
	"workload.WorkloadSpec.InterfacesEntry": &api.Struct{
		Fields: map[string]api.Field{
			"key": api.Field{Name: "key", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "", Pointer: true, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_STRING"},

			"value": api.Field{Name: "value", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "", Pointer: true, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "workload.WorkloadIntfSpec"},
		},
	},
	"workload.WorkloadStatus": &api.Struct{
		Kind: "", APIGroup: "", GetTypeFn: func() reflect.Type { return reflect.TypeOf(WorkloadStatus{}) },
		Fields: map[string]api.Field{
			"Interfaces": api.Field{Name: "Interfaces", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "interfaces", Pointer: false, Slice: false, Map: true, Inline: false, FromInline: false, KeyType: "TYPE_STRING", Type: "workload.WorkloadIntfStatus"},
		},
	},
	"workload.WorkloadStatus.InterfacesEntry": &api.Struct{
		Fields: map[string]api.Field{
			"key": api.Field{Name: "key", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "", Pointer: true, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_STRING"},

			"value": api.Field{Name: "value", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "", Pointer: true, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "workload.WorkloadIntfStatus"},
		},
	},
}

func init() {
	schema := runtime.GetDefaultScheme()
	schema.AddSchema(typesMapWorkload)
}
