#pragma once

#include <engine/Defines.h>
#include <memory>
#include <string>

namespace Walrus
{
    class WAPI Platform
    {
    public:
        Platform(std::string applicationName, i32 x, i32 y, i32 width, i32 height);
        ~Platform();

        Platform(const Platform&) = delete;
        Platform& operator=(const Platform&) = delete;

        bool PumpMessages();

        void* Allocate(u64 size, bool aligned);
        void Free(void* block, bool aligned);
        void* ZeroMemory(void* block, u64 size);
        void* CopyMemory(void* dest, const void* source, u64 size);
        void* SetMemory(void* dest, i32 value, u64 size);

        void ConsoleWrite(std::string message, u8 colour);
        void ConsoleWriteError(std::string message, u8 colour);

        double GetAbsoluteTime();

        // Sleep on the thread for the provided ms. This blocks the main thread.
        // Should only be used for giving time back to the OS for unused update power.
        // Therefore it is not exported.
        void Sleep(u64 ms);

    private:
        struct PlatformState;
        std::unique_ptr<PlatformState> m_State;
    };
} // namespace Walrus