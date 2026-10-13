// Stub of Haiku <Size.h> - functional.
#ifndef YELLOW_HAIKU_STUB_SIZE_H
#define YELLOW_HAIKU_STUB_SIZE_H

#include <SupportDefs.h>

class BSize {
public:
	float width;
	float height;

	BSize() : width(-1), height(-1) {}
	BSize(float width, float height) : width(width), height(height) {}

	bool operator==(const BSize& other) const { return width == other.width && height == other.height; }
	bool operator!=(const BSize& other) const { return !(*this == other); }
	bool IsValid() const { return width >= 0 && height >= 0; }
};

#endif
