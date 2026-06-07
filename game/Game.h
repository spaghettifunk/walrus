#pragma once

#include <engine/EntryPoint.h>
#include <engine/core/Logger.h>

namespace Walrus
{
    class GameApplication : public Application
    {
    public:
        explicit GameApplication(ApplicationSpecification specs)
            : Application(specs)
        {
        }

    protected:
        bool OnInitialize() override;
        bool OnUpdate(f32 deltaTime) override;
        bool OnRender(f32 deltaTime) override;
        void OnResize(u32 width, u32 height) override;
    };
} // namespace Walrus
