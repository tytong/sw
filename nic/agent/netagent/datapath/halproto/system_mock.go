// Code generated by MockGen. DO NOT EDIT.
// Source: system.pb.go

// Package halproto is a generated GoMock package.
package halproto

import (
	reflect "reflect"

	gomock "github.com/golang/mock/gomock"
	context "golang.org/x/net/context"
	grpc "google.golang.org/grpc"
)

// MockSystemClient is a mock of SystemClient interface
type MockSystemClient struct {
	ctrl     *gomock.Controller
	recorder *MockSystemClientMockRecorder
}

// MockSystemClientMockRecorder is the mock recorder for MockSystemClient
type MockSystemClientMockRecorder struct {
	mock *MockSystemClient
}

// NewMockSystemClient creates a new mock instance
func NewMockSystemClient(ctrl *gomock.Controller) *MockSystemClient {
	mock := &MockSystemClient{ctrl: ctrl}
	mock.recorder = &MockSystemClientMockRecorder{mock}
	return mock
}

// EXPECT returns an object that allows the caller to indicate expected use
func (m *MockSystemClient) EXPECT() *MockSystemClientMockRecorder {
	return m.recorder
}

// ApiStatsGet mocks base method
func (m *MockSystemClient) ApiStatsGet(ctx context.Context, in *Empty, opts ...grpc.CallOption) (*ApiStatsResponse, error) {
	varargs := []interface{}{ctx, in}
	for _, a := range opts {
		varargs = append(varargs, a)
	}
	ret := m.ctrl.Call(m, "ApiStatsGet", varargs...)
	ret0, _ := ret[0].(*ApiStatsResponse)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// ApiStatsGet indicates an expected call of ApiStatsGet
func (mr *MockSystemClientMockRecorder) ApiStatsGet(ctx, in interface{}, opts ...interface{}) *gomock.Call {
	varargs := append([]interface{}{ctx, in}, opts...)
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "ApiStatsGet", reflect.TypeOf((*MockSystemClient)(nil).ApiStatsGet), varargs...)
}

// SystemGet mocks base method
func (m *MockSystemClient) SystemGet(ctx context.Context, in *SystemGetRequest, opts ...grpc.CallOption) (*SystemResponse, error) {
	varargs := []interface{}{ctx, in}
	for _, a := range opts {
		varargs = append(varargs, a)
	}
	ret := m.ctrl.Call(m, "SystemGet", varargs...)
	ret0, _ := ret[0].(*SystemResponse)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// SystemGet indicates an expected call of SystemGet
func (mr *MockSystemClientMockRecorder) SystemGet(ctx, in interface{}, opts ...interface{}) *gomock.Call {
	varargs := append([]interface{}{ctx, in}, opts...)
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "SystemGet", reflect.TypeOf((*MockSystemClient)(nil).SystemGet), varargs...)
}

// SystemUuidGet mocks base method
func (m *MockSystemClient) SystemUuidGet(ctx context.Context, in *Empty, opts ...grpc.CallOption) (*SystemResponse, error) {
	varargs := []interface{}{ctx, in}
	for _, a := range opts {
		varargs = append(varargs, a)
	}
	ret := m.ctrl.Call(m, "SystemUuidGet", varargs...)
	ret0, _ := ret[0].(*SystemResponse)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// SystemUuidGet indicates an expected call of SystemUuidGet
func (mr *MockSystemClientMockRecorder) SystemUuidGet(ctx, in interface{}, opts ...interface{}) *gomock.Call {
	varargs := append([]interface{}{ctx, in}, opts...)
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "SystemUuidGet", reflect.TypeOf((*MockSystemClient)(nil).SystemUuidGet), varargs...)
}

// ClearIngressDropStats mocks base method
func (m *MockSystemClient) ClearIngressDropStats(ctx context.Context, in *Empty, opts ...grpc.CallOption) (*Empty, error) {
	varargs := []interface{}{ctx, in}
	for _, a := range opts {
		varargs = append(varargs, a)
	}
	ret := m.ctrl.Call(m, "ClearIngressDropStats", varargs...)
	ret0, _ := ret[0].(*Empty)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// ClearIngressDropStats indicates an expected call of ClearIngressDropStats
func (mr *MockSystemClientMockRecorder) ClearIngressDropStats(ctx, in interface{}, opts ...interface{}) *gomock.Call {
	varargs := append([]interface{}{ctx, in}, opts...)
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "ClearIngressDropStats", reflect.TypeOf((*MockSystemClient)(nil).ClearIngressDropStats), varargs...)
}

// ClearEgressDropStats mocks base method
func (m *MockSystemClient) ClearEgressDropStats(ctx context.Context, in *Empty, opts ...grpc.CallOption) (*Empty, error) {
	varargs := []interface{}{ctx, in}
	for _, a := range opts {
		varargs = append(varargs, a)
	}
	ret := m.ctrl.Call(m, "ClearEgressDropStats", varargs...)
	ret0, _ := ret[0].(*Empty)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// ClearEgressDropStats indicates an expected call of ClearEgressDropStats
func (mr *MockSystemClientMockRecorder) ClearEgressDropStats(ctx, in interface{}, opts ...interface{}) *gomock.Call {
	varargs := append([]interface{}{ctx, in}, opts...)
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "ClearEgressDropStats", reflect.TypeOf((*MockSystemClient)(nil).ClearEgressDropStats), varargs...)
}

// ClearPbDropStats mocks base method
func (m *MockSystemClient) ClearPbDropStats(ctx context.Context, in *Empty, opts ...grpc.CallOption) (*Empty, error) {
	varargs := []interface{}{ctx, in}
	for _, a := range opts {
		varargs = append(varargs, a)
	}
	ret := m.ctrl.Call(m, "ClearPbDropStats", varargs...)
	ret0, _ := ret[0].(*Empty)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// ClearPbDropStats indicates an expected call of ClearPbDropStats
func (mr *MockSystemClientMockRecorder) ClearPbDropStats(ctx, in interface{}, opts ...interface{}) *gomock.Call {
	varargs := append([]interface{}{ctx, in}, opts...)
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "ClearPbDropStats", reflect.TypeOf((*MockSystemClient)(nil).ClearPbDropStats), varargs...)
}

// ClearFteStats mocks base method
func (m *MockSystemClient) ClearFteStats(ctx context.Context, in *Empty, opts ...grpc.CallOption) (*Empty, error) {
	varargs := []interface{}{ctx, in}
	for _, a := range opts {
		varargs = append(varargs, a)
	}
	ret := m.ctrl.Call(m, "ClearFteStats", varargs...)
	ret0, _ := ret[0].(*Empty)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// ClearFteStats indicates an expected call of ClearFteStats
func (mr *MockSystemClientMockRecorder) ClearFteStats(ctx, in interface{}, opts ...interface{}) *gomock.Call {
	varargs := append([]interface{}{ctx, in}, opts...)
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "ClearFteStats", reflect.TypeOf((*MockSystemClient)(nil).ClearFteStats), varargs...)
}

// ClearFteTxRxStats mocks base method
func (m *MockSystemClient) ClearFteTxRxStats(ctx context.Context, in *Empty, opts ...grpc.CallOption) (*Empty, error) {
	varargs := []interface{}{ctx, in}
	for _, a := range opts {
		varargs = append(varargs, a)
	}
	ret := m.ctrl.Call(m, "ClearFteTxRxStats", varargs...)
	ret0, _ := ret[0].(*Empty)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// ClearFteTxRxStats indicates an expected call of ClearFteTxRxStats
func (mr *MockSystemClientMockRecorder) ClearFteTxRxStats(ctx, in interface{}, opts ...interface{}) *gomock.Call {
	varargs := append([]interface{}{ctx, in}, opts...)
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "ClearFteTxRxStats", reflect.TypeOf((*MockSystemClient)(nil).ClearFteTxRxStats), varargs...)
}

// ClearTableStats mocks base method
func (m *MockSystemClient) ClearTableStats(ctx context.Context, in *Empty, opts ...grpc.CallOption) (*Empty, error) {
	varargs := []interface{}{ctx, in}
	for _, a := range opts {
		varargs = append(varargs, a)
	}
	ret := m.ctrl.Call(m, "ClearTableStats", varargs...)
	ret0, _ := ret[0].(*Empty)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// ClearTableStats indicates an expected call of ClearTableStats
func (mr *MockSystemClientMockRecorder) ClearTableStats(ctx, in interface{}, opts ...interface{}) *gomock.Call {
	varargs := append([]interface{}{ctx, in}, opts...)
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "ClearTableStats", reflect.TypeOf((*MockSystemClient)(nil).ClearTableStats), varargs...)
}

// ClearPbStats mocks base method
func (m *MockSystemClient) ClearPbStats(ctx context.Context, in *Empty, opts ...grpc.CallOption) (*Empty, error) {
	varargs := []interface{}{ctx, in}
	for _, a := range opts {
		varargs = append(varargs, a)
	}
	ret := m.ctrl.Call(m, "ClearPbStats", varargs...)
	ret0, _ := ret[0].(*Empty)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// ClearPbStats indicates an expected call of ClearPbStats
func (mr *MockSystemClientMockRecorder) ClearPbStats(ctx, in interface{}, opts ...interface{}) *gomock.Call {
	varargs := append([]interface{}{ctx, in}, opts...)
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "ClearPbStats", reflect.TypeOf((*MockSystemClient)(nil).ClearPbStats), varargs...)
}

// ForwardingModeGet mocks base method
func (m *MockSystemClient) ForwardingModeGet(ctx context.Context, in *Empty, opts ...grpc.CallOption) (*ForwardingModeResponse, error) {
	varargs := []interface{}{ctx, in}
	for _, a := range opts {
		varargs = append(varargs, a)
	}
	ret := m.ctrl.Call(m, "ForwardingModeGet", varargs...)
	ret0, _ := ret[0].(*ForwardingModeResponse)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// ForwardingModeGet indicates an expected call of ForwardingModeGet
func (mr *MockSystemClientMockRecorder) ForwardingModeGet(ctx, in interface{}, opts ...interface{}) *gomock.Call {
	varargs := append([]interface{}{ctx, in}, opts...)
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "ForwardingModeGet", reflect.TypeOf((*MockSystemClient)(nil).ForwardingModeGet), varargs...)
}

// FeatureProfileGet mocks base method
func (m *MockSystemClient) FeatureProfileGet(ctx context.Context, in *Empty, opts ...grpc.CallOption) (*FeatureProfileResponse, error) {
	varargs := []interface{}{ctx, in}
	for _, a := range opts {
		varargs = append(varargs, a)
	}
	ret := m.ctrl.Call(m, "FeatureProfileGet", varargs...)
	ret0, _ := ret[0].(*FeatureProfileResponse)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// FeatureProfileGet indicates an expected call of FeatureProfileGet
func (mr *MockSystemClientMockRecorder) FeatureProfileGet(ctx, in interface{}, opts ...interface{}) *gomock.Call {
	varargs := append([]interface{}{ctx, in}, opts...)
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "FeatureProfileGet", reflect.TypeOf((*MockSystemClient)(nil).FeatureProfileGet), varargs...)
}

// MockSystemServer is a mock of SystemServer interface
type MockSystemServer struct {
	ctrl     *gomock.Controller
	recorder *MockSystemServerMockRecorder
}

// MockSystemServerMockRecorder is the mock recorder for MockSystemServer
type MockSystemServerMockRecorder struct {
	mock *MockSystemServer
}

// NewMockSystemServer creates a new mock instance
func NewMockSystemServer(ctrl *gomock.Controller) *MockSystemServer {
	mock := &MockSystemServer{ctrl: ctrl}
	mock.recorder = &MockSystemServerMockRecorder{mock}
	return mock
}

// EXPECT returns an object that allows the caller to indicate expected use
func (m *MockSystemServer) EXPECT() *MockSystemServerMockRecorder {
	return m.recorder
}

// ApiStatsGet mocks base method
func (m *MockSystemServer) ApiStatsGet(arg0 context.Context, arg1 *Empty) (*ApiStatsResponse, error) {
	ret := m.ctrl.Call(m, "ApiStatsGet", arg0, arg1)
	ret0, _ := ret[0].(*ApiStatsResponse)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// ApiStatsGet indicates an expected call of ApiStatsGet
func (mr *MockSystemServerMockRecorder) ApiStatsGet(arg0, arg1 interface{}) *gomock.Call {
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "ApiStatsGet", reflect.TypeOf((*MockSystemServer)(nil).ApiStatsGet), arg0, arg1)
}

// SystemGet mocks base method
func (m *MockSystemServer) SystemGet(arg0 context.Context, arg1 *SystemGetRequest) (*SystemResponse, error) {
	ret := m.ctrl.Call(m, "SystemGet", arg0, arg1)
	ret0, _ := ret[0].(*SystemResponse)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// SystemGet indicates an expected call of SystemGet
func (mr *MockSystemServerMockRecorder) SystemGet(arg0, arg1 interface{}) *gomock.Call {
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "SystemGet", reflect.TypeOf((*MockSystemServer)(nil).SystemGet), arg0, arg1)
}

// SystemUuidGet mocks base method
func (m *MockSystemServer) SystemUuidGet(arg0 context.Context, arg1 *Empty) (*SystemResponse, error) {
	ret := m.ctrl.Call(m, "SystemUuidGet", arg0, arg1)
	ret0, _ := ret[0].(*SystemResponse)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// SystemUuidGet indicates an expected call of SystemUuidGet
func (mr *MockSystemServerMockRecorder) SystemUuidGet(arg0, arg1 interface{}) *gomock.Call {
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "SystemUuidGet", reflect.TypeOf((*MockSystemServer)(nil).SystemUuidGet), arg0, arg1)
}

// ClearIngressDropStats mocks base method
func (m *MockSystemServer) ClearIngressDropStats(arg0 context.Context, arg1 *Empty) (*Empty, error) {
	ret := m.ctrl.Call(m, "ClearIngressDropStats", arg0, arg1)
	ret0, _ := ret[0].(*Empty)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// ClearIngressDropStats indicates an expected call of ClearIngressDropStats
func (mr *MockSystemServerMockRecorder) ClearIngressDropStats(arg0, arg1 interface{}) *gomock.Call {
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "ClearIngressDropStats", reflect.TypeOf((*MockSystemServer)(nil).ClearIngressDropStats), arg0, arg1)
}

// ClearEgressDropStats mocks base method
func (m *MockSystemServer) ClearEgressDropStats(arg0 context.Context, arg1 *Empty) (*Empty, error) {
	ret := m.ctrl.Call(m, "ClearEgressDropStats", arg0, arg1)
	ret0, _ := ret[0].(*Empty)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// ClearEgressDropStats indicates an expected call of ClearEgressDropStats
func (mr *MockSystemServerMockRecorder) ClearEgressDropStats(arg0, arg1 interface{}) *gomock.Call {
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "ClearEgressDropStats", reflect.TypeOf((*MockSystemServer)(nil).ClearEgressDropStats), arg0, arg1)
}

// ClearPbDropStats mocks base method
func (m *MockSystemServer) ClearPbDropStats(arg0 context.Context, arg1 *Empty) (*Empty, error) {
	ret := m.ctrl.Call(m, "ClearPbDropStats", arg0, arg1)
	ret0, _ := ret[0].(*Empty)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// ClearPbDropStats indicates an expected call of ClearPbDropStats
func (mr *MockSystemServerMockRecorder) ClearPbDropStats(arg0, arg1 interface{}) *gomock.Call {
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "ClearPbDropStats", reflect.TypeOf((*MockSystemServer)(nil).ClearPbDropStats), arg0, arg1)
}

// ClearFteStats mocks base method
func (m *MockSystemServer) ClearFteStats(arg0 context.Context, arg1 *Empty) (*Empty, error) {
	ret := m.ctrl.Call(m, "ClearFteStats", arg0, arg1)
	ret0, _ := ret[0].(*Empty)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// ClearFteStats indicates an expected call of ClearFteStats
func (mr *MockSystemServerMockRecorder) ClearFteStats(arg0, arg1 interface{}) *gomock.Call {
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "ClearFteStats", reflect.TypeOf((*MockSystemServer)(nil).ClearFteStats), arg0, arg1)
}

// ClearFteTxRxStats mocks base method
func (m *MockSystemServer) ClearFteTxRxStats(arg0 context.Context, arg1 *Empty) (*Empty, error) {
	ret := m.ctrl.Call(m, "ClearFteTxRxStats", arg0, arg1)
	ret0, _ := ret[0].(*Empty)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// ClearFteTxRxStats indicates an expected call of ClearFteTxRxStats
func (mr *MockSystemServerMockRecorder) ClearFteTxRxStats(arg0, arg1 interface{}) *gomock.Call {
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "ClearFteTxRxStats", reflect.TypeOf((*MockSystemServer)(nil).ClearFteTxRxStats), arg0, arg1)
}

// ClearTableStats mocks base method
func (m *MockSystemServer) ClearTableStats(arg0 context.Context, arg1 *Empty) (*Empty, error) {
	ret := m.ctrl.Call(m, "ClearTableStats", arg0, arg1)
	ret0, _ := ret[0].(*Empty)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// ClearTableStats indicates an expected call of ClearTableStats
func (mr *MockSystemServerMockRecorder) ClearTableStats(arg0, arg1 interface{}) *gomock.Call {
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "ClearTableStats", reflect.TypeOf((*MockSystemServer)(nil).ClearTableStats), arg0, arg1)
}

// ClearPbStats mocks base method
func (m *MockSystemServer) ClearPbStats(arg0 context.Context, arg1 *Empty) (*Empty, error) {
	ret := m.ctrl.Call(m, "ClearPbStats", arg0, arg1)
	ret0, _ := ret[0].(*Empty)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// ClearPbStats indicates an expected call of ClearPbStats
func (mr *MockSystemServerMockRecorder) ClearPbStats(arg0, arg1 interface{}) *gomock.Call {
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "ClearPbStats", reflect.TypeOf((*MockSystemServer)(nil).ClearPbStats), arg0, arg1)
}

// ForwardingModeGet mocks base method
func (m *MockSystemServer) ForwardingModeGet(arg0 context.Context, arg1 *Empty) (*ForwardingModeResponse, error) {
	ret := m.ctrl.Call(m, "ForwardingModeGet", arg0, arg1)
	ret0, _ := ret[0].(*ForwardingModeResponse)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// ForwardingModeGet indicates an expected call of ForwardingModeGet
func (mr *MockSystemServerMockRecorder) ForwardingModeGet(arg0, arg1 interface{}) *gomock.Call {
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "ForwardingModeGet", reflect.TypeOf((*MockSystemServer)(nil).ForwardingModeGet), arg0, arg1)
}

// FeatureProfileGet mocks base method
func (m *MockSystemServer) FeatureProfileGet(arg0 context.Context, arg1 *Empty) (*FeatureProfileResponse, error) {
	ret := m.ctrl.Call(m, "FeatureProfileGet", arg0, arg1)
	ret0, _ := ret[0].(*FeatureProfileResponse)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// FeatureProfileGet indicates an expected call of FeatureProfileGet
func (mr *MockSystemServerMockRecorder) FeatureProfileGet(arg0, arg1 interface{}) *gomock.Call {
	return mr.mock.ctrl.RecordCallWithMethodType(mr.mock, "FeatureProfileGet", reflect.TypeOf((*MockSystemServer)(nil).FeatureProfileGet), arg0, arg1)
}
