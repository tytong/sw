// Code generated by protoc-gen-grpc-pensando DO NOT EDIT.

/*
Package rolloutApiServer is a auto generated package.
Input file: rollout.proto
*/
package rollout

import (
	"reflect"

	"github.com/pensando/sw/api"
	"github.com/pensando/sw/venice/utils/runtime"
)

var typesMapRollout = map[string]*api.Struct{

	"rollout.Rollout": &api.Struct{
		GetTypeFn: func() reflect.Type { return reflect.TypeOf(Rollout{}) },
		Fields: map[string]api.Field{
			"TypeMeta": api.Field{Name: "TypeMeta", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "", Pointer: false, Slice: false, Map: false, Inline: true, FromInline: false, KeyType: "", Type: "api.TypeMeta"},

			"Kind": api.Field{Name: "Kind", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "kind", Pointer: true, Slice: false, Map: false, Inline: false, FromInline: true, KeyType: "", Type: "TYPE_STRING"},

			"APIVersion": api.Field{Name: "APIVersion", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "api-version", Pointer: true, Slice: false, Map: false, Inline: false, FromInline: true, KeyType: "", Type: "TYPE_STRING"},

			"O": api.Field{Name: "O", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "meta", Pointer: false, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "api.ObjectMeta"},

			"Spec": api.Field{Name: "Spec", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "spec", Pointer: false, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "rollout.RolloutSpec"},

			"Status": api.Field{Name: "Status", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "status", Pointer: false, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "rollout.RolloutStatus"},
		},

		CLITags: map[string]api.CLIInfo{
			"api-version":        api.CLIInfo{Path: "APIVersion", Skip: false, Insert: "", Help: ""},
			"completion-percent": api.CLIInfo{Path: "Status.CompletionPercentage", Skip: false, Insert: "", Help: ""},
			"duration":           api.CLIInfo{Path: "Spec.Duration", Skip: false, Insert: "", Help: ""},
			"kind":               api.CLIInfo{Path: "Kind", Skip: false, Insert: "", Help: ""},
			"max-nic-failures-before-abort": api.CLIInfo{Path: "Spec.MaxNICFailuresBeforeAbort", Skip: false, Insert: "", Help: ""},
			"max-parallel":                  api.CLIInfo{Path: "Spec.MaxParallel", Skip: false, Insert: "", Help: ""},
			"message":                       api.CLIInfo{Path: "Status.SmartNICsStatus[].Message", Skip: false, Insert: "", Help: ""},
			"name":                          api.CLIInfo{Path: "Status.SmartNICsStatus[].Name", Skip: false, Insert: "", Help: ""},
			"phase":                         api.CLIInfo{Path: "Status.SmartNICsStatus[].Phase", Skip: false, Insert: "", Help: ""},
			"prev-version":                  api.CLIInfo{Path: "Status.PreviousVersion", Skip: false, Insert: "", Help: ""},
			"reason":                        api.CLIInfo{Path: "Status.SmartNICsStatus[].Reason", Skip: false, Insert: "", Help: ""},
			"smartnic-must-match-constraint": api.CLIInfo{Path: "Spec.SmartNICMustMatchConstraint", Skip: false, Insert: "", Help: ""},
			"smartnics-only":                 api.CLIInfo{Path: "Spec.SmartNICsOnly", Skip: false, Insert: "", Help: ""},
			"state":                          api.CLIInfo{Path: "Status.OperationalState", Skip: false, Insert: "", Help: ""},
			"strategy":                       api.CLIInfo{Path: "Spec.Strategy", Skip: false, Insert: "", Help: ""},
			"suspend":                        api.CLIInfo{Path: "Spec.Suspend", Skip: false, Insert: "", Help: ""},
			"upgrade-type":                   api.CLIInfo{Path: "Spec.UpgradeType", Skip: false, Insert: "", Help: ""},
			"version":                        api.CLIInfo{Path: "Spec.Version", Skip: false, Insert: "", Help: ""},
		},
	},
	"rollout.RolloutPhase": &api.Struct{
		GetTypeFn: func() reflect.Type { return reflect.TypeOf(RolloutPhase{}) },
		Fields: map[string]api.Field{
			"Name": api.Field{Name: "Name", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "name", Pointer: true, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_STRING"},

			"Phase": api.Field{Name: "Phase", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "phase", Pointer: true, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_STRING"},

			"StartTime": api.Field{Name: "StartTime", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "start-time", Pointer: true, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "api.Timestamp"},

			"EndTime": api.Field{Name: "EndTime", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "end-time", Pointer: true, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "api.Timestamp"},

			"Reason": api.Field{Name: "Reason", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "reason", Pointer: true, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_STRING"},

			"Message": api.Field{Name: "Message", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "message", Pointer: true, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_STRING"},
		},
	},
	"rollout.RolloutSpec": &api.Struct{
		GetTypeFn: func() reflect.Type { return reflect.TypeOf(RolloutSpec{}) },
		Fields: map[string]api.Field{
			"Version": api.Field{Name: "Version", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "version", Pointer: true, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_STRING"},

			"ScheduledStartTime": api.Field{Name: "ScheduledStartTime", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "scheduled-start-time", Pointer: true, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "api.Timestamp"},

			"Duration": api.Field{Name: "Duration", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "duration", Pointer: true, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_STRING"},

			"Strategy": api.Field{Name: "Strategy", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "strategy", Pointer: true, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_STRING"},

			"MaxParallel": api.Field{Name: "MaxParallel", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "max-parallel", Pointer: true, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_UINT32"},

			"MaxNICFailuresBeforeAbort": api.Field{Name: "MaxNICFailuresBeforeAbort", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "max-nic-failures-before-abort", Pointer: true, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_UINT32"},

			"OrderConstraints": api.Field{Name: "OrderConstraints", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "order-constraints", Pointer: true, Slice: true, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "labels.Selector"},

			"Suspend": api.Field{Name: "Suspend", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "suspend", Pointer: true, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_BOOL"},

			"SmartNICsOnly": api.Field{Name: "SmartNICsOnly", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "smartnics-only", Pointer: true, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_BOOL"},

			"SmartNICMustMatchConstraint": api.Field{Name: "SmartNICMustMatchConstraint", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "smartnic-must-match-constraint", Pointer: true, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_BOOL"},

			"UpgradeType": api.Field{Name: "UpgradeType", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "upgrade-type", Pointer: true, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_STRING"},
		},
	},
	"rollout.RolloutStatus": &api.Struct{
		GetTypeFn: func() reflect.Type { return reflect.TypeOf(RolloutStatus{}) },
		Fields: map[string]api.Field{
			"ControllerNodesStatus": api.Field{Name: "ControllerNodesStatus", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "controller-nodes-status", Pointer: true, Slice: true, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "rollout.RolloutPhase"},

			"ControllerServicesStatus": api.Field{Name: "ControllerServicesStatus", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "controller-services-status", Pointer: true, Slice: true, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "rollout.RolloutPhase"},

			"SmartNICsStatus": api.Field{Name: "SmartNICsStatus", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "smartnics-status", Pointer: true, Slice: true, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "rollout.RolloutPhase"},

			"OperationalState": api.Field{Name: "OperationalState", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "state", Pointer: true, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_STRING"},

			"CompletionPercentage": api.Field{Name: "CompletionPercentage", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "completion-percent", Pointer: true, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_UINT32"},

			"StartTime": api.Field{Name: "StartTime", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "start-time", Pointer: true, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "api.Timestamp"},

			"EndTime": api.Field{Name: "EndTime", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "end-time", Pointer: true, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "api.Timestamp"},

			"PreviousVersion": api.Field{Name: "PreviousVersion", CLITag: api.CLIInfo{Path: "", Skip: false, Insert: "", Help: ""}, JSONTag: "prev-version", Pointer: true, Slice: false, Map: false, Inline: false, FromInline: false, KeyType: "", Type: "TYPE_STRING"},
		},
	},
}

func init() {
	schema := runtime.GetDefaultScheme()
	schema.AddSchema(typesMapRollout)
}
