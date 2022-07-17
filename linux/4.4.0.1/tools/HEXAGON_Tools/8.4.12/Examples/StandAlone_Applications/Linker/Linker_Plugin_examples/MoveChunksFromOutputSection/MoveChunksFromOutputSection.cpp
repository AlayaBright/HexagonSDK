#include "OutputSectionIteratorPlugin.h"
#include <algorithm>
#include <assert.h>
#include <cstring>
#include <iostream>
#include <string>
#include <unordered_map>

using namespace QCLD;

class InputChunk {
public:
  InputChunk(Chunk S) : Chunk(S) {}
  Chunk Chunk;
};

class DLL_A_EXPORT OSIter : public OutputSectionIteratorPlugin {

public:
  OSIter()
      : OutputSectionIteratorPlugin("GETOUTPUTCHUNKS"),
        RedistributeSection(nullptr), Hot(nullptr), Cold(nullptr),
        Unlikely(nullptr) {}

  // Perform any initialization here
  void Init(std::string Options) override {}

  void processOutputSection(OutputSection O) override {
    if (Linker->getState() == LinkerWrapper::AfterLayout) {
      std::cout << "Size of " << O.getName() << "\t" << O.getSize() << "\n";
      return;
    }
    if (Linker->getState() != LinkerWrapper::CreatingSections)
      return;

    if (O.getName() == ".redistribute")
      RedistributeSection = O;

    if (O.getName() == ".hot")
      Hot = O;

    if (O.getName() == ".cold")
      Cold = O;

    if (O.getName() == ".unlikely")
      Unlikely = O;
  }

  size_t getSize(std::vector<Chunk> &C) {
    size_t Size = 0;
    for (auto Chunk : C) {
      // Adjust the current size to the required alignment if needed
      uint64_t Align = Chunk.getAlignment();
      if (Align > 1) {
        assert(!(Align & (Align - 1)) && "Alignment is not a power of two!");
        Size = (Size + Align - 1) & ~(Align - 1);
      }
      Size += Chunk.getSize();
    }
    return Size;
  }

  // After the linker lays out the image, but before it creates the elf file,
  // it will call this run function.
  Status Run(bool Trace) override {
    if (Linker->getState() == LinkerWrapper::AfterLayout)
      return QCLD::Plugin::Status::SUCCESS;
    if (Linker->getState() != LinkerWrapper::CreatingSections)
      return QCLD::Plugin::Status::SUCCESS;

    QCLD::LinkerScriptRule HotRule = Hot.getLinkerScriptRules().front();
    QCLD::LinkerScriptRule ColdRule = Cold.getLinkerScriptRules().front();
    QCLD::LinkerScriptRule UnlikelyRule =
        Unlikely.getLinkerScriptRules().front();
    std::vector<Chunk> HotChunks, ColdChunks, UnlikelyChunks;
    for (auto &R : RedistributeSection.getLinkerScriptRules()) {
      std::vector<Chunk> AllChunks = R.getChunks();
      for (auto &C : AllChunks) {
        std::string ChunkName = C.getName();
        if (ChunkName.find("hot") != std::string::npos) {
          HotChunks.push_back(C);
          Linker->removeChunk(R, C);
        }
        if (ChunkName.find("cold") != std::string::npos) {
          ColdChunks.push_back(C);
          Linker->removeChunk(R, C);
        }
        if (ChunkName.find("unlikely") != std::string::npos) {
          UnlikelyChunks.push_back(C);
          Linker->removeChunk(R, C);
        }
      }
    }
    std::cerr << HotRule.asString() << "\t" << getSize(HotChunks) << "\n";
    std::cerr << ColdRule.asString() << "\t" << getSize(ColdChunks) << "\n";
    std::cerr << UnlikelyRule.asString() << "\t" << getSize(UnlikelyChunks)
              << "\n";
    Linker->updateChunks(HotRule, HotChunks);
    Linker->updateChunks(ColdRule, ColdChunks);
    Linker->updateChunks(UnlikelyRule, UnlikelyChunks);

    return Plugin::Status::SUCCESS;
  }

  void Destroy() override {}

  uint32_t GetLastError() override { return 0; }

  std::string GetLastErrorAsString() override { return "SUCCESS"; }

  std::string GetName() override { return "GETOUTPUTCHUNKS"; }

private:
  QCLD::OutputSection RedistributeSection;
  QCLD::OutputSection Hot;
  QCLD::OutputSection Cold;
  QCLD::OutputSection Unlikely;
  std::unordered_map<std::string, std::vector<InputChunk>> OutputSectionInfo;
};

std::unordered_map<std::string, Plugin *> Plugins;

Plugin *ThisPlugin = nullptr;

extern "C" {
bool DLL_A_EXPORT RegisterAll() {
  ThisPlugin = new OSIter();
  return true;
}
Plugin DLL_A_EXPORT *getPlugin(const char *T) { return ThisPlugin; }

void DLL_A_EXPORT Cleanup() {
  if (ThisPlugin)
    delete ThisPlugin;
  ThisPlugin = nullptr;
}
}
