// Code generated by protoc-gen-gogo. DO NOT EDIT.
// source: networkencryption.proto

package security

import proto "github.com/gogo/protobuf/proto"
import fmt "fmt"
import math "math"
import _ "github.com/pensando/grpc-gateway/third_party/googleapis/google/api"
import _ "github.com/pensando/sw/venice/utils/apigen/annotations"
import _ "github.com/gogo/protobuf/gogoproto"
import api "github.com/pensando/sw/api"

import io "io"

// Reference imports to suppress errors if they are not otherwise used.
var _ = proto.Marshal
var _ = fmt.Errorf
var _ = math.Inf

//
type IPsecProtocolSpec struct {
	// ESP encryption algorithm. Default is "aes-256-gcm-128" (See RFC4106)
	EncryptionTransform string `protobuf:"bytes,1,opt,name=EncryptionTransform,json=encryption-transform,omitempty,proto3" json:"encryption-transform,omitempty"`
	// ESP integrity algorithm.
	// Default is "NULL" (must be "NULL" if AES-GCM is used for encryption)
	IntegrityTransform string `protobuf:"bytes,2,opt,name=IntegrityTransform,json=integrity-transform,omitempty,proto3" json:"integrity-transform,omitempty"`
}

func (m *IPsecProtocolSpec) Reset()         { *m = IPsecProtocolSpec{} }
func (m *IPsecProtocolSpec) String() string { return proto.CompactTextString(m) }
func (*IPsecProtocolSpec) ProtoMessage()    {}
func (*IPsecProtocolSpec) Descriptor() ([]byte, []int) {
	return fileDescriptorNetworkencryption, []int{0}
}

func (m *IPsecProtocolSpec) GetEncryptionTransform() string {
	if m != nil {
		return m.EncryptionTransform
	}
	return ""
}

func (m *IPsecProtocolSpec) GetIntegrityTransform() string {
	if m != nil {
		return m.IntegrityTransform
	}
	return ""
}

//
type TLSProtocolSpec struct {
	// TLS version: only supported value at present is 1.2
	Version string `protobuf:"bytes,1,opt,name=Version,json=version,omitempty,proto3" json:"version,omitempty"`
	// The name of the cipher suite in IANA format
	// default is TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384
	CipherSuite string `protobuf:"bytes,2,opt,name=CipherSuite,json=cipher-suite,omitempty,proto3" json:"cipher-suite,omitempty"`
}

func (m *TLSProtocolSpec) Reset()                    { *m = TLSProtocolSpec{} }
func (m *TLSProtocolSpec) String() string            { return proto.CompactTextString(m) }
func (*TLSProtocolSpec) ProtoMessage()               {}
func (*TLSProtocolSpec) Descriptor() ([]byte, []int) { return fileDescriptorNetworkencryption, []int{1} }

func (m *TLSProtocolSpec) GetVersion() string {
	if m != nil {
		return m.Version
	}
	return ""
}

func (m *TLSProtocolSpec) GetCipherSuite() string {
	if m != nil {
		return m.CipherSuite
	}
	return ""
}

//
type TrafficEncryptionPolicy struct {
	//
	api.TypeMeta `protobuf:"bytes,1,opt,name=T,json=,inline,embedded=T" json:",inline"`
	//
	api.ObjectMeta `protobuf:"bytes,2,opt,name=O,json=meta,omitempty,embedded=O" json:"meta,omitempty"`
	// Spec contains the configuration of the encryption policy.
	Spec TrafficEncryptionPolicySpec `protobuf:"bytes,3,opt,name=Spec,json=spec,omitempty" json:"spec,omitempty"`
	// Status contains the current state of the encryption policy.
	Status TrafficEncryptionPolicyStatus `protobuf:"bytes,4,opt,name=Status,json=status,omitempty" json:"status,omitempty"`
}

func (m *TrafficEncryptionPolicy) Reset()         { *m = TrafficEncryptionPolicy{} }
func (m *TrafficEncryptionPolicy) String() string { return proto.CompactTextString(m) }
func (*TrafficEncryptionPolicy) ProtoMessage()    {}
func (*TrafficEncryptionPolicy) Descriptor() ([]byte, []int) {
	return fileDescriptorNetworkencryption, []int{2}
}

func (m *TrafficEncryptionPolicy) GetSpec() TrafficEncryptionPolicySpec {
	if m != nil {
		return m.Spec
	}
	return TrafficEncryptionPolicySpec{}
}

func (m *TrafficEncryptionPolicy) GetStatus() TrafficEncryptionPolicyStatus {
	if m != nil {
		return m.Status
	}
	return TrafficEncryptionPolicyStatus{}
}

//
type TrafficEncryptionPolicySpec struct {
	// Possible values: TLS, IPsec
	Mode string `protobuf:"bytes,1,opt,name=Mode,json=mode,omitempty,proto3" json:"mode,omitempty"`
	// TLS Parameters for workload-to-workload connections
	Tls TLSProtocolSpec `protobuf:"bytes,2,opt,name=Tls,json=tls,omitempty" json:"tls,omitempty"`
	// IPsec Parameters for node-to-node connections
	IPsec IPsecProtocolSpec `protobuf:"bytes,3,opt,name=IPsec,json=ipsec,omitempty" json:"ipsec,omitempty"`
	// How often the keys should be rotated, in seconds
	KeyRotationIntervalSecs uint32 `protobuf:"varint,4,opt,name=KeyRotationIntervalSecs,json=key-rotation-interval-secs,omitempty,proto3" json:"key-rotation-interval-secs,omitempty"`
}

func (m *TrafficEncryptionPolicySpec) Reset()         { *m = TrafficEncryptionPolicySpec{} }
func (m *TrafficEncryptionPolicySpec) String() string { return proto.CompactTextString(m) }
func (*TrafficEncryptionPolicySpec) ProtoMessage()    {}
func (*TrafficEncryptionPolicySpec) Descriptor() ([]byte, []int) {
	return fileDescriptorNetworkencryption, []int{3}
}

func (m *TrafficEncryptionPolicySpec) GetMode() string {
	if m != nil {
		return m.Mode
	}
	return ""
}

func (m *TrafficEncryptionPolicySpec) GetTls() TLSProtocolSpec {
	if m != nil {
		return m.Tls
	}
	return TLSProtocolSpec{}
}

func (m *TrafficEncryptionPolicySpec) GetIPsec() IPsecProtocolSpec {
	if m != nil {
		return m.IPsec
	}
	return IPsecProtocolSpec{}
}

func (m *TrafficEncryptionPolicySpec) GetKeyRotationIntervalSecs() uint32 {
	if m != nil {
		return m.KeyRotationIntervalSecs
	}
	return 0
}

//
type TrafficEncryptionPolicyStatus struct {
}

func (m *TrafficEncryptionPolicyStatus) Reset()         { *m = TrafficEncryptionPolicyStatus{} }
func (m *TrafficEncryptionPolicyStatus) String() string { return proto.CompactTextString(m) }
func (*TrafficEncryptionPolicyStatus) ProtoMessage()    {}
func (*TrafficEncryptionPolicyStatus) Descriptor() ([]byte, []int) {
	return fileDescriptorNetworkencryption, []int{4}
}

func init() {
	proto.RegisterType((*IPsecProtocolSpec)(nil), "security.IPsecProtocolSpec")
	proto.RegisterType((*TLSProtocolSpec)(nil), "security.TLSProtocolSpec")
	proto.RegisterType((*TrafficEncryptionPolicy)(nil), "security.TrafficEncryptionPolicy")
	proto.RegisterType((*TrafficEncryptionPolicySpec)(nil), "security.TrafficEncryptionPolicySpec")
	proto.RegisterType((*TrafficEncryptionPolicyStatus)(nil), "security.TrafficEncryptionPolicyStatus")
}
func (m *IPsecProtocolSpec) Marshal() (dAtA []byte, err error) {
	size := m.Size()
	dAtA = make([]byte, size)
	n, err := m.MarshalTo(dAtA)
	if err != nil {
		return nil, err
	}
	return dAtA[:n], nil
}

func (m *IPsecProtocolSpec) MarshalTo(dAtA []byte) (int, error) {
	var i int
	_ = i
	var l int
	_ = l
	if len(m.EncryptionTransform) > 0 {
		dAtA[i] = 0xa
		i++
		i = encodeVarintNetworkencryption(dAtA, i, uint64(len(m.EncryptionTransform)))
		i += copy(dAtA[i:], m.EncryptionTransform)
	}
	if len(m.IntegrityTransform) > 0 {
		dAtA[i] = 0x12
		i++
		i = encodeVarintNetworkencryption(dAtA, i, uint64(len(m.IntegrityTransform)))
		i += copy(dAtA[i:], m.IntegrityTransform)
	}
	return i, nil
}

func (m *TLSProtocolSpec) Marshal() (dAtA []byte, err error) {
	size := m.Size()
	dAtA = make([]byte, size)
	n, err := m.MarshalTo(dAtA)
	if err != nil {
		return nil, err
	}
	return dAtA[:n], nil
}

func (m *TLSProtocolSpec) MarshalTo(dAtA []byte) (int, error) {
	var i int
	_ = i
	var l int
	_ = l
	if len(m.Version) > 0 {
		dAtA[i] = 0xa
		i++
		i = encodeVarintNetworkencryption(dAtA, i, uint64(len(m.Version)))
		i += copy(dAtA[i:], m.Version)
	}
	if len(m.CipherSuite) > 0 {
		dAtA[i] = 0x12
		i++
		i = encodeVarintNetworkencryption(dAtA, i, uint64(len(m.CipherSuite)))
		i += copy(dAtA[i:], m.CipherSuite)
	}
	return i, nil
}

func (m *TrafficEncryptionPolicy) Marshal() (dAtA []byte, err error) {
	size := m.Size()
	dAtA = make([]byte, size)
	n, err := m.MarshalTo(dAtA)
	if err != nil {
		return nil, err
	}
	return dAtA[:n], nil
}

func (m *TrafficEncryptionPolicy) MarshalTo(dAtA []byte) (int, error) {
	var i int
	_ = i
	var l int
	_ = l
	dAtA[i] = 0xa
	i++
	i = encodeVarintNetworkencryption(dAtA, i, uint64(m.TypeMeta.Size()))
	n1, err := m.TypeMeta.MarshalTo(dAtA[i:])
	if err != nil {
		return 0, err
	}
	i += n1
	dAtA[i] = 0x12
	i++
	i = encodeVarintNetworkencryption(dAtA, i, uint64(m.ObjectMeta.Size()))
	n2, err := m.ObjectMeta.MarshalTo(dAtA[i:])
	if err != nil {
		return 0, err
	}
	i += n2
	dAtA[i] = 0x1a
	i++
	i = encodeVarintNetworkencryption(dAtA, i, uint64(m.Spec.Size()))
	n3, err := m.Spec.MarshalTo(dAtA[i:])
	if err != nil {
		return 0, err
	}
	i += n3
	dAtA[i] = 0x22
	i++
	i = encodeVarintNetworkencryption(dAtA, i, uint64(m.Status.Size()))
	n4, err := m.Status.MarshalTo(dAtA[i:])
	if err != nil {
		return 0, err
	}
	i += n4
	return i, nil
}

func (m *TrafficEncryptionPolicySpec) Marshal() (dAtA []byte, err error) {
	size := m.Size()
	dAtA = make([]byte, size)
	n, err := m.MarshalTo(dAtA)
	if err != nil {
		return nil, err
	}
	return dAtA[:n], nil
}

func (m *TrafficEncryptionPolicySpec) MarshalTo(dAtA []byte) (int, error) {
	var i int
	_ = i
	var l int
	_ = l
	if len(m.Mode) > 0 {
		dAtA[i] = 0xa
		i++
		i = encodeVarintNetworkencryption(dAtA, i, uint64(len(m.Mode)))
		i += copy(dAtA[i:], m.Mode)
	}
	dAtA[i] = 0x12
	i++
	i = encodeVarintNetworkencryption(dAtA, i, uint64(m.Tls.Size()))
	n5, err := m.Tls.MarshalTo(dAtA[i:])
	if err != nil {
		return 0, err
	}
	i += n5
	dAtA[i] = 0x1a
	i++
	i = encodeVarintNetworkencryption(dAtA, i, uint64(m.IPsec.Size()))
	n6, err := m.IPsec.MarshalTo(dAtA[i:])
	if err != nil {
		return 0, err
	}
	i += n6
	if m.KeyRotationIntervalSecs != 0 {
		dAtA[i] = 0x20
		i++
		i = encodeVarintNetworkencryption(dAtA, i, uint64(m.KeyRotationIntervalSecs))
	}
	return i, nil
}

func (m *TrafficEncryptionPolicyStatus) Marshal() (dAtA []byte, err error) {
	size := m.Size()
	dAtA = make([]byte, size)
	n, err := m.MarshalTo(dAtA)
	if err != nil {
		return nil, err
	}
	return dAtA[:n], nil
}

func (m *TrafficEncryptionPolicyStatus) MarshalTo(dAtA []byte) (int, error) {
	var i int
	_ = i
	var l int
	_ = l
	return i, nil
}

func encodeVarintNetworkencryption(dAtA []byte, offset int, v uint64) int {
	for v >= 1<<7 {
		dAtA[offset] = uint8(v&0x7f | 0x80)
		v >>= 7
		offset++
	}
	dAtA[offset] = uint8(v)
	return offset + 1
}
func (m *IPsecProtocolSpec) Size() (n int) {
	var l int
	_ = l
	l = len(m.EncryptionTransform)
	if l > 0 {
		n += 1 + l + sovNetworkencryption(uint64(l))
	}
	l = len(m.IntegrityTransform)
	if l > 0 {
		n += 1 + l + sovNetworkencryption(uint64(l))
	}
	return n
}

func (m *TLSProtocolSpec) Size() (n int) {
	var l int
	_ = l
	l = len(m.Version)
	if l > 0 {
		n += 1 + l + sovNetworkencryption(uint64(l))
	}
	l = len(m.CipherSuite)
	if l > 0 {
		n += 1 + l + sovNetworkencryption(uint64(l))
	}
	return n
}

func (m *TrafficEncryptionPolicy) Size() (n int) {
	var l int
	_ = l
	l = m.TypeMeta.Size()
	n += 1 + l + sovNetworkencryption(uint64(l))
	l = m.ObjectMeta.Size()
	n += 1 + l + sovNetworkencryption(uint64(l))
	l = m.Spec.Size()
	n += 1 + l + sovNetworkencryption(uint64(l))
	l = m.Status.Size()
	n += 1 + l + sovNetworkencryption(uint64(l))
	return n
}

func (m *TrafficEncryptionPolicySpec) Size() (n int) {
	var l int
	_ = l
	l = len(m.Mode)
	if l > 0 {
		n += 1 + l + sovNetworkencryption(uint64(l))
	}
	l = m.Tls.Size()
	n += 1 + l + sovNetworkencryption(uint64(l))
	l = m.IPsec.Size()
	n += 1 + l + sovNetworkencryption(uint64(l))
	if m.KeyRotationIntervalSecs != 0 {
		n += 1 + sovNetworkencryption(uint64(m.KeyRotationIntervalSecs))
	}
	return n
}

func (m *TrafficEncryptionPolicyStatus) Size() (n int) {
	var l int
	_ = l
	return n
}

func sovNetworkencryption(x uint64) (n int) {
	for {
		n++
		x >>= 7
		if x == 0 {
			break
		}
	}
	return n
}
func sozNetworkencryption(x uint64) (n int) {
	return sovNetworkencryption(uint64((x << 1) ^ uint64((int64(x) >> 63))))
}
func (m *IPsecProtocolSpec) Unmarshal(dAtA []byte) error {
	l := len(dAtA)
	iNdEx := 0
	for iNdEx < l {
		preIndex := iNdEx
		var wire uint64
		for shift := uint(0); ; shift += 7 {
			if shift >= 64 {
				return ErrIntOverflowNetworkencryption
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
			return fmt.Errorf("proto: IPsecProtocolSpec: wiretype end group for non-group")
		}
		if fieldNum <= 0 {
			return fmt.Errorf("proto: IPsecProtocolSpec: illegal tag %d (wire type %d)", fieldNum, wire)
		}
		switch fieldNum {
		case 1:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field EncryptionTransform", wireType)
			}
			var stringLen uint64
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowNetworkencryption
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
				return ErrInvalidLengthNetworkencryption
			}
			postIndex := iNdEx + intStringLen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			m.EncryptionTransform = string(dAtA[iNdEx:postIndex])
			iNdEx = postIndex
		case 2:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field IntegrityTransform", wireType)
			}
			var stringLen uint64
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowNetworkencryption
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
				return ErrInvalidLengthNetworkencryption
			}
			postIndex := iNdEx + intStringLen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			m.IntegrityTransform = string(dAtA[iNdEx:postIndex])
			iNdEx = postIndex
		default:
			iNdEx = preIndex
			skippy, err := skipNetworkencryption(dAtA[iNdEx:])
			if err != nil {
				return err
			}
			if skippy < 0 {
				return ErrInvalidLengthNetworkencryption
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
func (m *TLSProtocolSpec) Unmarshal(dAtA []byte) error {
	l := len(dAtA)
	iNdEx := 0
	for iNdEx < l {
		preIndex := iNdEx
		var wire uint64
		for shift := uint(0); ; shift += 7 {
			if shift >= 64 {
				return ErrIntOverflowNetworkencryption
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
			return fmt.Errorf("proto: TLSProtocolSpec: wiretype end group for non-group")
		}
		if fieldNum <= 0 {
			return fmt.Errorf("proto: TLSProtocolSpec: illegal tag %d (wire type %d)", fieldNum, wire)
		}
		switch fieldNum {
		case 1:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field Version", wireType)
			}
			var stringLen uint64
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowNetworkencryption
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
				return ErrInvalidLengthNetworkencryption
			}
			postIndex := iNdEx + intStringLen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			m.Version = string(dAtA[iNdEx:postIndex])
			iNdEx = postIndex
		case 2:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field CipherSuite", wireType)
			}
			var stringLen uint64
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowNetworkencryption
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
				return ErrInvalidLengthNetworkencryption
			}
			postIndex := iNdEx + intStringLen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			m.CipherSuite = string(dAtA[iNdEx:postIndex])
			iNdEx = postIndex
		default:
			iNdEx = preIndex
			skippy, err := skipNetworkencryption(dAtA[iNdEx:])
			if err != nil {
				return err
			}
			if skippy < 0 {
				return ErrInvalidLengthNetworkencryption
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
func (m *TrafficEncryptionPolicy) Unmarshal(dAtA []byte) error {
	l := len(dAtA)
	iNdEx := 0
	for iNdEx < l {
		preIndex := iNdEx
		var wire uint64
		for shift := uint(0); ; shift += 7 {
			if shift >= 64 {
				return ErrIntOverflowNetworkencryption
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
			return fmt.Errorf("proto: TrafficEncryptionPolicy: wiretype end group for non-group")
		}
		if fieldNum <= 0 {
			return fmt.Errorf("proto: TrafficEncryptionPolicy: illegal tag %d (wire type %d)", fieldNum, wire)
		}
		switch fieldNum {
		case 1:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field TypeMeta", wireType)
			}
			var msglen int
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowNetworkencryption
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
				return ErrInvalidLengthNetworkencryption
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
					return ErrIntOverflowNetworkencryption
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
				return ErrInvalidLengthNetworkencryption
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
					return ErrIntOverflowNetworkencryption
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
				return ErrInvalidLengthNetworkencryption
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
					return ErrIntOverflowNetworkencryption
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
				return ErrInvalidLengthNetworkencryption
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
			skippy, err := skipNetworkencryption(dAtA[iNdEx:])
			if err != nil {
				return err
			}
			if skippy < 0 {
				return ErrInvalidLengthNetworkencryption
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
func (m *TrafficEncryptionPolicySpec) Unmarshal(dAtA []byte) error {
	l := len(dAtA)
	iNdEx := 0
	for iNdEx < l {
		preIndex := iNdEx
		var wire uint64
		for shift := uint(0); ; shift += 7 {
			if shift >= 64 {
				return ErrIntOverflowNetworkencryption
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
			return fmt.Errorf("proto: TrafficEncryptionPolicySpec: wiretype end group for non-group")
		}
		if fieldNum <= 0 {
			return fmt.Errorf("proto: TrafficEncryptionPolicySpec: illegal tag %d (wire type %d)", fieldNum, wire)
		}
		switch fieldNum {
		case 1:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field Mode", wireType)
			}
			var stringLen uint64
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowNetworkencryption
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
				return ErrInvalidLengthNetworkencryption
			}
			postIndex := iNdEx + intStringLen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			m.Mode = string(dAtA[iNdEx:postIndex])
			iNdEx = postIndex
		case 2:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field Tls", wireType)
			}
			var msglen int
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowNetworkencryption
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
				return ErrInvalidLengthNetworkencryption
			}
			postIndex := iNdEx + msglen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			if err := m.Tls.Unmarshal(dAtA[iNdEx:postIndex]); err != nil {
				return err
			}
			iNdEx = postIndex
		case 3:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field IPsec", wireType)
			}
			var msglen int
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowNetworkencryption
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
				return ErrInvalidLengthNetworkencryption
			}
			postIndex := iNdEx + msglen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			if err := m.IPsec.Unmarshal(dAtA[iNdEx:postIndex]); err != nil {
				return err
			}
			iNdEx = postIndex
		case 4:
			if wireType != 0 {
				return fmt.Errorf("proto: wrong wireType = %d for field KeyRotationIntervalSecs", wireType)
			}
			m.KeyRotationIntervalSecs = 0
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowNetworkencryption
				}
				if iNdEx >= l {
					return io.ErrUnexpectedEOF
				}
				b := dAtA[iNdEx]
				iNdEx++
				m.KeyRotationIntervalSecs |= (uint32(b) & 0x7F) << shift
				if b < 0x80 {
					break
				}
			}
		default:
			iNdEx = preIndex
			skippy, err := skipNetworkencryption(dAtA[iNdEx:])
			if err != nil {
				return err
			}
			if skippy < 0 {
				return ErrInvalidLengthNetworkencryption
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
func (m *TrafficEncryptionPolicyStatus) Unmarshal(dAtA []byte) error {
	l := len(dAtA)
	iNdEx := 0
	for iNdEx < l {
		preIndex := iNdEx
		var wire uint64
		for shift := uint(0); ; shift += 7 {
			if shift >= 64 {
				return ErrIntOverflowNetworkencryption
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
			return fmt.Errorf("proto: TrafficEncryptionPolicyStatus: wiretype end group for non-group")
		}
		if fieldNum <= 0 {
			return fmt.Errorf("proto: TrafficEncryptionPolicyStatus: illegal tag %d (wire type %d)", fieldNum, wire)
		}
		switch fieldNum {
		default:
			iNdEx = preIndex
			skippy, err := skipNetworkencryption(dAtA[iNdEx:])
			if err != nil {
				return err
			}
			if skippy < 0 {
				return ErrInvalidLengthNetworkencryption
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
func skipNetworkencryption(dAtA []byte) (n int, err error) {
	l := len(dAtA)
	iNdEx := 0
	for iNdEx < l {
		var wire uint64
		for shift := uint(0); ; shift += 7 {
			if shift >= 64 {
				return 0, ErrIntOverflowNetworkencryption
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
					return 0, ErrIntOverflowNetworkencryption
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
					return 0, ErrIntOverflowNetworkencryption
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
				return 0, ErrInvalidLengthNetworkencryption
			}
			return iNdEx, nil
		case 3:
			for {
				var innerWire uint64
				var start int = iNdEx
				for shift := uint(0); ; shift += 7 {
					if shift >= 64 {
						return 0, ErrIntOverflowNetworkencryption
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
				next, err := skipNetworkencryption(dAtA[start:])
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
	ErrInvalidLengthNetworkencryption = fmt.Errorf("proto: negative length found during unmarshaling")
	ErrIntOverflowNetworkencryption   = fmt.Errorf("proto: integer overflow")
)

func init() { proto.RegisterFile("networkencryption.proto", fileDescriptorNetworkencryption) }

var fileDescriptorNetworkencryption = []byte{
	// 635 bytes of a gzipped FileDescriptorProto
	0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xff, 0x8c, 0x54, 0xdd, 0x4e, 0xd4, 0x40,
	0x18, 0xb5, 0x80, 0xa0, 0xb3, 0x22, 0x32, 0x04, 0x76, 0x59, 0x64, 0xab, 0x1b, 0x51, 0x62, 0xd8,
	0xd6, 0x60, 0x62, 0xa2, 0x97, 0x35, 0x68, 0x88, 0x10, 0x36, 0x6c, 0xe3, 0x8d, 0x26, 0xa6, 0x3b,
	0xfb, 0x6d, 0x19, 0xe9, 0xce, 0x34, 0x9d, 0x29, 0xa4, 0x31, 0x5e, 0x1a, 0x9f, 0xc0, 0x77, 0xe2,
	0x92, 0xf0, 0x00, 0x8d, 0x21, 0x5e, 0xed, 0x53, 0x98, 0xce, 0xce, 0xba, 0x2d, 0x3f, 0x8b, 0x77,
	0x3d, 0x67, 0xce, 0x77, 0xbe, 0xbf, 0x99, 0xa2, 0x32, 0x03, 0x79, 0xcc, 0xa3, 0x43, 0x60, 0x24,
	0x4a, 0x42, 0x49, 0x39, 0xb3, 0xc2, 0x88, 0x4b, 0x8e, 0xef, 0x08, 0x20, 0x71, 0x44, 0x65, 0x52,
	0x7d, 0xe8, 0x73, 0xee, 0x07, 0x60, 0x7b, 0x21, 0xb5, 0x3d, 0xc6, 0xb8, 0xf4, 0x32, 0x99, 0x18,
	0xe8, 0xaa, 0x5b, 0x3e, 0x95, 0x07, 0x71, 0xdb, 0x22, 0xbc, 0x67, 0x87, 0xc0, 0x84, 0xc7, 0x3a,
	0xdc, 0x16, 0xc7, 0xf6, 0x11, 0x30, 0x4a, 0xc0, 0x8e, 0x25, 0x0d, 0x44, 0x16, 0xea, 0x03, 0xcb,
	0x47, 0xdb, 0x94, 0x91, 0x20, 0xee, 0xc0, 0xd0, 0xa6, 0x91, 0xb3, 0xf1, 0xb9, 0xcf, 0x6d, 0x45,
	0xb7, 0xe3, 0xae, 0x42, 0x0a, 0xa8, 0x2f, 0x2d, 0x5f, 0xbb, 0x26, 0x6b, 0x56, 0x63, 0x0f, 0xa4,
	0x37, 0x90, 0xd5, 0xcf, 0x0c, 0x34, 0xbf, 0xdd, 0x14, 0x40, 0x9a, 0x19, 0x24, 0x3c, 0x68, 0x85,
	0x40, 0xf0, 0x17, 0xb4, 0xb0, 0xf5, 0xaf, 0x5d, 0x37, 0xf2, 0x98, 0xe8, 0xf2, 0xa8, 0x57, 0x31,
	0x1e, 0x19, 0xeb, 0x77, 0x9d, 0x7a, 0x3f, 0x35, 0x6b, 0xa3, 0x69, 0x34, 0xe4, 0xf0, 0x7c, 0x83,
	0xf7, 0xa8, 0x84, 0x5e, 0x28, 0x93, 0xfd, 0x1b, 0xce, 0xf1, 0x67, 0x84, 0xb7, 0x99, 0x04, 0x3f,
	0x1b, 0xdf, 0xc8, 0x7f, 0x42, 0xf9, 0x3f, 0xee, 0xa7, 0xe6, 0x2a, 0x1d, 0x9e, 0x5e, 0x69, 0x3f,
	0xfe, 0xb8, 0xfe, 0xcb, 0x40, 0x73, 0xee, 0x4e, 0xab, 0xd0, 0xd2, 0x6b, 0x34, 0xf3, 0x11, 0x22,
	0x41, 0x39, 0xd3, 0x6d, 0x2c, 0xf6, 0x53, 0x73, 0xfe, 0x68, 0x40, 0xe5, 0xac, 0x2f, 0x53, 0xf8,
	0x3d, 0x2a, 0xbd, 0xa5, 0xe1, 0x01, 0x44, 0xad, 0x98, 0x4a, 0xd0, 0x55, 0x56, 0xfb, 0xa9, 0xb9,
	0x44, 0x14, 0xdd, 0x10, 0x19, 0x9f, 0xf3, 0xb8, 0x86, 0xaf, 0xff, 0x9c, 0x44, 0x65, 0x37, 0xf2,
	0xba, 0x5d, 0x4a, 0x46, 0xe3, 0x6d, 0xf2, 0x80, 0x92, 0x04, 0xbf, 0x42, 0x86, 0xab, 0x2a, 0x2b,
	0x6d, 0xce, 0x5a, 0x5e, 0x48, 0x2d, 0x37, 0x09, 0x61, 0x17, 0xa4, 0xe7, 0x2c, 0x9c, 0xa4, 0xe6,
	0xad, 0xd3, 0xd4, 0x34, 0xfa, 0xa9, 0x39, 0xb3, 0x41, 0x59, 0x40, 0x19, 0xec, 0x0f, 0x3f, 0xf0,
	0x3b, 0x64, 0xec, 0xa9, 0x92, 0x4a, 0x9b, 0x73, 0x2a, 0x6e, 0xaf, 0xfd, 0x15, 0x88, 0x54, 0x91,
	0xd5, 0x5c, 0xe4, 0xfd, 0x6c, 0xe9, 0xb9, 0x1a, 0x2f, 0x60, 0xfc, 0x09, 0x4d, 0x65, 0x73, 0xaa,
	0x4c, 0x2a, 0xab, 0x35, 0x6b, 0x78, 0xb9, 0xad, 0x6b, 0x0a, 0xce, 0xc4, 0xce, 0x52, 0x96, 0x20,
	0x33, 0x17, 0x21, 0x90, 0xbc, 0x79, 0x11, 0x63, 0x82, 0xa6, 0x5b, 0xd2, 0x93, 0xb1, 0xa8, 0x4c,
	0x29, 0xfb, 0x67, 0x37, 0xdb, 0x2b, 0xb9, 0x53, 0xd1, 0x09, 0x1e, 0x08, 0x85, 0x73, 0x29, 0x2e,
	0x31, 0x6f, 0x9e, 0x9f, 0xfd, 0x58, 0x7e, 0x8a, 0x4a, 0xf6, 0xb7, 0x3d, 0xcb, 0x05, 0xe6, 0x31,
	0xf9, 0x1d, 0x97, 0xe5, 0xd5, 0xee, 0xf5, 0x3f, 0x13, 0x68, 0x65, 0x4c, 0x63, 0xf8, 0x05, 0x9a,
	0xda, 0xe5, 0x1d, 0xd0, 0x57, 0x05, 0xab, 0xf9, 0xf1, 0x0e, 0x14, 0xe6, 0x57, 0xc0, 0x78, 0x07,
	0x4d, 0xba, 0x81, 0xd0, 0x9b, 0x58, 0xce, 0xf5, 0x57, 0xbc, 0x87, 0xce, 0xa2, 0xee, 0x68, 0x56,
	0x06, 0xf9, 0x76, 0x8a, 0x10, 0xbb, 0xe8, 0xb6, 0x7a, 0x95, 0x7a, 0x1d, 0x2b, 0x23, 0xbf, 0x4b,
	0x8f, 0xd5, 0x29, 0x6b, 0xc7, 0x39, 0x1a, 0x8a, 0xc2, 0x16, 0x2e, 0x12, 0x98, 0xa3, 0xf2, 0x07,
	0x48, 0xf6, 0xf5, 0x1f, 0x26, 0x7b, 0x80, 0xd1, 0x91, 0x17, 0xb4, 0x80, 0x0c, 0xf6, 0x32, 0xeb,
	0xac, 0xf7, 0x53, 0xf3, 0xc9, 0x21, 0x24, 0x8d, 0x48, 0x6b, 0x1a, 0x54, 0x8b, 0x1a, 0x02, 0x48,
	0xbe, 0xde, 0xff, 0x52, 0xd5, 0x4d, 0xb4, 0x3a, 0x7e, 0xbf, 0xf7, 0x4e, 0xce, 0x6b, 0xc6, 0xe9,
	0x79, 0xcd, 0xf8, 0x7d, 0x5e, 0x33, 0x9a, 0x46, 0x7b, 0x5a, 0xfd, 0x95, 0x5e, 0xfe, 0x0d, 0x00,
	0x00, 0xff, 0xff, 0x0d, 0x2b, 0x81, 0xe8, 0x75, 0x05, 0x00, 0x00,
}
