// Stub of Haiku <FindDirectory.h> - functional (maps to POSIX paths).
#ifndef YELLOW_HAIKU_STUB_FINDDIRECTORY_H
#define YELLOW_HAIKU_STUB_FINDDIRECTORY_H

#include <SupportDefs.h>
#include <FSDefs.h>
#include <Path.h>
#include <cstring>
#include <cstdlib>
#include <cstdio>

inline status_t find_directory(directory_which which, BPath* path, bool, dev_t) {
	if (path == NULL) return B_BAD_VALUE;
	const char* home = getenv("HOME");
	if (home == NULL) home = "/tmp";
	char buffer[1024];
	switch (which) {
		case B_USER_SETTINGS_DIRECTORY:
			snprintf(buffer, sizeof(buffer), "%s/config/settings", home);
			break;
		case B_USER_CONFIG_DIRECTORY:
			snprintf(buffer, sizeof(buffer), "%s/config", home);
			break;
		case B_USER_DIRECTORY:
		case B_USER_DESKTOP_DIRECTORY:
			snprintf(buffer, sizeof(buffer), "%s/Desktop", home);
			break;
		case B_SYSTEM_DIRECTORY:
			snprintf(buffer, sizeof(buffer), "/boot/system");
			break;
		case B_BEOS_DIRECTORY:
			snprintf(buffer, sizeof(buffer), "/boot/system");
			break;
		case B_APPS_DIRECTORY:
			snprintf(buffer, sizeof(buffer), "/boot/system/apps");
			break;
		case B_PREFERENCES_DIRECTORY:
			snprintf(buffer, sizeof(buffer), "%s/config/settings", home);
			break;
		case B_CACHE_DIRECTORY:
			snprintf(buffer, sizeof(buffer), "%s/cache", home);
			break;
		default:
			snprintf(buffer, sizeof(buffer), "%s", home);
			break;
	}
	return path->SetTo(buffer, NULL);
}

#endif
