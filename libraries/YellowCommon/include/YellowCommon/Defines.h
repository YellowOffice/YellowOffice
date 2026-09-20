#ifndef YELLOW_COMMON_DEFINES_H
#define YELLOW_COMMON_DEFINES_H

#include <SupportDefs.h>

// YellowOffice suite defines
#define YELLOWOFFICE_SUITE_NAME "YellowOffice"
#define YELLOWOFFICE_VENDOR "YellowOffice Team"
#define YELLOWOFFICE_VERSION "0.1.0"

// YellowDocs defines
#define YELLOWDOCS_APP_NAME "YellowDocs"
#define YELLOWDOCS_APP_SIGNATURE "application/x-vnd.yellowdocs"
#define YELLOWDOCS_MIME_TYPE "application/x-vnd.yellowdocs"

// File format defines
#define YDF_FORMAT_NAME "Yellow Document Format"
#define YDF_FORMAT_EXTENSION "ydf"
#define YDF_MIME_TYPE "application/x-vnd.yellowdocs.ydf"

// Message defines
#define YELLOW_MSG_BASE 'YELW'

// Error codes
#define YELLOW_SUCCESS 0
#define YELLOW_ERROR_GENERIC -1
#define YELLOW_ERROR_MEMORY -2
#define YELLOW_ERROR_FILE -3
#define YELLOW_ERROR_PARSE -4
#define YELLOW_ERROR_UNSUPPORTED -5

// Logging levels
#define YELLOW_LOG_DEBUG 0
#define YELLOW_LOG_INFO 1
#define YELLOW_LOG_WARNING 2
#define YELLOW_LOG_ERROR 3

// Build configuration
#ifdef DEBUG
#define YELLOW_DEBUG 1
#else
#define YELLOW_DEBUG 0
#endif

// Platform detection
#ifdef __HAIKU__
#define YELLOW_PLATFORM_HAIKU 1
#else
#define YELLOW_PLATFORM_HAIKU 0
#endif

// Export/import for shared libraries
#if defined(_WIN32) || defined(__WIN32__) || defined(__CYGWIN__)
    #ifdef YELLOWCOMMON_BUILDING_DLL
        #define YELLOWCOMMON_API __declspec(dllexport)
    #else
        #define YELLOWCOMMON_API __declspec(dllimport)
    #endif
#elif __GNUC__ >= 4
    #define YELLOWCOMMON_API __attribute__((visibility("default")))
#else
    #define YELLOWCOMMON_API
#endif

#endif // YELLOW_COMMON_DEFINES_H
