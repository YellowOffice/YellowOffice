// Stub of Haiku <ScrollBar.h>/<ScrollView.h> - declarations.
#ifndef YELLOW_HAIKU_STUB_SCROLLBAR_H
#define YELLOW_HAIKU_STUB_SCROLLBAR_H

#include <SupportDefs.h>
#include <Rect.h>
#include <InterfaceDefs.h>
#include <Handler.h>
#include <Message.h>
#include <View.h>

class BScrollView;

class BScrollBar : public BView {
public:
	BScrollBar(BRect frame, const char* name, BView* target,
		float min, float max, uint32 resizingMode, uint32 flags);
	virtual ~BScrollBar();

	void SetRange(float min, float max);
	void GetRange(float* min, float* max) const;
	void SetValue(float value);
	float Value() const;
	void SetProportion(float proportion);
	float GetProportion() const;
	void SetSteps(float small, float large);
	void GetSteps(float* small, float* large) const;
	void SetPageSteps(float small, float large);
	void GetPageSteps(float* small, float* large) const;
	void SetTarget(BView* target);
	BView* Target() const;
	void SetEnabled(bool enabled);
};

class BScrollView : public BView {
public:
	BScrollView(const char* name, BView* target,
		uint32 resizingMode = B_FOLLOW_LEFT_TOP, uint32 flags = 0,
		bool horizontal = false, bool vertical = false,
		border_style border = B_FANCY_BORDER);
	BScrollView(const char* name, BView* target, uint32 flags,
		bool horizontal, bool vertical, border_style border = B_FANCY_BORDER);
	virtual ~BScrollView();

	BScrollBar* HorizontalScrollBar() const;
	BScrollBar* VerticalScrollBar() const;
	void SetBorderType(border_style border);
	border_style BorderType() const;
	void SetViewColor(rgb_color color);
	void SetTarget(BView* target);
	void UpdateScrollBars();
};

#endif
