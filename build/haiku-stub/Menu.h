// Stub of Haiku menu kit headers (<Menu.h>, <MenuBar.h>, <MenuItem.h>, <SeparatorItem.h>, <PopUpMenu.h>) - declarations.
#ifndef YELLOW_HAIKU_STUB_MENU_H
#define YELLOW_HAIKU_STUB_MENU_H

#include <SupportDefs.h>
#include <Rect.h>
#include <InterfaceDefs.h>
#include <Message.h>
#include <View.h>
#include <String.h>

class BMenuItem;
class BPopupMenu;
class BMessenger;

enum menu_layout_t {
	B_ITEM_MENU = 0,
	B_ITEM_MATRIX = 0x00000010,
	B_GRID_LAYOUT = 0x00000020,
};

// Minimal BMessenger placeholder used by menu target APIs.
class BMessenger {
public:
	BMessenger() {}
	BMessenger(const BMessenger& other) { (void)other; }
	template <class T> BMessenger(T* handler) { (void)handler; }
	status_t SendMessage(BMessage* message) const { (void)message; return B_OK; }
	status_t SendMessage(uint32 command) const { (void)command; return B_OK; }
	bool IsValid() const { return false; }
};

class BMenu : public BView {
public:
	BMenu(const char* name, menu_layout layout = B_ITEM_MENU);
	virtual ~BMenu();

	void AddItem(BMenuItem* item);
	void AddItem(BMenuItem* item, int32 index);
	void AddItem(BMenu* submenu, int32 index = -1);
	void RemoveItem(BMenuItem* item);
	BMenuItem* RemoveItem(int32 index);
	int32 CountItems() const;
	BMenuItem* ItemAt(int32 index) const;
	BMenuItem* FindItem(const char* name) const;
	BMenuItem* FindItem(uint32 command) const;
	void AddSeparatorItem();
	void SetEnabled(bool enabled);
	bool IsEnabled() const;
	void SetLabel(const char* label);
	const char* Label() const;
	void SetRadioMode(bool on);
	bool RadioMode() const;
	void SetMarked(bool marked);
	bool IsMarked() const;
	BMenu* Supermenu() const;
	void SetMaxRecentItems(uint32 count);
	void SetTargetForItems(BHandler* target);
	void SetTargetForItems(const BMessenger& target);

protected:
	void* fPrivate;
};

class BMenuBar : public BMenu {
public:
	BMenuBar(BRect frame, const char* name, menu_layout layout = B_HORIZONTAL,
		uint32 resizingMode = B_FOLLOW_LEFT | B_FOLLOW_TOP,
		uint32 flags = B_WILL_DRAW | B_FRAME_EVENTS);
	explicit BMenuBar(const char* name, menu_layout layout = B_HORIZONTAL,
		uint32 flags = B_WILL_DRAW | B_FRAME_EVENTS);
	virtual ~BMenuBar();

	void SetBorder(BView* border);
	BView* Border() const;
};

class BMenuItem : public BHandler {
public:
	BMenuItem(const char* label, BMessage* message, char shortcut = 0, uint32 modifiers = 0);
	virtual ~BMenuItem();

	void SetLabel(const char* label);
	const char* Label() const;
	void SetEnabled(bool enabled);
	bool IsEnabled() const;
	void SetMarked(bool marked);
	bool IsMarked() const;
	BMessage* Message() const;
	void SetMessage(BMessage* message);
	char Shortcut() const;
	uint32 Modifiers() const;
	BMenu* Menu() const;
	virtual void Invoke();
	void SetTarget(BHandler* target);
	void SetTarget(const BMessenger& target);
	void SetTrigger(char c);

private:
	friend class BMenu;
	void* fPrivate;
};

class BSeparatorItem : public BMenuItem {
public:
	BSeparatorItem();
	virtual ~BSeparatorItem();
};

class BPopupMenu : public BMenu {
public:
	BPopupMenu(const char* name, bool resizeToFit = true, uint32 flags = 0);
	virtual ~BPopupMenu();
	void Go(BPoint where, bool openImmediately = false, bool autoRectify = true, bool track = true);
};

#endif
