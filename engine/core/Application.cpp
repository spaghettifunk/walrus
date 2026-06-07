#include <engine/core/Application.h>
#include <engine/core/Logger.h>
#include <engine/core/Memory.h>
#include <engine/platform/Platform.h>
#include <engine/window/Window.h>

#include <iostream>

namespace Walrus
{
    Application::Application(ApplicationSpecification specs)
        : m_Specification(specs)
    {
        WFATAL("A test message: {}", 3.14f);
        WERROR("A test message: {}", 3.14f);
        WWARN("A test message: {}", 3.14f);
        WINFO("A test message: {}", 3.14f);
        WDEBUG("A test message: {}", 3.14f);
        WTRACE("A test message: {}", 3.14f);

        m_Running = true;
        m_Suspended = false;

        m_Window = Walrus::New<Window>(
            Walrus::MemoryTag::MEMORY_TAG_APPLICATION,
            m_Specification.Name,
            m_Specification.StartX,
            m_Specification.StartY,
            m_Specification.Width,
            m_Specification.Height);

        m_Initialized = true;
    }

    Application::~Application() = default;

    void Application::Run()
    {
        WINFO(Memory::GetMemoryUsageStr());

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

        m_Running = false;

        Walrus::Delete(m_Window, Walrus::MemoryTag::MEMORY_TAG_APPLICATION);
    }
} // namespace Walrus
