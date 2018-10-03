// Code generated by protoc-gen-grpc-pensando DO NOT EDIT.

/*
Package rolloutApiServer is a auto generated package.
Input file: svc_rollout.proto
*/
package rollout

import (
	"reflect"

	"github.com/pensando/sw/api"
	"github.com/pensando/sw/venice/utils/runtime"
)

var typesMapSvc_rollout = map[string]*api.Struct{

	"rollout.AutoMsgRolloutWatchHelper": &api.Struct{
		Kind: "", APIGroup: "", GetTypeFn: func() reflect.Type { return reflect.TypeOf(AutoMsgRolloutWatchHelper{}) },
		Fields: map[string]api.Field{
			"Events": api.Field{Name: "Events", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "", Pointer: true, Slice: true, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "rollout.AutoMsgRolloutWatchHelper.WatchEvent"},
		},
	},
	"rollout.AutoMsgRolloutWatchHelper.WatchEvent": &api.Struct{
		Kind: "", APIGroup: "", GetTypeFn: func() reflect.Type { return reflect.TypeOf(AutoMsgRolloutWatchHelper_WatchEvent{}) },
		Fields: map[string]api.Field{
			"Type": api.Field{Name: "Type", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "", Pointer: true, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_STRING"},

			"Object": api.Field{Name: "Object", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "", Pointer: true, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "rollout.Rollout"},
		},
	},
	"rollout.RolloutList": &api.Struct{
		Kind: "", APIGroup: "", GetTypeFn: func() reflect.Type { return reflect.TypeOf(RolloutList{}) },
		Fields: map[string]api.Field{
			"T": api.Field{Name: "T", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "", Pointer: false, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "api.TypeMeta"},

			"ListMeta": api.Field{Name: "ListMeta", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "", Pointer: false, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "api.ListMeta"},

			"Items": api.Field{Name: "Items", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "", Pointer: true, Slice: true, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "rollout.Rollout"},
		},
	},
}

func init() {
	schema := runtime.GetDefaultScheme()
	schema.AddSchema(typesMapSvc_rollout)
}
