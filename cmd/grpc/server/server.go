package server

import (
	"github.com/pensando/sw/cmd/env"
	"github.com/pensando/sw/cmd/grpc"
	"github.com/pensando/sw/cmd/grpc/service"
	"github.com/pensando/sw/cmd/types"
	"github.com/pensando/sw/utils/log"
	"github.com/pensando/sw/utils/rpckit"
)

// RunServer creates a gRPC server for cluster operations.
func RunServer(url, certFile, keyFile, caFile string, stopChannel chan bool) {
	// create an RPC server.
	rpcServer, err := rpckit.NewRPCServer("cmd", url)
	if err != nil {
		log.Fatalf("Error creating grpc server: %v", err)
	}
	defer func() { rpcServer.Stop() }()

	env.RPCServer = rpcServer

	// create and register the RPC handler for cluster object.
	grpc.RegisterClusterServer(rpcServer.GrpcServer, &clusterRPCHandler{})

	// create and register the RPC handler for SmartNIC service
	grpc.RegisterSmartNICServer(rpcServer.GrpcServer, NewSmartNICServer())

	if env.ResolverService != nil {
		types.RegisterServiceAPIServer(env.RPCServer.GrpcServer, service.NewRPCHandler(env.ResolverService))
	}

	// wait forever
	<-stopChannel
}
