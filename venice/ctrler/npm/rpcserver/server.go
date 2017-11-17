// {C} Copyright 2017 Pensando Systems Inc. All rights reserved.

package rpcserver

import (
	"github.com/pensando/sw/venice/ctrler/npm/rpcserver/netproto"
	"github.com/pensando/sw/venice/ctrler/npm/statemgr"
	"github.com/pensando/sw/venice/utils/debug"
	"github.com/pensando/sw/venice/utils/log"
	"github.com/pensando/sw/venice/utils/rpckit"
)

// RPCServer is the RPC server object
type RPCServer struct {
	stateMgr        *statemgr.Statemgr  // reference to network state manager
	grpcServer      *rpckit.RPCServer   // gRPC server instance
	networkHandler  *NetworkRPCServer   // network RPC server
	endpointHandler *EndpointRPCHandler // endpoint RPC handler
	securityHandler *SecurityRPCServer  // security RPC handler
	debugStats      *debug.Stats        // Debug stats
}

// Stop stops the rpc server
func (rs *RPCServer) Stop() error {
	// stop the rpc server
	return rs.grpcServer.Stop()
}

// NewRPCServer creates a new instance of
func NewRPCServer(listenURL string, stateMgr *statemgr.Statemgr, debugStats *debug.Stats) (*RPCServer, error) {
	// create an RPC server
	grpcServer, err := rpckit.NewRPCServer("netctrler", listenURL)
	if err != nil {
		log.Fatalf("Error creating rpc server. Err; %v", err)
	}

	// create network RPC server
	networkHandler, err := NewNetworkRPCServer(stateMgr, debugStats)
	if err != nil {
		log.Fatalf("Error creating network rpc server. Err; %v", err)
	}

	// create endpoint RPC server
	endpointHandler, err := NewEndpointRPCServer(stateMgr)
	if err != nil {
		log.Fatalf("Error creating endpoint rpc server. Err; %v", err)
	}

	securityHandler, err := NewSecurityRPCServer(stateMgr)
	if err != nil {
		log.Fatalf("Error creating security rpc server. Err; %v", err)
	}

	// register the RPC handlers and start the server
	netproto.RegisterNetworkApiServer(grpcServer.GrpcServer, networkHandler)
	netproto.RegisterEndpointApiServer(grpcServer.GrpcServer, endpointHandler)
	netproto.RegisterSecurityApiServer(grpcServer.GrpcServer, securityHandler)
	grpcServer.Start()

	// create rpc server object
	rpcServer := RPCServer{
		stateMgr:        stateMgr,
		grpcServer:      grpcServer,
		networkHandler:  networkHandler,
		endpointHandler: endpointHandler,
		securityHandler: securityHandler,
		debugStats:      debugStats,
	}

	return &rpcServer, nil
}
