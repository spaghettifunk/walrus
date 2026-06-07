#pragma once

#include <engine/Defines.h>
#include <string>

namespace Walrus
{
    class WAPI Platform
    {
    public:
        static void ConsoleWrite(std::string message, u8 colour);
        static void ConsoleWriteError(std::string message, u8 colour);

        static double GetAbsoluteTime();

        // Sleep on the thread for the provided ms. This blocks the current thread.
        // Should only be used for giving time back to the OS for unused update power.
        static void Sleep(u64 ms);
    };
} // namespace Walrus
