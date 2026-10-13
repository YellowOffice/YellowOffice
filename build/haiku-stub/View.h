// Stub of Haiku <View.h> - declarations only (never linked on host).
#ifndef YELLOW_HAIKU_STUB_VIEW_H
#define YELLOW_HAIKU_STUB_VIEW_H

#include <SupportDefs.h>
#include <Rect.h>
#include <Point.h>
#include <Size.h>
#include <GraphicsDefs.h>
#include <InterfaceDefs.h>
#include <Handler.h>
#include <Font.h>
#include <String.h>

class BWindow;
class BLayout;
class BBitmap;
class BScrollView;
class BPolygon;
class BRegion;

// mouse transit (real Haiku defines these in View.h)
enum {
	B_ENTERED_VIEW = 0,
	B_INSIDE_VIEW,
	B_EXITED_VIEW,
	B_OUTSIDE_VIEW
};

// set font mask (subset used by the real B_FONT_ALL default)
enum {
	B_FONT_ALL = 0x000001FF
};

class BView : public BHandler {
public:
	class BLooper* Looper() const;
	BHandler* Handler() const;
	BView(const char* name, uint32 flags);
	BView(BRect frame, const char* name, uint32 resizingMode, uint32 flags);
	virtual ~BView();

	virtual void Draw(BRect updateRect);
	virtual void MouseDown(BPoint where);
	virtual void MouseUp(BPoint where);
	virtual void MouseMoved(BPoint where, uint32 code, const BMessage* dragMessage);
	virtual void KeyDown(const char* bytes, int32 numBytes);
	virtual void KeyUp(const char* bytes, int32 numBytes);
	virtual void Pulse();
	virtual void FrameResized(float newWidth, float newHeight);
	virtual void AttachedToWindow();
	virtual void DetachedFromWindow();
	virtual void AllAttached();
	virtual void WindowActivated(bool active);
	virtual void GetPreferredSize(float* _width, float* _height);
	virtual void ResizeToPreferred();

	void SetViewColor(rgb_color color);
	void SetViewColor(uint8 r, uint8 g, uint8 b, uint8 a = 255);
	rgb_color ViewColor() const;
	void SetLowColor(rgb_color color);
	void SetLowColor(uint8 r, uint8 g, uint8 b, uint8 a = 255);
	rgb_color LowColor() const;
	void SetHighColor(rgb_color color);
	void SetHighColor(uint8 r, uint8 g, uint8 b, uint8 a = 255);
	rgb_color HighColor() const;
	void SetDrawingMode(drawing_mode mode);
	drawing_mode DrawingMode() const;
	void SetLineMode(line_mode cap = B_BUTT_CAP, line_mode join = B_MITER_JOIN);
	void SetPenSize(float size);
	float PenSize() const;

	void FillRect(BRect rect, pattern p = B_SOLID_HIGH);
	void StrokeRect(BRect rect, pattern p = B_SOLID_HIGH);
	void StrokeLine(BPoint from, BPoint to, pattern p = B_SOLID_HIGH);
	void DrawString(const char* string);
	void DrawString(const char* string, int32 length);
	void DrawString(const char* string, BPoint origin);
	void DrawString(const char* string, int32 length, BPoint origin);
	void DrawBitmap(const BBitmap* bitmap);
	void DrawBitmap(const BBitmap* bitmap, BPoint at);
	void DrawBitmap(const BBitmap* bitmap, BRect srcRect, BRect dstRect);
	void InvertRect(BRect rect);
	void FillPolygon(const BPoint* pointArray, int32 numPoints,
		pattern p = B_SOLID_HIGH);

	// Real Haiku has only the pointer overload (plus a mask parameter).
	void SetFont(const BFont* font, uint32 mask = B_FONT_ALL);
	void GetFont(BFont* font) const;
	float StringWidth(const char* string) const;
	float StringWidth(const BString& string) const;
	void GetFontHeight(font_height* height) const;

	void PushState();
	void PopState();
	void ClipToRect(BRect rect);
	void ClipToInverseRect(BRect rect);
	void SetClippingRegion(const void* region);

	BRect Bounds() const;
	BRect Frame() const;
	void MoveTo(BPoint where);
	void MoveTo(float x, float y);
	void MoveBy(float dx, float dy);
	void ResizeTo(float width, float height);
	void ResizeBy(float dx, float dy);
	void ConvertToParent(BPoint* point) const;
	void ConvertFromParent(BPoint* point) const;
	BPoint ConvertToParent(BPoint point) const;
	BPoint ConvertFromParent(BPoint point) const;

	void Invalidate();
	void Invalidate(BRect area);
	void SetFlags(uint32 flags);
	uint32 Flags() const;
	void SetExplicitMinSize(const BSize& size);
	void SetExplicitMaxSize(const BSize& size);
	void SetExplicitSize(const BSize& size);
	BSize ExplicitMinSize() const;
	BSize ExplicitMaxSize() const;
	BSize ExplicitSize() const;
	void SetLayout(BLayout* layout);
	BLayout* GetLayout() const;

	void Show();
	void Hide();
	bool IsHidden() const;
	BWindow* Window() const;
	BView* Parent() const;
	void AddChild(BView* child, BView* childBefore = NULL);
	void RemoveChild(BView* child);
	int32 CountChildren() const;
	BView* ChildAt(int32 index) const;
	void SetViewColor(BRect rect, rgb_color color);
	void ScrollBy(float dh, float dv);
	void ScrollTo(float x, float y);
	virtual void ScrollTo(BPoint where);
	void MakeFocus(bool focused = true);
	bool IsFocus() const;
	void SetMouseEventMask(uint32 mask, uint32 options = 0);
	void Sync() const;
	void Update();

	enum {
		B_ASYNCHRONOUS_CONTROLS = 0x00000008,
	};

private:
	friend class BScrollView;
	class BPrivateView* fPrivate;
};

#endif
