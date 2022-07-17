# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: msg_table.proto

import sys
_b=sys.version_info[0]<3 and (lambda x:x) or (lambda x:x.encode('latin1'))
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import symbol_database as _symbol_database
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()




DESCRIPTOR = _descriptor.FileDescriptor(
  name='msg_table.proto',
  package='MsgTable',
  syntax='proto3',
  serialized_options=None,
  serialized_pb=_b('\n\x0fmsg_table.proto\x12\x08MsgTable\"6\n\nTableEntry\x12\x14\n\x0c\x63\x61llsiteAddr\x18\x01 \x01(\r\x12\x12\n\nrodataAddr\x18\x02 \x01(\r\"1\n\x08MsgTable\x12%\n\x07\x65ntries\x18\x01 \x03(\x0b\x32\x14.MsgTable.TableEntryb\x06proto3')
)




_TABLEENTRY = _descriptor.Descriptor(
  name='TableEntry',
  full_name='MsgTable.TableEntry',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='callsiteAddr', full_name='MsgTable.TableEntry.callsiteAddr', index=0,
      number=1, type=13, cpp_type=3, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='rodataAddr', full_name='MsgTable.TableEntry.rodataAddr', index=1,
      number=2, type=13, cpp_type=3, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=29,
  serialized_end=83,
)


_MSGTABLE = _descriptor.Descriptor(
  name='MsgTable',
  full_name='MsgTable.MsgTable',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='entries', full_name='MsgTable.MsgTable.entries', index=0,
      number=1, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=85,
  serialized_end=134,
)

_MSGTABLE.fields_by_name['entries'].message_type = _TABLEENTRY
DESCRIPTOR.message_types_by_name['TableEntry'] = _TABLEENTRY
DESCRIPTOR.message_types_by_name['MsgTable'] = _MSGTABLE
_sym_db.RegisterFileDescriptor(DESCRIPTOR)

TableEntry = _reflection.GeneratedProtocolMessageType('TableEntry', (_message.Message,), {
  'DESCRIPTOR' : _TABLEENTRY,
  '__module__' : 'msg_table_pb2'
  # @@protoc_insertion_point(class_scope:MsgTable.TableEntry)
  })
_sym_db.RegisterMessage(TableEntry)

MsgTable = _reflection.GeneratedProtocolMessageType('MsgTable', (_message.Message,), {
  'DESCRIPTOR' : _MSGTABLE,
  '__module__' : 'msg_table_pb2'
  # @@protoc_insertion_point(class_scope:MsgTable.MsgTable)
  })
_sym_db.RegisterMessage(MsgTable)


# @@protoc_insertion_point(module_scope)