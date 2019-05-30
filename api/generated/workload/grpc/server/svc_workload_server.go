// Code generated by protoc-gen-grpc-pensando DO NOT EDIT.

/*
Package workloadApiServer is a auto generated package.
Input file: svc_workload.proto
*/
package workloadApiServer

import (
	"context"
	"fmt"
	"strings"
	"time"

	"github.com/pkg/errors"
	"google.golang.org/grpc"

	"github.com/pensando/sw/api"
	workload "github.com/pensando/sw/api/generated/workload"
	fieldhooks "github.com/pensando/sw/api/hooks/apiserver/fields"
	"github.com/pensando/sw/api/interfaces"
	"github.com/pensando/sw/api/listerwatcher"
	"github.com/pensando/sw/api/utils"
	"github.com/pensando/sw/venice/apiserver"
	"github.com/pensando/sw/venice/apiserver/pkg"
	"github.com/pensando/sw/venice/globals"
	"github.com/pensando/sw/venice/utils/ctxutils"
	"github.com/pensando/sw/venice/utils/kvstore"
	"github.com/pensando/sw/venice/utils/log"
	"github.com/pensando/sw/venice/utils/rpckit"
	"github.com/pensando/sw/venice/utils/runtime"
)

// dummy vars to suppress unused errors
var _ api.ObjectMeta
var _ listerwatcher.WatcherClient
var _ fmt.Stringer
var _ fieldhooks.Dummy

type sworkloadSvc_workloadBackend struct {
	Services map[string]apiserver.Service
	Messages map[string]apiserver.Message
	logger   log.Logger
	scheme   *runtime.Scheme

	endpointsWorkloadV1 *eWorkloadV1Endpoints
}

type eWorkloadV1Endpoints struct {
	Svc                      sworkloadSvc_workloadBackend
	fnAutoWatchSvcWorkloadV1 func(in *api.ListWatchOptions, stream grpc.ServerStream, svcprefix string) error

	fnAutoAddEndpoint    func(ctx context.Context, t interface{}) (interface{}, error)
	fnAutoAddWorkload    func(ctx context.Context, t interface{}) (interface{}, error)
	fnAutoDeleteEndpoint func(ctx context.Context, t interface{}) (interface{}, error)
	fnAutoDeleteWorkload func(ctx context.Context, t interface{}) (interface{}, error)
	fnAutoGetEndpoint    func(ctx context.Context, t interface{}) (interface{}, error)
	fnAutoGetWorkload    func(ctx context.Context, t interface{}) (interface{}, error)
	fnAutoListEndpoint   func(ctx context.Context, t interface{}) (interface{}, error)
	fnAutoListWorkload   func(ctx context.Context, t interface{}) (interface{}, error)
	fnAutoUpdateEndpoint func(ctx context.Context, t interface{}) (interface{}, error)
	fnAutoUpdateWorkload func(ctx context.Context, t interface{}) (interface{}, error)

	fnAutoWatchEndpoint func(in *api.ListWatchOptions, stream grpc.ServerStream, svcprefix string) error
	fnAutoWatchWorkload func(in *api.ListWatchOptions, stream grpc.ServerStream, svcprefix string) error
}

func (s *sworkloadSvc_workloadBackend) regMsgsFunc(l log.Logger, scheme *runtime.Scheme) {
	l.Infof("registering message for sworkloadSvc_workloadBackend")
	s.Messages = map[string]apiserver.Message{

		"workload.AutoMsgEndpointWatchHelper": apisrvpkg.NewMessage("workload.AutoMsgEndpointWatchHelper"),
		"workload.AutoMsgWorkloadWatchHelper": apisrvpkg.NewMessage("workload.AutoMsgWorkloadWatchHelper"),
		"workload.EndpointList": apisrvpkg.NewMessage("workload.EndpointList").WithKvListFunc(func(ctx context.Context, kvs kvstore.Interface, options *api.ListWatchOptions, prefix string) (interface{}, error) {

			into := workload.EndpointList{}
			into.Kind = "EndpointList"
			r := workload.Endpoint{}
			r.ObjectMeta = options.ObjectMeta
			key := r.MakeKey(prefix)

			if options.Tenant == "" {
				if strings.HasSuffix(key, "//") {
					key = key[:len(key)-1]
				}
			}

			ctx = apiutils.SetVar(ctx, "ObjKind", "workload.Endpoint")
			err := kvs.ListFiltered(ctx, key, &into, *options)
			if err != nil {
				l.ErrorLog("msg", "Object ListFiltered failed", "key", key, "error", err)
				return nil, err
			}
			return into, nil
		}).WithSelfLinkWriter(func(path, ver, prefix string, i interface{}) (interface{}, error) {
			r := i.(workload.EndpointList)
			r.APIVersion = ver
			for i := range r.Items {
				r.Items[i].SelfLink = r.Items[i].MakeURI("configs", ver, prefix)
			}
			return r, nil
		}).WithGetRuntimeObject(func(i interface{}) runtime.Object {
			r := i.(workload.EndpointList)
			return &r
		}),
		"workload.WorkloadList": apisrvpkg.NewMessage("workload.WorkloadList").WithKvListFunc(func(ctx context.Context, kvs kvstore.Interface, options *api.ListWatchOptions, prefix string) (interface{}, error) {

			into := workload.WorkloadList{}
			into.Kind = "WorkloadList"
			r := workload.Workload{}
			r.ObjectMeta = options.ObjectMeta
			key := r.MakeKey(prefix)

			if options.Tenant == "" {
				if strings.HasSuffix(key, "//") {
					key = key[:len(key)-1]
				}
			}

			ctx = apiutils.SetVar(ctx, "ObjKind", "workload.Workload")
			err := kvs.ListFiltered(ctx, key, &into, *options)
			if err != nil {
				l.ErrorLog("msg", "Object ListFiltered failed", "key", key, "error", err)
				return nil, err
			}
			return into, nil
		}).WithSelfLinkWriter(func(path, ver, prefix string, i interface{}) (interface{}, error) {
			r := i.(workload.WorkloadList)
			r.APIVersion = ver
			for i := range r.Items {
				r.Items[i].SelfLink = r.Items[i].MakeURI("configs", ver, prefix)
			}
			return r, nil
		}).WithGetRuntimeObject(func(i interface{}) runtime.Object {
			r := i.(workload.WorkloadList)
			return &r
		}),
		// Add a message handler for ListWatch options
		"api.ListWatchOptions": apisrvpkg.NewMessage("api.ListWatchOptions"),
	}

	apisrv.RegisterMessages("workload", s.Messages)
	// add messages to package.
	if pkgMessages == nil {
		pkgMessages = make(map[string]apiserver.Message)
	}
	for k, v := range s.Messages {
		pkgMessages[k] = v
	}
}

func (s *sworkloadSvc_workloadBackend) regSvcsFunc(ctx context.Context, logger log.Logger, grpcserver *rpckit.RPCServer, scheme *runtime.Scheme) {

	{
		srv := apisrvpkg.NewService("workload.WorkloadV1")
		s.endpointsWorkloadV1.fnAutoWatchSvcWorkloadV1 = srv.WatchFromKv

		s.endpointsWorkloadV1.fnAutoAddEndpoint = srv.AddMethod("AutoAddEndpoint",
			apisrvpkg.NewMethod(srv, pkgMessages["workload.Endpoint"], pkgMessages["workload.Endpoint"], "workload", "AutoAddEndpoint")).WithOper(apiintf.CreateOper).WithVersion("v1").WithMakeURI(func(i interface{}) (string, error) {
			return "", fmt.Errorf("not rest endpoint")
		}).HandleInvocation

		s.endpointsWorkloadV1.fnAutoAddWorkload = srv.AddMethod("AutoAddWorkload",
			apisrvpkg.NewMethod(srv, pkgMessages["workload.Workload"], pkgMessages["workload.Workload"], "workload", "AutoAddWorkload")).WithOper(apiintf.CreateOper).WithVersion("v1").WithMakeURI(func(i interface{}) (string, error) {
			in, ok := i.(workload.Workload)
			if !ok {
				return "", fmt.Errorf("wrong type")
			}
			return fmt.Sprint("/", globals.ConfigURIPrefix, "/", "workload/v1/tenant/", in.Tenant, "/workloads/", in.Name), nil
		}).HandleInvocation

		s.endpointsWorkloadV1.fnAutoDeleteEndpoint = srv.AddMethod("AutoDeleteEndpoint",
			apisrvpkg.NewMethod(srv, pkgMessages["workload.Endpoint"], pkgMessages["workload.Endpoint"], "workload", "AutoDeleteEndpoint")).WithOper(apiintf.DeleteOper).WithVersion("v1").WithMakeURI(func(i interface{}) (string, error) {
			return "", fmt.Errorf("not rest endpoint")
		}).HandleInvocation

		s.endpointsWorkloadV1.fnAutoDeleteWorkload = srv.AddMethod("AutoDeleteWorkload",
			apisrvpkg.NewMethod(srv, pkgMessages["workload.Workload"], pkgMessages["workload.Workload"], "workload", "AutoDeleteWorkload")).WithOper(apiintf.DeleteOper).WithVersion("v1").WithMakeURI(func(i interface{}) (string, error) {
			in, ok := i.(workload.Workload)
			if !ok {
				return "", fmt.Errorf("wrong type")
			}
			return fmt.Sprint("/", globals.ConfigURIPrefix, "/", "workload/v1/tenant/", in.Tenant, "/workloads/", in.Name), nil
		}).HandleInvocation

		s.endpointsWorkloadV1.fnAutoGetEndpoint = srv.AddMethod("AutoGetEndpoint",
			apisrvpkg.NewMethod(srv, pkgMessages["workload.Endpoint"], pkgMessages["workload.Endpoint"], "workload", "AutoGetEndpoint")).WithOper(apiintf.GetOper).WithVersion("v1").WithMakeURI(func(i interface{}) (string, error) {
			in, ok := i.(workload.Endpoint)
			if !ok {
				return "", fmt.Errorf("wrong type")
			}
			return fmt.Sprint("/", globals.ConfigURIPrefix, "/", "workload/v1/tenant/", in.Tenant, "/endpoints/", in.Name), nil
		}).HandleInvocation

		s.endpointsWorkloadV1.fnAutoGetWorkload = srv.AddMethod("AutoGetWorkload",
			apisrvpkg.NewMethod(srv, pkgMessages["workload.Workload"], pkgMessages["workload.Workload"], "workload", "AutoGetWorkload")).WithOper(apiintf.GetOper).WithVersion("v1").WithMakeURI(func(i interface{}) (string, error) {
			in, ok := i.(workload.Workload)
			if !ok {
				return "", fmt.Errorf("wrong type")
			}
			return fmt.Sprint("/", globals.ConfigURIPrefix, "/", "workload/v1/tenant/", in.Tenant, "/workloads/", in.Name), nil
		}).HandleInvocation

		s.endpointsWorkloadV1.fnAutoListEndpoint = srv.AddMethod("AutoListEndpoint",
			apisrvpkg.NewMethod(srv, pkgMessages["api.ListWatchOptions"], pkgMessages["workload.EndpointList"], "workload", "AutoListEndpoint")).WithOper(apiintf.ListOper).WithVersion("v1").WithMakeURI(func(i interface{}) (string, error) {
			in, ok := i.(api.ListWatchOptions)
			if !ok {
				return "", fmt.Errorf("wrong type")
			}
			return fmt.Sprint("/", globals.ConfigURIPrefix, "/", "workload/v1/tenant/", in.Tenant, "/endpoints/", in.Name), nil
		}).HandleInvocation

		s.endpointsWorkloadV1.fnAutoListWorkload = srv.AddMethod("AutoListWorkload",
			apisrvpkg.NewMethod(srv, pkgMessages["api.ListWatchOptions"], pkgMessages["workload.WorkloadList"], "workload", "AutoListWorkload")).WithOper(apiintf.ListOper).WithVersion("v1").WithMakeURI(func(i interface{}) (string, error) {
			in, ok := i.(api.ListWatchOptions)
			if !ok {
				return "", fmt.Errorf("wrong type")
			}
			return fmt.Sprint("/", globals.ConfigURIPrefix, "/", "workload/v1/tenant/", in.Tenant, "/workloads/", in.Name), nil
		}).HandleInvocation

		s.endpointsWorkloadV1.fnAutoUpdateEndpoint = srv.AddMethod("AutoUpdateEndpoint",
			apisrvpkg.NewMethod(srv, pkgMessages["workload.Endpoint"], pkgMessages["workload.Endpoint"], "workload", "AutoUpdateEndpoint")).WithOper(apiintf.UpdateOper).WithVersion("v1").WithMakeURI(func(i interface{}) (string, error) {
			return "", fmt.Errorf("not rest endpoint")
		}).HandleInvocation

		s.endpointsWorkloadV1.fnAutoUpdateWorkload = srv.AddMethod("AutoUpdateWorkload",
			apisrvpkg.NewMethod(srv, pkgMessages["workload.Workload"], pkgMessages["workload.Workload"], "workload", "AutoUpdateWorkload")).WithOper(apiintf.UpdateOper).WithVersion("v1").WithMakeURI(func(i interface{}) (string, error) {
			in, ok := i.(workload.Workload)
			if !ok {
				return "", fmt.Errorf("wrong type")
			}
			return fmt.Sprint("/", globals.ConfigURIPrefix, "/", "workload/v1/tenant/", in.Tenant, "/workloads/", in.Name), nil
		}).HandleInvocation

		s.endpointsWorkloadV1.fnAutoWatchEndpoint = pkgMessages["workload.Endpoint"].WatchFromKv

		s.endpointsWorkloadV1.fnAutoWatchWorkload = pkgMessages["workload.Workload"].WatchFromKv

		s.Services = map[string]apiserver.Service{
			"workload.WorkloadV1": srv,
		}
		apisrv.RegisterService("workload.WorkloadV1", srv)
		endpoints := workload.MakeWorkloadV1ServerEndpoints(s.endpointsWorkloadV1, logger)
		server := workload.MakeGRPCServerWorkloadV1(ctx, endpoints, logger)
		workload.RegisterWorkloadV1Server(grpcserver.GrpcServer, server)
		svcObjs := []string{"Endpoint", "Workload"}
		fieldhooks.RegisterImmutableFieldsServiceHooks("workload", "WorkloadV1", svcObjs)
	}
}

func (s *sworkloadSvc_workloadBackend) regWatchersFunc(ctx context.Context, logger log.Logger, grpcserver *rpckit.RPCServer, scheme *runtime.Scheme) {

	// Add Watchers
	{

		// Service watcher
		svc := s.Services["workload.WorkloadV1"]
		if svc != nil {
			svc.WithKvWatchFunc(func(l log.Logger, options *api.ListWatchOptions, kvs kvstore.Interface, stream interface{}, txfnMap map[string]func(from, to string, i interface{}) (interface{}, error), version, svcprefix string) error {
				key := globals.ConfigRootPrefix + "/workload"
				wstream := stream.(grpc.ServerStream)
				nctx, cancel := context.WithCancel(wstream.Context())
				defer cancel()
				watcher, err := kvs.WatchFiltered(nctx, key, *options)
				if err != nil {
					l.ErrorLog("msg", "error starting Watch for service", "error", err, "service", "WorkloadV1")
					return err
				}
				return listerwatcher.SvcWatch(nctx, watcher, wstream, txfnMap, version, l)
			})
		}

		pkgMessages["workload.Endpoint"].WithKvWatchFunc(func(l log.Logger, options *api.ListWatchOptions, kvs kvstore.Interface, stream interface{}, txfn func(from, to string, i interface{}) (interface{}, error), version, svcprefix string) error {
			o := workload.Endpoint{}
			key := o.MakeKey(svcprefix)
			if strings.HasSuffix(key, "//") {
				key = strings.TrimSuffix(key, "/")
			}
			wstream := stream.(workload.WorkloadV1_AutoWatchEndpointServer)
			nctx, cancel := context.WithCancel(wstream.Context())
			defer cancel()
			id := fmt.Sprintf("%s-%x", ctxutils.GetPeerID(nctx), &key)

			nctx = ctxutils.SetContextID(nctx, id)
			if kvs == nil {
				return fmt.Errorf("Nil KVS")
			}
			nctx = apiutils.SetVar(nctx, "ObjKind", "workload.Endpoint")
			l.InfoLog("msg", "KVWatcher starting watch", "WatcherID", id, "object", "workload.Endpoint")
			watcher, err := kvs.WatchFiltered(nctx, key, *options)
			if err != nil {
				l.ErrorLog("msg", "error starting Watch on KV", "error", err, "WatcherID", id, "bbject", "workload.Endpoint")
				return err
			}
			timer := time.NewTimer(apiserver.DefaultWatchHoldInterval)
			if !timer.Stop() {
				<-timer.C
			}
			running := false
			events := &workload.AutoMsgEndpointWatchHelper{}
			sendToStream := func() error {
				l.DebugLog("msg", "writing to stream", "len", len(events.Events))
				if err := wstream.Send(events); err != nil {
					l.ErrorLog("msg", "Stream send error'ed for Order", "error", err, "WatcherID", id, "bbject", "workload.Endpoint")
					return err
				}
				events = &workload.AutoMsgEndpointWatchHelper{}
				return nil
			}
			defer l.InfoLog("msg", "exiting watcher", "service", "workload.Endpoint")
			for {
				select {
				case ev, ok := <-watcher.EventChan():
					if !ok {
						l.ErrorLog("msg", "Channel closed for Watcher", "WatcherID", id, "bbject", "workload.Endpoint")
						return nil
					}
					evin, ok := ev.Object.(*workload.Endpoint)
					if !ok {
						status, ok := ev.Object.(*api.Status)
						if !ok {
							return errors.New("unknown error")
						}
						return fmt.Errorf("%v:(%s) %s", status.Code, status.Result, status.Message)
					}
					// XXX-TODO(sanjayt): Avoid a copy and update selflink at enqueue.
					cin, err := evin.Clone(nil)
					if err != nil {
						return fmt.Errorf("unable to clone object (%s)", err)
					}
					in := cin.(*workload.Endpoint)
					in.SelfLink = in.MakeURI(globals.ConfigURIPrefix, "v1", "workload")

					strEvent := &workload.AutoMsgEndpointWatchHelper_WatchEvent{
						Type:   string(ev.Type),
						Object: in,
					}
					l.DebugLog("msg", "received Endpoint watch event from KV", "type", ev.Type)
					if version != in.APIVersion {
						i, err := txfn(in.APIVersion, version, in)
						if err != nil {
							l.ErrorLog("msg", "Failed to transform message", "type", "Endpoint", "fromver", in.APIVersion, "tover", version, "WatcherID", id, "bbject", "workload.Endpoint")
							break
						}
						strEvent.Object = i.(*workload.Endpoint)
					}
					events.Events = append(events.Events, strEvent)
					if !running {
						running = true
						timer.Reset(apiserver.DefaultWatchHoldInterval)
					}
					if len(events.Events) >= apiserver.DefaultWatchBatchSize {
						if err = sendToStream(); err != nil {
							return err
						}
						if !timer.Stop() {
							<-timer.C
						}
						timer.Reset(apiserver.DefaultWatchHoldInterval)
					}
				case <-timer.C:
					running = false
					if err = sendToStream(); err != nil {
						return err
					}
				case <-nctx.Done():
					l.DebugLog("msg", "Context cancelled for Watcher", "WatcherID", id, "bbject", "workload.Endpoint")
					return wstream.Context().Err()
				}
			}
		})

		pkgMessages["workload.Workload"].WithKvWatchFunc(func(l log.Logger, options *api.ListWatchOptions, kvs kvstore.Interface, stream interface{}, txfn func(from, to string, i interface{}) (interface{}, error), version, svcprefix string) error {
			o := workload.Workload{}
			key := o.MakeKey(svcprefix)
			if strings.HasSuffix(key, "//") {
				key = strings.TrimSuffix(key, "/")
			}
			wstream := stream.(workload.WorkloadV1_AutoWatchWorkloadServer)
			nctx, cancel := context.WithCancel(wstream.Context())
			defer cancel()
			id := fmt.Sprintf("%s-%x", ctxutils.GetPeerID(nctx), &key)

			nctx = ctxutils.SetContextID(nctx, id)
			if kvs == nil {
				return fmt.Errorf("Nil KVS")
			}
			nctx = apiutils.SetVar(nctx, "ObjKind", "workload.Workload")
			l.InfoLog("msg", "KVWatcher starting watch", "WatcherID", id, "object", "workload.Workload")
			watcher, err := kvs.WatchFiltered(nctx, key, *options)
			if err != nil {
				l.ErrorLog("msg", "error starting Watch on KV", "error", err, "WatcherID", id, "bbject", "workload.Workload")
				return err
			}
			timer := time.NewTimer(apiserver.DefaultWatchHoldInterval)
			if !timer.Stop() {
				<-timer.C
			}
			running := false
			events := &workload.AutoMsgWorkloadWatchHelper{}
			sendToStream := func() error {
				l.DebugLog("msg", "writing to stream", "len", len(events.Events))
				if err := wstream.Send(events); err != nil {
					l.ErrorLog("msg", "Stream send error'ed for Order", "error", err, "WatcherID", id, "bbject", "workload.Workload")
					return err
				}
				events = &workload.AutoMsgWorkloadWatchHelper{}
				return nil
			}
			defer l.InfoLog("msg", "exiting watcher", "service", "workload.Workload")
			for {
				select {
				case ev, ok := <-watcher.EventChan():
					if !ok {
						l.ErrorLog("msg", "Channel closed for Watcher", "WatcherID", id, "bbject", "workload.Workload")
						return nil
					}
					evin, ok := ev.Object.(*workload.Workload)
					if !ok {
						status, ok := ev.Object.(*api.Status)
						if !ok {
							return errors.New("unknown error")
						}
						return fmt.Errorf("%v:(%s) %s", status.Code, status.Result, status.Message)
					}
					// XXX-TODO(sanjayt): Avoid a copy and update selflink at enqueue.
					cin, err := evin.Clone(nil)
					if err != nil {
						return fmt.Errorf("unable to clone object (%s)", err)
					}
					in := cin.(*workload.Workload)
					in.SelfLink = in.MakeURI(globals.ConfigURIPrefix, "v1", "workload")

					strEvent := &workload.AutoMsgWorkloadWatchHelper_WatchEvent{
						Type:   string(ev.Type),
						Object: in,
					}
					l.DebugLog("msg", "received Workload watch event from KV", "type", ev.Type)
					if version != in.APIVersion {
						i, err := txfn(in.APIVersion, version, in)
						if err != nil {
							l.ErrorLog("msg", "Failed to transform message", "type", "Workload", "fromver", in.APIVersion, "tover", version, "WatcherID", id, "bbject", "workload.Workload")
							break
						}
						strEvent.Object = i.(*workload.Workload)
					}
					events.Events = append(events.Events, strEvent)
					if !running {
						running = true
						timer.Reset(apiserver.DefaultWatchHoldInterval)
					}
					if len(events.Events) >= apiserver.DefaultWatchBatchSize {
						if err = sendToStream(); err != nil {
							return err
						}
						if !timer.Stop() {
							<-timer.C
						}
						timer.Reset(apiserver.DefaultWatchHoldInterval)
					}
				case <-timer.C:
					running = false
					if err = sendToStream(); err != nil {
						return err
					}
				case <-nctx.Done():
					l.DebugLog("msg", "Context cancelled for Watcher", "WatcherID", id, "bbject", "workload.Workload")
					return wstream.Context().Err()
				}
			}
		})

	}

}

func (s *sworkloadSvc_workloadBackend) CompleteRegistration(ctx context.Context, logger log.Logger,
	grpcserver *rpckit.RPCServer, scheme *runtime.Scheme) error {
	// register all messages in the package if not done already
	s.logger = logger
	s.scheme = scheme
	registerMessages(logger, scheme)
	registerServices(ctx, logger, grpcserver, scheme)
	registerWatchers(ctx, logger, grpcserver, scheme)
	return nil
}

func (s *sworkloadSvc_workloadBackend) Reset() {
	cleanupRegistration()
}

func (e *eWorkloadV1Endpoints) AutoAddEndpoint(ctx context.Context, t workload.Endpoint) (workload.Endpoint, error) {
	r, err := e.fnAutoAddEndpoint(ctx, t)
	if err == nil {
		return r.(workload.Endpoint), err
	}
	return workload.Endpoint{}, err

}
func (e *eWorkloadV1Endpoints) AutoAddWorkload(ctx context.Context, t workload.Workload) (workload.Workload, error) {
	r, err := e.fnAutoAddWorkload(ctx, t)
	if err == nil {
		return r.(workload.Workload), err
	}
	return workload.Workload{}, err

}
func (e *eWorkloadV1Endpoints) AutoDeleteEndpoint(ctx context.Context, t workload.Endpoint) (workload.Endpoint, error) {
	r, err := e.fnAutoDeleteEndpoint(ctx, t)
	if err == nil {
		return r.(workload.Endpoint), err
	}
	return workload.Endpoint{}, err

}
func (e *eWorkloadV1Endpoints) AutoDeleteWorkload(ctx context.Context, t workload.Workload) (workload.Workload, error) {
	r, err := e.fnAutoDeleteWorkload(ctx, t)
	if err == nil {
		return r.(workload.Workload), err
	}
	return workload.Workload{}, err

}
func (e *eWorkloadV1Endpoints) AutoGetEndpoint(ctx context.Context, t workload.Endpoint) (workload.Endpoint, error) {
	r, err := e.fnAutoGetEndpoint(ctx, t)
	if err == nil {
		return r.(workload.Endpoint), err
	}
	return workload.Endpoint{}, err

}
func (e *eWorkloadV1Endpoints) AutoGetWorkload(ctx context.Context, t workload.Workload) (workload.Workload, error) {
	r, err := e.fnAutoGetWorkload(ctx, t)
	if err == nil {
		return r.(workload.Workload), err
	}
	return workload.Workload{}, err

}
func (e *eWorkloadV1Endpoints) AutoListEndpoint(ctx context.Context, t api.ListWatchOptions) (workload.EndpointList, error) {
	r, err := e.fnAutoListEndpoint(ctx, t)
	if err == nil {
		return r.(workload.EndpointList), err
	}
	return workload.EndpointList{}, err

}
func (e *eWorkloadV1Endpoints) AutoListWorkload(ctx context.Context, t api.ListWatchOptions) (workload.WorkloadList, error) {
	r, err := e.fnAutoListWorkload(ctx, t)
	if err == nil {
		return r.(workload.WorkloadList), err
	}
	return workload.WorkloadList{}, err

}
func (e *eWorkloadV1Endpoints) AutoUpdateEndpoint(ctx context.Context, t workload.Endpoint) (workload.Endpoint, error) {
	r, err := e.fnAutoUpdateEndpoint(ctx, t)
	if err == nil {
		return r.(workload.Endpoint), err
	}
	return workload.Endpoint{}, err

}
func (e *eWorkloadV1Endpoints) AutoUpdateWorkload(ctx context.Context, t workload.Workload) (workload.Workload, error) {
	r, err := e.fnAutoUpdateWorkload(ctx, t)
	if err == nil {
		return r.(workload.Workload), err
	}
	return workload.Workload{}, err

}

func (e *eWorkloadV1Endpoints) AutoWatchEndpoint(in *api.ListWatchOptions, stream workload.WorkloadV1_AutoWatchEndpointServer) error {
	return e.fnAutoWatchEndpoint(in, stream, "workload")
}
func (e *eWorkloadV1Endpoints) AutoWatchWorkload(in *api.ListWatchOptions, stream workload.WorkloadV1_AutoWatchWorkloadServer) error {
	return e.fnAutoWatchWorkload(in, stream, "workload")
}
func (e *eWorkloadV1Endpoints) AutoWatchSvcWorkloadV1(in *api.ListWatchOptions, stream workload.WorkloadV1_AutoWatchSvcWorkloadV1Server) error {
	return e.fnAutoWatchSvcWorkloadV1(in, stream, "")
}

func init() {
	apisrv = apisrvpkg.MustGetAPIServer()

	svc := sworkloadSvc_workloadBackend{}
	addMsgRegFunc(svc.regMsgsFunc)
	addSvcRegFunc(svc.regSvcsFunc)
	addWatcherRegFunc(svc.regWatchersFunc)

	{
		e := eWorkloadV1Endpoints{Svc: svc}
		svc.endpointsWorkloadV1 = &e
	}
	apisrv.Register("workload.svc_workload.proto", &svc)
}
