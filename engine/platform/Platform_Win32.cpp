#include <engine/platform/Platform.h>

// Windows platform layer.
#if KPLATFORM_WINDOWS

#include <engine/core/Logger.h>

#include <stdlib.h>
#include <windows.h>
#include <windowsx.h> // param input extraction

namespace Walrus
{
    struct Platform::PlatformState
    {
        HINSTANCE hInstance = nullptr;
        HWND hwnd = nullptr;
    };

    // Clock
    static double clockFrequency;
    static LARGE_INTEGER startTime;

    LRESULT CALLBACK win32ProcessMessage(HWND hwnd, u32 msg, WPARAM wParam, LPARAM l_param);

    Platform::Platform(std::string applicationName, i32 x, i32 y, i32 width, i32 height)
        : m_State(std::make_unique<PlatformState>())
    {
        m_State->hInstance = GetModuleHandleA(0);

        // Setup and register window class.
        HICON icon = LoadIcon(m_State->hInstance, IDI_APPLICATION);
        WNDCLASSA wc;
        memset(&wc, 0, sizeof(wc));
        wc.style = CS_DBLCLKS; // Get double-clicks
        wc.lpfnWndProc = win32ProcessMessage;
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
            applicationName,
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

        // Clock setup
        LARGE_INTEGER frequency;
        QueryPerformanceFrequency(&frequency);
        clockFrequency = 1.0 / (f64)frequency.QuadPart;
        QueryPerformanceCounter(&startTime);
    }

    Platform::~Platform()
    {
        if (m_State->hwnd)
        {
            DestroyWindow(m_State->hwnd);
            m_State->hwnd = 0;
        }
    }

    bool Platform::PumpMessages()
    {
        MSG message;
        while (PeekMessageA(&message, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessageA(&message);
        }

        return true;
    }

    void* Platform::Allocate(u64 size, bool aligned)
    {
        return malloc(size);
    }

    void Platform::Free(void* block, bool aligned)
    {
        free(block);
    }

    void* Platform::ZeroMemory(void* block, u64 size)
    {
        return memset(block, 0, size);
    }

    void* Platform::CopyMemory(void* dest, const void* source, u64 size)
    {
        return memcpy(dest, source, size);
    }

    void* Platform::SetMemory(void* dest, i32 value, u64 size)
    {
        return memset(dest, value, size);
    }

    void Platform::ConsoleWrite(std::string message, u8 colour)
    {
        HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        // FATAL,ERROR,WARN,INFO,DEBUG,TRACE
        static u8 levels[6] = { 64, 4, 6, 2, 1, 8 };
        SetConsoleTextAttribute(consoleHandle, levels[colour]);
        OutputDebugStringA(message);
        u64 length = strlen(message);
        LPDWORD numberWritten = 0;
        WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), message, (DWORD)length, numberWritten, 0);
    }

    void Platform::ConsoleWriteError(std::string message, u8 colour)
    {
        HANDLE consoleHandle = GetStdHandle(STD_ERROR_HANDLE);
        // FATAL,ERROR,WARN,INFO,DEBUG,TRACE
        static u8 levels[6] = { 64, 4, 6, 2, 1, 8 };
        SetConsoleTextAttribute(consoleHandle, levels[colour]);
        OutputDebugStringA(message);
        u64 length = strlen(message);
        LPDWORD numberWritten = 0;
        WriteConsoleA(GetStdHandle(STD_ERROR_HANDLE), message, (DWORD)length, numberWritten, 0);
    }

    double Platform::GetAbsoluteTime()
    {
        LARGE_INTEGER nowTime;
        QueryPerformanceCounter(&nowTime);
        return (double)nowTime.QuadPart * clockFrequency;
    }

    void Platform::Sleep(u64 ms)
    {
        Sleep(ms);
    }

    LRESULT CALLBACK win32ProcessMessage(HWND hwnd, u32 msg, WPARAM wParam, LPARAM lParam)
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
#endif // KPLATFORM_WINDOWS