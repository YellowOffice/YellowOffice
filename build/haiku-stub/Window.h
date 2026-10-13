// Stub of Haiku <Window.h> - declarations only (never linked on host).
#ifndef YELLOW_HAIKU_STUB_WINDOW_H
#define YELLOW_HAIKU_STUB_WINDOW_H

#include <SupportDefs.h>
#include <Rect.h>
#include <Size.h>
#include <GraphicsDefs.h>
#include <InterfaceDefs.h>
#include <Looper.h>
#include <String.h>
#include <vector>

class BView;
class BMenuBar;
class BMessage;
class BBitmap;

enum window_type {
	B_UNTYPED_WINDOW = 0,
	B_TITLED_WINDOW = 1,
	B_MODAL_WINDOW = 2,
	B_DOCUMENT_WINDOW = 3,
	B_BORDERED_WINDOW = 4,
	B_POP_UP_WINDOW = 5,
	B_MEDIA_WINDOW = 6,
	B_FLOATING_WINDOW = 7,
	B_SYSTEM_MODAL_WINDOW = 8,
	B_LOOK_MODAL_WINDOW = 9,
};

enum window_flags {
	B_NOT_MOVABLE = 0x00000001,
	B_NOT_CLOSABLE = 0x00000002,
	B_NOT_ZOOMABLE = 0x00000004,
	B_NOT_MINIMIZABLE = 0x00000008,
	B_NOT_RESIZABLE = 0x00000010,
	B_NOT_VENETIAN_BLIND_CLOSEABLE = 0x00000020,
	B_AVOID_FOCUS = 0x00000040,
	B_AVOID_WORKSPACE = 0x00000080,
	B_AVOID_FRONT = 0x00000100,
	B_WILL_ACCEPT_FIRST_CLICK = 0x00000200,
	B_NO_WORKSPACE_ACTIVATION = 0x00000400,
	B_NOT_ANCHORED_ON_ACTIVATE = 0x00000800,
	B_AUTO_UPDATE_SIZE_LIMITS = 0x00001000,
	B_QUIT_ON_WINDOW_CLOSE = 0x00002000,
	B_SUPPORTS_ZOOM_ON_CLICK = 0x00004000,
	B_NO_CONNNECT_TO_INPUT_SERVER = 0x00008000,
	B_HIDE_ON_SUSPEND = 0x00010000,
	B_DO_NOT_HIDE_ON_SUSPEND = 0x00020000,
	B_ASYNCHRONOUS_CONTROLS = 0x00080000,
};

class BLayout;

class BWindow : public BLooper {
public:
	BWindow(BRect frame, const char* title, window_type type,
		uint32 flags = B_ASYNCHRONOUS_CONTROLS | B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE);
	virtual ~BWindow();

	virtual void MessageReceived(BMessage* message);
	virtual bool QuitRequested();
	virtual void FrameResized(float newWidth, float newHeight);
	virtual void WorkspacesChanged(uint32 oldWorkspaces, uint32 newWorkspaces);
	virtual void ScreenChanged(BRect screenSize, color_space depth);
	virtual void Zoom(BPoint origin, float width, float height);
	virtual void MenusBeginning();
	virtual void MenusEnded();

	void SetTitle(const char* title);
	const char* Title() const;
	void SetLayout(BLayout* layout);
	void AddChild(BView* child);
	void RemoveChild(BView* child);
	void Show();
	void Hide();
	void Quit();
	void Close();
	void Activate(bool active = true);
	bool IsActive() const;
	void MoveOnScreen(uint32 flags = 0);
	void SetSizeLimits(float minWidth, float maxWidth, float minHeight, float maxHeight);
	BRect Bounds() const;
	BRect Frame() const;
	BView* FindView(const char* name) const;
	BView* FindView(BPoint point) const;
	void SetKeyMenuBar(BMenuBar* bar);
	void SetPulseRate(bigtime_t rate);
	BBitmap* Render();
	void SetExplicitMinSize(const BSize& size);
	void SetExplicitMaxSize(const BSize& size);

	bool Lock();
	void Unlock();
	bool IsLocked() const;
	using BLooper::PostMessage;

	enum {
		B_CURRENT_WORKSPACE = 0xffffffff,
		B_MOVE_ON_NEXT_OPEN = 0x00000001,
		B_USE_AUTO_FOCUS = 0x00000002,
		B_DO_NOT_FRONT_ON_ACTIVATE = 0x00000004,
	};

private:
	class BPrivateWindow* fPrivate;
	std::vector<BView*> fStubChildren;
};

#endif
