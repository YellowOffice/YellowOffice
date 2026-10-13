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
	virtual void MouseMoved(BPoint where, uint32 transit, const BMessage* dragMessage);
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

	void FillRect(BRect rect);
	void FillRect(BRect rect, pattern p);
	void FillRect(BRect rect, uint32 invertMode);
	void StrokeRect(BRect rect);
	void StrokeLine(BPoint from, BPoint to);
	void DrawString(const char* string);
	void DrawString(const char* string, int32 length);
	void DrawString(const char* string, BPoint origin);
	void DrawString(const char* string, int32 length, BPoint origin);
	void DrawBitmap(const BBitmap* bitmap);
	void DrawBitmap(const BBitmap* bitmap, BPoint at);
	void DrawBitmap(const BBitmap* bitmap, BRect srcRect, BRect dstRect);
	void InvertRect(BRect rect);
	void FillPolygon(const void* points, int32 numPoints, bool closed = false);

	void SetFont(const BFont* font);
	void SetFont(const BFont& font);
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
	void ScrollBy(BPoint offset);
	void MakeFocus(bool focused = true);
	bool IsFocus() const;
	void SetMouseEventMask(uint32 flags);
	void Sync();
	void Update();

	enum {
		B_ASYNCHRONOUS_CONTROLS = 0x00000008,
	};

private:
	friend class BScrollView;
	class BPrivateView* fPrivate;
};

#endif
