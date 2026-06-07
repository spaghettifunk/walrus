#include <engine/EntryPoint.h>
#include <engine/core/Logger.h>

#include <iostream>

int main(int argc, char** argv)
{
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
        return 1;
    }

    application->Run();
    delete application;

    return 0;
}
