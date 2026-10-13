// Stub of Haiku <ListView.h>/<StringItem.h>/<OutlineListView.h> - declarations.
#ifndef YELLOW_HAIKU_STUB_LISTVIEW_H
#define YELLOW_HAIKU_STUB_LISTVIEW_H

#include <SupportDefs.h>
#include <Rect.h>
#include <InterfaceDefs.h>
#include <Message.h>
#include <View.h>
#include <String.h>
#include <vector>

enum list_view_type {
	B_SINGLE_SELECTION_LIST = 0,
	B_MULTIPLE_SELECTION_LIST = 1,
	B_NETWORK_LIST = 2,
};

class BListItem;
class BStringItem;
class BListView;
class BOutlineListView;

class BListItem {
public:
	BListItem();
	virtual ~BListItem();
	virtual int32 CompileIndex(BOutlineListView* owner, int32 parentIndex, int32 indexOnLevel);
	virtual void SetEnabled(bool enable);
	bool IsEnabled() const;
	virtual int32 Level() const;
	virtual BOutlineListView* OutlineListView() const;
	virtual BListView* Owner() const;
	int32 Index() const;

private:
	friend class BListView;
	friend class BOutlineListView;
	int32 fIndex;
	int32 fLevel;
	bool fEnabled;
	BListView* fOwner;
};

class BStringItem : public BListItem {
public:
	BStringItem(const char* text, int32 level = 0, bool expanded = true);
	virtual ~BStringItem();
	void SetText(const char* text);
	const char* Text() const;
	virtual void Select();
	virtual void Deselect();
	bool IsSelected() const;
	void SetDrawingMode(drawing_mode mode);

private:
	BString fText;
	bool fSelected;
};

class BListView : public BView {
public:
	BListView(BRect frame, const char* name, list_view_type type = B_SINGLE_SELECTION_LIST,
		uint32 resizingMode = B_FOLLOW_LEFT | B_FOLLOW_TOP,
		uint32 flags = B_WILL_DRAW | B_FRAME_EVENTS | B_NAVIGABLE);
	virtual ~BListView();

	virtual void MessageReceived(BMessage* message);
	virtual void MouseDown(BPoint where);
	virtual void KeyDown(const char* bytes, int32 numBytes);
	virtual void Draw(BRect updateRect);

	void AddItem(BListItem* item);
	void AddItem(BListItem* item, int32 index);
	bool RemoveItem(BListItem* item);
	BListItem* RemoveItem(int32 index);
	void MakeEmpty();
	int32 CountItems() const;
	BListItem* ItemAt(int32 index) const;
	int32 CurrentSelection() const;
	BListItem* FirstItem() const;
	BListItem* LastItem() const;
	void Select(int32 index);
	void Deselect(int32 index);
	void DeselectAll();
	void Invoke();
	void SetSelectionMessage(BMessage* message);
	void SetInvocationMessage(BMessage* message);
	void SetTarget(BHandler* target);
	void ScrollToSelection();

protected:
	std::vector<BListItem*> fItems;
};

class BOutlineListView : public BListView {
public:
	BOutlineListView(BRect frame, const char* name, list_view_type type = B_SINGLE_SELECTION_LIST,
		uint32 resizingMode = B_FOLLOW_LEFT | B_FOLLOW_TOP,
		uint32 flags = B_WILL_DRAW | B_FRAME_EVENTS | B_NAVIGABLE);
	virtual ~BOutlineListView();

	void AddUnder(BListItem* item, BListItem* superItem);
	void AddSiblingItem(BListItem* item, BListItem* superItem);
	BListItem* Superitem(BListItem* item) const;
	bool IsExpanded(BListItem* item) const;
	void Expand(BListItem* item);
	void Collapse(BListItem* item);
	int32 FullListIndexOf(BListItem* item) const;
	BListItem* FullListItemAt(int32 fullListIndex) const;
	int32 FullListCountItems() const;
};

#endif
