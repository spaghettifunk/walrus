#include <engine/window/Window.h>

#if WPLATFORM_APPLE

namespace Walrus
{
    struct Window::WindowState
    {
        bool initialized = false;
    };

    Window::Window(std::string, i32, i32, i32, i32)
        : m_State(std::make_unique<WindowState>())
    {
        m_State->initialized = true;
    }

    Window::~Window() = default;

    bool Window::PumpMessages()
    {
        return false;
    }
} // namespace Walrus

#endif
