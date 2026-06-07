#include <engine/platform/Platform.h>

#if WPLATFORM_APPLE

#include <chrono>
#include <iostream>
#include <thread>

namespace Walrus
{
    void Platform::ConsoleWrite(std::string message, u8)
    {
        std::cout << message;
    }

    void Platform::ConsoleWriteError(std::string message, u8)
    {
        std::cerr << message;
    }

    double Platform::GetAbsoluteTime()
    {
        static const auto startTime = std::chrono::steady_clock::now();
        const auto elapsed = std::chrono::steady_clock::now() - startTime;
        return std::chrono::duration<double>(elapsed).count();
    }

    void Platform::Sleep(u64 ms)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }
} // namespace Walrus

#endif
