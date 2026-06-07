#include <engine/platform/Platform.h>

#if WPLATFORM_LINUX

#include <cstdio>
#include <ctime>
#include <string>

#if _POSIX_C_SOURCE >= 199309L
#include <time.h>
#else
#include <unistd.h>
#endif

namespace Walrus
{
    void Platform::ConsoleWrite(std::string message, u8 colour)
    {
        // FATAL,ERROR,WARN,INFO,DEBUG,TRACE
        std::string colourStrings[] = { "0;41", "1;31", "1;33", "1;32", "1;34", "1;30" };
        printf("\033[%sm%s\033[0m", colourStrings[colour].c_str(), message.c_str());
    }

    void Platform::ConsoleWriteError(std::string message, u8 colour)
    {
        // FATAL,ERROR,WARN,INFO,DEBUG,TRACE
        std::string colourStrings[] = { "0;41", "1;31", "1;33", "1;32", "1;34", "1;30" };
        printf("\033[%sm%s\033[0m", colourStrings[colour].c_str(), message.c_str());
    }

    double Platform::GetAbsoluteTime()
    {
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        return now.tv_sec + now.tv_nsec * 0.000000001;
    }

    void Platform::Sleep(u64 ms)
    {
#if _POSIX_C_SOURCE >= 199309L
        struct timespec ts;
        ts.tv_sec = ms / 1000;
        ts.tv_nsec = (ms % 1000) * 1000 * 1000;
        nanosleep(&ts, 0);
#else
        if (ms >= 1000)
        {
            sleep(ms / 1000);
        }
        usleep((ms % 1000) * 1000);
#endif
    }
} // namespace Walrus

#endif
