#include "DotNet.h"
#include <vector>
#include <string>
#include <experimental/filesystem>
#include <algorithm>
#include "strnatcmp.h"

namespace fs = std::experimental::filesystem;

bool DotNet::InitializeDotNetRuntime(char* argv)
{
    std::vector<std::string> directories;
	std::string dir = "/usr/share/dotnet/shared/Microsoft.NETCore.App";
    for(auto& p : fs::recursive_directory_iterator(dir))
    {
        if (fs::is_directory(p))
            directories.push_back(p.path().string());
    }

	if(directories.size() == 0)
		return false;

	int index = 0;

	if(directories.size() == 1)
	{
		index = 0;
	}
	else if(directories.size() > 1)
	{
		//Get latest version by sorting directories by version, latest version is last index
		std::sort(directories.begin(), directories.end(), compareNat);
		index = directories.size() - 1;
	}
	
    bool init = false;
    std::string pathToEXE = std::string(argv);
	std::string clrDir = directories[index];
    std::string cwd = fs::current_path();
    std::string assemblyDir = cwd + "/";
    std::string assemblyName = "SharpAudio";
    std::string entryPointType = "SharpAudio.Program";
    std::string entryPointName = "Initialize";
    
    init = host.RunFromEntryPoint(
            pathToEXE, // path to this exe
            clrDir, // absolute path to coreCLR DLLs
            assemblyDir, // absolute path to DLL to run
            assemblyName,
            entryPointType,
            entryPointName);

    if(!init)
        return false;

    return true;
}