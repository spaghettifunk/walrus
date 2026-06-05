#pragma once

#include <engine/Defines.h>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

#define LOG_WARN_ENABLED 1
#define LOG_INFO_ENABLED 1
#define LOG_DEBUG_ENABLED 1
#define LOG_TRACE_ENABLED 1

// Disable debug and trace logging for release builds.
#if WRELEASE == 1
#define LOG_DEBUG_ENABLED 0
#define LOG_TRACE_ENABLED 0
#else
#define LOG_DEBUG_ENABLED 1
#define LOG_TRACE_ENABLED 1
#endif

namespace Walrus
{
    enum class LogLevel
    {
        Fatal,
        Error,
        Warn,
        Info,
        Debug,
        Trace
    };

    class WAPI Logger
    {
    public:
        static bool Initialize();
        static void Shutdown();

        static void Log(LogLevel level, std::string_view message);

        template<typename... Args>
        static void Log(LogLevel level, std::string_view message, Args&&... args)
        {
            Log(level, Format(message, std::forward<Args>(args)...));
        }

    private:
        template<typename T>
        static std::string ArgumentToString(T&& value)
        {
            std::ostringstream stream;
            stream << std::forward<T>(value);
            return stream.str();
        }

        static std::string Format(std::string_view message)
        {
            return std::string(message);
        }

        template<typename T, typename... Args>
        static std::string Format(std::string_view message, T&& value, Args&&... args)
        {
            std::string result(message);
            std::size_t position = result.find("{}");

            if (position == std::string::npos)
            {
                return result;
            }

            result.replace(position, 2, ArgumentToString(std::forward<T>(value)));
            return Format(result, std::forward<Args>(args)...);
        }
    };
} // namespace Walrus

// Logs a fatal-level message.
#define WFATAL(message, ...) ::Walrus::Logger::Log(::Walrus::LogLevel::Fatal, message __VA_OPT__(, ) __VA_ARGS__)

#ifndef WERROR
// Logs an error-level message.
#define WERROR(message, ...) ::Walrus::Logger::Log(::Walrus::LogLevel::Error, message __VA_OPT__(, ) __VA_ARGS__)
#endif

#if LOG_WARN_ENABLED == 1
// Logs a warning-level message.
#define WWARN(message, ...) ::Walrus::Logger::Log(::Walrus::LogLevel::Warn, message __VA_OPT__(, ) __VA_ARGS__)
#else
// Does nothing when LOG_WARN_ENABLED != 1
#define WWARN(message, ...)
#endif

#if LOG_INFO_ENABLED == 1
// Logs a info-level message.
#define WINFO(message, ...) ::Walrus::Logger::Log(::Walrus::LogLevel::Info, message __VA_OPT__(, ) __VA_ARGS__)
#else
// Does nothing when LOG_INFO_ENABLED != 1
#define WINFO(message, ...)
#endif

#if LOG_DEBUG_ENABLED == 1
// Logs a debug-level message.
#define WDEBUG(message, ...) ::Walrus::Logger::Log(::Walrus::LogLevel::Debug, message __VA_OPT__(, ) __VA_ARGS__)
#else
// Does nothing when LOG_DEBUG_ENABLED != 1
#define WDEBUG(message, ...)
#endif

#if LOG_TRACE_ENABLED == 1
// Logs a trace-level message.
#define WTRACE(message, ...) ::Walrus::Logger::Log(::Walrus::LogLevel::Trace, message __VA_OPT__(, ) __VA_ARGS__)
#else
// Does nothing when LOG_TRACE_ENABLED != 1
#define WTRACE(message, ...)
#endif
