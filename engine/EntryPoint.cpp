#include <engine/EntryPoint.h>
#include <engine/core/Logger.h>
#include <engine/core/Memory.h>

#include <iostream>

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    Walrus::Memory::Initialize();

    auto* application = Walrus::CreateApplication(
        Walrus::ApplicationSpecification{
            .Name = "Walrus Engine Test",
            .StartX = 100,
            .StartY = 100,
            .Width = 1280,
            .Height = 720,
        });
    if (!application)
    {
        WFATAL("Walrus: CreateApplication returned null.");
        Walrus::Memory::Shutdown();
        return 1;
    }

    application->Run();
    Walrus::Delete(application, Walrus::MemoryTag::MEMORY_TAG_GAME);

    Walrus::Memory::Shutdown();

    return 0;
}
