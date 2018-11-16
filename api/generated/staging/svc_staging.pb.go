// Code generated by protoc-gen-gogo. DO NOT EDIT.
// source: svc_staging.proto

package staging

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

import io "io"

// Reference imports to suppress errors if they are not otherwise used.
var _ = proto.Marshal
var _ = fmt.Errorf
var _ = math.Inf

// AutoMsgBufferWatchHelper is a wrapper object for watch events for Buffer objects
type AutoMsgBufferWatchHelper struct {
	Events []*AutoMsgBufferWatchHelper_WatchEvent `protobuf:"bytes,1,rep,name=Events,json=events" json:"events"`
}

func (m *AutoMsgBufferWatchHelper) Reset()         { *m = AutoMsgBufferWatchHelper{} }
func (m *AutoMsgBufferWatchHelper) String() string { return proto.CompactTextString(m) }
func (*AutoMsgBufferWatchHelper) ProtoMessage()    {}
func (*AutoMsgBufferWatchHelper) Descriptor() ([]byte, []int) {
	return fileDescriptorSvcStaging, []int{0}
}

func (m *AutoMsgBufferWatchHelper) GetEvents() []*AutoMsgBufferWatchHelper_WatchEvent {
	if m != nil {
		return m.Events
	}
	return nil
}

type AutoMsgBufferWatchHelper_WatchEvent struct {
	Type   string  `protobuf:"bytes,1,opt,name=Type,proto3" json:"type,omitempty"`
	Object *Buffer `protobuf:"bytes,2,opt,name=Object" json:"object,omitempty"`
}

func (m *AutoMsgBufferWatchHelper_WatchEvent) Reset()         { *m = AutoMsgBufferWatchHelper_WatchEvent{} }
func (m *AutoMsgBufferWatchHelper_WatchEvent) String() string { return proto.CompactTextString(m) }
func (*AutoMsgBufferWatchHelper_WatchEvent) ProtoMessage()    {}
func (*AutoMsgBufferWatchHelper_WatchEvent) Descriptor() ([]byte, []int) {
	return fileDescriptorSvcStaging, []int{0, 0}
}

func (m *AutoMsgBufferWatchHelper_WatchEvent) GetType() string {
	if m != nil {
		return m.Type
	}
	return ""
}

func (m *AutoMsgBufferWatchHelper_WatchEvent) GetObject() *Buffer {
	if m != nil {
		return m.Object
	}
	return nil
}

// BufferList is a container object for list of Buffer objects
type BufferList struct {
	api.TypeMeta `protobuf:"bytes,2,opt,name=T,json=,inline,embedded=T" json:",inline"`
	api.ListMeta `protobuf:"bytes,3,opt,name=ListMeta,json=list-meta,inline,embedded=ListMeta" json:"list-meta,inline"`
	// List of Buffer objects
	Items []*Buffer `protobuf:"bytes,4,rep,name=Items,json=items" json:"items"`
}

func (m *BufferList) Reset()                    { *m = BufferList{} }
func (m *BufferList) String() string            { return proto.CompactTextString(m) }
func (*BufferList) ProtoMessage()               {}
func (*BufferList) Descriptor() ([]byte, []int) { return fileDescriptorSvcStaging, []int{1} }

func (m *BufferList) GetItems() []*Buffer {
	if m != nil {
		return m.Items
	}
	return nil
}

func init() {
	proto.RegisterType((*AutoMsgBufferWatchHelper)(nil), "staging.AutoMsgBufferWatchHelper")
	proto.RegisterType((*AutoMsgBufferWatchHelper_WatchEvent)(nil), "staging.AutoMsgBufferWatchHelper.WatchEvent")
	proto.RegisterType((*BufferList)(nil), "staging.BufferList")
}

// Reference imports to suppress errors if they are not otherwise used.
var _ context.Context
var _ grpc.ClientConn

// This is a compile-time assertion to ensure that this generated file
// is compatible with the grpc package it is being compiled against.
const _ = grpc.SupportPackageIsVersion4

// Client API for StagingV1 service

type StagingV1Client interface {
	// Create Buffer object
	AutoAddBuffer(ctx context.Context, in *Buffer, opts ...grpc.CallOption) (*Buffer, error)
	// Delete Buffer object
	AutoDeleteBuffer(ctx context.Context, in *Buffer, opts ...grpc.CallOption) (*Buffer, error)
	// Get Buffer object
	AutoGetBuffer(ctx context.Context, in *Buffer, opts ...grpc.CallOption) (*Buffer, error)
	// List Buffer objects
	AutoListBuffer(ctx context.Context, in *api.ListWatchOptions, opts ...grpc.CallOption) (*BufferList, error)
	// Update Buffer object
	AutoUpdateBuffer(ctx context.Context, in *Buffer, opts ...grpc.CallOption) (*Buffer, error)
	// Watch Buffer objects. Supports WebSockets or HTTP long poll
	AutoWatchBuffer(ctx context.Context, in *api.ListWatchOptions, opts ...grpc.CallOption) (StagingV1_AutoWatchBufferClient, error)
	AutoWatchSvcStagingV1(ctx context.Context, in *api.ListWatchOptions, opts ...grpc.CallOption) (StagingV1_AutoWatchSvcStagingV1Client, error)
	Clear(ctx context.Context, in *ClearAction, opts ...grpc.CallOption) (*ClearAction, error)
	Commit(ctx context.Context, in *CommitAction, opts ...grpc.CallOption) (*CommitAction, error)
}

type stagingV1Client struct {
	cc *grpc.ClientConn
}

func NewStagingV1Client(cc *grpc.ClientConn) StagingV1Client {
	return &stagingV1Client{cc}
}

func (c *stagingV1Client) AutoAddBuffer(ctx context.Context, in *Buffer, opts ...grpc.CallOption) (*Buffer, error) {
	out := new(Buffer)
	err := grpc.Invoke(ctx, "/staging.StagingV1/AutoAddBuffer", in, out, c.cc, opts...)
	if err != nil {
		return nil, err
	}
	return out, nil
}

func (c *stagingV1Client) AutoDeleteBuffer(ctx context.Context, in *Buffer, opts ...grpc.CallOption) (*Buffer, error) {
	out := new(Buffer)
	err := grpc.Invoke(ctx, "/staging.StagingV1/AutoDeleteBuffer", in, out, c.cc, opts...)
	if err != nil {
		return nil, err
	}
	return out, nil
}

func (c *stagingV1Client) AutoGetBuffer(ctx context.Context, in *Buffer, opts ...grpc.CallOption) (*Buffer, error) {
	out := new(Buffer)
	err := grpc.Invoke(ctx, "/staging.StagingV1/AutoGetBuffer", in, out, c.cc, opts...)
	if err != nil {
		return nil, err
	}
	return out, nil
}

func (c *stagingV1Client) AutoListBuffer(ctx context.Context, in *api.ListWatchOptions, opts ...grpc.CallOption) (*BufferList, error) {
	out := new(BufferList)
	err := grpc.Invoke(ctx, "/staging.StagingV1/AutoListBuffer", in, out, c.cc, opts...)
	if err != nil {
		return nil, err
	}
	return out, nil
}

func (c *stagingV1Client) AutoUpdateBuffer(ctx context.Context, in *Buffer, opts ...grpc.CallOption) (*Buffer, error) {
	out := new(Buffer)
	err := grpc.Invoke(ctx, "/staging.StagingV1/AutoUpdateBuffer", in, out, c.cc, opts...)
	if err != nil {
		return nil, err
	}
	return out, nil
}

func (c *stagingV1Client) AutoWatchBuffer(ctx context.Context, in *api.ListWatchOptions, opts ...grpc.CallOption) (StagingV1_AutoWatchBufferClient, error) {
	stream, err := grpc.NewClientStream(ctx, &_StagingV1_serviceDesc.Streams[0], c.cc, "/staging.StagingV1/AutoWatchBuffer", opts...)
	if err != nil {
		return nil, err
	}
	x := &stagingV1AutoWatchBufferClient{stream}
	if err := x.ClientStream.SendMsg(in); err != nil {
		return nil, err
	}
	if err := x.ClientStream.CloseSend(); err != nil {
		return nil, err
	}
	return x, nil
}

type StagingV1_AutoWatchBufferClient interface {
	Recv() (*AutoMsgBufferWatchHelper, error)
	grpc.ClientStream
}

type stagingV1AutoWatchBufferClient struct {
	grpc.ClientStream
}

func (x *stagingV1AutoWatchBufferClient) Recv() (*AutoMsgBufferWatchHelper, error) {
	m := new(AutoMsgBufferWatchHelper)
	if err := x.ClientStream.RecvMsg(m); err != nil {
		return nil, err
	}
	return m, nil
}

func (c *stagingV1Client) AutoWatchSvcStagingV1(ctx context.Context, in *api.ListWatchOptions, opts ...grpc.CallOption) (StagingV1_AutoWatchSvcStagingV1Client, error) {
	stream, err := grpc.NewClientStream(ctx, &_StagingV1_serviceDesc.Streams[1], c.cc, "/staging.StagingV1/AutoWatchSvcStagingV1", opts...)
	if err != nil {
		return nil, err
	}
	x := &stagingV1AutoWatchSvcStagingV1Client{stream}
	if err := x.ClientStream.SendMsg(in); err != nil {
		return nil, err
	}
	if err := x.ClientStream.CloseSend(); err != nil {
		return nil, err
	}
	return x, nil
}

type StagingV1_AutoWatchSvcStagingV1Client interface {
	Recv() (*api.WatchEventList, error)
	grpc.ClientStream
}

type stagingV1AutoWatchSvcStagingV1Client struct {
	grpc.ClientStream
}

func (x *stagingV1AutoWatchSvcStagingV1Client) Recv() (*api.WatchEventList, error) {
	m := new(api.WatchEventList)
	if err := x.ClientStream.RecvMsg(m); err != nil {
		return nil, err
	}
	return m, nil
}

func (c *stagingV1Client) Clear(ctx context.Context, in *ClearAction, opts ...grpc.CallOption) (*ClearAction, error) {
	out := new(ClearAction)
	err := grpc.Invoke(ctx, "/staging.StagingV1/Clear", in, out, c.cc, opts...)
	if err != nil {
		return nil, err
	}
	return out, nil
}

func (c *stagingV1Client) Commit(ctx context.Context, in *CommitAction, opts ...grpc.CallOption) (*CommitAction, error) {
	out := new(CommitAction)
	err := grpc.Invoke(ctx, "/staging.StagingV1/Commit", in, out, c.cc, opts...)
	if err != nil {
		return nil, err
	}
	return out, nil
}

// Server API for StagingV1 service

type StagingV1Server interface {
	// Create Buffer object
	AutoAddBuffer(context.Context, *Buffer) (*Buffer, error)
	// Delete Buffer object
	AutoDeleteBuffer(context.Context, *Buffer) (*Buffer, error)
	// Get Buffer object
	AutoGetBuffer(context.Context, *Buffer) (*Buffer, error)
	// List Buffer objects
	AutoListBuffer(context.Context, *api.ListWatchOptions) (*BufferList, error)
	// Update Buffer object
	AutoUpdateBuffer(context.Context, *Buffer) (*Buffer, error)
	// Watch Buffer objects. Supports WebSockets or HTTP long poll
	AutoWatchBuffer(*api.ListWatchOptions, StagingV1_AutoWatchBufferServer) error
	AutoWatchSvcStagingV1(*api.ListWatchOptions, StagingV1_AutoWatchSvcStagingV1Server) error
	Clear(context.Context, *ClearAction) (*ClearAction, error)
	Commit(context.Context, *CommitAction) (*CommitAction, error)
}

func RegisterStagingV1Server(s *grpc.Server, srv StagingV1Server) {
	s.RegisterService(&_StagingV1_serviceDesc, srv)
}

func _StagingV1_AutoAddBuffer_Handler(srv interface{}, ctx context.Context, dec func(interface{}) error, interceptor grpc.UnaryServerInterceptor) (interface{}, error) {
	in := new(Buffer)
	if err := dec(in); err != nil {
		return nil, err
	}
	if interceptor == nil {
		return srv.(StagingV1Server).AutoAddBuffer(ctx, in)
	}
	info := &grpc.UnaryServerInfo{
		Server:     srv,
		FullMethod: "/staging.StagingV1/AutoAddBuffer",
	}
	handler := func(ctx context.Context, req interface{}) (interface{}, error) {
		return srv.(StagingV1Server).AutoAddBuffer(ctx, req.(*Buffer))
	}
	return interceptor(ctx, in, info, handler)
}

func _StagingV1_AutoDeleteBuffer_Handler(srv interface{}, ctx context.Context, dec func(interface{}) error, interceptor grpc.UnaryServerInterceptor) (interface{}, error) {
	in := new(Buffer)
	if err := dec(in); err != nil {
		return nil, err
	}
	if interceptor == nil {
		return srv.(StagingV1Server).AutoDeleteBuffer(ctx, in)
	}
	info := &grpc.UnaryServerInfo{
		Server:     srv,
		FullMethod: "/staging.StagingV1/AutoDeleteBuffer",
	}
	handler := func(ctx context.Context, req interface{}) (interface{}, error) {
		return srv.(StagingV1Server).AutoDeleteBuffer(ctx, req.(*Buffer))
	}
	return interceptor(ctx, in, info, handler)
}

func _StagingV1_AutoGetBuffer_Handler(srv interface{}, ctx context.Context, dec func(interface{}) error, interceptor grpc.UnaryServerInterceptor) (interface{}, error) {
	in := new(Buffer)
	if err := dec(in); err != nil {
		return nil, err
	}
	if interceptor == nil {
		return srv.(StagingV1Server).AutoGetBuffer(ctx, in)
	}
	info := &grpc.UnaryServerInfo{
		Server:     srv,
		FullMethod: "/staging.StagingV1/AutoGetBuffer",
	}
	handler := func(ctx context.Context, req interface{}) (interface{}, error) {
		return srv.(StagingV1Server).AutoGetBuffer(ctx, req.(*Buffer))
	}
	return interceptor(ctx, in, info, handler)
}

func _StagingV1_AutoListBuffer_Handler(srv interface{}, ctx context.Context, dec func(interface{}) error, interceptor grpc.UnaryServerInterceptor) (interface{}, error) {
	in := new(api.ListWatchOptions)
	if err := dec(in); err != nil {
		return nil, err
	}
	if interceptor == nil {
		return srv.(StagingV1Server).AutoListBuffer(ctx, in)
	}
	info := &grpc.UnaryServerInfo{
		Server:     srv,
		FullMethod: "/staging.StagingV1/AutoListBuffer",
	}
	handler := func(ctx context.Context, req interface{}) (interface{}, error) {
		return srv.(StagingV1Server).AutoListBuffer(ctx, req.(*api.ListWatchOptions))
	}
	return interceptor(ctx, in, info, handler)
}

func _StagingV1_AutoUpdateBuffer_Handler(srv interface{}, ctx context.Context, dec func(interface{}) error, interceptor grpc.UnaryServerInterceptor) (interface{}, error) {
	in := new(Buffer)
	if err := dec(in); err != nil {
		return nil, err
	}
	if interceptor == nil {
		return srv.(StagingV1Server).AutoUpdateBuffer(ctx, in)
	}
	info := &grpc.UnaryServerInfo{
		Server:     srv,
		FullMethod: "/staging.StagingV1/AutoUpdateBuffer",
	}
	handler := func(ctx context.Context, req interface{}) (interface{}, error) {
		return srv.(StagingV1Server).AutoUpdateBuffer(ctx, req.(*Buffer))
	}
	return interceptor(ctx, in, info, handler)
}

func _StagingV1_AutoWatchBuffer_Handler(srv interface{}, stream grpc.ServerStream) error {
	m := new(api.ListWatchOptions)
	if err := stream.RecvMsg(m); err != nil {
		return err
	}
	return srv.(StagingV1Server).AutoWatchBuffer(m, &stagingV1AutoWatchBufferServer{stream})
}

type StagingV1_AutoWatchBufferServer interface {
	Send(*AutoMsgBufferWatchHelper) error
	grpc.ServerStream
}

type stagingV1AutoWatchBufferServer struct {
	grpc.ServerStream
}

func (x *stagingV1AutoWatchBufferServer) Send(m *AutoMsgBufferWatchHelper) error {
	return x.ServerStream.SendMsg(m)
}

func _StagingV1_AutoWatchSvcStagingV1_Handler(srv interface{}, stream grpc.ServerStream) error {
	m := new(api.ListWatchOptions)
	if err := stream.RecvMsg(m); err != nil {
		return err
	}
	return srv.(StagingV1Server).AutoWatchSvcStagingV1(m, &stagingV1AutoWatchSvcStagingV1Server{stream})
}

type StagingV1_AutoWatchSvcStagingV1Server interface {
	Send(*api.WatchEventList) error
	grpc.ServerStream
}

type stagingV1AutoWatchSvcStagingV1Server struct {
	grpc.ServerStream
}

func (x *stagingV1AutoWatchSvcStagingV1Server) Send(m *api.WatchEventList) error {
	return x.ServerStream.SendMsg(m)
}

func _StagingV1_Clear_Handler(srv interface{}, ctx context.Context, dec func(interface{}) error, interceptor grpc.UnaryServerInterceptor) (interface{}, error) {
	in := new(ClearAction)
	if err := dec(in); err != nil {
		return nil, err
	}
	if interceptor == nil {
		return srv.(StagingV1Server).Clear(ctx, in)
	}
	info := &grpc.UnaryServerInfo{
		Server:     srv,
		FullMethod: "/staging.StagingV1/Clear",
	}
	handler := func(ctx context.Context, req interface{}) (interface{}, error) {
		return srv.(StagingV1Server).Clear(ctx, req.(*ClearAction))
	}
	return interceptor(ctx, in, info, handler)
}

func _StagingV1_Commit_Handler(srv interface{}, ctx context.Context, dec func(interface{}) error, interceptor grpc.UnaryServerInterceptor) (interface{}, error) {
	in := new(CommitAction)
	if err := dec(in); err != nil {
		return nil, err
	}
	if interceptor == nil {
		return srv.(StagingV1Server).Commit(ctx, in)
	}
	info := &grpc.UnaryServerInfo{
		Server:     srv,
		FullMethod: "/staging.StagingV1/Commit",
	}
	handler := func(ctx context.Context, req interface{}) (interface{}, error) {
		return srv.(StagingV1Server).Commit(ctx, req.(*CommitAction))
	}
	return interceptor(ctx, in, info, handler)
}

var _StagingV1_serviceDesc = grpc.ServiceDesc{
	ServiceName: "staging.StagingV1",
	HandlerType: (*StagingV1Server)(nil),
	Methods: []grpc.MethodDesc{
		{
			MethodName: "AutoAddBuffer",
			Handler:    _StagingV1_AutoAddBuffer_Handler,
		},
		{
			MethodName: "AutoDeleteBuffer",
			Handler:    _StagingV1_AutoDeleteBuffer_Handler,
		},
		{
			MethodName: "AutoGetBuffer",
			Handler:    _StagingV1_AutoGetBuffer_Handler,
		},
		{
			MethodName: "AutoListBuffer",
			Handler:    _StagingV1_AutoListBuffer_Handler,
		},
		{
			MethodName: "AutoUpdateBuffer",
			Handler:    _StagingV1_AutoUpdateBuffer_Handler,
		},
		{
			MethodName: "Clear",
			Handler:    _StagingV1_Clear_Handler,
		},
		{
			MethodName: "Commit",
			Handler:    _StagingV1_Commit_Handler,
		},
	},
	Streams: []grpc.StreamDesc{
		{
			StreamName:    "AutoWatchBuffer",
			Handler:       _StagingV1_AutoWatchBuffer_Handler,
			ServerStreams: true,
		},
		{
			StreamName:    "AutoWatchSvcStagingV1",
			Handler:       _StagingV1_AutoWatchSvcStagingV1_Handler,
			ServerStreams: true,
		},
	},
	Metadata: "svc_staging.proto",
}

func (m *AutoMsgBufferWatchHelper) Marshal() (dAtA []byte, err error) {
	size := m.Size()
	dAtA = make([]byte, size)
	n, err := m.MarshalTo(dAtA)
	if err != nil {
		return nil, err
	}
	return dAtA[:n], nil
}

func (m *AutoMsgBufferWatchHelper) MarshalTo(dAtA []byte) (int, error) {
	var i int
	_ = i
	var l int
	_ = l
	if len(m.Events) > 0 {
		for _, msg := range m.Events {
			dAtA[i] = 0xa
			i++
			i = encodeVarintSvcStaging(dAtA, i, uint64(msg.Size()))
			n, err := msg.MarshalTo(dAtA[i:])
			if err != nil {
				return 0, err
			}
			i += n
		}
	}
	return i, nil
}

func (m *AutoMsgBufferWatchHelper_WatchEvent) Marshal() (dAtA []byte, err error) {
	size := m.Size()
	dAtA = make([]byte, size)
	n, err := m.MarshalTo(dAtA)
	if err != nil {
		return nil, err
	}
	return dAtA[:n], nil
}

func (m *AutoMsgBufferWatchHelper_WatchEvent) MarshalTo(dAtA []byte) (int, error) {
	var i int
	_ = i
	var l int
	_ = l
	if len(m.Type) > 0 {
		dAtA[i] = 0xa
		i++
		i = encodeVarintSvcStaging(dAtA, i, uint64(len(m.Type)))
		i += copy(dAtA[i:], m.Type)
	}
	if m.Object != nil {
		dAtA[i] = 0x12
		i++
		i = encodeVarintSvcStaging(dAtA, i, uint64(m.Object.Size()))
		n1, err := m.Object.MarshalTo(dAtA[i:])
		if err != nil {
			return 0, err
		}
		i += n1
	}
	return i, nil
}

func (m *BufferList) Marshal() (dAtA []byte, err error) {
	size := m.Size()
	dAtA = make([]byte, size)
	n, err := m.MarshalTo(dAtA)
	if err != nil {
		return nil, err
	}
	return dAtA[:n], nil
}

func (m *BufferList) MarshalTo(dAtA []byte) (int, error) {
	var i int
	_ = i
	var l int
	_ = l
	dAtA[i] = 0x12
	i++
	i = encodeVarintSvcStaging(dAtA, i, uint64(m.TypeMeta.Size()))
	n2, err := m.TypeMeta.MarshalTo(dAtA[i:])
	if err != nil {
		return 0, err
	}
	i += n2
	dAtA[i] = 0x1a
	i++
	i = encodeVarintSvcStaging(dAtA, i, uint64(m.ListMeta.Size()))
	n3, err := m.ListMeta.MarshalTo(dAtA[i:])
	if err != nil {
		return 0, err
	}
	i += n3
	if len(m.Items) > 0 {
		for _, msg := range m.Items {
			dAtA[i] = 0x22
			i++
			i = encodeVarintSvcStaging(dAtA, i, uint64(msg.Size()))
			n, err := msg.MarshalTo(dAtA[i:])
			if err != nil {
				return 0, err
			}
			i += n
		}
	}
	return i, nil
}

func encodeVarintSvcStaging(dAtA []byte, offset int, v uint64) int {
	for v >= 1<<7 {
		dAtA[offset] = uint8(v&0x7f | 0x80)
		v >>= 7
		offset++
	}
	dAtA[offset] = uint8(v)
	return offset + 1
}
func (m *AutoMsgBufferWatchHelper) Size() (n int) {
	var l int
	_ = l
	if len(m.Events) > 0 {
		for _, e := range m.Events {
			l = e.Size()
			n += 1 + l + sovSvcStaging(uint64(l))
		}
	}
	return n
}

func (m *AutoMsgBufferWatchHelper_WatchEvent) Size() (n int) {
	var l int
	_ = l
	l = len(m.Type)
	if l > 0 {
		n += 1 + l + sovSvcStaging(uint64(l))
	}
	if m.Object != nil {
		l = m.Object.Size()
		n += 1 + l + sovSvcStaging(uint64(l))
	}
	return n
}

func (m *BufferList) Size() (n int) {
	var l int
	_ = l
	l = m.TypeMeta.Size()
	n += 1 + l + sovSvcStaging(uint64(l))
	l = m.ListMeta.Size()
	n += 1 + l + sovSvcStaging(uint64(l))
	if len(m.Items) > 0 {
		for _, e := range m.Items {
			l = e.Size()
			n += 1 + l + sovSvcStaging(uint64(l))
		}
	}
	return n
}

func sovSvcStaging(x uint64) (n int) {
	for {
		n++
		x >>= 7
		if x == 0 {
			break
		}
	}
	return n
}
func sozSvcStaging(x uint64) (n int) {
	return sovSvcStaging(uint64((x << 1) ^ uint64((int64(x) >> 63))))
}
func (m *AutoMsgBufferWatchHelper) Unmarshal(dAtA []byte) error {
	l := len(dAtA)
	iNdEx := 0
	for iNdEx < l {
		preIndex := iNdEx
		var wire uint64
		for shift := uint(0); ; shift += 7 {
			if shift >= 64 {
				return ErrIntOverflowSvcStaging
			}
			if iNdEx >= l {
				return io.ErrUnexpectedEOF
			}
			b := dAtA[iNdEx]
			iNdEx++
			wire |= (uint64(b) & 0x7F) << shift
			if b < 0x80 {
				break
			}
		}
		fieldNum := int32(wire >> 3)
		wireType := int(wire & 0x7)
		if wireType == 4 {
			return fmt.Errorf("proto: AutoMsgBufferWatchHelper: wiretype end group for non-group")
		}
		if fieldNum <= 0 {
			return fmt.Errorf("proto: AutoMsgBufferWatchHelper: illegal tag %d (wire type %d)", fieldNum, wire)
		}
		switch fieldNum {
		case 1:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field Events", wireType)
			}
			var msglen int
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowSvcStaging
				}
				if iNdEx >= l {
					return io.ErrUnexpectedEOF
				}
				b := dAtA[iNdEx]
				iNdEx++
				msglen |= (int(b) & 0x7F) << shift
				if b < 0x80 {
					break
				}
			}
			if msglen < 0 {
				return ErrInvalidLengthSvcStaging
			}
			postIndex := iNdEx + msglen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			m.Events = append(m.Events, &AutoMsgBufferWatchHelper_WatchEvent{})
			if err := m.Events[len(m.Events)-1].Unmarshal(dAtA[iNdEx:postIndex]); err != nil {
				return err
			}
			iNdEx = postIndex
		default:
			iNdEx = preIndex
			skippy, err := skipSvcStaging(dAtA[iNdEx:])
			if err != nil {
				return err
			}
			if skippy < 0 {
				return ErrInvalidLengthSvcStaging
			}
			if (iNdEx + skippy) > l {
				return io.ErrUnexpectedEOF
			}
			iNdEx += skippy
		}
	}

	if iNdEx > l {
		return io.ErrUnexpectedEOF
	}
	return nil
}
func (m *AutoMsgBufferWatchHelper_WatchEvent) Unmarshal(dAtA []byte) error {
	l := len(dAtA)
	iNdEx := 0
	for iNdEx < l {
		preIndex := iNdEx
		var wire uint64
		for shift := uint(0); ; shift += 7 {
			if shift >= 64 {
				return ErrIntOverflowSvcStaging
			}
			if iNdEx >= l {
				return io.ErrUnexpectedEOF
			}
			b := dAtA[iNdEx]
			iNdEx++
			wire |= (uint64(b) & 0x7F) << shift
			if b < 0x80 {
				break
			}
		}
		fieldNum := int32(wire >> 3)
		wireType := int(wire & 0x7)
		if wireType == 4 {
			return fmt.Errorf("proto: WatchEvent: wiretype end group for non-group")
		}
		if fieldNum <= 0 {
			return fmt.Errorf("proto: WatchEvent: illegal tag %d (wire type %d)", fieldNum, wire)
		}
		switch fieldNum {
		case 1:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field Type", wireType)
			}
			var stringLen uint64
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowSvcStaging
				}
				if iNdEx >= l {
					return io.ErrUnexpectedEOF
				}
				b := dAtA[iNdEx]
				iNdEx++
				stringLen |= (uint64(b) & 0x7F) << shift
				if b < 0x80 {
					break
				}
			}
			intStringLen := int(stringLen)
			if intStringLen < 0 {
				return ErrInvalidLengthSvcStaging
			}
			postIndex := iNdEx + intStringLen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			m.Type = string(dAtA[iNdEx:postIndex])
			iNdEx = postIndex
		case 2:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field Object", wireType)
			}
			var msglen int
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowSvcStaging
				}
				if iNdEx >= l {
					return io.ErrUnexpectedEOF
				}
				b := dAtA[iNdEx]
				iNdEx++
				msglen |= (int(b) & 0x7F) << shift
				if b < 0x80 {
					break
				}
			}
			if msglen < 0 {
				return ErrInvalidLengthSvcStaging
			}
			postIndex := iNdEx + msglen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			if m.Object == nil {
				m.Object = &Buffer{}
			}
			if err := m.Object.Unmarshal(dAtA[iNdEx:postIndex]); err != nil {
				return err
			}
			iNdEx = postIndex
		default:
			iNdEx = preIndex
			skippy, err := skipSvcStaging(dAtA[iNdEx:])
			if err != nil {
				return err
			}
			if skippy < 0 {
				return ErrInvalidLengthSvcStaging
			}
			if (iNdEx + skippy) > l {
				return io.ErrUnexpectedEOF
			}
			iNdEx += skippy
		}
	}

	if iNdEx > l {
		return io.ErrUnexpectedEOF
	}
	return nil
}
func (m *BufferList) Unmarshal(dAtA []byte) error {
	l := len(dAtA)
	iNdEx := 0
	for iNdEx < l {
		preIndex := iNdEx
		var wire uint64
		for shift := uint(0); ; shift += 7 {
			if shift >= 64 {
				return ErrIntOverflowSvcStaging
			}
			if iNdEx >= l {
				return io.ErrUnexpectedEOF
			}
			b := dAtA[iNdEx]
			iNdEx++
			wire |= (uint64(b) & 0x7F) << shift
			if b < 0x80 {
				break
			}
		}
		fieldNum := int32(wire >> 3)
		wireType := int(wire & 0x7)
		if wireType == 4 {
			return fmt.Errorf("proto: BufferList: wiretype end group for non-group")
		}
		if fieldNum <= 0 {
			return fmt.Errorf("proto: BufferList: illegal tag %d (wire type %d)", fieldNum, wire)
		}
		switch fieldNum {
		case 2:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field TypeMeta", wireType)
			}
			var msglen int
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowSvcStaging
				}
				if iNdEx >= l {
					return io.ErrUnexpectedEOF
				}
				b := dAtA[iNdEx]
				iNdEx++
				msglen |= (int(b) & 0x7F) << shift
				if b < 0x80 {
					break
				}
			}
			if msglen < 0 {
				return ErrInvalidLengthSvcStaging
			}
			postIndex := iNdEx + msglen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			if err := m.TypeMeta.Unmarshal(dAtA[iNdEx:postIndex]); err != nil {
				return err
			}
			iNdEx = postIndex
		case 3:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field ListMeta", wireType)
			}
			var msglen int
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowSvcStaging
				}
				if iNdEx >= l {
					return io.ErrUnexpectedEOF
				}
				b := dAtA[iNdEx]
				iNdEx++
				msglen |= (int(b) & 0x7F) << shift
				if b < 0x80 {
					break
				}
			}
			if msglen < 0 {
				return ErrInvalidLengthSvcStaging
			}
			postIndex := iNdEx + msglen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			if err := m.ListMeta.Unmarshal(dAtA[iNdEx:postIndex]); err != nil {
				return err
			}
			iNdEx = postIndex
		case 4:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field Items", wireType)
			}
			var msglen int
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowSvcStaging
				}
				if iNdEx >= l {
					return io.ErrUnexpectedEOF
				}
				b := dAtA[iNdEx]
				iNdEx++
				msglen |= (int(b) & 0x7F) << shift
				if b < 0x80 {
					break
				}
			}
			if msglen < 0 {
				return ErrInvalidLengthSvcStaging
			}
			postIndex := iNdEx + msglen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			m.Items = append(m.Items, &Buffer{})
			if err := m.Items[len(m.Items)-1].Unmarshal(dAtA[iNdEx:postIndex]); err != nil {
				return err
			}
			iNdEx = postIndex
		default:
			iNdEx = preIndex
			skippy, err := skipSvcStaging(dAtA[iNdEx:])
			if err != nil {
				return err
			}
			if skippy < 0 {
				return ErrInvalidLengthSvcStaging
			}
			if (iNdEx + skippy) > l {
				return io.ErrUnexpectedEOF
			}
			iNdEx += skippy
		}
	}

	if iNdEx > l {
		return io.ErrUnexpectedEOF
	}
	return nil
}
func skipSvcStaging(dAtA []byte) (n int, err error) {
	l := len(dAtA)
	iNdEx := 0
	for iNdEx < l {
		var wire uint64
		for shift := uint(0); ; shift += 7 {
			if shift >= 64 {
				return 0, ErrIntOverflowSvcStaging
			}
			if iNdEx >= l {
				return 0, io.ErrUnexpectedEOF
			}
			b := dAtA[iNdEx]
			iNdEx++
			wire |= (uint64(b) & 0x7F) << shift
			if b < 0x80 {
				break
			}
		}
		wireType := int(wire & 0x7)
		switch wireType {
		case 0:
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return 0, ErrIntOverflowSvcStaging
				}
				if iNdEx >= l {
					return 0, io.ErrUnexpectedEOF
				}
				iNdEx++
				if dAtA[iNdEx-1] < 0x80 {
					break
				}
			}
			return iNdEx, nil
		case 1:
			iNdEx += 8
			return iNdEx, nil
		case 2:
			var length int
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return 0, ErrIntOverflowSvcStaging
				}
				if iNdEx >= l {
					return 0, io.ErrUnexpectedEOF
				}
				b := dAtA[iNdEx]
				iNdEx++
				length |= (int(b) & 0x7F) << shift
				if b < 0x80 {
					break
				}
			}
			iNdEx += length
			if length < 0 {
				return 0, ErrInvalidLengthSvcStaging
			}
			return iNdEx, nil
		case 3:
			for {
				var innerWire uint64
				var start int = iNdEx
				for shift := uint(0); ; shift += 7 {
					if shift >= 64 {
						return 0, ErrIntOverflowSvcStaging
					}
					if iNdEx >= l {
						return 0, io.ErrUnexpectedEOF
					}
					b := dAtA[iNdEx]
					iNdEx++
					innerWire |= (uint64(b) & 0x7F) << shift
					if b < 0x80 {
						break
					}
				}
				innerWireType := int(innerWire & 0x7)
				if innerWireType == 4 {
					break
				}
				next, err := skipSvcStaging(dAtA[start:])
				if err != nil {
					return 0, err
				}
				iNdEx = start + next
			}
			return iNdEx, nil
		case 4:
			return iNdEx, nil
		case 5:
			iNdEx += 4
			return iNdEx, nil
		default:
			return 0, fmt.Errorf("proto: illegal wireType %d", wireType)
		}
	}
	panic("unreachable")
}

var (
	ErrInvalidLengthSvcStaging = fmt.Errorf("proto: negative length found during unmarshaling")
	ErrIntOverflowSvcStaging   = fmt.Errorf("proto: integer overflow")
)

func init() { proto.RegisterFile("svc_staging.proto", fileDescriptorSvcStaging) }

var fileDescriptorSvcStaging = []byte{
	// 888 bytes of a gzipped FileDescriptorProto
	0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xff, 0x94, 0x95, 0xcf, 0x6f, 0x1b, 0x45,
	0x14, 0xc7, 0x3d, 0x89, 0xbd, 0x6d, 0xc6, 0x8d, 0x9d, 0x4c, 0x1b, 0xd5, 0xbb, 0xaa, 0xbc, 0xee,
	0x22, 0xa4, 0xd4, 0x72, 0xbd, 0xa9, 0x11, 0x1c, 0x7c, 0xcb, 0x96, 0x42, 0x41, 0x14, 0x47, 0x6e,
	0xa0, 0x28, 0x17, 0x58, 0xaf, 0x27, 0x9b, 0x45, 0xfb, 0x4b, 0xde, 0xb1, 0xab, 0x08, 0xe5, 0x82,
	0x37, 0x17, 0x4e, 0x08, 0x6e, 0x48, 0x5c, 0xb8, 0x20, 0x21, 0x0e, 0xc8, 0x27, 0x94, 0x13, 0xc7,
	0x88, 0x53, 0x24, 0x0e, 0x48, 0x39, 0x58, 0x28, 0xca, 0xc9, 0x7f, 0x05, 0x9a, 0x99, 0x5d, 0xff,
	0x88, 0xed, 0xc4, 0xbd, 0x64, 0x67, 0x5e, 0xde, 0xf7, 0xfb, 0x3e, 0x33, 0x6f, 0x66, 0x0c, 0xd7,
	0x83, 0x8e, 0xf1, 0x65, 0x40, 0x74, 0xd3, 0x72, 0xcd, 0xb2, 0xdf, 0xf2, 0x88, 0x87, 0x6e, 0x45,
	0x53, 0xe9, 0x81, 0xe9, 0x79, 0xa6, 0x8d, 0x55, 0xdd, 0xb7, 0x54, 0xdd, 0x75, 0x3d, 0xa2, 0x13,
	0xcb, 0x73, 0x03, 0x9e, 0x26, 0x3d, 0x33, 0x2d, 0x72, 0xd0, 0x6e, 0x94, 0x0d, 0xcf, 0x51, 0x7d,
	0xec, 0x06, 0xba, 0xdb, 0xf4, 0xd4, 0xe0, 0xb5, 0xda, 0xc1, 0xae, 0x65, 0x60, 0xb5, 0x4d, 0x2c,
	0x3b, 0xa0, 0x52, 0x13, 0xbb, 0xe3, 0x6a, 0xd5, 0x72, 0x0d, 0xbb, 0xdd, 0xc4, 0xb1, 0xcd, 0xe3,
	0x31, 0x1b, 0xd3, 0x33, 0x3d, 0x95, 0x85, 0x1b, 0xed, 0x7d, 0x36, 0x63, 0x13, 0x36, 0x8a, 0xd2,
	0x57, 0x27, 0x58, 0xa5, 0xb7, 0xe7, 0x40, 0x50, 0x64, 0x07, 0x13, 0x9d, 0xa7, 0x29, 0x97, 0x00,
	0xe6, 0xb6, 0xdb, 0xc4, 0x7b, 0x11, 0x98, 0x5a, 0x7b, 0x7f, 0x1f, 0xb7, 0x5e, 0xe9, 0xc4, 0x38,
	0x78, 0x8e, 0x6d, 0x1f, 0xb7, 0xd0, 0x0e, 0x14, 0x9e, 0x75, 0xb0, 0x4b, 0x82, 0x1c, 0x28, 0x2c,
	0x6f, 0xa6, 0x2b, 0xa5, 0x72, 0x5c, 0x63, 0x9e, 0xa4, 0xcc, 0xc6, 0x4c, 0xa4, 0xc1, 0x41, 0x5f,
	0x16, 0x30, 0xd3, 0xd7, 0xa3, 0xaf, 0x84, 0x21, 0x1c, 0x65, 0xa0, 0x02, 0x4c, 0xee, 0x1e, 0xfa,
	0x38, 0x07, 0x0a, 0x60, 0x73, 0x45, 0x43, 0x83, 0xbe, 0x9c, 0x21, 0x87, 0x3e, 0x2e, 0x79, 0x8e,
	0x45, 0xb0, 0xe3, 0x93, 0x43, 0xf4, 0x2e, 0x14, 0x6a, 0x8d, 0xaf, 0xb1, 0x41, 0x72, 0x4b, 0x05,
	0xb0, 0x99, 0xae, 0x64, 0x87, 0x04, 0xbc, 0xb4, 0x76, 0x6f, 0xd0, 0x97, 0xd7, 0x3c, 0x96, 0x32,
	0x92, 0x55, 0xb3, 0xe7, 0xc7, 0x62, 0xfa, 0x35, 0x2d, 0x74, 0xc0, 0xb0, 0x94, 0x7f, 0x01, 0x84,
	0x5c, 0xf1, 0x89, 0x15, 0x10, 0xf4, 0x1e, 0x04, 0xbb, 0x91, 0xe3, 0x6a, 0x59, 0xf7, 0xad, 0x32,
	0xc5, 0x78, 0x81, 0x89, 0xae, 0xdd, 0x3d, 0xed, 0xcb, 0x89, 0xb3, 0xbe, 0x0c, 0x06, 0x7d, 0xf9,
	0x56, 0xc9, 0x72, 0x6d, 0xcb, 0xc5, 0xf5, 0x78, 0x80, 0x6a, 0xf0, 0x36, 0xd5, 0xd3, 0xcc, 0xdc,
	0xf2, 0x98, 0x3c, 0x0e, 0x6a, 0x0f, 0xc6, 0xe4, 0x6b, 0xb6, 0x15, 0x90, 0xc7, 0x74, 0xaf, 0x63,
	0x9f, 0xa9, 0x08, 0xda, 0x82, 0xa9, 0x8f, 0x08, 0x76, 0x82, 0x5c, 0x92, 0x6d, 0xf0, 0xd4, 0xf2,
	0x56, 0x06, 0x7d, 0x39, 0x45, 0x57, 0x15, 0xd4, 0xf9, 0xa7, 0x9a, 0x39, 0x3f, 0x16, 0x21, 0xf5,
	0xe1, 0x2b, 0xab, 0xfc, 0x01, 0xe1, 0xca, 0x4b, 0x2e, 0xfa, 0xfc, 0x09, 0x3a, 0x82, 0xab, 0xb4,
	0x35, 0xdb, 0xcd, 0x26, 0x37, 0x40, 0x57, 0x1d, 0xa5, 0xab, 0x01, 0xe5, 0xe3, 0x5e, 0x28, 0x0a,
	0x46, 0x0b, 0xeb, 0x04, 0xff, 0x19, 0x8a, 0xe0, 0xaf, 0x50, 0x04, 0xdf, 0xfe, 0x73, 0xf9, 0xe3,
	0x52, 0x05, 0x26, 0xaa, 0xa0, 0xb8, 0x97, 0x65, 0x1f, 0xe5, 0xb6, 0xda, 0x60, 0xe9, 0x81, 0x22,
	0xa9, 0x04, 0xbb, 0xba, 0x4b, 0xd4, 0x6f, 0x6a, 0xe5, 0x5d, 0x36, 0x3a, 0x8a, 0xff, 0x87, 0xbe,
	0x07, 0x70, 0x8d, 0xd6, 0x7f, 0x1f, 0xdb, 0x98, 0xe0, 0x85, 0x11, 0xf6, 0x28, 0x42, 0x93, 0x69,
	0x26, 0x10, 0x34, 0x98, 0xa8, 0x26, 0xf6, 0xee, 0xd3, 0xbf, 0xc5, 0xf5, 0xb8, 0x08, 0xad, 0xfb,
	0xa9, 0xee, 0xe0, 0xa3, 0xe2, 0x5b, 0xf3, 0x49, 0x86, 0x49, 0xe8, 0x3b, 0xc0, 0xb7, 0xe4, 0x43,
	0x4c, 0x16, 0xe6, 0x79, 0xd5, 0x0b, 0xc5, 0x65, 0x13, 0x93, 0x79, 0x30, 0x68, 0x1a, 0x06, 0x2d,
	0x04, 0x13, 0x02, 0x98, 0xa1, 0x30, 0xf4, 0xbc, 0x44, 0x34, 0x1b, 0xc3, 0x03, 0xc4, 0x2e, 0x46,
	0xcd, 0x67, 0x6f, 0x81, 0x74, 0xf7, 0x0a, 0x13, 0x4d, 0x50, 0x3e, 0xe8, 0x85, 0x62, 0x92, 0xf6,
	0x7e, 0x02, 0x6c, 0x8b, 0x81, 0x65, 0x18, 0xd8, 0xb0, 0x4d, 0xe8, 0xba, 0x36, 0x3d, 0xe7, 0x5d,
	0xfa, 0xcc, 0x6f, 0xea, 0x6f, 0xd0, 0x25, 0x44, 0xbb, 0xd4, 0x66, 0x9a, 0xa8, 0x7e, 0x02, 0x7d,
	0x05, 0xb3, 0xd4, 0x89, 0xa1, 0x5f, 0xbf, 0xa0, 0x87, 0x37, 0xbe, 0x1d, 0xca, 0x7a, 0x2f, 0x14,
	0x53, 0xec, 0xd6, 0xc6, 0xfe, 0x5b, 0x00, 0x7d, 0x01, 0x37, 0x86, 0x15, 0x5e, 0x76, 0x8c, 0xd1,
	0x51, 0x9f, 0xbb, 0x71, 0x34, 0x3c, 0x7a, 0x64, 0xd8, 0xc6, 0xcd, 0x74, 0xfe, 0x15, 0xc0, 0xd4,
	0x53, 0x1b, 0xeb, 0x2d, 0x74, 0x6f, 0xc8, 0xc6, 0xe6, 0xdb, 0x06, 0xb5, 0x92, 0x66, 0x46, 0x95,
	0xd6, 0xf4, 0x75, 0xf9, 0x3b, 0x14, 0x05, 0xbe, 0x1a, 0xd6, 0x8f, 0x1d, 0x7e, 0x71, 0xf2, 0xfc,
	0xe2, 0xdc, 0x9f, 0x3a, 0x05, 0xaa, 0x41, 0xed, 0x94, 0x47, 0x0b, 0x1c, 0x18, 0x9e, 0x8a, 0x7e,
	0x07, 0x50, 0x78, 0xea, 0x39, 0x8e, 0x45, 0xd0, 0xc6, 0x08, 0x8a, 0x05, 0x22, 0xd6, 0xd9, 0x61,
	0x85, 0xdc, 0x08, 0x5b, 0xe7, 0xb0, 0x32, 0x87, 0xcd, 0xcd, 0x20, 0x60, 0x7e, 0x4a, 0x71, 0x21,
	0x5a, 0x96, 0x2b, 0xfd, 0x0c, 0x7e, 0xe8, 0x8a, 0x4b, 0x9d, 0x27, 0x3f, 0x75, 0xc5, 0xf8, 0x17,
	0xf3, 0x97, 0x6e, 0x5c, 0xf5, 0xb7, 0xae, 0xf8, 0x10, 0x46, 0x63, 0x94, 0xf4, 0xbd, 0x80, 0x20,
	0x76, 0xa4, 0x11, 0xbd, 0x70, 0x28, 0x7a, 0x05, 0x4e, 0xba, 0x62, 0x09, 0x45, 0x49, 0x92, 0x10,
	0x21, 0xdc, 0x19, 0x5f, 0x5a, 0x71, 0x62, 0x76, 0xd2, 0x15, 0x1f, 0x0d, 0xf3, 0x53, 0x7c, 0x7f,
	0xd3, 0x63, 0x5d, 0x2b, 0x8e, 0x4f, 0xb4, 0x3b, 0xa7, 0x17, 0x79, 0x70, 0x76, 0x91, 0x07, 0xff,
	0x5d, 0xe4, 0xc1, 0x0e, 0x68, 0x08, 0xec, 0xa7, 0xf0, 0x9d, 0xff, 0x03, 0x00, 0x00, 0xff, 0xff,
	0xf0, 0xda, 0x53, 0x2f, 0xf2, 0x07, 0x00, 0x00,
}
