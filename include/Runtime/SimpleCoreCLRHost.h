/*
 *  Copyright (c) Hubert Jarosz. All rights reserved.
 *  Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

#ifndef SIMPLECORECLRHOST_HPP
#define SIMPLECORECLRHOST_HPP

#include "coreclr.h"
#include <memory>
#include <string>

class SimpleCoreCLRHost
{
public:
        SimpleCoreCLRHost();
        bool RunFromEntryPoint(
                const std::string& currentExePath,
                const std::string& clrFilesAbsolutePath,
                const std::string& managedAssemblyAbsoluteDir,
                const std::string& assemblyName,
                const std::string& entryPointType,
                const std::string& entryPointName);
        std::unique_ptr<CoreCLR> clr;
};

#endif