// Stub of Haiku <Rect.h> - functional.
#ifndef YELLOW_HAIKU_STUB_RECT_H
#define YELLOW_HAIKU_STUB_RECT_H

#include <SupportDefs.h>
#include <Point.h>

class BRect {
public:
	float left;
	float top;
	float right;
	float bottom;

	BRect() : left(0), top(0), right(-1), bottom(-1) {}
	BRect(float left, float top, float right, float bottom)
		: left(left), top(top), right(right), bottom(bottom) {}
	BRect(BPoint leftTop, BPoint rightBottom)
		: left(leftTop.x), top(leftTop.y), right(rightBottom.x), bottom(rightBottom.y) {}

	float Width() const { return right - left + 1; }
	float Height() const { return bottom - top + 1; }
	BPoint LeftTop() const { return BPoint(left, top); }
	BPoint RightBottom() const { return BPoint(right, bottom); }
	BPoint Center() const { return BPoint((left + right) / 2, (top + bottom) / 2); }

	bool operator==(const BRect& other) const {
		return left == other.left && top == other.top && right == other.right && bottom == other.bottom;
	}
	bool operator!=(const BRect& other) const { return !(*this == other); }

	bool IsValid() const { return right >= left && bottom >= top; }
	bool Contains(const BPoint& point) const {
		return point.x >= left && point.x <= right && point.y >= top && point.y <= bottom;
	}
	bool Intersects(const BRect& other) const {
		return left <= other.right && right >= other.left && top <= other.bottom && bottom >= other.top;
	}
	void OffsetBy(float dx, float dy) { left += dx; top += dy; right += dx; bottom += dy; }
	void OffsetBy(BPoint offset) { OffsetBy(offset.x, offset.y); }
	void InflateBy(float dx, float dy) { left -= dx; top -= dy; right += dx; bottom += dy; }
	BRect InsetByCopy(float dx, float dy) const {
		BRect result(*this);
		result.InflateBy(dx, dy);
		return result;
	}
};

#endif
