// Code generated by protoc-gen-gogo. DO NOT EDIT.
// source: sgpolicy.proto

package security

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

//
type SGRule struct {
	// match ports for the rule
	Ports string `protobuf:"bytes,1,opt,name=Ports,json=ports,omitempty,proto3" json:"ports,omitempty"`
	// Rule action (allow/deny/log/train)
	Action string `protobuf:"bytes,2,opt,name=Action,json=action,omitempty,proto3" json:"action,omitempty"`
	// Peer group for the rule (from/to group depending on direction)
	PeerGroup string `protobuf:"bytes,3,opt,name=PeerGroup,json=peer-group,omitempty,proto3" json:"peer-group,omitempty"`
	// List of Apps to match for the rule
	Apps []string `protobuf:"bytes,4,rep,name=Apps,json=apps,omitempty" json:"apps,omitempty"`
	// AppUser or AppUserGroup to match for the rule
	// AppUser is derived from application payload such as database login or
	// other application authentication mechanisms
	// FIXME: oneof does not translate well in golang - will enforce via validation
	// Used when policy is applied on a single user
	AppUser string `protobuf:"bytes,5,opt,name=AppUser,json=app-user,omitempty,proto3" json:"app-user,omitempty"`
	// Used when policy is applied on a group of users
	AppUserGrp string `protobuf:"bytes,6,opt,name=AppUserGrp,json=app-user-group,omitempty,proto3" json:"app-user-group,omitempty"`
}

func (m *SGRule) Reset()                    { *m = SGRule{} }
func (m *SGRule) String() string            { return proto.CompactTextString(m) }
func (*SGRule) ProtoMessage()               {}
func (*SGRule) Descriptor() ([]byte, []int) { return fileDescriptorSgpolicy, []int{0} }

func (m *SGRule) GetPorts() string {
	if m != nil {
		return m.Ports
	}
	return ""
}

func (m *SGRule) GetAction() string {
	if m != nil {
		return m.Action
	}
	return ""
}

func (m *SGRule) GetPeerGroup() string {
	if m != nil {
		return m.PeerGroup
	}
	return ""
}

func (m *SGRule) GetApps() []string {
	if m != nil {
		return m.Apps
	}
	return nil
}

func (m *SGRule) GetAppUser() string {
	if m != nil {
		return m.AppUser
	}
	return ""
}

func (m *SGRule) GetAppUserGrp() string {
	if m != nil {
		return m.AppUserGrp
	}
	return ""
}

// Sgpolicy represents a security policy for security groups
type Sgpolicy struct {
	//
	api.TypeMeta `protobuf:"bytes,1,opt,name=T,json=,inline,embedded=T" json:",inline"`
	//
	api.ObjectMeta `protobuf:"bytes,2,opt,name=O,json=meta,omitempty,embedded=O" json:"meta,omitempty"`
	// Spec contains the configuration of the sgpolicy.
	Spec SgpolicySpec `protobuf:"bytes,3,opt,name=Spec,json=spec,omitempty" json:"spec,omitempty"`
	// Status contains the current state of the sgpolicy.
	Status SgpolicyStatus `protobuf:"bytes,4,opt,name=Status,json=status,omitempty" json:"status,omitempty"`
}

func (m *Sgpolicy) Reset()                    { *m = Sgpolicy{} }
func (m *Sgpolicy) String() string            { return proto.CompactTextString(m) }
func (*Sgpolicy) ProtoMessage()               {}
func (*Sgpolicy) Descriptor() ([]byte, []int) { return fileDescriptorSgpolicy, []int{1} }

func (m *Sgpolicy) GetSpec() SgpolicySpec {
	if m != nil {
		return m.Spec
	}
	return SgpolicySpec{}
}

func (m *Sgpolicy) GetStatus() SgpolicyStatus {
	if m != nil {
		return m.Status
	}
	return SgpolicyStatus{}
}

//
type SgpolicySpec struct {
	// list of security groups this policy is attached to
	AttachGroups []string `protobuf:"bytes,1,rep,name=AttachGroups,json=attach-groups,omitempty" json:"attach-groups,omitempty"`
	// Incoming rules
	InRules []SGRule `protobuf:"bytes,2,rep,name=InRules,json=in-rules,omitempty" json:"in-rules,omitempty"`
	// Outgoing rules
	OutRules []SGRule `protobuf:"bytes,3,rep,name=OutRules,json=out-rules,omitempty" json:"out-rules,omitempty"`
}

func (m *SgpolicySpec) Reset()                    { *m = SgpolicySpec{} }
func (m *SgpolicySpec) String() string            { return proto.CompactTextString(m) }
func (*SgpolicySpec) ProtoMessage()               {}
func (*SgpolicySpec) Descriptor() ([]byte, []int) { return fileDescriptorSgpolicy, []int{2} }

func (m *SgpolicySpec) GetAttachGroups() []string {
	if m != nil {
		return m.AttachGroups
	}
	return nil
}

func (m *SgpolicySpec) GetInRules() []SGRule {
	if m != nil {
		return m.InRules
	}
	return nil
}

func (m *SgpolicySpec) GetOutRules() []SGRule {
	if m != nil {
		return m.OutRules
	}
	return nil
}

//
type SgpolicyStatus struct {
	// list of workloads in this group
	Workloads []string `protobuf:"bytes,1,rep,name=Workloads,json=workloads,omitempty" json:"workloads,omitempty"`
}

func (m *SgpolicyStatus) Reset()                    { *m = SgpolicyStatus{} }
func (m *SgpolicyStatus) String() string            { return proto.CompactTextString(m) }
func (*SgpolicyStatus) ProtoMessage()               {}
func (*SgpolicyStatus) Descriptor() ([]byte, []int) { return fileDescriptorSgpolicy, []int{3} }

func (m *SgpolicyStatus) GetWorkloads() []string {
	if m != nil {
		return m.Workloads
	}
	return nil
}

func init() {
	proto.RegisterType((*SGRule)(nil), "security.SGRule")
	proto.RegisterType((*Sgpolicy)(nil), "security.Sgpolicy")
	proto.RegisterType((*SgpolicySpec)(nil), "security.SgpolicySpec")
	proto.RegisterType((*SgpolicyStatus)(nil), "security.SgpolicyStatus")
}
func (m *SGRule) Marshal() (dAtA []byte, err error) {
	size := m.Size()
	dAtA = make([]byte, size)
	n, err := m.MarshalTo(dAtA)
	if err != nil {
		return nil, err
	}
	return dAtA[:n], nil
}

func (m *SGRule) MarshalTo(dAtA []byte) (int, error) {
	var i int
	_ = i
	var l int
	_ = l
	if len(m.Ports) > 0 {
		dAtA[i] = 0xa
		i++
		i = encodeVarintSgpolicy(dAtA, i, uint64(len(m.Ports)))
		i += copy(dAtA[i:], m.Ports)
	}
	if len(m.Action) > 0 {
		dAtA[i] = 0x12
		i++
		i = encodeVarintSgpolicy(dAtA, i, uint64(len(m.Action)))
		i += copy(dAtA[i:], m.Action)
	}
	if len(m.PeerGroup) > 0 {
		dAtA[i] = 0x1a
		i++
		i = encodeVarintSgpolicy(dAtA, i, uint64(len(m.PeerGroup)))
		i += copy(dAtA[i:], m.PeerGroup)
	}
	if len(m.Apps) > 0 {
		for _, s := range m.Apps {
			dAtA[i] = 0x22
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
	if len(m.AppUser) > 0 {
		dAtA[i] = 0x2a
		i++
		i = encodeVarintSgpolicy(dAtA, i, uint64(len(m.AppUser)))
		i += copy(dAtA[i:], m.AppUser)
	}
	if len(m.AppUserGrp) > 0 {
		dAtA[i] = 0x32
		i++
		i = encodeVarintSgpolicy(dAtA, i, uint64(len(m.AppUserGrp)))
		i += copy(dAtA[i:], m.AppUserGrp)
	}
	return i, nil
}

func (m *Sgpolicy) Marshal() (dAtA []byte, err error) {
	size := m.Size()
	dAtA = make([]byte, size)
	n, err := m.MarshalTo(dAtA)
	if err != nil {
		return nil, err
	}
	return dAtA[:n], nil
}

func (m *Sgpolicy) MarshalTo(dAtA []byte) (int, error) {
	var i int
	_ = i
	var l int
	_ = l
	dAtA[i] = 0xa
	i++
	i = encodeVarintSgpolicy(dAtA, i, uint64(m.TypeMeta.Size()))
	n1, err := m.TypeMeta.MarshalTo(dAtA[i:])
	if err != nil {
		return 0, err
	}
	i += n1
	dAtA[i] = 0x12
	i++
	i = encodeVarintSgpolicy(dAtA, i, uint64(m.ObjectMeta.Size()))
	n2, err := m.ObjectMeta.MarshalTo(dAtA[i:])
	if err != nil {
		return 0, err
	}
	i += n2
	dAtA[i] = 0x1a
	i++
	i = encodeVarintSgpolicy(dAtA, i, uint64(m.Spec.Size()))
	n3, err := m.Spec.MarshalTo(dAtA[i:])
	if err != nil {
		return 0, err
	}
	i += n3
	dAtA[i] = 0x22
	i++
	i = encodeVarintSgpolicy(dAtA, i, uint64(m.Status.Size()))
	n4, err := m.Status.MarshalTo(dAtA[i:])
	if err != nil {
		return 0, err
	}
	i += n4
	return i, nil
}

func (m *SgpolicySpec) Marshal() (dAtA []byte, err error) {
	size := m.Size()
	dAtA = make([]byte, size)
	n, err := m.MarshalTo(dAtA)
	if err != nil {
		return nil, err
	}
	return dAtA[:n], nil
}

func (m *SgpolicySpec) MarshalTo(dAtA []byte) (int, error) {
	var i int
	_ = i
	var l int
	_ = l
	if len(m.AttachGroups) > 0 {
		for _, s := range m.AttachGroups {
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
	if len(m.InRules) > 0 {
		for _, msg := range m.InRules {
			dAtA[i] = 0x12
			i++
			i = encodeVarintSgpolicy(dAtA, i, uint64(msg.Size()))
			n, err := msg.MarshalTo(dAtA[i:])
			if err != nil {
				return 0, err
			}
			i += n
		}
	}
	if len(m.OutRules) > 0 {
		for _, msg := range m.OutRules {
			dAtA[i] = 0x1a
			i++
			i = encodeVarintSgpolicy(dAtA, i, uint64(msg.Size()))
			n, err := msg.MarshalTo(dAtA[i:])
			if err != nil {
				return 0, err
			}
			i += n
		}
	}
	return i, nil
}

func (m *SgpolicyStatus) Marshal() (dAtA []byte, err error) {
	size := m.Size()
	dAtA = make([]byte, size)
	n, err := m.MarshalTo(dAtA)
	if err != nil {
		return nil, err
	}
	return dAtA[:n], nil
}

func (m *SgpolicyStatus) MarshalTo(dAtA []byte) (int, error) {
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
	return i, nil
}

func encodeVarintSgpolicy(dAtA []byte, offset int, v uint64) int {
	for v >= 1<<7 {
		dAtA[offset] = uint8(v&0x7f | 0x80)
		v >>= 7
		offset++
	}
	dAtA[offset] = uint8(v)
	return offset + 1
}
func (m *SGRule) Size() (n int) {
	var l int
	_ = l
	l = len(m.Ports)
	if l > 0 {
		n += 1 + l + sovSgpolicy(uint64(l))
	}
	l = len(m.Action)
	if l > 0 {
		n += 1 + l + sovSgpolicy(uint64(l))
	}
	l = len(m.PeerGroup)
	if l > 0 {
		n += 1 + l + sovSgpolicy(uint64(l))
	}
	if len(m.Apps) > 0 {
		for _, s := range m.Apps {
			l = len(s)
			n += 1 + l + sovSgpolicy(uint64(l))
		}
	}
	l = len(m.AppUser)
	if l > 0 {
		n += 1 + l + sovSgpolicy(uint64(l))
	}
	l = len(m.AppUserGrp)
	if l > 0 {
		n += 1 + l + sovSgpolicy(uint64(l))
	}
	return n
}

func (m *Sgpolicy) Size() (n int) {
	var l int
	_ = l
	l = m.TypeMeta.Size()
	n += 1 + l + sovSgpolicy(uint64(l))
	l = m.ObjectMeta.Size()
	n += 1 + l + sovSgpolicy(uint64(l))
	l = m.Spec.Size()
	n += 1 + l + sovSgpolicy(uint64(l))
	l = m.Status.Size()
	n += 1 + l + sovSgpolicy(uint64(l))
	return n
}

func (m *SgpolicySpec) Size() (n int) {
	var l int
	_ = l
	if len(m.AttachGroups) > 0 {
		for _, s := range m.AttachGroups {
			l = len(s)
			n += 1 + l + sovSgpolicy(uint64(l))
		}
	}
	if len(m.InRules) > 0 {
		for _, e := range m.InRules {
			l = e.Size()
			n += 1 + l + sovSgpolicy(uint64(l))
		}
	}
	if len(m.OutRules) > 0 {
		for _, e := range m.OutRules {
			l = e.Size()
			n += 1 + l + sovSgpolicy(uint64(l))
		}
	}
	return n
}

func (m *SgpolicyStatus) Size() (n int) {
	var l int
	_ = l
	if len(m.Workloads) > 0 {
		for _, s := range m.Workloads {
			l = len(s)
			n += 1 + l + sovSgpolicy(uint64(l))
		}
	}
	return n
}

func sovSgpolicy(x uint64) (n int) {
	for {
		n++
		x >>= 7
		if x == 0 {
			break
		}
	}
	return n
}
func sozSgpolicy(x uint64) (n int) {
	return sovSgpolicy(uint64((x << 1) ^ uint64((int64(x) >> 63))))
}
func (m *SGRule) Unmarshal(dAtA []byte) error {
	l := len(dAtA)
	iNdEx := 0
	for iNdEx < l {
		preIndex := iNdEx
		var wire uint64
		for shift := uint(0); ; shift += 7 {
			if shift >= 64 {
				return ErrIntOverflowSgpolicy
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
			return fmt.Errorf("proto: SGRule: wiretype end group for non-group")
		}
		if fieldNum <= 0 {
			return fmt.Errorf("proto: SGRule: illegal tag %d (wire type %d)", fieldNum, wire)
		}
		switch fieldNum {
		case 1:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field Ports", wireType)
			}
			var stringLen uint64
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowSgpolicy
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
				return ErrInvalidLengthSgpolicy
			}
			postIndex := iNdEx + intStringLen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			m.Ports = string(dAtA[iNdEx:postIndex])
			iNdEx = postIndex
		case 2:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field Action", wireType)
			}
			var stringLen uint64
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowSgpolicy
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
				return ErrInvalidLengthSgpolicy
			}
			postIndex := iNdEx + intStringLen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			m.Action = string(dAtA[iNdEx:postIndex])
			iNdEx = postIndex
		case 3:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field PeerGroup", wireType)
			}
			var stringLen uint64
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowSgpolicy
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
				return ErrInvalidLengthSgpolicy
			}
			postIndex := iNdEx + intStringLen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			m.PeerGroup = string(dAtA[iNdEx:postIndex])
			iNdEx = postIndex
		case 4:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field Apps", wireType)
			}
			var stringLen uint64
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowSgpolicy
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
				return ErrInvalidLengthSgpolicy
			}
			postIndex := iNdEx + intStringLen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			m.Apps = append(m.Apps, string(dAtA[iNdEx:postIndex]))
			iNdEx = postIndex
		case 5:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field AppUser", wireType)
			}
			var stringLen uint64
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowSgpolicy
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
				return ErrInvalidLengthSgpolicy
			}
			postIndex := iNdEx + intStringLen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			m.AppUser = string(dAtA[iNdEx:postIndex])
			iNdEx = postIndex
		case 6:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field AppUserGrp", wireType)
			}
			var stringLen uint64
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowSgpolicy
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
				return ErrInvalidLengthSgpolicy
			}
			postIndex := iNdEx + intStringLen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			m.AppUserGrp = string(dAtA[iNdEx:postIndex])
			iNdEx = postIndex
		default:
			iNdEx = preIndex
			skippy, err := skipSgpolicy(dAtA[iNdEx:])
			if err != nil {
				return err
			}
			if skippy < 0 {
				return ErrInvalidLengthSgpolicy
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
func (m *Sgpolicy) Unmarshal(dAtA []byte) error {
	l := len(dAtA)
	iNdEx := 0
	for iNdEx < l {
		preIndex := iNdEx
		var wire uint64
		for shift := uint(0); ; shift += 7 {
			if shift >= 64 {
				return ErrIntOverflowSgpolicy
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
			return fmt.Errorf("proto: Sgpolicy: wiretype end group for non-group")
		}
		if fieldNum <= 0 {
			return fmt.Errorf("proto: Sgpolicy: illegal tag %d (wire type %d)", fieldNum, wire)
		}
		switch fieldNum {
		case 1:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field TypeMeta", wireType)
			}
			var msglen int
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowSgpolicy
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
				return ErrInvalidLengthSgpolicy
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
					return ErrIntOverflowSgpolicy
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
				return ErrInvalidLengthSgpolicy
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
					return ErrIntOverflowSgpolicy
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
				return ErrInvalidLengthSgpolicy
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
					return ErrIntOverflowSgpolicy
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
				return ErrInvalidLengthSgpolicy
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
			skippy, err := skipSgpolicy(dAtA[iNdEx:])
			if err != nil {
				return err
			}
			if skippy < 0 {
				return ErrInvalidLengthSgpolicy
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
func (m *SgpolicySpec) Unmarshal(dAtA []byte) error {
	l := len(dAtA)
	iNdEx := 0
	for iNdEx < l {
		preIndex := iNdEx
		var wire uint64
		for shift := uint(0); ; shift += 7 {
			if shift >= 64 {
				return ErrIntOverflowSgpolicy
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
			return fmt.Errorf("proto: SgpolicySpec: wiretype end group for non-group")
		}
		if fieldNum <= 0 {
			return fmt.Errorf("proto: SgpolicySpec: illegal tag %d (wire type %d)", fieldNum, wire)
		}
		switch fieldNum {
		case 1:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field AttachGroups", wireType)
			}
			var stringLen uint64
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowSgpolicy
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
				return ErrInvalidLengthSgpolicy
			}
			postIndex := iNdEx + intStringLen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			m.AttachGroups = append(m.AttachGroups, string(dAtA[iNdEx:postIndex]))
			iNdEx = postIndex
		case 2:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field InRules", wireType)
			}
			var msglen int
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowSgpolicy
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
				return ErrInvalidLengthSgpolicy
			}
			postIndex := iNdEx + msglen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			m.InRules = append(m.InRules, SGRule{})
			if err := m.InRules[len(m.InRules)-1].Unmarshal(dAtA[iNdEx:postIndex]); err != nil {
				return err
			}
			iNdEx = postIndex
		case 3:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field OutRules", wireType)
			}
			var msglen int
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowSgpolicy
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
				return ErrInvalidLengthSgpolicy
			}
			postIndex := iNdEx + msglen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			m.OutRules = append(m.OutRules, SGRule{})
			if err := m.OutRules[len(m.OutRules)-1].Unmarshal(dAtA[iNdEx:postIndex]); err != nil {
				return err
			}
			iNdEx = postIndex
		default:
			iNdEx = preIndex
			skippy, err := skipSgpolicy(dAtA[iNdEx:])
			if err != nil {
				return err
			}
			if skippy < 0 {
				return ErrInvalidLengthSgpolicy
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
func (m *SgpolicyStatus) Unmarshal(dAtA []byte) error {
	l := len(dAtA)
	iNdEx := 0
	for iNdEx < l {
		preIndex := iNdEx
		var wire uint64
		for shift := uint(0); ; shift += 7 {
			if shift >= 64 {
				return ErrIntOverflowSgpolicy
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
			return fmt.Errorf("proto: SgpolicyStatus: wiretype end group for non-group")
		}
		if fieldNum <= 0 {
			return fmt.Errorf("proto: SgpolicyStatus: illegal tag %d (wire type %d)", fieldNum, wire)
		}
		switch fieldNum {
		case 1:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field Workloads", wireType)
			}
			var stringLen uint64
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowSgpolicy
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
				return ErrInvalidLengthSgpolicy
			}
			postIndex := iNdEx + intStringLen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			m.Workloads = append(m.Workloads, string(dAtA[iNdEx:postIndex]))
			iNdEx = postIndex
		default:
			iNdEx = preIndex
			skippy, err := skipSgpolicy(dAtA[iNdEx:])
			if err != nil {
				return err
			}
			if skippy < 0 {
				return ErrInvalidLengthSgpolicy
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
func skipSgpolicy(dAtA []byte) (n int, err error) {
	l := len(dAtA)
	iNdEx := 0
	for iNdEx < l {
		var wire uint64
		for shift := uint(0); ; shift += 7 {
			if shift >= 64 {
				return 0, ErrIntOverflowSgpolicy
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
					return 0, ErrIntOverflowSgpolicy
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
					return 0, ErrIntOverflowSgpolicy
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
				return 0, ErrInvalidLengthSgpolicy
			}
			return iNdEx, nil
		case 3:
			for {
				var innerWire uint64
				var start int = iNdEx
				for shift := uint(0); ; shift += 7 {
					if shift >= 64 {
						return 0, ErrIntOverflowSgpolicy
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
				next, err := skipSgpolicy(dAtA[start:])
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
	ErrInvalidLengthSgpolicy = fmt.Errorf("proto: negative length found during unmarshaling")
	ErrIntOverflowSgpolicy   = fmt.Errorf("proto: integer overflow")
)

func init() { proto.RegisterFile("sgpolicy.proto", fileDescriptorSgpolicy) }

var fileDescriptorSgpolicy = []byte{
	// 660 bytes of a gzipped FileDescriptorProto
	0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xff, 0x7c, 0x54, 0x51, 0x4f, 0xdb, 0x3c,
	0x14, 0xfd, 0xd2, 0x42, 0x69, 0x5d, 0xbe, 0x82, 0x5c, 0x04, 0x19, 0x20, 0x82, 0x2a, 0x4d, 0xe2,
	0x81, 0x26, 0xa8, 0x48, 0x3c, 0x6c, 0x4f, 0xed, 0xb4, 0xa1, 0x4d, 0xdb, 0xca, 0x68, 0xd1, 0x9e,
	0xdd, 0x70, 0x17, 0xb2, 0xa5, 0xb6, 0x15, 0x3b, 0x43, 0xd5, 0xb4, 0xc7, 0x69, 0x7f, 0x0d, 0x69,
	0x2f, 0x68, 0x3f, 0x20, 0x9a, 0x78, 0xcc, 0xeb, 0xfe, 0xc0, 0x64, 0x37, 0x91, 0x0c, 0x6d, 0x79,
	0xcb, 0x3d, 0xf7, 0x9e, 0xe3, 0xdb, 0xe3, 0xe3, 0xa2, 0x86, 0x08, 0x38, 0x8b, 0x42, 0x7f, 0xe2,
	0xf2, 0x98, 0x49, 0x86, 0xab, 0x02, 0xfc, 0x24, 0x0e, 0xe5, 0x64, 0x7b, 0x37, 0x60, 0x2c, 0x88,
	0xc0, 0x23, 0x3c, 0xf4, 0x08, 0xa5, 0x4c, 0x12, 0x19, 0x32, 0x2a, 0xa6, 0x73, 0xdb, 0x2f, 0x83,
	0x50, 0x5e, 0x25, 0x23, 0xd7, 0x67, 0x63, 0x8f, 0x03, 0x15, 0x84, 0x5e, 0x32, 0x4f, 0x5c, 0x7b,
	0x5f, 0x81, 0x86, 0x3e, 0x78, 0x89, 0x0c, 0x23, 0xa1, 0xa8, 0x01, 0x50, 0x93, 0xed, 0x85, 0xd4,
	0x8f, 0x92, 0x4b, 0x28, 0x64, 0xda, 0x86, 0x4c, 0xc0, 0x02, 0xe6, 0x69, 0x78, 0x94, 0x7c, 0xd2,
	0x95, 0x2e, 0xf4, 0x57, 0x3e, 0xfe, 0x74, 0xc1, 0xa9, 0x6a, 0xc7, 0x31, 0x48, 0x92, 0x8f, 0x1d,
	0x3d, 0x32, 0x16, 0x91, 0x11, 0x44, 0xc2, 0x13, 0x10, 0x81, 0x2f, 0x59, 0x9c, 0x33, 0xdc, 0x47,
	0x18, 0x7a, 0x42, 0x78, 0x12, 0x28, 0xa1, 0x72, 0x3a, 0xdf, 0xfa, 0x5b, 0x42, 0x95, 0xc1, 0xe9,
	0x79, 0x12, 0x01, 0x3e, 0x46, 0xcb, 0x67, 0x2c, 0x96, 0xc2, 0xb6, 0xf6, 0xad, 0x83, 0x5a, 0xaf,
	0x99, 0xa5, 0xce, 0x1a, 0x57, 0xc0, 0x21, 0x1b, 0x87, 0x12, 0xc6, 0x5c, 0x4e, 0xce, 0x1f, 0x02,
	0xf8, 0x04, 0x55, 0xba, 0xbe, 0x72, 0xc4, 0x2e, 0x69, 0xd6, 0x46, 0x96, 0x3a, 0xeb, 0x44, 0x23,
	0x06, 0x6d, 0x06, 0xc1, 0x5d, 0x54, 0x3b, 0x03, 0x88, 0x4f, 0x63, 0x96, 0x70, 0xbb, 0xac, 0xa9,
	0x76, 0x96, 0x3a, 0x1b, 0x1c, 0x20, 0x6e, 0x07, 0x0a, 0x35, 0xe8, 0x73, 0x51, 0x7c, 0x84, 0x96,
	0xba, 0x9c, 0x0b, 0x7b, 0x69, 0xbf, 0x7c, 0x50, 0xeb, 0xe1, 0x2c, 0x75, 0x1a, 0x84, 0x73, 0x73,
	0xdb, 0x07, 0x35, 0x7e, 0x8e, 0x56, 0xba, 0x9c, 0x5f, 0x08, 0x88, 0xed, 0x65, 0x7d, 0xe4, 0x66,
	0x96, 0x3a, 0x98, 0x70, 0xde, 0x4e, 0x04, 0xc4, 0x06, 0x71, 0x0e, 0x86, 0xdf, 0x20, 0x94, 0x93,
	0x4f, 0x63, 0x6e, 0x57, 0x34, 0x7f, 0x37, 0x4b, 0x1d, 0xbb, 0x98, 0x9d, 0x59, 0x7b, 0x61, 0xa7,
	0xf5, 0xab, 0x84, 0xaa, 0x83, 0x3c, 0xaf, 0xf8, 0x04, 0x59, 0x43, 0xed, 0x79, 0xbd, 0xf3, 0xbf,
	0x4b, 0x78, 0xe8, 0x0e, 0x27, 0x1c, 0xde, 0x81, 0x24, 0xbd, 0xe6, 0x4d, 0xea, 0xfc, 0x77, 0x9b,
	0x3a, 0x56, 0x96, 0x3a, 0x2b, 0x87, 0x21, 0x8d, 0x42, 0x0a, 0xe7, 0xc5, 0x07, 0x7e, 0x85, 0xac,
	0xbe, 0x76, 0xbd, 0xde, 0x59, 0xd3, 0xbc, 0xfe, 0xe8, 0x33, 0xf8, 0x52, 0x33, 0xb7, 0x0d, 0x66,
	0x43, 0x05, 0xca, 0x74, 0xe5, 0x7e, 0x8d, 0xdf, 0xa2, 0xa5, 0x01, 0x07, 0x5f, 0xdf, 0x42, 0xbd,
	0xb3, 0xe9, 0x16, 0x0f, 0xc7, 0x2d, 0x36, 0x54, 0xdd, 0xde, 0xa6, 0x52, 0x54, 0x6a, 0x82, 0x83,
	0x6f, 0xaa, 0xdd, 0xaf, 0xf1, 0x10, 0x55, 0x06, 0x92, 0xc8, 0x44, 0xdd, 0x8b, 0xd2, 0xb3, 0xe7,
	0xe8, 0xe9, 0x7e, 0xcf, 0xce, 0x15, 0xd7, 0x85, 0xae, 0xcd, 0xb8, 0x3c, 0x44, 0x9e, 0xed, 0xfc,
	0xfe, 0xf1, 0x64, 0x0b, 0xd5, 0xbd, 0x6f, 0x7d, 0x77, 0xa8, 0xe3, 0xfb, 0x1d, 0x57, 0x8b, 0x07,
	0xdf, 0xfa, 0x59, 0x42, 0xab, 0xe6, 0xae, 0xf8, 0x3d, 0x5a, 0xed, 0x4a, 0x49, 0xfc, 0x2b, 0x1d,
	0x2f, 0x15, 0x68, 0x95, 0x10, 0xe7, 0x66, 0xea, 0xc7, 0x16, 0xd1, 0xbd, 0xe9, 0xa5, 0x98, 0xc7,
	0x2e, 0x6a, 0xe0, 0x0f, 0x68, 0xe5, 0x35, 0x55, 0x6f, 0x44, 0xd8, 0xa5, 0xfd, 0xf2, 0x41, 0xbd,
	0xb3, 0x6e, 0xfc, 0x28, 0xfd, 0x78, 0xa6, 0x86, 0xab, 0x34, 0x85, 0xb4, 0x1d, 0xab, 0x49, 0x33,
	0x4d, 0xb3, 0x18, 0x1e, 0xa2, 0x6a, 0x3f, 0x91, 0x53, 0xcd, 0xf2, 0x02, 0xcd, 0x9d, 0x5c, 0xb3,
	0xc9, 0x12, 0x39, 0x23, 0x3a, 0x0f, 0x6c, 0x5d, 0xa0, 0xc6, 0x7d, 0x93, 0xf1, 0x0b, 0x54, 0xfb,
	0xc8, 0xe2, 0x2f, 0x11, 0x23, 0x97, 0x85, 0x0f, 0x3b, 0xb9, 0x0f, 0xcd, 0xeb, 0xa2, 0x61, 0xca,
	0xce, 0x01, 0x7b, 0xab, 0x37, 0x77, 0x7b, 0xd6, 0xed, 0xdd, 0x9e, 0xf5, 0xe7, 0x6e, 0xcf, 0x3a,
	0xb3, 0x46, 0x15, 0xfd, 0xdf, 0x71, 0xfc, 0x2f, 0x00, 0x00, 0xff, 0xff, 0x1c, 0xe4, 0xf5, 0x48,
	0x74, 0x05, 0x00, 0x00,
}
