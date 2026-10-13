// Stub of Haiku <Archivable.h> - declarations.
#ifndef YELLOW_HAIKU_STUB_ARCHIVABLE_H
#define YELLOW_HAIKU_STUB_ARCHIVABLE_H

#include <SupportDefs.h>

class BMessage;

class BArchivable {
public:
	BArchivable() {}
	virtual ~BArchivable() {}
	virtual status_t Archive(BMessage* data, bool deep = true) const { (void)data; (void)deep; return B_NOT_SUPPORTED; }
	virtual status_t Perform(uint32 what, void* argument) { (void)what; (void)argument; return B_NOT_SUPPORTED; }
};

#endif
