// Stub of Haiku <Layout.h>/<LayoutItem.h>/<GroupLayout.h>/<GroupView.h>/<LayoutBuilder.h> - declarations.
#ifndef YELLOW_HAIKU_STUB_LAYOUT_H
#define YELLOW_HAIKU_STUB_LAYOUT_H

#include <SupportDefs.h>
#include <Rect.h>
#include <Size.h>
#include <View.h>

class BLayoutItem;
class BView;
class BMenu;
class BWindow;

enum {
	B_USE_DEFAULT_SPACING = -1,
	B_USE_ITEM_SPACING = -2,
	B_USE_MAIN_AXIS_SPACING = -3,
	B_USE_CROSS_AXIS_SPACING = -4,
};

class BLayout {
public:
	BLayout();
	virtual ~BLayout();

	virtual void AddItem(BLayoutItem* item) = 0;
	virtual void RemoveItem(BLayoutItem* item) = 0;
	virtual int32 CountItems() const = 0;
	virtual BLayoutItem* ItemAt(int32 index) const = 0;

	void SetInsets(float left, float top, float right, float bottom);
	void GetInsets(float* left, float* top, float* right, float* bottom) const;
	void SetDefaultSpacing(float spacing);
	void SetExplicitMinSize(const BSize& size);
	void SetExplicitMaxSize(const BSize& size);
	void SetExplicitSize(const BSize& size);
	BView* TargetView() const;
	void InvalidateLayout();
};

class BTwoDimensionalLayout : public BLayout {
public:
	BTwoDimensionalLayout();
	virtual ~BTwoDimensionalLayout();
};

class BAbstractLayout : public BLayout {
public:
	BAbstractLayout();
	virtual ~BAbstractLayout();
	virtual void AddItem(BLayoutItem* item);
	virtual void RemoveItem(BLayoutItem* item);
	virtual int32 CountItems() const;
	virtual BLayoutItem* ItemAt(int32 index) const;
};

class BGroupLayout : public BAbstractLayout {
public:
	explicit BGroupLayout(uint32 orientation, float spacing = B_USE_DEFAULT_SPACING);
	BGroupLayout(BView* targetView, uint32 orientation, float spacing = B_USE_DEFAULT_SPACING);
	virtual ~BGroupLayout();

	void AddItem(BLayoutItem* item);
	void AddView(BView* view);
	void AddView(BView* view, float weight);
	void AddStrut(float size);
	void AddGlue();
	uint32 Orientation() const;
	void SetSpacing(float spacing);
	float ItemSpacing() const;
};

class BLayoutItem {
public:
	virtual ~BLayoutItem();
	virtual BView* View() const;
	virtual void SetVisible(bool visible);
	virtual bool IsVisible() const;
	void SetExplicitMinSize(const BSize& size);
	void SetExplicitMaxSize(const BSize& size);
	void SetExplicitSize(const BSize& size);
	void SetInsets(float left, float top, float right, float bottom);
};

class BLayoutBuilder {
public:
	template <class ParentBuilder = void>
	class Group {
	public:
		Group(uint32 orientation, float spacing = B_USE_DEFAULT_SPACING);
		Group(BView* targetView);
		Group(BWindow* window);
		Group(BLayout* layout);
		Group(ParentBuilder parent);
		Group(ParentBuilder parent, uint32 orientation, float spacing = B_USE_DEFAULT_SPACING);

		Group& SetInsets(float left, float top, float right, float bottom);
		Group& SetInsets(float inset);
		Group& SetSpacing(float spacing);
		Group& Add(BView* view, float weight = 0);
		Group& Add(BLayoutItem* item, float weight = 0);
		Group& Add(BLayout* layout, float weight = 0);
		Group& AddGroup(uint32 orientation, float spacing = B_USE_DEFAULT_SPACING, float weight = 0);
		Group& AddStrut(float size);
		Group& AddGlue();
		Group& AddSeparator();
		Group& AddMenu(BMenu* menu);
		Group& End();
	};

	template <class ParentBuilder = void>
	class Grid {
	public:
		Grid(float horizontalSpacing = B_USE_DEFAULT_SPACING, float verticalSpacing = B_USE_DEFAULT_SPACING);
		Grid(ParentBuilder parent, float horizontalSpacing = B_USE_DEFAULT_SPACING, float verticalSpacing = B_USE_DEFAULT_SPACING);
		Grid& SetInsets(float left, float top, float right, float bottom);
		Grid& SetInsets(float inset);
		Grid& Add(BView* view, int32 x, int32 y, uint32 width = 1, uint32 height = 1);
		Grid& Add(BLayoutItem* item, int32 x, int32 y, uint32 width = 1, uint32 height = 1);
		Grid& AddColumn(float weight);
		Grid& AddRow(float weight);
		Grid& End();
	};

	template <class ParentBuilder = void>
	class Cards {
	public:
		Cards();
		Cards(ParentBuilder parent);
		Cards& Add(BView* view);
		Cards& Add(BLayout* layout);
		Cards& SetVisibleItem(int32 index);
		Cards& End();
	};
};

#endif
