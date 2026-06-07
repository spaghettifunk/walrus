#include <engine/platform/Platform.h>

// Windows platform layer.
#if WPLATFORM_WINDOWS

#include <cstring>
#include <windows.h>

namespace Walrus
{
    void Platform::ConsoleWrite(std::string message, u8 colour)
    {
        HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        // FATAL,ERROR,WARN,INFO,DEBUG,TRACE
        static u8 levels[6] = { 64, 4, 6, 2, 1, 8 };
        SetConsoleTextAttribute(consoleHandle, levels[colour]);
        OutputDebugStringA(message.c_str());
        u64 length = strlen(message.c_str());
        LPDWORD numberWritten = 0;
        WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), message.c_str(), (DWORD)length, numberWritten, 0);
    }

    void Platform::ConsoleWriteError(std::string message, u8 colour)
    {
        HANDLE consoleHandle = GetStdHandle(STD_ERROR_HANDLE);
        // FATAL,ERROR,WARN,INFO,DEBUG,TRACE
        static u8 levels[6] = { 64, 4, 6, 2, 1, 8 };
        SetConsoleTextAttribute(consoleHandle, levels[colour]);
        OutputDebugStringA(message.c_str());
        u64 length = strlen(message.c_str());
        LPDWORD numberWritten = 0;
        WriteConsoleA(GetStdHandle(STD_ERROR_HANDLE), message.c_str(), (DWORD)length, numberWritten, 0);
    }

    double Platform::GetAbsoluteTime()
    {
        static double clockFrequency = []()
        {
            LARGE_INTEGER frequency;
            QueryPerformanceFrequency(&frequency);
            return 1.0 / (f64)frequency.QuadPart;
        }();
        static LARGE_INTEGER startTime = []()
        {
            LARGE_INTEGER time;
            QueryPerformanceCounter(&time);
            return time;
        }();

        LARGE_INTEGER nowTime;
        QueryPerformanceCounter(&nowTime);
        return (double)(nowTime.QuadPart - startTime.QuadPart) * clockFrequency;
    }

    void Platform::Sleep(u64 ms)
    {
        ::Sleep((DWORD)ms);
    }
} // namespace Walrus

#endif // WPLATFORM_WINDOWS
