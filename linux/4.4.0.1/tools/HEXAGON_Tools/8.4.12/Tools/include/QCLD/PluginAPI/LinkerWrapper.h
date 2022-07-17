//===- LinkerWrapper.h ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef LINKER_WRAPPER_H
#define LINKER_WRAPPER_H

#include "PluginADT.h"
#include <system_error>
#include <unordered_map>
#include <vector>

namespace llvm {
class Timer;
}

namespace mcld {
class Module;
class Section;
class Plugin;
} // namespace mcld

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

// This is a Thin Linker Wrapper. The plugin calls the functions that are part
// of the Linker wrapper only.
class DLL_A_EXPORT LinkerWrapper {
public:
  enum State {
    Unknown,
    Initializing,
    BeforeLayout,
    CreatingSections,
    AfterLayout
  };

  /// Construct LinkerWrapper object.
  explicit LinkerWrapper(mcld::Plugin *, mcld::Module *);

  /// Any memory that needs to live for the lifetime of the linker should use
  /// function. This function allocates memory and returns an error if memory
  /// cannot be allocated.
  std::error_code allocateMemory(std::string PluginName, uint32_t Sz,
                                 uint32_t &AllocatedSize, uint8_t **Buf);

  //  This returns a vector of uses that are referred from the Chunk.
  std::vector<Use> getUses(Chunk &C);

  /// This function can be called by the SectionIterator, and
  /// returns a vector of uses that are referred from the Chunk.
  std::vector<Use> getUses(Section &S);

  /// This returns a linker symbol for inspection for a symbol that the user
  /// wants to lookup.
  Symbol getSymbol(std::string Sym) const;

  /// This allows the plugin to override a output section for Section.
  void setOutputSection(Section &S, std::string OutputSection,
                        std::string Annotation = "");

  /// The plugin can query the linker script to see what output section does a
  /// Section match.
  OutputSection getOutputSection(Section &S) const;

  /// The plugin can query the linker script if it defines an OutputSection.
  OutputSection getOutputSection(std::string OutputSection);

  /// Finish assigning output sections.
  bool finishAssignOutputSections();

  /// The plugin can set the linker to fatally error, if it recognizes that a
  /// dependency or the plugin doesnot capture all the requirements of the
  /// functionality.
  void setLinkerFatalError();

  // Get the linker plugin associated with the wrapper.
  mcld::Plugin *getPlugin() const { return m_Plugin; }

  /// Match pattern against a string.
  bool matchPattern(const std::string &Pattern, const std::string &Name) const;

  // Destroy the Linker Wrapper.
  ~LinkerWrapper();

  // Get the state of the
  State getState() const;

  // Add a symbol to the Chunk.
  void addSymbolToChunk(Chunk &C, std::string Symbol, uint64_t Val);

  // Create a Padding Chunk.
  Chunk createPaddingChunk(uint32_t Alignment, size_t PaddingSize);

  // Replace contents of a chunk.
  enum DLL_A_EXPORT ReplaceStatus : uint8_t {
    InvalidHandle,
    InvalidSymbol,
    NoChunkForSymbol,
    ChunkIsBSS,
    SymbolIsSmall,
    Ok,
  };

  ReplaceStatus replaceSymbolContent(QCLD::Symbol S, const uint8_t *Buf,
                                     size_t Sz);

  void addChunk(QCLD::LinkerScriptRule &Rule, QCLD::Chunk &C,
                std::string A = "");

  void removeChunk(QCLD::LinkerScriptRule &Rule, QCLD::Chunk &C,
                   std::string A = "");

  void updateChunks(QCLD::LinkerScriptRule &Rule, std::vector<QCLD::Chunk> &V,
                    std::string A = "");

  // Return Linker and LLVM repository versions.
  std::string getRepositoryVersion() const;

  /// Record Plugin Data from a plugin.
  void recordPluginData(uint32_t Key, void *Data, std::string Annotation);

  /// Return the recorded Data for the plugin
  std::vector<PluginData> getPluginData(std::string PluginName);

  /// Profiling Plugin support
  llvm::Timer *CreateTimer(std::string Name, std::string Description,
                           bool IsEnabled);

  /// Register a relocation for the plugin to call back.
  bool registerReloc(uint32_t RelocType, std::string Name = "");

  /// Relocation Handler.
  RelocationHandler getRelocationHandler() const;

  /// Is Multi-threaded ?
  bool isMultiThreaded() const;

private:
  mcld::Module *m_Module;
  mcld::Plugin *m_Plugin;
};
} // namespace QCLD

#endif
