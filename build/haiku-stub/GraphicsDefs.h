// Stub of Haiku <GraphicsDefs.h> - functional.
#ifndef YELLOW_HAIKU_STUB_GRAPHICSDEFS_H
#define YELLOW_HAIKU_STUB_GRAPHICSDEFS_H

#include <SupportDefs.h>

struct rgb_color {
	uint8 red;
	uint8 green;
	uint8 blue;
	uint8 alpha;

	bool operator==(const rgb_color& other) const {
		return red == other.red && green == other.green && blue == other.blue && alpha == other.alpha;
	}
	bool operator!=(const rgb_color& other) const { return !(*this == other); }
};

inline rgb_color make_color(uint8 r, uint8 g, uint8 b, uint8 a = 255) {
	rgb_color c = { r, g, b, a };
	return c;
}

#define make_rgb_color make_color

enum color_space {
	B_NO_COLOR_SPACE = 0x00000000,
	B_RGB32 = 0x14,
	B_RGBA32 = 0x15,
	B_RGB32_BIG = 0x18,
	B_GRAYSCALE_8 = 0x19,
};

typedef char font_family[64];
typedef char font_style[64];

enum font_family_constants {
	B_FAMILY_COUNT = 0,
};

enum font_style_constants {
	B_STYLE_COUNT = 0,
	B_BOLD_FACE = 0x00000001,
	B_ITALIC_FACE = 0x00000002,
	B_BOLD_ITALIC_FACE = 0x00000003,
	B_UNDERSCORE_FACE = 0x00000004,
	B_STRIKEOUT_FACE = 0x00000020,
	B_REGULAR_FACE = 0x00000000,
};

enum font_flags {
	B_DISABLE_FONT_ANTIALIASING = 0x00000001,
	B_DISABLE_FONT_LIGATURES = 0x00000004,
	B_FORCE_GLYPH_SPACING = 0x00000010,
};

enum font_spacing {
	B_SPACING = 0,
	B_CHAR_SPACING = 1,
	B_STRING_SPACING = 2,
	B_GLYPH_SPACING = 3,
};

enum drawing_mode {
	B_OP_COPY = 0x00,
	B_OP_AND = 0x01,
	B_OP_ADD = 0x02,
	B_OP_SUBTRACT = 0x03,
	B_OP_BLEND = 0x05,
	B_OP_INVERT = 0x06,
	B_OP_MIN = 0x07,
	B_OP_MAX = 0x08,
	B_OP_SELECT = 0x09,
	B_OP_ALPHA = 0x0a,
};

enum line_mode {
	B_BUTT_CAP = 0x00,
	B_ROUND_CAP = 0x01,
	B_SQUARE_CAP = 0x02,
	B_MITER_JOIN = 10,
	B_ROUND_JOIN = 11,
	B_BEVEL_JOIN = 12,
};

enum gradient_type {
	B_RADIAL_GRADIENT = 0,
	B_LINEAR_GRADIENT = 1,
};

struct font_height {
	float ascent;
	float descent;
	float leading;
};

struct escapement_delta {
	float n;
	float s;
};

struct tuned_font_info {
	float face_size;
	uint32 flags;
	float shear;
	float rotation;
};

struct pattern {
	uint8 data[8];
};

struct gradient_data {
	int32 angle;
	int32 radius;
	rgb_color solid_colors[2];
	struct {
		float offset;
		rgb_color color;
	} gradient_colors[2];
};

struct gradient {
	gradient_data data;
};

#endif
