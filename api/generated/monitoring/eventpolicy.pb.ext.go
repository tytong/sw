// Code generated by protoc-gen-grpc-pensando DO NOT EDIT.

/*
Package monitoring is a auto generated package.
Input file: eventpolicy.proto
*/
package monitoring

import (
	"errors"
	fmt "fmt"

	listerwatcher "github.com/pensando/sw/api/listerwatcher"
	"github.com/pensando/sw/venice/utils/kvstore"
	"github.com/pensando/sw/venice/utils/log"

	validators "github.com/pensando/sw/venice/utils/apigen/validators"

	"github.com/pensando/sw/venice/globals"
	"github.com/pensando/sw/venice/utils/runtime"
)

// Dummy definitions to suppress nonused warnings
var _ kvstore.Interface
var _ log.Logger
var _ listerwatcher.WatcherClient

var _ validators.DummyVar
var validatorMapEventpolicy = make(map[string]map[string][]func(string, interface{}) error)

// MakeKey generates a KV store key for the object
func (m *EventPolicy) MakeKey(prefix string) string {
	return fmt.Sprint(globals.ConfigRootPrefix, "/", prefix, "/", "event-policy/", m.Tenant, "/", m.Name)
}

func (m *EventPolicy) MakeURI(cat, ver, prefix string) string {
	in := m
	return fmt.Sprint("/", cat, "/", prefix, "/", ver, "/tenant/", in.Tenant, "/event-policy/", in.Name)
}

// Clone clones the object into into or creates one of into is nil
func (m *EventExport) Clone(into interface{}) (interface{}, error) {
	var out *EventExport
	var ok bool
	if into == nil {
		out = &EventExport{}
	} else {
		out, ok = into.(*EventExport)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *EventExport) Defaults(ver string) bool {
	var ret bool
	ret = true
	switch ver {
	default:
		m.Format = "SYSLOG_BSD"
	}
	return ret
}

// Clone clones the object into into or creates one of into is nil
func (m *EventPolicy) Clone(into interface{}) (interface{}, error) {
	var out *EventPolicy
	var ok bool
	if into == nil {
		out = &EventPolicy{}
	} else {
		out, ok = into.(*EventPolicy)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *EventPolicy) Defaults(ver string) bool {
	m.Kind = "EventPolicy"
	m.Tenant, m.Namespace = "default", "default"
	var ret bool
	ret = m.Spec.Defaults(ver) || ret
	return ret
}

// Clone clones the object into into or creates one of into is nil
func (m *EventPolicySpec) Clone(into interface{}) (interface{}, error) {
	var out *EventPolicySpec
	var ok bool
	if into == nil {
		out = &EventPolicySpec{}
	} else {
		out, ok = into.(*EventPolicySpec)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *EventPolicySpec) Defaults(ver string) bool {
	var ret bool
	for k := range m.Exports {
		if m.Exports[k] != nil {
			i := m.Exports[k]
			ret = i.Defaults(ver) || ret
		}
	}
	return ret
}

// Clone clones the object into into or creates one of into is nil
func (m *EventPolicyStatus) Clone(into interface{}) (interface{}, error) {
	var out *EventPolicyStatus
	var ok bool
	if into == nil {
		out = &EventPolicyStatus{}
	} else {
		out, ok = into.(*EventPolicyStatus)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *EventPolicyStatus) Defaults(ver string) bool {
	return false
}

// Validators

func (m *EventExport) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	if m.Selector != nil {
		dlmtr := "."
		if path == "" {
			dlmtr = ""
		}
		npath := path + dlmtr + "Selector"
		if errs := m.Selector.Validate(ver, npath, ignoreStatus); errs != nil {
			ret = append(ret, errs...)
		}
	}
	if m.Target != nil {
		dlmtr := "."
		if path == "" {
			dlmtr = ""
		}
		npath := path + dlmtr + "Target"
		if errs := m.Target.Validate(ver, npath, ignoreStatus); errs != nil {
			ret = append(ret, errs...)
		}
	}
	if vs, ok := validatorMapEventpolicy["EventExport"][ver]; ok {
		for _, v := range vs {
			if err := v(path, m); err != nil {
				ret = append(ret, err)
			}
		}
	} else if vs, ok := validatorMapEventpolicy["EventExport"]["all"]; ok {
		for _, v := range vs {
			if err := v(path, m); err != nil {
				ret = append(ret, err)
			}
		}
	}
	return ret
}

func (m *EventPolicy) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	{
		dlmtr := "."
		if path == "" {
			dlmtr = ""
		}
		ret = m.ObjectMeta.Validate(ver, path+dlmtr+"ObjectMeta", ignoreStatus)
	}

	dlmtr := "."
	if path == "" {
		dlmtr = ""
	}
	npath := path + dlmtr + "Spec"
	if errs := m.Spec.Validate(ver, npath, ignoreStatus); errs != nil {
		ret = append(ret, errs...)
	}
	return ret
}

func (m *EventPolicySpec) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	for k, v := range m.Exports {
		dlmtr := "."
		if path == "" {
			dlmtr = ""
		}
		npath := fmt.Sprintf("%s%sExports[%v]", path, dlmtr, k)
		if errs := v.Validate(ver, npath, ignoreStatus); errs != nil {
			ret = append(ret, errs...)
		}
	}
	return ret
}

func (m *EventPolicyStatus) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	return ret
}

// Transformers

func init() {
	scheme := runtime.GetDefaultScheme()
	scheme.AddKnownTypes(
		&EventPolicy{},
	)

	validatorMapEventpolicy = make(map[string]map[string][]func(string, interface{}) error)

	validatorMapEventpolicy["EventExport"] = make(map[string][]func(string, interface{}) error)
	validatorMapEventpolicy["EventExport"]["all"] = append(validatorMapEventpolicy["EventExport"]["all"], func(path string, i interface{}) error {
		m := i.(*EventExport)

		if _, ok := MonitoringExportFormat_value[m.Format]; !ok {
			return errors.New("EventExport.Format did not match allowed strings")
		}
		return nil
	})

}
