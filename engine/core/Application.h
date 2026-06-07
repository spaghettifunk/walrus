#pragma once

#include <engine/Defines.h>
#include <engine/platform/Platform.h>

namespace Walrus
{
    struct ApplicationSpecification
    {
        std::string Name = "Walrus";
        i32 StartX = 100;
        i32 StartY = 100;
        i32 Width = 1280;
        i32 Height = 720;
    };

    class WAPI Application
    {
    public:
        explicit Application(ApplicationSpecification specification);
        virtual ~Application();

        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;

        virtual void Run();

    protected:
        virtual bool OnInitialize()
        {
            return true;
        }
        virtual bool OnUpdate(f32)
        {
            return true;
        }
        virtual bool OnRender(f32)
        {
            return true;
        }
        virtual void OnResize(u32, u32)
        {
        }

    private:
        ApplicationSpecification m_Specification;
        std::unique_ptr<Platform> m_Platform;
        bool m_Running = false;
        bool m_Suspended = false;
        f64 m_LastTime = 0.0;
    };
} // namespace Walrus
