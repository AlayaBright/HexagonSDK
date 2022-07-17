//===- LinkerPluginConfig.h -----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef LINKER_PLUGIN_CONFIG_H
#define LINKER_PLUGIN_CONFIG_H

#include "PluginADT.h"
#include <string>
#include <vector>

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
#endif

class DLL_A_EXPORT Plugin;
struct DLL_A_EXPORT Use;

class DLL_A_EXPORT LinkerPluginConfig {
public:
  explicit LinkerPluginConfig(QCLD::Plugin *);

  virtual void Init() = 0;

  virtual void RelocCallBack(QCLD::Use U) = 0;

  QCLD::Plugin *getPlugin() const { return Plugin; }

  virtual ~LinkerPluginConfig() {}

private:
  QCLD::Plugin *Plugin;
};

} // namespace QCLD
#endif
