// Code generated by protoc-gen-grpc-pensando DO NOT EDIT.

/*
Package monitoringApiServer is a auto generated package.
Input file: troubleshooting.proto
*/
package monitoringApiServer

import (
	"context"
	"fmt"
	"time"

	"github.com/gogo/protobuf/types"
	"github.com/satori/go.uuid"

	"github.com/pensando/sw/api"
	monitoring "github.com/pensando/sw/api/generated/monitoring"
	"github.com/pensando/sw/api/listerwatcher"
	"github.com/pensando/sw/venice/apiserver"
	"github.com/pensando/sw/venice/apiserver/pkg"
	"github.com/pensando/sw/venice/utils/kvstore"
	"github.com/pensando/sw/venice/utils/log"
	"github.com/pensando/sw/venice/utils/rpckit"
	"github.com/pensando/sw/venice/utils/runtime"
)

// dummy vars to suppress unused errors
var _ api.ObjectMeta
var _ listerwatcher.WatcherClient
var _ fmt.Stringer

type smonitoringTroubleshootingBackend struct {
	Services map[string]apiserver.Service
	Messages map[string]apiserver.Message
	logger   log.Logger
	scheme   *runtime.Scheme
}

func (s *smonitoringTroubleshootingBackend) regMsgsFunc(l log.Logger, scheme *runtime.Scheme) {
	l.Infof("registering message for smonitoringTroubleshootingBackend")
	s.Messages = map[string]apiserver.Message{

		"monitoring.PingPktStats":   apisrvpkg.NewMessage("monitoring.PingPktStats"),
		"monitoring.PingStats":      apisrvpkg.NewMessage("monitoring.PingStats"),
		"monitoring.TimeWindow":     apisrvpkg.NewMessage("monitoring.TimeWindow"),
		"monitoring.TraceRouteInfo": apisrvpkg.NewMessage("monitoring.TraceRouteInfo"),
		"monitoring.TroubleshootingSession": apisrvpkg.NewMessage("monitoring.TroubleshootingSession").WithKeyGenerator(func(i interface{}, prefix string) string {
			if i == nil {
				r := monitoring.TroubleshootingSession{}
				return r.MakeKey(prefix)
			}
			r := i.(monitoring.TroubleshootingSession)
			return r.MakeKey(prefix)
		}).WithObjectVersionWriter(func(i interface{}, version string) interface{} {
			r := i.(monitoring.TroubleshootingSession)
			r.Kind = "TroubleshootingSession"
			r.APIVersion = version
			return r
		}).WithKvUpdater(func(ctx context.Context, kvs kvstore.Interface, i interface{}, prefix string, create, ignoreStatus bool) (interface{}, error) {
			r := i.(monitoring.TroubleshootingSession)
			key := r.MakeKey(prefix)
			r.Kind = "TroubleshootingSession"
			var err error
			if create {
				err = kvs.Create(ctx, key, &r)
				if err != nil {
					l.ErrorLog("msg", "KV create failed", "key", key, "error", err)
				}
			} else {
				if ignoreStatus {
					updateFunc := func(obj runtime.Object) (runtime.Object, error) {
						saved := obj.(*monitoring.TroubleshootingSession)
						if r.ResourceVersion != "" && r.ResourceVersion != saved.ResourceVersion {
							return nil, fmt.Errorf("Resource Version specified does not match Object version")
						}
						r.Status = saved.Status
						return &r, nil
					}
					into := &monitoring.TroubleshootingSession{}
					err = kvs.ConsistentUpdate(ctx, key, into, updateFunc)
				} else {
					if r.ResourceVersion != "" {
						l.Infof("resource version is specified %s\n", r.ResourceVersion)
						err = kvs.Update(ctx, key, &r, kvstore.Compare(kvstore.WithVersion(key), "=", r.ResourceVersion))
					} else {
						err = kvs.Update(ctx, key, &r)
					}
					if err != nil {
						l.ErrorLog("msg", "KV update failed", "key", key, "error", err)
					}
				}

			}
			return r, err
		}).WithKvTxnUpdater(func(ctx context.Context, txn kvstore.Txn, i interface{}, prefix string, create bool) error {
			r := i.(monitoring.TroubleshootingSession)
			key := r.MakeKey(prefix)
			var err error
			if create {
				err = txn.Create(key, &r)
				if err != nil {
					l.ErrorLog("msg", "KV transaction create failed", "key", key, "error", err)
				}
			} else {
				err = txn.Update(key, &r)
				if err != nil {
					l.ErrorLog("msg", "KV transaction update failed", "key", key, "error", err)
				}
			}
			return err
		}).WithUUIDWriter(func(i interface{}) (interface{}, error) {
			r := i.(monitoring.TroubleshootingSession)
			r.UUID = uuid.NewV4().String()
			return r, nil
		}).WithCreationTimeWriter(func(i interface{}) (interface{}, error) {
			r := i.(monitoring.TroubleshootingSession)
			var err error
			ts, err := types.TimestampProto(time.Now())
			if err == nil {
				r.CreationTime.Timestamp = *ts
			}
			return r, err
		}).WithModTimeWriter(func(i interface{}) (interface{}, error) {
			r := i.(monitoring.TroubleshootingSession)
			var err error
			ts, err := types.TimestampProto(time.Now())
			if err == nil {
				r.ModTime.Timestamp = *ts
			}
			return r, err
		}).WithSelfLinkWriter(func(path, ver, prefix string, i interface{}) (interface{}, error) {
			r := i.(monitoring.TroubleshootingSession)
			r.SelfLink = path
			return r, nil
		}).WithKvGetter(func(ctx context.Context, kvs kvstore.Interface, key string) (interface{}, error) {
			r := monitoring.TroubleshootingSession{}
			err := kvs.Get(ctx, key, &r)
			if err != nil {
				l.ErrorLog("msg", "Object get failed", "key", key, "error", err)
			}
			return r, err
		}).WithKvDelFunc(func(ctx context.Context, kvs kvstore.Interface, key string) (interface{}, error) {
			r := monitoring.TroubleshootingSession{}
			err := kvs.Delete(ctx, key, &r)
			if err != nil {
				l.ErrorLog("msg", "Object delete failed", "key", key, "error", err)
			}
			return r, err
		}).WithKvTxnDelFunc(func(ctx context.Context, txn kvstore.Txn, key string) error {
			err := txn.Delete(key)
			if err != nil {
				l.ErrorLog("msg", "Object Txn delete failed", "key", key, "error", err)
			}
			return err
		}).WithValidate(func(i interface{}, ver string, ignoreStatus bool) []error {
			r := i.(monitoring.TroubleshootingSession)
			return r.Validate(ver, "", ignoreStatus)
		}),

		"monitoring.TroubleshootingSessionSpec":   apisrvpkg.NewMessage("monitoring.TroubleshootingSessionSpec"),
		"monitoring.TroubleshootingSessionStatus": apisrvpkg.NewMessage("monitoring.TroubleshootingSessionStatus"),
		"monitoring.TsAuditTrail":                 apisrvpkg.NewMessage("monitoring.TsAuditTrail"),
		"monitoring.TsFlowCounters":               apisrvpkg.NewMessage("monitoring.TsFlowCounters"),
		"monitoring.TsFlowLogs":                   apisrvpkg.NewMessage("monitoring.TsFlowLogs"),
		"monitoring.TsPolicy":                     apisrvpkg.NewMessage("monitoring.TsPolicy"),
		"monitoring.TsReport":                     apisrvpkg.NewMessage("monitoring.TsReport"),
		"monitoring.TsResult":                     apisrvpkg.NewMessage("monitoring.TsResult"),
		"monitoring.TsStats":                      apisrvpkg.NewMessage("monitoring.TsStats"),
		// Add a message handler for ListWatch options
		"api.ListWatchOptions": apisrvpkg.NewMessage("api.ListWatchOptions"),
	}

	apisrv.RegisterMessages("monitoring", s.Messages)
	// add messages to package.
	if pkgMessages == nil {
		pkgMessages = make(map[string]apiserver.Message)
	}
	for k, v := range s.Messages {
		pkgMessages[k] = v
	}
}

func (s *smonitoringTroubleshootingBackend) regSvcsFunc(ctx context.Context, logger log.Logger, grpcserver *rpckit.RPCServer, scheme *runtime.Scheme) {

}

func (s *smonitoringTroubleshootingBackend) regWatchersFunc(ctx context.Context, logger log.Logger, grpcserver *rpckit.RPCServer, scheme *runtime.Scheme) {

}

func (s *smonitoringTroubleshootingBackend) CompleteRegistration(ctx context.Context, logger log.Logger,
	grpcserver *rpckit.RPCServer, scheme *runtime.Scheme) error {
	// register all messages in the package if not done already
	s.logger = logger
	s.scheme = scheme
	registerMessages(logger, scheme)
	registerServices(ctx, logger, grpcserver, scheme)
	registerWatchers(ctx, logger, grpcserver, scheme)
	return nil
}

func (s *smonitoringTroubleshootingBackend) Reset() {
	cleanupRegistration()
}

func init() {
	apisrv = apisrvpkg.MustGetAPIServer()

	svc := smonitoringTroubleshootingBackend{}
	addMsgRegFunc(svc.regMsgsFunc)
	addSvcRegFunc(svc.regSvcsFunc)
	addWatcherRegFunc(svc.regWatchersFunc)

	apisrv.Register("monitoring.troubleshooting.proto", &svc)
}
