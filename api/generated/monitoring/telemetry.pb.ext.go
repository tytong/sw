// Code generated by protoc-gen-grpc-pensando DO NOT EDIT.

/*
Package monitoring is a auto generated package.
Input file: telemetry.proto
*/
package monitoring

import (
	"errors"
	fmt "fmt"

	listerwatcher "github.com/pensando/sw/api/listerwatcher"
	"github.com/pensando/sw/venice/utils/kvstore"
	"github.com/pensando/sw/venice/utils/log"
	"github.com/pensando/sw/venice/utils/runtime"

	validators "github.com/pensando/sw/venice/utils/apigen/validators"

	"github.com/pensando/sw/venice/globals"
)

// Dummy definitions to suppress nonused warnings
var _ kvstore.Interface
var _ log.Logger
var _ listerwatcher.WatcherClient

var _ validators.DummyVar
var validatorMapTelemetry = make(map[string]map[string][]func(string, interface{}) error)

// MakeKey generates a KV store key for the object
func (m *FlowExportPolicy) MakeKey(prefix string) string {
	return fmt.Sprint(globals.RootPrefix, "/", prefix, "/", "flowExportPolicy/", m.Tenant, "/", m.Name)
}

func (m *FlowExportPolicy) MakeURI(cat, ver, prefix string) string {
	in := m
	return fmt.Sprint("/", cat, "/", prefix, "/", ver, "/tenant/", in.Tenant, "/flowExportPolicy/", in.Name)
}

// MakeKey generates a KV store key for the object
func (m *FwlogPolicy) MakeKey(prefix string) string {
	return fmt.Sprint(globals.RootPrefix, "/", prefix, "/", "fwlogPolicy/", m.Tenant, "/", m.Name)
}

func (m *FwlogPolicy) MakeURI(cat, ver, prefix string) string {
	in := m
	return fmt.Sprint("/", cat, "/", prefix, "/", ver, "/tenant/", in.Tenant, "/fwlogPolicy/", in.Name)
}

// MakeKey generates a KV store key for the object
func (m *StatsPolicy) MakeKey(prefix string) string {
	return fmt.Sprint(globals.RootPrefix, "/", prefix, "/", "statsPolicy/", m.Tenant, "/", m.Name)
}

func (m *StatsPolicy) MakeURI(cat, ver, prefix string) string {
	in := m
	return fmt.Sprint("/", cat, "/", prefix, "/", ver, "/tenant/", in.Tenant, "/statsPolicy/", in.Name)
}

// Clone clones the object into into or creates one of into is nil
func (m *FlowExportPolicy) Clone(into interface{}) (interface{}, error) {
	var out *FlowExportPolicy
	var ok bool
	if into == nil {
		out = &FlowExportPolicy{}
	} else {
		out, ok = into.(*FlowExportPolicy)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *FlowExportPolicy) Defaults(ver string) bool {
	m.Kind = "FlowExportPolicy"
	var ret bool
	ret = m.Spec.Defaults(ver) || ret
	return ret
}

// Clone clones the object into into or creates one of into is nil
func (m *FlowExportSpec) Clone(into interface{}) (interface{}, error) {
	var out *FlowExportSpec
	var ok bool
	if into == nil {
		out = &FlowExportSpec{}
	} else {
		out, ok = into.(*FlowExportSpec)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *FlowExportSpec) Defaults(ver string) bool {
	var ret bool
	for k := range m.Targets {
		i := m.Targets[k]
		ret = i.Defaults(ver) || ret
	}
	return ret
}

// Clone clones the object into into or creates one of into is nil
func (m *FlowExportStatus) Clone(into interface{}) (interface{}, error) {
	var out *FlowExportStatus
	var ok bool
	if into == nil {
		out = &FlowExportStatus{}
	} else {
		out, ok = into.(*FlowExportStatus)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *FlowExportStatus) Defaults(ver string) bool {
	return false
}

// Clone clones the object into into or creates one of into is nil
func (m *FlowExportTarget) Clone(into interface{}) (interface{}, error) {
	var out *FlowExportTarget
	var ok bool
	if into == nil {
		out = &FlowExportTarget{}
	} else {
		out, ok = into.(*FlowExportTarget)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *FlowExportTarget) Defaults(ver string) bool {
	var ret bool
	ret = true
	switch ver {
	default:
		m.Format = "Ipfix"
	}
	return ret
}

// Clone clones the object into into or creates one of into is nil
func (m *FwlogExport) Clone(into interface{}) (interface{}, error) {
	var out *FwlogExport
	var ok bool
	if into == nil {
		out = &FwlogExport{}
	} else {
		out, ok = into.(*FwlogExport)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *FwlogExport) Defaults(ver string) bool {
	var ret bool
	ret = true
	switch ver {
	default:
		for k := range m.Filter {
			m.Filter[k] = "FWLOG_ALL"
		}
		m.Format = "SYSLOG_BSD"
	}
	return ret
}

// Clone clones the object into into or creates one of into is nil
func (m *FwlogPolicy) Clone(into interface{}) (interface{}, error) {
	var out *FwlogPolicy
	var ok bool
	if into == nil {
		out = &FwlogPolicy{}
	} else {
		out, ok = into.(*FwlogPolicy)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *FwlogPolicy) Defaults(ver string) bool {
	m.Kind = "FwlogPolicy"
	var ret bool
	ret = m.Spec.Defaults(ver) || ret
	return ret
}

// Clone clones the object into into or creates one of into is nil
func (m *FwlogSpec) Clone(into interface{}) (interface{}, error) {
	var out *FwlogSpec
	var ok bool
	if into == nil {
		out = &FwlogSpec{}
	} else {
		out, ok = into.(*FwlogSpec)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *FwlogSpec) Defaults(ver string) bool {
	var ret bool
	for k := range m.Exports {
		if m.Exports[k] != nil {
			i := m.Exports[k]
			ret = i.Defaults(ver) || ret
		}
	}
	ret = true
	switch ver {
	default:
		for k := range m.Filter {
			m.Filter[k] = "FWLOG_ALL"
		}
	}
	return ret
}

// Clone clones the object into into or creates one of into is nil
func (m *FwlogStatus) Clone(into interface{}) (interface{}, error) {
	var out *FwlogStatus
	var ok bool
	if into == nil {
		out = &FwlogStatus{}
	} else {
		out, ok = into.(*FwlogStatus)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *FwlogStatus) Defaults(ver string) bool {
	return false
}

// Clone clones the object into into or creates one of into is nil
func (m *StatsPolicy) Clone(into interface{}) (interface{}, error) {
	var out *StatsPolicy
	var ok bool
	if into == nil {
		out = &StatsPolicy{}
	} else {
		out, ok = into.(*StatsPolicy)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *StatsPolicy) Defaults(ver string) bool {
	m.Kind = "StatsPolicy"
	return false
}

// Clone clones the object into into or creates one of into is nil
func (m *StatsSpec) Clone(into interface{}) (interface{}, error) {
	var out *StatsSpec
	var ok bool
	if into == nil {
		out = &StatsSpec{}
	} else {
		out, ok = into.(*StatsSpec)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *StatsSpec) Defaults(ver string) bool {
	return false
}

// Clone clones the object into into or creates one of into is nil
func (m *StatsStatus) Clone(into interface{}) (interface{}, error) {
	var out *StatsStatus
	var ok bool
	if into == nil {
		out = &StatsStatus{}
	} else {
		out, ok = into.(*StatsStatus)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *StatsStatus) Defaults(ver string) bool {
	return false
}

// Validators

func (m *FlowExportPolicy) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error

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

func (m *FlowExportSpec) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	for k, v := range m.Targets {
		dlmtr := "."
		if path == "" {
			dlmtr = ""
		}
		npath := fmt.Sprintf("%s%sTargets[%d]", path, dlmtr, k)
		if errs := v.Validate(ver, npath, ignoreStatus); errs != nil {
			ret = append(ret, errs...)
		}
	}
	return ret
}

func (m *FlowExportStatus) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	return ret
}

func (m *FlowExportTarget) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	if vs, ok := validatorMapTelemetry["FlowExportTarget"][ver]; ok {
		for _, v := range vs {
			if err := v(path, m); err != nil {
				ret = append(ret, err)
			}
		}
	} else if vs, ok := validatorMapTelemetry["FlowExportTarget"]["all"]; ok {
		for _, v := range vs {
			if err := v(path, m); err != nil {
				ret = append(ret, err)
			}
		}
	}
	return ret
}

func (m *FwlogExport) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	if vs, ok := validatorMapTelemetry["FwlogExport"][ver]; ok {
		for _, v := range vs {
			if err := v(path, m); err != nil {
				ret = append(ret, err)
			}
		}
	} else if vs, ok := validatorMapTelemetry["FwlogExport"]["all"]; ok {
		for _, v := range vs {
			if err := v(path, m); err != nil {
				ret = append(ret, err)
			}
		}
	}
	return ret
}

func (m *FwlogPolicy) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error

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

func (m *FwlogSpec) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	for k, v := range m.Exports {
		dlmtr := "."
		if path == "" {
			dlmtr = ""
		}
		npath := fmt.Sprintf("%s%sExports[%d]", path, dlmtr, k)
		if errs := v.Validate(ver, npath, ignoreStatus); errs != nil {
			ret = append(ret, errs...)
		}
	}
	if vs, ok := validatorMapTelemetry["FwlogSpec"][ver]; ok {
		for _, v := range vs {
			if err := v(path, m); err != nil {
				ret = append(ret, err)
			}
		}
	} else if vs, ok := validatorMapTelemetry["FwlogSpec"]["all"]; ok {
		for _, v := range vs {
			if err := v(path, m); err != nil {
				ret = append(ret, err)
			}
		}
	}
	return ret
}

func (m *FwlogStatus) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	return ret
}

func (m *StatsPolicy) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	return ret
}

func (m *StatsSpec) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	return ret
}

func (m *StatsStatus) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	return ret
}

// Transformers

func init() {
	scheme := runtime.GetDefaultScheme()
	scheme.AddKnownTypes(
		&FlowExportPolicy{},
		&FwlogPolicy{},
		&StatsPolicy{},
	)

	validatorMapTelemetry = make(map[string]map[string][]func(string, interface{}) error)

	validatorMapTelemetry["FlowExportTarget"] = make(map[string][]func(string, interface{}) error)
	validatorMapTelemetry["FlowExportTarget"]["all"] = append(validatorMapTelemetry["FlowExportTarget"]["all"], func(path string, i interface{}) error {
		m := i.(*FlowExportTarget)

		if _, ok := FlowExportTarget_Formats_value[m.Format]; !ok {
			return errors.New("FlowExportTarget.Format did not match allowed strings")
		}
		return nil
	})

	validatorMapTelemetry["FwlogExport"] = make(map[string][]func(string, interface{}) error)
	validatorMapTelemetry["FwlogExport"]["all"] = append(validatorMapTelemetry["FwlogExport"]["all"], func(path string, i interface{}) error {
		m := i.(*FwlogExport)

		for k, v := range m.Filter {
			if _, ok := FwlogFilter_value[v]; !ok {
				return fmt.Errorf("%v[%v] did not match allowed strings", path+"."+"Filter", k)
			}
		}
		return nil
	})

	validatorMapTelemetry["FwlogExport"]["all"] = append(validatorMapTelemetry["FwlogExport"]["all"], func(path string, i interface{}) error {
		m := i.(*FwlogExport)

		if _, ok := MonitoringExportFormat_value[m.Format]; !ok {
			return errors.New("FwlogExport.Format did not match allowed strings")
		}
		return nil
	})

	validatorMapTelemetry["FwlogSpec"] = make(map[string][]func(string, interface{}) error)
	validatorMapTelemetry["FwlogSpec"]["all"] = append(validatorMapTelemetry["FwlogSpec"]["all"], func(path string, i interface{}) error {
		m := i.(*FwlogSpec)

		for k, v := range m.Filter {
			if _, ok := FwlogFilter_value[v]; !ok {
				return fmt.Errorf("%v[%v] did not match allowed strings", path+"."+"Filter", k)
			}
		}
		return nil
	})

}
