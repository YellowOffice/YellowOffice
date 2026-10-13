// YellowOffice - Haiku API stub headers (development/CI use ONLY).
// Minimal declarations mirroring the Haiku OS API so the project can be
// syntax-checked and its non-UI unit tests executed on non-Haiku hosts.
// Never shipped; the real headers on Haiku (/boot/system/develop/headers/os)
// take precedence when building on Haiku itself.
#ifndef YELLOW_HAIKU_STUB_SUPPORTDEFS_H
#define YELLOW_HAIKU_STUB_SUPPORTDEFS_H

#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
typedef volatile int8_t vint8;
typedef volatile int16_t vint16;
typedef volatile int32_t vint32;
typedef volatile int64_t vint64;
typedef volatile uint8_t vuint8;
typedef volatile uint16_t vuint16;
typedef volatile uint32_t vuint32;
typedef volatile uint64_t vuint64;

typedef int64 bigtime_t;
typedef int64 thread_id;
typedef int64 team_id;
typedef int64 port_id;
typedef int64 image_id;
typedef int64 area_id;
typedef int64 sem_id;

typedef uint32 type_code;
typedef uint32 node_flavor;
typedef uint32 menu_layout;

// Standard type codes from real Haiku SupportDefs.h (subset).
// Note: B_ANY_TYPE lives in FSDefs.h (= 0, node flavor), not here.
enum {
	B_BOOL_TYPE = 'BOOL',
	B_MIME_TYPE = 'MIME',
	B_RAW_TYPE = 'RAWT',
	B_REF_TYPE = 'REFS',
	B_STRING_TYPE = 'STRL',
};

enum {
	B_OS_NAME_LENGTH = 64,
	B_FILE_NAME_LENGTH = 1024,
	B_PATH_NAME_LENGTH = 1024,
	B_MIME_TYPE_LENGTH = 256,
};

const bigtime_t B_INFINITE_TIMEOUT = 0x7fffffffffffffffull;

enum status_constants {
	B_ERROR = (-2147483647 - 1),
	B_OK = 0,
	B_NO_ERROR = 0,
	B_ENTRY_NOT_FOUND = -2147483647,
	B_FILE_EXISTS = -138,
	B_NAME_TOO_LONG = -136,
	B_TIMED_OUT = -137,
	B_NOT_SUPPORTED = -1368,
	B_BAD_VALUE = -2147483647 + 1,
	B_BAD_TYPE = -2147483647 + 2,
	B_BAD_DATA = -2147483647 + 3,
	B_MISMATCHED_TYPES = -2147483647 + 4,
	B_BAD_OBJECT_ID = -2147483647 + 5,
	B_ALREADY_EXISTS = -2147483647 + 6,
	B_BUFFER_OVERFLOW = -2147483647 + 7,
	B_INTERRUPTED = -2147483647 + 8,
	B_PERMISSION_DENIED = -2147483647 + 9,
	B_NO_MEMORY = -2147483647 + 10,
	B_DUPLICATE_ADDR = -2147483647 + 11,
	B_BAD_WINDOW_REPLY = -2147483647 + 12,
	B_INVALID_REPLY = -2147483647 + 13,
	B_CLIENT_TIMED_OUT = -2147483647 + 14,
	B_NO_MORE_FDS = -2147483647 + 15,
	B_BAD_SEM_ID = -2147483647 + 16,
	B_BAD_TEAM_ID = -2147483647 + 17,
	B_BAD_PORT_ID = -2147483647 + 18,
	B_BAD_IMAGE_ID = -2147483647 + 19,
	B_BAD_AREA_ID = -2147483647 + 20,
	B_WAIT_FOR_REPLY = -2147483647 + 21,
	B_NAME_NOT_FOUND = -2147483647 + 24,
	B_NAME_IN_USE = -2147483647 + 25,
};

#ifndef __DEFINED_status_t
#define __DEFINED_status_t
typedef int32 status_t;
#endif

typedef uint32 mode_t_compat;

#endif // YELLOW_HAIKU_STUB_SUPPORTDEFS_H
