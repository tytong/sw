// Code generated by protoc-gen-grpc-pensando DO NOT EDIT.

/*
 * Package restapi is a auto generated package.
 * Input file: eventpolicy.proto
 */
package restapi

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
	"net/http"
	"time"

	"github.com/gogo/protobuf/types"
	"github.com/gorilla/mux"

	"github.com/pensando/sw/api"
	"github.com/pensando/sw/nic/agent/httputils"
	evtsmgrprotos "github.com/pensando/sw/venice/ctrler/evtsmgr/rpcserver/protos"
)

// addEventPolicyAPIRoutes adds EventPolicy
func addEventPolicyAPIRoutes(r *mux.Router, srv *RestServer) {

	r.Methods("GET").Subrouter().HandleFunc("/{ObjectMeta.Tenant}/{ObjectMeta.Namespace}/{ObjectMeta.Name}", httputils.MakeHTTPHandler(srv.EventPolicyGetHandler))

	r.Methods("GET").Subrouter().HandleFunc("/", httputils.MakeHTTPHandler(srv.EventPolicyListHandler))

	r.Methods("POST").Subrouter().HandleFunc("/", httputils.MakeHTTPHandler(srv.EventPolicyPostHandler))

	r.Methods("DELETE").Subrouter().HandleFunc("/{ObjectMeta.Tenant}/{ObjectMeta.Namespace}/{ObjectMeta.Name}", httputils.MakeHTTPHandler(srv.EventPolicyDeleteHandler))

	r.Methods("PUT").Subrouter().HandleFunc("/{ObjectMeta.Tenant}/{ObjectMeta.Namespace}/{ObjectMeta.Name}", httputils.MakeHTTPHandler(srv.EventPolicyPutHandler))

}

func (s *RestServer) EventPolicyGetHandler(r *http.Request) (interface{}, error) {
	var o evtsmgrprotos.EventPolicy

	o.TypeMeta.Kind = "EventPolicy"
	o.ObjectMeta.Tenant = mux.Vars(r)["ObjectMeta.Tenant"]
	o.ObjectMeta.Namespace = mux.Vars(r)["ObjectMeta.Namespace"]
	o.ObjectMeta.Name = mux.Vars(r)["ObjectMeta.Name"]

	p, err := s.handler.GetEventPolicy(r.Context(), &o)
	return p, err

}

func (s *RestServer) EventPolicyListHandler(r *http.Request) (interface{}, error) {
	p, err := s.handler.ListEventPolicy(r.Context())
	return p, err
}

func (s *RestServer) EventPolicyPostHandler(r *http.Request) (interface{}, error) {
	var o evtsmgrprotos.EventPolicy

	var res Response
	b, _ := ioutil.ReadAll(r.Body)
	err := json.Unmarshal(b, &o)
	if err != nil {
		return nil, err
	}
	c, _ := types.TimestampProto(time.Now())
	o.CreationTime = api.Timestamp{
		Timestamp: *c,
	}
	o.ModTime = api.Timestamp{
		Timestamp: *c,
	}
	err = s.handler.CreateEventPolicy(r.Context(), &o)
	if err != nil {
		res.StatusCode = http.StatusInternalServerError
		res.Error = err.Error()
		return res, err
	}
	res.References = []string{fmt.Sprintf("%s%s/%s/%s", r.RequestURI, o.Tenant, o.Namespace, o.Name)}
	res.StatusCode = http.StatusOK
	return res, err

}

func (s *RestServer) EventPolicyDeleteHandler(r *http.Request) (interface{}, error) {
	var o evtsmgrprotos.EventPolicy

	o.TypeMeta.Kind = "EventPolicy"
	o.ObjectMeta.Tenant = mux.Vars(r)["ObjectMeta.Tenant"]
	o.ObjectMeta.Namespace = mux.Vars(r)["ObjectMeta.Namespace"]
	o.ObjectMeta.Name = mux.Vars(r)["ObjectMeta.Name"]

	return Response{}, s.handler.DeleteEventPolicy(r.Context(), &o)

}

func (s *RestServer) EventPolicyPutHandler(r *http.Request) (interface{}, error) {
	var o evtsmgrprotos.EventPolicy

	var res Response
	b, _ := ioutil.ReadAll(r.Body)
	err := json.Unmarshal(b, &o)
	if err != nil {
		return nil, err
	}
	m, _ := types.TimestampProto(time.Now())
	o.ModTime = api.Timestamp{
		Timestamp: *m,
	}
	err = s.handler.UpdateEventPolicy(r.Context(), &o)
	if err != nil {
		res.StatusCode = http.StatusInternalServerError
		res.Error = err.Error()
		return res, err
	}
	res.References = []string{fmt.Sprintf("%s%s/%s/%s", r.RequestURI, o.Tenant, o.Namespace, o.Name)}
	res.StatusCode = http.StatusOK
	return res, err

}
