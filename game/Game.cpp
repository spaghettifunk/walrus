#include <engine/EntryPoint.h>
#include <engine/core/Logger.h>

#include <iostream>

class GameApplication final : public Walrus::Application
{
public:
    explicit GameApplication(Walrus::ApplicationCommandLineArgs args)
        : Walrus::Application(args)
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

        Walrus::Application::Run();
    }
};

namespace Walrus
{
    Application* CreateApplication(ApplicationCommandLineArgs args)
    {
        return new GameApplication(args);
    }
} // namespace Walrus
