// Stub of Haiku <Looper.h> - declaration-only (UI linking not performed on host).
#ifndef YELLOW_HAIKU_STUB_LOOPER_H
#define YELLOW_HAIKU_STUB_LOOPER_H

#include <SupportDefs.h>
#include <Handler.h>
#include <Message.h>

class BLooper {
public:
	BLooper(const char* name = NULL, int32 priority = B_NORMAL_PRIORITY, int32 portCapacity = B_DEFAULT_QUEUE_CAPACITY);
	virtual ~BLooper();

	virtual void MessageReceived(BMessage* message);
	virtual bool QuitRequested();

	status_t Lock();
	void Unlock();
	bool IsLocked() const;

	void AddHandler(BHandler* handler);
	void RemoveHandler(BHandler* handler);
	bool DispatchMessage(BMessage* message, BHandler* handler);

	void Run();
	void Quit();
	status_t PostMessage(BMessage* message);
	status_t PostMessage(uint32 command);
	BMessage* CurrentMessage() const;

	enum {
		B_NORMAL_PRIORITY = 0,
		B_DEFAULT_QUEUE_CAPACITY = 64,
		B_LOOPER_PORT_DEFAULT_SIZE = 4096,
		B_LOOPER_PRIO_DEFAULT = B_NORMAL_PRIORITY,
	};

private:
	class BPrivate* fPrivate;
};

#endif
