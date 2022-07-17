//===- PluginADT.h --------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef PLUGIN_ADT_H
#define PLUGIN_ADT_H

#include "LinkerPlugin.h"
#include <string>
#include <sys/types.h>
#include <vector>

namespace mcld {
// Forward Declarations, that are not defined anywhere.
class Fragment;

class Relocation;

class Section;

class ResolveInfo;

class ELFSection;

class InputFile;

class PluginData;

class Relocator;

}; // namespace mcld

namespace llvm {
class Timer;
}

namespace QCLD {

// This particular piece is duplicated in all the header files. This is
// make sure the customer includes one header file. Heaer files that are
// included in specific CPP files will also export the definition.
#ifdef _WIN32
// MSVC produces a warning from exporting STL types. This is to prevent
// customers from mixing STL implementations. We dont have that use case
// as we ask the customers to use the same runtime as per what is built with the
// tools. Disable this warning.
#pragma warning(disable : 4251)
#ifndef DLL_A_EXPORT
#ifndef IN_DLL
#define DLL_A_EXPORT __declspec(dllexport)
#else
#define DLL_A_EXPORT __declspec(dllimport)
#endif // IN_DLL
#endif // DLL_A_EXPORT
#endif // WIN32

#ifndef _WIN32
#define DLL_A_EXPORT
#endif // WIN32

#if defined(__GNUC__) || defined(__clang__)
#define DEPRECATED __attribute__((deprecated))
#elif defined(_MSC_VER)
#define DEPRECATED __declspec(deprecated)
#else
#pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#define DEPRECATED
#endif

struct DLL_A_EXPORT Chunk;
struct DLL_A_EXPORT OutputSection;
struct DLL_A_EXPORT Section;
struct DLL_A_EXPORT Symbol;
struct DLL_A_EXPORT Use;
struct DLL_A_EXPORT InputFile;
struct DLL_A_EXPORT PluginData;

/// Chunk represents a part of a section or it can be linker inserted.
/// A set of chunks form a Section.
struct DLL_A_EXPORT Chunk {
  explicit Chunk(mcld::Fragment *F) : m_Fragment(F) {}

  Chunk() : m_Fragment(nullptr) {}

  /// Return the name of the Chunk. This is same as the section that the Chunk
  /// belongs to.
  std::string getName() const;

  /// Return the InputFile where the Chunk originates from.
  QCLD::InputFile getInputFile() const;

  /// Size of the Chunk.
  uint32_t getSize() const;

  /// Alignment of the Chunk.
  uint32_t getAlignment() const;

  /// Return the Section where the Chunk would live.
  QCLD::Section getSection() const;

  /// Return the fragment for the Chunk.
  mcld::Fragment *getFragment() const { return m_Fragment; }

  /// Return the RAW data what the Chunk contains.
  const char *getRawData() const;

  /// Get symbols that are part of a Chunk.
  std::vector<QCLD::Symbol> getSymbols() const;

  /// Helper comparator function that iterates over the uses of a Chunk.
  struct DLL_A_EXPORT Compare {
    bool operator()(const Chunk &A, const Chunk &B) const;
  };

  ///
  /// Operators
  ///
  bool operator()(const Chunk &A, const Chunk &B) const;

  bool operator<(const Chunk &RHS) const { return m_Fragment < RHS.m_Fragment; }

  bool operator>(const Chunk &RHS) const { return m_Fragment > RHS.m_Fragment; }

  bool operator==(const Chunk &RHS) const {
    return m_Fragment == RHS.m_Fragment;
  }

  operator bool() const { return m_Fragment != nullptr; }

  /// Check if the symbol has any content.
  bool hasContent() const;

  /// Does the chunk contain code ?
  bool isCode() const;

  /// Retrieve the address of the Chunk.
  size_t getAddress() const;

  /// Get Dependent Sections.
  std::vector<QCLD::Section> getDependentSections() const;

private:
  /// Internal Data structure.
  mcld::Fragment *m_Fragment;
};

// Rule represents a Linker Script rule inside an OutputSection.
struct DLL_A_EXPORT LinkerScriptRule {

  enum DLL_A_EXPORT State : uint8_t {
    Empty,
    NoChunk,
    DuplicateChunk,
    NotEmpty,
    Ok
  };

  LinkerScriptRule() : m_Input(nullptr) {}

  LinkerScriptRule(void *I) : m_Input(I) {}

  // Can sections selected by the linker script rule be moved to another
  // output section ?
  bool canMoveSections() const;

  // Is this a linker inserted rule
  bool isLinkerInsertedRule() const;

  /// Is the rule corresponding to an KEEP sections.
  bool isKeep() const;

  /// Does the rule have any expressions.
  bool hasExpressions() const;

  /// Returns true if an expression modifies '.'
  bool doesExpressionModifyDot() const;

  /// Dump the rule as string for any diagnostics.
  std::string asString() const;

  // Add/Remove/Update chunks for any section updates.
  DEPRECATED void addChunk(QCLD::Chunk C);

  DEPRECATED void removeChunk(QCLD::Chunk C);

  DEPRECATED void updateChunks(std::vector<QCLD::Chunk> C);

  DEPRECATED State addChunk(QCLD::Chunk C, bool Verify);

  DEPRECATED State removeChunk(QCLD::Chunk C, bool Verify);

  DEPRECATED State updateChunks(std::vector<QCLD::Chunk> C, bool Verify);

  std::vector<QCLD::Chunk> getChunks() const;

  std::vector<QCLD::Section> getSections();

  // Get all the Chunks in the rule that correspond to Section S.
  std::vector<QCLD::Chunk> getChunks(QCLD::Section S);

  void *getInputSection() const { return m_Input; }

  /// Conversion Operator
  operator bool() const { return m_Input != nullptr; }

  ///
  /// Operators
  ///
  bool operator()(const LinkerScriptRule &A, const LinkerScriptRule &B) const {
    return A.m_Input != B.m_Input;
  }

  bool operator<(const LinkerScriptRule &RHS) const {
    return m_Input < RHS.m_Input;
  }

  bool operator>(const LinkerScriptRule &RHS) const {
    return m_Input > RHS.m_Input;
  }

  bool operator==(const LinkerScriptRule &RHS) const {
    return m_Input == RHS.m_Input;
  }

private:
  void *m_Input;
};

// OutputSection represents a Output Section.
struct DLL_A_EXPORT OutputSection {
  explicit OutputSection(void *O) : m_OutputSection(O) {}

  /// Return the name of the input section.
  std::string getName() const;

  /// Alignment of the section.
  uint64_t getAlignment() const;

  /// Alignment of the section.
  uint64_t getFlags() const;

  /// Get the Index of the output section.
  uint64_t getIndex() const;

  /// Get the size of the output section.
  uint64_t getSize() const;

  /// Get all linker script rules for the OutputSection.
  std::vector<QCLD::LinkerScriptRule> getLinkerScriptRules();

  void *getOutputSection() const { return m_OutputSection; }

  /// Conversion Operator
  operator bool() const { return m_OutputSection != nullptr; }

  ///
  /// Operators
  ///
  bool operator()(const OutputSection &A, const OutputSection &B) const {
    return A.m_OutputSection != B.m_OutputSection;
  }

  bool operator<(const OutputSection &RHS) const {
    return m_OutputSection < RHS.m_OutputSection;
  }

  bool operator>(const OutputSection &RHS) const {
    return m_OutputSection > RHS.m_OutputSection;
  }

  bool operator==(const OutputSection &RHS) const {
    return m_OutputSection == RHS.m_OutputSection;
  }

private:
  // Internal Data structure.
  void *m_OutputSection;
};

// Section represents a Input Section.
struct DLL_A_EXPORT Section {
  enum SectionType : uint8_t { Default, Padding };

  explicit Section(mcld::Section *S, SectionType T = SectionType::Default)
      : m_Section(S), Type(T) {}

  /// Return the name of the input section.
  std::string getName() const;

  SectionType getType() const { return Type; }

  /// Return the InputFile of the input section.
  QCLD::InputFile getInputFile() const;

  /// Get Input index of the Section in the file.
  uint32_t getIndex() const;

  // Size of the section.
  uint32_t getSize() const;

  // Return if the section is being discarded.
  bool isDiscarded() const;

  /// Alignment of the section.
  uint32_t getAlignment() const;

  /// Match a pattern.
  bool matchPattern(const std::string &Pattern) const;

  /// Get symbols that are part of a Section.
  std::vector<QCLD::Symbol> getSymbols() const;

  /// Get Chunks that are part of a Section.
  std::vector<QCLD::Chunk> getChunks() const;

  /// This is a dangerous function, that allows a section to be discarded from
  /// being emitted in the ELF file.
  void markAsDiscarded();

  /// Does the section contain code ?
  bool isCode() const;

  mcld::Section *getSection() const { return m_Section; }

  /// Get the linker script rule that was matched for this section.
  LinkerScriptRule getLinkerScriptRule() const;

  /// Get Dependent Sections.
  std::vector<QCLD::Section> getDependentSections() const;

  /// Conversion Operator
  operator bool() const { return m_Section != nullptr; }

  ///
  /// Operators
  ///
  bool operator()(const Section &A, const Section &B) const {
    return A.m_Section != B.m_Section;
  }

  bool operator<(const Section &RHS) const { return m_Section < RHS.m_Section; }

  bool operator>(const Section &RHS) const { return m_Section > RHS.m_Section; }

  bool operator==(const Section &RHS) const {
    return m_Section == RHS.m_Section;
  }

private:
  // Internal Data structure.
  mcld::Section *m_Section;
  SectionType Type;
};

// MemoryBlocks represent output sections and their content.
struct DLL_A_EXPORT Block {
  Block() : Data(nullptr), Size(0), Address(0), Alignment(1) {}
  const uint8_t *Data; /// Data thats passed to the plugin
  uint32_t Size;       /// Size of the data in bytes.
  uint32_t Address;    /// Address of the data
  uint32_t Alignment;  /// Alignment of the data
  std::string Name;    /// Name of the block
};

// This represents a Use. This represents references from a Chunk.
struct DLL_A_EXPORT Use {
  enum DLL_A_EXPORT Status : uint8_t { Ok, SymbolDoesNotExist, Error };
  explicit Use(mcld::Relocation *R) : m_Relocation(R) {}

  /// Return the symbol name for a Use. The symbol name is returned as how
  /// the symbol name is recorded in the input ELF file.
  std::string getName() const;

  /// Return the Chunk for a use.
  Chunk getChunk() const;

  /// Get the source of the use.
  Chunk getSourceChunk() const;

  /// Return the Linker Symbol for a use.
  QCLD::Symbol getSymbol() const;

  /// Get the Relocation.
  mcld::Relocation *getRelocation() const { return m_Relocation; }

  /// The user may want to wish to reset the symbol to a different
  /// Symbol.
  Use::Status resetSymbol(QCLD::Symbol S);

  /// Get the Relocation Type.
  uint32_t getType() const;

  // Get the offset in Chunk.
  off_t getOffsetInChunk() const;

  // Get the address of the Use.
  size_t getSourceAddress() const;

  /// Conversion Operator
  operator bool() const { return m_Relocation != nullptr; }

  ///
  /// Operators
  ///
  bool operator()(const Use &A, const Use &B) const {
    return A.m_Relocation != B.m_Relocation;
  }

  bool operator<(const Use &RHS) const {
    return m_Relocation < RHS.m_Relocation;
  }

  bool operator>(const Use &RHS) const {
    return m_Relocation > RHS.m_Relocation;
  }

  bool operator==(const Use &RHS) const {
    return m_Relocation == RHS.m_Relocation;
  }

private:
  /// Internal Data structure.
  mcld::Relocation *m_Relocation;
};

// This corresponds to a symbol in the Linker.
struct DLL_A_EXPORT Symbol {
  explicit Symbol(mcld::ResolveInfo *S) : m_Symbol(S) {}

  /// Return the Chunk for a symbol.
  Chunk getChunk() const;

  /// Return the Name of the symbol.
  std::string getName() const;

  /// Return true if symbol is Local.
  bool isLocal() const;

  /// Return true if symbol is Weak.
  bool isWeak() const;

  /// Return true if symbol is Global.
  bool isGlobal() const;

  /// Return true if symbol is Undefined.
  bool isUndef() const;

  /// Return true if symbol is Common.
  bool isCommon() const;

  /// Return true if symbol is Section.
  bool isSection() const;

  /// Return true if symbol is Function.
  bool isFunction() const;

  /// Return true if symbol is Garbage collected.
  bool isGarbageCollected() const;

  /// Get the ressolved path for a Symbol.
  std::string getResolvedPath() const;

  /// Get the size of the symbol.
  uint32_t getSize() const;

  // Get the Symbol Value.
  size_t getValue() const;

  // Get the Symbol Address. Most times the symbol value
  // will be equal to the address of the symbol. If there
  // is a need to access the address of the symbol before
  // a symbol gets finalized, getAddress can be utilized.
  size_t getAddress() const;

  // Get the offset in Chunk.
  off_t getOffsetInChunk() const;

  /// Get the Symbol.
  mcld::ResolveInfo *getSymbol() const { return m_Symbol; }

  /// Conversion Operator
  operator bool() const { return m_Symbol != nullptr; }

  ///
  /// Operators
  ///
  bool operator()(const Symbol &A, const Symbol &B) const {
    return A.m_Symbol != B.m_Symbol;
  }

  bool operator<(const Symbol &RHS) const { return m_Symbol < RHS.m_Symbol; }

  bool operator>(const Symbol &RHS) const { return m_Symbol > RHS.m_Symbol; }

  bool operator==(const Symbol &RHS) const { return m_Symbol == RHS.m_Symbol; }

private:
  /// Internal Data structure
  mcld::ResolveInfo *m_Symbol;
};

/// InputFile represents a ObjectFile that is processed by the Linker.
struct DLL_A_EXPORT InputFile {
  explicit InputFile(mcld::InputFile *I) : m_InputFile(I) {}

  /// Get the Filename of the input File.
  std::string getFileName() const;

  /// Do we have an input file ?
  bool hasInputFile() const;

  /// Check if the file is an archive file.
  bool isArchive() const;

  /// Get the member name of the Input File.
  std::string getMemberName() const;

  /// Get symbols from the Input File.
  std::vector<QCLD::Symbol> getSymbols() const;

  /// Conversion Operator
  operator bool() const { return m_InputFile != nullptr; }

  ///
  /// Operators
  ///
  bool operator()(const InputFile &A, const InputFile &B) const {
    return A.m_InputFile != B.m_InputFile;
  }

  bool operator<(const InputFile &RHS) const {
    return m_InputFile < RHS.m_InputFile;
  }

  bool operator>(const InputFile &RHS) const {
    return m_InputFile > RHS.m_InputFile;
  }

  bool operator==(const InputFile &RHS) const {
    return m_InputFile == RHS.m_InputFile;
  }

private:
  mcld::InputFile *m_InputFile;
};

/// Plugin Data is a way for storing data and communicating between plugins.
struct DLL_A_EXPORT PluginData {
  explicit PluginData(mcld::PluginData *D) : m_Data(D) {}

  void *getData() const;

  uint32_t getKey() const;

  std::string getAnnotation() const;

  /// Conversion Operator
  operator bool() const { return m_Data != nullptr; }

  ///
  /// Operators
  ///
  bool operator()(const PluginData &A, const PluginData &B) const {
    return A.m_Data != B.m_Data;
  }

  bool operator<(const PluginData &RHS) const { return m_Data < RHS.m_Data; }

  bool operator>(const PluginData &RHS) const { return m_Data > RHS.m_Data; }

  bool operator==(const PluginData &RHS) const { return m_Data == RHS.m_Data; }

private:
  mcld::PluginData *m_Data;
};

/// Plugin Profiling support
struct DLL_A_EXPORT AutoTimer {
  explicit AutoTimer(llvm::Timer *T);

  /// Start Timer
  void start();

  /// End Timer
  void end();

  ~AutoTimer();

  /// Conversion Operator
  operator bool() const { return m_Timer != nullptr; }

  ///
  /// Operators
  ///
  bool operator()(const AutoTimer &A, const AutoTimer &B) const {
    return A.m_Timer != B.m_Timer;
  }

  bool operator<(const AutoTimer &RHS) const { return m_Timer < RHS.m_Timer; }

  bool operator>(const AutoTimer &RHS) const { return m_Timer > RHS.m_Timer; }

  bool operator==(const AutoTimer &RHS) const { return m_Timer == RHS.m_Timer; }

private:
  llvm::Timer *m_Timer;
};

struct DLL_A_EXPORT Timer {
  Timer(llvm::Timer *T);

  /// Start Timer
  void start();

  /// End Timer
  void end();

  ~Timer();

  /// Conversion Operator
  operator bool() const { return m_Timer != nullptr; }

  ///
  /// Operators
  ///
  bool operator()(const Timer &A, const Timer &B) const {
    return A.m_Timer != B.m_Timer;
  }

  bool operator<(const Timer &RHS) const { return m_Timer < RHS.m_Timer; }

  bool operator>(const Timer &RHS) const { return m_Timer > RHS.m_Timer; }

  bool operator==(const Timer &RHS) const { return m_Timer == RHS.m_Timer; }

private:
  llvm::Timer *m_Timer;
};

/// A class that the plugin can use to handle relocations.
struct DLL_A_EXPORT RelocationHandler {

  RelocationHandler(mcld::Relocator *R);

  /// Get Relocation Type for a relocation.
  uint32_t getRelocationType(std::string Name) const;

  /// Get the Relocation name from its type.
  std::string getRelocationName(uint32_t Type) const;

  ~RelocationHandler();

  /// Conversion Operator
  operator bool() const { return Relocator != nullptr; }

  ///
  /// Operators
  ///
  bool operator()(const RelocationHandler &A,
                  const RelocationHandler &B) const {
    return A.Relocator != B.Relocator;
  }

  bool operator<(const RelocationHandler &RHS) const {
    return Relocator < RHS.Relocator;
  }

  bool operator>(const RelocationHandler &RHS) const {
    return Relocator > RHS.Relocator;
  }

  bool operator==(const RelocationHandler &RHS) const {
    return Relocator == RHS.Relocator;
  }

private:
  mcld::Relocator *Relocator;
};

}; // namespace QCLD

#endif
