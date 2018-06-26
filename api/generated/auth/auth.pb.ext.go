// Code generated by protoc-gen-grpc-pensando DO NOT EDIT.

/*
Package auth is a auto generated package.
Input file: auth.proto
*/
package auth

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
var validatorMapAuth = make(map[string]map[string][]func(string, interface{}) error)

// MakeKey generates a KV store key for the object
func (m *AuthenticationPolicy) MakeKey(prefix string) string {
	return fmt.Sprint(globals.RootPrefix, "/", prefix, "/", "authn-policy/", m.Name)
}

func (m *AuthenticationPolicy) MakeURI(ver, prefix string) string {
	in := m
	return fmt.Sprint("/", ver, "/", prefix, "/authn-policy/", in.Name)
}

// MakeKey generates a KV store key for the object
func (m *Role) MakeKey(prefix string) string {
	return fmt.Sprint(globals.RootPrefix, "/", prefix, "/", "roles/", m.Tenant, "/", m.Name)
}

func (m *Role) MakeURI(ver, prefix string) string {
	in := m
	return fmt.Sprint("/", ver, "/", prefix, "/", in.Tenant, "/roles/", in.Name)
}

// MakeKey generates a KV store key for the object
func (m *RoleBinding) MakeKey(prefix string) string {
	return fmt.Sprint(globals.RootPrefix, "/", prefix, "/", "role-bindings/", m.Tenant, "/", m.Name)
}

func (m *RoleBinding) MakeURI(ver, prefix string) string {
	in := m
	return fmt.Sprint("/", ver, "/", prefix, "/", in.Tenant, "/role-bindings/", in.Name)
}

// MakeKey generates a KV store key for the object
func (m *User) MakeKey(prefix string) string {
	return fmt.Sprint(globals.RootPrefix, "/", prefix, "/", "users/", m.Tenant, "/", m.Name)
}

func (m *User) MakeURI(ver, prefix string) string {
	in := m
	return fmt.Sprint("/", ver, "/", prefix, "/", in.Tenant, "/users/", in.Name)
}

// Clone clones the object into into or creates one of into is nil
func (m *AuthenticationPolicy) Clone(into interface{}) (interface{}, error) {
	var out *AuthenticationPolicy
	var ok bool
	if into == nil {
		out = &AuthenticationPolicy{}
	} else {
		out, ok = into.(*AuthenticationPolicy)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *AuthenticationPolicy) Defaults(ver string) bool {
	var ret bool
	ret = m.Spec.Defaults(ver) || ret
	return ret
}

// Clone clones the object into into or creates one of into is nil
func (m *AuthenticationPolicySpec) Clone(into interface{}) (interface{}, error) {
	var out *AuthenticationPolicySpec
	var ok bool
	if into == nil {
		out = &AuthenticationPolicySpec{}
	} else {
		out, ok = into.(*AuthenticationPolicySpec)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *AuthenticationPolicySpec) Defaults(ver string) bool {
	var ret bool
	ret = m.Authenticators.Defaults(ver) || ret
	return ret
}

// Clone clones the object into into or creates one of into is nil
func (m *AuthenticationPolicyStatus) Clone(into interface{}) (interface{}, error) {
	var out *AuthenticationPolicyStatus
	var ok bool
	if into == nil {
		out = &AuthenticationPolicyStatus{}
	} else {
		out, ok = into.(*AuthenticationPolicyStatus)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *AuthenticationPolicyStatus) Defaults(ver string) bool {
	return false
}

// Clone clones the object into into or creates one of into is nil
func (m *Authenticators) Clone(into interface{}) (interface{}, error) {
	var out *Authenticators
	var ok bool
	if into == nil {
		out = &Authenticators{}
	} else {
		out, ok = into.(*Authenticators)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *Authenticators) Defaults(ver string) bool {
	var ret bool
	if m.Radius != nil {
		ret = m.Radius.Defaults(ver) || ret
	}
	ret = true
	switch ver {
	default:
		for k := range m.AuthenticatorOrder {
			m.AuthenticatorOrder[k] = "LOCAL"
		}
	}
	return ret
}

// Clone clones the object into into or creates one of into is nil
func (m *Ldap) Clone(into interface{}) (interface{}, error) {
	var out *Ldap
	var ok bool
	if into == nil {
		out = &Ldap{}
	} else {
		out, ok = into.(*Ldap)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *Ldap) Defaults(ver string) bool {
	return false
}

// Clone clones the object into into or creates one of into is nil
func (m *LdapAttributeMapping) Clone(into interface{}) (interface{}, error) {
	var out *LdapAttributeMapping
	var ok bool
	if into == nil {
		out = &LdapAttributeMapping{}
	} else {
		out, ok = into.(*LdapAttributeMapping)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *LdapAttributeMapping) Defaults(ver string) bool {
	return false
}

// Clone clones the object into into or creates one of into is nil
func (m *LdapServer) Clone(into interface{}) (interface{}, error) {
	var out *LdapServer
	var ok bool
	if into == nil {
		out = &LdapServer{}
	} else {
		out, ok = into.(*LdapServer)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *LdapServer) Defaults(ver string) bool {
	return false
}

// Clone clones the object into into or creates one of into is nil
func (m *Local) Clone(into interface{}) (interface{}, error) {
	var out *Local
	var ok bool
	if into == nil {
		out = &Local{}
	} else {
		out, ok = into.(*Local)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *Local) Defaults(ver string) bool {
	return false
}

// Clone clones the object into into or creates one of into is nil
func (m *PasswordCredential) Clone(into interface{}) (interface{}, error) {
	var out *PasswordCredential
	var ok bool
	if into == nil {
		out = &PasswordCredential{}
	} else {
		out, ok = into.(*PasswordCredential)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *PasswordCredential) Defaults(ver string) bool {
	return false
}

// Clone clones the object into into or creates one of into is nil
func (m *Permission) Clone(into interface{}) (interface{}, error) {
	var out *Permission
	var ok bool
	if into == nil {
		out = &Permission{}
	} else {
		out, ok = into.(*Permission)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *Permission) Defaults(ver string) bool {
	var ret bool
	ret = true
	switch ver {
	default:
		for k := range m.Actions {
			m.Actions[k] = "ALL_ACTIONS"
		}
		m.ResourceKind = "ALL_RESOURCE_KINDS"
	}
	return ret
}

// Clone clones the object into into or creates one of into is nil
func (m *Radius) Clone(into interface{}) (interface{}, error) {
	var out *Radius
	var ok bool
	if into == nil {
		out = &Radius{}
	} else {
		out, ok = into.(*Radius)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *Radius) Defaults(ver string) bool {
	var ret bool
	for k := range m.Servers {
		if m.Servers[k] != nil {
			ret = m.Servers[k].Defaults(ver) || ret
		}
	}
	return ret
}

// Clone clones the object into into or creates one of into is nil
func (m *RadiusServer) Clone(into interface{}) (interface{}, error) {
	var out *RadiusServer
	var ok bool
	if into == nil {
		out = &RadiusServer{}
	} else {
		out, ok = into.(*RadiusServer)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *RadiusServer) Defaults(ver string) bool {
	var ret bool
	ret = true
	switch ver {
	default:
		m.AuthMethod = "PAP"
	}
	return ret
}

// Clone clones the object into into or creates one of into is nil
func (m *Role) Clone(into interface{}) (interface{}, error) {
	var out *Role
	var ok bool
	if into == nil {
		out = &Role{}
	} else {
		out, ok = into.(*Role)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *Role) Defaults(ver string) bool {
	var ret bool
	ret = m.Spec.Defaults(ver) || ret
	return ret
}

// Clone clones the object into into or creates one of into is nil
func (m *RoleBinding) Clone(into interface{}) (interface{}, error) {
	var out *RoleBinding
	var ok bool
	if into == nil {
		out = &RoleBinding{}
	} else {
		out, ok = into.(*RoleBinding)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *RoleBinding) Defaults(ver string) bool {
	return false
}

// Clone clones the object into into or creates one of into is nil
func (m *RoleBindingSpec) Clone(into interface{}) (interface{}, error) {
	var out *RoleBindingSpec
	var ok bool
	if into == nil {
		out = &RoleBindingSpec{}
	} else {
		out, ok = into.(*RoleBindingSpec)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *RoleBindingSpec) Defaults(ver string) bool {
	return false
}

// Clone clones the object into into or creates one of into is nil
func (m *RoleBindingStatus) Clone(into interface{}) (interface{}, error) {
	var out *RoleBindingStatus
	var ok bool
	if into == nil {
		out = &RoleBindingStatus{}
	} else {
		out, ok = into.(*RoleBindingStatus)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *RoleBindingStatus) Defaults(ver string) bool {
	return false
}

// Clone clones the object into into or creates one of into is nil
func (m *RoleSpec) Clone(into interface{}) (interface{}, error) {
	var out *RoleSpec
	var ok bool
	if into == nil {
		out = &RoleSpec{}
	} else {
		out, ok = into.(*RoleSpec)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *RoleSpec) Defaults(ver string) bool {
	var ret bool
	for k := range m.Permissions {
		ret = m.Permissions[k].Defaults(ver) || ret
	}
	return ret
}

// Clone clones the object into into or creates one of into is nil
func (m *RoleStatus) Clone(into interface{}) (interface{}, error) {
	var out *RoleStatus
	var ok bool
	if into == nil {
		out = &RoleStatus{}
	} else {
		out, ok = into.(*RoleStatus)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *RoleStatus) Defaults(ver string) bool {
	return false
}

// Clone clones the object into into or creates one of into is nil
func (m *TLSOptions) Clone(into interface{}) (interface{}, error) {
	var out *TLSOptions
	var ok bool
	if into == nil {
		out = &TLSOptions{}
	} else {
		out, ok = into.(*TLSOptions)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *TLSOptions) Defaults(ver string) bool {
	return false
}

// Clone clones the object into into or creates one of into is nil
func (m *User) Clone(into interface{}) (interface{}, error) {
	var out *User
	var ok bool
	if into == nil {
		out = &User{}
	} else {
		out, ok = into.(*User)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *User) Defaults(ver string) bool {
	var ret bool
	ret = m.Spec.Defaults(ver) || ret
	return ret
}

// Clone clones the object into into or creates one of into is nil
func (m *UserSpec) Clone(into interface{}) (interface{}, error) {
	var out *UserSpec
	var ok bool
	if into == nil {
		out = &UserSpec{}
	} else {
		out, ok = into.(*UserSpec)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *UserSpec) Defaults(ver string) bool {
	var ret bool
	ret = true
	switch ver {
	default:
		m.Type = "LOCAL"
	}
	return ret
}

// Clone clones the object into into or creates one of into is nil
func (m *UserStatus) Clone(into interface{}) (interface{}, error) {
	var out *UserStatus
	var ok bool
	if into == nil {
		out = &UserStatus{}
	} else {
		out, ok = into.(*UserStatus)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *UserStatus) Defaults(ver string) bool {
	return false
}

// Validators

func (m *AuthenticationPolicy) Validate(ver, path string, ignoreStatus bool) []error {
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

func (m *AuthenticationPolicySpec) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error

	dlmtr := "."
	if path == "" {
		dlmtr = ""
	}
	npath := path + dlmtr + "Authenticators"
	if errs := m.Authenticators.Validate(ver, npath, ignoreStatus); errs != nil {
		ret = append(ret, errs...)
	}
	return ret
}

func (m *AuthenticationPolicyStatus) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	return ret
}

func (m *Authenticators) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	if m.Radius != nil {
		dlmtr := "."
		if path == "" {
			dlmtr = ""
		}
		npath := path + dlmtr + "Radius"
		if errs := m.Radius.Validate(ver, npath, ignoreStatus); errs != nil {
			ret = append(ret, errs...)
		}
	}
	if vs, ok := validatorMapAuth["Authenticators"][ver]; ok {
		for _, v := range vs {
			if err := v(path, m); err != nil {
				ret = append(ret, err)
			}
		}
	} else if vs, ok := validatorMapAuth["Authenticators"]["all"]; ok {
		for _, v := range vs {
			if err := v(path, m); err != nil {
				ret = append(ret, err)
			}
		}
	}
	return ret
}

func (m *Ldap) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	return ret
}

func (m *LdapAttributeMapping) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	return ret
}

func (m *LdapServer) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	return ret
}

func (m *Local) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	return ret
}

func (m *PasswordCredential) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	return ret
}

func (m *Permission) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	if vs, ok := validatorMapAuth["Permission"][ver]; ok {
		for _, v := range vs {
			if err := v(path, m); err != nil {
				ret = append(ret, err)
			}
		}
	} else if vs, ok := validatorMapAuth["Permission"]["all"]; ok {
		for _, v := range vs {
			if err := v(path, m); err != nil {
				ret = append(ret, err)
			}
		}
	}
	return ret
}

func (m *Radius) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	for k, v := range m.Servers {
		dlmtr := "."
		if path == "" {
			dlmtr = ""
		}
		npath := fmt.Sprintf("%s%sServers[%d]", path, dlmtr, k)
		if errs := v.Validate(ver, npath, ignoreStatus); errs != nil {
			ret = append(ret, errs...)
		}
	}
	return ret
}

func (m *RadiusServer) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	if vs, ok := validatorMapAuth["RadiusServer"][ver]; ok {
		for _, v := range vs {
			if err := v(path, m); err != nil {
				ret = append(ret, err)
			}
		}
	} else if vs, ok := validatorMapAuth["RadiusServer"]["all"]; ok {
		for _, v := range vs {
			if err := v(path, m); err != nil {
				ret = append(ret, err)
			}
		}
	}
	return ret
}

func (m *Role) Validate(ver, path string, ignoreStatus bool) []error {
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

func (m *RoleBinding) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	return ret
}

func (m *RoleBindingSpec) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	return ret
}

func (m *RoleBindingStatus) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	return ret
}

func (m *RoleSpec) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	for k, v := range m.Permissions {
		dlmtr := "."
		if path == "" {
			dlmtr = ""
		}
		npath := fmt.Sprintf("%s%sPermissions[%d]", path, dlmtr, k)
		if errs := v.Validate(ver, npath, ignoreStatus); errs != nil {
			ret = append(ret, errs...)
		}
	}
	return ret
}

func (m *RoleStatus) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	return ret
}

func (m *TLSOptions) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	return ret
}

func (m *User) Validate(ver, path string, ignoreStatus bool) []error {
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

func (m *UserSpec) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	if vs, ok := validatorMapAuth["UserSpec"][ver]; ok {
		for _, v := range vs {
			if err := v(path, m); err != nil {
				ret = append(ret, err)
			}
		}
	} else if vs, ok := validatorMapAuth["UserSpec"]["all"]; ok {
		for _, v := range vs {
			if err := v(path, m); err != nil {
				ret = append(ret, err)
			}
		}
	}
	return ret
}

func (m *UserStatus) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	return ret
}

func init() {
	scheme := runtime.GetDefaultScheme()
	scheme.AddKnownTypes(
		&AuthenticationPolicy{},
		&Role{},
		&RoleBinding{},
		&User{},
	)

	validatorMapAuth = make(map[string]map[string][]func(string, interface{}) error)

	validatorMapAuth["Authenticators"] = make(map[string][]func(string, interface{}) error)
	validatorMapAuth["Authenticators"]["all"] = append(validatorMapAuth["Authenticators"]["all"], func(path string, i interface{}) error {
		m := i.(*Authenticators)

		for k, v := range m.AuthenticatorOrder {
			if _, ok := Authenticators_AuthenticatorType_value[v]; !ok {
				return fmt.Errorf("%v[%v] did not match allowed strings", path+"."+"AuthenticatorOrder", k)
			}
		}
		return nil
	})

	validatorMapAuth["Permission"] = make(map[string][]func(string, interface{}) error)
	validatorMapAuth["Permission"]["all"] = append(validatorMapAuth["Permission"]["all"], func(path string, i interface{}) error {
		m := i.(*Permission)

		for k, v := range m.Actions {
			if _, ok := Permission_ActionType_value[v]; !ok {
				return fmt.Errorf("%v[%v] did not match allowed strings", path+"."+"Actions", k)
			}
		}
		return nil
	})

	validatorMapAuth["Permission"]["all"] = append(validatorMapAuth["Permission"]["all"], func(path string, i interface{}) error {
		m := i.(*Permission)

		if _, ok := Permission_ResrcKind_value[m.ResourceKind]; !ok {
			return errors.New("Permission.ResourceKind did not match allowed strings")
		}
		return nil
	})

	validatorMapAuth["RadiusServer"] = make(map[string][]func(string, interface{}) error)
	validatorMapAuth["RadiusServer"]["all"] = append(validatorMapAuth["RadiusServer"]["all"], func(path string, i interface{}) error {
		m := i.(*RadiusServer)

		if _, ok := Radius_AuthMethod_value[m.AuthMethod]; !ok {
			return errors.New("RadiusServer.AuthMethod did not match allowed strings")
		}
		return nil
	})

	validatorMapAuth["UserSpec"] = make(map[string][]func(string, interface{}) error)
	validatorMapAuth["UserSpec"]["all"] = append(validatorMapAuth["UserSpec"]["all"], func(path string, i interface{}) error {
		m := i.(*UserSpec)

		if _, ok := UserSpec_UserType_value[m.Type]; !ok {
			return errors.New("UserSpec.Type did not match allowed strings")
		}
		return nil
	})

}
