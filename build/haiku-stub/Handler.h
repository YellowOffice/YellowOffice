// Stub of Haiku <Handler.h> and message constants shared by handlers.
#ifndef YELLOW_HAIKU_STUB_HANDLER_H
#define YELLOW_HAIKU_STUB_HANDLER_H

#include <SupportDefs.h>
#include <Message.h>

class BLooper;

class BHandler {
public:
	BHandler(const char* name = NULL) : fLooper(NULL) { (void)name; }
	virtual ~BHandler() {}

	virtual void MessageReceived(BMessage* message) { (void)message; }
	virtual void DetachedFromLooper() {}

	BLooper* Looper() const { return fLooper; }
	void SetLooper(BLooper* looper) { fLooper = looper; }

private:
	BLooper* fLooper;
};

#endif
