// Code generated by protoc-gen-grpc-pensando DO NOT EDIT.

/*
Package metrics_queryApiServer is a auto generated package.
Input file: metrics_query.proto
*/
package metrics_queryApiServer

import (
	"context"
	"fmt"

	"github.com/pensando/sw/api"
	"github.com/pensando/sw/api/listerwatcher"
	"github.com/pensando/sw/venice/apiserver"
	"github.com/pensando/sw/venice/apiserver/pkg"
	"github.com/pensando/sw/venice/utils/log"
	"github.com/pensando/sw/venice/utils/rpckit"
	"github.com/pensando/sw/venice/utils/runtime"
)

// dummy vars to suppress unused errors
var _ api.ObjectMeta
var _ listerwatcher.WatcherClient
var _ fmt.Stringer

type smetrics_queryMetrics_queryBackend struct {
	Services map[string]apiserver.Service
	Messages map[string]apiserver.Message
	logger   log.Logger
	scheme   *runtime.Scheme
}

func (s *smetrics_queryMetrics_queryBackend) regMsgsFunc(l log.Logger, scheme *runtime.Scheme) {
	l.Infof("registering message for smetrics_queryMetrics_queryBackend")
	s.Messages = map[string]apiserver.Message{

		"metrics_query.MetricSpec":     apisrvpkg.NewMessage("metrics_query.MetricSpec"),
		"metrics_query.ObjectSelector": apisrvpkg.NewMessage("metrics_query.ObjectSelector"),
		"metrics_query.PaginationSpec": apisrvpkg.NewMessage("metrics_query.PaginationSpec"),
		"metrics_query.QueryResponse":  apisrvpkg.NewMessage("metrics_query.QueryResponse"),
		"metrics_query.QueryResult":    apisrvpkg.NewMessage("metrics_query.QueryResult"),
		"metrics_query.QuerySpec":      apisrvpkg.NewMessage("metrics_query.QuerySpec"),
		"metrics_query.ResultSeries":   apisrvpkg.NewMessage("metrics_query.ResultSeries"),
		"metrics_query.TimeRange":      apisrvpkg.NewMessage("metrics_query.TimeRange"),
		// Add a message handler for ListWatch options
		"api.ListWatchOptions": apisrvpkg.NewMessage("api.ListWatchOptions"),
	}

	apisrv.RegisterMessages("metrics_query", s.Messages)
	// add messages to package.
	if pkgMessages == nil {
		pkgMessages = make(map[string]apiserver.Message)
	}
	for k, v := range s.Messages {
		pkgMessages[k] = v
	}
}

func (s *smetrics_queryMetrics_queryBackend) regSvcsFunc(ctx context.Context, logger log.Logger, grpcserver *rpckit.RPCServer, scheme *runtime.Scheme) {

}

func (s *smetrics_queryMetrics_queryBackend) regWatchersFunc(ctx context.Context, logger log.Logger, grpcserver *rpckit.RPCServer, scheme *runtime.Scheme) {

}

func (s *smetrics_queryMetrics_queryBackend) CompleteRegistration(ctx context.Context, logger log.Logger,
	grpcserver *rpckit.RPCServer, scheme *runtime.Scheme) error {
	// register all messages in the package if not done already
	s.logger = logger
	s.scheme = scheme
	registerMessages(logger, scheme)
	registerServices(ctx, logger, grpcserver, scheme)
	registerWatchers(ctx, logger, grpcserver, scheme)
	return nil
}

func (s *smetrics_queryMetrics_queryBackend) Reset() {
	cleanupRegistration()
}

func init() {
	apisrv = apisrvpkg.MustGetAPIServer()

	svc := smetrics_queryMetrics_queryBackend{}
	addMsgRegFunc(svc.regMsgsFunc)
	addSvcRegFunc(svc.regSvcsFunc)
	addWatcherRegFunc(svc.regWatchersFunc)

	apisrv.Register("metrics_query.metrics_query.proto", &svc)
}
