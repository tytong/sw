// Code generated by protoc-gen-grpc-pensando DO NOT EDIT.
/*
 * Package restapi is a auto generated package.
 * Input file: accel_metrics.proto
 */

package restapi

import (
	"net/http"

	"github.com/gorilla/mux"

	"github.com/pensando/sw/nic/agent/httputils"
	"github.com/pensando/sw/nic/delphi/proto/goproto"
	_ "github.com/pensando/sw/nic/utils/ntranslate/accel_metrics"
	"github.com/pensando/sw/venice/utils/log"
	"github.com/pensando/sw/venice/utils/ntranslate"
)

func init() {
	name := "/telemetry/v1/metrics/accelhwringmetrics/"
	if prefixRoutes == nil {
		prefixRoutes = make(map[string]routeAddFunc)
	}
	prefixRoutes[name] = addAccelHwRingMetricsAPIRoutes
}

// addAccelHwRingMetricsAPIRoutes adds routes for AccelHwRingMetrics
func addAccelHwRingMetricsAPIRoutes(r *mux.Router, srv *RestServer) {
	r.Methods("GET").Subrouter().HandleFunc("/{Meta.Tenant}/{Meta.Name}/", httputils.MakeHTTPHandler(srv.runAccelHwRingMetricsGetHandler))
	r.Methods("GET").Subrouter().HandleFunc("/", httputils.MakeHTTPHandler(srv.runAccelHwRingMetricsListHandler))
}

// runAccelHwRingMetricsListHandler is the List Handler for AccelHwRingMetrics
func (s *RestServer) runAccelHwRingMetricsListHandler(r *http.Request) (interface{}, error) {
	iter, err := goproto.NewAccelHwRingMetricsIterator()
	if err != nil {
		log.Infof("Error: %s", err)
	}
	var mtr []goproto.AccelHwRingMetrics
	tstr := ntranslate.MustGetTranslator()
	for iter.HasNext() {
		temp := iter.Next()
		temp.ObjectMeta = *(tstr.GetObjectMeta("AccelHwRingMetricsKey", temp.GetKey()))
		mtr = append(mtr, *temp)
		log.Infof("New AccelHwRingMetrics: %+v", *temp)
	}
	log.Infof("Got GET LIST request")
	return mtr, nil
}

// runAccelHwRingMetricsGetHandler is the Get Handler for AccelHwRingMetrics
func (s *RestServer) runAccelHwRingMetricsGetHandler(r *http.Request) (interface{}, error) {
	log.Infof("Got GET request AccelHwRingMetrics/%s", mux.Vars(r)["Meta.Name"])
	return nil, nil
}

func init() {
	name := "/telemetry/v1/metrics/accelseqqueuemetrics/"
	if prefixRoutes == nil {
		prefixRoutes = make(map[string]routeAddFunc)
	}
	prefixRoutes[name] = addAccelSeqQueueMetricsAPIRoutes
}

// addAccelSeqQueueMetricsAPIRoutes adds routes for AccelSeqQueueMetrics
func addAccelSeqQueueMetricsAPIRoutes(r *mux.Router, srv *RestServer) {
	r.Methods("GET").Subrouter().HandleFunc("/{Meta.Tenant}/{Meta.Name}/", httputils.MakeHTTPHandler(srv.runAccelSeqQueueMetricsGetHandler))
	r.Methods("GET").Subrouter().HandleFunc("/", httputils.MakeHTTPHandler(srv.runAccelSeqQueueMetricsListHandler))
}

// runAccelSeqQueueMetricsListHandler is the List Handler for AccelSeqQueueMetrics
func (s *RestServer) runAccelSeqQueueMetricsListHandler(r *http.Request) (interface{}, error) {
	iter, err := goproto.NewAccelSeqQueueMetricsIterator()
	if err != nil {
		log.Infof("Error: %s", err)
	}
	var mtr []goproto.AccelSeqQueueMetrics
	tstr := ntranslate.MustGetTranslator()
	for iter.HasNext() {
		temp := iter.Next()
		temp.ObjectMeta = *(tstr.GetObjectMeta("AccelSeqQueueMetricsKey", temp.GetKey()))
		mtr = append(mtr, *temp)
		log.Infof("New AccelSeqQueueMetrics: %+v", *temp)
	}
	log.Infof("Got GET LIST request")
	return mtr, nil
}

// runAccelSeqQueueMetricsGetHandler is the Get Handler for AccelSeqQueueMetrics
func (s *RestServer) runAccelSeqQueueMetricsGetHandler(r *http.Request) (interface{}, error) {
	log.Infof("Got GET request AccelSeqQueueMetrics/%s", mux.Vars(r)["Meta.Name"])
	return nil, nil
}
