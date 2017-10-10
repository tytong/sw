// Code generated by MockGen. DO NOT EDIT.
// Source: nwsec.pb.go

// Package halproto is a generated GoMock package.
package halproto

import (
	gomock "github.com/golang/mock/gomock"
	context "golang.org/x/net/context"
	grpc "google.golang.org/grpc"
	reflect "reflect"
)

// MockisSecurityProfileKeyHandle_KeyOrHandle is a mock of isSecurityProfileKeyHandle_KeyOrHandle interface
type MockisSecurityProfileKeyHandle_KeyOrHandle struct {
	ctrl     *gomock.Controller
	recorder *MockisSecurityProfileKeyHandle_KeyOrHandleMockRecorder
}

// MockisSecurityProfileKeyHandle_KeyOrHandleMockRecorder is the mock recorder for MockisSecurityProfileKeyHandle_KeyOrHandle
type MockisSecurityProfileKeyHandle_KeyOrHandleMockRecorder struct {
	mock *MockisSecurityProfileKeyHandle_KeyOrHandle
}

// NewMockisSecurityProfileKeyHandle_KeyOrHandle creates a new mock instance
func NewMockisSecurityProfileKeyHandle_KeyOrHandle(ctrl *gomock.Controller) *MockisSecurityProfileKeyHandle_KeyOrHandle {
	mock := &MockisSecurityProfileKeyHandle_KeyOrHandle{ctrl: ctrl}
	mock.recorder = &MockisSecurityProfileKeyHandle_KeyOrHandleMockRecorder{mock}
	return mock
}

// EXPECT returns an object that allows the caller to indicate expected use
func (m *MockisSecurityProfileKeyHandle_KeyOrHandle) EXPECT() *MockisSecurityProfileKeyHandle_KeyOrHandleMockRecorder {
	return m.recorder
}

// isSecurityProfileKeyHandle_KeyOrHandle mocks base method
func (m *MockisSecurityProfileKeyHandle_KeyOrHandle) isSecurityProfileKeyHandle_KeyOrHandle() {
	m.ctrl.Call(m, "isSecurityProfileKeyHandle_KeyOrHandle")
}

// isSecurityProfileKeyHandle_KeyOrHandle indicates an expected call of isSecurityProfileKeyHandle_KeyOrHandle
func (mr *MockisSecurityProfileKeyHandle_KeyOrHandleMockRecorder) isSecurityProfileKeyHandle_KeyOrHandle() *gomock.Call {
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "isSecurityProfileKeyHandle_KeyOrHandle", reflect.TypeOf((*MockisSecurityProfileKeyHandle_KeyOrHandle)(nil).isSecurityProfileKeyHandle_KeyOrHandle))
}

// MarshalTo mocks base method
func (m *MockisSecurityProfileKeyHandle_KeyOrHandle) MarshalTo(arg0 []byte) (int, error) {
	ret := m.ctrl.Call(m, "MarshalTo", arg0)
	ret0, _ := ret[0].(int)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// MarshalTo indicates an expected call of MarshalTo
func (mr *MockisSecurityProfileKeyHandle_KeyOrHandleMockRecorder) MarshalTo(arg0 interface{}) *gomock.Call {
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "MarshalTo", reflect.TypeOf((*MockisSecurityProfileKeyHandle_KeyOrHandle)(nil).MarshalTo), arg0)
}

// Size mocks base method
func (m *MockisSecurityProfileKeyHandle_KeyOrHandle) Size() int {
	ret := m.ctrl.Call(m, "Size")
	ret0, _ := ret[0].(int)
	return ret0
}

// Size indicates an expected call of Size
func (mr *MockisSecurityProfileKeyHandle_KeyOrHandleMockRecorder) Size() *gomock.Call {
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "Size", reflect.TypeOf((*MockisSecurityProfileKeyHandle_KeyOrHandle)(nil).Size))
}

// MockisService_L4Info is a mock of isService_L4Info interface
type MockisService_L4Info struct {
	ctrl     *gomock.Controller
	recorder *MockisService_L4InfoMockRecorder
}

// MockisService_L4InfoMockRecorder is the mock recorder for MockisService_L4Info
type MockisService_L4InfoMockRecorder struct {
	mock *MockisService_L4Info
}

// NewMockisService_L4Info creates a new mock instance
func NewMockisService_L4Info(ctrl *gomock.Controller) *MockisService_L4Info {
	mock := &MockisService_L4Info{ctrl: ctrl}
	mock.recorder = &MockisService_L4InfoMockRecorder{mock}
	return mock
}

// EXPECT returns an object that allows the caller to indicate expected use
func (m *MockisService_L4Info) EXPECT() *MockisService_L4InfoMockRecorder {
	return m.recorder
}

// isService_L4Info mocks base method
func (m *MockisService_L4Info) isService_L4Info() {
	m.ctrl.Call(m, "isService_L4Info")
}

// isService_L4Info indicates an expected call of isService_L4Info
func (mr *MockisService_L4InfoMockRecorder) isService_L4Info() *gomock.Call {
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "isService_L4Info", reflect.TypeOf((*MockisService_L4Info)(nil).isService_L4Info))
}

// MarshalTo mocks base method
func (m *MockisService_L4Info) MarshalTo(arg0 []byte) (int, error) {
	ret := m.ctrl.Call(m, "MarshalTo", arg0)
	ret0, _ := ret[0].(int)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// MarshalTo indicates an expected call of MarshalTo
func (mr *MockisService_L4InfoMockRecorder) MarshalTo(arg0 interface{}) *gomock.Call {
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "MarshalTo", reflect.TypeOf((*MockisService_L4Info)(nil).MarshalTo), arg0)
}

// Size mocks base method
func (m *MockisService_L4Info) Size() int {
	ret := m.ctrl.Call(m, "Size")
	ret0, _ := ret[0].(int)
	return ret0
}

// Size indicates an expected call of Size
func (mr *MockisService_L4InfoMockRecorder) Size() *gomock.Call {
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "Size", reflect.TypeOf((*MockisService_L4Info)(nil).Size))
}

// MockisDoSService_L4Info is a mock of isDoSService_L4Info interface
type MockisDoSService_L4Info struct {
	ctrl     *gomock.Controller
	recorder *MockisDoSService_L4InfoMockRecorder
}

// MockisDoSService_L4InfoMockRecorder is the mock recorder for MockisDoSService_L4Info
type MockisDoSService_L4InfoMockRecorder struct {
	mock *MockisDoSService_L4Info
}

// NewMockisDoSService_L4Info creates a new mock instance
func NewMockisDoSService_L4Info(ctrl *gomock.Controller) *MockisDoSService_L4Info {
	mock := &MockisDoSService_L4Info{ctrl: ctrl}
	mock.recorder = &MockisDoSService_L4InfoMockRecorder{mock}
	return mock
}

// EXPECT returns an object that allows the caller to indicate expected use
func (m *MockisDoSService_L4Info) EXPECT() *MockisDoSService_L4InfoMockRecorder {
	return m.recorder
}

// isDoSService_L4Info mocks base method
func (m *MockisDoSService_L4Info) isDoSService_L4Info() {
	m.ctrl.Call(m, "isDoSService_L4Info")
}

// isDoSService_L4Info indicates an expected call of isDoSService_L4Info
func (mr *MockisDoSService_L4InfoMockRecorder) isDoSService_L4Info() *gomock.Call {
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "isDoSService_L4Info", reflect.TypeOf((*MockisDoSService_L4Info)(nil).isDoSService_L4Info))
}

// MarshalTo mocks base method
func (m *MockisDoSService_L4Info) MarshalTo(arg0 []byte) (int, error) {
	ret := m.ctrl.Call(m, "MarshalTo", arg0)
	ret0, _ := ret[0].(int)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// MarshalTo indicates an expected call of MarshalTo
func (mr *MockisDoSService_L4InfoMockRecorder) MarshalTo(arg0 interface{}) *gomock.Call {
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "MarshalTo", reflect.TypeOf((*MockisDoSService_L4Info)(nil).MarshalTo), arg0)
}

// Size mocks base method
func (m *MockisDoSService_L4Info) Size() int {
	ret := m.ctrl.Call(m, "Size")
	ret0, _ := ret[0].(int)
	return ret0
}

// Size indicates an expected call of Size
func (mr *MockisDoSService_L4InfoMockRecorder) Size() *gomock.Call {
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "Size", reflect.TypeOf((*MockisDoSService_L4Info)(nil).Size))
}

// MockisSecurityGroupKeyHandle_KeyOrHandle is a mock of isSecurityGroupKeyHandle_KeyOrHandle interface
type MockisSecurityGroupKeyHandle_KeyOrHandle struct {
	ctrl     *gomock.Controller
	recorder *MockisSecurityGroupKeyHandle_KeyOrHandleMockRecorder
}

// MockisSecurityGroupKeyHandle_KeyOrHandleMockRecorder is the mock recorder for MockisSecurityGroupKeyHandle_KeyOrHandle
type MockisSecurityGroupKeyHandle_KeyOrHandleMockRecorder struct {
	mock *MockisSecurityGroupKeyHandle_KeyOrHandle
}

// NewMockisSecurityGroupKeyHandle_KeyOrHandle creates a new mock instance
func NewMockisSecurityGroupKeyHandle_KeyOrHandle(ctrl *gomock.Controller) *MockisSecurityGroupKeyHandle_KeyOrHandle {
	mock := &MockisSecurityGroupKeyHandle_KeyOrHandle{ctrl: ctrl}
	mock.recorder = &MockisSecurityGroupKeyHandle_KeyOrHandleMockRecorder{mock}
	return mock
}

// EXPECT returns an object that allows the caller to indicate expected use
func (m *MockisSecurityGroupKeyHandle_KeyOrHandle) EXPECT() *MockisSecurityGroupKeyHandle_KeyOrHandleMockRecorder {
	return m.recorder
}

// isSecurityGroupKeyHandle_KeyOrHandle mocks base method
func (m *MockisSecurityGroupKeyHandle_KeyOrHandle) isSecurityGroupKeyHandle_KeyOrHandle() {
	m.ctrl.Call(m, "isSecurityGroupKeyHandle_KeyOrHandle")
}

// isSecurityGroupKeyHandle_KeyOrHandle indicates an expected call of isSecurityGroupKeyHandle_KeyOrHandle
func (mr *MockisSecurityGroupKeyHandle_KeyOrHandleMockRecorder) isSecurityGroupKeyHandle_KeyOrHandle() *gomock.Call {
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "isSecurityGroupKeyHandle_KeyOrHandle", reflect.TypeOf((*MockisSecurityGroupKeyHandle_KeyOrHandle)(nil).isSecurityGroupKeyHandle_KeyOrHandle))
}

// MarshalTo mocks base method
func (m *MockisSecurityGroupKeyHandle_KeyOrHandle) MarshalTo(arg0 []byte) (int, error) {
	ret := m.ctrl.Call(m, "MarshalTo", arg0)
	ret0, _ := ret[0].(int)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// MarshalTo indicates an expected call of MarshalTo
func (mr *MockisSecurityGroupKeyHandle_KeyOrHandleMockRecorder) MarshalTo(arg0 interface{}) *gomock.Call {
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "MarshalTo", reflect.TypeOf((*MockisSecurityGroupKeyHandle_KeyOrHandle)(nil).MarshalTo), arg0)
}

// Size mocks base method
func (m *MockisSecurityGroupKeyHandle_KeyOrHandle) Size() int {
	ret := m.ctrl.Call(m, "Size")
	ret0, _ := ret[0].(int)
	return ret0
}

// Size indicates an expected call of Size
func (mr *MockisSecurityGroupKeyHandle_KeyOrHandleMockRecorder) Size() *gomock.Call {
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "Size", reflect.TypeOf((*MockisSecurityGroupKeyHandle_KeyOrHandle)(nil).Size))
}

// MockNwSecurityClient is a mock of NwSecurityClient interface
type MockNwSecurityClient struct {
	ctrl     *gomock.Controller
	recorder *MockNwSecurityClientMockRecorder
}

// MockNwSecurityClientMockRecorder is the mock recorder for MockNwSecurityClient
type MockNwSecurityClientMockRecorder struct {
	mock *MockNwSecurityClient
}

// NewMockNwSecurityClient creates a new mock instance
func NewMockNwSecurityClient(ctrl *gomock.Controller) *MockNwSecurityClient {
	mock := &MockNwSecurityClient{ctrl: ctrl}
	mock.recorder = &MockNwSecurityClientMockRecorder{mock}
	return mock
}

// EXPECT returns an object that allows the caller to indicate expected use
func (m *MockNwSecurityClient) EXPECT() *MockNwSecurityClientMockRecorder {
	return m.recorder
}

// SecurityProfileCreate mocks base method
func (m *MockNwSecurityClient) SecurityProfileCreate(ctx context.Context, in *SecurityProfileRequestMsg, opts ...grpc.CallOption) (*SecurityProfileResponseMsg, error) {
	varargs := []interface{}{ctx, in}
	for _, a := range opts {
		varargs = append(varargs, a)
	}
	ret := m.ctrl.Call(m, "SecurityProfileCreate", varargs...)
	ret0, _ := ret[0].(*SecurityProfileResponseMsg)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// SecurityProfileCreate indicates an expected call of SecurityProfileCreate
func (mr *MockNwSecurityClientMockRecorder) SecurityProfileCreate(ctx, in interface{}, opts ...interface{}) *gomock.Call {
	varargs := append([]interface{}{ctx, in}, opts...)
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "SecurityProfileCreate", reflect.TypeOf((*MockNwSecurityClient)(nil).SecurityProfileCreate), varargs...)
}

// SecurityProfileUpdate mocks base method
func (m *MockNwSecurityClient) SecurityProfileUpdate(ctx context.Context, in *SecurityProfileRequestMsg, opts ...grpc.CallOption) (*SecurityProfileResponseMsg, error) {
	varargs := []interface{}{ctx, in}
	for _, a := range opts {
		varargs = append(varargs, a)
	}
	ret := m.ctrl.Call(m, "SecurityProfileUpdate", varargs...)
	ret0, _ := ret[0].(*SecurityProfileResponseMsg)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// SecurityProfileUpdate indicates an expected call of SecurityProfileUpdate
func (mr *MockNwSecurityClientMockRecorder) SecurityProfileUpdate(ctx, in interface{}, opts ...interface{}) *gomock.Call {
	varargs := append([]interface{}{ctx, in}, opts...)
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "SecurityProfileUpdate", reflect.TypeOf((*MockNwSecurityClient)(nil).SecurityProfileUpdate), varargs...)
}

// SecurityProfileDelete mocks base method
func (m *MockNwSecurityClient) SecurityProfileDelete(ctx context.Context, in *SecurityProfileDeleteRequestMsg, opts ...grpc.CallOption) (*SecurityProfileDeleteResponseMsg, error) {
	varargs := []interface{}{ctx, in}
	for _, a := range opts {
		varargs = append(varargs, a)
	}
	ret := m.ctrl.Call(m, "SecurityProfileDelete", varargs...)
	ret0, _ := ret[0].(*SecurityProfileDeleteResponseMsg)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// SecurityProfileDelete indicates an expected call of SecurityProfileDelete
func (mr *MockNwSecurityClientMockRecorder) SecurityProfileDelete(ctx, in interface{}, opts ...interface{}) *gomock.Call {
	varargs := append([]interface{}{ctx, in}, opts...)
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "SecurityProfileDelete", reflect.TypeOf((*MockNwSecurityClient)(nil).SecurityProfileDelete), varargs...)
}

// SecurityProfileGet mocks base method
func (m *MockNwSecurityClient) SecurityProfileGet(ctx context.Context, in *SecurityProfileGetRequestMsg, opts ...grpc.CallOption) (*SecurityProfileGetResponseMsg, error) {
	varargs := []interface{}{ctx, in}
	for _, a := range opts {
		varargs = append(varargs, a)
	}
	ret := m.ctrl.Call(m, "SecurityProfileGet", varargs...)
	ret0, _ := ret[0].(*SecurityProfileGetResponseMsg)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// SecurityProfileGet indicates an expected call of SecurityProfileGet
func (mr *MockNwSecurityClientMockRecorder) SecurityProfileGet(ctx, in interface{}, opts ...interface{}) *gomock.Call {
	varargs := append([]interface{}{ctx, in}, opts...)
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "SecurityProfileGet", reflect.TypeOf((*MockNwSecurityClient)(nil).SecurityProfileGet), varargs...)
}

// SecurityGroupCreate mocks base method
func (m *MockNwSecurityClient) SecurityGroupCreate(ctx context.Context, in *SecurityGroupRequestMsg, opts ...grpc.CallOption) (*SecurityGroupResponseMsg, error) {
	varargs := []interface{}{ctx, in}
	for _, a := range opts {
		varargs = append(varargs, a)
	}
	ret := m.ctrl.Call(m, "SecurityGroupCreate", varargs...)
	ret0, _ := ret[0].(*SecurityGroupResponseMsg)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// SecurityGroupCreate indicates an expected call of SecurityGroupCreate
func (mr *MockNwSecurityClientMockRecorder) SecurityGroupCreate(ctx, in interface{}, opts ...interface{}) *gomock.Call {
	varargs := append([]interface{}{ctx, in}, opts...)
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "SecurityGroupCreate", reflect.TypeOf((*MockNwSecurityClient)(nil).SecurityGroupCreate), varargs...)
}

// SecurityGroupUpdate mocks base method
func (m *MockNwSecurityClient) SecurityGroupUpdate(ctx context.Context, in *SecurityGroupRequestMsg, opts ...grpc.CallOption) (*SecurityGroupResponseMsg, error) {
	varargs := []interface{}{ctx, in}
	for _, a := range opts {
		varargs = append(varargs, a)
	}
	ret := m.ctrl.Call(m, "SecurityGroupUpdate", varargs...)
	ret0, _ := ret[0].(*SecurityGroupResponseMsg)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// SecurityGroupUpdate indicates an expected call of SecurityGroupUpdate
func (mr *MockNwSecurityClientMockRecorder) SecurityGroupUpdate(ctx, in interface{}, opts ...interface{}) *gomock.Call {
	varargs := append([]interface{}{ctx, in}, opts...)
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "SecurityGroupUpdate", reflect.TypeOf((*MockNwSecurityClient)(nil).SecurityGroupUpdate), varargs...)
}

// SecurityGroupDelete mocks base method
func (m *MockNwSecurityClient) SecurityGroupDelete(ctx context.Context, in *SecurityGroupDeleteRequestMsg, opts ...grpc.CallOption) (*SecurityGroupDeleteResponseMsg, error) {
	varargs := []interface{}{ctx, in}
	for _, a := range opts {
		varargs = append(varargs, a)
	}
	ret := m.ctrl.Call(m, "SecurityGroupDelete", varargs...)
	ret0, _ := ret[0].(*SecurityGroupDeleteResponseMsg)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// SecurityGroupDelete indicates an expected call of SecurityGroupDelete
func (mr *MockNwSecurityClientMockRecorder) SecurityGroupDelete(ctx, in interface{}, opts ...interface{}) *gomock.Call {
	varargs := append([]interface{}{ctx, in}, opts...)
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "SecurityGroupDelete", reflect.TypeOf((*MockNwSecurityClient)(nil).SecurityGroupDelete), varargs...)
}

// SecurityGroupGet mocks base method
func (m *MockNwSecurityClient) SecurityGroupGet(ctx context.Context, in *SecurityGroupGetRequestMsg, opts ...grpc.CallOption) (*SecurityGroupGetResponseMsg, error) {
	varargs := []interface{}{ctx, in}
	for _, a := range opts {
		varargs = append(varargs, a)
	}
	ret := m.ctrl.Call(m, "SecurityGroupGet", varargs...)
	ret0, _ := ret[0].(*SecurityGroupGetResponseMsg)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// SecurityGroupGet indicates an expected call of SecurityGroupGet
func (mr *MockNwSecurityClientMockRecorder) SecurityGroupGet(ctx, in interface{}, opts ...interface{}) *gomock.Call {
	varargs := append([]interface{}{ctx, in}, opts...)
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "SecurityGroupGet", reflect.TypeOf((*MockNwSecurityClient)(nil).SecurityGroupGet), varargs...)
}

// DoSPolicyCreate mocks base method
func (m *MockNwSecurityClient) DoSPolicyCreate(ctx context.Context, in *DoSPolicyRequestMsg, opts ...grpc.CallOption) (*DoSPolicyResponseMsg, error) {
	varargs := []interface{}{ctx, in}
	for _, a := range opts {
		varargs = append(varargs, a)
	}
	ret := m.ctrl.Call(m, "DoSPolicyCreate", varargs...)
	ret0, _ := ret[0].(*DoSPolicyResponseMsg)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// DoSPolicyCreate indicates an expected call of DoSPolicyCreate
func (mr *MockNwSecurityClientMockRecorder) DoSPolicyCreate(ctx, in interface{}, opts ...interface{}) *gomock.Call {
	varargs := append([]interface{}{ctx, in}, opts...)
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "DoSPolicyCreate", reflect.TypeOf((*MockNwSecurityClient)(nil).DoSPolicyCreate), varargs...)
}

// DoSPolicyUpdate mocks base method
func (m *MockNwSecurityClient) DoSPolicyUpdate(ctx context.Context, in *DoSPolicyRequestMsg, opts ...grpc.CallOption) (*DoSPolicyResponseMsg, error) {
	varargs := []interface{}{ctx, in}
	for _, a := range opts {
		varargs = append(varargs, a)
	}
	ret := m.ctrl.Call(m, "DoSPolicyUpdate", varargs...)
	ret0, _ := ret[0].(*DoSPolicyResponseMsg)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// DoSPolicyUpdate indicates an expected call of DoSPolicyUpdate
func (mr *MockNwSecurityClientMockRecorder) DoSPolicyUpdate(ctx, in interface{}, opts ...interface{}) *gomock.Call {
	varargs := append([]interface{}{ctx, in}, opts...)
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "DoSPolicyUpdate", reflect.TypeOf((*MockNwSecurityClient)(nil).DoSPolicyUpdate), varargs...)
}

// DoSPolicyDelete mocks base method
func (m *MockNwSecurityClient) DoSPolicyDelete(ctx context.Context, in *DoSPolicyDeleteRequestMsg, opts ...grpc.CallOption) (*DoSPolicyDeleteResponseMsg, error) {
	varargs := []interface{}{ctx, in}
	for _, a := range opts {
		varargs = append(varargs, a)
	}
	ret := m.ctrl.Call(m, "DoSPolicyDelete", varargs...)
	ret0, _ := ret[0].(*DoSPolicyDeleteResponseMsg)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// DoSPolicyDelete indicates an expected call of DoSPolicyDelete
func (mr *MockNwSecurityClientMockRecorder) DoSPolicyDelete(ctx, in interface{}, opts ...interface{}) *gomock.Call {
	varargs := append([]interface{}{ctx, in}, opts...)
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "DoSPolicyDelete", reflect.TypeOf((*MockNwSecurityClient)(nil).DoSPolicyDelete), varargs...)
}

// DoSPolicyGet mocks base method
func (m *MockNwSecurityClient) DoSPolicyGet(ctx context.Context, in *DoSPolicyGetRequestMsg, opts ...grpc.CallOption) (*DoSPolicyGetResponseMsg, error) {
	varargs := []interface{}{ctx, in}
	for _, a := range opts {
		varargs = append(varargs, a)
	}
	ret := m.ctrl.Call(m, "DoSPolicyGet", varargs...)
	ret0, _ := ret[0].(*DoSPolicyGetResponseMsg)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// DoSPolicyGet indicates an expected call of DoSPolicyGet
func (mr *MockNwSecurityClientMockRecorder) DoSPolicyGet(ctx, in interface{}, opts ...interface{}) *gomock.Call {
	varargs := append([]interface{}{ctx, in}, opts...)
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "DoSPolicyGet", reflect.TypeOf((*MockNwSecurityClient)(nil).DoSPolicyGet), varargs...)
}

// MockNwSecurityServer is a mock of NwSecurityServer interface
type MockNwSecurityServer struct {
	ctrl     *gomock.Controller
	recorder *MockNwSecurityServerMockRecorder
}

// MockNwSecurityServerMockRecorder is the mock recorder for MockNwSecurityServer
type MockNwSecurityServerMockRecorder struct {
	mock *MockNwSecurityServer
}

// NewMockNwSecurityServer creates a new mock instance
func NewMockNwSecurityServer(ctrl *gomock.Controller) *MockNwSecurityServer {
	mock := &MockNwSecurityServer{ctrl: ctrl}
	mock.recorder = &MockNwSecurityServerMockRecorder{mock}
	return mock
}

// EXPECT returns an object that allows the caller to indicate expected use
func (m *MockNwSecurityServer) EXPECT() *MockNwSecurityServerMockRecorder {
	return m.recorder
}

// SecurityProfileCreate mocks base method
func (m *MockNwSecurityServer) SecurityProfileCreate(arg0 context.Context, arg1 *SecurityProfileRequestMsg) (*SecurityProfileResponseMsg, error) {
	ret := m.ctrl.Call(m, "SecurityProfileCreate", arg0, arg1)
	ret0, _ := ret[0].(*SecurityProfileResponseMsg)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// SecurityProfileCreate indicates an expected call of SecurityProfileCreate
func (mr *MockNwSecurityServerMockRecorder) SecurityProfileCreate(arg0, arg1 interface{}) *gomock.Call {
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "SecurityProfileCreate", reflect.TypeOf((*MockNwSecurityServer)(nil).SecurityProfileCreate), arg0, arg1)
}

// SecurityProfileUpdate mocks base method
func (m *MockNwSecurityServer) SecurityProfileUpdate(arg0 context.Context, arg1 *SecurityProfileRequestMsg) (*SecurityProfileResponseMsg, error) {
	ret := m.ctrl.Call(m, "SecurityProfileUpdate", arg0, arg1)
	ret0, _ := ret[0].(*SecurityProfileResponseMsg)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// SecurityProfileUpdate indicates an expected call of SecurityProfileUpdate
func (mr *MockNwSecurityServerMockRecorder) SecurityProfileUpdate(arg0, arg1 interface{}) *gomock.Call {
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "SecurityProfileUpdate", reflect.TypeOf((*MockNwSecurityServer)(nil).SecurityProfileUpdate), arg0, arg1)
}

// SecurityProfileDelete mocks base method
func (m *MockNwSecurityServer) SecurityProfileDelete(arg0 context.Context, arg1 *SecurityProfileDeleteRequestMsg) (*SecurityProfileDeleteResponseMsg, error) {
	ret := m.ctrl.Call(m, "SecurityProfileDelete", arg0, arg1)
	ret0, _ := ret[0].(*SecurityProfileDeleteResponseMsg)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// SecurityProfileDelete indicates an expected call of SecurityProfileDelete
func (mr *MockNwSecurityServerMockRecorder) SecurityProfileDelete(arg0, arg1 interface{}) *gomock.Call {
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "SecurityProfileDelete", reflect.TypeOf((*MockNwSecurityServer)(nil).SecurityProfileDelete), arg0, arg1)
}

// SecurityProfileGet mocks base method
func (m *MockNwSecurityServer) SecurityProfileGet(arg0 context.Context, arg1 *SecurityProfileGetRequestMsg) (*SecurityProfileGetResponseMsg, error) {
	ret := m.ctrl.Call(m, "SecurityProfileGet", arg0, arg1)
	ret0, _ := ret[0].(*SecurityProfileGetResponseMsg)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// SecurityProfileGet indicates an expected call of SecurityProfileGet
func (mr *MockNwSecurityServerMockRecorder) SecurityProfileGet(arg0, arg1 interface{}) *gomock.Call {
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "SecurityProfileGet", reflect.TypeOf((*MockNwSecurityServer)(nil).SecurityProfileGet), arg0, arg1)
}

// SecurityGroupCreate mocks base method
func (m *MockNwSecurityServer) SecurityGroupCreate(arg0 context.Context, arg1 *SecurityGroupRequestMsg) (*SecurityGroupResponseMsg, error) {
	ret := m.ctrl.Call(m, "SecurityGroupCreate", arg0, arg1)
	ret0, _ := ret[0].(*SecurityGroupResponseMsg)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// SecurityGroupCreate indicates an expected call of SecurityGroupCreate
func (mr *MockNwSecurityServerMockRecorder) SecurityGroupCreate(arg0, arg1 interface{}) *gomock.Call {
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "SecurityGroupCreate", reflect.TypeOf((*MockNwSecurityServer)(nil).SecurityGroupCreate), arg0, arg1)
}

// SecurityGroupUpdate mocks base method
func (m *MockNwSecurityServer) SecurityGroupUpdate(arg0 context.Context, arg1 *SecurityGroupRequestMsg) (*SecurityGroupResponseMsg, error) {
	ret := m.ctrl.Call(m, "SecurityGroupUpdate", arg0, arg1)
	ret0, _ := ret[0].(*SecurityGroupResponseMsg)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// SecurityGroupUpdate indicates an expected call of SecurityGroupUpdate
func (mr *MockNwSecurityServerMockRecorder) SecurityGroupUpdate(arg0, arg1 interface{}) *gomock.Call {
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "SecurityGroupUpdate", reflect.TypeOf((*MockNwSecurityServer)(nil).SecurityGroupUpdate), arg0, arg1)
}

// SecurityGroupDelete mocks base method
func (m *MockNwSecurityServer) SecurityGroupDelete(arg0 context.Context, arg1 *SecurityGroupDeleteRequestMsg) (*SecurityGroupDeleteResponseMsg, error) {
	ret := m.ctrl.Call(m, "SecurityGroupDelete", arg0, arg1)
	ret0, _ := ret[0].(*SecurityGroupDeleteResponseMsg)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// SecurityGroupDelete indicates an expected call of SecurityGroupDelete
func (mr *MockNwSecurityServerMockRecorder) SecurityGroupDelete(arg0, arg1 interface{}) *gomock.Call {
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "SecurityGroupDelete", reflect.TypeOf((*MockNwSecurityServer)(nil).SecurityGroupDelete), arg0, arg1)
}

// SecurityGroupGet mocks base method
func (m *MockNwSecurityServer) SecurityGroupGet(arg0 context.Context, arg1 *SecurityGroupGetRequestMsg) (*SecurityGroupGetResponseMsg, error) {
	ret := m.ctrl.Call(m, "SecurityGroupGet", arg0, arg1)
	ret0, _ := ret[0].(*SecurityGroupGetResponseMsg)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// SecurityGroupGet indicates an expected call of SecurityGroupGet
func (mr *MockNwSecurityServerMockRecorder) SecurityGroupGet(arg0, arg1 interface{}) *gomock.Call {
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "SecurityGroupGet", reflect.TypeOf((*MockNwSecurityServer)(nil).SecurityGroupGet), arg0, arg1)
}

// DoSPolicyCreate mocks base method
func (m *MockNwSecurityServer) DoSPolicyCreate(arg0 context.Context, arg1 *DoSPolicyRequestMsg) (*DoSPolicyResponseMsg, error) {
	ret := m.ctrl.Call(m, "DoSPolicyCreate", arg0, arg1)
	ret0, _ := ret[0].(*DoSPolicyResponseMsg)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// DoSPolicyCreate indicates an expected call of DoSPolicyCreate
func (mr *MockNwSecurityServerMockRecorder) DoSPolicyCreate(arg0, arg1 interface{}) *gomock.Call {
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "DoSPolicyCreate", reflect.TypeOf((*MockNwSecurityServer)(nil).DoSPolicyCreate), arg0, arg1)
}

// DoSPolicyUpdate mocks base method
func (m *MockNwSecurityServer) DoSPolicyUpdate(arg0 context.Context, arg1 *DoSPolicyRequestMsg) (*DoSPolicyResponseMsg, error) {
	ret := m.ctrl.Call(m, "DoSPolicyUpdate", arg0, arg1)
	ret0, _ := ret[0].(*DoSPolicyResponseMsg)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// DoSPolicyUpdate indicates an expected call of DoSPolicyUpdate
func (mr *MockNwSecurityServerMockRecorder) DoSPolicyUpdate(arg0, arg1 interface{}) *gomock.Call {
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "DoSPolicyUpdate", reflect.TypeOf((*MockNwSecurityServer)(nil).DoSPolicyUpdate), arg0, arg1)
}

// DoSPolicyDelete mocks base method
func (m *MockNwSecurityServer) DoSPolicyDelete(arg0 context.Context, arg1 *DoSPolicyDeleteRequestMsg) (*DoSPolicyDeleteResponseMsg, error) {
	ret := m.ctrl.Call(m, "DoSPolicyDelete", arg0, arg1)
	ret0, _ := ret[0].(*DoSPolicyDeleteResponseMsg)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// DoSPolicyDelete indicates an expected call of DoSPolicyDelete
func (mr *MockNwSecurityServerMockRecorder) DoSPolicyDelete(arg0, arg1 interface{}) *gomock.Call {
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "DoSPolicyDelete", reflect.TypeOf((*MockNwSecurityServer)(nil).DoSPolicyDelete), arg0, arg1)
}

// DoSPolicyGet mocks base method
func (m *MockNwSecurityServer) DoSPolicyGet(arg0 context.Context, arg1 *DoSPolicyGetRequestMsg) (*DoSPolicyGetResponseMsg, error) {
	ret := m.ctrl.Call(m, "DoSPolicyGet", arg0, arg1)
	ret0, _ := ret[0].(*DoSPolicyGetResponseMsg)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// DoSPolicyGet indicates an expected call of DoSPolicyGet
func (mr *MockNwSecurityServerMockRecorder) DoSPolicyGet(arg0, arg1 interface{}) *gomock.Call {
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "DoSPolicyGet", reflect.TypeOf((*MockNwSecurityServer)(nil).DoSPolicyGet), arg0, arg1)
}
