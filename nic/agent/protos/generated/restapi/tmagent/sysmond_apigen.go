// Code generated by protoc-gen-grpc-pensando DO NOT EDIT.
/*
 * Package restapi is a auto generated package.
 * Input file: sysmond.proto
 */

package restapi

import (
	"fmt"
	"net/http"

	"github.com/fatih/structs"
	"github.com/gorilla/mux"

	"github.com/pensando/sw/nic/agent/httputils"
	"github.com/pensando/sw/nic/delphi/proto/goproto"
	_ "github.com/pensando/sw/nic/utils/ntranslate/sysmond"
	"github.com/pensando/sw/venice/utils/log"
	"github.com/pensando/sw/venice/utils/tsdb"
)

// AddAsicFrequencyMetricsAPIRoutes adds routes for AsicFrequencyMetrics
func (s *RestServer) AddAsicFrequencyMetricsAPIRoutes(r *mux.Router) {
	r.Methods("GET").Subrouter().HandleFunc("/{Meta.Tenant}/{Meta.Name}/", httputils.MakeHTTPHandler(s.getAsicFrequencyMetricsHandler))
	r.Methods("GET").Subrouter().HandleFunc("/", httputils.MakeHTTPHandler(s.listAsicFrequencyMetricsHandler))
}

// listAsicFrequencyMetricsHandler is the List Handler for AsicFrequencyMetrics
func (s *RestServer) listAsicFrequencyMetricsHandler(r *http.Request) (interface{}, error) {
	iter, err := goproto.NewAsicFrequencyMetricsIterator()
	if err != nil {
		return nil, fmt.Errorf("failed to get metrics, error: %s", err)
	}

	// for OSX tests
	if iter == nil {
		return nil, nil
	}

	var mtr []goproto.AsicFrequencyMetrics

	for iter.HasNext() {
		temp := iter.Next()
		if temp == nil {
			continue
		}

		objMeta := s.GetObjectMeta("AsicFrequencyMetricsKey", temp.GetKey())
		if objMeta == nil {
			log.Errorf("failed to get objMeta for AsicFrequencyMetrics key %+v", temp.GetKey())
			continue
		}

		temp.ObjectMeta = *objMeta
		mtr = append(mtr, *temp)
	}
	iter.Free()
	return mtr, nil
}

// getAsicFrequencyMetricsPoints returns tags and fields to save in Venice TSDB
func (s *RestServer) getAsicFrequencyMetricsPoints() ([]*tsdb.Point, error) {
	iter, err := goproto.NewAsicFrequencyMetricsIterator()
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
		objMeta := s.GetObjectMeta("AsicFrequencyMetricsKey", m.GetKey())
		if objMeta == nil {
			log.Errorf("failed to get objMeta for AsicFrequencyMetrics key %+v", m.GetKey())
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

// getAsicFrequencyMetricsHandler is the Get Handler for AsicFrequencyMetrics
func (s *RestServer) getAsicFrequencyMetricsHandler(r *http.Request) (interface{}, error) {
	log.Infof("Got GET request AsicFrequencyMetrics/%s", mux.Vars(r)["Meta.Name"])
	return nil, nil
}

// AddAsicPowerMetricsAPIRoutes adds routes for AsicPowerMetrics
func (s *RestServer) AddAsicPowerMetricsAPIRoutes(r *mux.Router) {
	r.Methods("GET").Subrouter().HandleFunc("/{Meta.Tenant}/{Meta.Name}/", httputils.MakeHTTPHandler(s.getAsicPowerMetricsHandler))
	r.Methods("GET").Subrouter().HandleFunc("/", httputils.MakeHTTPHandler(s.listAsicPowerMetricsHandler))
}

// listAsicPowerMetricsHandler is the List Handler for AsicPowerMetrics
func (s *RestServer) listAsicPowerMetricsHandler(r *http.Request) (interface{}, error) {
	iter, err := goproto.NewAsicPowerMetricsIterator()
	if err != nil {
		return nil, fmt.Errorf("failed to get metrics, error: %s", err)
	}

	// for OSX tests
	if iter == nil {
		return nil, nil
	}

	var mtr []goproto.AsicPowerMetrics

	for iter.HasNext() {
		temp := iter.Next()
		if temp == nil {
			continue
		}

		objMeta := s.GetObjectMeta("AsicPowerMetricsKey", temp.GetKey())
		if objMeta == nil {
			log.Errorf("failed to get objMeta for AsicPowerMetrics key %+v", temp.GetKey())
			continue
		}

		temp.ObjectMeta = *objMeta
		mtr = append(mtr, *temp)
	}
	iter.Free()
	return mtr, nil
}

// getAsicPowerMetricsPoints returns tags and fields to save in Venice TSDB
func (s *RestServer) getAsicPowerMetricsPoints() ([]*tsdb.Point, error) {
	iter, err := goproto.NewAsicPowerMetricsIterator()
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
		objMeta := s.GetObjectMeta("AsicPowerMetricsKey", m.GetKey())
		if objMeta == nil {
			log.Errorf("failed to get objMeta for AsicPowerMetrics key %+v", m.GetKey())
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

// getAsicPowerMetricsHandler is the Get Handler for AsicPowerMetrics
func (s *RestServer) getAsicPowerMetricsHandler(r *http.Request) (interface{}, error) {
	log.Infof("Got GET request AsicPowerMetrics/%s", mux.Vars(r)["Meta.Name"])
	return nil, nil
}

// AddAsicTemperatureMetricsAPIRoutes adds routes for AsicTemperatureMetrics
func (s *RestServer) AddAsicTemperatureMetricsAPIRoutes(r *mux.Router) {
	r.Methods("GET").Subrouter().HandleFunc("/{Meta.Tenant}/{Meta.Name}/", httputils.MakeHTTPHandler(s.getAsicTemperatureMetricsHandler))
	r.Methods("GET").Subrouter().HandleFunc("/", httputils.MakeHTTPHandler(s.listAsicTemperatureMetricsHandler))
}

// listAsicTemperatureMetricsHandler is the List Handler for AsicTemperatureMetrics
func (s *RestServer) listAsicTemperatureMetricsHandler(r *http.Request) (interface{}, error) {
	iter, err := goproto.NewAsicTemperatureMetricsIterator()
	if err != nil {
		return nil, fmt.Errorf("failed to get metrics, error: %s", err)
	}

	// for OSX tests
	if iter == nil {
		return nil, nil
	}

	var mtr []goproto.AsicTemperatureMetrics

	for iter.HasNext() {
		temp := iter.Next()
		if temp == nil {
			continue
		}

		objMeta := s.GetObjectMeta("AsicTemperatureMetricsKey", temp.GetKey())
		if objMeta == nil {
			log.Errorf("failed to get objMeta for AsicTemperatureMetrics key %+v", temp.GetKey())
			continue
		}

		temp.ObjectMeta = *objMeta
		mtr = append(mtr, *temp)
	}
	iter.Free()
	return mtr, nil
}

// getAsicTemperatureMetricsPoints returns tags and fields to save in Venice TSDB
func (s *RestServer) getAsicTemperatureMetricsPoints() ([]*tsdb.Point, error) {
	iter, err := goproto.NewAsicTemperatureMetricsIterator()
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
		objMeta := s.GetObjectMeta("AsicTemperatureMetricsKey", m.GetKey())
		if objMeta == nil {
			log.Errorf("failed to get objMeta for AsicTemperatureMetrics key %+v", m.GetKey())
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

// getAsicTemperatureMetricsHandler is the Get Handler for AsicTemperatureMetrics
func (s *RestServer) getAsicTemperatureMetricsHandler(r *http.Request) (interface{}, error) {
	log.Infof("Got GET request AsicTemperatureMetrics/%s", mux.Vars(r)["Meta.Name"])
	return nil, nil
}
