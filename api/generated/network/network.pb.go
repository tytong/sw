// Code generated by protoc-gen-gogo. DO NOT EDIT.
// source: network.proto

package network

import proto "github.com/gogo/protobuf/proto"
import fmt "fmt"
import math "math"
import _ "github.com/pensando/grpc-gateway/third_party/googleapis/google/api"
import _ "github.com/pensando/sw/venice/utils/apigen/annotations"
import _ "github.com/gogo/protobuf/gogoproto"
import api "github.com/pensando/sw/api"
import _ "github.com/pensando/sw/api/labels"
import _ "github.com/pensando/sw/api/generated/cluster"

import io "io"

// Reference imports to suppress errors if they are not otherwise used.
var _ = proto.Marshal
var _ = fmt.Errorf
var _ = math.Inf

// Network represents a subnet
type Network struct {
	//
	api.TypeMeta `protobuf:"bytes,1,opt,name=T,json=,inline,embedded=T" json:",inline"`
	//
	api.ObjectMeta `protobuf:"bytes,2,opt,name=O,json=meta,omitempty,embedded=O" json:"meta,omitempty"`
	// Spec contains the configuration of the network.
	Spec NetworkSpec `protobuf:"bytes,3,opt,name=Spec,json=spec,omitempty" json:"spec,omitempty"`
	// Status contains the current state of the network.
	Status NetworkStatus `protobuf:"bytes,4,opt,name=Status,json=status,omitempty" json:"status,omitempty"`
}

func (m *Network) Reset()                    { *m = Network{} }
func (m *Network) String() string            { return proto.CompactTextString(m) }
func (*Network) ProtoMessage()               {}
func (*Network) Descriptor() ([]byte, []int) { return fileDescriptorNetwork, []int{0} }

func (m *Network) GetSpec() NetworkSpec {
	if m != nil {
		return m.Spec
	}
	return NetworkSpec{}
}

func (m *Network) GetStatus() NetworkStatus {
	if m != nil {
		return m.Status
	}
	return NetworkStatus{}
}

// spec part of network object
type NetworkSpec struct {
	// type of network. (vlan/vxlan/routed etc)
	Type string `protobuf:"bytes,1,opt,name=Type,json=type,omitempty,proto3" json:"type,omitempty"`
	// IPv4 subnet CIDR
	IPv4Subnet string `protobuf:"bytes,2,opt,name=IPv4Subnet,json=ipv4-subnet,omitempty,proto3" json:"ipv4-subnet,omitempty"`
	// IPv4 gateway for this subnet
	IPv4Gateway string `protobuf:"bytes,3,opt,name=IPv4Gateway,json=ipv4-gateway,omitempty,proto3" json:"ipv4-gateway,omitempty"`
	// IPv6 subnet CIDR
	IPv6Subnet string `protobuf:"bytes,4,opt,name=IPv6Subnet,json=ipv6-subnet,omitempty,proto3" json:"ipv6-subnet,omitempty"`
	// IPv6 gateway
	IPv6Gateway string `protobuf:"bytes,5,opt,name=IPv6Gateway,json=ipv6-gateway,omitempty,proto3" json:"ipv6-gateway,omitempty"`
	// Vlan ID for the network
	VlanID uint32 `protobuf:"varint,6,opt,name=VlanID,json=vlan-id,omitempty,proto3" json:"vlan-id,omitempty"`
	// Vxlan VNI for the network
	VxlanVNI uint32 `protobuf:"varint,7,opt,name=VxlanVNI,json=vxlan-vni,omitempty,proto3" json:"vxlan-vni,omitempty"`
}

func (m *NetworkSpec) Reset()                    { *m = NetworkSpec{} }
func (m *NetworkSpec) String() string            { return proto.CompactTextString(m) }
func (*NetworkSpec) ProtoMessage()               {}
func (*NetworkSpec) Descriptor() ([]byte, []int) { return fileDescriptorNetwork, []int{1} }

func (m *NetworkSpec) GetType() string {
	if m != nil {
		return m.Type
	}
	return ""
}

func (m *NetworkSpec) GetIPv4Subnet() string {
	if m != nil {
		return m.IPv4Subnet
	}
	return ""
}

func (m *NetworkSpec) GetIPv4Gateway() string {
	if m != nil {
		return m.IPv4Gateway
	}
	return ""
}

func (m *NetworkSpec) GetIPv6Subnet() string {
	if m != nil {
		return m.IPv6Subnet
	}
	return ""
}

func (m *NetworkSpec) GetIPv6Gateway() string {
	if m != nil {
		return m.IPv6Gateway
	}
	return ""
}

func (m *NetworkSpec) GetVlanID() uint32 {
	if m != nil {
		return m.VlanID
	}
	return 0
}

func (m *NetworkSpec) GetVxlanVNI() uint32 {
	if m != nil {
		return m.VxlanVNI
	}
	return 0
}

// status part of network object
type NetworkStatus struct {
	// list of all workloads in this network
	Workloads []string `protobuf:"bytes,1,rep,name=Workloads,json=workloads,omitempty" json:"workloads,omitempty"`
	// allocated IPv4 addresses (bitmap)
	AllocatedIPv4Addrs []byte `protobuf:"bytes,2,opt,name=AllocatedIPv4Addrs,json=allocated-ipv4-addrs,omitempty,proto3" json:"allocated-ipv4-addrs,omitempty" venice:"sskip"`
}

func (m *NetworkStatus) Reset()                    { *m = NetworkStatus{} }
func (m *NetworkStatus) String() string            { return proto.CompactTextString(m) }
func (*NetworkStatus) ProtoMessage()               {}
func (*NetworkStatus) Descriptor() ([]byte, []int) { return fileDescriptorNetwork, []int{2} }

func (m *NetworkStatus) GetWorkloads() []string {
	if m != nil {
		return m.Workloads
	}
	return nil
}

func (m *NetworkStatus) GetAllocatedIPv4Addrs() []byte {
	if m != nil {
		return m.AllocatedIPv4Addrs
	}
	return nil
}

func init() {
	proto.RegisterType((*Network)(nil), "network.Network")
	proto.RegisterType((*NetworkSpec)(nil), "network.NetworkSpec")
	proto.RegisterType((*NetworkStatus)(nil), "network.NetworkStatus")
}
func (m *Network) Marshal() (dAtA []byte, err error) {
	size := m.Size()
	dAtA = make([]byte, size)
	n, err := m.MarshalTo(dAtA)
	if err != nil {
		return nil, err
	}
	return dAtA[:n], nil
}

func (m *Network) MarshalTo(dAtA []byte) (int, error) {
	var i int
	_ = i
	var l int
	_ = l
	dAtA[i] = 0xa
	i++
	i = encodeVarintNetwork(dAtA, i, uint64(m.TypeMeta.Size()))
	n1, err := m.TypeMeta.MarshalTo(dAtA[i:])
	if err != nil {
		return 0, err
	}
	i += n1
	dAtA[i] = 0x12
	i++
	i = encodeVarintNetwork(dAtA, i, uint64(m.ObjectMeta.Size()))
	n2, err := m.ObjectMeta.MarshalTo(dAtA[i:])
	if err != nil {
		return 0, err
	}
	i += n2
	dAtA[i] = 0x1a
	i++
	i = encodeVarintNetwork(dAtA, i, uint64(m.Spec.Size()))
	n3, err := m.Spec.MarshalTo(dAtA[i:])
	if err != nil {
		return 0, err
	}
	i += n3
	dAtA[i] = 0x22
	i++
	i = encodeVarintNetwork(dAtA, i, uint64(m.Status.Size()))
	n4, err := m.Status.MarshalTo(dAtA[i:])
	if err != nil {
		return 0, err
	}
	i += n4
	return i, nil
}

func (m *NetworkSpec) Marshal() (dAtA []byte, err error) {
	size := m.Size()
	dAtA = make([]byte, size)
	n, err := m.MarshalTo(dAtA)
	if err != nil {
		return nil, err
	}
	return dAtA[:n], nil
}

func (m *NetworkSpec) MarshalTo(dAtA []byte) (int, error) {
	var i int
	_ = i
	var l int
	_ = l
	if len(m.Type) > 0 {
		dAtA[i] = 0xa
		i++
		i = encodeVarintNetwork(dAtA, i, uint64(len(m.Type)))
		i += copy(dAtA[i:], m.Type)
	}
	if len(m.IPv4Subnet) > 0 {
		dAtA[i] = 0x12
		i++
		i = encodeVarintNetwork(dAtA, i, uint64(len(m.IPv4Subnet)))
		i += copy(dAtA[i:], m.IPv4Subnet)
	}
	if len(m.IPv4Gateway) > 0 {
		dAtA[i] = 0x1a
		i++
		i = encodeVarintNetwork(dAtA, i, uint64(len(m.IPv4Gateway)))
		i += copy(dAtA[i:], m.IPv4Gateway)
	}
	if len(m.IPv6Subnet) > 0 {
		dAtA[i] = 0x22
		i++
		i = encodeVarintNetwork(dAtA, i, uint64(len(m.IPv6Subnet)))
		i += copy(dAtA[i:], m.IPv6Subnet)
	}
	if len(m.IPv6Gateway) > 0 {
		dAtA[i] = 0x2a
		i++
		i = encodeVarintNetwork(dAtA, i, uint64(len(m.IPv6Gateway)))
		i += copy(dAtA[i:], m.IPv6Gateway)
	}
	if m.VlanID != 0 {
		dAtA[i] = 0x30
		i++
		i = encodeVarintNetwork(dAtA, i, uint64(m.VlanID))
	}
	if m.VxlanVNI != 0 {
		dAtA[i] = 0x38
		i++
		i = encodeVarintNetwork(dAtA, i, uint64(m.VxlanVNI))
	}
	return i, nil
}

func (m *NetworkStatus) Marshal() (dAtA []byte, err error) {
	size := m.Size()
	dAtA = make([]byte, size)
	n, err := m.MarshalTo(dAtA)
	if err != nil {
		return nil, err
	}
	return dAtA[:n], nil
}

func (m *NetworkStatus) MarshalTo(dAtA []byte) (int, error) {
	var i int
	_ = i
	var l int
	_ = l
	if len(m.Workloads) > 0 {
		for _, s := range m.Workloads {
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
	if len(m.AllocatedIPv4Addrs) > 0 {
		dAtA[i] = 0x12
		i++
		i = encodeVarintNetwork(dAtA, i, uint64(len(m.AllocatedIPv4Addrs)))
		i += copy(dAtA[i:], m.AllocatedIPv4Addrs)
	}
	return i, nil
}

func encodeVarintNetwork(dAtA []byte, offset int, v uint64) int {
	for v >= 1<<7 {
		dAtA[offset] = uint8(v&0x7f | 0x80)
		v >>= 7
		offset++
	}
	dAtA[offset] = uint8(v)
	return offset + 1
}
func (m *Network) Size() (n int) {
	var l int
	_ = l
	l = m.TypeMeta.Size()
	n += 1 + l + sovNetwork(uint64(l))
	l = m.ObjectMeta.Size()
	n += 1 + l + sovNetwork(uint64(l))
	l = m.Spec.Size()
	n += 1 + l + sovNetwork(uint64(l))
	l = m.Status.Size()
	n += 1 + l + sovNetwork(uint64(l))
	return n
}

func (m *NetworkSpec) Size() (n int) {
	var l int
	_ = l
	l = len(m.Type)
	if l > 0 {
		n += 1 + l + sovNetwork(uint64(l))
	}
	l = len(m.IPv4Subnet)
	if l > 0 {
		n += 1 + l + sovNetwork(uint64(l))
	}
	l = len(m.IPv4Gateway)
	if l > 0 {
		n += 1 + l + sovNetwork(uint64(l))
	}
	l = len(m.IPv6Subnet)
	if l > 0 {
		n += 1 + l + sovNetwork(uint64(l))
	}
	l = len(m.IPv6Gateway)
	if l > 0 {
		n += 1 + l + sovNetwork(uint64(l))
	}
	if m.VlanID != 0 {
		n += 1 + sovNetwork(uint64(m.VlanID))
	}
	if m.VxlanVNI != 0 {
		n += 1 + sovNetwork(uint64(m.VxlanVNI))
	}
	return n
}

func (m *NetworkStatus) Size() (n int) {
	var l int
	_ = l
	if len(m.Workloads) > 0 {
		for _, s := range m.Workloads {
			l = len(s)
			n += 1 + l + sovNetwork(uint64(l))
		}
	}
	l = len(m.AllocatedIPv4Addrs)
	if l > 0 {
		n += 1 + l + sovNetwork(uint64(l))
	}
	return n
}

func sovNetwork(x uint64) (n int) {
	for {
		n++
		x >>= 7
		if x == 0 {
			break
		}
	}
	return n
}
func sozNetwork(x uint64) (n int) {
	return sovNetwork(uint64((x << 1) ^ uint64((int64(x) >> 63))))
}
func (m *Network) Unmarshal(dAtA []byte) error {
	l := len(dAtA)
	iNdEx := 0
	for iNdEx < l {
		preIndex := iNdEx
		var wire uint64
		for shift := uint(0); ; shift += 7 {
			if shift >= 64 {
				return ErrIntOverflowNetwork
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
			return fmt.Errorf("proto: Network: wiretype end group for non-group")
		}
		if fieldNum <= 0 {
			return fmt.Errorf("proto: Network: illegal tag %d (wire type %d)", fieldNum, wire)
		}
		switch fieldNum {
		case 1:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field TypeMeta", wireType)
			}
			var msglen int
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowNetwork
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
				return ErrInvalidLengthNetwork
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
					return ErrIntOverflowNetwork
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
				return ErrInvalidLengthNetwork
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
					return ErrIntOverflowNetwork
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
				return ErrInvalidLengthNetwork
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
					return ErrIntOverflowNetwork
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
				return ErrInvalidLengthNetwork
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
			skippy, err := skipNetwork(dAtA[iNdEx:])
			if err != nil {
				return err
			}
			if skippy < 0 {
				return ErrInvalidLengthNetwork
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
func (m *NetworkSpec) Unmarshal(dAtA []byte) error {
	l := len(dAtA)
	iNdEx := 0
	for iNdEx < l {
		preIndex := iNdEx
		var wire uint64
		for shift := uint(0); ; shift += 7 {
			if shift >= 64 {
				return ErrIntOverflowNetwork
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
			return fmt.Errorf("proto: NetworkSpec: wiretype end group for non-group")
		}
		if fieldNum <= 0 {
			return fmt.Errorf("proto: NetworkSpec: illegal tag %d (wire type %d)", fieldNum, wire)
		}
		switch fieldNum {
		case 1:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field Type", wireType)
			}
			var stringLen uint64
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowNetwork
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
				return ErrInvalidLengthNetwork
			}
			postIndex := iNdEx + intStringLen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			m.Type = string(dAtA[iNdEx:postIndex])
			iNdEx = postIndex
		case 2:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field IPv4Subnet", wireType)
			}
			var stringLen uint64
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowNetwork
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
				return ErrInvalidLengthNetwork
			}
			postIndex := iNdEx + intStringLen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			m.IPv4Subnet = string(dAtA[iNdEx:postIndex])
			iNdEx = postIndex
		case 3:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field IPv4Gateway", wireType)
			}
			var stringLen uint64
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowNetwork
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
				return ErrInvalidLengthNetwork
			}
			postIndex := iNdEx + intStringLen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			m.IPv4Gateway = string(dAtA[iNdEx:postIndex])
			iNdEx = postIndex
		case 4:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field IPv6Subnet", wireType)
			}
			var stringLen uint64
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowNetwork
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
				return ErrInvalidLengthNetwork
			}
			postIndex := iNdEx + intStringLen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			m.IPv6Subnet = string(dAtA[iNdEx:postIndex])
			iNdEx = postIndex
		case 5:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field IPv6Gateway", wireType)
			}
			var stringLen uint64
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowNetwork
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
				return ErrInvalidLengthNetwork
			}
			postIndex := iNdEx + intStringLen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			m.IPv6Gateway = string(dAtA[iNdEx:postIndex])
			iNdEx = postIndex
		case 6:
			if wireType != 0 {
				return fmt.Errorf("proto: wrong wireType = %d for field VlanID", wireType)
			}
			m.VlanID = 0
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowNetwork
				}
				if iNdEx >= l {
					return io.ErrUnexpectedEOF
				}
				b := dAtA[iNdEx]
				iNdEx++
				m.VlanID |= (uint32(b) & 0x7F) << shift
				if b < 0x80 {
					break
				}
			}
		case 7:
			if wireType != 0 {
				return fmt.Errorf("proto: wrong wireType = %d for field VxlanVNI", wireType)
			}
			m.VxlanVNI = 0
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowNetwork
				}
				if iNdEx >= l {
					return io.ErrUnexpectedEOF
				}
				b := dAtA[iNdEx]
				iNdEx++
				m.VxlanVNI |= (uint32(b) & 0x7F) << shift
				if b < 0x80 {
					break
				}
			}
		default:
			iNdEx = preIndex
			skippy, err := skipNetwork(dAtA[iNdEx:])
			if err != nil {
				return err
			}
			if skippy < 0 {
				return ErrInvalidLengthNetwork
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
func (m *NetworkStatus) Unmarshal(dAtA []byte) error {
	l := len(dAtA)
	iNdEx := 0
	for iNdEx < l {
		preIndex := iNdEx
		var wire uint64
		for shift := uint(0); ; shift += 7 {
			if shift >= 64 {
				return ErrIntOverflowNetwork
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
			return fmt.Errorf("proto: NetworkStatus: wiretype end group for non-group")
		}
		if fieldNum <= 0 {
			return fmt.Errorf("proto: NetworkStatus: illegal tag %d (wire type %d)", fieldNum, wire)
		}
		switch fieldNum {
		case 1:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field Workloads", wireType)
			}
			var stringLen uint64
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowNetwork
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
				return ErrInvalidLengthNetwork
			}
			postIndex := iNdEx + intStringLen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			m.Workloads = append(m.Workloads, string(dAtA[iNdEx:postIndex]))
			iNdEx = postIndex
		case 2:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field AllocatedIPv4Addrs", wireType)
			}
			var byteLen int
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowNetwork
				}
				if iNdEx >= l {
					return io.ErrUnexpectedEOF
				}
				b := dAtA[iNdEx]
				iNdEx++
				byteLen |= (int(b) & 0x7F) << shift
				if b < 0x80 {
					break
				}
			}
			if byteLen < 0 {
				return ErrInvalidLengthNetwork
			}
			postIndex := iNdEx + byteLen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			m.AllocatedIPv4Addrs = append(m.AllocatedIPv4Addrs[:0], dAtA[iNdEx:postIndex]...)
			if m.AllocatedIPv4Addrs == nil {
				m.AllocatedIPv4Addrs = []byte{}
			}
			iNdEx = postIndex
		default:
			iNdEx = preIndex
			skippy, err := skipNetwork(dAtA[iNdEx:])
			if err != nil {
				return err
			}
			if skippy < 0 {
				return ErrInvalidLengthNetwork
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
func skipNetwork(dAtA []byte) (n int, err error) {
	l := len(dAtA)
	iNdEx := 0
	for iNdEx < l {
		var wire uint64
		for shift := uint(0); ; shift += 7 {
			if shift >= 64 {
				return 0, ErrIntOverflowNetwork
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
					return 0, ErrIntOverflowNetwork
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
					return 0, ErrIntOverflowNetwork
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
				return 0, ErrInvalidLengthNetwork
			}
			return iNdEx, nil
		case 3:
			for {
				var innerWire uint64
				var start int = iNdEx
				for shift := uint(0); ; shift += 7 {
					if shift >= 64 {
						return 0, ErrIntOverflowNetwork
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
				next, err := skipNetwork(dAtA[start:])
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
	ErrInvalidLengthNetwork = fmt.Errorf("proto: negative length found during unmarshaling")
	ErrIntOverflowNetwork   = fmt.Errorf("proto: integer overflow")
)

func init() { proto.RegisterFile("network.proto", fileDescriptorNetwork) }

var fileDescriptorNetwork = []byte{
	// 632 bytes of a gzipped FileDescriptorProto
	0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xff, 0x84, 0x94, 0x5f, 0x4f, 0x13, 0x4d,
	0x18, 0xc5, 0xdf, 0x85, 0xbe, 0x2d, 0x9d, 0x52, 0xd4, 0x41, 0x61, 0x01, 0x6d, 0x49, 0x13, 0x13,
	0x2e, 0xe8, 0xae, 0x51, 0xd2, 0x18, 0x62, 0x62, 0xd8, 0xf8, 0x0f, 0x8d, 0x40, 0x0a, 0xc1, 0xeb,
	0xe9, 0xee, 0xb8, 0x8e, 0x4c, 0x67, 0x36, 0x9d, 0x69, 0x6b, 0x63, 0xbc, 0xf4, 0xab, 0x11, 0x2e,
	0x89, 0x1f, 0xa0, 0x31, 0x5c, 0x7a, 0xa9, 0x5f, 0xc0, 0xec, 0xd3, 0xd9, 0x38, 0xb5, 0x2d, 0xde,
	0xed, 0x73, 0xe6, 0x9c, 0x5f, 0x4f, 0x9f, 0xd9, 0x2c, 0x2a, 0x0b, 0xaa, 0xfb, 0xb2, 0x73, 0xe6,
	0x25, 0x1d, 0xa9, 0x25, 0x2e, 0x98, 0x71, 0xfd, 0x6e, 0x2c, 0x65, 0xcc, 0xa9, 0x4f, 0x12, 0xe6,
	0x13, 0x21, 0xa4, 0x26, 0x9a, 0x49, 0xa1, 0x46, 0xb6, 0xf5, 0xe7, 0x31, 0xd3, 0x1f, 0xba, 0x2d,
	0x2f, 0x94, 0x6d, 0x3f, 0xa1, 0x42, 0x11, 0x11, 0x49, 0x5f, 0xf5, 0xfd, 0x1e, 0x15, 0x2c, 0xa4,
	0x7e, 0x57, 0x33, 0xae, 0xd2, 0x68, 0x4c, 0x85, 0x9d, 0xf6, 0x99, 0x08, 0x79, 0x37, 0xa2, 0x19,
	0xa6, 0x6e, 0x61, 0x62, 0x19, 0x4b, 0x1f, 0xe4, 0x56, 0xf7, 0x3d, 0x4c, 0x30, 0xc0, 0x93, 0xb1,
	0xdf, 0x9f, 0xf1, 0xab, 0x69, 0xc7, 0x36, 0xd5, 0xc4, 0xd8, 0x1e, 0x5c, 0x63, 0xe3, 0xa4, 0x45,
	0xb9, 0xf2, 0x15, 0xe5, 0x34, 0xd4, 0xb2, 0x63, 0x12, 0xde, 0x35, 0x09, 0x70, 0x28, 0x5f, 0x53,
	0x41, 0x84, 0x1e, 0xf9, 0x6b, 0xe7, 0x73, 0xa8, 0x70, 0x30, 0x5a, 0x14, 0x6e, 0x20, 0xe7, 0xc4,
	0x75, 0x36, 0x9d, 0xad, 0xd2, 0xc3, 0xb2, 0x47, 0x12, 0xe6, 0x9d, 0x0c, 0x12, 0xfa, 0x96, 0x6a,
	0x12, 0x2c, 0x5f, 0x0c, 0xab, 0xff, 0x5d, 0x0e, 0xab, 0xce, 0x8f, 0x61, 0xb5, 0xb0, 0xcd, 0x04,
	0x67, 0x82, 0x36, 0xb3, 0x07, 0xfc, 0x02, 0x39, 0x87, 0xee, 0x1c, 0xe4, 0x6e, 0x40, 0xee, 0xb0,
	0xf5, 0x91, 0x86, 0x1a, 0x92, 0xeb, 0x56, 0x72, 0x29, 0xfd, 0x67, 0xdb, 0xb2, 0xcd, 0x34, 0x6d,
	0x27, 0x7a, 0xd0, 0xfc, 0x6b, 0xc6, 0xaf, 0x51, 0xee, 0x38, 0xa1, 0xa1, 0x3b, 0x0f, 0xa8, 0xdb,
	0x5e, 0x76, 0x9f, 0xa6, 0x5f, 0x7a, 0x16, 0xac, 0xa4, 0xbc, 0x94, 0xa5, 0x12, 0x1a, 0xda, 0xac,
	0xf1, 0x19, 0x37, 0x51, 0xfe, 0x58, 0x13, 0xdd, 0x55, 0x6e, 0x0e, 0x68, 0x2b, 0x13, 0x34, 0x38,
	0x0d, 0x5c, 0xc3, 0xbb, 0xa9, 0x60, 0xb6, 0x88, 0x13, 0xca, 0xee, 0xc6, 0xb7, 0xaf, 0x6b, 0xab,
	0xa8, 0xe4, 0x7f, 0x3e, 0xf4, 0x4e, 0x60, 0x87, 0x5f, 0xf0, 0x82, 0xa1, 0xaa, 0xda, 0xaf, 0x79,
	0x54, 0xb2, 0x8a, 0xe2, 0x1d, 0x94, 0x4b, 0xd7, 0x07, 0xfb, 0x2c, 0x42, 0x6d, 0x58, 0x81, 0x1e,
	0x24, 0xd4, 0xae, 0x3d, 0x3e, 0xe3, 0x57, 0x08, 0xed, 0x1f, 0xf5, 0x76, 0x8e, 0xbb, 0x2d, 0x41,
	0x35, 0xec, 0xb4, 0x18, 0xdc, 0x33, 0xd9, 0x3b, 0x2c, 0xe9, 0xed, 0xd4, 0x15, 0x1c, 0x59, 0x88,
	0xe9, 0x32, 0x7e, 0x83, 0x4a, 0x29, 0xe9, 0x25, 0xd1, 0xb4, 0x4f, 0x06, 0xb0, 0xd3, 0x62, 0x50,
	0x31, 0xa8, 0x15, 0xc8, 0xc4, 0xa3, 0x33, 0x8b, 0x35, 0x43, 0x37, 0xb5, 0x1a, 0xa6, 0x56, 0x6e,
	0xa2, 0x56, 0x63, 0x7a, 0xad, 0xc6, 0xac, 0x5a, 0x8d, 0xac, 0xd6, 0xff, 0x13, 0xb5, 0x1a, 0x33,
	0x6a, 0x4d, 0xd1, 0xf1, 0x13, 0x94, 0x3f, 0xe5, 0x44, 0xec, 0x3f, 0x73, 0xf3, 0x9b, 0xce, 0x56,
	0x39, 0x58, 0x33, 0x9c, 0x5b, 0x3d, 0x4e, 0x44, 0x9d, 0x45, 0x16, 0x62, 0x52, 0xc2, 0x01, 0x5a,
	0x38, 0xfd, 0xc4, 0x89, 0x38, 0x3d, 0xd8, 0x77, 0x0b, 0x90, 0xdf, 0x30, 0xf9, 0xe5, 0x5e, 0xaa,
	0xd7, 0x7b, 0x82, 0x59, 0x84, 0x69, 0x62, 0xed, 0xdc, 0x41, 0xe5, 0xb1, 0x17, 0x0a, 0x3f, 0x45,
	0xc5, 0x77, 0xb2, 0x73, 0xc6, 0x25, 0x89, 0x94, 0xeb, 0x6c, 0xce, 0x6f, 0x15, 0x83, 0xd5, 0x14,
	0xd9, 0xcf, 0x44, 0x1b, 0x39, 0x45, 0xc4, 0x1a, 0xe1, 0x3d, 0xce, 0x65, 0x48, 0x34, 0x8d, 0xd2,
	0x1b, 0xdc, 0x8b, 0xa2, 0x8e, 0x82, 0x57, 0x61, 0x31, 0x78, 0x6c, 0x0a, 0x56, 0x48, 0xe6, 0xa8,
	0xc3, 0x8d, 0x91, 0xd4, 0xf3, 0x87, 0xf1, 0x73, 0x58, 0x5d, 0x1a, 0x7d, 0xbf, 0x76, 0x6b, 0x4a,
	0x9d, 0xb1, 0xa4, 0xd6, 0xfc, 0x47, 0x22, 0x58, 0xbc, 0xb8, 0xaa, 0x38, 0x97, 0x57, 0x15, 0xe7,
	0xfb, 0x55, 0xc5, 0x39, 0x72, 0x5a, 0x79, 0xf8, 0x3c, 0x3c, 0xfa, 0x1d, 0x00, 0x00, 0xff, 0xff,
	0xfd, 0xf1, 0x88, 0x4a, 0x55, 0x05, 0x00, 0x00,
}
