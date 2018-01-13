// Code generated by protoc-gen-gogo. DO NOT EDIT.
// source: export.proto

package api

import proto "github.com/gogo/protobuf/proto"
import fmt "fmt"
import math "math"
import _ "github.com/gogo/protobuf/gogoproto"

import io "io"

// Reference imports to suppress errors if they are not otherwise used.
var _ = proto.Marshal
var _ = fmt.Errorf
var _ = math.Inf

// ----------------------------- Export Config -----------------------------
// Export Config specifies server address and user credentials
type ExportConfig struct {
	// IP address or URL of the collector/entity to which the data is to be exported
	Destination string `protobuf:"bytes,1,opt,name=Destination,proto3" json:"destination,omitempty"`
	// protocol and Port number where an external collector is gathering the data
	// example "TCP/2055"
	Transport string `protobuf:"bytes,2,opt,name=Transport,proto3" json:"transport,omitempty"`
	// Credentials provide secure access to the collector
	Credentials *ExternalCred `protobuf:"bytes,3,opt,name=Credentials" json:"credentials,omitempty"`
}

func (m *ExportConfig) Reset()                    { *m = ExportConfig{} }
func (m *ExportConfig) String() string            { return proto.CompactTextString(m) }
func (*ExportConfig) ProtoMessage()               {}
func (*ExportConfig) Descriptor() ([]byte, []int) { return fileDescriptorExport, []int{0} }

func (m *ExportConfig) GetDestination() string {
	if m != nil {
		return m.Destination
	}
	return ""
}

func (m *ExportConfig) GetTransport() string {
	if m != nil {
		return m.Transport
	}
	return ""
}

func (m *ExportConfig) GetCredentials() *ExternalCred {
	if m != nil {
		return m.Credentials
	}
	return nil
}

// ------------------------ ExternalCred Object ----------------------------
// ExternalCred defines credentials required to access an external entity, such as
// a stats collector, compute orchestration entity, or a syslog server.
// External entity may support a variety of methods, like username/password,
// TLS Client authentication, or Bearer Token based authentication. User is
// expected to configure one of the methods
type ExternalCred struct {
	// UserName is the login id to be used towards the external entity
	UserName string `protobuf:"bytes,1,opt,name=UserName,proto3" json:"username,omitempty"`
	// Password is one time specified, not visibile on read operations
	// Only valid when UserName is defined
	// TBD: need to add (venice.secret) = "true" support for this
	Password string `protobuf:"bytes,2,opt,name=Password,proto3" json:"password,omitempty"`
	// External entity supports bearer tokens for authentication and authorization
	// Token refresh is not supported using OAuth2
	// TBD: need to add (venice.secret) = "true" support for this
	BearerToken string `protobuf:"bytes,3,opt,name=BearerToken,proto3" json:"bearer-token,omitempty"`
	// CertData holds PEM-encoded bytes (typically read from a client certificate file).
	CertData []byte `protobuf:"bytes,4,opt,name=CertData,proto3" json:"cert-data,omitempty"`
	// KeyData holds PEM-encoded bytes (typically read from a client certificate key file).
	// TBD: need to add (venice.secret) = "true" support for this
	KeyData []byte `protobuf:"bytes,5,opt,name=KeyData,proto3" json:"key-data,omitempty"`
	// CaData holds PEM-encoded bytes (typically read from a root certificates bundle).
	CaData []byte `protobuf:"bytes,6,opt,name=CaData,proto3" json:"ca-data,omitempty"`
}

func (m *ExternalCred) Reset()                    { *m = ExternalCred{} }
func (m *ExternalCred) String() string            { return proto.CompactTextString(m) }
func (*ExternalCred) ProtoMessage()               {}
func (*ExternalCred) Descriptor() ([]byte, []int) { return fileDescriptorExport, []int{1} }

func (m *ExternalCred) GetUserName() string {
	if m != nil {
		return m.UserName
	}
	return ""
}

func (m *ExternalCred) GetPassword() string {
	if m != nil {
		return m.Password
	}
	return ""
}

func (m *ExternalCred) GetBearerToken() string {
	if m != nil {
		return m.BearerToken
	}
	return ""
}

func (m *ExternalCred) GetCertData() []byte {
	if m != nil {
		return m.CertData
	}
	return nil
}

func (m *ExternalCred) GetKeyData() []byte {
	if m != nil {
		return m.KeyData
	}
	return nil
}

func (m *ExternalCred) GetCaData() []byte {
	if m != nil {
		return m.CaData
	}
	return nil
}

func init() {
	proto.RegisterType((*ExportConfig)(nil), "api.ExportConfig")
	proto.RegisterType((*ExternalCred)(nil), "api.ExternalCred")
}
func (m *ExportConfig) Marshal() (dAtA []byte, err error) {
	size := m.Size()
	dAtA = make([]byte, size)
	n, err := m.MarshalTo(dAtA)
	if err != nil {
		return nil, err
	}
	return dAtA[:n], nil
}

func (m *ExportConfig) MarshalTo(dAtA []byte) (int, error) {
	var i int
	_ = i
	var l int
	_ = l
	if len(m.Destination) > 0 {
		dAtA[i] = 0xa
		i++
		i = encodeVarintExport(dAtA, i, uint64(len(m.Destination)))
		i += copy(dAtA[i:], m.Destination)
	}
	if len(m.Transport) > 0 {
		dAtA[i] = 0x12
		i++
		i = encodeVarintExport(dAtA, i, uint64(len(m.Transport)))
		i += copy(dAtA[i:], m.Transport)
	}
	if m.Credentials != nil {
		dAtA[i] = 0x1a
		i++
		i = encodeVarintExport(dAtA, i, uint64(m.Credentials.Size()))
		n1, err := m.Credentials.MarshalTo(dAtA[i:])
		if err != nil {
			return 0, err
		}
		i += n1
	}
	return i, nil
}

func (m *ExternalCred) Marshal() (dAtA []byte, err error) {
	size := m.Size()
	dAtA = make([]byte, size)
	n, err := m.MarshalTo(dAtA)
	if err != nil {
		return nil, err
	}
	return dAtA[:n], nil
}

func (m *ExternalCred) MarshalTo(dAtA []byte) (int, error) {
	var i int
	_ = i
	var l int
	_ = l
	if len(m.UserName) > 0 {
		dAtA[i] = 0xa
		i++
		i = encodeVarintExport(dAtA, i, uint64(len(m.UserName)))
		i += copy(dAtA[i:], m.UserName)
	}
	if len(m.Password) > 0 {
		dAtA[i] = 0x12
		i++
		i = encodeVarintExport(dAtA, i, uint64(len(m.Password)))
		i += copy(dAtA[i:], m.Password)
	}
	if len(m.BearerToken) > 0 {
		dAtA[i] = 0x1a
		i++
		i = encodeVarintExport(dAtA, i, uint64(len(m.BearerToken)))
		i += copy(dAtA[i:], m.BearerToken)
	}
	if len(m.CertData) > 0 {
		dAtA[i] = 0x22
		i++
		i = encodeVarintExport(dAtA, i, uint64(len(m.CertData)))
		i += copy(dAtA[i:], m.CertData)
	}
	if len(m.KeyData) > 0 {
		dAtA[i] = 0x2a
		i++
		i = encodeVarintExport(dAtA, i, uint64(len(m.KeyData)))
		i += copy(dAtA[i:], m.KeyData)
	}
	if len(m.CaData) > 0 {
		dAtA[i] = 0x32
		i++
		i = encodeVarintExport(dAtA, i, uint64(len(m.CaData)))
		i += copy(dAtA[i:], m.CaData)
	}
	return i, nil
}

func encodeVarintExport(dAtA []byte, offset int, v uint64) int {
	for v >= 1<<7 {
		dAtA[offset] = uint8(v&0x7f | 0x80)
		v >>= 7
		offset++
	}
	dAtA[offset] = uint8(v)
	return offset + 1
}
func (m *ExportConfig) Size() (n int) {
	var l int
	_ = l
	l = len(m.Destination)
	if l > 0 {
		n += 1 + l + sovExport(uint64(l))
	}
	l = len(m.Transport)
	if l > 0 {
		n += 1 + l + sovExport(uint64(l))
	}
	if m.Credentials != nil {
		l = m.Credentials.Size()
		n += 1 + l + sovExport(uint64(l))
	}
	return n
}

func (m *ExternalCred) Size() (n int) {
	var l int
	_ = l
	l = len(m.UserName)
	if l > 0 {
		n += 1 + l + sovExport(uint64(l))
	}
	l = len(m.Password)
	if l > 0 {
		n += 1 + l + sovExport(uint64(l))
	}
	l = len(m.BearerToken)
	if l > 0 {
		n += 1 + l + sovExport(uint64(l))
	}
	l = len(m.CertData)
	if l > 0 {
		n += 1 + l + sovExport(uint64(l))
	}
	l = len(m.KeyData)
	if l > 0 {
		n += 1 + l + sovExport(uint64(l))
	}
	l = len(m.CaData)
	if l > 0 {
		n += 1 + l + sovExport(uint64(l))
	}
	return n
}

func sovExport(x uint64) (n int) {
	for {
		n++
		x >>= 7
		if x == 0 {
			break
		}
	}
	return n
}
func sozExport(x uint64) (n int) {
	return sovExport(uint64((x << 1) ^ uint64((int64(x) >> 63))))
}
func (m *ExportConfig) Unmarshal(dAtA []byte) error {
	l := len(dAtA)
	iNdEx := 0
	for iNdEx < l {
		preIndex := iNdEx
		var wire uint64
		for shift := uint(0); ; shift += 7 {
			if shift >= 64 {
				return ErrIntOverflowExport
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
			return fmt.Errorf("proto: ExportConfig: wiretype end group for non-group")
		}
		if fieldNum <= 0 {
			return fmt.Errorf("proto: ExportConfig: illegal tag %d (wire type %d)", fieldNum, wire)
		}
		switch fieldNum {
		case 1:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field Destination", wireType)
			}
			var stringLen uint64
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowExport
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
				return ErrInvalidLengthExport
			}
			postIndex := iNdEx + intStringLen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			m.Destination = string(dAtA[iNdEx:postIndex])
			iNdEx = postIndex
		case 2:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field Transport", wireType)
			}
			var stringLen uint64
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowExport
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
				return ErrInvalidLengthExport
			}
			postIndex := iNdEx + intStringLen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			m.Transport = string(dAtA[iNdEx:postIndex])
			iNdEx = postIndex
		case 3:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field Credentials", wireType)
			}
			var msglen int
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowExport
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
				return ErrInvalidLengthExport
			}
			postIndex := iNdEx + msglen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			if m.Credentials == nil {
				m.Credentials = &ExternalCred{}
			}
			if err := m.Credentials.Unmarshal(dAtA[iNdEx:postIndex]); err != nil {
				return err
			}
			iNdEx = postIndex
		default:
			iNdEx = preIndex
			skippy, err := skipExport(dAtA[iNdEx:])
			if err != nil {
				return err
			}
			if skippy < 0 {
				return ErrInvalidLengthExport
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
func (m *ExternalCred) Unmarshal(dAtA []byte) error {
	l := len(dAtA)
	iNdEx := 0
	for iNdEx < l {
		preIndex := iNdEx
		var wire uint64
		for shift := uint(0); ; shift += 7 {
			if shift >= 64 {
				return ErrIntOverflowExport
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
			return fmt.Errorf("proto: ExternalCred: wiretype end group for non-group")
		}
		if fieldNum <= 0 {
			return fmt.Errorf("proto: ExternalCred: illegal tag %d (wire type %d)", fieldNum, wire)
		}
		switch fieldNum {
		case 1:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field UserName", wireType)
			}
			var stringLen uint64
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowExport
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
				return ErrInvalidLengthExport
			}
			postIndex := iNdEx + intStringLen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			m.UserName = string(dAtA[iNdEx:postIndex])
			iNdEx = postIndex
		case 2:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field Password", wireType)
			}
			var stringLen uint64
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowExport
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
				return ErrInvalidLengthExport
			}
			postIndex := iNdEx + intStringLen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			m.Password = string(dAtA[iNdEx:postIndex])
			iNdEx = postIndex
		case 3:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field BearerToken", wireType)
			}
			var stringLen uint64
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowExport
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
				return ErrInvalidLengthExport
			}
			postIndex := iNdEx + intStringLen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			m.BearerToken = string(dAtA[iNdEx:postIndex])
			iNdEx = postIndex
		case 4:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field CertData", wireType)
			}
			var byteLen int
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowExport
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
				return ErrInvalidLengthExport
			}
			postIndex := iNdEx + byteLen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			m.CertData = append(m.CertData[:0], dAtA[iNdEx:postIndex]...)
			if m.CertData == nil {
				m.CertData = []byte{}
			}
			iNdEx = postIndex
		case 5:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field KeyData", wireType)
			}
			var byteLen int
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowExport
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
				return ErrInvalidLengthExport
			}
			postIndex := iNdEx + byteLen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			m.KeyData = append(m.KeyData[:0], dAtA[iNdEx:postIndex]...)
			if m.KeyData == nil {
				m.KeyData = []byte{}
			}
			iNdEx = postIndex
		case 6:
			if wireType != 2 {
				return fmt.Errorf("proto: wrong wireType = %d for field CaData", wireType)
			}
			var byteLen int
			for shift := uint(0); ; shift += 7 {
				if shift >= 64 {
					return ErrIntOverflowExport
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
				return ErrInvalidLengthExport
			}
			postIndex := iNdEx + byteLen
			if postIndex > l {
				return io.ErrUnexpectedEOF
			}
			m.CaData = append(m.CaData[:0], dAtA[iNdEx:postIndex]...)
			if m.CaData == nil {
				m.CaData = []byte{}
			}
			iNdEx = postIndex
		default:
			iNdEx = preIndex
			skippy, err := skipExport(dAtA[iNdEx:])
			if err != nil {
				return err
			}
			if skippy < 0 {
				return ErrInvalidLengthExport
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
func skipExport(dAtA []byte) (n int, err error) {
	l := len(dAtA)
	iNdEx := 0
	for iNdEx < l {
		var wire uint64
		for shift := uint(0); ; shift += 7 {
			if shift >= 64 {
				return 0, ErrIntOverflowExport
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
					return 0, ErrIntOverflowExport
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
					return 0, ErrIntOverflowExport
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
				return 0, ErrInvalidLengthExport
			}
			return iNdEx, nil
		case 3:
			for {
				var innerWire uint64
				var start int = iNdEx
				for shift := uint(0); ; shift += 7 {
					if shift >= 64 {
						return 0, ErrIntOverflowExport
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
				next, err := skipExport(dAtA[start:])
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
	ErrInvalidLengthExport = fmt.Errorf("proto: negative length found during unmarshaling")
	ErrIntOverflowExport   = fmt.Errorf("proto: integer overflow")
)

func init() { proto.RegisterFile("export.proto", fileDescriptorExport) }

var fileDescriptorExport = []byte{
	// 379 bytes of a gzipped FileDescriptorProto
	0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xff, 0x5c, 0x92, 0xc1, 0x0e, 0xd2, 0x30,
	0x18, 0xc7, 0x2d, 0x28, 0x42, 0xc7, 0x41, 0x6a, 0xc0, 0xc9, 0x61, 0x23, 0x9c, 0x38, 0xb8, 0x61,
	0x20, 0x9e, 0xf4, 0xb4, 0xc1, 0xc9, 0x84, 0x18, 0x83, 0x0f, 0xd0, 0x6d, 0x1f, 0xb3, 0x81, 0xad,
	0x4b, 0x57, 0x22, 0xbc, 0x89, 0x8f, 0x64, 0x3c, 0x19, 0x1f, 0x60, 0x31, 0x78, 0xdb, 0x53, 0x98,
	0x95, 0xcd, 0x35, 0xdc, 0xd6, 0x7f, 0x7f, 0xbf, 0x6f, 0xdf, 0x7f, 0x19, 0x1e, 0xc2, 0x25, 0xe3,
	0x42, 0xba, 0x99, 0xe0, 0x92, 0x93, 0x2e, 0xcd, 0xd8, 0xd4, 0x89, 0x99, 0xfc, 0x7a, 0x0e, 0xdc,
	0x90, 0x27, 0xcb, 0x98, 0xc7, 0x7c, 0xa9, 0xee, 0x82, 0xf3, 0x41, 0x9d, 0xd4, 0x41, 0x3d, 0xdd,
	0x9d, 0xf9, 0x6f, 0x84, 0x87, 0x5b, 0x35, 0xc4, 0xe7, 0xe9, 0x81, 0xc5, 0xe4, 0x3d, 0x36, 0x36,
	0x90, 0x4b, 0x96, 0x52, 0xc9, 0x78, 0x6a, 0xa2, 0x19, 0x5a, 0x0c, 0xbc, 0xd7, 0x65, 0x61, 0x8f,
	0xa3, 0x36, 0x7e, 0xc3, 0x13, 0x26, 0x21, 0xc9, 0xe4, 0xf5, 0xb3, 0x4e, 0x93, 0x77, 0x78, 0xb0,
	0x17, 0x34, 0xcd, 0xab, 0x79, 0x66, 0x47, 0xa9, 0xaf, 0xca, 0xc2, 0x7e, 0x29, 0x9b, 0x50, 0x13,
	0x5b, 0x92, 0xec, 0xb0, 0xe1, 0x0b, 0x88, 0x20, 0x95, 0x8c, 0x9e, 0x72, 0xb3, 0x3b, 0x43, 0x0b,
	0x63, 0x35, 0x72, 0x69, 0xc6, 0xdc, 0xed, 0x45, 0x82, 0x48, 0xe9, 0xa9, 0xba, 0xbf, 0xaf, 0x11,
	0xb6, 0xa4, 0xbe, 0x86, 0x36, 0x60, 0xfe, 0xb3, 0x53, 0x95, 0x6a, 0x45, 0xb2, 0xc2, 0xfd, 0x2f,
	0x39, 0x88, 0x1d, 0x4d, 0xa0, 0x6e, 0x34, 0x29, 0x0b, 0x9b, 0x9c, 0xf3, 0x0a, 0x49, 0x40, 0x9b,
	0xf3, 0x9f, 0xab, 0x9c, 0x4f, 0x34, 0xcf, 0xbf, 0x71, 0x11, 0xd5, 0x55, 0x94, 0x93, 0xd5, 0x99,
	0xee, 0x34, 0x1c, 0xf9, 0x80, 0x0d, 0x0f, 0xa8, 0x00, 0xb1, 0xe7, 0x47, 0x48, 0x55, 0x91, 0x81,
	0x37, 0x2d, 0x0b, 0x7b, 0x12, 0xa8, 0xd8, 0x91, 0x55, 0xae, 0xaf, 0xad, 0xe1, 0x64, 0x8d, 0xfb,
	0x3e, 0x08, 0xb9, 0xa1, 0x92, 0x9a, 0x4f, 0x67, 0x68, 0x31, 0xbc, 0x7f, 0xbc, 0x10, 0x84, 0x74,
	0x22, 0x2a, 0xa9, 0xfe, 0xca, 0x06, 0x24, 0x6f, 0xf1, 0xf3, 0x8f, 0x70, 0x55, 0xce, 0x33, 0xe5,
	0xa8, 0x2d, 0x8f, 0x70, 0x7d, 0x54, 0x1a, 0x8c, 0x38, 0xb8, 0xe7, 0x53, 0x25, 0xf4, 0x94, 0x30,
	0x2e, 0x0b, 0x7b, 0x14, 0xd2, 0x47, 0xbe, 0x86, 0xbc, 0x17, 0x3f, 0x6e, 0x16, 0xfa, 0x75, 0xb3,
	0xd0, 0x9f, 0x9b, 0x85, 0xbe, 0xff, 0xb5, 0x9e, 0x04, 0x3d, 0xf5, 0xeb, 0xac, 0xff, 0x05, 0x00,
	0x00, 0xff, 0xff, 0xfb, 0x75, 0xb8, 0x83, 0x7e, 0x02, 0x00, 0x00,
}
