// Stub of Haiku <Point.h> - functional.
#ifndef YELLOW_HAIKU_STUB_POINT_H
#define YELLOW_HAIKU_STUB_POINT_H

#include <SupportDefs.h>

class BPoint {
public:
	float x;
	float y;

	BPoint() : x(0), y(0) {}
	BPoint(float inX, float inY) : x(inX), y(inY) {}

	BPoint& operator+=(const BPoint& other) { x += other.x; y += other.y; return *this; }
	BPoint& operator-=(const BPoint& other) { x -= other.x; y -= other.y; return *this; }
	BPoint& operator+=(float offset) { x += offset; y += offset; return *this; }
	BPoint& operator-=(float offset) { x -= offset; y -= offset; return *this; }
	BPoint& operator*=(float factor) { x *= factor; y *= factor; return *this; }
	BPoint& operator/=(float factor) { x /= factor; y /= factor; return *this; }

	BPoint operator-() const { return BPoint(-x, -y); }
	bool operator==(const BPoint& other) const { return x == other.x && y == other.y; }
	bool operator!=(const BPoint& other) const { return !(*this == other); }
};

inline BPoint operator+(BPoint a, const BPoint& b) { a += b; return a; }
inline BPoint operator-(BPoint a, const BPoint& b) { a -= b; return a; }
inline BPoint operator*(BPoint p, float f) { p *= f; return p; }
inline BPoint operator*(float f, BPoint p) { p *= f; return p; }
inline BPoint operator/(BPoint p, float f) { p /= f; return p; }

#endif
