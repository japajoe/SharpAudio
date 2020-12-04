#include "Program.h"
#include "SharpAudioAPI.h"
#include <memory>

int main(int argc, char** argv)
{
    if(SharpAudioAPI::Initialize(argv[0]))
    {
        auto application = std::make_unique<Program>();
        application->Run(argc, argv);
        SharpAudioAPI::StopApplication();
    }

    return 0;
}
