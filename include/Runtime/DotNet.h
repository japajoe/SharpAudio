#ifndef DOTNET_HPP
#define DOTNET_HPP

#include "SimpleCoreCLRHost.h"

class DotNet
{
public:
    bool InitializeDotNetRuntime(char* argv);
    SimpleCoreCLRHost* GetHost() { return &host; }
private:
    SimpleCoreCLRHost host;
};

#endif