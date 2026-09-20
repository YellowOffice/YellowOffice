#ifndef YELLOW_COMMON_LOGGER_H
#define YELLOW_COMMON_LOGGER_H

#include <SupportDefs.h>
#include <String.h>
#include <vector>

#include "Defines.h"

namespace YellowCommon {

class Logger {
public:
    Logger(const char* name = nullptr);
    ~Logger();

    enum Level {
        DEBUG = YELLOW_LOG_DEBUG,
        INFO = YELLOW_LOG_INFO,
        WARNING = YELLOW_LOG_WARNING,
        ERROR = YELLOW_LOG_ERROR
    };

    void SetLevel(Level level);
    Level GetLevel() const;

    void SetName(const char* name);
    const char* GetName() const;

    void Log(Level level, const char* format, ...) const;
    void Debug(const char* format, ...) const;
    void Info(const char* format, ...) const;
    void Warning(const char* format, ...) const;
    void Error(const char* format, ...) const;

    static Logger& GetGlobalLogger();
    static void SetGlobalLevel(Level level);

private:
    BString fName;
    Level fLevel;
    
    static Logger* sGlobalLogger;
};

} // namespace YellowCommon

#endif // YELLOW_COMMON_LOGGER_H
