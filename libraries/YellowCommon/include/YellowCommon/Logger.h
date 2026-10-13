#ifndef YELLOW_COMMON_LOGGER_H
#define YELLOW_COMMON_LOGGER_H

#include <SupportDefs.h>
#include <String.h>
#include <vector>
#include <stdarg.h>

#include "Defines.h"

namespace YellowCommon {

class Logger {
public:
    Logger(const char* name = nullptr);
    ~Logger();

    // Named to avoid colliding with the DEBUG build macro (Haiku debug
    // builds define DEBUG=1, which broke the old enumerator).
    enum Level {
        LOG_DEBUG = YELLOW_LOG_DEBUG,
        LOG_INFO = YELLOW_LOG_INFO,
        LOG_WARNING = YELLOW_LOG_WARNING,
        LOG_ERROR = YELLOW_LOG_ERROR
    };

    void SetLevel(Level level);
    Level GetLevel() const;

    void SetName(const char* name);
    const char* GetName() const;

    void Log(Level level, const char* format, ...) const;
    void LogV(Level level, const char* format, va_list args) const;
    void Debug(const char* format, ...) const;
    void Info(const char* format, ...) const;
    void Warning(const char* format, ...) const;
    void Error(const char* format, ...) const;

    static Logger& GetGlobalLogger();
    static void SetGlobalLevel(Level level);
    static void DeleteGlobalLogger();

private:
    BString fName;
    Level fLevel;

    static Logger* sGlobalLogger;
};

} // namespace YellowCommon

#endif // YELLOW_COMMON_LOGGER_H
