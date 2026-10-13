// Stub of Haiku <Font.h> - functional (deterministic fake metrics for host tests).
#ifndef YELLOW_HAIKU_STUB_FONT_H
#define YELLOW_HAIKU_STUB_FONT_H

#include <SupportDefs.h>
#include <GraphicsDefs.h>
#include <String.h>
#include <cstring>

class BFont {
public:
	BFont() : fSize(12.0f), fFlags(0), fFace(B_REGULAR_FACE), fShear(0.0f), fSpacing(B_CHAR_SPACING) {
		std::strcpy(fFamily, "Noto Sans");
		std::strcpy(fStyle, "Regular");
	}
	BFont(const BFont& other) = default;
	BFont& operator=(const BFont& other) = default;

	void SetSize(float size) { fSize = size; }
	float Size() const { return fSize; }

	void SetFlags(uint32 flags) { fFlags = flags; }
	uint32 Flags() const { return fFlags; }

	void SetFace(uint32 face) { fFace = face; }
	uint32 Face() const { return fFace; }

	status_t SetFamilyAndStyle(const font_family family, const font_style style) {
		std::strncpy(fFamily, family, sizeof(fFamily) - 1);
		fFamily[sizeof(fFamily) - 1] = '\0';
		std::strncpy(fStyle, style, sizeof(fStyle) - 1);
		fStyle[sizeof(fStyle) - 1] = '\0';
		return B_OK;
	}
	void SetFamilyAndStyle(uint32 code) { (void)code; }
	status_t SetFamilyAndFace(const font_family family, uint32 face) {
		std::strncpy(fFamily, family, sizeof(fFamily) - 1);
		fFamily[sizeof(fFamily) - 1] = '\0';
		fFace = face;
		return B_OK;
	}
	void GetFamilyAndStyle(font_family family, font_style style) const {
		std::strncpy(family, fFamily, 63);
		family[63] = '\0';
		std::strncpy(style, fStyle, 63);
		style[63] = '\0';
	}
	const char* Family() const { return fFamily; }
	const char* Style() const { return fStyle; }

	void SetShear(float shear) { fShear = shear; }
	float Shear() const { return fShear; }
	void SetRotation(float rotation) { (void)rotation; }
	void SetSpacing(uint32 spacing) { fSpacing = spacing; }

	// Deterministic stub metrics: 0.5 * size per byte.
	float StringWidth(const char* string) const {
		if (string == nullptr) return 0.0f;
		return (float)strlen(string) * fSize * 0.5f;
	}
	float StringWidth(const char* string, int32 length) const {
		if (string == nullptr || length <= 0) return 0.0f;
		return (float)length * fSize * 0.5f;
	}
	float StringWidth(const BString& string) const { return StringWidth(string.String()); }

	void GetHeight(font_height* height) const {
		if (height == nullptr) return;
		height->ascent = fSize * 0.8f;
		height->descent = fSize * 0.2f;
		height->leading = fSize * 0.1f;
	}

	bool operator==(const BFont& other) const {
		return fSize == other.fSize && fFlags == other.fFlags && fFace == other.fFace
			&& strcmp(fFamily, other.fFamily) == 0;
	}
	bool operator!=(const BFont& other) const { return !(*this == other); }

private:
	char fFamily[64];
	char fStyle[64];
	float fSize;
	uint32 fFlags;
	uint32 fFace;
	float fShear;
	uint32 fSpacing;
};

#endif
