// Code generated by protoc-gen-grpc-pensando DO NOT EDIT.

/*
Package auditApiServer is a auto generated package.
Input file: audit.proto
*/
package audit

import (
	"reflect"

	"github.com/pensando/sw/api"
	"github.com/pensando/sw/venice/utils/runtime"
)

var typesMapAudit = map[string]*api.Struct{

	"audit.AuditEvent": &api.Struct{
		Kind: "AuditEvent", APIGroup: "audit", Scopes: []string{"Cluster"}, GetTypeFn: func() reflect.Type { return reflect.TypeOf(AuditEvent{}) },
		Fields: map[string]api.Field{
			"TypeMeta": api.Field{Name: "TypeMeta", CLITag: api.CLIInfo{ID: "T", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: true, FromInline: false, KeyType: "", Type: "api.TypeMeta"},

			"ObjectMeta": api.Field{Name: "ObjectMeta", CLITag: api.CLIInfo{ID: "meta", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "meta", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "api.ObjectMeta"},

			"EventAttributes": api.Field{Name: "EventAttributes", CLITag: api.CLIInfo{ID: "Attributes", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: true, FromInline: false, KeyType: "", Type: "audit.EventAttributes"},

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

			"Stage": api.Field{Name: "Stage", CLITag: api.CLIInfo{ID: "stage", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "stage", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: true, KeyType: "", Type: "TYPE_STRING"},

			"Level": api.Field{Name: "Level", CLITag: api.CLIInfo{ID: "level", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "level", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: true, KeyType: "", Type: "TYPE_STRING"},

			"User": api.Field{Name: "User", CLITag: api.CLIInfo{ID: "user", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "user", Pointer: true, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: true, KeyType: "", Type: "api.ObjectRef"},

			"ClientIPs": api.Field{Name: "ClientIPs", CLITag: api.CLIInfo{ID: "client-ips", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "client-ips", Pointer: false, Slice: true, Mutable: true, Map: false, Inline: false, FromInline: true, KeyType: "", Type: "TYPE_STRING"},

			"Resource": api.Field{Name: "Resource", CLITag: api.CLIInfo{ID: "resource", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "resource", Pointer: true, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: true, KeyType: "", Type: "api.ObjectRef"},

			"Action": api.Field{Name: "Action", CLITag: api.CLIInfo{ID: "action", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "action", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: true, KeyType: "", Type: "TYPE_STRING"},

			"Outcome": api.Field{Name: "Outcome", CLITag: api.CLIInfo{ID: "outcome", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "outcome", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: true, KeyType: "", Type: "TYPE_STRING"},

			"RequestURI": api.Field{Name: "RequestURI", CLITag: api.CLIInfo{ID: "request-uri", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "request-uri", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: true, KeyType: "", Type: "TYPE_STRING"},

			"RequestObject": api.Field{Name: "RequestObject", CLITag: api.CLIInfo{ID: "request-object", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "request-object", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: true, KeyType: "", Type: "TYPE_STRING"},

			"ResponseObject": api.Field{Name: "ResponseObject", CLITag: api.CLIInfo{ID: "response-object", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "response-object", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: true, KeyType: "", Type: "TYPE_STRING"},

			"GatewayNode": api.Field{Name: "GatewayNode", CLITag: api.CLIInfo{ID: "gateway-node", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "gateway-node", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: true, KeyType: "", Type: "TYPE_STRING"},

			"GatewayIP": api.Field{Name: "GatewayIP", CLITag: api.CLIInfo{ID: "gateway-ip", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "gateway-ip", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: true, KeyType: "", Type: "TYPE_STRING"},

			"ServiceName": api.Field{Name: "ServiceName", CLITag: api.CLIInfo{ID: "service-name", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "service-name", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: true, KeyType: "", Type: "TYPE_STRING"},

			"Data": api.Field{Name: "Data", CLITag: api.CLIInfo{ID: "data", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "data", Pointer: true, Slice: false, Mutable: true, Map: true, Inline: false, FromInline: true, KeyType: "TYPE_STRING", Type: "TYPE_STRING"},
		},
	},
	"audit.AuditEventList": &api.Struct{
		Kind: "", APIGroup: "", Scopes: []string{}, GetTypeFn: func() reflect.Type { return reflect.TypeOf(AuditEventList{}) },
		Fields: map[string]api.Field{
			"TypeMeta": api.Field{Name: "TypeMeta", CLITag: api.CLIInfo{ID: "T", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "api.TypeMeta"},

			"ListMeta": api.Field{Name: "ListMeta", CLITag: api.CLIInfo{ID: "ListMeta", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "api.ListMeta"},

			"Items": api.Field{Name: "Items", CLITag: api.CLIInfo{ID: "items", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "items", Pointer: true, Slice: true, Mutable: true, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "audit.AuditEvent"},

			"Kind": api.Field{Name: "Kind", CLITag: api.CLIInfo{ID: "kind", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "kind", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: true, KeyType: "", Type: "TYPE_STRING"},

			"APIVersion": api.Field{Name: "APIVersion", CLITag: api.CLIInfo{ID: "api-version", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "api-version", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: true, KeyType: "", Type: "TYPE_STRING"},

			"ResourceVersion": api.Field{Name: "ResourceVersion", CLITag: api.CLIInfo{ID: "resource-version", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "resource-version", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: true, KeyType: "", Type: "TYPE_STRING"},
		},
	},
	"audit.AuditEventRequest": &api.Struct{
		Kind: "", APIGroup: "", Scopes: []string{}, GetTypeFn: func() reflect.Type { return reflect.TypeOf(AuditEventRequest{}) },
		Fields: map[string]api.Field{
			"UUID": api.Field{Name: "UUID", CLITag: api.CLIInfo{ID: "uuid", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "uuid", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_STRING"},
		},
	},
	"audit.EventAttributes": &api.Struct{
		Kind: "", APIGroup: "", Scopes: []string{}, GetTypeFn: func() reflect.Type { return reflect.TypeOf(EventAttributes{}) },
		Fields: map[string]api.Field{
			"Stage": api.Field{Name: "Stage", CLITag: api.CLIInfo{ID: "stage", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "stage", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_STRING"},

			"Level": api.Field{Name: "Level", CLITag: api.CLIInfo{ID: "level", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "level", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_STRING"},

			"User": api.Field{Name: "User", CLITag: api.CLIInfo{ID: "user", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "user", Pointer: true, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "api.ObjectRef"},

			"ClientIPs": api.Field{Name: "ClientIPs", CLITag: api.CLIInfo{ID: "client-ips", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "client-ips", Pointer: false, Slice: true, Mutable: true, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_STRING"},

			"Resource": api.Field{Name: "Resource", CLITag: api.CLIInfo{ID: "resource", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "resource", Pointer: true, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "api.ObjectRef"},

			"Action": api.Field{Name: "Action", CLITag: api.CLIInfo{ID: "action", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "action", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_STRING"},

			"Outcome": api.Field{Name: "Outcome", CLITag: api.CLIInfo{ID: "outcome", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "outcome", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_STRING"},

			"RequestURI": api.Field{Name: "RequestURI", CLITag: api.CLIInfo{ID: "request-uri", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "request-uri", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_STRING"},

			"RequestObject": api.Field{Name: "RequestObject", CLITag: api.CLIInfo{ID: "request-object", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "request-object", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_STRING"},

			"ResponseObject": api.Field{Name: "ResponseObject", CLITag: api.CLIInfo{ID: "response-object", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "response-object", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_STRING"},

			"GatewayNode": api.Field{Name: "GatewayNode", CLITag: api.CLIInfo{ID: "gateway-node", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "gateway-node", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_STRING"},

			"GatewayIP": api.Field{Name: "GatewayIP", CLITag: api.CLIInfo{ID: "gateway-ip", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "gateway-ip", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_STRING"},

			"ServiceName": api.Field{Name: "ServiceName", CLITag: api.CLIInfo{ID: "service-name", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "service-name", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_STRING"},

			"Data": api.Field{Name: "Data", CLITag: api.CLIInfo{ID: "data", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "data", Pointer: true, Slice: false, Mutable: true, Map: true, Inline: false, FromInline: false, KeyType: "TYPE_STRING", Type: "TYPE_STRING"},
		},
	},
	"audit.EventAttributes.DataEntry": &api.Struct{
		Fields: map[string]api.Field{
			"key": api.Field{Name: "key", CLITag: api.CLIInfo{ID: "key", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_STRING"},

			"value": api.Field{Name: "value", CLITag: api.CLIInfo{ID: "value", Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "", Pointer: false, Slice: false, Mutable: true, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_STRING"},
		},
	},
}

var keyMapAudit = map[string][]api.PathsMap{}

func init() {
	schema := runtime.GetDefaultScheme()
	schema.AddSchema(typesMapAudit)
	schema.AddPaths(keyMapAudit)
}
