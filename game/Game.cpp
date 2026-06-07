#include <game/Game.h>

#include <iostream>

namespace Walrus
{
    bool GameApplication::OnInitialize()
    {
        return true;
    }

    bool GameApplication::OnUpdate(f32)
    {
        return true;
    }

    bool GameApplication::OnRender(f32)
    {
        return true;
    }

    void GameApplication::OnResize(u32, u32)
    {
    }

    Application* CreateApplication(ApplicationSpecification specs)
    {
        return new GameApplication(specs);
    }
} // namespace Walrus
