#include <engine/window/Window.h>

// Windows window layer.
#if WPLATFORM_WINDOWS

#include <engine/core/Logger.h>

#include <cstring>
#include <windows.h>
#include <windowsx.h> // param input extraction

namespace Walrus
{
    struct Window::WindowState
    {
        HINSTANCE hInstance = nullptr;
        HWND hwnd = nullptr;
    };

    LRESULT CALLBACK Win32ProcessMessage(HWND hwnd, u32 msg, WPARAM wParam, LPARAM lParam);

    Window::Window(std::string applicationName, i32 x, i32 y, i32 width, i32 height)
        : m_State(std::make_unique<WindowState>())
    {
        m_State->hInstance = GetModuleHandleA(0);

        // Setup and register window class.
        HICON icon = LoadIcon(m_State->hInstance, IDI_APPLICATION);
        WNDCLASSA wc;
        memset(&wc, 0, sizeof(wc));
        wc.style = CS_DBLCLKS; // Get double-clicks
        wc.lpfnWndProc = Win32ProcessMessage;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = m_State->hInstance;
        wc.hIcon = icon;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW); // NULL; // Manage the cursor manually
        wc.hbrBackground = NULL; // Transparent
        wc.lpszClassName = "walrus_window_class";

        if (!RegisterClassA(&wc))
        {
            MessageBoxA(0, "Window registration failed", "Error", MB_ICONEXCLAMATION | MB_OK);
        }

        // Create window
        u32 clientX = x;
        u32 clientY = y;
        u32 clientWidth = width;
        u32 clientHeight = height;

        u32 windowX = clientX;
        u32 windowY = clientY;
        u32 windowWidth = clientWidth;
        u32 windowHeight = clientHeight;

        u32 windowStyle = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION;
        u32 windowExStyle = WS_EX_APPWINDOW;

        windowStyle |= WS_MAXIMIZEBOX;
        windowStyle |= WS_MINIMIZEBOX;
        windowStyle |= WS_THICKFRAME;

        // Obtain the size of the border.
        RECT borderRect = { 0, 0, 0, 0 };
        AdjustWindowRectEx(&borderRect, windowStyle, 0, windowExStyle);

        // In this case, the border rectangle is negative.
        windowX += borderRect.left;
        windowY += borderRect.top;

        // Grow by the size of the OS border.
        windowWidth += borderRect.right - borderRect.left;
        windowHeight += borderRect.bottom - borderRect.top;

        HWND handle = CreateWindowExA(
            windowExStyle,
            "walrus_window_class",
            applicationName.c_str(),
            windowStyle,
            windowX,
            windowY,
            windowWidth,
            windowHeight,
            0,
            0,
            m_State->hInstance,
            0);

        if (handle == 0)
        {
            MessageBoxA(NULL, "Window creation failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);

            WFATAL("Window creation failed!");
        }
        else
        {
            m_State->hwnd = handle;
        }

        // Show the window
        bool shouldActivate = true; // TODO: if the window should not accept input, this should be false.
        i32 showWindowCommandFlags = shouldActivate ? SW_SHOW : SW_SHOWNOACTIVATE;
        // If initially minimized, use SW_MINIMIZE : SW_SHOWMINNOACTIVE;
        // If initially maximized, use SW_SHOWMAXIMIZED : SW_MAXIMIZE
        ShowWindow(m_State->hwnd, showWindowCommandFlags);
    }

    Window::~Window()
    {
        if (m_State->hwnd)
        {
            DestroyWindow(m_State->hwnd);
            m_State->hwnd = 0;
        }
    }

    bool Window::PumpMessages()
    {
        MSG message;
        while (PeekMessageA(&message, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessageA(&message);
        }

        return true;
    }

    LRESULT CALLBACK Win32ProcessMessage(HWND hwnd, u32 msg, WPARAM wParam, LPARAM lParam)
    {
        switch (msg)
        {
        case WM_ERASEBKGND:
            // Notify the OS that erasing will be handled by the application to prevent flicker.
            return 1;
        case WM_CLOSE:
            // TODO: Fire an event for the application to quit.
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_SIZE:
            {
                // Get the updated size.
                // RECT r;
                // GetClientRect(hwnd, &r);
                // u32 width = r.right - r.left;
                // u32 height = r.bottom - r.top;

                // TODO: Fire an event for window resize.
            }
            break;
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP:
            {
                // Key pressed/released
                // bool pressed = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN);
                // TODO: input processing
            }
            break;
        case WM_MOUSEMOVE:
            {
                // Mouse move
                // i32 x_position = GET_X_LPARAM(lParam);
                // i32 y_position = GET_Y_LPARAM(lParam);
                // TODO: input processing.
            }
            break;
        case WM_MOUSEWHEEL:
            {
                // i32 z_delta = GET_WHEEL_DELTA_WPARAM(wParam);
                // if (z_delta != 0) {
                //     // Flatten the input to an OS-independent (-1, 1)
                //     z_delta = (z_delta < 0) ? -1 : 1;
                //     // TODO: input processing.
                // }
            }
            break;
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP:
            {
                // bool pressed = msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MBUTTONDOWN;
                //  TODO: input processing.
            }
            break;
        }

        return DefWindowProcA(hwnd, msg, wParam, lParam);
    }
} // namespace Walrus

#endif // WPLATFORM_WINDOWS
