// Code generated by protoc-gen-grpc-pensando DO NOT EDIT.

package grpcclient

import (
	"context"
	"errors"
	oldlog "log"

	"github.com/go-kit/kit/endpoint"
	grpctransport "github.com/go-kit/kit/transport/grpc"
	"google.golang.org/grpc"

	api "github.com/pensando/sw/api"
	network "github.com/pensando/sw/api/generated/network"
	listerwatcher "github.com/pensando/sw/api/listerwatcher"
	apiserver "github.com/pensando/sw/venice/apiserver"
	"github.com/pensando/sw/venice/globals"
	"github.com/pensando/sw/venice/utils/kvstore"
	"github.com/pensando/sw/venice/utils/log"
	"github.com/pensando/sw/venice/utils/trace"
)

// Dummy vars to suppress import errors
var _ api.TypeMeta
var _ listerwatcher.WatcherClient
var _ kvstore.Interface

// NewNetworkV1 sets up a new client for NetworkV1
func NewNetworkV1(conn *grpc.ClientConn, logger log.Logger) network.ServiceNetworkV1Client {

	var lAutoAddLbPolicyEndpoint endpoint.Endpoint
	{
		lAutoAddLbPolicyEndpoint = grpctransport.NewClient(
			conn,
			"network.NetworkV1",
			"AutoAddLbPolicy",
			network.EncodeGrpcReqLbPolicy,
			network.DecodeGrpcRespLbPolicy,
			&network.LbPolicy{},
			grpctransport.ClientBefore(trace.ToGRPCRequest(logger)),
			grpctransport.ClientBefore(dummyBefore),
		).Endpoint()
		lAutoAddLbPolicyEndpoint = trace.ClientEndPoint("NetworkV1:AutoAddLbPolicy")(lAutoAddLbPolicyEndpoint)
	}
	var lAutoAddNetworkEndpoint endpoint.Endpoint
	{
		lAutoAddNetworkEndpoint = grpctransport.NewClient(
			conn,
			"network.NetworkV1",
			"AutoAddNetwork",
			network.EncodeGrpcReqNetwork,
			network.DecodeGrpcRespNetwork,
			&network.Network{},
			grpctransport.ClientBefore(trace.ToGRPCRequest(logger)),
			grpctransport.ClientBefore(dummyBefore),
		).Endpoint()
		lAutoAddNetworkEndpoint = trace.ClientEndPoint("NetworkV1:AutoAddNetwork")(lAutoAddNetworkEndpoint)
	}
	var lAutoAddServiceEndpoint endpoint.Endpoint
	{
		lAutoAddServiceEndpoint = grpctransport.NewClient(
			conn,
			"network.NetworkV1",
			"AutoAddService",
			network.EncodeGrpcReqService,
			network.DecodeGrpcRespService,
			&network.Service{},
			grpctransport.ClientBefore(trace.ToGRPCRequest(logger)),
			grpctransport.ClientBefore(dummyBefore),
		).Endpoint()
		lAutoAddServiceEndpoint = trace.ClientEndPoint("NetworkV1:AutoAddService")(lAutoAddServiceEndpoint)
	}
	var lAutoDeleteLbPolicyEndpoint endpoint.Endpoint
	{
		lAutoDeleteLbPolicyEndpoint = grpctransport.NewClient(
			conn,
			"network.NetworkV1",
			"AutoDeleteLbPolicy",
			network.EncodeGrpcReqLbPolicy,
			network.DecodeGrpcRespLbPolicy,
			&network.LbPolicy{},
			grpctransport.ClientBefore(trace.ToGRPCRequest(logger)),
			grpctransport.ClientBefore(dummyBefore),
		).Endpoint()
		lAutoDeleteLbPolicyEndpoint = trace.ClientEndPoint("NetworkV1:AutoDeleteLbPolicy")(lAutoDeleteLbPolicyEndpoint)
	}
	var lAutoDeleteNetworkEndpoint endpoint.Endpoint
	{
		lAutoDeleteNetworkEndpoint = grpctransport.NewClient(
			conn,
			"network.NetworkV1",
			"AutoDeleteNetwork",
			network.EncodeGrpcReqNetwork,
			network.DecodeGrpcRespNetwork,
			&network.Network{},
			grpctransport.ClientBefore(trace.ToGRPCRequest(logger)),
			grpctransport.ClientBefore(dummyBefore),
		).Endpoint()
		lAutoDeleteNetworkEndpoint = trace.ClientEndPoint("NetworkV1:AutoDeleteNetwork")(lAutoDeleteNetworkEndpoint)
	}
	var lAutoDeleteServiceEndpoint endpoint.Endpoint
	{
		lAutoDeleteServiceEndpoint = grpctransport.NewClient(
			conn,
			"network.NetworkV1",
			"AutoDeleteService",
			network.EncodeGrpcReqService,
			network.DecodeGrpcRespService,
			&network.Service{},
			grpctransport.ClientBefore(trace.ToGRPCRequest(logger)),
			grpctransport.ClientBefore(dummyBefore),
		).Endpoint()
		lAutoDeleteServiceEndpoint = trace.ClientEndPoint("NetworkV1:AutoDeleteService")(lAutoDeleteServiceEndpoint)
	}
	var lAutoGetLbPolicyEndpoint endpoint.Endpoint
	{
		lAutoGetLbPolicyEndpoint = grpctransport.NewClient(
			conn,
			"network.NetworkV1",
			"AutoGetLbPolicy",
			network.EncodeGrpcReqLbPolicy,
			network.DecodeGrpcRespLbPolicy,
			&network.LbPolicy{},
			grpctransport.ClientBefore(trace.ToGRPCRequest(logger)),
			grpctransport.ClientBefore(dummyBefore),
		).Endpoint()
		lAutoGetLbPolicyEndpoint = trace.ClientEndPoint("NetworkV1:AutoGetLbPolicy")(lAutoGetLbPolicyEndpoint)
	}
	var lAutoGetNetworkEndpoint endpoint.Endpoint
	{
		lAutoGetNetworkEndpoint = grpctransport.NewClient(
			conn,
			"network.NetworkV1",
			"AutoGetNetwork",
			network.EncodeGrpcReqNetwork,
			network.DecodeGrpcRespNetwork,
			&network.Network{},
			grpctransport.ClientBefore(trace.ToGRPCRequest(logger)),
			grpctransport.ClientBefore(dummyBefore),
		).Endpoint()
		lAutoGetNetworkEndpoint = trace.ClientEndPoint("NetworkV1:AutoGetNetwork")(lAutoGetNetworkEndpoint)
	}
	var lAutoGetServiceEndpoint endpoint.Endpoint
	{
		lAutoGetServiceEndpoint = grpctransport.NewClient(
			conn,
			"network.NetworkV1",
			"AutoGetService",
			network.EncodeGrpcReqService,
			network.DecodeGrpcRespService,
			&network.Service{},
			grpctransport.ClientBefore(trace.ToGRPCRequest(logger)),
			grpctransport.ClientBefore(dummyBefore),
		).Endpoint()
		lAutoGetServiceEndpoint = trace.ClientEndPoint("NetworkV1:AutoGetService")(lAutoGetServiceEndpoint)
	}
	var lAutoListLbPolicyEndpoint endpoint.Endpoint
	{
		lAutoListLbPolicyEndpoint = grpctransport.NewClient(
			conn,
			"network.NetworkV1",
			"AutoListLbPolicy",
			network.EncodeGrpcReqListWatchOptions,
			network.DecodeGrpcRespLbPolicyList,
			&network.LbPolicyList{},
			grpctransport.ClientBefore(trace.ToGRPCRequest(logger)),
			grpctransport.ClientBefore(dummyBefore),
		).Endpoint()
		lAutoListLbPolicyEndpoint = trace.ClientEndPoint("NetworkV1:AutoListLbPolicy")(lAutoListLbPolicyEndpoint)
	}
	var lAutoListNetworkEndpoint endpoint.Endpoint
	{
		lAutoListNetworkEndpoint = grpctransport.NewClient(
			conn,
			"network.NetworkV1",
			"AutoListNetwork",
			network.EncodeGrpcReqListWatchOptions,
			network.DecodeGrpcRespNetworkList,
			&network.NetworkList{},
			grpctransport.ClientBefore(trace.ToGRPCRequest(logger)),
			grpctransport.ClientBefore(dummyBefore),
		).Endpoint()
		lAutoListNetworkEndpoint = trace.ClientEndPoint("NetworkV1:AutoListNetwork")(lAutoListNetworkEndpoint)
	}
	var lAutoListServiceEndpoint endpoint.Endpoint
	{
		lAutoListServiceEndpoint = grpctransport.NewClient(
			conn,
			"network.NetworkV1",
			"AutoListService",
			network.EncodeGrpcReqListWatchOptions,
			network.DecodeGrpcRespServiceList,
			&network.ServiceList{},
			grpctransport.ClientBefore(trace.ToGRPCRequest(logger)),
			grpctransport.ClientBefore(dummyBefore),
		).Endpoint()
		lAutoListServiceEndpoint = trace.ClientEndPoint("NetworkV1:AutoListService")(lAutoListServiceEndpoint)
	}
	var lAutoUpdateLbPolicyEndpoint endpoint.Endpoint
	{
		lAutoUpdateLbPolicyEndpoint = grpctransport.NewClient(
			conn,
			"network.NetworkV1",
			"AutoUpdateLbPolicy",
			network.EncodeGrpcReqLbPolicy,
			network.DecodeGrpcRespLbPolicy,
			&network.LbPolicy{},
			grpctransport.ClientBefore(trace.ToGRPCRequest(logger)),
			grpctransport.ClientBefore(dummyBefore),
		).Endpoint()
		lAutoUpdateLbPolicyEndpoint = trace.ClientEndPoint("NetworkV1:AutoUpdateLbPolicy")(lAutoUpdateLbPolicyEndpoint)
	}
	var lAutoUpdateNetworkEndpoint endpoint.Endpoint
	{
		lAutoUpdateNetworkEndpoint = grpctransport.NewClient(
			conn,
			"network.NetworkV1",
			"AutoUpdateNetwork",
			network.EncodeGrpcReqNetwork,
			network.DecodeGrpcRespNetwork,
			&network.Network{},
			grpctransport.ClientBefore(trace.ToGRPCRequest(logger)),
			grpctransport.ClientBefore(dummyBefore),
		).Endpoint()
		lAutoUpdateNetworkEndpoint = trace.ClientEndPoint("NetworkV1:AutoUpdateNetwork")(lAutoUpdateNetworkEndpoint)
	}
	var lAutoUpdateServiceEndpoint endpoint.Endpoint
	{
		lAutoUpdateServiceEndpoint = grpctransport.NewClient(
			conn,
			"network.NetworkV1",
			"AutoUpdateService",
			network.EncodeGrpcReqService,
			network.DecodeGrpcRespService,
			&network.Service{},
			grpctransport.ClientBefore(trace.ToGRPCRequest(logger)),
			grpctransport.ClientBefore(dummyBefore),
		).Endpoint()
		lAutoUpdateServiceEndpoint = trace.ClientEndPoint("NetworkV1:AutoUpdateService")(lAutoUpdateServiceEndpoint)
	}
	return network.EndpointsNetworkV1Client{
		Client: network.NewNetworkV1Client(conn),

		AutoAddLbPolicyEndpoint:    lAutoAddLbPolicyEndpoint,
		AutoAddNetworkEndpoint:     lAutoAddNetworkEndpoint,
		AutoAddServiceEndpoint:     lAutoAddServiceEndpoint,
		AutoDeleteLbPolicyEndpoint: lAutoDeleteLbPolicyEndpoint,
		AutoDeleteNetworkEndpoint:  lAutoDeleteNetworkEndpoint,
		AutoDeleteServiceEndpoint:  lAutoDeleteServiceEndpoint,
		AutoGetLbPolicyEndpoint:    lAutoGetLbPolicyEndpoint,
		AutoGetNetworkEndpoint:     lAutoGetNetworkEndpoint,
		AutoGetServiceEndpoint:     lAutoGetServiceEndpoint,
		AutoListLbPolicyEndpoint:   lAutoListLbPolicyEndpoint,
		AutoListNetworkEndpoint:    lAutoListNetworkEndpoint,
		AutoListServiceEndpoint:    lAutoListServiceEndpoint,
		AutoUpdateLbPolicyEndpoint: lAutoUpdateLbPolicyEndpoint,
		AutoUpdateNetworkEndpoint:  lAutoUpdateNetworkEndpoint,
		AutoUpdateServiceEndpoint:  lAutoUpdateServiceEndpoint,
	}
}

// NewNetworkV1Backend creates an instrumented client with middleware
func NewNetworkV1Backend(conn *grpc.ClientConn, logger log.Logger) network.ServiceNetworkV1Client {
	cl := NewNetworkV1(conn, logger)
	cl = network.LoggingNetworkV1MiddlewareClient(logger)(cl)
	return cl
}

type grpcObjNetworkV1Network struct {
	logger log.Logger
	client network.ServiceNetworkV1Client
}

func (a *grpcObjNetworkV1Network) Create(ctx context.Context, in *network.Network) (*network.Network, error) {
	a.logger.DebugLog("msg", "received call", "object", "Network", "oper", "create")
	if in == nil {
		return nil, errors.New("invalid input")
	}
	nctx := addVersion(ctx, "v1")
	return a.client.AutoAddNetwork(nctx, in)
}

func (a *grpcObjNetworkV1Network) Update(ctx context.Context, in *network.Network) (*network.Network, error) {
	a.logger.DebugLog("msg", "received call", "object", "Network", "oper", "update")
	if in == nil {
		return nil, errors.New("invalid input")
	}
	nctx := addVersion(ctx, "v1")
	return a.client.AutoUpdateNetwork(nctx, in)
}

func (a *grpcObjNetworkV1Network) Get(ctx context.Context, objMeta *api.ObjectMeta) (*network.Network, error) {
	a.logger.DebugLog("msg", "received call", "object", "Network", "oper", "get")
	if objMeta == nil {
		return nil, errors.New("invalid input")
	}
	in := network.Network{}
	in.ObjectMeta = *objMeta
	nctx := addVersion(ctx, "v1")
	return a.client.AutoGetNetwork(nctx, &in)
}

func (a *grpcObjNetworkV1Network) Delete(ctx context.Context, objMeta *api.ObjectMeta) (*network.Network, error) {
	a.logger.DebugLog("msg", "received call", "object", "Network", "oper", "delete")
	if objMeta == nil {
		return nil, errors.New("invalid input")
	}
	in := network.Network{}
	in.ObjectMeta = *objMeta
	nctx := addVersion(ctx, "v1")
	return a.client.AutoDeleteNetwork(nctx, &in)
}

func (a *grpcObjNetworkV1Network) List(ctx context.Context, options *api.ListWatchOptions) ([]*network.Network, error) {
	a.logger.DebugLog("msg", "received call", "object", "Network", "oper", "list")
	if options == nil {
		return nil, errors.New("invalid input")
	}
	nctx := addVersion(ctx, "v1")
	r, err := a.client.AutoListNetwork(nctx, options)
	if err == nil {
		return r.Items, nil
	}
	return nil, err
}

func (a *grpcObjNetworkV1Network) Watch(ctx context.Context, options *api.ListWatchOptions) (kvstore.Watcher, error) {
	a.logger.DebugLog("msg", "received call", "object", "Network", "oper", "WatchOper")
	nctx := addVersion(ctx, "v1")
	if options == nil {
		return nil, errors.New("invalid input")
	}
	stream, err := a.client.AutoWatchNetwork(nctx, options)
	if err != nil {
		return nil, err
	}
	wstream := stream.(network.NetworkV1_AutoWatchNetworkClient)
	bridgefn := func(lw *listerwatcher.WatcherClient) {
		for {
			r, err := wstream.Recv()
			if err != nil {
				a.logger.ErrorLog("msg", "error on receive", "error", err)
				close(lw.OutCh)
				return
			}
			for _, e := range r.Events {
				ev := kvstore.WatchEvent{
					Type:   kvstore.WatchEventType(e.Type),
					Object: e.Object,
				}
				select {
				case lw.OutCh <- &ev:
				case <-wstream.Context().Done():
					close(lw.OutCh)
					return
				}
			}
		}
	}
	lw := listerwatcher.NewWatcherClient(wstream, bridgefn)
	lw.Run()
	return lw, nil
}

func (a *grpcObjNetworkV1Network) Allowed(oper apiserver.APIOperType) bool {
	return true
}

type restObjNetworkV1Network struct {
	endpoints network.EndpointsNetworkV1RestClient
	instance  string
}

func (a *restObjNetworkV1Network) Create(ctx context.Context, in *network.Network) (*network.Network, error) {
	if in == nil {
		return nil, errors.New("invalid input")
	}
	return a.endpoints.AutoAddNetwork(ctx, in)
}

func (a *restObjNetworkV1Network) Update(ctx context.Context, in *network.Network) (*network.Network, error) {
	if in == nil {
		return nil, errors.New("invalid input")
	}
	return a.endpoints.AutoUpdateNetwork(ctx, in)
}

func (a *restObjNetworkV1Network) Get(ctx context.Context, objMeta *api.ObjectMeta) (*network.Network, error) {
	if objMeta == nil {
		return nil, errors.New("invalid input")
	}
	in := network.Network{}
	in.ObjectMeta = *objMeta
	return a.endpoints.AutoGetNetwork(ctx, &in)
}

func (a *restObjNetworkV1Network) Delete(ctx context.Context, objMeta *api.ObjectMeta) (*network.Network, error) {
	if objMeta == nil {
		return nil, errors.New("invalid input")
	}
	in := network.Network{}
	in.ObjectMeta = *objMeta
	return a.endpoints.AutoDeleteNetwork(ctx, &in)
}

func (a *restObjNetworkV1Network) List(ctx context.Context, options *api.ListWatchOptions) ([]*network.Network, error) {
	if options == nil {
		return nil, errors.New("invalid input")
	}

	if options.Tenant == "" {
		options.Tenant = globals.DefaultTenant
	}
	r, err := a.endpoints.AutoListNetwork(ctx, options)
	if err == nil {
		return r.Items, nil
	}
	return nil, err
}

func (a *restObjNetworkV1Network) Watch(ctx context.Context, options *api.ListWatchOptions) (kvstore.Watcher, error) {
	if options == nil {
		return nil, errors.New("invalid input")
	}
	return a.endpoints.AutoWatchNetwork(ctx, options)
}

func (a *restObjNetworkV1Network) Allowed(oper apiserver.APIOperType) bool {
	switch oper {
	case apiserver.CreateOper:
		return true
	case apiserver.UpdateOper:
		return true
	case apiserver.GetOper:
		return true
	case apiserver.DeleteOper:
		return true
	case apiserver.ListOper:
		return true
	case apiserver.WatchOper:
		return true
	default:
		return false
	}
}

type grpcObjNetworkV1Service struct {
	logger log.Logger
	client network.ServiceNetworkV1Client
}

func (a *grpcObjNetworkV1Service) Create(ctx context.Context, in *network.Service) (*network.Service, error) {
	a.logger.DebugLog("msg", "received call", "object", "Service", "oper", "create")
	if in == nil {
		return nil, errors.New("invalid input")
	}
	nctx := addVersion(ctx, "v1")
	return a.client.AutoAddService(nctx, in)
}

func (a *grpcObjNetworkV1Service) Update(ctx context.Context, in *network.Service) (*network.Service, error) {
	a.logger.DebugLog("msg", "received call", "object", "Service", "oper", "update")
	if in == nil {
		return nil, errors.New("invalid input")
	}
	nctx := addVersion(ctx, "v1")
	return a.client.AutoUpdateService(nctx, in)
}

func (a *grpcObjNetworkV1Service) Get(ctx context.Context, objMeta *api.ObjectMeta) (*network.Service, error) {
	a.logger.DebugLog("msg", "received call", "object", "Service", "oper", "get")
	if objMeta == nil {
		return nil, errors.New("invalid input")
	}
	in := network.Service{}
	in.ObjectMeta = *objMeta
	nctx := addVersion(ctx, "v1")
	return a.client.AutoGetService(nctx, &in)
}

func (a *grpcObjNetworkV1Service) Delete(ctx context.Context, objMeta *api.ObjectMeta) (*network.Service, error) {
	a.logger.DebugLog("msg", "received call", "object", "Service", "oper", "delete")
	if objMeta == nil {
		return nil, errors.New("invalid input")
	}
	in := network.Service{}
	in.ObjectMeta = *objMeta
	nctx := addVersion(ctx, "v1")
	return a.client.AutoDeleteService(nctx, &in)
}

func (a *grpcObjNetworkV1Service) List(ctx context.Context, options *api.ListWatchOptions) ([]*network.Service, error) {
	a.logger.DebugLog("msg", "received call", "object", "Service", "oper", "list")
	if options == nil {
		return nil, errors.New("invalid input")
	}
	nctx := addVersion(ctx, "v1")
	r, err := a.client.AutoListService(nctx, options)
	if err == nil {
		return r.Items, nil
	}
	return nil, err
}

func (a *grpcObjNetworkV1Service) Watch(ctx context.Context, options *api.ListWatchOptions) (kvstore.Watcher, error) {
	a.logger.DebugLog("msg", "received call", "object", "Service", "oper", "WatchOper")
	nctx := addVersion(ctx, "v1")
	if options == nil {
		return nil, errors.New("invalid input")
	}
	stream, err := a.client.AutoWatchService(nctx, options)
	if err != nil {
		return nil, err
	}
	wstream := stream.(network.NetworkV1_AutoWatchServiceClient)
	bridgefn := func(lw *listerwatcher.WatcherClient) {
		for {
			r, err := wstream.Recv()
			if err != nil {
				a.logger.ErrorLog("msg", "error on receive", "error", err)
				close(lw.OutCh)
				return
			}
			for _, e := range r.Events {
				ev := kvstore.WatchEvent{
					Type:   kvstore.WatchEventType(e.Type),
					Object: e.Object,
				}
				select {
				case lw.OutCh <- &ev:
				case <-wstream.Context().Done():
					close(lw.OutCh)
					return
				}
			}
		}
	}
	lw := listerwatcher.NewWatcherClient(wstream, bridgefn)
	lw.Run()
	return lw, nil
}

func (a *grpcObjNetworkV1Service) Allowed(oper apiserver.APIOperType) bool {
	return true
}

type restObjNetworkV1Service struct {
	endpoints network.EndpointsNetworkV1RestClient
	instance  string
}

func (a *restObjNetworkV1Service) Create(ctx context.Context, in *network.Service) (*network.Service, error) {
	if in == nil {
		return nil, errors.New("invalid input")
	}
	return a.endpoints.AutoAddService(ctx, in)
}

func (a *restObjNetworkV1Service) Update(ctx context.Context, in *network.Service) (*network.Service, error) {
	if in == nil {
		return nil, errors.New("invalid input")
	}
	return a.endpoints.AutoUpdateService(ctx, in)
}

func (a *restObjNetworkV1Service) Get(ctx context.Context, objMeta *api.ObjectMeta) (*network.Service, error) {
	if objMeta == nil {
		return nil, errors.New("invalid input")
	}
	in := network.Service{}
	in.ObjectMeta = *objMeta
	return a.endpoints.AutoGetService(ctx, &in)
}

func (a *restObjNetworkV1Service) Delete(ctx context.Context, objMeta *api.ObjectMeta) (*network.Service, error) {
	if objMeta == nil {
		return nil, errors.New("invalid input")
	}
	in := network.Service{}
	in.ObjectMeta = *objMeta
	return a.endpoints.AutoDeleteService(ctx, &in)
}

func (a *restObjNetworkV1Service) List(ctx context.Context, options *api.ListWatchOptions) ([]*network.Service, error) {
	if options == nil {
		return nil, errors.New("invalid input")
	}

	if options.Tenant == "" {
		options.Tenant = globals.DefaultTenant
	}
	r, err := a.endpoints.AutoListService(ctx, options)
	if err == nil {
		return r.Items, nil
	}
	return nil, err
}

func (a *restObjNetworkV1Service) Watch(ctx context.Context, options *api.ListWatchOptions) (kvstore.Watcher, error) {
	if options == nil {
		return nil, errors.New("invalid input")
	}
	return a.endpoints.AutoWatchService(ctx, options)
}

func (a *restObjNetworkV1Service) Allowed(oper apiserver.APIOperType) bool {
	switch oper {
	case apiserver.CreateOper:
		return true
	case apiserver.UpdateOper:
		return true
	case apiserver.GetOper:
		return true
	case apiserver.DeleteOper:
		return true
	case apiserver.ListOper:
		return true
	case apiserver.WatchOper:
		return true
	default:
		return false
	}
}

type grpcObjNetworkV1LbPolicy struct {
	logger log.Logger
	client network.ServiceNetworkV1Client
}

func (a *grpcObjNetworkV1LbPolicy) Create(ctx context.Context, in *network.LbPolicy) (*network.LbPolicy, error) {
	a.logger.DebugLog("msg", "received call", "object", "LbPolicy", "oper", "create")
	if in == nil {
		return nil, errors.New("invalid input")
	}
	nctx := addVersion(ctx, "v1")
	return a.client.AutoAddLbPolicy(nctx, in)
}

func (a *grpcObjNetworkV1LbPolicy) Update(ctx context.Context, in *network.LbPolicy) (*network.LbPolicy, error) {
	a.logger.DebugLog("msg", "received call", "object", "LbPolicy", "oper", "update")
	if in == nil {
		return nil, errors.New("invalid input")
	}
	nctx := addVersion(ctx, "v1")
	return a.client.AutoUpdateLbPolicy(nctx, in)
}

func (a *grpcObjNetworkV1LbPolicy) Get(ctx context.Context, objMeta *api.ObjectMeta) (*network.LbPolicy, error) {
	a.logger.DebugLog("msg", "received call", "object", "LbPolicy", "oper", "get")
	if objMeta == nil {
		return nil, errors.New("invalid input")
	}
	in := network.LbPolicy{}
	in.ObjectMeta = *objMeta
	nctx := addVersion(ctx, "v1")
	return a.client.AutoGetLbPolicy(nctx, &in)
}

func (a *grpcObjNetworkV1LbPolicy) Delete(ctx context.Context, objMeta *api.ObjectMeta) (*network.LbPolicy, error) {
	a.logger.DebugLog("msg", "received call", "object", "LbPolicy", "oper", "delete")
	if objMeta == nil {
		return nil, errors.New("invalid input")
	}
	in := network.LbPolicy{}
	in.ObjectMeta = *objMeta
	nctx := addVersion(ctx, "v1")
	return a.client.AutoDeleteLbPolicy(nctx, &in)
}

func (a *grpcObjNetworkV1LbPolicy) List(ctx context.Context, options *api.ListWatchOptions) ([]*network.LbPolicy, error) {
	a.logger.DebugLog("msg", "received call", "object", "LbPolicy", "oper", "list")
	if options == nil {
		return nil, errors.New("invalid input")
	}
	nctx := addVersion(ctx, "v1")
	r, err := a.client.AutoListLbPolicy(nctx, options)
	if err == nil {
		return r.Items, nil
	}
	return nil, err
}

func (a *grpcObjNetworkV1LbPolicy) Watch(ctx context.Context, options *api.ListWatchOptions) (kvstore.Watcher, error) {
	a.logger.DebugLog("msg", "received call", "object", "LbPolicy", "oper", "WatchOper")
	nctx := addVersion(ctx, "v1")
	if options == nil {
		return nil, errors.New("invalid input")
	}
	stream, err := a.client.AutoWatchLbPolicy(nctx, options)
	if err != nil {
		return nil, err
	}
	wstream := stream.(network.NetworkV1_AutoWatchLbPolicyClient)
	bridgefn := func(lw *listerwatcher.WatcherClient) {
		for {
			r, err := wstream.Recv()
			if err != nil {
				a.logger.ErrorLog("msg", "error on receive", "error", err)
				close(lw.OutCh)
				return
			}
			for _, e := range r.Events {
				ev := kvstore.WatchEvent{
					Type:   kvstore.WatchEventType(e.Type),
					Object: e.Object,
				}
				select {
				case lw.OutCh <- &ev:
				case <-wstream.Context().Done():
					close(lw.OutCh)
					return
				}
			}
		}
	}
	lw := listerwatcher.NewWatcherClient(wstream, bridgefn)
	lw.Run()
	return lw, nil
}

func (a *grpcObjNetworkV1LbPolicy) Allowed(oper apiserver.APIOperType) bool {
	return true
}

type restObjNetworkV1LbPolicy struct {
	endpoints network.EndpointsNetworkV1RestClient
	instance  string
}

func (a *restObjNetworkV1LbPolicy) Create(ctx context.Context, in *network.LbPolicy) (*network.LbPolicy, error) {
	if in == nil {
		return nil, errors.New("invalid input")
	}
	return a.endpoints.AutoAddLbPolicy(ctx, in)
}

func (a *restObjNetworkV1LbPolicy) Update(ctx context.Context, in *network.LbPolicy) (*network.LbPolicy, error) {
	if in == nil {
		return nil, errors.New("invalid input")
	}
	return a.endpoints.AutoUpdateLbPolicy(ctx, in)
}

func (a *restObjNetworkV1LbPolicy) Get(ctx context.Context, objMeta *api.ObjectMeta) (*network.LbPolicy, error) {
	if objMeta == nil {
		return nil, errors.New("invalid input")
	}
	in := network.LbPolicy{}
	in.ObjectMeta = *objMeta
	return a.endpoints.AutoGetLbPolicy(ctx, &in)
}

func (a *restObjNetworkV1LbPolicy) Delete(ctx context.Context, objMeta *api.ObjectMeta) (*network.LbPolicy, error) {
	if objMeta == nil {
		return nil, errors.New("invalid input")
	}
	in := network.LbPolicy{}
	in.ObjectMeta = *objMeta
	return a.endpoints.AutoDeleteLbPolicy(ctx, &in)
}

func (a *restObjNetworkV1LbPolicy) List(ctx context.Context, options *api.ListWatchOptions) ([]*network.LbPolicy, error) {
	if options == nil {
		return nil, errors.New("invalid input")
	}

	if options.Tenant == "" {
		options.Tenant = globals.DefaultTenant
	}
	r, err := a.endpoints.AutoListLbPolicy(ctx, options)
	if err == nil {
		return r.Items, nil
	}
	return nil, err
}

func (a *restObjNetworkV1LbPolicy) Watch(ctx context.Context, options *api.ListWatchOptions) (kvstore.Watcher, error) {
	if options == nil {
		return nil, errors.New("invalid input")
	}
	return a.endpoints.AutoWatchLbPolicy(ctx, options)
}

func (a *restObjNetworkV1LbPolicy) Allowed(oper apiserver.APIOperType) bool {
	switch oper {
	case apiserver.CreateOper:
		return true
	case apiserver.UpdateOper:
		return true
	case apiserver.GetOper:
		return true
	case apiserver.DeleteOper:
		return true
	case apiserver.ListOper:
		return true
	case apiserver.WatchOper:
		return true
	default:
		return false
	}
}

type crudClientNetworkV1 struct {
	logger log.Logger
	client network.ServiceNetworkV1Client

	grpcNetwork  network.NetworkV1NetworkInterface
	grpcService  network.NetworkV1ServiceInterface
	grpcLbPolicy network.NetworkV1LbPolicyInterface
}

// NewGrpcCrudClientNetworkV1 creates a GRPC client for the service
func NewGrpcCrudClientNetworkV1(conn *grpc.ClientConn, logger log.Logger) network.NetworkV1Interface {
	client := NewNetworkV1Backend(conn, logger)
	return &crudClientNetworkV1{
		logger: logger,
		client: client,

		grpcNetwork:  &grpcObjNetworkV1Network{client: client, logger: logger},
		grpcService:  &grpcObjNetworkV1Service{client: client, logger: logger},
		grpcLbPolicy: &grpcObjNetworkV1LbPolicy{client: client, logger: logger},
	}
}

func (a *crudClientNetworkV1) Network() network.NetworkV1NetworkInterface {
	return a.grpcNetwork
}

func (a *crudClientNetworkV1) Service() network.NetworkV1ServiceInterface {
	return a.grpcService
}

func (a *crudClientNetworkV1) LbPolicy() network.NetworkV1LbPolicyInterface {
	return a.grpcLbPolicy
}

func (a *crudClientNetworkV1) Watch(ctx context.Context, options *api.ListWatchOptions) (kvstore.Watcher, error) {
	a.logger.DebugLog("msg", "received call", "object", "NetworkV1", "oper", "WatchOper")
	nctx := addVersion(ctx, "v1")
	if options == nil {
		return nil, errors.New("invalid input")
	}
	stream, err := a.client.AutoWatchSvcNetworkV1(nctx, options)
	if err != nil {
		return nil, err
	}
	wstream := stream.(network.NetworkV1_AutoWatchSvcNetworkV1Client)
	bridgefn := func(lw *listerwatcher.WatcherClient) {
		for {
			r, err := wstream.Recv()
			if err != nil {
				a.logger.ErrorLog("msg", "error on receive", "error", err)
				close(lw.OutCh)
				return
			}
			for _, e := range r.Events {
				ev := kvstore.WatchEvent{Type: kvstore.WatchEventType(e.Type)}
				robj, err := listerwatcher.GetObject(e)
				if err != nil {
					a.logger.ErrorLog("msg", "error on receive unmarshall", "error", err)
					close(lw.OutCh)
					return
				}
				ev.Object = robj
				select {
				case lw.OutCh <- &ev:
				case <-wstream.Context().Done():
					close(lw.OutCh)
					return
				}
			}
		}
	}
	lw := listerwatcher.NewWatcherClient(wstream, bridgefn)
	lw.Run()
	return lw, nil
}

type crudRestClientNetworkV1 struct {
	restNetwork  network.NetworkV1NetworkInterface
	restService  network.NetworkV1ServiceInterface
	restLbPolicy network.NetworkV1LbPolicyInterface
}

// NewRestCrudClientNetworkV1 creates a REST client for the service.
func NewRestCrudClientNetworkV1(url string) network.NetworkV1Interface {
	endpoints, err := network.MakeNetworkV1RestClientEndpoints(url)
	if err != nil {
		oldlog.Fatal("failed to create client")
	}
	return &crudRestClientNetworkV1{

		restNetwork:  &restObjNetworkV1Network{endpoints: endpoints, instance: url},
		restService:  &restObjNetworkV1Service{endpoints: endpoints, instance: url},
		restLbPolicy: &restObjNetworkV1LbPolicy{endpoints: endpoints, instance: url},
	}
}

// NewStagedRestCrudClientNetworkV1 creates a REST client for the service.
func NewStagedRestCrudClientNetworkV1(url string, id string) network.NetworkV1Interface {
	endpoints, err := network.MakeNetworkV1StagedRestClientEndpoints(url, id)
	if err != nil {
		oldlog.Fatal("failed to create client")
	}
	return &crudRestClientNetworkV1{

		restNetwork:  &restObjNetworkV1Network{endpoints: endpoints, instance: url},
		restService:  &restObjNetworkV1Service{endpoints: endpoints, instance: url},
		restLbPolicy: &restObjNetworkV1LbPolicy{endpoints: endpoints, instance: url},
	}
}

func (a *crudRestClientNetworkV1) Network() network.NetworkV1NetworkInterface {
	return a.restNetwork
}

func (a *crudRestClientNetworkV1) Service() network.NetworkV1ServiceInterface {
	return a.restService
}

func (a *crudRestClientNetworkV1) LbPolicy() network.NetworkV1LbPolicyInterface {
	return a.restLbPolicy
}

func (a *crudRestClientNetworkV1) Watch(ctx context.Context, options *api.ListWatchOptions) (kvstore.Watcher, error) {
	return nil, errors.New("method unimplemented")
}
