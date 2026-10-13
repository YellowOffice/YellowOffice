#include "YellowCommon/Logger.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

namespace YellowCommon {

Logger* Logger::sGlobalLogger = nullptr;

Logger::Logger(const char* name)
    : fLevel(LOG_INFO)
{
    if (name) {
        fName = name;
    } else {
        fName = "YellowOffice";
    }
}

Logger::~Logger()
{
}

void Logger::SetLevel(Level level)
{
    fLevel = level;
}

Logger::Level Logger::GetLevel() const
{
    return fLevel;
}

void Logger::SetName(const char* name)
{
    if (name) {
        fName = name;
    }
}

const char* Logger::GetName() const
{
    return fName.String();
}

void Logger::Log(Level level, const char* format, ...) const
{
    if (level < fLevel || !format) {
        return;
    }

    va_list args;
    va_start(args, format);
    LogV(level, format, args);
    va_end(args);
}

void Logger::LogV(Level level, const char* format, va_list args) const
{
    if (level < fLevel || !format) {
        return;
    }

    const char* levelStr = "";
    switch (level) {
        case LOG_DEBUG:
            levelStr = "[DEBUG]";
            break;
        case LOG_INFO:
            levelStr = "[INFO]";
            break;
        case LOG_WARNING:
            levelStr = "[WARNING]";
            break;
        case LOG_ERROR:
            levelStr = "[ERROR]";
            break;
    }

    char buffer[4096];
    vsnprintf(buffer, sizeof(buffer), format, args);

    fprintf(stderr, "%s [%s] %s\n", levelStr, fName.String(), buffer);
    fflush(stderr);
}

void Logger::Debug(const char* format, ...) const
{
    va_list args;
    va_start(args, format);
    LogV(LOG_DEBUG, format, args);
    va_end(args);
}

void Logger::Info(const char* format, ...) const
{
    va_list args;
    va_start(args, format);
    LogV(LOG_INFO, format, args);
    va_end(args);
}

void Logger::Warning(const char* format, ...) const
{
    va_list args;
    va_start(args, format);
    LogV(LOG_WARNING, format, args);
    va_end(args);
}

void Logger::Error(const char* format, ...) const
{
    va_list args;
    va_start(args, format);
    LogV(LOG_ERROR, format, args);
    va_end(args);
}

Logger& Logger::GetGlobalLogger()
{
    if (!sGlobalLogger) {
        sGlobalLogger = new Logger("YellowOffice");
    }
    return *sGlobalLogger;
}

void Logger::SetGlobalLevel(Level level)
{
    GetGlobalLogger().SetLevel(level);
}

void Logger::DeleteGlobalLogger()
{
    delete sGlobalLogger;
    sGlobalLogger = nullptr;
}

} // namespace YellowCommon
