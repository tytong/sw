// Code generated by protoc-gen-gogo. DO NOT EDIT.
// source: securitygroup.proto

package security

import proto "github.com/gogo/protobuf/proto"
import fmt "fmt"
import math "math"
import _ "github.com/pensando/grpc-gateway/third_party/googleapis/google/api"
import _ "github.com/pensando/sw/venice/utils/apigen/annotations"
import _ "github.com/gogo/protobuf/gogoproto"
import api "github.com/pensando/sw/api"
import labels "github.com/pensando/sw/api/labels"
import _ "github.com/pensando/sw/api/generated/cluster"

import io "io"

// Reference imports to suppress errors if they are not otherwise used.
var _ = proto.Marshal
var _ = fmt.Errorf
var _ = math.Inf

// SecurityGroup represents a security zone or domain
type SecurityGroup struct {
	//
	api.TypeMeta `protobuf:"bytes,1,opt,name=T,json=,inline,embedded=T" json:",inline"`
	//
	api.ObjectMeta `protobuf:"bytes,2,opt,name=O,json=meta,omitempty,embedded=O" json:"meta,omitempty"`
	// Spec contains the configuration of the security group.
	Spec SecurityGroupSpec `protobuf:"bytes,3,opt,name=Spec,json=spec,omitempty" json:"spec,omitempty"`
	// Status contains the current state of the security group.
	Status SecurityGroupStatus `protobuf:"bytes,4,opt,name=Status,json=status,omitempty" json:"status,omitempty"`
}

func (m *SecurityGroup) Reset()                    { *m = SecurityGroup{} }
func (m *SecurityGroup) String() string            { return proto.CompactTextString(m) }
func (*SecurityGroup) ProtoMessage()               {}
func (*SecurityGroup) Descriptor() ([]byte, []int) { return fileDescriptorSecuritygroup, []int{0} }

func (m *SecurityGroup) GetSpec() SecurityGroupSpec {
	if m != nil {
		return m.Spec
	}
	return SecurityGroupSpec{}
}

func (m *SecurityGroup) GetStatus() SecurityGroupStatus {
	if m != nil {
		return m.Status
	}
	return SecurityGroupStatus{}
}

// security group configuration
type SecurityGroupSpec struct {
	// Workload selector (list of labels)
	WorkloadSelector *labels.Selector `protobuf:"bytes,1,opt,name=WorkloadSelector,json=workload-selector,omitempty" json:"workload-selector,omitempty"`
	// Service object selector
	ServiceSelector []string `protobuf:"bytes,2,rep,name=ServiceSelector,json=service-labels,omitempty" json:"service-labels,omitempty"`
	// list of CIDRs that are part of this security group
	MatchPrefixes []string `protobuf:"bytes,3,rep,name=MatchPrefixes,json=match-prefixes,omitempty" json:"match-prefixes,omitempty"`
}

func (m *SecurityGroupSpec) Reset()                    { *m = SecurityGroupSpec{} }
func (m *SecurityGroupSpec) String() string            { return proto.CompactTextString(m) }
func (*SecurityGroupSpec) ProtoMessage()               {}
func (*SecurityGroupSpec) Descriptor() ([]byte, []int) { return fileDescriptorSecuritygroup, []int{1} }

func (m *SecurityGroupSpec) GetWorkloadSelector() *labels.Selector {
	if m != nil {
		return m.WorkloadSelector
	}
	return nil
}

func (m *SecurityGroupSpec) GetServiceSelector() []string {
	if m != nil {
		return m.ServiceSelector
	}
	return nil
}

func (m *SecurityGroupSpec) GetMatchPrefixes() []string {
	if m != nil {
		return m.MatchPrefixes
	}
	return nil
}

// security group status
type SecurityGroupStatus struct {
	// list of workloads that are part of this security group
	Workloads []string `protobuf:"bytes,1,rep,name=Workloads,json=workloads,omitempty" json:"workloads,omitempty"`
	// list of all policies attached to this security group
	Policies []string `protobuf:"bytes,2,rep,name=Policies" json:"Policies,omitempty"`
}

func (m *SecurityGroupStatus) Reset()                    { *m = SecurityGroupStatus{} }
func (m *SecurityGroupStatus) String() string            { return proto.CompactTextString(m) }
func (*SecurityGroupStatus) ProtoMessage()               {}
func (*SecurityGroupStatus) Descriptor() ([]byte, []int) { return fileDescriptorSecuritygroup, []int{2} }

func (m *SecurityGroupStatus) GetWorkloads() []string {
	if m != nil {
		return m.Workloads
	}
	return nil
}

func (m *SecurityGroupStatus) GetPolicies() []string {
	if m != nil {
		return m.Policies
	}
	return nil
}

func init() {
	proto.RegisterType((*SecurityGroup)(nil), "security.SecurityGroup")
	proto.RegisterType((*SecurityGroupSpec)(nil), "security.SecurityGroupSpec")
	proto.RegisterType((*SecurityGroupStatus)(nil), "security.SecurityGroupStatus")
}
func (m *SecurityGroup) Marshal() (dAtA []byte, err error) {
	size := m.Size()
	dAtA = make([]byte, size)
	n, err := m.MarshalTo(dAtA)
	if err != nil {
		return nil, err
	}
	return dAtA[:n], nil
}

func (m *SecurityGroup) MarshalTo(dAtA []byte) (int, error) {
	var i int
	_ = i
	var l int
	_ = l
	dAtA[i] = 0xa
	i++
	i = encodeVarintSecuritygroup(dAtA, i, uint64(m.TypeMeta.Size()))
	n1, err := m.TypeMeta.MarshalTo(dAtA[i:])
	if err != nil {
		return 0, err
	}
	i += n1
	dAtA[i] = 0x12
	i++
	i = encodeVarintSecuritygroup(dAtA, i, uint64(m.ObjectMeta.Size()))
	n2, err := m.ObjectMeta.MarshalTo(dAtA[i:])
	if err != nil {
		return 0, err
	}
	i += n2
	dAtA[i] = 0x1a
	i++
	i = encodeVarintSecuritygroup(dAtA, i, uint64(m.Spec.Size()))
	n3, err := m.Spec.MarshalTo(dAtA[i:])
	if err != nil {
		return 0, err
	}
	i += n3
	dAtA[i] = 0x22
	i++
	i = encodeVarintSecuritygroup(dAtA, i, uint64(m.Status.Size()))
	n4, err := m.Status.MarshalTo(dAtA[i:])
	if err != nil {
		return 0, err
	}
	i += n4
	return i, nil
}

func (m *SecurityGroupSpec) Marshal() (dAtA []byte, err error) {
	size := m.Size()
	dAtA = make([]byte, size)
	n, err := m.MarshalTo(dAtA)
	if err != nil {
		return nil, err
	}
	return dAtA[:n], nil
}

func (m *SecurityGroupSpec) MarshalTo(dAtA []byte) (int, error) {
	var i int
	_ = i
	var l int
	_ = l
	if m.WorkloadSelector != nil {
		dAtA[i] = 0xa
		i++
		i = encodeVarintSecuritygroup(dAtA, i, uint64(m.WorkloadSelector.Size()))
		n5, err := m.WorkloadSelector.MarshalTo(dAtA[i:])
		if err != nil {
			return 0, err
		}
		i += n5
	}
	if len(m.ServiceSelector) > 0 {
		for _, s := range m.ServiceSelector {
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
	if len(m.MatchPrefixes) > 0 {
		for _, s := range m.MatchPrefixes {
			dAtA[i] = 0x1a
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

func (m *SecurityGroupStatus) Marshal() (dAtA []byte, err error) {
	size := m.Size()
	dAtA = make([]byte, size)
	n, err := m.MarshalTo(dAtA)
	if err != nil {
		return nil, err
	}
	return dAtA[:n], nil
}

func (m *SecurityGroupStatus) MarshalTo(dAtA []byte) (int, error) {
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
	if len(m.Policies) > 0 {
		for _, s := range m.Policies {
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

func encodeVarintSecuritygroup(dAtA []byte, offset int, v uint64) int {
	for v >= 1<<7 {
		dAtA[offset] = uint8(v&0x7f | 0x80)
		v >>= 7
		offset++
	}
	dAtA[offset] = uint8(v)
	return offset + 1
}
func (m *SecurityGroup) Size() (n int) {
	var l int
	_ = l
	l = m.TypeMeta.Size()
	n += 1 + l + sovSecuritygroup(uint64(l))
	l = m.ObjectMeta.Size()
	n += 1 + l + sovSecuritygroup(uint64(l))
	l = m.Spec.Size()
	n += 1 + l + sovSecuritygroup(uint64(l))
	l = m.Status.Size()
	n += 1 + l + sovSecuritygroup(uint64(l))
	return n
}

func (m *SecurityGroupSpec) Size() (n int) {
	var l int
	_ = l
	if m.WorkloadSelector != nil {
		l = m.WorkloadSelector.Size()
		n += 1 + l + sovSecuritygroup(uint64(l))
	}
	if len(m.ServiceSelector) > 0 {
		for _, s := range m.ServiceSelector {
			l = len(s)
			n += 1 + l + sovSecuritygroup(uint64(l))
		}
	}
	if len(m.MatchPrefixes) > 0 {
		for _, s := range m.MatchPrefixes {
			l = len(s)
			n += 1 + l + sovSecuritygroup(uint64(l))
		}
	}
	return n
}

func (m *SecurityGroupStatus) Size() (n int) {
	var l int
	_ = l
	if len(m.Workloads) > 0 {
		for _, s := range m.Workloads {
			l = len(s)
			n += 1 + l + sovSecuritygroup(uint64(l))
		}
	}
	if len(m.Policies) > 0 {
		for _, s := range m.Policies {
			l = len(s)
			n += 1 + l + sovSecuritygroup(uint64(l))
		}
	}
	return n
}

func sovSecuritygroup(x uint64) (n int) {
	for {
		n++
		x >>= 7
		if x == 0 {
			break
		}
	}
	return n
}
func sozSecuritygroup(x uint64) (n int) {
	return sovSecuritygroup(uint64((x << 1) ^ uint64((int64(x) >> 63))))
}
func (m *SecurityGroup) Unmarshal(dAtA []byte) error {
	l := len(dAtA)
	iNdEx := 0
	for iNdEx < l {
		preIndex := iNdEx
		var wire uint64
		for shift := uint(0); ; shift += 7 {
			if shift >= 64 {
				return ErrIntOverflowSecuritygroup
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
			return fmt.Errorf("proto: SecurityGroup: wiretype end group for non-group")
		}
		if fieldNum <= 0 {
			return fmt.Errorf("proto: SecurityGroup: illegal tag %d (wire type %d)", fieldNum, wire)
		}
		switch fieldNum {
		case 1:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field TypeMeta", wireType)
			}
			var msglen int
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowSecuritygroup
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
				return ErrInvalidLengthSecuritygroup
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
					return ErrIntOverflowSecuritygroup
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
				return ErrInvalidLengthSecuritygroup
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
					return ErrIntOverflowSecuritygroup
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
				return ErrInvalidLengthSecuritygroup
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
					return ErrIntOverflowSecuritygroup
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
				return ErrInvalidLengthSecuritygroup
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
			skippy, err := skipSecuritygroup(dAtA[iNdEx:])
			if err != nil {
				return err
			}
			if skippy < 0 {
				return ErrInvalidLengthSecuritygroup
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
func (m *SecurityGroupSpec) Unmarshal(dAtA []byte) error {
	l := len(dAtA)
	iNdEx := 0
	for iNdEx < l {
		preIndex := iNdEx
		var wire uint64
		for shift := uint(0); ; shift += 7 {
			if shift >= 64 {
				return ErrIntOverflowSecuritygroup
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
			return fmt.Errorf("proto: SecurityGroupSpec: wiretype end group for non-group")
		}
		if fieldNum <= 0 {
			return fmt.Errorf("proto: SecurityGroupSpec: illegal tag %d (wire type %d)", fieldNum, wire)
		}
		switch fieldNum {
		case 1:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field WorkloadSelector", wireType)
			}
			var msglen int
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowSecuritygroup
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
				return ErrInvalidLengthSecuritygroup
			}
			postIndex := iNdEx + msglen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			if m.WorkloadSelector == nil {
				m.WorkloadSelector = &labels.Selector{}
			}
			if err := m.WorkloadSelector.Unmarshal(dAtA[iNdEx:postIndex]); err != nil {
				return err
			}
			iNdEx = postIndex
		case 2:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field ServiceSelector", wireType)
			}
			var stringLen uint64
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowSecuritygroup
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
				return ErrInvalidLengthSecuritygroup
			}
			postIndex := iNdEx + intStringLen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			m.ServiceSelector = append(m.ServiceSelector, string(dAtA[iNdEx:postIndex]))
			iNdEx = postIndex
		case 3:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field MatchPrefixes", wireType)
			}
			var stringLen uint64
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowSecuritygroup
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
				return ErrInvalidLengthSecuritygroup
			}
			postIndex := iNdEx + intStringLen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			m.MatchPrefixes = append(m.MatchPrefixes, string(dAtA[iNdEx:postIndex]))
			iNdEx = postIndex
		default:
			iNdEx = preIndex
			skippy, err := skipSecuritygroup(dAtA[iNdEx:])
			if err != nil {
				return err
			}
			if skippy < 0 {
				return ErrInvalidLengthSecuritygroup
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
func (m *SecurityGroupStatus) Unmarshal(dAtA []byte) error {
	l := len(dAtA)
	iNdEx := 0
	for iNdEx < l {
		preIndex := iNdEx
		var wire uint64
		for shift := uint(0); ; shift += 7 {
			if shift >= 64 {
				return ErrIntOverflowSecuritygroup
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
			return fmt.Errorf("proto: SecurityGroupStatus: wiretype end group for non-group")
		}
		if fieldNum <= 0 {
			return fmt.Errorf("proto: SecurityGroupStatus: illegal tag %d (wire type %d)", fieldNum, wire)
		}
		switch fieldNum {
		case 1:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field Workloads", wireType)
			}
			var stringLen uint64
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowSecuritygroup
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
				return ErrInvalidLengthSecuritygroup
			}
			postIndex := iNdEx + intStringLen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			m.Workloads = append(m.Workloads, string(dAtA[iNdEx:postIndex]))
			iNdEx = postIndex
		case 2:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field Policies", wireType)
			}
			var stringLen uint64
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowSecuritygroup
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
				return ErrInvalidLengthSecuritygroup
			}
			postIndex := iNdEx + intStringLen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			m.Policies = append(m.Policies, string(dAtA[iNdEx:postIndex]))
			iNdEx = postIndex
		default:
			iNdEx = preIndex
			skippy, err := skipSecuritygroup(dAtA[iNdEx:])
			if err != nil {
				return err
			}
			if skippy < 0 {
				return ErrInvalidLengthSecuritygroup
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
func skipSecuritygroup(dAtA []byte) (n int, err error) {
	l := len(dAtA)
	iNdEx := 0
	for iNdEx < l {
		var wire uint64
		for shift := uint(0); ; shift += 7 {
			if shift >= 64 {
				return 0, ErrIntOverflowSecuritygroup
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
					return 0, ErrIntOverflowSecuritygroup
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
					return 0, ErrIntOverflowSecuritygroup
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
				return 0, ErrInvalidLengthSecuritygroup
			}
			return iNdEx, nil
		case 3:
			for {
				var innerWire uint64
				var start int = iNdEx
				for shift := uint(0); ; shift += 7 {
					if shift >= 64 {
						return 0, ErrIntOverflowSecuritygroup
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
				next, err := skipSecuritygroup(dAtA[start:])
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
	ErrInvalidLengthSecuritygroup = fmt.Errorf("proto: negative length found during unmarshaling")
	ErrIntOverflowSecuritygroup   = fmt.Errorf("proto: integer overflow")
)

func init() { proto.RegisterFile("securitygroup.proto", fileDescriptorSecuritygroup) }

var fileDescriptorSecuritygroup = []byte{
	// 541 bytes of a gzipped FileDescriptorProto
	0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xff, 0x7c, 0x93, 0xc1, 0x6e, 0xd3, 0x40,
	0x10, 0x86, 0x71, 0x5a, 0x95, 0x76, 0x4b, 0x48, 0xd8, 0x48, 0xc8, 0x24, 0x25, 0x8e, 0x22, 0x21,
	0xf5, 0xd0, 0x78, 0x11, 0x48, 0x1c, 0x38, 0x1a, 0x01, 0xa7, 0x2a, 0x69, 0x12, 0x09, 0x71, 0xdc,
	0x6c, 0xa6, 0xce, 0x82, 0xb3, 0x6b, 0x79, 0xd7, 0x29, 0x11, 0xe2, 0xc8, 0x83, 0xf0, 0x36, 0x39,
	0x56, 0x3c, 0x80, 0x05, 0x39, 0xe6, 0x29, 0x90, 0xd7, 0x76, 0xe5, 0xd2, 0x26, 0x37, 0xcf, 0xcc,
	0xff, 0x7f, 0x33, 0x3b, 0x93, 0xa0, 0x86, 0x02, 0x16, 0x47, 0x5c, 0x2f, 0xfd, 0x48, 0xc6, 0xa1,
	0x1b, 0x46, 0x52, 0x4b, 0x7c, 0x58, 0x24, 0x9b, 0x27, 0xbe, 0x94, 0x7e, 0x00, 0x84, 0x86, 0x9c,
	0x50, 0x21, 0xa4, 0xa6, 0x9a, 0x4b, 0xa1, 0x32, 0x5d, 0xf3, 0xbd, 0xcf, 0xf5, 0x2c, 0x9e, 0xb8,
	0x4c, 0xce, 0x49, 0x08, 0x42, 0x51, 0x31, 0x95, 0x44, 0x5d, 0x91, 0x05, 0x08, 0xce, 0x80, 0xc4,
	0x9a, 0x07, 0x2a, 0xb5, 0xfa, 0x20, 0xca, 0x6e, 0xc2, 0x05, 0x0b, 0xe2, 0x29, 0x14, 0x98, 0x5e,
	0x09, 0xe3, 0x4b, 0x5f, 0x12, 0x93, 0x9e, 0xc4, 0x97, 0x26, 0x32, 0x81, 0xf9, 0xca, 0xe5, 0x2f,
	0xb6, 0x74, 0x4d, 0x67, 0x9c, 0x83, 0xa6, 0xb9, 0xec, 0xe5, 0x0e, 0x59, 0x40, 0x27, 0x10, 0x28,
	0xa2, 0x20, 0x00, 0xa6, 0x65, 0x94, 0x3b, 0xdc, 0x1d, 0x0e, 0xa3, 0x50, 0x44, 0x83, 0xa0, 0x42,
	0x67, 0xfa, 0xee, 0xdf, 0x0a, 0xaa, 0x8e, 0xf2, 0x4d, 0x7d, 0x4c, 0xd7, 0x87, 0xdf, 0x20, 0x6b,
	0x6c, 0x5b, 0x1d, 0xeb, 0xf4, 0xf8, 0x55, 0xd5, 0xa5, 0x21, 0x77, 0xc7, 0xcb, 0x10, 0xce, 0x41,
	0x53, 0xaf, 0xb1, 0x4a, 0x9c, 0x07, 0xd7, 0x89, 0x63, 0x6d, 0x12, 0xe7, 0xe1, 0x19, 0x17, 0x01,
	0x17, 0x30, 0x2c, 0x3e, 0xf0, 0x07, 0x64, 0xf5, 0xed, 0x8a, 0xf1, 0xd5, 0x8c, 0xaf, 0x3f, 0xf9,
	0x02, 0x4c, 0x1b, 0x67, 0xb3, 0xe4, 0x7c, 0x9c, 0xbe, 0xef, 0x4c, 0xce, 0xb9, 0x86, 0x79, 0xa8,
	0x97, 0xc3, 0xff, 0x62, 0x7c, 0x81, 0xf6, 0x47, 0x21, 0x30, 0x7b, 0xcf, 0xa0, 0x5a, 0x6e, 0x71,
	0x47, 0xf7, 0xd6, 0x98, 0xa9, 0xc4, 0x7b, 0x9a, 0x62, 0x53, 0xa4, 0x0a, 0x81, 0x95, 0x91, 0xb7,
	0x63, 0xfc, 0x19, 0x1d, 0x8c, 0x34, 0xd5, 0xb1, 0xb2, 0xf7, 0x0d, 0xf4, 0xf9, 0x36, 0xa8, 0x11,
	0x79, 0x76, 0x8e, 0xad, 0x2b, 0x13, 0x97, 0xc0, 0x77, 0x32, 0x6f, 0xbb, 0xbf, 0x7f, 0x3e, 0x6b,
	0xa3, 0x63, 0xf2, 0xbd, 0xef, 0x8e, 0xcd, 0x5e, 0x7f, 0xe0, 0x5a, 0x01, 0xef, 0x99, 0xdf, 0xa3,
	0xea, 0xfe, 0xaa, 0xa0, 0x27, 0x77, 0x86, 0xc7, 0x33, 0x54, 0xff, 0x24, 0xa3, 0xaf, 0x81, 0xa4,
	0xd3, 0x51, 0x7e, 0xc3, 0x7c, 0xed, 0x75, 0x37, 0xbb, 0xad, 0x5b, 0xe4, 0x3d, 0x67, 0x93, 0x38,
	0xad, 0xab, 0x5c, 0xdd, 0x2b, 0x4e, 0x5e, 0x1a, 0x6c, 0x57, 0x11, 0xf7, 0x51, 0x6d, 0x04, 0xd1,
	0x82, 0x33, 0xb8, 0x69, 0x54, 0xe9, 0xec, 0x9d, 0x1e, 0x79, 0x27, 0x9b, 0xc4, 0xb1, 0x55, 0x56,
	0xea, 0x65, 0x3d, 0x4b, 0xcc, 0xad, 0x15, 0x7c, 0x81, 0xaa, 0xe7, 0x54, 0xb3, 0xd9, 0x20, 0x82,
	0x4b, 0xfe, 0x0d, 0x94, 0xbd, 0x67, 0x70, 0x9d, 0x55, 0x76, 0x61, 0x7b, 0x9e, 0x16, 0x7b, 0x61,
	0x5e, 0x2d, 0x23, 0xb7, 0x55, 0xba, 0x0b, 0xd4, 0xb8, 0xe7, 0x14, 0xf8, 0x1d, 0x3a, 0x2a, 0x96,
	0xa4, 0x6c, 0xcb, 0x74, 0x69, 0xe5, 0x5d, 0x1a, 0xc5, 0x93, 0xcb, 0x0d, 0xee, 0x4b, 0xe2, 0x26,
	0x3a, 0x1c, 0xc8, 0x80, 0x33, 0x0e, 0x2a, 0x7b, 0xf8, 0xf0, 0x26, 0xf6, 0x1e, 0xad, 0xd6, 0x6d,
	0xeb, 0x7a, 0xdd, 0xb6, 0xfe, 0xac, 0xdb, 0xd6, 0xc0, 0x9a, 0x1c, 0x98, 0xbf, 0xc5, 0xeb, 0x7f,
	0x01, 0x00, 0x00, 0xff, 0xff, 0xb6, 0x8f, 0x69, 0x51, 0x54, 0x04, 0x00, 0x00,
}
