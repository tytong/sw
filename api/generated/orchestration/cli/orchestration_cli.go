// Code generated by protoc-gen-grpc-pensando DO NOT EDIT.

/*
Package orchestrationCliUtilsBackend is a auto generated package.
Input file: orchestration.proto
*/
package cli

import (
	"github.com/pensando/sw/api"
	"github.com/pensando/sw/api/generated/orchestration"
	"github.com/pensando/sw/venice/cli/gen"
)

// CreateOrchestratorFlags specifies flags for Orchestrator create operation
var CreateOrchestratorFlags = []gen.CliFlag{
	{
		ID:     "manage-namespaces",
		Type:   "StringSlice",
		Help:   "",
		Skip:   false,
		Insert: "",
	},
	{
		ID:     "type",
		Type:   "String",
		Help:   "",
		Skip:   false,
		Insert: "",
	},
	{
		ID:     "uri",
		Type:   "String",
		Help:   "",
		Skip:   false,
		Insert: "",
	},
}

func removeOrchestratorOper(obj interface{}) error {
	if v, ok := obj.(*orchestration.Orchestrator); ok {
		v.UUID = ""
		v.ResourceVersion = ""
		v.CreationTime = api.Timestamp{}
		v.ModTime = api.Timestamp{}
		v.Status = orchestration.OrchestratorStatus{}
	}
	return nil
}

func init() {
	cl := gen.GetInfo()

	cl.AddCliInfo("orchestration.Orchestrator", "create", CreateOrchestratorFlags)
	cl.AddRemoveObjOperFunc("orchestration.Orchestrator", removeOrchestratorOper)

}
