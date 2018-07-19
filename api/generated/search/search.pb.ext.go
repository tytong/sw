// Code generated by protoc-gen-grpc-pensando DO NOT EDIT.

/*
Package search is a auto generated package.
Input file: search.proto
*/
package search

import (
	"errors"
	fmt "fmt"

	listerwatcher "github.com/pensando/sw/api/listerwatcher"
	"github.com/pensando/sw/venice/utils/kvstore"
	"github.com/pensando/sw/venice/utils/log"
	"github.com/pensando/sw/venice/utils/runtime"

	validators "github.com/pensando/sw/venice/utils/apigen/validators"
)

// Dummy definitions to suppress nonused warnings
var _ kvstore.Interface
var _ log.Logger
var _ listerwatcher.WatcherClient

var _ validators.DummyVar
var validatorMapSearch = make(map[string]map[string][]func(string, interface{}) error)

// Clone clones the object into into or creates one of into is nil
func (m *Category) Clone(into interface{}) (interface{}, error) {
	var out *Category
	var ok bool
	if into == nil {
		out = &Category{}
	} else {
		out, ok = into.(*Category)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *Category) Defaults(ver string) bool {
	return false
}

// Clone clones the object into into or creates one of into is nil
func (m *CategoryAggregation) Clone(into interface{}) (interface{}, error) {
	var out *CategoryAggregation
	var ok bool
	if into == nil {
		out = &CategoryAggregation{}
	} else {
		out, ok = into.(*CategoryAggregation)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *CategoryAggregation) Defaults(ver string) bool {
	return false
}

// Clone clones the object into into or creates one of into is nil
func (m *CategoryPreview) Clone(into interface{}) (interface{}, error) {
	var out *CategoryPreview
	var ok bool
	if into == nil {
		out = &CategoryPreview{}
	} else {
		out, ok = into.(*CategoryPreview)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *CategoryPreview) Defaults(ver string) bool {
	return false
}

// Clone clones the object into into or creates one of into is nil
func (m *Entry) Clone(into interface{}) (interface{}, error) {
	var out *Entry
	var ok bool
	if into == nil {
		out = &Entry{}
	} else {
		out, ok = into.(*Entry)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *Entry) Defaults(ver string) bool {
	return false
}

// Clone clones the object into into or creates one of into is nil
func (m *EntryList) Clone(into interface{}) (interface{}, error) {
	var out *EntryList
	var ok bool
	if into == nil {
		out = &EntryList{}
	} else {
		out, ok = into.(*EntryList)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *EntryList) Defaults(ver string) bool {
	return false
}

// Clone clones the object into into or creates one of into is nil
func (m *Error) Clone(into interface{}) (interface{}, error) {
	var out *Error
	var ok bool
	if into == nil {
		out = &Error{}
	} else {
		out, ok = into.(*Error)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *Error) Defaults(ver string) bool {
	return false
}

// Clone clones the object into into or creates one of into is nil
func (m *Kind) Clone(into interface{}) (interface{}, error) {
	var out *Kind
	var ok bool
	if into == nil {
		out = &Kind{}
	} else {
		out, ok = into.(*Kind)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *Kind) Defaults(ver string) bool {
	return false
}

// Clone clones the object into into or creates one of into is nil
func (m *KindAggregation) Clone(into interface{}) (interface{}, error) {
	var out *KindAggregation
	var ok bool
	if into == nil {
		out = &KindAggregation{}
	} else {
		out, ok = into.(*KindAggregation)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *KindAggregation) Defaults(ver string) bool {
	return false
}

// Clone clones the object into into or creates one of into is nil
func (m *KindPreview) Clone(into interface{}) (interface{}, error) {
	var out *KindPreview
	var ok bool
	if into == nil {
		out = &KindPreview{}
	} else {
		out, ok = into.(*KindPreview)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *KindPreview) Defaults(ver string) bool {
	return false
}

// Clone clones the object into into or creates one of into is nil
func (m *SearchQuery) Clone(into interface{}) (interface{}, error) {
	var out *SearchQuery
	var ok bool
	if into == nil {
		out = &SearchQuery{}
	} else {
		out, ok = into.(*SearchQuery)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *SearchQuery) Defaults(ver string) bool {
	var ret bool
	for k := range m.Texts {
		if m.Texts[k] != nil {
			i := m.Texts[k]
			ret = i.Defaults(ver) || ret
		}
	}
	ret = true
	switch ver {
	default:
		for k := range m.Categories {
			m.Categories[k] = "Cluster"
		}
		for k := range m.Kinds {
			m.Kinds[k] = "Cluster"
		}
	}
	return ret
}

// Clone clones the object into into or creates one of into is nil
func (m *SearchRequest) Clone(into interface{}) (interface{}, error) {
	var out *SearchRequest
	var ok bool
	if into == nil {
		out = &SearchRequest{}
	} else {
		out, ok = into.(*SearchRequest)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *SearchRequest) Defaults(ver string) bool {
	var ret bool
	if m.Query != nil {
		ret = m.Query.Defaults(ver) || ret
	}
	ret = true
	switch ver {
	default:
		m.MaxResults = 10
		m.Mode = "Full"
	}
	return ret
}

// Clone clones the object into into or creates one of into is nil
func (m *SearchResponse) Clone(into interface{}) (interface{}, error) {
	var out *SearchResponse
	var ok bool
	if into == nil {
		out = &SearchResponse{}
	} else {
		out, ok = into.(*SearchResponse)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *SearchResponse) Defaults(ver string) bool {
	return false
}

// Clone clones the object into into or creates one of into is nil
func (m *TenantAggregation) Clone(into interface{}) (interface{}, error) {
	var out *TenantAggregation
	var ok bool
	if into == nil {
		out = &TenantAggregation{}
	} else {
		out, ok = into.(*TenantAggregation)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *TenantAggregation) Defaults(ver string) bool {
	return false
}

// Clone clones the object into into or creates one of into is nil
func (m *TenantPreview) Clone(into interface{}) (interface{}, error) {
	var out *TenantPreview
	var ok bool
	if into == nil {
		out = &TenantPreview{}
	} else {
		out, ok = into.(*TenantPreview)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *TenantPreview) Defaults(ver string) bool {
	return false
}

// Clone clones the object into into or creates one of into is nil
func (m *TextRequirement) Clone(into interface{}) (interface{}, error) {
	var out *TextRequirement
	var ok bool
	if into == nil {
		out = &TextRequirement{}
	} else {
		out, ok = into.(*TextRequirement)
		if !ok {
			return nil, fmt.Errorf("mismatched object types")
		}
	}
	*out = *m
	return out, nil
}

// Default sets up the defaults for the object
func (m *TextRequirement) Defaults(ver string) bool {
	var ret bool
	return ret
}

// Validators

func (m *Category) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	return ret
}

func (m *CategoryAggregation) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	return ret
}

func (m *CategoryPreview) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	return ret
}

func (m *Entry) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	return ret
}

func (m *EntryList) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	return ret
}

func (m *Error) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	return ret
}

func (m *Kind) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	return ret
}

func (m *KindAggregation) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	return ret
}

func (m *KindPreview) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	return ret
}

func (m *SearchQuery) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	if m.Fields != nil {
		dlmtr := "."
		if path == "" {
			dlmtr = ""
		}
		npath := path + dlmtr + "Fields"
		if errs := m.Fields.Validate(ver, npath, ignoreStatus); errs != nil {
			ret = append(ret, errs...)
		}
	}
	if m.Labels != nil {
		dlmtr := "."
		if path == "" {
			dlmtr = ""
		}
		npath := path + dlmtr + "Labels"
		if errs := m.Labels.Validate(ver, npath, ignoreStatus); errs != nil {
			ret = append(ret, errs...)
		}
	}
	for k, v := range m.Texts {
		dlmtr := "."
		if path == "" {
			dlmtr = ""
		}
		npath := fmt.Sprintf("%s%sTexts[%d]", path, dlmtr, k)
		if errs := v.Validate(ver, npath, ignoreStatus); errs != nil {
			ret = append(ret, errs...)
		}
	}
	if vs, ok := validatorMapSearch["SearchQuery"][ver]; ok {
		for _, v := range vs {
			if err := v(path, m); err != nil {
				ret = append(ret, err)
			}
		}
	} else if vs, ok := validatorMapSearch["SearchQuery"]["all"]; ok {
		for _, v := range vs {
			if err := v(path, m); err != nil {
				ret = append(ret, err)
			}
		}
	}
	return ret
}

func (m *SearchRequest) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	if m.Query != nil {
		dlmtr := "."
		if path == "" {
			dlmtr = ""
		}
		npath := path + dlmtr + "Query"
		if errs := m.Query.Validate(ver, npath, ignoreStatus); errs != nil {
			ret = append(ret, errs...)
		}
	}
	if vs, ok := validatorMapSearch["SearchRequest"][ver]; ok {
		for _, v := range vs {
			if err := v(path, m); err != nil {
				ret = append(ret, err)
			}
		}
	} else if vs, ok := validatorMapSearch["SearchRequest"]["all"]; ok {
		for _, v := range vs {
			if err := v(path, m); err != nil {
				ret = append(ret, err)
			}
		}
	}
	return ret
}

func (m *SearchResponse) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	return ret
}

func (m *TenantAggregation) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	return ret
}

func (m *TenantPreview) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	return ret
}

func (m *TextRequirement) Validate(ver, path string, ignoreStatus bool) []error {
	var ret []error
	if vs, ok := validatorMapSearch["TextRequirement"][ver]; ok {
		for _, v := range vs {
			if err := v(path, m); err != nil {
				ret = append(ret, err)
			}
		}
	} else if vs, ok := validatorMapSearch["TextRequirement"]["all"]; ok {
		for _, v := range vs {
			if err := v(path, m); err != nil {
				ret = append(ret, err)
			}
		}
	}
	return ret
}

// Transformers

func init() {
	scheme := runtime.GetDefaultScheme()
	scheme.AddKnownTypes()

	validatorMapSearch = make(map[string]map[string][]func(string, interface{}) error)

	validatorMapSearch["SearchQuery"] = make(map[string][]func(string, interface{}) error)
	validatorMapSearch["SearchQuery"]["all"] = append(validatorMapSearch["SearchQuery"]["all"], func(path string, i interface{}) error {
		m := i.(*SearchQuery)

		for k, v := range m.Categories {
			if _, ok := Category_Type_value[v]; !ok {
				return fmt.Errorf("%v[%v] did not match allowed strings", path+"."+"Categories", k)
			}
		}
		return nil
	})
	validatorMapSearch["SearchQuery"]["all"] = append(validatorMapSearch["SearchQuery"]["all"], func(path string, i interface{}) error {
		m := i.(*SearchQuery)
		args := make([]string, 0)
		args = append(args, "0")
		args = append(args, "64")

		for _, v := range m.Categories {
			if !validators.StrLen(v, args) {
				return fmt.Errorf("%v failed validation", path+"."+"Categories")
			}
		}
		return nil
	})

	validatorMapSearch["SearchQuery"]["all"] = append(validatorMapSearch["SearchQuery"]["all"], func(path string, i interface{}) error {
		m := i.(*SearchQuery)

		for k, v := range m.Kinds {
			if _, ok := Kind_Type_value[v]; !ok {
				return fmt.Errorf("%v[%v] did not match allowed strings", path+"."+"Kinds", k)
			}
		}
		return nil
	})
	validatorMapSearch["SearchQuery"]["all"] = append(validatorMapSearch["SearchQuery"]["all"], func(path string, i interface{}) error {
		m := i.(*SearchQuery)
		args := make([]string, 0)
		args = append(args, "0")
		args = append(args, "64")

		for _, v := range m.Kinds {
			if !validators.StrLen(v, args) {
				return fmt.Errorf("%v failed validation", path+"."+"Kinds")
			}
		}
		return nil
	})

	validatorMapSearch["SearchRequest"] = make(map[string][]func(string, interface{}) error)
	validatorMapSearch["SearchRequest"]["all"] = append(validatorMapSearch["SearchRequest"]["all"], func(path string, i interface{}) error {
		m := i.(*SearchRequest)
		args := make([]string, 0)
		args = append(args, "0")
		args = append(args, "1023")

		if !validators.IntRange(m.From, args) {
			return fmt.Errorf("%v failed validation", path+"."+"From")
		}
		return nil
	})

	validatorMapSearch["SearchRequest"]["all"] = append(validatorMapSearch["SearchRequest"]["all"], func(path string, i interface{}) error {
		m := i.(*SearchRequest)
		args := make([]string, 0)
		args = append(args, "0")
		args = append(args, "8192")

		if !validators.IntRange(m.MaxResults, args) {
			return fmt.Errorf("%v failed validation", path+"."+"MaxResults")
		}
		return nil
	})

	validatorMapSearch["SearchRequest"]["all"] = append(validatorMapSearch["SearchRequest"]["all"], func(path string, i interface{}) error {
		m := i.(*SearchRequest)

		if _, ok := SearchRequest_RequestMode_value[m.Mode]; !ok {
			return errors.New("SearchRequest.Mode did not match allowed strings")
		}
		return nil
	})

	validatorMapSearch["SearchRequest"]["all"] = append(validatorMapSearch["SearchRequest"]["all"], func(path string, i interface{}) error {
		m := i.(*SearchRequest)
		args := make([]string, 0)
		args = append(args, "0")
		args = append(args, "256")

		if !validators.StrLen(m.QueryString, args) {
			return fmt.Errorf("%v failed validation", path+"."+"QueryString")
		}
		return nil
	})

	validatorMapSearch["SearchRequest"]["all"] = append(validatorMapSearch["SearchRequest"]["all"], func(path string, i interface{}) error {
		m := i.(*SearchRequest)
		args := make([]string, 0)
		args = append(args, "0")
		args = append(args, "256")

		if !validators.StrLen(m.SortBy, args) {
			return fmt.Errorf("%v failed validation", path+"."+"SortBy")
		}
		return nil
	})

	validatorMapSearch["TextRequirement"] = make(map[string][]func(string, interface{}) error)
	validatorMapSearch["TextRequirement"]["all"] = append(validatorMapSearch["TextRequirement"]["all"], func(path string, i interface{}) error {
		m := i.(*TextRequirement)
		args := make([]string, 0)
		args = append(args, "0")
		args = append(args, "256")

		for _, v := range m.Text {
			if !validators.StrLen(v, args) {
				return fmt.Errorf("%v failed validation", path+"."+"Text")
			}
		}
		return nil
	})

}
