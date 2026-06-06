#include <engine/EntryPoint.h>
#include <engine/core/Logger.h>
#include <engine/platform/Platform.h>

#include <iostream>

using namespace Walrus;

class GameApplication final : public Application
{
public:
    explicit GameApplication(ApplicationCommandLineArgs args)
        : Application(args)
    {
    }

    void Run() override
    {
        WFATAL("A test message: %f", 3.14f);
        WERROR("A test message: %f", 3.14f);
        WWARN("A test message: %f", 3.14f);
        WINFO("A test message: %f", 3.14f);
        WDEBUG("A test message: %f", 3.14f);
        WTRACE("A test message: %f", 3.14f);

        Platform platform("Walrus Engine Test", 100, 100, 1280, 720);

        while (platform.PumpMessages())
        {
        }

        Application::Run();
    }
};

namespace Walrus
{
    Application* CreateApplication(ApplicationCommandLineArgs args)
    {
        return new GameApplication(args);
    }
} // namespace Walrus
