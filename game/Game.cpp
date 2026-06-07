#include <game/Game.h>

#include <iostream>

namespace Walrus
{
    bool GameApplication::OnInitialize()
    {
        return true;
    }

    bool GameApplication::OnUpdate(f32 deltaTime)
    {
        m_State->DeltaTime = deltaTime;
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
        auto* game = Walrus::New<GameApplication>(Walrus::MemoryTag::MEMORY_TAG_GAME, specs);
        return game;
    }
} // namespace Walrus
