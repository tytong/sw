// Code generated by protoc-gen-gogo. DO NOT EDIT.
// source: workload.proto

package workload

import proto "github.com/gogo/protobuf/proto"
import fmt "fmt"
import math "math"
import _ "github.com/pensando/grpc-gateway/third_party/googleapis/google/api"
import _ "github.com/pensando/sw/venice/utils/apigen/annotations"
import _ "github.com/gogo/protobuf/gogoproto"
import api "github.com/pensando/sw/api"
import _ "github.com/pensando/sw/api/labels"

import io "io"

// Reference imports to suppress errors if they are not otherwise used.
var _ = proto.Marshal
var _ = fmt.Errorf
var _ = math.Inf

// ----------------------------- Workload Object -----------------------------
//
// Workload represents a VM, container/pod or Baremetal.
//
type Workload struct {
	//
	api.TypeMeta `protobuf:"bytes,1,opt,name=T,json=,inline,embedded=T" json:",inline"`
	//
	api.ObjectMeta `protobuf:"bytes,2,opt,name=O,json=meta,omitempty,embedded=O" json:"meta,omitempty"`
	// Spec contains the configuration of the Workload.
	Spec WorkloadSpec `protobuf:"bytes,3,opt,name=Spec,json=spec,omitempty" json:"spec,omitempty"`
	// Status contains the current state of the Workload.
	Status WorkloadStatus `protobuf:"bytes,4,opt,name=Status,json=status,omitempty" json:"status,omitempty"`
}

func (m *Workload) Reset()                    { *m = Workload{} }
func (m *Workload) String() string            { return proto.CompactTextString(m) }
func (*Workload) ProtoMessage()               {}
func (*Workload) Descriptor() ([]byte, []int) { return fileDescriptorWorkload, []int{0} }

func (m *Workload) GetSpec() WorkloadSpec {
	if m != nil {
		return m.Spec
	}
	return WorkloadSpec{}
}

func (m *Workload) GetStatus() WorkloadStatus {
	if m != nil {
		return m.Status
	}
	return WorkloadStatus{}
}

// Spec of a Workload interface
type WorkloadIntfSpec struct {
	// Micro-segmentation vlan assigned for this interface
	MicroSegVlan uint32 `protobuf:"varint,1,opt,name=MicroSegVlan,json=micro-seg-vlan,omitempty,proto3" json:"micro-seg-vlan,omitempty"`
	// External vlan assigned for this interface
	ExternalVlan uint32 `protobuf:"varint,2,opt,name=ExternalVlan,json=external-vlan,omitempty,proto3" json:"external-vlan,omitempty"`
}

func (m *WorkloadIntfSpec) Reset()                    { *m = WorkloadIntfSpec{} }
func (m *WorkloadIntfSpec) String() string            { return proto.CompactTextString(m) }
func (*WorkloadIntfSpec) ProtoMessage()               {}
func (*WorkloadIntfSpec) Descriptor() ([]byte, []int) { return fileDescriptorWorkload, []int{1} }

func (m *WorkloadIntfSpec) GetMicroSegVlan() uint32 {
	if m != nil {
		return m.MicroSegVlan
	}
	return 0
}

func (m *WorkloadIntfSpec) GetExternalVlan() uint32 {
	if m != nil {
		return m.ExternalVlan
	}
	return 0
}

// Status of a Workload interface
type WorkloadIntfStatus struct {
	// List of all IP addresses configured and discovered on a Workload Interface
	IpAddrs []string `protobuf:"bytes,1,rep,name=IpAddrs,json=ip-addrs,omitempty" json:"ip-addrs,omitempty"`
}

func (m *WorkloadIntfStatus) Reset()                    { *m = WorkloadIntfStatus{} }
func (m *WorkloadIntfStatus) String() string            { return proto.CompactTextString(m) }
func (*WorkloadIntfStatus) ProtoMessage()               {}
func (*WorkloadIntfStatus) Descriptor() ([]byte, []int) { return fileDescriptorWorkload, []int{2} }

func (m *WorkloadIntfStatus) GetIpAddrs() []string {
	if m != nil {
		return m.IpAddrs
	}
	return nil
}

// Spec part of Workload object
type WorkloadSpec struct {
	// Hostname of the server where the workload is running.
	HostName string `protobuf:"bytes,1,opt,name=HostName,json=host-name,omitempty,proto3" json:"host-name,omitempty"`
	// Spec of all interfaces in the Workload identified by Primary MAC
	Interfaces map[string]WorkloadIntfSpec `protobuf:"bytes,2,rep,name=Interfaces,json=interfaces,omitempty" json:"interfaces,omitempty" protobuf_key:"bytes,1,opt,name=key,proto3" protobuf_val:"bytes,2,opt,name=value"`
}

func (m *WorkloadSpec) Reset()                    { *m = WorkloadSpec{} }
func (m *WorkloadSpec) String() string            { return proto.CompactTextString(m) }
func (*WorkloadSpec) ProtoMessage()               {}
func (*WorkloadSpec) Descriptor() ([]byte, []int) { return fileDescriptorWorkload, []int{3} }

func (m *WorkloadSpec) GetHostName() string {
	if m != nil {
		return m.HostName
	}
	return ""
}

func (m *WorkloadSpec) GetInterfaces() map[string]WorkloadIntfSpec {
	if m != nil {
		return m.Interfaces
	}
	return nil
}

// Status part of Workload object
type WorkloadStatus struct {
	// Status of all interfaces in the Workload identified by Primary MAC
	Interfaces map[string]WorkloadIntfStatus `protobuf:"bytes,1,rep,name=Interfaces,json=interfaces,omitempty" json:"interfaces,omitempty" protobuf_key:"bytes,1,opt,name=key,proto3" protobuf_val:"bytes,2,opt,name=value"`
	// List of all endpoints associated with this Workload
	Endpoints []string `protobuf:"bytes,2,rep,name=Endpoints,json=endpoints,omitempty" json:"endpoints,omitempty"`
}

func (m *WorkloadStatus) Reset()                    { *m = WorkloadStatus{} }
func (m *WorkloadStatus) String() string            { return proto.CompactTextString(m) }
func (*WorkloadStatus) ProtoMessage()               {}
func (*WorkloadStatus) Descriptor() ([]byte, []int) { return fileDescriptorWorkload, []int{4} }

func (m *WorkloadStatus) GetInterfaces() map[string]WorkloadIntfStatus {
	if m != nil {
		return m.Interfaces
	}
	return nil
}

func (m *WorkloadStatus) GetEndpoints() []string {
	if m != nil {
		return m.Endpoints
	}
	return nil
}

func init() {
	proto.RegisterType((*Workload)(nil), "workload.Workload")
	proto.RegisterType((*WorkloadIntfSpec)(nil), "workload.WorkloadIntfSpec")
	proto.RegisterType((*WorkloadIntfStatus)(nil), "workload.WorkloadIntfStatus")
	proto.RegisterType((*WorkloadSpec)(nil), "workload.WorkloadSpec")
	proto.RegisterType((*WorkloadStatus)(nil), "workload.WorkloadStatus")
}
func (m *Workload) Marshal() (dAtA []byte, err error) {
	size := m.Size()
	dAtA = make([]byte, size)
	n, err := m.MarshalTo(dAtA)
	if err != nil {
		return nil, err
	}
	return dAtA[:n], nil
}

func (m *Workload) MarshalTo(dAtA []byte) (int, error) {
	var i int
	_ = i
	var l int
	_ = l
	dAtA[i] = 0xa
	i++
	i = encodeVarintWorkload(dAtA, i, uint64(m.TypeMeta.Size()))
	n1, err := m.TypeMeta.MarshalTo(dAtA[i:])
	if err != nil {
		return 0, err
	}
	i += n1
	dAtA[i] = 0x12
	i++
	i = encodeVarintWorkload(dAtA, i, uint64(m.ObjectMeta.Size()))
	n2, err := m.ObjectMeta.MarshalTo(dAtA[i:])
	if err != nil {
		return 0, err
	}
	i += n2
	dAtA[i] = 0x1a
	i++
	i = encodeVarintWorkload(dAtA, i, uint64(m.Spec.Size()))
	n3, err := m.Spec.MarshalTo(dAtA[i:])
	if err != nil {
		return 0, err
	}
	i += n3
	dAtA[i] = 0x22
	i++
	i = encodeVarintWorkload(dAtA, i, uint64(m.Status.Size()))
	n4, err := m.Status.MarshalTo(dAtA[i:])
	if err != nil {
		return 0, err
	}
	i += n4
	return i, nil
}

func (m *WorkloadIntfSpec) Marshal() (dAtA []byte, err error) {
	size := m.Size()
	dAtA = make([]byte, size)
	n, err := m.MarshalTo(dAtA)
	if err != nil {
		return nil, err
	}
	return dAtA[:n], nil
}

func (m *WorkloadIntfSpec) MarshalTo(dAtA []byte) (int, error) {
	var i int
	_ = i
	var l int
	_ = l
	if m.MicroSegVlan != 0 {
		dAtA[i] = 0x8
		i++
		i = encodeVarintWorkload(dAtA, i, uint64(m.MicroSegVlan))
	}
	if m.ExternalVlan != 0 {
		dAtA[i] = 0x10
		i++
		i = encodeVarintWorkload(dAtA, i, uint64(m.ExternalVlan))
	}
	return i, nil
}

func (m *WorkloadIntfStatus) Marshal() (dAtA []byte, err error) {
	size := m.Size()
	dAtA = make([]byte, size)
	n, err := m.MarshalTo(dAtA)
	if err != nil {
		return nil, err
	}
	return dAtA[:n], nil
}

func (m *WorkloadIntfStatus) MarshalTo(dAtA []byte) (int, error) {
	var i int
	_ = i
	var l int
	_ = l
	if len(m.IpAddrs) > 0 {
		for _, s := range m.IpAddrs {
			dAtA[i] = 0xa
			i++
			l = len(s)
			for l >= 1<<7 {
				dAtA[i] = uint8(uint64(l)&0x7f | 0x80)
				l >>= 7
				i++
			}
			dAtA[i] = uint8(l)
			i++
			i += copy(dAtA[i:], s)
		}
	}
	return i, nil
}

func (m *WorkloadSpec) Marshal() (dAtA []byte, err error) {
	size := m.Size()
	dAtA = make([]byte, size)
	n, err := m.MarshalTo(dAtA)
	if err != nil {
		return nil, err
	}
	return dAtA[:n], nil
}

func (m *WorkloadSpec) MarshalTo(dAtA []byte) (int, error) {
	var i int
	_ = i
	var l int
	_ = l
	if len(m.HostName) > 0 {
		dAtA[i] = 0xa
		i++
		i = encodeVarintWorkload(dAtA, i, uint64(len(m.HostName)))
		i += copy(dAtA[i:], m.HostName)
	}
	if len(m.Interfaces) > 0 {
		for k, _ := range m.Interfaces {
			dAtA[i] = 0x12
			i++
			v := m.Interfaces[k]
			msgSize := 0
			if (&v) != nil {
				msgSize = (&v).Size()
				msgSize += 1 + sovWorkload(uint64(msgSize))
			}
			mapSize := 1 + len(k) + sovWorkload(uint64(len(k))) + msgSize
			i = encodeVarintWorkload(dAtA, i, uint64(mapSize))
			dAtA[i] = 0xa
			i++
			i = encodeVarintWorkload(dAtA, i, uint64(len(k)))
			i += copy(dAtA[i:], k)
			dAtA[i] = 0x12
			i++
			i = encodeVarintWorkload(dAtA, i, uint64((&v).Size()))
			n5, err := (&v).MarshalTo(dAtA[i:])
			if err != nil {
				return 0, err
			}
			i += n5
		}
	}
	return i, nil
}

func (m *WorkloadStatus) Marshal() (dAtA []byte, err error) {
	size := m.Size()
	dAtA = make([]byte, size)
	n, err := m.MarshalTo(dAtA)
	if err != nil {
		return nil, err
	}
	return dAtA[:n], nil
}

func (m *WorkloadStatus) MarshalTo(dAtA []byte) (int, error) {
	var i int
	_ = i
	var l int
	_ = l
	if len(m.Interfaces) > 0 {
		for k, _ := range m.Interfaces {
			dAtA[i] = 0xa
			i++
			v := m.Interfaces[k]
			msgSize := 0
			if (&v) != nil {
				msgSize = (&v).Size()
				msgSize += 1 + sovWorkload(uint64(msgSize))
			}
			mapSize := 1 + len(k) + sovWorkload(uint64(len(k))) + msgSize
			i = encodeVarintWorkload(dAtA, i, uint64(mapSize))
			dAtA[i] = 0xa
			i++
			i = encodeVarintWorkload(dAtA, i, uint64(len(k)))
			i += copy(dAtA[i:], k)
			dAtA[i] = 0x12
			i++
			i = encodeVarintWorkload(dAtA, i, uint64((&v).Size()))
			n6, err := (&v).MarshalTo(dAtA[i:])
			if err != nil {
				return 0, err
			}
			i += n6
		}
	}
	if len(m.Endpoints) > 0 {
		for _, s := range m.Endpoints {
			dAtA[i] = 0x12
			i++
			l = len(s)
			for l >= 1<<7 {
				dAtA[i] = uint8(uint64(l)&0x7f | 0x80)
				l >>= 7
				i++
			}
			dAtA[i] = uint8(l)
			i++
			i += copy(dAtA[i:], s)
		}
	}
	return i, nil
}

func encodeVarintWorkload(dAtA []byte, offset int, v uint64) int {
	for v >= 1<<7 {
		dAtA[offset] = uint8(v&0x7f | 0x80)
		v >>= 7
		offset++
	}
	dAtA[offset] = uint8(v)
	return offset + 1
}
func (m *Workload) Size() (n int) {
	var l int
	_ = l
	l = m.TypeMeta.Size()
	n += 1 + l + sovWorkload(uint64(l))
	l = m.ObjectMeta.Size()
	n += 1 + l + sovWorkload(uint64(l))
	l = m.Spec.Size()
	n += 1 + l + sovWorkload(uint64(l))
	l = m.Status.Size()
	n += 1 + l + sovWorkload(uint64(l))
	return n
}

func (m *WorkloadIntfSpec) Size() (n int) {
	var l int
	_ = l
	if m.MicroSegVlan != 0 {
		n += 1 + sovWorkload(uint64(m.MicroSegVlan))
	}
	if m.ExternalVlan != 0 {
		n += 1 + sovWorkload(uint64(m.ExternalVlan))
	}
	return n
}

func (m *WorkloadIntfStatus) Size() (n int) {
	var l int
	_ = l
	if len(m.IpAddrs) > 0 {
		for _, s := range m.IpAddrs {
			l = len(s)
			n += 1 + l + sovWorkload(uint64(l))
		}
	}
	return n
}

func (m *WorkloadSpec) Size() (n int) {
	var l int
	_ = l
	l = len(m.HostName)
	if l > 0 {
		n += 1 + l + sovWorkload(uint64(l))
	}
	if len(m.Interfaces) > 0 {
		for k, v := range m.Interfaces {
			_ = k
			_ = v
			l = v.Size()
			mapEntrySize := 1 + len(k) + sovWorkload(uint64(len(k))) + 1 + l + sovWorkload(uint64(l))
			n += mapEntrySize + 1 + sovWorkload(uint64(mapEntrySize))
		}
	}
	return n
}

func (m *WorkloadStatus) Size() (n int) {
	var l int
	_ = l
	if len(m.Interfaces) > 0 {
		for k, v := range m.Interfaces {
			_ = k
			_ = v
			l = v.Size()
			mapEntrySize := 1 + len(k) + sovWorkload(uint64(len(k))) + 1 + l + sovWorkload(uint64(l))
			n += mapEntrySize + 1 + sovWorkload(uint64(mapEntrySize))
		}
	}
	if len(m.Endpoints) > 0 {
		for _, s := range m.Endpoints {
			l = len(s)
			n += 1 + l + sovWorkload(uint64(l))
		}
	}
	return n
}

func sovWorkload(x uint64) (n int) {
	for {
		n++
		x >>= 7
		if x == 0 {
			break
		}
	}
	return n
}
func sozWorkload(x uint64) (n int) {
	return sovWorkload(uint64((x << 1) ^ uint64((int64(x) >> 63))))
}
func (m *Workload) Unmarshal(dAtA []byte) error {
	l := len(dAtA)
	iNdEx := 0
	for iNdEx < l {
		preIndex := iNdEx
		var wire uint64
		for shift := uint(0); ; shift += 7 {
			if shift >= 64 {
				return ErrIntOverflowWorkload
			}
			if iNdEx >= l {
				return io.ErrUnexpectedEOF
			}
			b := dAtA[iNdEx]
			iNdEx++
			wire |= (uint64(b) & 0x7F) << shift
			if b < 0x80 {
				break
			}
		}
		fieldNum := int32(wire >> 3)
		wireType := int(wire & 0x7)
		if wireType == 4 {
			return fmt.Errorf("proto: Workload: wiretype end group for non-group")
		}
		if fieldNum <= 0 {
			return fmt.Errorf("proto: Workload: illegal tag %d (wire type %d)", fieldNum, wire)
		}
		switch fieldNum {
		case 1:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field TypeMeta", wireType)
			}
			var msglen int
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowWorkload
				}
				if iNdEx >= l {
					return io.ErrUnexpectedEOF
				}
				b := dAtA[iNdEx]
				iNdEx++
				msglen |= (int(b) & 0x7F) << shift
				if b < 0x80 {
					break
				}
			}
			if msglen < 0 {
				return ErrInvalidLengthWorkload
			}
			postIndex := iNdEx + msglen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			if err := m.TypeMeta.Unmarshal(dAtA[iNdEx:postIndex]); err != nil {
				return err
			}
			iNdEx = postIndex
		case 2:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field ObjectMeta", wireType)
			}
			var msglen int
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowWorkload
				}
				if iNdEx >= l {
					return io.ErrUnexpectedEOF
				}
				b := dAtA[iNdEx]
				iNdEx++
				msglen |= (int(b) & 0x7F) << shift
				if b < 0x80 {
					break
				}
			}
			if msglen < 0 {
				return ErrInvalidLengthWorkload
			}
			postIndex := iNdEx + msglen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			if err := m.ObjectMeta.Unmarshal(dAtA[iNdEx:postIndex]); err != nil {
				return err
			}
			iNdEx = postIndex
		case 3:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field Spec", wireType)
			}
			var msglen int
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowWorkload
				}
				if iNdEx >= l {
					return io.ErrUnexpectedEOF
				}
				b := dAtA[iNdEx]
				iNdEx++
				msglen |= (int(b) & 0x7F) << shift
				if b < 0x80 {
					break
				}
			}
			if msglen < 0 {
				return ErrInvalidLengthWorkload
			}
			postIndex := iNdEx + msglen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			if err := m.Spec.Unmarshal(dAtA[iNdEx:postIndex]); err != nil {
				return err
			}
			iNdEx = postIndex
		case 4:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field Status", wireType)
			}
			var msglen int
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowWorkload
				}
				if iNdEx >= l {
					return io.ErrUnexpectedEOF
				}
				b := dAtA[iNdEx]
				iNdEx++
				msglen |= (int(b) & 0x7F) << shift
				if b < 0x80 {
					break
				}
			}
			if msglen < 0 {
				return ErrInvalidLengthWorkload
			}
			postIndex := iNdEx + msglen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			if err := m.Status.Unmarshal(dAtA[iNdEx:postIndex]); err != nil {
				return err
			}
			iNdEx = postIndex
		default:
			iNdEx = preIndex
			skippy, err := skipWorkload(dAtA[iNdEx:])
			if err != nil {
				return err
			}
			if skippy < 0 {
				return ErrInvalidLengthWorkload
			}
			if (iNdEx + skippy) > l {
				return io.ErrUnexpectedEOF
			}
			iNdEx += skippy
		}
	}

	if iNdEx > l {
		return io.ErrUnexpectedEOF
	}
	return nil
}
func (m *WorkloadIntfSpec) Unmarshal(dAtA []byte) error {
	l := len(dAtA)
	iNdEx := 0
	for iNdEx < l {
		preIndex := iNdEx
		var wire uint64
		for shift := uint(0); ; shift += 7 {
			if shift >= 64 {
				return ErrIntOverflowWorkload
			}
			if iNdEx >= l {
				return io.ErrUnexpectedEOF
			}
			b := dAtA[iNdEx]
			iNdEx++
			wire |= (uint64(b) & 0x7F) << shift
			if b < 0x80 {
				break
			}
		}
		fieldNum := int32(wire >> 3)
		wireType := int(wire & 0x7)
		if wireType == 4 {
			return fmt.Errorf("proto: WorkloadIntfSpec: wiretype end group for non-group")
		}
		if fieldNum <= 0 {
			return fmt.Errorf("proto: WorkloadIntfSpec: illegal tag %d (wire type %d)", fieldNum, wire)
		}
		switch fieldNum {
		case 1:
			if wireType != 0 {
				return fmt.Errorf("proto: wrong wireType = %d for field MicroSegVlan", wireType)
			}
			m.MicroSegVlan = 0
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowWorkload
				}
				if iNdEx >= l {
					return io.ErrUnexpectedEOF
				}
				b := dAtA[iNdEx]
				iNdEx++
				m.MicroSegVlan |= (uint32(b) & 0x7F) << shift
				if b < 0x80 {
					break
				}
			}
		case 2:
			if wireType != 0 {
				return fmt.Errorf("proto: wrong wireType = %d for field ExternalVlan", wireType)
			}
			m.ExternalVlan = 0
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowWorkload
				}
				if iNdEx >= l {
					return io.ErrUnexpectedEOF
				}
				b := dAtA[iNdEx]
				iNdEx++
				m.ExternalVlan |= (uint32(b) & 0x7F) << shift
				if b < 0x80 {
					break
				}
			}
		default:
			iNdEx = preIndex
			skippy, err := skipWorkload(dAtA[iNdEx:])
			if err != nil {
				return err
			}
			if skippy < 0 {
				return ErrInvalidLengthWorkload
			}
			if (iNdEx + skippy) > l {
				return io.ErrUnexpectedEOF
			}
			iNdEx += skippy
		}
	}

	if iNdEx > l {
		return io.ErrUnexpectedEOF
	}
	return nil
}
func (m *WorkloadIntfStatus) Unmarshal(dAtA []byte) error {
	l := len(dAtA)
	iNdEx := 0
	for iNdEx < l {
		preIndex := iNdEx
		var wire uint64
		for shift := uint(0); ; shift += 7 {
			if shift >= 64 {
				return ErrIntOverflowWorkload
			}
			if iNdEx >= l {
				return io.ErrUnexpectedEOF
			}
			b := dAtA[iNdEx]
			iNdEx++
			wire |= (uint64(b) & 0x7F) << shift
			if b < 0x80 {
				break
			}
		}
		fieldNum := int32(wire >> 3)
		wireType := int(wire & 0x7)
		if wireType == 4 {
			return fmt.Errorf("proto: WorkloadIntfStatus: wiretype end group for non-group")
		}
		if fieldNum <= 0 {
			return fmt.Errorf("proto: WorkloadIntfStatus: illegal tag %d (wire type %d)", fieldNum, wire)
		}
		switch fieldNum {
		case 1:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field IpAddrs", wireType)
			}
			var stringLen uint64
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowWorkload
				}
				if iNdEx >= l {
					return io.ErrUnexpectedEOF
				}
				b := dAtA[iNdEx]
				iNdEx++
				stringLen |= (uint64(b) & 0x7F) << shift
				if b < 0x80 {
					break
				}
			}
			intStringLen := int(stringLen)
			if intStringLen < 0 {
				return ErrInvalidLengthWorkload
			}
			postIndex := iNdEx + intStringLen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			m.IpAddrs = append(m.IpAddrs, string(dAtA[iNdEx:postIndex]))
			iNdEx = postIndex
		default:
			iNdEx = preIndex
			skippy, err := skipWorkload(dAtA[iNdEx:])
			if err != nil {
				return err
			}
			if skippy < 0 {
				return ErrInvalidLengthWorkload
			}
			if (iNdEx + skippy) > l {
				return io.ErrUnexpectedEOF
			}
			iNdEx += skippy
		}
	}

	if iNdEx > l {
		return io.ErrUnexpectedEOF
	}
	return nil
}
func (m *WorkloadSpec) Unmarshal(dAtA []byte) error {
	l := len(dAtA)
	iNdEx := 0
	for iNdEx < l {
		preIndex := iNdEx
		var wire uint64
		for shift := uint(0); ; shift += 7 {
			if shift >= 64 {
				return ErrIntOverflowWorkload
			}
			if iNdEx >= l {
				return io.ErrUnexpectedEOF
			}
			b := dAtA[iNdEx]
			iNdEx++
			wire |= (uint64(b) & 0x7F) << shift
			if b < 0x80 {
				break
			}
		}
		fieldNum := int32(wire >> 3)
		wireType := int(wire & 0x7)
		if wireType == 4 {
			return fmt.Errorf("proto: WorkloadSpec: wiretype end group for non-group")
		}
		if fieldNum <= 0 {
			return fmt.Errorf("proto: WorkloadSpec: illegal tag %d (wire type %d)", fieldNum, wire)
		}
		switch fieldNum {
		case 1:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field HostName", wireType)
			}
			var stringLen uint64
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowWorkload
				}
				if iNdEx >= l {
					return io.ErrUnexpectedEOF
				}
				b := dAtA[iNdEx]
				iNdEx++
				stringLen |= (uint64(b) & 0x7F) << shift
				if b < 0x80 {
					break
				}
			}
			intStringLen := int(stringLen)
			if intStringLen < 0 {
				return ErrInvalidLengthWorkload
			}
			postIndex := iNdEx + intStringLen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			m.HostName = string(dAtA[iNdEx:postIndex])
			iNdEx = postIndex
		case 2:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field Interfaces", wireType)
			}
			var msglen int
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowWorkload
				}
				if iNdEx >= l {
					return io.ErrUnexpectedEOF
				}
				b := dAtA[iNdEx]
				iNdEx++
				msglen |= (int(b) & 0x7F) << shift
				if b < 0x80 {
					break
				}
			}
			if msglen < 0 {
				return ErrInvalidLengthWorkload
			}
			postIndex := iNdEx + msglen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			if m.Interfaces == nil {
				m.Interfaces = make(map[string]WorkloadIntfSpec)
			}
			var mapkey string
			mapvalue := &WorkloadIntfSpec{}
			for iNdEx < postIndex {
				entryPreIndex := iNdEx
				var wire uint64
				for shift := uint(0); ; shift += 7 {
					if shift >= 64 {
						return ErrIntOverflowWorkload
					}
					if iNdEx >= l {
						return io.ErrUnexpectedEOF
					}
					b := dAtA[iNdEx]
					iNdEx++
					wire |= (uint64(b) & 0x7F) << shift
					if b < 0x80 {
						break
					}
				}
				fieldNum := int32(wire >> 3)
				if fieldNum == 1 {
					var stringLenmapkey uint64
					for shift := uint(0); ; shift += 7 {
						if shift >= 64 {
							return ErrIntOverflowWorkload
						}
						if iNdEx >= l {
							return io.ErrUnexpectedEOF
						}
						b := dAtA[iNdEx]
						iNdEx++
						stringLenmapkey |= (uint64(b) & 0x7F) << shift
						if b < 0x80 {
							break
						}
					}
					intStringLenmapkey := int(stringLenmapkey)
					if intStringLenmapkey < 0 {
						return ErrInvalidLengthWorkload
					}
					postStringIndexmapkey := iNdEx + intStringLenmapkey
					if postStringIndexmapkey > l {
						return io.ErrUnexpectedEOF
					}
					mapkey = string(dAtA[iNdEx:postStringIndexmapkey])
					iNdEx = postStringIndexmapkey
				} else if fieldNum == 2 {
					var mapmsglen int
					for shift := uint(0); ; shift += 7 {
						if shift >= 64 {
							return ErrIntOverflowWorkload
						}
						if iNdEx >= l {
							return io.ErrUnexpectedEOF
						}
						b := dAtA[iNdEx]
						iNdEx++
						mapmsglen |= (int(b) & 0x7F) << shift
						if b < 0x80 {
							break
						}
					}
					if mapmsglen < 0 {
						return ErrInvalidLengthWorkload
					}
					postmsgIndex := iNdEx + mapmsglen
					if mapmsglen < 0 {
						return ErrInvalidLengthWorkload
					}
					if postmsgIndex > l {
						return io.ErrUnexpectedEOF
					}
					mapvalue = &WorkloadIntfSpec{}
					if err := mapvalue.Unmarshal(dAtA[iNdEx:postmsgIndex]); err != nil {
						return err
					}
					iNdEx = postmsgIndex
				} else {
					iNdEx = entryPreIndex
					skippy, err := skipWorkload(dAtA[iNdEx:])
					if err != nil {
						return err
					}
					if skippy < 0 {
						return ErrInvalidLengthWorkload
					}
					if (iNdEx + skippy) > postIndex {
						return io.ErrUnexpectedEOF
					}
					iNdEx += skippy
				}
			}
			m.Interfaces[mapkey] = *mapvalue
			iNdEx = postIndex
		default:
			iNdEx = preIndex
			skippy, err := skipWorkload(dAtA[iNdEx:])
			if err != nil {
				return err
			}
			if skippy < 0 {
				return ErrInvalidLengthWorkload
			}
			if (iNdEx + skippy) > l {
				return io.ErrUnexpectedEOF
			}
			iNdEx += skippy
		}
	}

	if iNdEx > l {
		return io.ErrUnexpectedEOF
	}
	return nil
}
func (m *WorkloadStatus) Unmarshal(dAtA []byte) error {
	l := len(dAtA)
	iNdEx := 0
	for iNdEx < l {
		preIndex := iNdEx
		var wire uint64
		for shift := uint(0); ; shift += 7 {
			if shift >= 64 {
				return ErrIntOverflowWorkload
			}
			if iNdEx >= l {
				return io.ErrUnexpectedEOF
			}
			b := dAtA[iNdEx]
			iNdEx++
			wire |= (uint64(b) & 0x7F) << shift
			if b < 0x80 {
				break
			}
		}
		fieldNum := int32(wire >> 3)
		wireType := int(wire & 0x7)
		if wireType == 4 {
			return fmt.Errorf("proto: WorkloadStatus: wiretype end group for non-group")
		}
		if fieldNum <= 0 {
			return fmt.Errorf("proto: WorkloadStatus: illegal tag %d (wire type %d)", fieldNum, wire)
		}
		switch fieldNum {
		case 1:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field Interfaces", wireType)
			}
			var msglen int
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowWorkload
				}
				if iNdEx >= l {
					return io.ErrUnexpectedEOF
				}
				b := dAtA[iNdEx]
				iNdEx++
				msglen |= (int(b) & 0x7F) << shift
				if b < 0x80 {
					break
				}
			}
			if msglen < 0 {
				return ErrInvalidLengthWorkload
			}
			postIndex := iNdEx + msglen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			if m.Interfaces == nil {
				m.Interfaces = make(map[string]WorkloadIntfStatus)
			}
			var mapkey string
			mapvalue := &WorkloadIntfStatus{}
			for iNdEx < postIndex {
				entryPreIndex := iNdEx
				var wire uint64
				for shift := uint(0); ; shift += 7 {
					if shift >= 64 {
						return ErrIntOverflowWorkload
					}
					if iNdEx >= l {
						return io.ErrUnexpectedEOF
					}
					b := dAtA[iNdEx]
					iNdEx++
					wire |= (uint64(b) & 0x7F) << shift
					if b < 0x80 {
						break
					}
				}
				fieldNum := int32(wire >> 3)
				if fieldNum == 1 {
					var stringLenmapkey uint64
					for shift := uint(0); ; shift += 7 {
						if shift >= 64 {
							return ErrIntOverflowWorkload
						}
						if iNdEx >= l {
							return io.ErrUnexpectedEOF
						}
						b := dAtA[iNdEx]
						iNdEx++
						stringLenmapkey |= (uint64(b) & 0x7F) << shift
						if b < 0x80 {
							break
						}
					}
					intStringLenmapkey := int(stringLenmapkey)
					if intStringLenmapkey < 0 {
						return ErrInvalidLengthWorkload
					}
					postStringIndexmapkey := iNdEx + intStringLenmapkey
					if postStringIndexmapkey > l {
						return io.ErrUnexpectedEOF
					}
					mapkey = string(dAtA[iNdEx:postStringIndexmapkey])
					iNdEx = postStringIndexmapkey
				} else if fieldNum == 2 {
					var mapmsglen int
					for shift := uint(0); ; shift += 7 {
						if shift >= 64 {
							return ErrIntOverflowWorkload
						}
						if iNdEx >= l {
							return io.ErrUnexpectedEOF
						}
						b := dAtA[iNdEx]
						iNdEx++
						mapmsglen |= (int(b) & 0x7F) << shift
						if b < 0x80 {
							break
						}
					}
					if mapmsglen < 0 {
						return ErrInvalidLengthWorkload
					}
					postmsgIndex := iNdEx + mapmsglen
					if mapmsglen < 0 {
						return ErrInvalidLengthWorkload
					}
					if postmsgIndex > l {
						return io.ErrUnexpectedEOF
					}
					mapvalue = &WorkloadIntfStatus{}
					if err := mapvalue.Unmarshal(dAtA[iNdEx:postmsgIndex]); err != nil {
						return err
					}
					iNdEx = postmsgIndex
				} else {
					iNdEx = entryPreIndex
					skippy, err := skipWorkload(dAtA[iNdEx:])
					if err != nil {
						return err
					}
					if skippy < 0 {
						return ErrInvalidLengthWorkload
					}
					if (iNdEx + skippy) > postIndex {
						return io.ErrUnexpectedEOF
					}
					iNdEx += skippy
				}
			}
			m.Interfaces[mapkey] = *mapvalue
			iNdEx = postIndex
		case 2:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field Endpoints", wireType)
			}
			var stringLen uint64
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowWorkload
				}
				if iNdEx >= l {
					return io.ErrUnexpectedEOF
				}
				b := dAtA[iNdEx]
				iNdEx++
				stringLen |= (uint64(b) & 0x7F) << shift
				if b < 0x80 {
					break
				}
			}
			intStringLen := int(stringLen)
			if intStringLen < 0 {
				return ErrInvalidLengthWorkload
			}
			postIndex := iNdEx + intStringLen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			m.Endpoints = append(m.Endpoints, string(dAtA[iNdEx:postIndex]))
			iNdEx = postIndex
		default:
			iNdEx = preIndex
			skippy, err := skipWorkload(dAtA[iNdEx:])
			if err != nil {
				return err
			}
			if skippy < 0 {
				return ErrInvalidLengthWorkload
			}
			if (iNdEx + skippy) > l {
				return io.ErrUnexpectedEOF
			}
			iNdEx += skippy
		}
	}

	if iNdEx > l {
		return io.ErrUnexpectedEOF
	}
	return nil
}
func skipWorkload(dAtA []byte) (n int, err error) {
	l := len(dAtA)
	iNdEx := 0
	for iNdEx < l {
		var wire uint64
		for shift := uint(0); ; shift += 7 {
			if shift >= 64 {
				return 0, ErrIntOverflowWorkload
			}
			if iNdEx >= l {
				return 0, io.ErrUnexpectedEOF
			}
			b := dAtA[iNdEx]
			iNdEx++
			wire |= (uint64(b) & 0x7F) << shift
			if b < 0x80 {
				break
			}
		}
		wireType := int(wire & 0x7)
		switch wireType {
		case 0:
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return 0, ErrIntOverflowWorkload
				}
				if iNdEx >= l {
					return 0, io.ErrUnexpectedEOF
				}
				iNdEx++
				if dAtA[iNdEx-1] < 0x80 {
					break
				}
			}
			return iNdEx, nil
		case 1:
			iNdEx += 8
			return iNdEx, nil
		case 2:
			var length int
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return 0, ErrIntOverflowWorkload
				}
				if iNdEx >= l {
					return 0, io.ErrUnexpectedEOF
				}
				b := dAtA[iNdEx]
				iNdEx++
				length |= (int(b) & 0x7F) << shift
				if b < 0x80 {
					break
				}
			}
			iNdEx += length
			if length < 0 {
				return 0, ErrInvalidLengthWorkload
			}
			return iNdEx, nil
		case 3:
			for {
				var innerWire uint64
				var start int = iNdEx
				for shift := uint(0); ; shift += 7 {
					if shift >= 64 {
						return 0, ErrIntOverflowWorkload
					}
					if iNdEx >= l {
						return 0, io.ErrUnexpectedEOF
					}
					b := dAtA[iNdEx]
					iNdEx++
					innerWire |= (uint64(b) & 0x7F) << shift
					if b < 0x80 {
						break
					}
				}
				innerWireType := int(innerWire & 0x7)
				if innerWireType == 4 {
					break
				}
				next, err := skipWorkload(dAtA[start:])
				if err != nil {
					return 0, err
				}
				iNdEx = start + next
			}
			return iNdEx, nil
		case 4:
			return iNdEx, nil
		case 5:
			iNdEx += 4
			return iNdEx, nil
		default:
			return 0, fmt.Errorf("proto: illegal wireType %d", wireType)
		}
	}
	panic("unreachable")
}

var (
	ErrInvalidLengthWorkload = fmt.Errorf("proto: negative length found during unmarshaling")
	ErrIntOverflowWorkload   = fmt.Errorf("proto: integer overflow")
)

func init() { proto.RegisterFile("workload.proto", fileDescriptorWorkload) }

var fileDescriptorWorkload = []byte{
	// 676 bytes of a gzipped FileDescriptorProto
	0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xff, 0xac, 0x94, 0x4d, 0x4f, 0x13, 0x41,
	0x18, 0x80, 0xdd, 0x2d, 0x02, 0x1d, 0x0a, 0x34, 0x83, 0xc1, 0xa5, 0x36, 0x94, 0x34, 0xd1, 0xd4,
	0x84, 0xee, 0x56, 0xfc, 0x88, 0x72, 0x31, 0xae, 0xa9, 0xb1, 0x89, 0x88, 0x29, 0x55, 0x63, 0x4c,
	0x8c, 0xd3, 0xed, 0xcb, 0x32, 0xb2, 0x3b, 0xb3, 0xd9, 0x99, 0x82, 0x8d, 0xf1, 0xa8, 0xff, 0xc2,
	0x9b, 0x27, 0x7f, 0x09, 0x07, 0x0f, 0xc4, 0x78, 0x6e, 0x0c, 0x47, 0x7e, 0x85, 0xd9, 0xe9, 0x2e,
	0x6e, 0x4b, 0x6b, 0x3c, 0x78, 0x9b, 0xf7, 0xeb, 0x79, 0x3f, 0x77, 0xd1, 0xc2, 0x21, 0x0f, 0xf7,
	0x3d, 0x4e, 0x3a, 0x66, 0x10, 0x72, 0xc9, 0xf1, 0x6c, 0x22, 0x17, 0x8a, 0x2e, 0xe7, 0xae, 0x07,
	0x16, 0x09, 0xa8, 0x45, 0x18, 0xe3, 0x92, 0x48, 0xca, 0x99, 0x18, 0xf8, 0x15, 0xea, 0x2e, 0x95,
	0x7b, 0xdd, 0xb6, 0xe9, 0x70, 0xdf, 0x0a, 0x80, 0x09, 0xc2, 0x3a, 0xdc, 0x12, 0x87, 0xd6, 0x01,
	0x30, 0xea, 0x80, 0xd5, 0x95, 0xd4, 0x13, 0x51, 0xa8, 0x0b, 0x2c, 0x1d, 0x6d, 0x51, 0xe6, 0x78,
	0xdd, 0x0e, 0x24, 0x98, 0x6a, 0x0a, 0xe3, 0x72, 0x97, 0x5b, 0x4a, 0xdd, 0xee, 0xee, 0x2a, 0x49,
	0x09, 0xea, 0x15, 0xbb, 0x5f, 0x9d, 0x90, 0x35, 0xaa, 0xd1, 0x07, 0x49, 0x62, 0xb7, 0xda, 0x5f,
	0xdc, 0x3c, 0xd2, 0x06, 0x4f, 0x58, 0x02, 0x3c, 0x70, 0x24, 0x0f, 0x07, 0x11, 0xe5, 0xef, 0x3a,
	0x9a, 0x7d, 0x19, 0x77, 0x8e, 0xef, 0x20, 0xad, 0x65, 0x68, 0x6b, 0x5a, 0x65, 0x6e, 0x63, 0xde,
	0x24, 0x01, 0x35, 0x5b, 0xbd, 0x00, 0xb6, 0x40, 0x12, 0x7b, 0xe9, 0xa8, 0x5f, 0xba, 0x70, 0xdc,
	0x2f, 0x69, 0xa7, 0xfd, 0xd2, 0xcc, 0x3a, 0x65, 0x1e, 0x65, 0xd0, 0x4c, 0x1e, 0xf8, 0x11, 0xd2,
	0xb6, 0x0d, 0x5d, 0xc5, 0x2d, 0xaa, 0xb8, 0xed, 0xf6, 0x3b, 0x70, 0xa4, 0x8a, 0x2c, 0xa4, 0x22,
	0x17, 0xa2, 0x52, 0xd7, 0xb9, 0x4f, 0x25, 0xf8, 0x81, 0xec, 0x35, 0x47, 0x64, 0xfc, 0x04, 0x4d,
	0xed, 0x04, 0xe0, 0x18, 0x19, 0x85, 0x5a, 0x36, 0xcf, 0x56, 0x94, 0x54, 0x18, 0x59, 0xed, 0xe5,
	0x88, 0x18, 0xd1, 0x44, 0x00, 0x4e, 0x9a, 0x36, 0x2c, 0xe3, 0x16, 0x9a, 0xde, 0x91, 0x44, 0x76,
	0x85, 0x31, 0xa5, 0x78, 0xc6, 0x18, 0x9e, 0xb2, 0xdb, 0x46, 0x4c, 0xcc, 0x0b, 0x25, 0xa7, 0x98,
	0xe7, 0x34, 0x9b, 0xc5, 0x1f, 0x9f, 0x56, 0x0c, 0x34, 0x67, 0x7d, 0xd8, 0x36, 0x5b, 0xc0, 0x08,
	0x93, 0x1f, 0x71, 0x36, 0xe1, 0x8a, 0xf2, 0x4f, 0x0d, 0xe5, 0x13, 0x78, 0x83, 0xc9, 0xdd, 0xa8,
	0x60, 0xfc, 0x16, 0xe5, 0xb6, 0xa8, 0x13, 0xf2, 0x1d, 0x70, 0x5f, 0x78, 0x84, 0xa9, 0x09, 0xcf,
	0xdb, 0xb5, 0x6f, 0x9f, 0x57, 0xf2, 0x0d, 0x26, 0x9b, 0x84, 0xb9, 0x50, 0xb9, 0xb1, 0x7e, 0xab,
	0x76, 0xef, 0xf6, 0xf5, 0xd3, 0x7e, 0xc9, 0xf0, 0x23, 0xdf, 0xaa, 0x00, 0xb7, 0x7a, 0xe0, 0x11,
	0x96, 0x2a, 0x66, 0xa2, 0x05, 0xbf, 0x41, 0xb9, 0xfa, 0x7b, 0x09, 0x21, 0x23, 0x9e, 0xca, 0xa0,
	0xab, 0x0c, 0xd6, 0x84, 0x0c, 0x97, 0x21, 0xf6, 0x1d, 0x4d, 0x30, 0xc9, 0x50, 0x7e, 0x8e, 0xf0,
	0x50, 0x57, 0x6a, 0x28, 0xf8, 0x3e, 0x9a, 0x69, 0x04, 0x0f, 0x3a, 0x9d, 0x50, 0x18, 0xda, 0x5a,
	0xa6, 0x92, 0x55, 0xbb, 0x8e, 0xf6, 0x8c, 0x69, 0x50, 0x25, 0x91, 0x3e, 0xc5, 0x1e, 0xa3, 0x2b,
	0x7f, 0xd1, 0x51, 0x2e, 0xbd, 0x5a, 0x6c, 0xa3, 0xd9, 0xc7, 0x5c, 0xc8, 0xa7, 0xc4, 0x07, 0x35,
	0xa5, 0xac, 0x7d, 0x25, 0x46, 0x2e, 0xed, 0x71, 0x21, 0xab, 0x8c, 0xf8, 0x90, 0x62, 0x8e, 0x53,
	0x62, 0x86, 0x50, 0x83, 0x49, 0x08, 0x77, 0x89, 0x03, 0xc2, 0xd0, 0xd7, 0x32, 0x95, 0xb9, 0x8d,
	0x6b, 0xe3, 0x4f, 0xc9, 0xfc, 0xe3, 0x58, 0x67, 0x32, 0xec, 0xd9, 0xc5, 0xf8, 0x10, 0x2e, 0xd1,
	0x33, 0x43, 0x2a, 0xdd, 0x58, 0x6d, 0xe1, 0x15, 0x5a, 0x1c, 0xc1, 0xe0, 0x3c, 0xca, 0xec, 0x43,
	0x6f, 0xd0, 0x41, 0x33, 0x7a, 0xe2, 0x1a, 0xba, 0x78, 0x40, 0xbc, 0x2e, 0xc4, 0x5f, 0x49, 0xe1,
	0x7c, 0x3d, 0xc9, 0xb5, 0x34, 0x07, 0x8e, 0x9b, 0xfa, 0x5d, 0xad, 0xfc, 0x55, 0x47, 0x0b, 0xc3,
	0xa7, 0x8a, 0x83, 0xa1, 0xee, 0x34, 0xd5, 0x5d, 0x65, 0xd2, 0x61, 0xff, 0x8f, 0xfe, 0xf0, 0x43,
	0x94, 0xad, 0xb3, 0x4e, 0xc0, 0x29, 0x93, 0x83, 0x71, 0xa6, 0x96, 0x02, 0x89, 0x21, 0xbd, 0x94,
	0x31, 0xca, 0xc2, 0xeb, 0x7f, 0x19, 0xd2, 0xc6, 0xf0, 0x90, 0x8a, 0x13, 0x86, 0xa4, 0x5a, 0x4b,
	0x8d, 0xc9, 0xce, 0x1d, 0x9d, 0xac, 0x6a, 0xc7, 0x27, 0xab, 0xda, 0xaf, 0x93, 0x55, 0xed, 0x99,
	0xd6, 0x9e, 0x56, 0xbf, 0xb6, 0x9b, 0xbf, 0x03, 0x00, 0x00, 0xff, 0xff, 0x68, 0x3c, 0xf8, 0xdb,
	0xe3, 0x05, 0x00, 0x00,
}
