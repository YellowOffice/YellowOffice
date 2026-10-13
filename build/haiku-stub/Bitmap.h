// Stub of Haiku <Bitmap.h> - declarations.
#ifndef YELLOW_HAIKU_STUB_BITMAP_H
#define YELLOW_HAIKU_STUB_BITMAP_H

#include <SupportDefs.h>
#include <Rect.h>
#include <GraphicsDefs.h>
#include <Archivable.h>
#include <String.h>

class BRect;

class BBitmap : public BArchivable {
public:
	BBitmap(const BRect& bounds, bool acceptsChildren = false,
		color_space colorSpace = B_RGBA32);
	BBitmap(const BRect& bounds, uint32 flags, color_space colorSpace = B_RGBA32,
		bool acceptsChildren = false);
	BBitmap(const char* path, color_space colorSpace = B_RGBA32,
		uint32 flags = 0);
	virtual ~BBitmap();

	status_t InitCheck() const;
	status_t Load(const char* path);
	status_t Save(const char* path) const;
	void* Bits() const;
	long BitsLength() const;
	color_space ColorSpace() const;
	BRect Bounds() const;
	status_t GetBitmap(BBitmap** bitmap) const;
	status_t ImportBits(const BBitmap* other);
	status_t SetBits(const void* buffer, long length, int32 bitsOffset, color_space colorSpace);
	status_t CopyFrom(const BBitmap* srcBitmap, BRect srcRect);

	enum {
		B_BITMAP_ACCEPTS_CHILDREN = 1 << 0,
		B_BITMAP_LOCK_SURFACE = 1 << 1,
		B_BITMAP_IS_CONTIGUOUS = 1 << 2,
		B_BITMAP_IS_OFFSCREEN = 1 << 3,
		B_BITMAP_NO_SHARE = 1 << 5,
	};
};

#endif
