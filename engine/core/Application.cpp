#include <engine/core/Application.h>
#include <engine/core/Logger.h>

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
        m_Platform = std::make_unique<Platform>(
            m_Specification.Name, m_Specification.StartX, m_Specification.StartY, m_Specification.Width, m_Specification.Height);

        if (!OnInitialize())
            return;

        while (m_Running)
        {
            if (!m_Platform->PumpMessages())
            {
                m_Running = false;
                break;
            }

            // const f32 deltaTime = CalculateDeltaTime();

            if (!m_Suspended)
            {
                if (!OnUpdate(0))
                    break;

                if (!OnRender(0))
                    break;
            }
        }
    }
} // namespace Walrus
