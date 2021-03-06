// Code generated by protoc-gen-grpc-pensando DO NOT EDIT.

/*
Package aggwatch is a auto generated package.
Input file: svc_aggwatch.proto
*/
package aggwatch

import (
	"context"

	"github.com/pensando/sw/api"
	"github.com/pensando/sw/venice/utils/log"
)

// Dummy definitions to suppress nonused warnings
var _ api.ObjectMeta

type grpcServerAggWatchV1 struct {
	Endpoints EndpointsAggWatchV1Server
}

// MakeGRPCServerAggWatchV1 creates a GRPC server for AggWatchV1 service
func MakeGRPCServerAggWatchV1(ctx context.Context, endpoints EndpointsAggWatchV1Server, logger log.Logger) AggWatchV1Server {
	return &grpcServerAggWatchV1{
		Endpoints: endpoints,
	}
}

func (s *grpcServerAggWatchV1) AutoWatchSvcAggWatchV1(in *api.AggWatchOptions, stream AggWatchV1_AutoWatchSvcAggWatchV1Server) error {
	return s.Endpoints.AutoWatchSvcAggWatchV1(in, stream)
}
