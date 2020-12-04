/*
 *  Copyright (c) Hubert Jarosz. All rights reserved.
 *  Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

#include "SimpleCoreCLRHost.h"

#include "DynamicLinker.h"
#include <climits>
#ifndef PATH_MAX
#   define PATH_MAX 4096
#endif



#if defined(__APPLE__)
constexpr char coreClrDll[] = "libcoreclr.dylib";
#else
constexpr char coreClrDll[] = "libcoreclr.so";
#endif




SimpleCoreCLRHost::SimpleCoreCLRHost()
{

}

bool SimpleCoreCLRHost::RunFromEntryPoint(
            const std::string& currentExePath,
            const std::string& clrFilesAbsolutePath,
            const std::string& managedAssemblyAbsoluteDir,
            const std::string& assemblyName,
            const std::string& entryPointType,
            const std::string& entryPointName)
{

  std::string coreClrDllPath = clrFilesAbsolutePath + "/" + coreClrDll;
  if ( coreClrDllPath.size() >= PATH_MAX ) {
      std::cerr << "Path to libcoreclr.so too long!" << std::endl;
      return false;
  }

  clr = std::make_unique<CoreCLR>(coreClrDllPath, managedAssemblyAbsoluteDir, clrFilesAbsolutePath, currentExePath);

  

  //hello = reinterpret_cast<sayHello1_ptr>(handle);

  /*
   *  If arguments are in a different order then second arg is 0 in C#.
   *  probably something with padding/offset/ptr byte size
   */
  return true;

}
