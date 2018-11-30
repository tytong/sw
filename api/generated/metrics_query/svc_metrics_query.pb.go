// Code generated by protoc-gen-gogo. DO NOT EDIT.
// source: svc_metrics_query.proto

package metrics_query

import proto "github.com/gogo/protobuf/proto"
import fmt "fmt"
import math "math"
import _ "github.com/pensando/grpc-gateway/third_party/googleapis/google/api"
import _ "github.com/pensando/sw/venice/utils/apigen/annotations"
import _ "github.com/gogo/protobuf/gogoproto"
import api "github.com/pensando/sw/api"

import (
	context "golang.org/x/net/context"
	grpc "google.golang.org/grpc"
)

// Reference imports to suppress errors if they are not otherwise used.
var _ = proto.Marshal
var _ = fmt.Errorf
var _ = math.Inf

// Reference imports to suppress errors if they are not otherwise used.
var _ context.Context
var _ grpc.ClientConn

// This is a compile-time assertion to ensure that this generated file
// is compatible with the grpc package it is being compiled against.
const _ = grpc.SupportPackageIsVersion4

// Client API for MetricsV1 service

type MetricsV1Client interface {
	AutoWatchSvcMetricsV1(ctx context.Context, in *api.ListWatchOptions, opts ...grpc.CallOption) (MetricsV1_AutoWatchSvcMetricsV1Client, error)
	//  Query is the telemetry metrics query RPC, http://localhost:9000/metrics/v1/query
	Query(ctx context.Context, in *QueryList, opts ...grpc.CallOption) (*QueryResponse, error)
}

type metricsV1Client struct {
	cc *grpc.ClientConn
}

func NewMetricsV1Client(cc *grpc.ClientConn) MetricsV1Client {
	return &metricsV1Client{cc}
}

func (c *metricsV1Client) AutoWatchSvcMetricsV1(ctx context.Context, in *api.ListWatchOptions, opts ...grpc.CallOption) (MetricsV1_AutoWatchSvcMetricsV1Client, error) {
	stream, err := grpc.NewClientStream(ctx, &_MetricsV1_serviceDesc.Streams[0], c.cc, "/metrics_query.MetricsV1/AutoWatchSvcMetricsV1", opts...)
	if err != nil {
		return nil, err
	}
	x := &metricsV1AutoWatchSvcMetricsV1Client{stream}
	if err := x.ClientStream.SendMsg(in); err != nil {
		return nil, err
	}
	if err := x.ClientStream.CloseSend(); err != nil {
		return nil, err
	}
	return x, nil
}

type MetricsV1_AutoWatchSvcMetricsV1Client interface {
	Recv() (*api.WatchEventList, error)
	grpc.ClientStream
}

type metricsV1AutoWatchSvcMetricsV1Client struct {
	grpc.ClientStream
}

func (x *metricsV1AutoWatchSvcMetricsV1Client) Recv() (*api.WatchEventList, error) {
	m := new(api.WatchEventList)
	if err := x.ClientStream.RecvMsg(m); err != nil {
		return nil, err
	}
	return m, nil
}

func (c *metricsV1Client) Query(ctx context.Context, in *QueryList, opts ...grpc.CallOption) (*QueryResponse, error) {
	out := new(QueryResponse)
	err := grpc.Invoke(ctx, "/metrics_query.MetricsV1/Query", in, out, c.cc, opts...)
	if err != nil {
		return nil, err
	}
	return out, nil
}

// Server API for MetricsV1 service

type MetricsV1Server interface {
	AutoWatchSvcMetricsV1(*api.ListWatchOptions, MetricsV1_AutoWatchSvcMetricsV1Server) error
	//  Query is the telemetry metrics query RPC, http://localhost:9000/metrics/v1/query
	Query(context.Context, *QueryList) (*QueryResponse, error)
}

func RegisterMetricsV1Server(s *grpc.Server, srv MetricsV1Server) {
	s.RegisterService(&_MetricsV1_serviceDesc, srv)
}

func _MetricsV1_AutoWatchSvcMetricsV1_Handler(srv interface{}, stream grpc.ServerStream) error {
	m := new(api.ListWatchOptions)
	if err := stream.RecvMsg(m); err != nil {
		return err
	}
	return srv.(MetricsV1Server).AutoWatchSvcMetricsV1(m, &metricsV1AutoWatchSvcMetricsV1Server{stream})
}

type MetricsV1_AutoWatchSvcMetricsV1Server interface {
	Send(*api.WatchEventList) error
	grpc.ServerStream
}

type metricsV1AutoWatchSvcMetricsV1Server struct {
	grpc.ServerStream
}

func (x *metricsV1AutoWatchSvcMetricsV1Server) Send(m *api.WatchEventList) error {
	return x.ServerStream.SendMsg(m)
}

func _MetricsV1_Query_Handler(srv interface{}, ctx context.Context, dec func(interface{}) error, interceptor grpc.UnaryServerInterceptor) (interface{}, error) {
	in := new(QueryList)
	if err := dec(in); err != nil {
		return nil, err
	}
	if interceptor == nil {
		return srv.(MetricsV1Server).Query(ctx, in)
	}
	info := &grpc.UnaryServerInfo{
		Server:     srv,
		FullMethod: "/metrics_query.MetricsV1/Query",
	}
	handler := func(ctx context.Context, req interface{}) (interface{}, error) {
		return srv.(MetricsV1Server).Query(ctx, req.(*QueryList))
	}
	return interceptor(ctx, in, info, handler)
}

var _MetricsV1_serviceDesc = grpc.ServiceDesc{
	ServiceName: "metrics_query.MetricsV1",
	HandlerType: (*MetricsV1Server)(nil),
	Methods: []grpc.MethodDesc{
		{
			MethodName: "Query",
			Handler:    _MetricsV1_Query_Handler,
		},
	},
	Streams: []grpc.StreamDesc{
		{
			StreamName:    "AutoWatchSvcMetricsV1",
			Handler:       _MetricsV1_AutoWatchSvcMetricsV1_Handler,
			ServerStreams: true,
		},
	},
	Metadata: "svc_metrics_query.proto",
}

func init() { proto.RegisterFile("svc_metrics_query.proto", fileDescriptorSvcMetricsQuery) }

var fileDescriptorSvcMetricsQuery = []byte{
	// 352 bytes of a gzipped FileDescriptorProto
	0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xff, 0x74, 0x90, 0x3f, 0x4b, 0x33, 0x31,
	0x1c, 0xc7, 0x9b, 0x42, 0xfb, 0x3c, 0xcf, 0x3d, 0x38, 0x78, 0xa5, 0x68, 0xce, 0xd2, 0xa1, 0xe0,
	0x52, 0xe8, 0xc5, 0xea, 0xe6, 0xa6, 0xd0, 0x4d, 0xf1, 0x1f, 0xa8, 0x88, 0x50, 0xd2, 0xf4, 0xe7,
	0x35, 0x70, 0x4d, 0x62, 0x93, 0xbb, 0xe2, 0x7a, 0xed, 0x2b, 0xd0, 0xad, 0x93, 0xb3, 0xa3, 0x93,
	0xa3, 0xa3, 0x9b, 0x82, 0x6f, 0x40, 0x8a, 0x2f, 0x44, 0x2e, 0xad, 0xd8, 0x42, 0xdd, 0xf2, 0xfb,
	0xfe, 0xe3, 0x43, 0x9c, 0x15, 0x1d, 0xb3, 0x66, 0x17, 0x4c, 0x8f, 0x33, 0xdd, 0xbc, 0x8e, 0xa0,
	0x77, 0xe3, 0xab, 0x9e, 0x34, 0xd2, 0x5d, 0x9a, 0x13, 0xbd, 0x52, 0x20, 0x65, 0x10, 0x02, 0xa1,
	0x8a, 0x13, 0x2a, 0x84, 0x34, 0xd4, 0x70, 0x29, 0xf4, 0x24, 0xec, 0x35, 0x02, 0x6e, 0x3a, 0x51,
	0xcb, 0x67, 0xb2, 0x4b, 0x14, 0x08, 0x4d, 0x45, 0x5b, 0x12, 0xdd, 0x27, 0x31, 0x08, 0xce, 0x80,
	0x44, 0x86, 0x87, 0x3a, 0xad, 0x06, 0x20, 0x66, 0xdb, 0x84, 0x0b, 0x16, 0x46, 0x6d, 0xf8, 0x9e,
	0xa9, 0xcd, 0xcc, 0x04, 0x32, 0x90, 0xc4, 0xca, 0xad, 0xe8, 0xca, 0x5e, 0xf6, 0xb0, 0xaf, 0x69,
	0xbc, 0xb0, 0x80, 0xdb, 0x5b, 0xff, 0x05, 0x25, 0x05, 0xef, 0x82, 0xa1, 0x93, 0xd8, 0xe6, 0x2b,
	0x72, 0xfe, 0xed, 0x4f, 0xea, 0xa7, 0x75, 0xf7, 0xdc, 0x29, 0xee, 0x44, 0x46, 0x9e, 0x51, 0xc3,
	0x3a, 0x27, 0x31, 0xfb, 0x31, 0x8a, 0x3e, 0x55, 0xdc, 0xdf, 0xe3, 0xda, 0x58, 0xef, 0x40, 0x59,
	0x6e, 0xaf, 0x60, 0x65, 0x2b, 0x35, 0x62, 0x10, 0x26, 0x0d, 0x54, 0x96, 0x1f, 0x87, 0x38, 0xd7,
	0x4f, 0xb5, 0xa7, 0x21, 0x46, 0xcf, 0x43, 0x9c, 0xd9, 0x40, 0xee, 0xa5, 0x93, 0x3b, 0x4a, 0xe9,
	0xdc, 0x55, 0x7f, 0x9e, 0xd6, 0xaa, 0x69, 0xcf, 0x2b, 0x2d, 0x72, 0x8e, 0x41, 0x2b, 0x29, 0x34,
	0x54, 0xd6, 0x92, 0xf7, 0xcf, 0xbb, 0x6c, 0xb1, 0x92, 0x27, 0xd6, 0xdd, 0x46, 0xd5, 0x8b, 0xbf,
	0xee, 0xf4, 0xf0, 0x9c, 0xdb, 0x01, 0xce, 0xc6, 0xf5, 0xd1, 0x00, 0x67, 0x76, 0xab, 0xa3, 0x04,
	0xff, 0x57, 0x20, 0x6a, 0x8c, 0x1b, 0xda, 0x86, 0xf0, 0x3e, 0xc1, 0x99, 0x87, 0x04, 0xff, 0x99,
	0x8e, 0xbf, 0x8c, 0xcb, 0xe8, 0x6d, 0x5c, 0x46, 0x1f, 0xe3, 0x32, 0x3a, 0x44, 0xad, 0xbc, 0xfd,
	0x86, 0xad, 0xaf, 0x00, 0x00, 0x00, 0xff, 0xff, 0xa7, 0x71, 0x66, 0x0c, 0x00, 0x02, 0x00, 0x00,
}
