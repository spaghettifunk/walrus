#include <engine/Application.h>
#include <engine/core/Logger.h>

#include <iostream>

namespace Walrus
{
    Application::Application(ApplicationCommandLineArgs args)
        : Args(args)
    {
    }

    Application::~Application() = default;

    void Application::Run()
    {
    }
} // namespace Walrus
