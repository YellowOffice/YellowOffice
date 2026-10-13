// Stub of Haiku <Application.h> - declarations only (never linked on host).
#ifndef YELLOW_HAIKU_STUB_APPLICATION_H
#define YELLOW_HAIKU_STUB_APPLICATION_H

#include <SupportDefs.h>
#include <Looper.h>
#include <Message.h>
#include <Rect.h>

class BWindow;
class BRect;

class BApplication : public BLooper {
public:
	BApplication(const char* signature);
	virtual ~BApplication();

	virtual void ReadyToRun();
	virtual void AboutRequested();
	virtual void RefsReceived(BMessage* message);
	virtual void ArgvReceived(int32 argc, char** argv);
	virtual void MessageReceived(BMessage* message);
	virtual bool QuitRequested();
	virtual void Pulse();

	void Run();
	void Quit();
	int32 CountWindows() const;
	BWindow* WindowAt(int32 index) const;
	void SetPulseRate(bigtime_t rate);
	bigtime_t PulseRate() const;
	status_t GetAppSignature(char* signature) const;
	void SetAppSignature(const char* signature);
	void SetAppPath(const char* path);
	status_t GetAppPath(BString* path) const;

	enum {
		B_ARGV_ONLY = 1 << 0,
		B_MULTIPLE_LAUNCH = 1 << 1,
		B_SINGLE_LAUNCH = 1 << 2,
		B_FULL_LAUNCH = 1 << 3,
		B_BACKGROUND_APP = 1 << 4,
		B_LAUNCH_ON_STARTUP = 1 << 5,
		B_ARGV_RECEIVES_STARTED = 1 << 6,
		B_ABOUT_REQUESTED = 0x41424f55,
		B_REFS_RECEIVED = 0x52454653,
		B_QUERY_UPDATE = 0x51555044,
		B_PRINTER_CHANGED = 0x50524e43,
		B_FONTS_CHANGED = 0x464f4e54,
		B_LOCALE_CHANGED = 0x4c4f434c,
		B_UI_SETTINGS_CHANGED = 0x55495343,
		B_GET_SIZE_FOR_MENU = '_FSZ',
		B_SILENT_RELAUNCH = 0x73696c74,
	};

private:
	class BPrivateApplication* fPrivate;
};

extern BApplication* be_app;

#endif
