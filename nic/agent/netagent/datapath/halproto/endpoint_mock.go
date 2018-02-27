// Code generated by MockGen. DO NOT EDIT.
// Source: endpoint.pb.go

package halproto

import (
	reflect "reflect"

	gomock "github.com/golang/mock/gomock"
	context "golang.org/x/net/context"
	grpc "google.golang.org/grpc"
)

// MockisEndpointDeleteRequest_DeleteBy is a mock of isEndpointDeleteRequest_DeleteBy interface
type MockisEndpointDeleteRequest_DeleteBy struct {
	ctrl     *gomock.Controller
	recorder *MockisEndpointDeleteRequest_DeleteByMockRecorder
}

// MockisEndpointDeleteRequest_DeleteByMockRecorder is the mock recorder for MockisEndpointDeleteRequest_DeleteBy
type MockisEndpointDeleteRequest_DeleteByMockRecorder struct {
	mock *MockisEndpointDeleteRequest_DeleteBy
}

// NewMockisEndpointDeleteRequest_DeleteBy creates a new mock instance
func NewMockisEndpointDeleteRequest_DeleteBy(ctrl *gomock.Controller) *MockisEndpointDeleteRequest_DeleteBy {
	mock := &MockisEndpointDeleteRequest_DeleteBy{ctrl: ctrl}
	mock.recorder = &MockisEndpointDeleteRequest_DeleteByMockRecorder{mock}
	return mock
}

// EXPECT returns an object that allows the caller to indicate expected use
func (_m *MockisEndpointDeleteRequest_DeleteBy) EXPECT() *MockisEndpointDeleteRequest_DeleteByMockRecorder {
	return _m.recorder
}

// isEndpointDeleteRequest_DeleteBy mocks base method
func (_m *MockisEndpointDeleteRequest_DeleteBy) isEndpointDeleteRequest_DeleteBy() {
	_m.ctrl.Call(_m, "isEndpointDeleteRequest_DeleteBy")
}

// isEndpointDeleteRequest_DeleteBy indicates an expected call of isEndpointDeleteRequest_DeleteBy
func (_mr *MockisEndpointDeleteRequest_DeleteByMockRecorder) isEndpointDeleteRequest_DeleteBy() *gomock.Call {
	return _mr.mock.ctrl.RecordCallWithMethodType(_mr.mock, "isEndpointDeleteRequest_DeleteBy", reflect.TypeOf((*MockisEndpointDeleteRequest_DeleteBy)(nil).isEndpointDeleteRequest_DeleteBy))
}

// MarshalTo mocks base method
func (_m *MockisEndpointDeleteRequest_DeleteBy) MarshalTo(_param0 []byte) (int, error) {
	ret := _m.ctrl.Call(_m, "MarshalTo", _param0)
	ret0, _ := ret[0].(int)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// MarshalTo indicates an expected call of MarshalTo
func (_mr *MockisEndpointDeleteRequest_DeleteByMockRecorder) MarshalTo(arg0 interface{}) *gomock.Call {
	return _mr.mock.ctrl.RecordCallWithMethodType(_mr.mock, "MarshalTo", reflect.TypeOf((*MockisEndpointDeleteRequest_DeleteBy)(nil).MarshalTo), arg0)
}

// Size mocks base method
func (_m *MockisEndpointDeleteRequest_DeleteBy) Size() int {
	ret := _m.ctrl.Call(_m, "Size")
	ret0, _ := ret[0].(int)
	return ret0
}

// Size indicates an expected call of Size
func (_mr *MockisEndpointDeleteRequest_DeleteByMockRecorder) Size() *gomock.Call {
	return _mr.mock.ctrl.RecordCallWithMethodType(_mr.mock, "Size", reflect.TypeOf((*MockisEndpointDeleteRequest_DeleteBy)(nil).Size))
}

// MockEndpointClient is a mock of EndpointClient interface
type MockEndpointClient struct {
	ctrl     *gomock.Controller
	recorder *MockEndpointClientMockRecorder
}

// MockEndpointClientMockRecorder is the mock recorder for MockEndpointClient
type MockEndpointClientMockRecorder struct {
	mock *MockEndpointClient
}

// NewMockEndpointClient creates a new mock instance
func NewMockEndpointClient(ctrl *gomock.Controller) *MockEndpointClient {
	mock := &MockEndpointClient{ctrl: ctrl}
	mock.recorder = &MockEndpointClientMockRecorder{mock}
	return mock
}

// EXPECT returns an object that allows the caller to indicate expected use
func (_m *MockEndpointClient) EXPECT() *MockEndpointClientMockRecorder {
	return _m.recorder
}

// EndpointCreate mocks base method
func (_m *MockEndpointClient) EndpointCreate(ctx context.Context, in *EndpointRequestMsg, opts ...grpc.CallOption) (*EndpointResponseMsg, error) {
	_s := []interface{}{ctx, in}
	for _, _x := range opts {
		_s = append(_s, _x)
	}
	ret := _m.ctrl.Call(_m, "EndpointCreate", _s...)
	ret0, _ := ret[0].(*EndpointResponseMsg)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// EndpointCreate indicates an expected call of EndpointCreate
func (_mr *MockEndpointClientMockRecorder) EndpointCreate(arg0, arg1 interface{}, arg2 ...interface{}) *gomock.Call {
	_s := append([]interface{}{arg0, arg1}, arg2...)
	return _mr.mock.ctrl.RecordCallWithMethodType(_mr.mock, "EndpointCreate", reflect.TypeOf((*MockEndpointClient)(nil).EndpointCreate), _s...)
}

// EndpointUpdate mocks base method
func (_m *MockEndpointClient) EndpointUpdate(ctx context.Context, in *EndpointUpdateRequestMsg, opts ...grpc.CallOption) (*EndpointUpdateResponseMsg, error) {
	_s := []interface{}{ctx, in}
	for _, _x := range opts {
		_s = append(_s, _x)
	}
	ret := _m.ctrl.Call(_m, "EndpointUpdate", _s...)
	ret0, _ := ret[0].(*EndpointUpdateResponseMsg)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// EndpointUpdate indicates an expected call of EndpointUpdate
func (_mr *MockEndpointClientMockRecorder) EndpointUpdate(arg0, arg1 interface{}, arg2 ...interface{}) *gomock.Call {
	_s := append([]interface{}{arg0, arg1}, arg2...)
	return _mr.mock.ctrl.RecordCallWithMethodType(_mr.mock, "EndpointUpdate", reflect.TypeOf((*MockEndpointClient)(nil).EndpointUpdate), _s...)
}

// EndpointDelete mocks base method
func (_m *MockEndpointClient) EndpointDelete(ctx context.Context, in *EndpointDeleteRequestMsg, opts ...grpc.CallOption) (*EndpointDeleteResponseMsg, error) {
	_s := []interface{}{ctx, in}
	for _, _x := range opts {
		_s = append(_s, _x)
	}
	ret := _m.ctrl.Call(_m, "EndpointDelete", _s...)
	ret0, _ := ret[0].(*EndpointDeleteResponseMsg)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// EndpointDelete indicates an expected call of EndpointDelete
func (_mr *MockEndpointClientMockRecorder) EndpointDelete(arg0, arg1 interface{}, arg2 ...interface{}) *gomock.Call {
	_s := append([]interface{}{arg0, arg1}, arg2...)
	return _mr.mock.ctrl.RecordCallWithMethodType(_mr.mock, "EndpointDelete", reflect.TypeOf((*MockEndpointClient)(nil).EndpointDelete), _s...)
}

// EndpointGet mocks base method
func (_m *MockEndpointClient) EndpointGet(ctx context.Context, in *EndpointGetRequestMsg, opts ...grpc.CallOption) (*EndpointGetResponseMsg, error) {
	_s := []interface{}{ctx, in}
	for _, _x := range opts {
		_s = append(_s, _x)
	}
	ret := _m.ctrl.Call(_m, "EndpointGet", _s...)
	ret0, _ := ret[0].(*EndpointGetResponseMsg)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// EndpointGet indicates an expected call of EndpointGet
func (_mr *MockEndpointClientMockRecorder) EndpointGet(arg0, arg1 interface{}, arg2 ...interface{}) *gomock.Call {
	_s := append([]interface{}{arg0, arg1}, arg2...)
	return _mr.mock.ctrl.RecordCallWithMethodType(_mr.mock, "EndpointGet", reflect.TypeOf((*MockEndpointClient)(nil).EndpointGet), _s...)
}

// MockEndpointServer is a mock of EndpointServer interface
type MockEndpointServer struct {
	ctrl     *gomock.Controller
	recorder *MockEndpointServerMockRecorder
}

// MockEndpointServerMockRecorder is the mock recorder for MockEndpointServer
type MockEndpointServerMockRecorder struct {
	mock *MockEndpointServer
}

// NewMockEndpointServer creates a new mock instance
func NewMockEndpointServer(ctrl *gomock.Controller) *MockEndpointServer {
	mock := &MockEndpointServer{ctrl: ctrl}
	mock.recorder = &MockEndpointServerMockRecorder{mock}
	return mock
}

// EXPECT returns an object that allows the caller to indicate expected use
func (_m *MockEndpointServer) EXPECT() *MockEndpointServerMockRecorder {
	return _m.recorder
}

// EndpointCreate mocks base method
func (_m *MockEndpointServer) EndpointCreate(_param0 context.Context, _param1 *EndpointRequestMsg) (*EndpointResponseMsg, error) {
	ret := _m.ctrl.Call(_m, "EndpointCreate", _param0, _param1)
	ret0, _ := ret[0].(*EndpointResponseMsg)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// EndpointCreate indicates an expected call of EndpointCreate
func (_mr *MockEndpointServerMockRecorder) EndpointCreate(arg0, arg1 interface{}) *gomock.Call {
	return _mr.mock.ctrl.RecordCallWithMethodType(_mr.mock, "EndpointCreate", reflect.TypeOf((*MockEndpointServer)(nil).EndpointCreate), arg0, arg1)
}

// EndpointUpdate mocks base method
func (_m *MockEndpointServer) EndpointUpdate(_param0 context.Context, _param1 *EndpointUpdateRequestMsg) (*EndpointUpdateResponseMsg, error) {
	ret := _m.ctrl.Call(_m, "EndpointUpdate", _param0, _param1)
	ret0, _ := ret[0].(*EndpointUpdateResponseMsg)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// EndpointUpdate indicates an expected call of EndpointUpdate
func (_mr *MockEndpointServerMockRecorder) EndpointUpdate(arg0, arg1 interface{}) *gomock.Call {
	return _mr.mock.ctrl.RecordCallWithMethodType(_mr.mock, "EndpointUpdate", reflect.TypeOf((*MockEndpointServer)(nil).EndpointUpdate), arg0, arg1)
}

// EndpointDelete mocks base method
func (_m *MockEndpointServer) EndpointDelete(_param0 context.Context, _param1 *EndpointDeleteRequestMsg) (*EndpointDeleteResponseMsg, error) {
	ret := _m.ctrl.Call(_m, "EndpointDelete", _param0, _param1)
	ret0, _ := ret[0].(*EndpointDeleteResponseMsg)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// EndpointDelete indicates an expected call of EndpointDelete
func (_mr *MockEndpointServerMockRecorder) EndpointDelete(arg0, arg1 interface{}) *gomock.Call {
	return _mr.mock.ctrl.RecordCallWithMethodType(_mr.mock, "EndpointDelete", reflect.TypeOf((*MockEndpointServer)(nil).EndpointDelete), arg0, arg1)
}

// EndpointGet mocks base method
func (_m *MockEndpointServer) EndpointGet(_param0 context.Context, _param1 *EndpointGetRequestMsg) (*EndpointGetResponseMsg, error) {
	ret := _m.ctrl.Call(_m, "EndpointGet", _param0, _param1)
	ret0, _ := ret[0].(*EndpointGetResponseMsg)
	ret1, _ := ret[1].(error)
	return ret0, ret1
}

// EndpointGet indicates an expected call of EndpointGet
func (_mr *MockEndpointServerMockRecorder) EndpointGet(arg0, arg1 interface{}) *gomock.Call {
	return _mr.mock.ctrl.RecordCallWithMethodType(_mr.mock, "EndpointGet", reflect.TypeOf((*MockEndpointServer)(nil).EndpointGet), arg0, arg1)
}
