#pragma once

#include <engine/Defines.h>

#include <memory>
#include <string>

namespace Walrus
{
    class WAPI Window
    {
    public:
        Window(std::string applicationName, i32 x, i32 y, i32 width, i32 height);
        ~Window();

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        bool PumpMessages();

    private:
        struct WindowState;
        std::unique_ptr<WindowState> m_State;
    };
} // namespace Walrus
