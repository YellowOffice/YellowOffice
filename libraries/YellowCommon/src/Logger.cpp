#include "YellowCommon/Logger.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

namespace YellowCommon {

// Global logger instance
Logger* Logger::sGlobalLogger = nullptr;

// Logger implementation
Logger::Logger(const char* name)
    : fLevel(YELLOW_LOG_INFO)
{
    if (name) {
        fName = name;
    } else {
        fName = "YellowOffice";
    }
}

Logger::~Logger()
{
    // Don't delete global logger here
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
    if (level < fLevel) {
        return;
    }

    const char* levelStr = "";
    switch (level) {
        case DEBUG:
            levelStr = "[DEBUG]";
            break;
        case INFO:
            levelStr = "[INFO]";
            break;
        case WARNING:
            levelStr = "[WARNING]";
            break;
        case ERROR:
            levelStr = "[ERROR]";
            break;
    }

    fprintf(stderr, "%s [%s] ", levelStr, fName.String());
    
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    
    fprintf(stderr, "\n");
    fflush(stderr);
}

void Logger::Debug(const char* format, ...) const
{
    va_list args;
    va_start(args, format);
    Log(DEBUG, format, args);
    va_end(args);
}

void Logger::Info(const char* format, ...) const
{
    va_list args;
    va_start(args, format);
    Log(INFO, format, args);
    va_end(args);
}

void Logger::Warning(const char* format, ...) const
{
    va_list args;
    va_start(args, format);
    Log(WARNING, format, args);
    va_end(args);
}

void Logger::Error(const char* format, ...) const
{
    va_list args;
    va_start(args, format);
    Log(ERROR, format, args);
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

} // namespace YellowCommon
