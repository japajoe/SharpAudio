/*
 *  Copyright (c) Hubert Jarosz. All rights reserved.
 *  Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

#ifndef CORECLR_HPP
#define CORECLR_HPP

#include <memory>

namespace DynamicLinker
{
class DynamicLinker;
}

struct CoreclrHandle
{
    void* hostHandle{nullptr};
    unsigned int domainId{0};
};

class CoreCLR
{
private:
    std::shared_ptr<DynamicLinker::DynamicLinker> dl;
    CoreclrHandle coreclrHandle{};

    void initializeCoreCLRDynamicLibrary(const std::string&);
    void initializeCoreCLR(const std::string&, const std::string&, const std::string&);
public:
    explicit CoreCLR(const std::string&, const std::string&, const std::string&, const std::string&);
    ~CoreCLR();

    void* getCSharpFunctionPtr(const std::string&, const std::string&, const std::string&);
};

#endif