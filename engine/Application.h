#pragma once

#include "Defines.h"

namespace Walrus
{
    struct ApplicationCommandLineArgs
    {
        i32 Count = 0;
        char** Args = nullptr;
    };

    class Application
    {
    public:
        explicit Application(ApplicationCommandLineArgs args = {});
        virtual ~Application();

        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;

        virtual void Run();

    protected:
        ApplicationCommandLineArgs Args;
    };
} // namespace Walrus
