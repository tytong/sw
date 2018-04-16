// Code generated by protoc-gen-grpc-pensando DO NOT EDIT.

/*
Package monitoring is a auto generated package.
Input file: protos/mirror.proto
*/
package monitoring

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

type grpcServerMirrorSessionV1 struct {
	Endpoints EndpointsMirrorSessionV1Server

	AutoAddMirrorSessionHdlr    grpctransport.Handler
	AutoDeleteMirrorSessionHdlr grpctransport.Handler
	AutoGetMirrorSessionHdlr    grpctransport.Handler
	AutoListMirrorSessionHdlr   grpctransport.Handler
	AutoUpdateMirrorSessionHdlr grpctransport.Handler
}

// MakeGRPCServerMirrorSessionV1 creates a GRPC server for MirrorSessionV1 service
func MakeGRPCServerMirrorSessionV1(ctx context.Context, endpoints EndpointsMirrorSessionV1Server, logger log.Logger) MirrorSessionV1Server {
	options := []grpctransport.ServerOption{
		grpctransport.ServerErrorLogger(logger),
		grpctransport.ServerBefore(recoverVersion),
	}
	return &grpcServerMirrorSessionV1{
		Endpoints: endpoints,
		AutoAddMirrorSessionHdlr: grpctransport.NewServer(
			endpoints.AutoAddMirrorSessionEndpoint,
			DecodeGrpcReqMirrorSession,
			EncodeGrpcRespMirrorSession,
			append(options, grpctransport.ServerBefore(trace.FromGRPCRequest("AutoAddMirrorSession", logger)))...,
		),

		AutoDeleteMirrorSessionHdlr: grpctransport.NewServer(
			endpoints.AutoDeleteMirrorSessionEndpoint,
			DecodeGrpcReqMirrorSession,
			EncodeGrpcRespMirrorSession,
			append(options, grpctransport.ServerBefore(trace.FromGRPCRequest("AutoDeleteMirrorSession", logger)))...,
		),

		AutoGetMirrorSessionHdlr: grpctransport.NewServer(
			endpoints.AutoGetMirrorSessionEndpoint,
			DecodeGrpcReqMirrorSession,
			EncodeGrpcRespMirrorSession,
			append(options, grpctransport.ServerBefore(trace.FromGRPCRequest("AutoGetMirrorSession", logger)))...,
		),

		AutoListMirrorSessionHdlr: grpctransport.NewServer(
			endpoints.AutoListMirrorSessionEndpoint,
			DecodeGrpcReqListWatchOptions,
			EncodeGrpcRespMirrorSessionList,
			append(options, grpctransport.ServerBefore(trace.FromGRPCRequest("AutoListMirrorSession", logger)))...,
		),

		AutoUpdateMirrorSessionHdlr: grpctransport.NewServer(
			endpoints.AutoUpdateMirrorSessionEndpoint,
			DecodeGrpcReqMirrorSession,
			EncodeGrpcRespMirrorSession,
			append(options, grpctransport.ServerBefore(trace.FromGRPCRequest("AutoUpdateMirrorSession", logger)))...,
		),
	}
}

func (s *grpcServerMirrorSessionV1) AutoAddMirrorSession(ctx oldcontext.Context, req *MirrorSession) (*MirrorSession, error) {
	_, resp, err := s.AutoAddMirrorSessionHdlr.ServeGRPC(ctx, req)
	if err != nil {
		return nil, err
	}
	r := resp.(respMirrorSessionV1AutoAddMirrorSession).V
	return &r, resp.(respMirrorSessionV1AutoAddMirrorSession).Err
}

func decodeHTTPrespMirrorSessionV1AutoAddMirrorSession(_ context.Context, r *http.Response) (interface{}, error) {
	if r.StatusCode != http.StatusOK {
		return nil, errorDecoder(r)
	}
	var resp MirrorSession
	err := json.NewDecoder(r.Body).Decode(&resp)
	return &resp, err
}

func (s *grpcServerMirrorSessionV1) AutoDeleteMirrorSession(ctx oldcontext.Context, req *MirrorSession) (*MirrorSession, error) {
	_, resp, err := s.AutoDeleteMirrorSessionHdlr.ServeGRPC(ctx, req)
	if err != nil {
		return nil, err
	}
	r := resp.(respMirrorSessionV1AutoDeleteMirrorSession).V
	return &r, resp.(respMirrorSessionV1AutoDeleteMirrorSession).Err
}

func decodeHTTPrespMirrorSessionV1AutoDeleteMirrorSession(_ context.Context, r *http.Response) (interface{}, error) {
	if r.StatusCode != http.StatusOK {
		return nil, errorDecoder(r)
	}
	var resp MirrorSession
	err := json.NewDecoder(r.Body).Decode(&resp)
	return &resp, err
}

func (s *grpcServerMirrorSessionV1) AutoGetMirrorSession(ctx oldcontext.Context, req *MirrorSession) (*MirrorSession, error) {
	_, resp, err := s.AutoGetMirrorSessionHdlr.ServeGRPC(ctx, req)
	if err != nil {
		return nil, err
	}
	r := resp.(respMirrorSessionV1AutoGetMirrorSession).V
	return &r, resp.(respMirrorSessionV1AutoGetMirrorSession).Err
}

func decodeHTTPrespMirrorSessionV1AutoGetMirrorSession(_ context.Context, r *http.Response) (interface{}, error) {
	if r.StatusCode != http.StatusOK {
		return nil, errorDecoder(r)
	}
	var resp MirrorSession
	err := json.NewDecoder(r.Body).Decode(&resp)
	return &resp, err
}

func (s *grpcServerMirrorSessionV1) AutoListMirrorSession(ctx oldcontext.Context, req *api.ListWatchOptions) (*MirrorSessionList, error) {
	_, resp, err := s.AutoListMirrorSessionHdlr.ServeGRPC(ctx, req)
	if err != nil {
		return nil, err
	}
	r := resp.(respMirrorSessionV1AutoListMirrorSession).V
	return &r, resp.(respMirrorSessionV1AutoListMirrorSession).Err
}

func decodeHTTPrespMirrorSessionV1AutoListMirrorSession(_ context.Context, r *http.Response) (interface{}, error) {
	if r.StatusCode != http.StatusOK {
		return nil, errorDecoder(r)
	}
	var resp MirrorSessionList
	err := json.NewDecoder(r.Body).Decode(&resp)
	return &resp, err
}

func (s *grpcServerMirrorSessionV1) AutoUpdateMirrorSession(ctx oldcontext.Context, req *MirrorSession) (*MirrorSession, error) {
	_, resp, err := s.AutoUpdateMirrorSessionHdlr.ServeGRPC(ctx, req)
	if err != nil {
		return nil, err
	}
	r := resp.(respMirrorSessionV1AutoUpdateMirrorSession).V
	return &r, resp.(respMirrorSessionV1AutoUpdateMirrorSession).Err
}

func decodeHTTPrespMirrorSessionV1AutoUpdateMirrorSession(_ context.Context, r *http.Response) (interface{}, error) {
	if r.StatusCode != http.StatusOK {
		return nil, errorDecoder(r)
	}
	var resp MirrorSession
	err := json.NewDecoder(r.Body).Decode(&resp)
	return &resp, err
}

func (s *grpcServerMirrorSessionV1) AutoWatchMirrorSession(in *api.ListWatchOptions, stream MirrorSessionV1_AutoWatchMirrorSessionServer) error {
	return s.Endpoints.AutoWatchMirrorSession(in, stream)
}

func encodeHTTPAppProtoSelector(ctx context.Context, req *http.Request, request interface{}) error {
	return encodeHTTPRequest(ctx, req, request)
}

func decodeHTTPAppProtoSelector(_ context.Context, r *http.Request) (interface{}, error) {
	var req AppProtoSelector
	if e := json.NewDecoder(r.Body).Decode(&req); e != nil {
		return nil, e
	}
	return req, nil
}

// EncodeGrpcReqAppProtoSelector encodes GRPC request
func EncodeGrpcReqAppProtoSelector(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*AppProtoSelector)
	return req, nil
}

// DecodeGrpcReqAppProtoSelector decodes GRPC request
func DecodeGrpcReqAppProtoSelector(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*AppProtoSelector)
	return req, nil
}

// EncodeGrpcRespAppProtoSelector encodes GRC response
func EncodeGrpcRespAppProtoSelector(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

// DecodeGrpcRespAppProtoSelector decodes GRPC response
func DecodeGrpcRespAppProtoSelector(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

func encodeHTTPMatchRule(ctx context.Context, req *http.Request, request interface{}) error {
	return encodeHTTPRequest(ctx, req, request)
}

func decodeHTTPMatchRule(_ context.Context, r *http.Request) (interface{}, error) {
	var req MatchRule
	if e := json.NewDecoder(r.Body).Decode(&req); e != nil {
		return nil, e
	}
	return req, nil
}

// EncodeGrpcReqMatchRule encodes GRPC request
func EncodeGrpcReqMatchRule(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*MatchRule)
	return req, nil
}

// DecodeGrpcReqMatchRule decodes GRPC request
func DecodeGrpcReqMatchRule(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*MatchRule)
	return req, nil
}

// EncodeGrpcRespMatchRule encodes GRC response
func EncodeGrpcRespMatchRule(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

// DecodeGrpcRespMatchRule decodes GRPC response
func DecodeGrpcRespMatchRule(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

func encodeHTTPMatchSelector(ctx context.Context, req *http.Request, request interface{}) error {
	return encodeHTTPRequest(ctx, req, request)
}

func decodeHTTPMatchSelector(_ context.Context, r *http.Request) (interface{}, error) {
	var req MatchSelector
	if e := json.NewDecoder(r.Body).Decode(&req); e != nil {
		return nil, e
	}
	return req, nil
}

// EncodeGrpcReqMatchSelector encodes GRPC request
func EncodeGrpcReqMatchSelector(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*MatchSelector)
	return req, nil
}

// DecodeGrpcReqMatchSelector decodes GRPC request
func DecodeGrpcReqMatchSelector(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*MatchSelector)
	return req, nil
}

// EncodeGrpcRespMatchSelector encodes GRC response
func EncodeGrpcRespMatchSelector(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

// DecodeGrpcRespMatchSelector decodes GRPC response
func DecodeGrpcRespMatchSelector(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

func encodeHTTPMirrorCollector(ctx context.Context, req *http.Request, request interface{}) error {
	return encodeHTTPRequest(ctx, req, request)
}

func decodeHTTPMirrorCollector(_ context.Context, r *http.Request) (interface{}, error) {
	var req MirrorCollector
	if e := json.NewDecoder(r.Body).Decode(&req); e != nil {
		return nil, e
	}
	return req, nil
}

// EncodeGrpcReqMirrorCollector encodes GRPC request
func EncodeGrpcReqMirrorCollector(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*MirrorCollector)
	return req, nil
}

// DecodeGrpcReqMirrorCollector decodes GRPC request
func DecodeGrpcReqMirrorCollector(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*MirrorCollector)
	return req, nil
}

// EncodeGrpcRespMirrorCollector encodes GRC response
func EncodeGrpcRespMirrorCollector(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

// DecodeGrpcRespMirrorCollector decodes GRPC response
func DecodeGrpcRespMirrorCollector(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

func encodeHTTPMirrorSession(ctx context.Context, req *http.Request, request interface{}) error {
	return encodeHTTPRequest(ctx, req, request)
}

func decodeHTTPMirrorSession(_ context.Context, r *http.Request) (interface{}, error) {
	var req MirrorSession
	if e := json.NewDecoder(r.Body).Decode(&req); e != nil {
		return nil, e
	}
	return req, nil
}

// EncodeGrpcReqMirrorSession encodes GRPC request
func EncodeGrpcReqMirrorSession(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*MirrorSession)
	return req, nil
}

// DecodeGrpcReqMirrorSession decodes GRPC request
func DecodeGrpcReqMirrorSession(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*MirrorSession)
	return req, nil
}

// EncodeGrpcRespMirrorSession encodes GRC response
func EncodeGrpcRespMirrorSession(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

// DecodeGrpcRespMirrorSession decodes GRPC response
func DecodeGrpcRespMirrorSession(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

func encodeHTTPMirrorSessionList(ctx context.Context, req *http.Request, request interface{}) error {
	return encodeHTTPRequest(ctx, req, request)
}

func decodeHTTPMirrorSessionList(_ context.Context, r *http.Request) (interface{}, error) {
	var req MirrorSessionList
	if e := json.NewDecoder(r.Body).Decode(&req); e != nil {
		return nil, e
	}
	return req, nil
}

// EncodeGrpcReqMirrorSessionList encodes GRPC request
func EncodeGrpcReqMirrorSessionList(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*MirrorSessionList)
	return req, nil
}

// DecodeGrpcReqMirrorSessionList decodes GRPC request
func DecodeGrpcReqMirrorSessionList(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*MirrorSessionList)
	return req, nil
}

// EncodeGrpcRespMirrorSessionList endodes the GRPC response
func EncodeGrpcRespMirrorSessionList(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

// DecodeGrpcRespMirrorSessionList decodes the GRPC response
func DecodeGrpcRespMirrorSessionList(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

func encodeHTTPMirrorSessionSpec(ctx context.Context, req *http.Request, request interface{}) error {
	return encodeHTTPRequest(ctx, req, request)
}

func decodeHTTPMirrorSessionSpec(_ context.Context, r *http.Request) (interface{}, error) {
	var req MirrorSessionSpec
	if e := json.NewDecoder(r.Body).Decode(&req); e != nil {
		return nil, e
	}
	return req, nil
}

// EncodeGrpcReqMirrorSessionSpec encodes GRPC request
func EncodeGrpcReqMirrorSessionSpec(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*MirrorSessionSpec)
	return req, nil
}

// DecodeGrpcReqMirrorSessionSpec decodes GRPC request
func DecodeGrpcReqMirrorSessionSpec(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*MirrorSessionSpec)
	return req, nil
}

// EncodeGrpcRespMirrorSessionSpec encodes GRC response
func EncodeGrpcRespMirrorSessionSpec(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

// DecodeGrpcRespMirrorSessionSpec decodes GRPC response
func DecodeGrpcRespMirrorSessionSpec(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

func encodeHTTPMirrorSessionStatus(ctx context.Context, req *http.Request, request interface{}) error {
	return encodeHTTPRequest(ctx, req, request)
}

func decodeHTTPMirrorSessionStatus(_ context.Context, r *http.Request) (interface{}, error) {
	var req MirrorSessionStatus
	if e := json.NewDecoder(r.Body).Decode(&req); e != nil {
		return nil, e
	}
	return req, nil
}

// EncodeGrpcReqMirrorSessionStatus encodes GRPC request
func EncodeGrpcReqMirrorSessionStatus(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*MirrorSessionStatus)
	return req, nil
}

// DecodeGrpcReqMirrorSessionStatus decodes GRPC request
func DecodeGrpcReqMirrorSessionStatus(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*MirrorSessionStatus)
	return req, nil
}

// EncodeGrpcRespMirrorSessionStatus encodes GRC response
func EncodeGrpcRespMirrorSessionStatus(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

// DecodeGrpcRespMirrorSessionStatus decodes GRPC response
func DecodeGrpcRespMirrorSessionStatus(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

func encodeHTTPMirrorStartConditions(ctx context.Context, req *http.Request, request interface{}) error {
	return encodeHTTPRequest(ctx, req, request)
}

func decodeHTTPMirrorStartConditions(_ context.Context, r *http.Request) (interface{}, error) {
	var req MirrorStartConditions
	if e := json.NewDecoder(r.Body).Decode(&req); e != nil {
		return nil, e
	}
	return req, nil
}

// EncodeGrpcReqMirrorStartConditions encodes GRPC request
func EncodeGrpcReqMirrorStartConditions(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*MirrorStartConditions)
	return req, nil
}

// DecodeGrpcReqMirrorStartConditions decodes GRPC request
func DecodeGrpcReqMirrorStartConditions(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*MirrorStartConditions)
	return req, nil
}

// EncodeGrpcRespMirrorStartConditions encodes GRC response
func EncodeGrpcRespMirrorStartConditions(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

// DecodeGrpcRespMirrorStartConditions decodes GRPC response
func DecodeGrpcRespMirrorStartConditions(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

func encodeHTTPMirrorStopConditions(ctx context.Context, req *http.Request, request interface{}) error {
	return encodeHTTPRequest(ctx, req, request)
}

func decodeHTTPMirrorStopConditions(_ context.Context, r *http.Request) (interface{}, error) {
	var req MirrorStopConditions
	if e := json.NewDecoder(r.Body).Decode(&req); e != nil {
		return nil, e
	}
	return req, nil
}

// EncodeGrpcReqMirrorStopConditions encodes GRPC request
func EncodeGrpcReqMirrorStopConditions(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*MirrorStopConditions)
	return req, nil
}

// DecodeGrpcReqMirrorStopConditions decodes GRPC request
func DecodeGrpcReqMirrorStopConditions(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*MirrorStopConditions)
	return req, nil
}

// EncodeGrpcRespMirrorStopConditions encodes GRC response
func EncodeGrpcRespMirrorStopConditions(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

// DecodeGrpcRespMirrorStopConditions decodes GRPC response
func DecodeGrpcRespMirrorStopConditions(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

func encodeHTTPSmartNICMirrorSessionStatus(ctx context.Context, req *http.Request, request interface{}) error {
	return encodeHTTPRequest(ctx, req, request)
}

func decodeHTTPSmartNICMirrorSessionStatus(_ context.Context, r *http.Request) (interface{}, error) {
	var req SmartNICMirrorSessionStatus
	if e := json.NewDecoder(r.Body).Decode(&req); e != nil {
		return nil, e
	}
	return req, nil
}

// EncodeGrpcReqSmartNICMirrorSessionStatus encodes GRPC request
func EncodeGrpcReqSmartNICMirrorSessionStatus(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*SmartNICMirrorSessionStatus)
	return req, nil
}

// DecodeGrpcReqSmartNICMirrorSessionStatus decodes GRPC request
func DecodeGrpcReqSmartNICMirrorSessionStatus(ctx context.Context, request interface{}) (interface{}, error) {
	req := request.(*SmartNICMirrorSessionStatus)
	return req, nil
}

// EncodeGrpcRespSmartNICMirrorSessionStatus encodes GRC response
func EncodeGrpcRespSmartNICMirrorSessionStatus(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}

// DecodeGrpcRespSmartNICMirrorSessionStatus decodes GRPC response
func DecodeGrpcRespSmartNICMirrorSessionStatus(ctx context.Context, response interface{}) (interface{}, error) {
	return response, nil
}
