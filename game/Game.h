#pragma once

#include <engine/EntryPoint.h>
#include <engine/core/Logger.h>
#include <engine/core/Memory.h>

namespace Walrus
{
    class GameApplication : public Application
    {
    public:
        explicit GameApplication(ApplicationSpecification specs)
            : Application(specs)
        {
            m_State = static_cast<GameState*>(Walrus::Memory::Allocate(sizeof(GameState), Walrus::MemoryTag::MEMORY_TAG_GAME));
        }

        ~GameApplication() override
        {
            Walrus::Memory::Free(m_State, sizeof(GameState), Walrus::MemoryTag::MEMORY_TAG_GAME);
        }

    protected:
        bool OnInitialize() override;
        bool OnUpdate(f32 deltaTime) override;
        bool OnRender(f32 deltaTime) override;
        void OnResize(u32 width, u32 height) override;

    private:
        struct GameState
        {
            f32 DeltaTime = 0.0f;
        };
        GameState* m_State = nullptr;
    };
} // namespace Walrus
