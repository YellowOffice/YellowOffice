// Stub of Haiku <Roster.h> - declarations.
#ifndef YELLOW_HAIKU_STUB_ROSTER_H
#define YELLOW_HAIKU_STUB_ROSTER_H

#include <SupportDefs.h>
#include <Message.h>
#include <StorageDefs.h>
#include <String.h>

class BRoster {
public:
	BRoster();
	~BRoster();

	bool IsRunning(const char* signature) const;
	status_t Launch(const char* signature, BMessage* message = NULL) const;
	status_t Launch(const char* signature, int32 argc, char** argv, team_id* team = NULL) const;
	status_t ActivateApp(const char* signature) const;
	status_t GetAppInfo(const char* signature, struct app_info* info) const;
};

extern BRoster be_roster;

struct app_info {
	team_id team;
	image_id image;
	entry_ref ref;
	uint32 flags;
	char signature[128];
};

#endif
