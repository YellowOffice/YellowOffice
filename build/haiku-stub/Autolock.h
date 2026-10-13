// Stub of Haiku <Autolock.h> - functional.
#ifndef YELLOW_HAIKU_STUB_AUTOLOCK_H
#define YELLOW_HAIKU_STUB_AUTOLOCK_H

#include <SupportDefs.h>

class BLocker;

template <class T>
class BAutolock {
public:
	BAutolock(T& object) : fObject(&object), fLocked(false) { fLocked = fObject->Lock(); }
	BAutolock(T* object) : fObject(object), fLocked(false) { if (fObject) fLocked = fObject->Lock(); }
	~BAutolock() { if (fObject && fLocked) fObject->Unlock(); }
	bool IsLocked() const { return fLocked; }

private:
	T* fObject;
	bool fLocked;
};

#endif
