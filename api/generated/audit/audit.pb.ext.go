// Code generated by protoc-gen-grpc-pensando DO NOT EDIT.

/*
Package audit is a auto generated package.
Input file: audit.proto
*/
package audit

import (
	fmt "fmt"

	listerwatcher "github.com/pensando/sw/api/listerwatcher"
	"github.com/pensando/sw/venice/utils/kvstore"
	"github.com/pensando/sw/venice/utils/log"

	validators "github.com/pensando/sw/venice/utils/apigen/validators"

	"github.com/pensando/sw/api/interfaces"
	"github.com/pensando/sw/venice/utils/runtime"
)

// Dummy definitions to suppress nonused warnings
var _ kvstore.Interface
var _ log.Logger
var _ listerwatcher.WatcherClient

var _ validators.DummyVar
var validatorMapAudit = make(map[string]map[string][]func(string, interface{}) error)

// Clone clones the object into into or creates one of into is nil
func (m *Event) Clone(into interface{}) (interface{}, error) {
	var out *Event
	var ok bool
	if into == nil {
		out = &Event{}
	} else {
		out, ok = into.(*Event)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *Event) Defaults(ver string) bool {
	var ret bool
	ret = m.EventAttributes.Defaults(ver) || ret
	return ret
}

// Clone clones the object into into or creates one of into is nil
func (m *EventAttributes) Clone(into interface{}) (interface{}, error) {
	var out *EventAttributes
	var ok bool
	if into == nil {
		out = &EventAttributes{}
	} else {
		out, ok = into.(*EventAttributes)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *EventAttributes) Defaults(ver string) bool {
	var ret bool
	ret = true
	switch ver {
	default:
		m.Level = "Basic"
		m.Outcome = "Success"
		m.Stage = "RequestAuthorization"
	}
	return ret
}

// Clone clones the object into into or creates one of into is nil
func (m *EventRequest) Clone(into interface{}) (interface{}, error) {
	var out *EventRequest
	var ok bool
	if into == nil {
		out = &EventRequest{}
	} else {
		out, ok = into.(*EventRequest)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *EventRequest) Defaults(ver string) bool {
	return false
}

// Validators and Requirements

func (m *Event) References(tenant string, path string, resp map[string]apiintf.ReferenceObj) {

}

func (m *Event) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error

	{
		dlmtr := "."
		if path == "" {
			dlmtr = ""
		}
		npath := path + dlmtr + "EventAttributes"
		if errs := m.EventAttributes.Validate(ver, npath, ignoreStatus); errs != nil {
			ret = append(ret, errs...)
		}
	}
	return ret
}

func (m *EventAttributes) References(tenant string, path string, resp map[string]apiintf.ReferenceObj) {

}

func (m *EventAttributes) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	if vs, ok := validatorMapAudit["EventAttributes"][ver]; ok {
		for _, v := range vs {
			if err := v(path, m); err != nil {
				ret = append(ret, err)
			}
		}
	} else if vs, ok := validatorMapAudit["EventAttributes"]["all"]; ok {
		for _, v := range vs {
			if err := v(path, m); err != nil {
				ret = append(ret, err)
			}
		}
	}
	return ret
}

func (m *EventRequest) References(tenant string, path string, resp map[string]apiintf.ReferenceObj) {

}

func (m *EventRequest) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	return ret
}

// Transformers

func init() {
	scheme := runtime.GetDefaultScheme()
	scheme.AddKnownTypes()

	validatorMapAudit = make(map[string]map[string][]func(string, interface{}) error)

	validatorMapAudit["EventAttributes"] = make(map[string][]func(string, interface{}) error)
	validatorMapAudit["EventAttributes"]["all"] = append(validatorMapAudit["EventAttributes"]["all"], func(path string, i interface{}) error {
		m := i.(*EventAttributes)

		if _, ok := Level_value[m.Level]; !ok {
			return fmt.Errorf("%v did not match allowed strings", path+"."+"Level")
		}
		return nil
	})

	validatorMapAudit["EventAttributes"]["all"] = append(validatorMapAudit["EventAttributes"]["all"], func(path string, i interface{}) error {
		m := i.(*EventAttributes)

		if _, ok := Outcome_value[m.Outcome]; !ok {
			return fmt.Errorf("%v did not match allowed strings", path+"."+"Outcome")
		}
		return nil
	})

	validatorMapAudit["EventAttributes"]["all"] = append(validatorMapAudit["EventAttributes"]["all"], func(path string, i interface{}) error {
		m := i.(*EventAttributes)
		if !validators.URI(m.RequestURI) {
			return fmt.Errorf("%v validation failed", path+"."+"RequestURI")
		}
		return nil
	})

	validatorMapAudit["EventAttributes"]["all"] = append(validatorMapAudit["EventAttributes"]["all"], func(path string, i interface{}) error {
		m := i.(*EventAttributes)

		if _, ok := Stage_value[m.Stage]; !ok {
			return fmt.Errorf("%v did not match allowed strings", path+"."+"Stage")
		}
		return nil
	})

}
