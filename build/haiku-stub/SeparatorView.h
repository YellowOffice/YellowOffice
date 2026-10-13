// Stub of Haiku <SeparatorView.h>/<Box.h>/<StringView.h> - declarations.
#ifndef YELLOW_HAIKU_STUB_SEPARATORVIEW_H
#define YELLOW_HAIKU_STUB_SEPARATORVIEW_H

#include <SupportDefs.h>
#include <Rect.h>
#include <InterfaceDefs.h>
#include <View.h>
#include <String.h>

typedef uint32 orientation;

class BSeparatorView : public BView {
public:
	BSeparatorView(uint32 flags);
	BSeparatorView(const char* name, uint32 flags, uint32 resizingMode, orientation orientation);
	virtual ~BSeparatorView();
	void Draw(BRect updateRect);
	void SetOrientation(orientation orientation);
	orientation Orientation() const;
};

class BBox : public BView {
public:
	BBox(BRect frame, uint32 flags, border_style border = B_FANCY_BORDER);
	BBox(const char* name, uint32 flags, border_style border = B_FANCY_BORDER);
	virtual ~BBox();
	void Draw(BRect updateRect);
	void SetLabel(const char* label);
	const char* Label() const;
	void SetBorder(border_style border);
};

class BStringView : public BView {
public:
	BStringView(BRect frame, const char* name, const char* text,
		uint32 resizingMode = B_FOLLOW_LEFT_TOP, uint32 flags = B_WILL_DRAW);
	BStringView(const char* name, const char* text, uint32 flags = B_WILL_DRAW);
	virtual ~BStringView();
	void Draw(BRect updateRect);
	void SetText(const char* text);
	const char* Text() const;
};

#endif
