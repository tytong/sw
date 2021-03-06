// Code generated by protoc-gen-go. DO NOT EDIT.
// source: compiler_test.proto

/*
Package testproto is a generated protocol buffer package.

It is generated from these files:
	compiler_test.proto

It has these top-level messages:
	MessageA
	MessageKey
	MessageB
	MessageC
	MessageD
*/
package testproto

import proto "github.com/golang/protobuf/proto"
import fmt "fmt"
import math "math"
import clientApi "github.com/pensando/sw/nic/delphi/gosdk/client_api"
import delphi "github.com/pensando/sw/nic/delphi/proto/delphi"

// Reference imports to suppress errors if they are not otherwise used.
var _ = proto.Marshal
var _ = fmt.Errorf
var _ = math.Inf

// This is a compile-time assertion to ensure that this generated file
// is compatible with the proto package it is being compiled against.
// A compilation error at this line likely means your copy of the
// proto package needs to be updated.
const _ = proto.ProtoPackageIsVersion2 // please upgrade the proto package

// Simplest possible message
type MessageA struct {
	Meta        *delphi.ObjectMeta `protobuf:"bytes,1,opt,name=Meta" json:"Meta,omitempty"`
	Key         uint32             `protobuf:"varint,2,opt,name=Key" json:"Key,omitempty"`
	StringValue string             `protobuf:"bytes,3,opt,name=StringValue" json:"StringValue,omitempty"`
}

func (m *MessageA) GetDelphiMessage() proto.Message {
	return m
}

func (m *MessageA) GetDelphiMeta() *delphi.ObjectMeta {
	return m.Meta
}

func (m *MessageA) SetDelphiMeta(meta *delphi.ObjectMeta) {
	m.Meta = meta
}

func (m *MessageA) GetDelphiKey() string {
	return fmt.Sprintf("%v", m.Key)
}

func (m *MessageA) GetDelphiKind() string {
	return "MessageA"
}

func (m *MessageA) GetDelphiPath() string {
	return fmt.Sprintf("%s|%s", m.GetDelphiKind(), m.GetDelphiKey())
}

func (m *MessageA) DelphiClone() clientApi.BaseObject {
	obj, _ := proto.Clone(m).(*MessageA)
	return obj
}

func MessageAMount(client clientApi.Client, mode delphi.MountMode) {
	client.MountKind("MessageA", mode)
}

func MessageAMountKey(client clientApi.Client, key uint32, mode delphi.MountMode) {
	client.MountKindKey("MessageA", fmt.Sprintf("%v", key), mode)
}

func GetMessageA(client clientApi.Client, key uint32) *MessageA {
	o := client.GetObject("MessageA", fmt.Sprintf("%v", key))
	if o == nil {
		return nil
	}
	obj, ok := o.(*MessageA)
	if ok != true {
		panic("Cast failed")
	}
	return obj
}

func (m *MessageA) IsPersistent() bool {
	return false
}
func MessageAFactory(sdkClient clientApi.Client, data []byte) (clientApi.BaseObject, error) {
	var msg MessageA
	err := proto.Unmarshal(data, &msg)
	if err != nil {
		return nil, err
	}
	return &msg, nil
}

func MessageAWatch(client clientApi.Client, reactor MessageAReactor) {
	client.WatchKind("MessageA", reactor)
}
func MessageAList(client clientApi.Client) []*MessageA {
	bobjs := client.List("MessageA")
	objs := make([]*MessageA, 0)
	for _, bobj := range bobjs {
		obj, _ := bobj.(*MessageA)
		objs = append(objs, obj)
	}
	return objs
}
func (m *MessageA) TriggerEvent(sdkClient clientApi.Client, old clientApi.BaseObject, op delphi.ObjectOperation, rl []clientApi.BaseReactor) {
	for _, r := range rl {
		rctr, ok := r.(MessageAReactor)
		if ok == false {
			panic("Not a Reactor")
		}
		if op == delphi.ObjectOperation_SetOp {
			if old == nil {
				rctr.OnMessageACreate(m)
			} else {
				oldObj, ok := old.(*MessageA)
				if ok == false {
					panic("Not an MessageA object")
				}
				rctr.OnMessageAUpdate(oldObj, m)
			}
		} else {
			rctr.OnMessageADelete(m)
		}
	}
}

type MessageAReactor interface {
	OnMessageACreate(obj *MessageA)
	OnMessageAUpdate(old *MessageA, obj *MessageA)
	OnMessageADelete(obj *MessageA)
}

func (m *MessageA) Reset()                    { *m = MessageA{} }
func (m *MessageA) String() string            { return proto.CompactTextString(m) }
func (*MessageA) ProtoMessage()               {}
func (*MessageA) Descriptor() ([]byte, []int) { return fileDescriptor0, []int{0} }

func (m *MessageA) GetMeta() *delphi.ObjectMeta {
	if m != nil {
		return m.Meta
	}
	return nil
}

func (m *MessageA) GetKey() uint32 {
	if m != nil {
		return m.Key
	}
	return 0
}

func (m *MessageA) GetStringValue() string {
	if m != nil {
		return m.StringValue
	}
	return ""
}

// Message with key being a message
type MessageKey struct {
	Value uint32 `protobuf:"varint,1,opt,name=Value" json:"Value,omitempty"`
}

func (m *MessageKey) Reset()                    { *m = MessageKey{} }
func (m *MessageKey) String() string            { return proto.CompactTextString(m) }
func (*MessageKey) ProtoMessage()               {}
func (*MessageKey) Descriptor() ([]byte, []int) { return fileDescriptor0, []int{1} }

func (m *MessageKey) GetValue() uint32 {
	if m != nil {
		return m.Value
	}
	return 0
}

type MessageB struct {
	Meta        *delphi.ObjectMeta `protobuf:"bytes,1,opt,name=Meta" json:"Meta,omitempty"`
	Key         *MessageKey        `protobuf:"bytes,2,opt,name=Key" json:"Key,omitempty"`
	StringValue string             `protobuf:"bytes,3,opt,name=StringValue" json:"StringValue,omitempty"`
}

func (m *MessageB) GetDelphiMessage() proto.Message {
	return m
}

func (m *MessageB) GetDelphiMeta() *delphi.ObjectMeta {
	return m.Meta
}

func (m *MessageB) SetDelphiMeta(meta *delphi.ObjectMeta) {
	m.Meta = meta
}

func (m *MessageB) GetDelphiKey() string {
	return m.Key.String()
}

func (m *MessageB) GetDelphiKind() string {
	return "MessageB"
}

func (m *MessageB) GetDelphiPath() string {
	return fmt.Sprintf("%s|%s", m.GetDelphiKind(), m.GetDelphiKey())
}

func (m *MessageB) DelphiClone() clientApi.BaseObject {
	obj, _ := proto.Clone(m).(*MessageB)
	return obj
}

func MessageBMount(client clientApi.Client, mode delphi.MountMode) {
	client.MountKind("MessageB", mode)
}

func MessageBMountKey(client clientApi.Client, key *MessageKey, mode delphi.MountMode) {
	client.MountKindKey("MessageB", key.String(), mode)
}

func GetMessageB(client clientApi.Client, key *MessageKey) *MessageB {
	o := client.GetObject("MessageB", key.String())
	if o == nil {
		return nil
	}
	obj, ok := o.(*MessageB)
	if ok != true {
		panic("Cast failed")
	}
	return obj
}

func (m *MessageB) IsPersistent() bool {
	return true
}
func MessageBFactory(sdkClient clientApi.Client, data []byte) (clientApi.BaseObject, error) {
	var msg MessageB
	err := proto.Unmarshal(data, &msg)
	if err != nil {
		return nil, err
	}
	return &msg, nil
}

func MessageBWatch(client clientApi.Client, reactor MessageBReactor) {
	client.WatchKind("MessageB", reactor)
}
func MessageBList(client clientApi.Client) []*MessageB {
	bobjs := client.List("MessageB")
	objs := make([]*MessageB, 0)
	for _, bobj := range bobjs {
		obj, _ := bobj.(*MessageB)
		objs = append(objs, obj)
	}
	return objs
}
func (m *MessageB) TriggerEvent(sdkClient clientApi.Client, old clientApi.BaseObject, op delphi.ObjectOperation, rl []clientApi.BaseReactor) {
	for _, r := range rl {
		rctr, ok := r.(MessageBReactor)
		if ok == false {
			panic("Not a Reactor")
		}
		if op == delphi.ObjectOperation_SetOp {
			if old == nil {
				rctr.OnMessageBCreate(m)
			} else {
				oldObj, ok := old.(*MessageB)
				if ok == false {
					panic("Not an MessageB object")
				}
				rctr.OnMessageBUpdate(oldObj, m)
			}
		} else {
			rctr.OnMessageBDelete(m)
		}
	}
}

type MessageBReactor interface {
	OnMessageBCreate(obj *MessageB)
	OnMessageBUpdate(old *MessageB, obj *MessageB)
	OnMessageBDelete(obj *MessageB)
}

func (m *MessageB) Reset()                    { *m = MessageB{} }
func (m *MessageB) String() string            { return proto.CompactTextString(m) }
func (*MessageB) ProtoMessage()               {}
func (*MessageB) Descriptor() ([]byte, []int) { return fileDescriptor0, []int{2} }

func (m *MessageB) GetMeta() *delphi.ObjectMeta {
	if m != nil {
		return m.Meta
	}
	return nil
}

func (m *MessageB) GetKey() *MessageKey {
	if m != nil {
		return m.Key
	}
	return nil
}

func (m *MessageB) GetStringValue() string {
	if m != nil {
		return m.StringValue
	}
	return ""
}

// Singleton message
type MessageC struct {
	Meta        *delphi.ObjectMeta `protobuf:"bytes,1,opt,name=Meta" json:"Meta,omitempty"`
	StringValue string             `protobuf:"bytes,3,opt,name=StringValue" json:"StringValue,omitempty"`
}

func (m *MessageC) GetDelphiMessage() proto.Message {
	return m
}

func (m *MessageC) GetDelphiMeta() *delphi.ObjectMeta {
	return m.Meta
}

func (m *MessageC) SetDelphiMeta(meta *delphi.ObjectMeta) {
	m.Meta = meta
}

func (m *MessageC) GetDelphiKey() string {
	return "default"
}

func (m *MessageC) GetDelphiKind() string {
	return "MessageC"
}

func (m *MessageC) GetDelphiPath() string {
	return fmt.Sprintf("%s|%s", m.GetDelphiKind(), m.GetDelphiKey())
}

func (m *MessageC) DelphiClone() clientApi.BaseObject {
	obj, _ := proto.Clone(m).(*MessageC)
	return obj
}

func MessageCMount(client clientApi.Client, mode delphi.MountMode) {
	client.MountKind("MessageC", mode)
}

func GetMessageC(client clientApi.Client) *MessageC {
	o := client.GetObject("MessageC", "default")
	if o == nil {
		return nil
	}
	obj, ok := o.(*MessageC)
	if ok != true {
		panic("Cast failed")
	}
	return obj
}

func (m *MessageC) IsPersistent() bool {
	return false
}
func MessageCFactory(sdkClient clientApi.Client, data []byte) (clientApi.BaseObject, error) {
	var msg MessageC
	err := proto.Unmarshal(data, &msg)
	if err != nil {
		return nil, err
	}
	return &msg, nil
}

func MessageCWatch(client clientApi.Client, reactor MessageCReactor) {
	client.WatchKind("MessageC", reactor)
}
func MessageCList(client clientApi.Client) []*MessageC {
	bobjs := client.List("MessageC")
	objs := make([]*MessageC, 0)
	for _, bobj := range bobjs {
		obj, _ := bobj.(*MessageC)
		objs = append(objs, obj)
	}
	return objs
}
func (m *MessageC) TriggerEvent(sdkClient clientApi.Client, old clientApi.BaseObject, op delphi.ObjectOperation, rl []clientApi.BaseReactor) {
	for _, r := range rl {
		rctr, ok := r.(MessageCReactor)
		if ok == false {
			panic("Not a Reactor")
		}
		if op == delphi.ObjectOperation_SetOp {
			if old == nil {
				rctr.OnMessageCCreate(m)
			} else {
				oldObj, ok := old.(*MessageC)
				if ok == false {
					panic("Not an MessageC object")
				}
				rctr.OnMessageCUpdate(oldObj, m)
			}
		} else {
			rctr.OnMessageCDelete(m)
		}
	}
}

type MessageCReactor interface {
	OnMessageCCreate(obj *MessageC)
	OnMessageCUpdate(old *MessageC, obj *MessageC)
	OnMessageCDelete(obj *MessageC)
}

func (m *MessageC) Reset()                    { *m = MessageC{} }
func (m *MessageC) String() string            { return proto.CompactTextString(m) }
func (*MessageC) ProtoMessage()               {}
func (*MessageC) Descriptor() ([]byte, []int) { return fileDescriptor0, []int{3} }

func (m *MessageC) GetMeta() *delphi.ObjectMeta {
	if m != nil {
		return m.Meta
	}
	return nil
}

func (m *MessageC) GetStringValue() string {
	if m != nil {
		return m.StringValue
	}
	return ""
}

type MessageD struct {
	Meta *delphi.ObjectMeta `protobuf:"bytes,1,opt,name=Meta" json:"Meta,omitempty"`
	Key  uint32             `protobuf:"varint,2,opt,name=Key" json:"Key,omitempty"`
	RefA string             `protobuf:"bytes,3,opt,name=RefA" json:"RefA,omitempty"`
}

func (m *MessageD) GetDelphiMessage() proto.Message {
	return m
}

func (m *MessageD) GetDelphiMeta() *delphi.ObjectMeta {
	return m.Meta
}

func (m *MessageD) SetDelphiMeta(meta *delphi.ObjectMeta) {
	m.Meta = meta
}

func (m *MessageD) GetDelphiKey() string {
	return fmt.Sprintf("%v", m.Key)
}

func (m *MessageD) GetDelphiKind() string {
	return "MessageD"
}

func (m *MessageD) GetDelphiPath() string {
	return fmt.Sprintf("%s|%s", m.GetDelphiKind(), m.GetDelphiKey())
}

func (m *MessageD) DelphiClone() clientApi.BaseObject {
	obj, _ := proto.Clone(m).(*MessageD)
	return obj
}

func MessageDMount(client clientApi.Client, mode delphi.MountMode) {
	client.MountKind("MessageD", mode)
}

func MessageDMountKey(client clientApi.Client, key uint32, mode delphi.MountMode) {
	client.MountKindKey("MessageD", fmt.Sprintf("%v", key), mode)
}

func GetMessageD(client clientApi.Client, key uint32) *MessageD {
	o := client.GetObject("MessageD", fmt.Sprintf("%v", key))
	if o == nil {
		return nil
	}
	obj, ok := o.(*MessageD)
	if ok != true {
		panic("Cast failed")
	}
	return obj
}

func (m *MessageD) IsPersistent() bool {
	return false
}
func MessageDFactory(sdkClient clientApi.Client, data []byte) (clientApi.BaseObject, error) {
	var msg MessageD
	err := proto.Unmarshal(data, &msg)
	if err != nil {
		return nil, err
	}
	return &msg, nil
}

func MessageDWatch(client clientApi.Client, reactor MessageDReactor) {
	client.WatchKind("MessageD", reactor)
}
func MessageDList(client clientApi.Client) []*MessageD {
	bobjs := client.List("MessageD")
	objs := make([]*MessageD, 0)
	for _, bobj := range bobjs {
		obj, _ := bobj.(*MessageD)
		objs = append(objs, obj)
	}
	return objs
}
func (m *MessageD) TriggerEvent(sdkClient clientApi.Client, old clientApi.BaseObject, op delphi.ObjectOperation, rl []clientApi.BaseReactor) {
	for _, r := range rl {
		rctr, ok := r.(MessageDReactor)
		if ok == false {
			panic("Not a Reactor")
		}
		if op == delphi.ObjectOperation_SetOp {
			if old == nil {
				rctr.OnMessageDCreate(m)
			} else {
				oldObj, ok := old.(*MessageD)
				if ok == false {
					panic("Not an MessageD object")
				}
				rctr.OnMessageDUpdate(oldObj, m)
			}
		} else {
			rctr.OnMessageDDelete(m)
		}
	}
}

type MessageDReactor interface {
	OnMessageDCreate(obj *MessageD)
	OnMessageDUpdate(old *MessageD, obj *MessageD)
	OnMessageDDelete(obj *MessageD)
}

func (o *MessageD) LinkToRefA(f *MessageA) {
	o.RefA = f.GetDelphiKey()
}
func MessageDGetRefAObj(client clientApi.Client, o *MessageD) *MessageA {
	obj := client.GetObject("MessageA", o.RefA)
	if obj == nil {
		return nil
	}
	cobj, ok := obj.(*MessageA)
	if ok != true {
		panic("Cast failed")
	}
	return cobj
}
func GetMessageDFromRefA(client clientApi.Client, f *MessageA) *MessageD {
	o := client.GetFromIndex("MessageA", "MessageD", "RefA", f.GetDelphiKey())
	if o == nil {
		return nil
	}
	obj, ok := o.(*MessageD)
	if ok != true {
		panic("Cast failed")
	}
	return obj
}
func MessageDRefAKeyExtractor(o clientApi.BaseObject) string {
	obj, _ := o.(*MessageD)
	return obj.RefA
}
func (m *MessageD) Reset()                    { *m = MessageD{} }
func (m *MessageD) String() string            { return proto.CompactTextString(m) }
func (*MessageD) ProtoMessage()               {}
func (*MessageD) Descriptor() ([]byte, []int) { return fileDescriptor0, []int{4} }

func (m *MessageD) GetMeta() *delphi.ObjectMeta {
	if m != nil {
		return m.Meta
	}
	return nil
}

func (m *MessageD) GetKey() uint32 {
	if m != nil {
		return m.Key
	}
	return 0
}

func (m *MessageD) GetRefA() string {
	if m != nil {
		return m.RefA
	}
	return ""
}

func init() {
	clientApi.RegisterFactory("MessageA", MessageAFactory)
	proto.RegisterType((*MessageA)(nil), "testproto.MessageA")
	proto.RegisterType((*MessageKey)(nil), "testproto.MessageKey")
	clientApi.RegisterFactory("MessageB", MessageBFactory)
	proto.RegisterType((*MessageB)(nil), "testproto.MessageB")
	clientApi.RegisterFactory("MessageC", MessageCFactory)
	proto.RegisterType((*MessageC)(nil), "testproto.MessageC")
	clientApi.RegisterFactory("MessageD", MessageDFactory)
	clientApi.CreateIndex("MessageD", "RefA", "MessageA", MessageDRefAKeyExtractor)
	proto.RegisterType((*MessageD)(nil), "testproto.MessageD")
}

func init() { proto.RegisterFile("compiler_test.proto", fileDescriptor0) }

var fileDescriptor0 = []byte{
	// 237 bytes of a gzipped FileDescriptorProto
	0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xff, 0xe2, 0x12, 0x4e, 0xce, 0xcf, 0x2d,
	0xc8, 0xcc, 0x49, 0x2d, 0x8a, 0x2f, 0x49, 0x2d, 0x2e, 0xd1, 0x2b, 0x28, 0xca, 0x2f, 0xc9, 0x17,
	0xe2, 0x04, 0xb1, 0xc1, 0x4c, 0x29, 0x9e, 0x94, 0xd4, 0x9c, 0x82, 0x8c, 0x4c, 0x88, 0x84, 0x52,
	0x1a, 0x17, 0x87, 0x6f, 0x6a, 0x71, 0x71, 0x62, 0x7a, 0xaa, 0xa3, 0x90, 0x1a, 0x17, 0x8b, 0x6f,
	0x6a, 0x49, 0xa2, 0x04, 0xa3, 0x02, 0xa3, 0x06, 0xb7, 0x91, 0x90, 0x1e, 0x54, 0xa1, 0x7f, 0x52,
	0x56, 0x6a, 0x72, 0x09, 0x48, 0x26, 0x08, 0x2c, 0x2f, 0x24, 0xc0, 0xc5, 0xec, 0x9d, 0x5a, 0x29,
	0xc1, 0xa4, 0xc0, 0xa8, 0xc1, 0x1b, 0x04, 0x62, 0x0a, 0x29, 0x70, 0x71, 0x07, 0x97, 0x14, 0x65,
	0xe6, 0xa5, 0x87, 0x25, 0xe6, 0x94, 0xa6, 0x4a, 0x30, 0x2b, 0x30, 0x6a, 0x70, 0x06, 0x21, 0x0b,
	0x29, 0x29, 0x71, 0x71, 0x41, 0xed, 0x01, 0xa9, 0x17, 0xe1, 0x62, 0x85, 0xa8, 0x64, 0x04, 0x9b,
	0x01, 0xe1, 0x28, 0x35, 0x33, 0xc2, 0x1d, 0xe3, 0x44, 0xb4, 0x63, 0xd4, 0x11, 0x8e, 0xe1, 0x36,
	0x12, 0xd5, 0x83, 0xfb, 0x53, 0x0f, 0x61, 0x1d, 0x91, 0x6e, 0xb4, 0x62, 0x59, 0x30, 0x5d, 0x89,
	0x51, 0x29, 0x0a, 0xee, 0x08, 0x67, 0xa2, 0x1d, 0x41, 0x84, 0xd9, 0x1d, 0x20, 0xb3, 0x33, 0xe0,
	0x66, 0xbb, 0x50, 0x10, 0xda, 0x4a, 0x5c, 0x2c, 0x41, 0xa9, 0x69, 0x8e, 0x10, 0x6b, 0x9c, 0xf8,
	0xba, 0xa6, 0x2b, 0x71, 0x21, 0x62, 0x31, 0x08, 0x2c, 0x97, 0xc4, 0x06, 0x0e, 0x04, 0x63, 0x40,
	0x00, 0x00, 0x00, 0xff, 0xff, 0x10, 0xd3, 0x1b, 0xe1, 0x0e, 0x02, 0x00, 0x00,
}
