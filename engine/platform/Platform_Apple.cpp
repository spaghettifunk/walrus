#include <engine/platform/Platform.h>

#if WPLATFORM_APPLE

#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <thread>

namespace Walrus
{
    struct Platform::PlatformState
    {
        bool initialized = false;
        std::chrono::steady_clock::time_point startTime;
    };

    Platform::Platform(std::string, i32, i32, i32, i32)
        : m_State(std::make_unique<PlatformState>())
    {
        m_State->initialized = true;
        m_State->startTime = std::chrono::steady_clock::now();
    }

    Platform::~Platform() = default;

    bool Platform::PumpMessages()
    {
        return false;
    }

    void* Platform::Allocate(u64 size, bool)
    {
        return std::malloc(size);
    }

    void Platform::Free(void* block, bool)
    {
        std::free(block);
    }

    void* Platform::ZeroMemory(void* block, u64 size)
    {
        return std::memset(block, 0, size);
    }

    void* Platform::CopyMemory(void* dest, const void* source, u64 size)
    {
        return std::memcpy(dest, source, size);
    }

    void* Platform::SetMemory(void* dest, i32 value, u64 size)
    {
        return std::memset(dest, value, size);
    }

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
        const auto elapsed = std::chrono::steady_clock::now() - m_State->startTime;
        return std::chrono::duration<double>(elapsed).count();
    }

    void Platform::Sleep(u64 ms)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }
} // namespace Walrus

#endif
