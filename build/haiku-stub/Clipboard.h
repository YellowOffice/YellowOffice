// Stub of Haiku <Clipboard.h> - declarations matching the real API.
//
// Real BClipboard has NO FindData/AddData/HasData; clipboard I/O goes
// through Data() (a BMessage*) and BMessage::FindData/AddData.
#ifndef YELLOW_HAIKU_STUB_CLIPBOARD_H
#define YELLOW_HAIKU_STUB_CLIPBOARD_H

#include <SupportDefs.h>
#include <Handler.h>
#include <Message.h>

class BClipboard : public BHandler {
public:
	BClipboard(const char* name, bool transient = false);
	virtual ~BClipboard();

	const char* Name() const;
	uint32 LocalCount() const;
	uint32 SystemCount() const;

	bool Lock();
	void Unlock();
	bool IsLocked() const;

	status_t Clear();
	status_t Commit();
	status_t Commit(bool failIfChanged);
	status_t Revert();

	BMessage* Data() const;
	// StartWatching/StopWatching/DataSource take a BMessenger in the real
	// API; omitted here (no BMessenger stub, and unused by the project).
};

extern BClipboard* be_clipboard;

#define B_CLIPBOARD_NAME "system/clipboard"
#define B_RAW_DATA_TYPE "application/x-vnd.haiku-raw-data"

#endif
