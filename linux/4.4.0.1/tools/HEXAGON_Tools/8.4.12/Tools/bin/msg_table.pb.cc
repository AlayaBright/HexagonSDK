// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: msg_table.proto

#include "msg_table.pb.h"

#include <algorithm>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
extern PROTOBUF_INTERNAL_EXPORT_msg_5ftable_2eproto ::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<0> scc_info_TableEntry_msg_5ftable_2eproto;
namespace MsgTable {
class TableEntryDefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<TableEntry> _instance;
} _TableEntry_default_instance_;
class MsgTableDefaultTypeInternal {
 public:
  ::PROTOBUF_NAMESPACE_ID::internal::ExplicitlyConstructed<MsgTable> _instance;
} _MsgTable_default_instance_;
}  // namespace MsgTable
static void InitDefaultsscc_info_MsgTable_msg_5ftable_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::MsgTable::_MsgTable_default_instance_;
    new (ptr) ::MsgTable::MsgTable();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
  ::MsgTable::MsgTable::InitAsDefaultInstance();
}

::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<1> scc_info_MsgTable_msg_5ftable_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 1, 0, InitDefaultsscc_info_MsgTable_msg_5ftable_2eproto}, {
      &scc_info_TableEntry_msg_5ftable_2eproto.base,}};

static void InitDefaultsscc_info_TableEntry_msg_5ftable_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::MsgTable::_TableEntry_default_instance_;
    new (ptr) ::MsgTable::TableEntry();
    ::PROTOBUF_NAMESPACE_ID::internal::OnShutdownDestroyMessage(ptr);
  }
  ::MsgTable::TableEntry::InitAsDefaultInstance();
}

::PROTOBUF_NAMESPACE_ID::internal::SCCInfo<0> scc_info_TableEntry_msg_5ftable_2eproto =
    {{ATOMIC_VAR_INIT(::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase::kUninitialized), 0, 0, InitDefaultsscc_info_TableEntry_msg_5ftable_2eproto}, {}};

static ::PROTOBUF_NAMESPACE_ID::Metadata file_level_metadata_msg_5ftable_2eproto[2];
static constexpr ::PROTOBUF_NAMESPACE_ID::EnumDescriptor const** file_level_enum_descriptors_msg_5ftable_2eproto = nullptr;
static constexpr ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor const** file_level_service_descriptors_msg_5ftable_2eproto = nullptr;

const ::PROTOBUF_NAMESPACE_ID::uint32 TableStruct_msg_5ftable_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::MsgTable::TableEntry, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::MsgTable::TableEntry, callsiteaddr_),
  PROTOBUF_FIELD_OFFSET(::MsgTable::TableEntry, rodataaddr_),
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::MsgTable::MsgTable, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::MsgTable::MsgTable, entries_),
};
static const ::PROTOBUF_NAMESPACE_ID::internal::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, sizeof(::MsgTable::TableEntry)},
  { 7, -1, sizeof(::MsgTable::MsgTable)},
};

static ::PROTOBUF_NAMESPACE_ID::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::MsgTable::_TableEntry_default_instance_),
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::MsgTable::_MsgTable_default_instance_),
};

const char descriptor_table_protodef_msg_5ftable_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\017msg_table.proto\022\010MsgTable\"6\n\nTableEntr"
  "y\022\024\n\014callsiteAddr\030\001 \001(\r\022\022\n\nrodataAddr\030\002 "
  "\001(\r\"1\n\010MsgTable\022%\n\007entries\030\001 \003(\0132\024.MsgTa"
  "ble.TableEntryb\006proto3"
  ;
static const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable*const descriptor_table_msg_5ftable_2eproto_deps[1] = {
};
static ::PROTOBUF_NAMESPACE_ID::internal::SCCInfoBase*const descriptor_table_msg_5ftable_2eproto_sccs[2] = {
  &scc_info_MsgTable_msg_5ftable_2eproto.base,
  &scc_info_TableEntry_msg_5ftable_2eproto.base,
};
static ::PROTOBUF_NAMESPACE_ID::internal::once_flag descriptor_table_msg_5ftable_2eproto_once;
static bool descriptor_table_msg_5ftable_2eproto_initialized = false;
const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_msg_5ftable_2eproto = {
  &descriptor_table_msg_5ftable_2eproto_initialized, descriptor_table_protodef_msg_5ftable_2eproto, "msg_table.proto", 142,
  &descriptor_table_msg_5ftable_2eproto_once, descriptor_table_msg_5ftable_2eproto_sccs, descriptor_table_msg_5ftable_2eproto_deps, 2, 0,
  schemas, file_default_instances, TableStruct_msg_5ftable_2eproto::offsets,
  file_level_metadata_msg_5ftable_2eproto, 2, file_level_enum_descriptors_msg_5ftable_2eproto, file_level_service_descriptors_msg_5ftable_2eproto,
};

// Force running AddDescriptors() at dynamic initialization time.
static bool dynamic_init_dummy_msg_5ftable_2eproto = (  ::PROTOBUF_NAMESPACE_ID::internal::AddDescriptors(&descriptor_table_msg_5ftable_2eproto), true);
namespace MsgTable {

// ===================================================================

void TableEntry::InitAsDefaultInstance() {
}
class TableEntry::_Internal {
 public:
};

TableEntry::TableEntry()
  : ::PROTOBUF_NAMESPACE_ID::Message(), _internal_metadata_(nullptr) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:MsgTable.TableEntry)
}
TableEntry::TableEntry(const TableEntry& from)
  : ::PROTOBUF_NAMESPACE_ID::Message(),
      _internal_metadata_(nullptr) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::memcpy(&callsiteaddr_, &from.callsiteaddr_,
    static_cast<size_t>(reinterpret_cast<char*>(&rodataaddr_) -
    reinterpret_cast<char*>(&callsiteaddr_)) + sizeof(rodataaddr_));
  // @@protoc_insertion_point(copy_constructor:MsgTable.TableEntry)
}

void TableEntry::SharedCtor() {
  ::memset(&callsiteaddr_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&rodataaddr_) -
      reinterpret_cast<char*>(&callsiteaddr_)) + sizeof(rodataaddr_));
}

TableEntry::~TableEntry() {
  // @@protoc_insertion_point(destructor:MsgTable.TableEntry)
  SharedDtor();
}

void TableEntry::SharedDtor() {
}

void TableEntry::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const TableEntry& TableEntry::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_TableEntry_msg_5ftable_2eproto.base);
  return *internal_default_instance();
}


void TableEntry::Clear() {
// @@protoc_insertion_point(message_clear_start:MsgTable.TableEntry)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  ::memset(&callsiteaddr_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&rodataaddr_) -
      reinterpret_cast<char*>(&callsiteaddr_)) + sizeof(rodataaddr_));
  _internal_metadata_.Clear();
}

const char* TableEntry::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {
      // uint32 callsiteAddr = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          callsiteaddr_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      // uint32 rodataAddr = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 16)) {
          rodataaddr_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint(&ptr);
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag, &_internal_metadata_, ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }  // switch
  }  // while
success:
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* TableEntry::InternalSerializeWithCachedSizesToArray(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:MsgTable.TableEntry)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // uint32 callsiteAddr = 1;
  if (this->callsiteaddr() != 0) {
    stream->EnsureSpace(&target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(1, this->_internal_callsiteaddr(), target);
  }

  // uint32 rodataAddr = 2;
  if (this->rodataaddr() != 0) {
    stream->EnsureSpace(&target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteUInt32ToArray(2, this->_internal_rodataaddr(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields(), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:MsgTable.TableEntry)
  return target;
}

size_t TableEntry::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:MsgTable.TableEntry)
  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // uint32 callsiteAddr = 1;
  if (this->callsiteaddr() != 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
        this->_internal_callsiteaddr());
  }

  // uint32 rodataAddr = 2;
  if (this->rodataaddr() != 0) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::UInt32Size(
        this->_internal_rodataaddr());
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    return ::PROTOBUF_NAMESPACE_ID::internal::ComputeUnknownFieldsSize(
        _internal_metadata_, total_size, &_cached_size_);
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void TableEntry::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:MsgTable.TableEntry)
  GOOGLE_DCHECK_NE(&from, this);
  const TableEntry* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<TableEntry>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:MsgTable.TableEntry)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:MsgTable.TableEntry)
    MergeFrom(*source);
  }
}

void TableEntry::MergeFrom(const TableEntry& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:MsgTable.TableEntry)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from.callsiteaddr() != 0) {
    _internal_set_callsiteaddr(from._internal_callsiteaddr());
  }
  if (from.rodataaddr() != 0) {
    _internal_set_rodataaddr(from._internal_rodataaddr());
  }
}

void TableEntry::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:MsgTable.TableEntry)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void TableEntry::CopyFrom(const TableEntry& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:MsgTable.TableEntry)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool TableEntry::IsInitialized() const {
  return true;
}

void TableEntry::InternalSwap(TableEntry* other) {
  using std::swap;
  _internal_metadata_.Swap(&other->_internal_metadata_);
  swap(callsiteaddr_, other->callsiteaddr_);
  swap(rodataaddr_, other->rodataaddr_);
}

::PROTOBUF_NAMESPACE_ID::Metadata TableEntry::GetMetadata() const {
  return GetMetadataStatic();
}


// ===================================================================

void MsgTable::InitAsDefaultInstance() {
}
class MsgTable::_Internal {
 public:
};

MsgTable::MsgTable()
  : ::PROTOBUF_NAMESPACE_ID::Message(), _internal_metadata_(nullptr) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:MsgTable.MsgTable)
}
MsgTable::MsgTable(const MsgTable& from)
  : ::PROTOBUF_NAMESPACE_ID::Message(),
      _internal_metadata_(nullptr),
      entries_(from.entries_) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  // @@protoc_insertion_point(copy_constructor:MsgTable.MsgTable)
}

void MsgTable::SharedCtor() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&scc_info_MsgTable_msg_5ftable_2eproto.base);
}

MsgTable::~MsgTable() {
  // @@protoc_insertion_point(destructor:MsgTable.MsgTable)
  SharedDtor();
}

void MsgTable::SharedDtor() {
}

void MsgTable::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const MsgTable& MsgTable::default_instance() {
  ::PROTOBUF_NAMESPACE_ID::internal::InitSCC(&::scc_info_MsgTable_msg_5ftable_2eproto.base);
  return *internal_default_instance();
}


void MsgTable::Clear() {
// @@protoc_insertion_point(message_clear_start:MsgTable.MsgTable)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  entries_.Clear();
  _internal_metadata_.Clear();
}

const char* MsgTable::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {
      // repeated .MsgTable.TableEntry entries = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 10)) {
          ptr -= 1;
          do {
            ptr += 1;
            ptr = ctx->ParseMessage(_internal_add_entries(), ptr);
            CHK_(ptr);
            if (!ctx->DataAvailable(ptr)) break;
          } while (::PROTOBUF_NAMESPACE_ID::internal::ExpectTag<10>(ptr));
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag, &_internal_metadata_, ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }  // switch
  }  // while
success:
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* MsgTable::InternalSerializeWithCachedSizesToArray(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:MsgTable.MsgTable)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // repeated .MsgTable.TableEntry entries = 1;
  for (unsigned int i = 0,
      n = static_cast<unsigned int>(this->_internal_entries_size()); i < n; i++) {
    stream->EnsureSpace(&target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      InternalWriteMessageToArray(1, this->_internal_entries(i), target, stream);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields(), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:MsgTable.MsgTable)
  return target;
}

size_t MsgTable::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:MsgTable.MsgTable)
  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // repeated .MsgTable.TableEntry entries = 1;
  total_size += 1UL * this->_internal_entries_size();
  for (const auto& msg : this->entries_) {
    total_size +=
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::MessageSize(msg);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    return ::PROTOBUF_NAMESPACE_ID::internal::ComputeUnknownFieldsSize(
        _internal_metadata_, total_size, &_cached_size_);
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void MsgTable::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:MsgTable.MsgTable)
  GOOGLE_DCHECK_NE(&from, this);
  const MsgTable* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<MsgTable>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:MsgTable.MsgTable)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:MsgTable.MsgTable)
    MergeFrom(*source);
  }
}

void MsgTable::MergeFrom(const MsgTable& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:MsgTable.MsgTable)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  entries_.MergeFrom(from.entries_);
}

void MsgTable::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:MsgTable.MsgTable)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void MsgTable::CopyFrom(const MsgTable& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:MsgTable.MsgTable)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool MsgTable::IsInitialized() const {
  return true;
}

void MsgTable::InternalSwap(MsgTable* other) {
  using std::swap;
  _internal_metadata_.Swap(&other->_internal_metadata_);
  entries_.InternalSwap(&other->entries_);
}

::PROTOBUF_NAMESPACE_ID::Metadata MsgTable::GetMetadata() const {
  return GetMetadataStatic();
}


// @@protoc_insertion_point(namespace_scope)
}  // namespace MsgTable
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::MsgTable::TableEntry* Arena::CreateMaybeMessage< ::MsgTable::TableEntry >(Arena* arena) {
  return Arena::CreateInternal< ::MsgTable::TableEntry >(arena);
}
template<> PROTOBUF_NOINLINE ::MsgTable::MsgTable* Arena::CreateMaybeMessage< ::MsgTable::MsgTable >(Arena* arena) {
  return Arena::CreateInternal< ::MsgTable::MsgTable >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
