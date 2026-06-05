#include <engine/EntryPoint.h>
#include <engine/core/Logger.h>

#include <iostream>

int main(int argc, char** argv)
{
    auto* application = Walrus::CreateApplication({ argc, argv });
    if (!application)
    {
        WFATAL("Walrus: CreateApplication returned null.");
        return 1;
    }

    application->Run();
    delete application;

    return 0;
}
