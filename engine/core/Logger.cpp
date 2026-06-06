#include <engine/core/Logger.h>

#include <iostream>

namespace Walrus
{
    static constexpr std::string_view ToString(LogLevel level)
    {
        switch (level)
        {
        case LogLevel::Fatal:
            return "\033[35m[FATAL]";
        case LogLevel::Error:
            return "\033[31m[ERROR]";
        case LogLevel::Warn:
            return "\033[33m[WARN]";
        case LogLevel::Info:
            return "\033[37m[INFO]";
        case LogLevel::Debug:
            return "\033[34m[DEBUG]";
        case LogLevel::Trace:
            return "\033[32m[TRACE]";
        }
        return "UNKNOWN";
    }

    bool Logger::Initialize()
    {
        return true;
    }

    void Logger::Shutdown()
    {
    }

    void Logger::Log(LogLevel level, std::string_view message)
    {
        std::cout << ToString(level) << " " << message << "\033[0m" << std::endl;
    }
} // namespace Walrus
