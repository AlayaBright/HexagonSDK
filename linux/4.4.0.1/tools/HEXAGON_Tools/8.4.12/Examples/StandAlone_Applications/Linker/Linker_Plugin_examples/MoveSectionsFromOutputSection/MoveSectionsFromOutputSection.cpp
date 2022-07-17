#include "OutputSectionIteratorPlugin.h"
#include <algorithm>
#include <assert.h>
#include <cstring>
#include <iostream>
#include <string>
#include <unordered_map>

using namespace QCLD;

class InputSection {
public:
  InputSection(Section S) : Section(S) {}
  Section Section;
  bool isMoved = false;
};

class DLL_A_EXPORT OSIter : public OutputSectionIteratorPlugin {

public:
  OSIter() : OutputSectionIteratorPlugin("GETOUTPUT") {}

  // Perform any initialization here
  void Init(std::string Options) override {}

  void processOutputSection(OutputSection O) override {
    if (Linker->getState() == LinkerWrapper::AfterLayout) {
      std::cout << "Size of " << O.getName() << "\t" << O.getSize() << "\n";
      return;
    }
    if (Linker->getState() == LinkerWrapper::CreatingSections)
      return;

    if (O.getName() != ".redistribute")
      return;

    for (auto &R : O.getLinkerScriptRules()) {
      for (auto &S : R.getSections())
        OutputSectionInfo[O.getName()].push_back(InputSection(S));
    }
  }

  size_t GetSize(std::string Name) {
    size_t Size = 0;
    for (auto &InS : OutputSectionInfo[Name])
      if (!InS.isMoved) {
        Section &S = InS.Section;
        // Adjust the current size to the required alignment if needed
        uint64_t Align = S.getAlignment();
        if (Align > 1) {
          assert(!(Align & (Align - 1)) && "Alignment is not a power of two!");
          Size = (Size + Align - 1) & ~(Align - 1);
        }
        Size += S.getSize();
      }
    return Size;
  }

  // After the linker lays out the image, but before it creates the elf file,
  // it will call this run function.
  Status Run(bool Trace) override {

    if (Linker->getState() == LinkerWrapper::AfterLayout)
      return QCLD::Plugin::Status::SUCCESS;
    if (Linker->getState() == LinkerWrapper::CreatingSections)
      return QCLD::Plugin::Status::SUCCESS;
    for (auto &O : OutputSectionInfo) {
      for (auto &I : O.second) {
        Section S = I.Section;
        std::string SectionName = S.getName();
        if (SectionName.find("hot") != std::string::npos) {
          Linker->setOutputSection(S, ".hot");
          I.isMoved = true;
        }
        if (SectionName.find("cold") != std::string::npos) {
          Linker->setOutputSection(S, ".cold");
          I.isMoved = true;
        }
        if (SectionName.find("unlikely") != std::string::npos) {
          Linker->setOutputSection(S, ".unlikely");
          I.isMoved = true;
        }
      }
    }
    std::cout << "Size of output Section " << GetSize(".redistribute") << "\n";
    Linker->finishAssignOutputSections();
    return Plugin::Status::SUCCESS;
  }

  void Destroy() override {}

  uint32_t GetLastError() override { return 0; }

  std::string GetLastErrorAsString() override { return "SUCCESS"; }

  std::string GetName() override { return "GETOUTPUT"; }

private:
  std::vector<OutputSection> OutputSections;
  std::unordered_map<std::string, std::vector<InputSection>> OutputSectionInfo;
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
