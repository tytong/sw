// Code generated by MockGen. DO NOT EDIT.
// Source: /import//src/github.com/pensando/sw/venice/utils/kvstore/watch.go

package mock

import (
	reflect "reflect"

	gomock "github.com/golang/mock/gomock"

	kvstore "github.com/pensando/sw/venice/utils/kvstore"
)

// MockWatcher is a mock of Watcher interface
type MockWatcher struct {
	ctrl     *gomock.Controller
	recorder *MockWatcherMockRecorder
}

// MockWatcherMockRecorder is the mock recorder for MockWatcher
type MockWatcherMockRecorder struct {
	mock *MockWatcher
}

// NewMockWatcher creates a new mock instance
func NewMockWatcher(ctrl *gomock.Controller) *MockWatcher {
	mock := &MockWatcher{ctrl: ctrl}
	mock.recorder = &MockWatcherMockRecorder{mock}
	return mock
}

// EXPECT returns an object that allows the caller to indicate expected use
func (_m *MockWatcher) EXPECT() *MockWatcherMockRecorder {
	return _m.recorder
}

// EventChan mocks base method
func (_m *MockWatcher) EventChan() <-chan *kvstore.WatchEvent {
	ret := _m.ctrl.Call(_m, "EventChan")
	ret0, _ := ret[0].(<-chan *kvstore.WatchEvent)
	return ret0
}

// EventChan indicates an expected call of EventChan
func (_mr *MockWatcherMockRecorder) EventChan() *gomock.Call {
	return _mr.mock.ctrl.RecordCallWithMethodType(_mr.mock, "EventChan", reflect.TypeOf((*MockWatcher)(nil).EventChan))
}

// Stop mocks base method
func (_m *MockWatcher) Stop() {
	_m.ctrl.Call(_m, "Stop")
}

// Stop indicates an expected call of Stop
func (_mr *MockWatcherMockRecorder) Stop() *gomock.Call {
	return _mr.mock.ctrl.RecordCallWithMethodType(_mr.mock, "Stop", reflect.TypeOf((*MockWatcher)(nil).Stop))
}
