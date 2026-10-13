// Stub of Haiku <FilePanel.h> - declarations.
#ifndef YELLOW_HAIKU_STUB_FILEPANEL_H
#define YELLOW_HAIKU_STUB_FILEPANEL_H

#include <SupportDefs.h>
#include <Rect.h>
#include <Window.h>
#include <Message.h>
#include <StorageDefs.h>
#include <String.h>

class BRefFilter;
class BMessenger;
class BNode;
struct stat;

enum file_panel_mode {
	B_OPEN_PANEL = 0,
	B_SAVE_PANEL = 1,
};

#define B_SAVE_REQUESTED 'save'
#define B_REFS_RECEIVED 'refs'
#define B_CANCEL 'cancel'

enum {
	B_FILE_NODE = 0x01,
	B_DIRECTORY_NODE = 0x02,
	B_SYMLINK_NODE = 0x04,
	B_ANY_NODE = 0xff,
};

class BFilePanel : public BWindow {
public:
	BFilePanel(file_panel_mode mode = B_OPEN_PANEL, BMessenger* target = NULL,
		const entry_ref* directory = NULL, uint32 nodeFlavors = B_FILE_NODE,
		bool multipleSelection = false, BMessage* message = NULL,
		BRefFilter* filter = NULL, bool modal = false, bool hideWhenDone = true);
	virtual ~BFilePanel();

	virtual void MessageReceived(BMessage* message);
	void SetTarget(const BMessenger& target);
	void SetTarget(BHandler* target);
	void Show();
	void Hide();
	void SetText(const char* text);
	void SetSaveText(const char* text);
	const char* SaveText() const;
	void SetPanelMode(file_panel_mode mode);
	file_panel_mode PanelMode() const;
	void SetFilter(BRefFilter* filter);
	status_t GetPanelDirectory(entry_ref* ref) const;
	status_t SetPanelDirectory(const entry_ref* ref);
};

class BRefFilter {
public:
	BRefFilter() {}
	virtual ~BRefFilter() {}
	virtual bool Filter(const entry_ref* ref, BNode* node) = 0;
	virtual bool Filter(const entry_ref* ref, const struct stat* stat) {
		(void)ref; (void)stat; return true;
	}
};

#endif
