// Code generated by protoc-gen-grpc-pensando DO NOT EDIT.

/*
Package cluster is a auto generated package.
Input file: cmd.proto
*/
package cluster

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
var validatorMapCmd = make(map[string]map[string][]func(string, interface{}) error)

// MakeKey generates a KV store key for the object
func (m *Cluster) MakeKey(prefix string) string {
	return fmt.Sprint(globals.RootPrefix, "/", prefix, "/", "cluster/", m.Name)
}

func (m *Cluster) MakeURI(ver, prefix string) string {
	in := m
	return fmt.Sprint("/", ver, "/", prefix, "/cluster/", in.Name)
}

// MakeKey generates a KV store key for the object
func (m *Host) MakeKey(prefix string) string {
	return fmt.Sprint(globals.RootPrefix, "/", prefix, "/", "hosts/", m.Name)
}

func (m *Host) MakeURI(ver, prefix string) string {
	in := m
	return fmt.Sprint("/", ver, "/", prefix, "/hosts/", in.Name)
}

// MakeKey generates a KV store key for the object
func (m *Node) MakeKey(prefix string) string {
	return fmt.Sprint(globals.RootPrefix, "/", prefix, "/", "nodes/", m.Name)
}

func (m *Node) MakeURI(ver, prefix string) string {
	in := m
	return fmt.Sprint("/", ver, "/", prefix, "/nodes/", in.Name)
}

// MakeKey generates a KV store key for the object
func (m *SmartNIC) MakeKey(prefix string) string {
	return fmt.Sprint(globals.RootPrefix, "/", prefix, "/", "smartnics/", m.Name)
}

func (m *SmartNIC) MakeURI(ver, prefix string) string {
	in := m
	return fmt.Sprint("/", ver, "/", prefix, "/smartnics/", in.Name)
}

// Clone clones the object into into or creates one of into is nil
func (m *Cluster) Clone(into interface{}) (interface{}, error) {
	var out *Cluster
	var ok bool
	if into == nil {
		out = &Cluster{}
	} else {
		out, ok = into.(*Cluster)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *Cluster) Defaults(ver string) bool {
	return false
}

// Clone clones the object into into or creates one of into is nil
func (m *ClusterSpec) Clone(into interface{}) (interface{}, error) {
	var out *ClusterSpec
	var ok bool
	if into == nil {
		out = &ClusterSpec{}
	} else {
		out, ok = into.(*ClusterSpec)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *ClusterSpec) Defaults(ver string) bool {
	return false
}

// Clone clones the object into into or creates one of into is nil
func (m *ClusterStatus) Clone(into interface{}) (interface{}, error) {
	var out *ClusterStatus
	var ok bool
	if into == nil {
		out = &ClusterStatus{}
	} else {
		out, ok = into.(*ClusterStatus)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *ClusterStatus) Defaults(ver string) bool {
	return false
}

// Clone clones the object into into or creates one of into is nil
func (m *Host) Clone(into interface{}) (interface{}, error) {
	var out *Host
	var ok bool
	if into == nil {
		out = &Host{}
	} else {
		out, ok = into.(*Host)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *Host) Defaults(ver string) bool {
	var ret bool
	ret = m.Status.Defaults(ver) || ret
	return ret
}

// Clone clones the object into into or creates one of into is nil
func (m *HostIntfSpec) Clone(into interface{}) (interface{}, error) {
	var out *HostIntfSpec
	var ok bool
	if into == nil {
		out = &HostIntfSpec{}
	} else {
		out, ok = into.(*HostIntfSpec)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *HostIntfSpec) Defaults(ver string) bool {
	return false
}

// Clone clones the object into into or creates one of into is nil
func (m *HostIntfStatus) Clone(into interface{}) (interface{}, error) {
	var out *HostIntfStatus
	var ok bool
	if into == nil {
		out = &HostIntfStatus{}
	} else {
		out, ok = into.(*HostIntfStatus)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *HostIntfStatus) Defaults(ver string) bool {
	return false
}

// Clone clones the object into into or creates one of into is nil
func (m *HostSpec) Clone(into interface{}) (interface{}, error) {
	var out *HostSpec
	var ok bool
	if into == nil {
		out = &HostSpec{}
	} else {
		out, ok = into.(*HostSpec)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *HostSpec) Defaults(ver string) bool {
	return false
}

// Clone clones the object into into or creates one of into is nil
func (m *HostStatus) Clone(into interface{}) (interface{}, error) {
	var out *HostStatus
	var ok bool
	if into == nil {
		out = &HostStatus{}
	} else {
		out, ok = into.(*HostStatus)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *HostStatus) Defaults(ver string) bool {
	var ret bool
	ret = true
	switch ver {
	default:
		m.Type = "UNKNOWN"
	}
	return ret
}

// Clone clones the object into into or creates one of into is nil
func (m *Node) Clone(into interface{}) (interface{}, error) {
	var out *Node
	var ok bool
	if into == nil {
		out = &Node{}
	} else {
		out, ok = into.(*Node)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *Node) Defaults(ver string) bool {
	var ret bool
	ret = m.Spec.Defaults(ver) || ret
	ret = m.Status.Defaults(ver) || ret
	return ret
}

// Clone clones the object into into or creates one of into is nil
func (m *NodeCondition) Clone(into interface{}) (interface{}, error) {
	var out *NodeCondition
	var ok bool
	if into == nil {
		out = &NodeCondition{}
	} else {
		out, ok = into.(*NodeCondition)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *NodeCondition) Defaults(ver string) bool {
	var ret bool
	ret = true
	switch ver {
	default:
		m.Status = "UNKNOWN"
		m.Type = "LEADER"
	}
	return ret
}

// Clone clones the object into into or creates one of into is nil
func (m *NodeSpec) Clone(into interface{}) (interface{}, error) {
	var out *NodeSpec
	var ok bool
	if into == nil {
		out = &NodeSpec{}
	} else {
		out, ok = into.(*NodeSpec)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *NodeSpec) Defaults(ver string) bool {
	var ret bool
	ret = true
	switch ver {
	default:
		for k := range m.Roles {
			m.Roles[k] = "CONTROLLER"
		}
	}
	return ret
}

// Clone clones the object into into or creates one of into is nil
func (m *NodeStatus) Clone(into interface{}) (interface{}, error) {
	var out *NodeStatus
	var ok bool
	if into == nil {
		out = &NodeStatus{}
	} else {
		out, ok = into.(*NodeStatus)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *NodeStatus) Defaults(ver string) bool {
	var ret bool
	for k := range m.Conditions {
		if m.Conditions[k] != nil {
			ret = m.Conditions[k].Defaults(ver) || ret
		}
	}
	ret = true
	switch ver {
	default:
		m.Phase = "UNKNOWN"
	}
	return ret
}

// Clone clones the object into into or creates one of into is nil
func (m *PortCondition) Clone(into interface{}) (interface{}, error) {
	var out *PortCondition
	var ok bool
	if into == nil {
		out = &PortCondition{}
	} else {
		out, ok = into.(*PortCondition)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *PortCondition) Defaults(ver string) bool {
	var ret bool
	ret = true
	switch ver {
	default:
		m.Status = "UNKNOWN"
		m.Type = "PORT_UP"
	}
	return ret
}

// Clone clones the object into into or creates one of into is nil
func (m *PortSpec) Clone(into interface{}) (interface{}, error) {
	var out *PortSpec
	var ok bool
	if into == nil {
		out = &PortSpec{}
	} else {
		out, ok = into.(*PortSpec)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *PortSpec) Defaults(ver string) bool {
	return false
}

// Clone clones the object into into or creates one of into is nil
func (m *PortStatus) Clone(into interface{}) (interface{}, error) {
	var out *PortStatus
	var ok bool
	if into == nil {
		out = &PortStatus{}
	} else {
		out, ok = into.(*PortStatus)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *PortStatus) Defaults(ver string) bool {
	var ret bool
	for k := range m.Conditions {
		if m.Conditions[k] != nil {
			ret = m.Conditions[k].Defaults(ver) || ret
		}
	}
	return ret
}

// Clone clones the object into into or creates one of into is nil
func (m *SmartNIC) Clone(into interface{}) (interface{}, error) {
	var out *SmartNIC
	var ok bool
	if into == nil {
		out = &SmartNIC{}
	} else {
		out, ok = into.(*SmartNIC)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *SmartNIC) Defaults(ver string) bool {
	var ret bool
	ret = m.Spec.Defaults(ver) || ret
	ret = m.Status.Defaults(ver) || ret
	return ret
}

// Clone clones the object into into or creates one of into is nil
func (m *SmartNICCondition) Clone(into interface{}) (interface{}, error) {
	var out *SmartNICCondition
	var ok bool
	if into == nil {
		out = &SmartNICCondition{}
	} else {
		out, ok = into.(*SmartNICCondition)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *SmartNICCondition) Defaults(ver string) bool {
	var ret bool
	ret = true
	switch ver {
	default:
		m.Status = "UNKNOWN"
		m.Type = "HEALTHY"
	}
	return ret
}

// Clone clones the object into into or creates one of into is nil
func (m *SmartNICSpec) Clone(into interface{}) (interface{}, error) {
	var out *SmartNICSpec
	var ok bool
	if into == nil {
		out = &SmartNICSpec{}
	} else {
		out, ok = into.(*SmartNICSpec)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *SmartNICSpec) Defaults(ver string) bool {
	var ret bool
	ret = true
	switch ver {
	default:
		m.Phase = "UNKNOWN"
	}
	return ret
}

// Clone clones the object into into or creates one of into is nil
func (m *SmartNICStatus) Clone(into interface{}) (interface{}, error) {
	var out *SmartNICStatus
	var ok bool
	if into == nil {
		out = &SmartNICStatus{}
	} else {
		out, ok = into.(*SmartNICStatus)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *SmartNICStatus) Defaults(ver string) bool {
	var ret bool
	for k := range m.Conditions {
		if m.Conditions[k] != nil {
			ret = m.Conditions[k].Defaults(ver) || ret
		}
	}
	for k := range m.Ports {
		if m.Ports[k] != nil {
			ret = m.Ports[k].Defaults(ver) || ret
		}
	}
	return ret
}

// Validators

func (m *Cluster) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	return ret
}

func (m *ClusterSpec) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	return ret
}

func (m *ClusterStatus) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	return ret
}

func (m *Host) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	if !ignoreStatus {

		dlmtr := "."
		if path == "" {
			dlmtr = ""
		}
		npath := path + dlmtr + "Status"
		if errs := m.Status.Validate(ver, npath, ignoreStatus); errs != nil {
			ret = append(ret, errs...)
		}
	}
	return ret
}

func (m *HostIntfSpec) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	return ret
}

func (m *HostIntfStatus) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	return ret
}

func (m *HostSpec) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	return ret
}

func (m *HostStatus) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	if vs, ok := validatorMapCmd["HostStatus"][ver]; ok {
		for _, v := range vs {
			if err := v(path, m); err != nil {
				ret = append(ret, err)
			}
		}
	} else if vs, ok := validatorMapCmd["HostStatus"]["all"]; ok {
		for _, v := range vs {
			if err := v(path, m); err != nil {
				ret = append(ret, err)
			}
		}
	}
	return ret
}

func (m *Node) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error

	dlmtr := "."
	if path == "" {
		dlmtr = ""
	}
	npath := path + dlmtr + "Spec"
	if errs := m.Spec.Validate(ver, npath, ignoreStatus); errs != nil {
		ret = append(ret, errs...)
	}
	if !ignoreStatus {

		dlmtr := "."
		if path == "" {
			dlmtr = ""
		}
		npath := path + dlmtr + "Status"
		if errs := m.Status.Validate(ver, npath, ignoreStatus); errs != nil {
			ret = append(ret, errs...)
		}
	}
	return ret
}

func (m *NodeCondition) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	if vs, ok := validatorMapCmd["NodeCondition"][ver]; ok {
		for _, v := range vs {
			if err := v(path, m); err != nil {
				ret = append(ret, err)
			}
		}
	} else if vs, ok := validatorMapCmd["NodeCondition"]["all"]; ok {
		for _, v := range vs {
			if err := v(path, m); err != nil {
				ret = append(ret, err)
			}
		}
	}
	return ret
}

func (m *NodeSpec) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	if vs, ok := validatorMapCmd["NodeSpec"][ver]; ok {
		for _, v := range vs {
			if err := v(path, m); err != nil {
				ret = append(ret, err)
			}
		}
	} else if vs, ok := validatorMapCmd["NodeSpec"]["all"]; ok {
		for _, v := range vs {
			if err := v(path, m); err != nil {
				ret = append(ret, err)
			}
		}
	}
	return ret
}

func (m *NodeStatus) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	for k, v := range m.Conditions {
		dlmtr := "."
		if path == "" {
			dlmtr = ""
		}
		npath := fmt.Sprintf("%s%sConditions[%d]", path, dlmtr, k)
		if errs := v.Validate(ver, npath, ignoreStatus); errs != nil {
			ret = append(ret, errs...)
		}
	}
	if vs, ok := validatorMapCmd["NodeStatus"][ver]; ok {
		for _, v := range vs {
			if err := v(path, m); err != nil {
				ret = append(ret, err)
			}
		}
	} else if vs, ok := validatorMapCmd["NodeStatus"]["all"]; ok {
		for _, v := range vs {
			if err := v(path, m); err != nil {
				ret = append(ret, err)
			}
		}
	}
	return ret
}

func (m *PortCondition) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	if vs, ok := validatorMapCmd["PortCondition"][ver]; ok {
		for _, v := range vs {
			if err := v(path, m); err != nil {
				ret = append(ret, err)
			}
		}
	} else if vs, ok := validatorMapCmd["PortCondition"]["all"]; ok {
		for _, v := range vs {
			if err := v(path, m); err != nil {
				ret = append(ret, err)
			}
		}
	}
	return ret
}

func (m *PortSpec) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	return ret
}

func (m *PortStatus) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	for k, v := range m.Conditions {
		dlmtr := "."
		if path == "" {
			dlmtr = ""
		}
		npath := fmt.Sprintf("%s%sConditions[%d]", path, dlmtr, k)
		if errs := v.Validate(ver, npath, ignoreStatus); errs != nil {
			ret = append(ret, errs...)
		}
	}
	return ret
}

func (m *SmartNIC) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error

	dlmtr := "."
	if path == "" {
		dlmtr = ""
	}
	npath := path + dlmtr + "Spec"
	if errs := m.Spec.Validate(ver, npath, ignoreStatus); errs != nil {
		ret = append(ret, errs...)
	}
	if !ignoreStatus {

		dlmtr := "."
		if path == "" {
			dlmtr = ""
		}
		npath := path + dlmtr + "Status"
		if errs := m.Status.Validate(ver, npath, ignoreStatus); errs != nil {
			ret = append(ret, errs...)
		}
	}
	return ret
}

func (m *SmartNICCondition) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	if vs, ok := validatorMapCmd["SmartNICCondition"][ver]; ok {
		for _, v := range vs {
			if err := v(path, m); err != nil {
				ret = append(ret, err)
			}
		}
	} else if vs, ok := validatorMapCmd["SmartNICCondition"]["all"]; ok {
		for _, v := range vs {
			if err := v(path, m); err != nil {
				ret = append(ret, err)
			}
		}
	}
	return ret
}

func (m *SmartNICSpec) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	if vs, ok := validatorMapCmd["SmartNICSpec"][ver]; ok {
		for _, v := range vs {
			if err := v(path, m); err != nil {
				ret = append(ret, err)
			}
		}
	} else if vs, ok := validatorMapCmd["SmartNICSpec"]["all"]; ok {
		for _, v := range vs {
			if err := v(path, m); err != nil {
				ret = append(ret, err)
			}
		}
	}
	return ret
}

func (m *SmartNICStatus) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	for k, v := range m.Conditions {
		dlmtr := "."
		if path == "" {
			dlmtr = ""
		}
		npath := fmt.Sprintf("%s%sConditions[%d]", path, dlmtr, k)
		if errs := v.Validate(ver, npath, ignoreStatus); errs != nil {
			ret = append(ret, errs...)
		}
	}
	for k, v := range m.Ports {
		dlmtr := "."
		if path == "" {
			dlmtr = ""
		}
		npath := fmt.Sprintf("%s%sPorts[%d]", path, dlmtr, k)
		if errs := v.Validate(ver, npath, ignoreStatus); errs != nil {
			ret = append(ret, errs...)
		}
	}
	return ret
}

func init() {
	scheme := runtime.GetDefaultScheme()
	scheme.AddKnownTypes(
		&Cluster{},
		&Host{},
		&Node{},
		&SmartNIC{},
	)

	validatorMapCmd = make(map[string]map[string][]func(string, interface{}) error)

	validatorMapCmd["HostStatus"] = make(map[string][]func(string, interface{}) error)
	validatorMapCmd["HostStatus"]["all"] = append(validatorMapCmd["HostStatus"]["all"], func(path string, i interface{}) error {
		m := i.(*HostStatus)

		if _, ok := HostStatus_HostType_value[m.Type]; !ok {
			return errors.New("HostStatus.Type did not match allowed strings")
		}
		return nil
	})

	validatorMapCmd["NodeCondition"] = make(map[string][]func(string, interface{}) error)
	validatorMapCmd["NodeCondition"]["all"] = append(validatorMapCmd["NodeCondition"]["all"], func(path string, i interface{}) error {
		m := i.(*NodeCondition)

		if _, ok := ConditionStatus_value[m.Status]; !ok {
			return errors.New("NodeCondition.Status did not match allowed strings")
		}
		return nil
	})

	validatorMapCmd["NodeCondition"]["all"] = append(validatorMapCmd["NodeCondition"]["all"], func(path string, i interface{}) error {
		m := i.(*NodeCondition)

		if _, ok := NodeCondition_ConditionType_value[m.Type]; !ok {
			return errors.New("NodeCondition.Type did not match allowed strings")
		}
		return nil
	})

	validatorMapCmd["NodeSpec"] = make(map[string][]func(string, interface{}) error)
	validatorMapCmd["NodeSpec"]["all"] = append(validatorMapCmd["NodeSpec"]["all"], func(path string, i interface{}) error {
		m := i.(*NodeSpec)

		for k, v := range m.Roles {
			if _, ok := NodeSpec_NodeRole_value[v]; !ok {
				return fmt.Errorf("%v[%v] did not match allowed strings", path+"."+"Roles", k)
			}
		}
		return nil
	})

	validatorMapCmd["NodeStatus"] = make(map[string][]func(string, interface{}) error)
	validatorMapCmd["NodeStatus"]["all"] = append(validatorMapCmd["NodeStatus"]["all"], func(path string, i interface{}) error {
		m := i.(*NodeStatus)

		if _, ok := NodeStatus_NodePhase_value[m.Phase]; !ok {
			return errors.New("NodeStatus.Phase did not match allowed strings")
		}
		return nil
	})

	validatorMapCmd["PortCondition"] = make(map[string][]func(string, interface{}) error)
	validatorMapCmd["PortCondition"]["all"] = append(validatorMapCmd["PortCondition"]["all"], func(path string, i interface{}) error {
		m := i.(*PortCondition)

		if _, ok := ConditionStatus_value[m.Status]; !ok {
			return errors.New("PortCondition.Status did not match allowed strings")
		}
		return nil
	})

	validatorMapCmd["PortCondition"]["all"] = append(validatorMapCmd["PortCondition"]["all"], func(path string, i interface{}) error {
		m := i.(*PortCondition)

		if _, ok := PortCondition_ConditionType_value[m.Type]; !ok {
			return errors.New("PortCondition.Type did not match allowed strings")
		}
		return nil
	})

	validatorMapCmd["SmartNICCondition"] = make(map[string][]func(string, interface{}) error)
	validatorMapCmd["SmartNICCondition"]["all"] = append(validatorMapCmd["SmartNICCondition"]["all"], func(path string, i interface{}) error {
		m := i.(*SmartNICCondition)

		if _, ok := ConditionStatus_value[m.Status]; !ok {
			return errors.New("SmartNICCondition.Status did not match allowed strings")
		}
		return nil
	})

	validatorMapCmd["SmartNICCondition"]["all"] = append(validatorMapCmd["SmartNICCondition"]["all"], func(path string, i interface{}) error {
		m := i.(*SmartNICCondition)

		if _, ok := SmartNICCondition_ConditionType_value[m.Type]; !ok {
			return errors.New("SmartNICCondition.Type did not match allowed strings")
		}
		return nil
	})

	validatorMapCmd["SmartNICSpec"] = make(map[string][]func(string, interface{}) error)
	validatorMapCmd["SmartNICSpec"]["all"] = append(validatorMapCmd["SmartNICSpec"]["all"], func(path string, i interface{}) error {
		m := i.(*SmartNICSpec)

		if _, ok := SmartNICSpec_SmartNICPhase_value[m.Phase]; !ok {
			return errors.New("SmartNICSpec.Phase did not match allowed strings")
		}
		return nil
	})

}
