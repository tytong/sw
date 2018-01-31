// Generated by tmpl
// https://github.com/benbjohnson/tmpl
//
// DO NOT EDIT!
// Source: iterator.gen_test.go.tmpl

package cachedb

import (
	"testing"

	"github.com/influxdata/influxdb/pkg/metrics"
	"github.com/influxdata/influxdb/pkg/tracing"
	"github.com/influxdata/influxdb/query"
)

var (
	testGroup   = metrics.MustRegisterGroup("testg")
	testCounter = metrics.MustRegisterCounter("cursors_ref", metrics.WithGroup(testGroup))
)

func getFloatValuesFromSlice(t []int64) Values {
	iv := make(FloatValues, len(t))
	ret := make(Values, len(t))
	for i, v := range t {
		iv[i].unixnano = v
		ret[i] = iv[i]
	}
	return ret
}

func TestFloatIterator_Next(t *testing.T) {
	vals := []int64{10, 7, 45, 66, 31, 2, 77}
	cv := getFloatValuesFromSlice(vals)
	// seek to last item
	curs := newFloatCursor(int64(77), false, cv, nil)
	opt := query.IteratorOptions{}
	aux := []cursorAt{}
	it := newFloatIterator("blah", query.Tags{}, opt, curs, aux, nil, nil)
	count := 0
	for {
		p, err := it.Next()
		if err != nil {
			t.Fatal(err)
		}
		if p == nil {
			break
		}
		count++
	}

	if count != len(vals) {
		t.Errorf("Exp %d, got %d", len(vals), count)
	}

	it.Close()
}

func TestInstrFloatIterator_Next(t *testing.T) {
	group := metrics.NewGroup(testGroup)
	_, span := tracing.NewTrace("foo")

	vals := []int64{10, 7, 45, 66, 31, 2, 77}
	cv := getFloatValuesFromSlice(vals)
	// seek to last item
	curs := newFloatCursor(int64(77), false, cv, nil)
	opt := query.IteratorOptions{}
	aux := []cursorAt{}
	iit := newFloatIterator("blah", query.Tags{}, opt, curs, aux, nil, nil)
	it := newFloatInstrumentedIterator(iit, span, group)

	//	count := 0
	//	for {
	//                p, err := it.Next()
	//                if err != nil {
	//                        t.Fatal(err)
	//                }
	//                if p == nil {
	//                        break
	//                }
	//                count++
	//        }
	//
	//	if count != len(vals) {
	//		t.Errorf("Exp %d, got %d", len(vals), count)
	//	}

	it.Close()
}

func getIntegerValuesFromSlice(t []int64) Values {
	iv := make(IntegerValues, len(t))
	ret := make(Values, len(t))
	for i, v := range t {
		iv[i].unixnano = v
		ret[i] = iv[i]
	}
	return ret
}

func TestIntegerIterator_Next(t *testing.T) {
	vals := []int64{10, 7, 45, 66, 31, 2, 77}
	cv := getIntegerValuesFromSlice(vals)
	// seek to last item
	curs := newIntegerCursor(int64(77), false, cv, nil)
	opt := query.IteratorOptions{}
	aux := []cursorAt{}
	it := newIntegerIterator("blah", query.Tags{}, opt, curs, aux, nil, nil)
	count := 0
	for {
		p, err := it.Next()
		if err != nil {
			t.Fatal(err)
		}
		if p == nil {
			break
		}
		count++
	}

	if count != len(vals) {
		t.Errorf("Exp %d, got %d", len(vals), count)
	}

	it.Close()
}

func TestInstrIntegerIterator_Next(t *testing.T) {
	group := metrics.NewGroup(testGroup)
	_, span := tracing.NewTrace("foo")

	vals := []int64{10, 7, 45, 66, 31, 2, 77}
	cv := getIntegerValuesFromSlice(vals)
	// seek to last item
	curs := newIntegerCursor(int64(77), false, cv, nil)
	opt := query.IteratorOptions{}
	aux := []cursorAt{}
	iit := newIntegerIterator("blah", query.Tags{}, opt, curs, aux, nil, nil)
	it := newIntegerInstrumentedIterator(iit, span, group)

	//	count := 0
	//	for {
	//                p, err := it.Next()
	//                if err != nil {
	//                        t.Fatal(err)
	//                }
	//                if p == nil {
	//                        break
	//                }
	//                count++
	//        }
	//
	//	if count != len(vals) {
	//		t.Errorf("Exp %d, got %d", len(vals), count)
	//	}

	it.Close()
}

func getUnsignedValuesFromSlice(t []int64) Values {
	iv := make(UnsignedValues, len(t))
	ret := make(Values, len(t))
	for i, v := range t {
		iv[i].unixnano = v
		ret[i] = iv[i]
	}
	return ret
}

func TestUnsignedIterator_Next(t *testing.T) {
	vals := []int64{10, 7, 45, 66, 31, 2, 77}
	cv := getUnsignedValuesFromSlice(vals)
	// seek to last item
	curs := newUnsignedCursor(int64(77), false, cv, nil)
	opt := query.IteratorOptions{}
	aux := []cursorAt{}
	it := newUnsignedIterator("blah", query.Tags{}, opt, curs, aux, nil, nil)
	count := 0
	for {
		p, err := it.Next()
		if err != nil {
			t.Fatal(err)
		}
		if p == nil {
			break
		}
		count++
	}

	if count != len(vals) {
		t.Errorf("Exp %d, got %d", len(vals), count)
	}

	it.Close()
}

func TestInstrUnsignedIterator_Next(t *testing.T) {
	group := metrics.NewGroup(testGroup)
	_, span := tracing.NewTrace("foo")

	vals := []int64{10, 7, 45, 66, 31, 2, 77}
	cv := getUnsignedValuesFromSlice(vals)
	// seek to last item
	curs := newUnsignedCursor(int64(77), false, cv, nil)
	opt := query.IteratorOptions{}
	aux := []cursorAt{}
	iit := newUnsignedIterator("blah", query.Tags{}, opt, curs, aux, nil, nil)
	it := newUnsignedInstrumentedIterator(iit, span, group)

	//	count := 0
	//	for {
	//                p, err := it.Next()
	//                if err != nil {
	//                        t.Fatal(err)
	//                }
	//                if p == nil {
	//                        break
	//                }
	//                count++
	//        }
	//
	//	if count != len(vals) {
	//		t.Errorf("Exp %d, got %d", len(vals), count)
	//	}

	it.Close()
}

func getStringValuesFromSlice(t []int64) Values {
	iv := make(StringValues, len(t))
	ret := make(Values, len(t))
	for i, v := range t {
		iv[i].unixnano = v
		ret[i] = iv[i]
	}
	return ret
}

func TestStringIterator_Next(t *testing.T) {
	vals := []int64{10, 7, 45, 66, 31, 2, 77}
	cv := getStringValuesFromSlice(vals)
	// seek to last item
	curs := newStringCursor(int64(77), false, cv, nil)
	opt := query.IteratorOptions{}
	aux := []cursorAt{}
	it := newStringIterator("blah", query.Tags{}, opt, curs, aux, nil, nil)
	count := 0
	for {
		p, err := it.Next()
		if err != nil {
			t.Fatal(err)
		}
		if p == nil {
			break
		}
		count++
	}

	if count != len(vals) {
		t.Errorf("Exp %d, got %d", len(vals), count)
	}

	it.Close()
}

func TestInstrStringIterator_Next(t *testing.T) {
	group := metrics.NewGroup(testGroup)
	_, span := tracing.NewTrace("foo")

	vals := []int64{10, 7, 45, 66, 31, 2, 77}
	cv := getStringValuesFromSlice(vals)
	// seek to last item
	curs := newStringCursor(int64(77), false, cv, nil)
	opt := query.IteratorOptions{}
	aux := []cursorAt{}
	iit := newStringIterator("blah", query.Tags{}, opt, curs, aux, nil, nil)
	it := newStringInstrumentedIterator(iit, span, group)

	//	count := 0
	//	for {
	//                p, err := it.Next()
	//                if err != nil {
	//                        t.Fatal(err)
	//                }
	//                if p == nil {
	//                        break
	//                }
	//                count++
	//        }
	//
	//	if count != len(vals) {
	//		t.Errorf("Exp %d, got %d", len(vals), count)
	//	}

	it.Close()
}

func getBooleanValuesFromSlice(t []int64) Values {
	iv := make(BooleanValues, len(t))
	ret := make(Values, len(t))
	for i, v := range t {
		iv[i].unixnano = v
		ret[i] = iv[i]
	}
	return ret
}

func TestBooleanIterator_Next(t *testing.T) {
	vals := []int64{10, 7, 45, 66, 31, 2, 77}
	cv := getBooleanValuesFromSlice(vals)
	// seek to last item
	curs := newBooleanCursor(int64(77), false, cv, nil)
	opt := query.IteratorOptions{}
	aux := []cursorAt{}
	it := newBooleanIterator("blah", query.Tags{}, opt, curs, aux, nil, nil)
	count := 0
	for {
		p, err := it.Next()
		if err != nil {
			t.Fatal(err)
		}
		if p == nil {
			break
		}
		count++
	}

	if count != len(vals) {
		t.Errorf("Exp %d, got %d", len(vals), count)
	}

	it.Close()
}

func TestInstrBooleanIterator_Next(t *testing.T) {
	group := metrics.NewGroup(testGroup)
	_, span := tracing.NewTrace("foo")

	vals := []int64{10, 7, 45, 66, 31, 2, 77}
	cv := getBooleanValuesFromSlice(vals)
	// seek to last item
	curs := newBooleanCursor(int64(77), false, cv, nil)
	opt := query.IteratorOptions{}
	aux := []cursorAt{}
	iit := newBooleanIterator("blah", query.Tags{}, opt, curs, aux, nil, nil)
	it := newBooleanInstrumentedIterator(iit, span, group)

	//	count := 0
	//	for {
	//                p, err := it.Next()
	//                if err != nil {
	//                        t.Fatal(err)
	//                }
	//                if p == nil {
	//                        break
	//                }
	//                count++
	//        }
	//
	//	if count != len(vals) {
	//		t.Errorf("Exp %d, got %d", len(vals), count)
	//	}

	it.Close()
}
