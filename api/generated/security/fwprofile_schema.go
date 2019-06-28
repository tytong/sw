// Code generated by protoc-gen-grpc-pensando DO NOT EDIT.

/*
Package securityApiServer is a auto generated package.
Input file: fwprofile.proto
*/
package security

import (
	"reflect"

	"github.com/pensando/sw/api"
	"github.com/pensando/sw/venice/utils/runtime"
)

var typesMapFwprofile = map[string]*api.Struct{

	"security.FirewallProfile": &api.Struct{
		Kind: "FirewallProfile", APIGroup: "security", Scopes: []string{"Tenant"}, GetTypeFn: func() reflect.Type { return reflect.TypeOf(FirewallProfile{}) },
		Fields: map[string]api.Field{
			"TypeMeta": api.Field{Name: "TypeMeta", CLITag: api.CLIInfo{ID: "T", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: true, FromInline: false, KeyType: "", Type: "api.TypeMeta"},

			"ObjectMeta": api.Field{Name: "ObjectMeta", CLITag: api.CLIInfo{ID: "meta", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "meta", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "api.ObjectMeta"},

			"Spec": api.Field{Name: "Spec", CLITag: api.CLIInfo{ID: "spec", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "spec", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "security.FirewallProfileSpec"},

			"Status": api.Field{Name: "Status", CLITag: api.CLIInfo{ID: "status", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "status", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "security.FirewallProfileStatus"},

			"Kind": api.Field{Name: "Kind", CLITag: api.CLIInfo{ID: "kind", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "kind", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: true, KeyType: "", Type: "TYPE_STRING"},

			"APIVersion": api.Field{Name: "APIVersion", CLITag: api.CLIInfo{ID: "api-version", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "api-version", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: true, KeyType: "", Type: "TYPE_STRING"},

			"Name": api.Field{Name: "Name", CLITag: api.CLIInfo{ID: "name", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "name", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: true, KeyType: "", Type: "TYPE_STRING"},

			"Tenant": api.Field{Name: "Tenant", CLITag: api.CLIInfo{ID: "tenant", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "tenant", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: true, KeyType: "", Type: "TYPE_STRING"},

			"Namespace": api.Field{Name: "Namespace", CLITag: api.CLIInfo{ID: "namespace", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "namespace", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: true, KeyType: "", Type: "TYPE_STRING"},

			"GenerationID": api.Field{Name: "GenerationID", CLITag: api.CLIInfo{ID: "generation-id", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "generation-id", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: true, KeyType: "", Type: "TYPE_STRING"},

			"ResourceVersion": api.Field{Name: "ResourceVersion", CLITag: api.CLIInfo{ID: "resource-version", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "resource-version", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: true, KeyType: "", Type: "TYPE_STRING"},

			"UUID": api.Field{Name: "UUID", CLITag: api.CLIInfo{ID: "uuid", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "uuid", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: true, KeyType: "", Type: "TYPE_STRING"},

			"Labels": api.Field{Name: "Labels", CLITag: api.CLIInfo{ID: "labels", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "labels", Pointer: true, Slice: false, Mutable: true, Map: true, Inline: false, FromInline: true, KeyType: "TYPE_STRING", Type: "TYPE_STRING"},

			"CreationTime": api.Field{Name: "CreationTime", CLITag: api.CLIInfo{ID: "creation-time", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "creation-time", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: true, KeyType: "", Type: "api.Timestamp"},

			"ModTime": api.Field{Name: "ModTime", CLITag: api.CLIInfo{ID: "mod-time", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "mod-time", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: true, KeyType: "", Type: "api.Timestamp"},

			"SelfLink": api.Field{Name: "SelfLink", CLITag: api.CLIInfo{ID: "self-link", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "self-link", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: true, KeyType: "", Type: "TYPE_STRING"},
		},

		CLITags: map[string]api.CLIInfo{
			"api-version":                  api.CLIInfo{Path: "APIVersion", Skip: false, Insert: "", Help: ""},
			"drop-timeout":                 api.CLIInfo{Path: "Spec.DropTimeout", Skip: false, Insert: "", Help: ""},
			"generation-id":                api.CLIInfo{Path: "GenerationID", Skip: false, Insert: "", Help: ""},
			"icmp-drop-timeout":            api.CLIInfo{Path: "Spec.ICMPDropTimeout", Skip: false, Insert: "", Help: ""},
			"icmp-timeout":                 api.CLIInfo{Path: "Spec.IcmpTimeout", Skip: false, Insert: "", Help: ""},
			"kind":                         api.CLIInfo{Path: "Kind", Skip: false, Insert: "", Help: ""},
			"labels":                       api.CLIInfo{Path: "Labels", Skip: false, Insert: "", Help: ""},
			"min-version":                  api.CLIInfo{Path: "Status.PropagationStatus.MinVersion", Skip: false, Insert: "", Help: ""},
			"name":                         api.CLIInfo{Path: "Name", Skip: false, Insert: "", Help: ""},
			"namespace":                    api.CLIInfo{Path: "Namespace", Skip: false, Insert: "", Help: ""},
			"pending":                      api.CLIInfo{Path: "Status.PropagationStatus.Pending", Skip: false, Insert: "", Help: ""},
			"resource-version":             api.CLIInfo{Path: "ResourceVersion", Skip: false, Insert: "", Help: ""},
			"self-link":                    api.CLIInfo{Path: "SelfLink", Skip: false, Insert: "", Help: ""},
			"session-idle-timeout":         api.CLIInfo{Path: "Spec.SessionIdleTimeout", Skip: false, Insert: "", Help: ""},
			"tcp-close-timeout":            api.CLIInfo{Path: "Spec.TCPCloseTimeout", Skip: false, Insert: "", Help: ""},
			"tcp-connection-setup-timeout": api.CLIInfo{Path: "Spec.TCPConnectionSetupTimeout", Skip: false, Insert: "", Help: ""},
			"tcp-drop-timeout":             api.CLIInfo{Path: "Spec.TCPDropTimeout", Skip: false, Insert: "", Help: ""},
			"tcp-half-closed-timeout":      api.CLIInfo{Path: "Spec.TCPHalfClosedTimeout", Skip: false, Insert: "", Help: ""},
			"tcp-timeout":                  api.CLIInfo{Path: "Spec.TcpTimeout", Skip: false, Insert: "", Help: ""},
			"tenant":                       api.CLIInfo{Path: "Tenant", Skip: false, Insert: "", Help: ""},
			"udp-drop-timeout":             api.CLIInfo{Path: "Spec.UDPDropTimeout", Skip: false, Insert: "", Help: ""},
			"udp-timeout":                  api.CLIInfo{Path: "Spec.UdpTimeout", Skip: false, Insert: "", Help: ""},
			"updated":                      api.CLIInfo{Path: "Status.PropagationStatus.Updated", Skip: false, Insert: "", Help: ""},
			"uuid":                         api.CLIInfo{Path: "UUID", Skip: false, Insert: "", Help: ""},
		},
	},
	"security.FirewallProfilePropagationStatus": &api.Struct{
		Kind: "", APIGroup: "", Scopes: []string{}, GetTypeFn: func() reflect.Type { return reflect.TypeOf(FirewallProfilePropagationStatus{}) },
		Fields: map[string]api.Field{
			"GenerationID": api.Field{Name: "GenerationID", CLITag: api.CLIInfo{ID: "generation-id", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "generation-id", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_STRING"},

			"Updated": api.Field{Name: "Updated", CLITag: api.CLIInfo{ID: "updated", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "updated", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_INT32"},

			"Pending": api.Field{Name: "Pending", CLITag: api.CLIInfo{ID: "pending", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "pending", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_INT32"},

			"MinVersion": api.Field{Name: "MinVersion", CLITag: api.CLIInfo{ID: "min-version", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "min-version", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_STRING"},
		},
	},
	"security.FirewallProfileSpec": &api.Struct{
		Kind: "", APIGroup: "", Scopes: []string{}, GetTypeFn: func() reflect.Type { return reflect.TypeOf(FirewallProfileSpec{}) },
		Fields: map[string]api.Field{
			"SessionIdleTimeout": api.Field{Name: "SessionIdleTimeout", CLITag: api.CLIInfo{ID: "session-idle-timeout", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "session-idle-timeout", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_STRING"},

			"TCPConnectionSetupTimeout": api.Field{Name: "TCPConnectionSetupTimeout", CLITag: api.CLIInfo{ID: "tcp-connection-setup-timeout", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "tcp-connection-setup-timeout", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_STRING"},

			"TCPCloseTimeout": api.Field{Name: "TCPCloseTimeout", CLITag: api.CLIInfo{ID: "tcp-close-timeout", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "tcp-close-timeout", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_STRING"},

			"TCPHalfClosedTimeout": api.Field{Name: "TCPHalfClosedTimeout", CLITag: api.CLIInfo{ID: "tcp-half-closed-timeout", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "tcp-half-closed-timeout", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_STRING"},

			"TCPDropTimeout": api.Field{Name: "TCPDropTimeout", CLITag: api.CLIInfo{ID: "tcp-drop-timeout", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "tcp-drop-timeout", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_STRING"},

			"UDPDropTimeout": api.Field{Name: "UDPDropTimeout", CLITag: api.CLIInfo{ID: "udp-drop-timeout", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "udp-drop-timeout", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_STRING"},

			"ICMPDropTimeout": api.Field{Name: "ICMPDropTimeout", CLITag: api.CLIInfo{ID: "icmp-drop-timeout", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "icmp-drop-timeout", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_STRING"},

			"DropTimeout": api.Field{Name: "DropTimeout", CLITag: api.CLIInfo{ID: "drop-timeout", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "drop-timeout", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_STRING"},

			"TcpTimeout": api.Field{Name: "TcpTimeout", CLITag: api.CLIInfo{ID: "tcp-timeout", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "tcp-timeout", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_STRING"},

			"UdpTimeout": api.Field{Name: "UdpTimeout", CLITag: api.CLIInfo{ID: "udp-timeout", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "udp-timeout", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_STRING"},

			"IcmpTimeout": api.Field{Name: "IcmpTimeout", CLITag: api.CLIInfo{ID: "icmp-timeout", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "icmp-timeout", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_STRING"},
		},
	},
	"security.FirewallProfileStatus": &api.Struct{
		Kind: "", APIGroup: "", Scopes: []string{}, GetTypeFn: func() reflect.Type { return reflect.TypeOf(FirewallProfileStatus{}) },
		Fields: map[string]api.Field{
			"PropagationStatus": api.Field{Name: "PropagationStatus", CLITag: api.CLIInfo{ID: "propagation-status", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "propagation-status", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "security.FirewallProfilePropagationStatus"},
		},
	},
}

var keyMapFwprofile = map[string][]api.PathsMap{}

func init() {
	schema := runtime.GetDefaultScheme()
	schema.AddSchema(typesMapFwprofile)
	schema.AddPaths(keyMapFwprofile)
}
