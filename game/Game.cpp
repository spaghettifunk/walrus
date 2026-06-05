#include "../engine/EntryPoint.h"

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
        std::cout << "Game started through the Walrus engine." << std::endl;
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
