// Code generated by protoc-gen-grpc-pensando DO NOT EDIT.
/*
 * Package restapi is a auto generated package.
 * Input file: metrics.proto
 */

package restapi

import (
	"fmt"
	"net/http"

	"github.com/fatih/structs"
	"github.com/gorilla/mux"

	"github.com/pensando/sw/nic/agent/httputils"
	"github.com/pensando/sw/nic/delphi/proto/goproto"
	_ "github.com/pensando/sw/nic/utils/ntranslate/metrics"
	"github.com/pensando/sw/venice/utils/log"
	"github.com/pensando/sw/venice/utils/tsdb"
)

// addLifMetricsAPIRoutes adds routes for LifMetrics
func addLifMetricsAPIRoutes(r *mux.Router, srv *RestServer) {
	r.Methods("GET").Subrouter().HandleFunc("/{Meta.Tenant}/{Meta.Name}/", httputils.MakeHTTPHandler(srv.getLifMetricsHandler))
	r.Methods("GET").Subrouter().HandleFunc("/", httputils.MakeHTTPHandler(srv.listLifMetricsHandler))
}

// listLifMetricsHandler is the List Handler for LifMetrics
func (s *RestServer) listLifMetricsHandler(r *http.Request) (interface{}, error) {
	iter, err := goproto.NewLifMetricsIterator()
	if err != nil {
		return nil, fmt.Errorf("failed to get metrics, error: %s", err)
	}

	// for OSX tests
	if iter == nil {
		return nil, nil
	}

	var mtr []goproto.LifMetrics

	for iter.HasNext() {
		temp := iter.Next()
		if temp == nil {
			continue
		}

		objMeta := s.keyTranslator.GetObjectMeta("LifMetricsKey", temp.GetKey())
		if objMeta == nil {
			log.Errorf("failed to get objMeta for LifMetrics key %+v", temp.GetKey())
			continue
		}

		temp.ObjectMeta = *objMeta
		mtr = append(mtr, *temp)
	}
	iter.Free()
	return mtr, nil
}

// getLifMetricsPoints returns tags and fields to save in Venice TSDB
func (s *RestServer) getLifMetricsPoints() ([]*tsdb.Point, error) {
	iter, err := goproto.NewLifMetricsIterator()
	if err != nil {
		return nil, fmt.Errorf("failed to get metrics, error: %s", err)
	}

	// for OSX tests
	if iter == nil {
		return nil, nil
	}

	points := []*tsdb.Point{}

	for iter.HasNext() {
		m := iter.Next()
		if m == nil {
			continue
		}

		// translate key to meta
		objMeta := s.keyTranslator.GetObjectMeta("LifMetricsKey", m.GetKey())
		if objMeta == nil {
			log.Errorf("failed to get objMeta for LifMetrics key %+v", m.GetKey())
			continue
		}
		tags := s.getTagsFromMeta(objMeta)
		fields := structs.Map(m)

		if len(fields) > 0 {
			delete(fields, "ObjectMeta")
			points = append(points, &tsdb.Point{Tags: tags, Fields: fields})
		}
	}

	iter.Free()
	return points, nil
}

// getLifMetricsHandler is the Get Handler for LifMetrics
func (s *RestServer) getLifMetricsHandler(r *http.Request) (interface{}, error) {
	log.Infof("Got GET request LifMetrics/%s", mux.Vars(r)["Meta.Name"])
	return nil, nil
}
