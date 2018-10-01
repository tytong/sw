// Code generated by protoc-gen-grpc-pensando DO NOT EDIT.

/*
Package rollout is a auto generated package.
Input file: svc_rollout.proto
*/
package rollout

import (
	"context"
	"encoding/json"
	"net/http"

	grpctransport "github.com/go-kit/kit/transport/grpc"
	oldcontext "golang.org/x/net/context"

	"github.com/pensando/sw/api"
	"github.com/pensando/sw/venice/utils/log"
	"github.com/pensando/sw/venice/utils/trace"
)

// Dummy definitions to suppress nonused warnings
var _ api.ObjectMeta

type grpcServerRolloutV1 struct {
	Endpoints EndpointsRolloutV1Server

	AutoAddRolloutHdlr    grpctransport.Handler
	AutoDeleteRolloutHdlr grpctransport.Handler
	AutoGetRolloutHdlr    grpctransport.Handler
	AutoListRolloutHdlr   grpctransport.Handler
	AutoUpdateRolloutHdlr grpctransport.Handler
}

// MakeGRPCServerRolloutV1 creates a GRPC server for RolloutV1 service
func MakeGRPCServerRolloutV1(ctx context.Context, endpoints EndpointsRolloutV1Server, logger log.Logger) RolloutV1Server {
	options := []grpctransport.ServerOption{
		grpctransport.ServerErrorLogger(logger),
		grpctransport.ServerBefore(recoverVersion),
	}
	return &grpcServerRolloutV1{
		Endpoints: endpoints,
		AutoAddRolloutHdlr: grpctransport.NewServer(
			endpoints.AutoAddRolloutEndpoint,
			DecodeGrpcReqRollout,
			EncodeGrpcRespRollout,
			append(options, grpctransport.ServerBefore(trace.FromGRPCRequest("AutoAddRollout", logger)))...,
		),

		AutoDeleteRolloutHdlr: grpctransport.NewServer(
			endpoints.AutoDeleteRolloutEndpoint,
			DecodeGrpcReqRollout,
			EncodeGrpcRespRollout,
			append(options, grpctransport.ServerBefore(trace.FromGRPCRequest("AutoDeleteRollout", logger)))...,
		),

		AutoGetRolloutHdlr: grpctransport.NewServer(
			endpoints.AutoGetRolloutEndpoint,
			DecodeGrpcReqRollout,
			EncodeGrpcRespRollout,
			append(options, grpctransport.ServerBefore(trace.FromGRPCRequest("AutoGetRollout", logger)))...,
		),

		AutoListRolloutHdlr: grpctransport.NewServer(
			endpoints.AutoListRolloutEndpoint,
			DecodeGrpcReqListWatchOptions,
			EncodeGrpcRespRolloutList,
			append(options, grpctransport.ServerBefore(trace.FromGRPCRequest("AutoListRollout", logger)))...,
		),

		AutoUpdateRolloutHdlr: grpctransport.NewServer(
			endpoints.AutoUpdateRolloutEndpoint,
			DecodeGrpcReqRollout,
			EncodeGrpcRespRollout,
			append(options, grpctransport.ServerBefore(trace.FromGRPCRequest("AutoUpdateRollout", logger)))...,
		),
	}
}

func (s *grpcServerRolloutV1) AutoAddRollout(ctx oldcontext.Context, req *Rollout) (*Rollout, error) {
	_, resp, err := s.AutoAddRolloutHdlr.ServeGRPC(ctx, req)
	if err != nil {
		return nil, err
	}
	r := resp.(respRolloutV1AutoAddRollout).V
	return &r, resp.(respRolloutV1AutoAddRollout).Err
}

func decodeHTTPrespRolloutV1AutoAddRollout(_ context.Context, r *http.Response) (interface{}, error) {
	if r.StatusCode != http.StatusOK {
		return nil, errorDecoder(r)
	}
	var resp Rollout
	err := json.NewDecoder(r.Body).Decode(&resp)
	return &resp, err
}

func (s *grpcServerRolloutV1) AutoDeleteRollout(ctx oldcontext.Context, req *Rollout) (*Rollout, error) {
	_, resp, err := s.AutoDeleteRolloutHdlr.ServeGRPC(ctx, req)
	if err != nil {
		return nil, err
	}
	r := resp.(respRolloutV1AutoDeleteRollout).V
	return &r, resp.(respRolloutV1AutoDeleteRollout).Err
}

func decodeHTTPrespRolloutV1AutoDeleteRollout(_ context.Context, r *http.Response) (interface{}, error) {
	if r.StatusCode != http.StatusOK {
		return nil, errorDecoder(r)
	}
	var resp Rollout
	err := json.NewDecoder(r.Body).Decode(&resp)
	return &resp, err
}

func (s *grpcServerRolloutV1) AutoGetRollout(ctx oldcontext.Context, req *Rollout) (*Rollout, error) {
	_, resp, err := s.AutoGetRolloutHdlr.ServeGRPC(ctx, req)
	if err != nil {
		return nil, err
	}
	r := resp.(respRolloutV1AutoGetRollout).V
	return &r, resp.(respRolloutV1AutoGetRollout).Err
}

func decodeHTTPrespRolloutV1AutoGetRollout(_ context.Context, r *http.Response) (interface{}, error) {
	if r.StatusCode != http.StatusOK {
		return nil, errorDecoder(r)
	}
	var resp Rollout
	err := json.NewDecoder(r.Body).Decode(&resp)
	return &resp, err
}

func (s *grpcServerRolloutV1) AutoListRollout(ctx oldcontext.Context, req *api.ListWatchOptions) (*RolloutList, error) {
	_, resp, err := s.AutoListRolloutHdlr.ServeGRPC(ctx, req)
	if err != nil {
		return nil, err
	}
	r := resp.(respRolloutV1AutoListRollout).V
	return &r, resp.(respRolloutV1AutoListRollout).Err
}

func decodeHTTPrespRolloutV1AutoListRollout(_ context.Context, r *http.Response) (interface{}, error) {
	if r.StatusCode != http.StatusOK {
		return nil, errorDecoder(r)
	}
	var resp RolloutList
	err := json.NewDecoder(r.Body).Decode(&resp)
	return &resp, err
}

func (s *grpcServerRolloutV1) AutoUpdateRollout(ctx oldcontext.Context, req *Rollout) (*Rollout, error) {
	_, resp, err := s.AutoUpdateRolloutHdlr.ServeGRPC(ctx, req)
	if err != nil {
		return nil, err
	}
	r := resp.(respRolloutV1AutoUpdateRollout).V
	return &r, resp.(respRolloutV1AutoUpdateRollout).Err
}

func decodeHTTPrespRolloutV1AutoUpdateRollout(_ context.Context, r *http.Response) (interface{}, error) {
	if r.StatusCode != http.StatusOK {
		return nil, errorDecoder(r)
	}
	var resp Rollout
	err := json.NewDecoder(r.Body).Decode(&resp)
	return &resp, err
}

func (s *grpcServerRolloutV1) AutoWatchSvcRolloutV1(in *api.ListWatchOptions, stream RolloutV1_AutoWatchSvcRolloutV1Server) error {
	return s.Endpoints.AutoWatchSvcRolloutV1(in, stream)
}

func (s *grpcServerRolloutV1) AutoWatchRollout(in *api.ListWatchOptions, stream RolloutV1_AutoWatchRolloutServer) error {
	return s.Endpoints.AutoWatchRollout(in, stream)
}

func encodeHTTPRolloutList(ctx context.Context, req *http.Request, request interface{}) error {
	return encodeHTTPRequest(ctx, req, request)
}

func decodeHTTPRolloutList(_ context.Context, r *http.Request) (interface{}, error) {
	var req RolloutList
	if e := json.NewDecoder(r.Body).Decode(&req); e != nil {
		return nil, e
	}
	return req, nil
}

// EncodeGrpcReqRolloutList encodes GRPC request
func EncodeGrpcReqRolloutList(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*RolloutList)
	return req, nil
}

// DecodeGrpcReqRolloutList decodes GRPC request
func DecodeGrpcReqRolloutList(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*RolloutList)
	return req, nil
}

// EncodeGrpcRespRolloutList endodes the GRPC response
func EncodeGrpcRespRolloutList(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

// DecodeGrpcRespRolloutList decodes the GRPC response
func DecodeGrpcRespRolloutList(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}
