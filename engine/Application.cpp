#include "Application.h"

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
        std::cout << "Walrus engine running with " << Args.Count << " argument(s)." << std::endl;
    }
} // namespace Walrus
