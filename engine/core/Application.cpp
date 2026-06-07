#include <engine/core/Application.h>
#include <engine/core/Logger.h>
#include <engine/platform/Platform.h>
#include <engine/window/Window.h>

#include <iostream>

namespace Walrus
{
    Application::Application(ApplicationSpecification specs)
        : m_Specification(specs)
    {
    }

    Application::~Application() = default;

    void Application::Run()
    {
        WFATAL("A test message: %f", 3.14f);
        WERROR("A test message: %f", 3.14f);
        WWARN("A test message: %f", 3.14f);
        WINFO("A test message: %f", 3.14f);
        WDEBUG("A test message: %f", 3.14f);
        WTRACE("A test message: %f", 3.14f);

        m_Running = true;
        m_Window = std::make_unique<Window>(
            m_Specification.Name, m_Specification.StartX, m_Specification.StartY, m_Specification.Width, m_Specification.Height);

        if (!OnInitialize())
            return;

        m_LastTime = Platform::GetAbsoluteTime();

        while (m_Running)
        {
            if (!m_Window->PumpMessages())
            {
                m_Running = false;
                break;
            }

            const f64 currentTime = Platform::GetAbsoluteTime();
            const f32 deltaTime = static_cast<f32>(currentTime - m_LastTime);
            m_LastTime = currentTime;

            if (!m_Suspended)
            {
                if (!OnUpdate(deltaTime))
                    break;

                if (!OnRender(deltaTime))
                    break;
            }
        }
    }
} // namespace Walrus
