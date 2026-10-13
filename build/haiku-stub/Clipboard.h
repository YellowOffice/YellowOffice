// Stub of Haiku <Clipboard.h> - declarations.
#ifndef YELLOW_HAIKU_STUB_CLIPBOARD_H
#define YELLOW_HAIKU_STUB_CLIPBOARD_H

#include <SupportDefs.h>
#include <Handler.h>
#include <Message.h>
#include <String.h>

class BClipboard : public BHandler {
public:
	BClipboard(const char* name, bool makeDefault = false);
	virtual ~BClipboard();

	status_t Lock();
	void Unlock();
	bool IsLocked() const;
	status_t Clear();
	status_t Commit();
	BMessage* Data() const;
	status_t AddData(const char* type, const BMessage* source, bool makeDataPrivate = false);
	status_t FindData(const char* type, void** data, size_t* length) const;
	bool HasData(const char* type) const;
	status_t BecomeClipboard();
	const char* Name() const;
};

extern BClipboard* be_clipboard;

#define B_CLIPBOARD_NAME "system/clipboard"
#define B_PRIMARY_CLIPBOARD "system/clipboard"
#define B_RAW_DATA_TYPE "application/x-vnd.yd-raw-data"
#define B_TEXT_FLAVOR "text/plain"

#endif
