// Stub of Haiku <InterfaceDefs.h> - functional constants.
#ifndef YELLOW_HAIKU_STUB_INTERFACEDEFS_H
#define YELLOW_HAIKU_STUB_INTERFACEDEFS_H

#include <SupportDefs.h>
#include <GraphicsDefs.h>

enum border_style {
	B_BORDERED = 0,
	B_PLAIN_BORDER = 0,
	B_FANCY_BORDER = 1,
	B_NO_BORDER = 2,
};

// Shared orientation constants (Haiku defines these once, used for menus and layouts).
enum orientation_constants {
	B_HORIZONTAL = 0x00000001,
	B_VERTICAL = 0x00000002,
};

enum border_type {
	B_FANCY_BORDER_T = 1,
};

enum button_width {
	B_WIDTH_AS_USUAL,
	B_WIDTH_FROM_WIDEST,
	B_WIDTH_FROM_LABEL,
};

enum button_spacing {
	B_EVEN_SPACING,
	B_OFFSET_SPACING,
};

enum alert_type {
	B_EMPTY_ALERT = 0,
	B_IDEA_ALERT = 1,
	B_INFO_ALERT = 2,
	B_WARNING_ALERT = 3,
	B_STOP_ALERT = 4,
};

enum menu_properties {
	B_TOP_LEVEL = 1,
	B_SUBMENU = 2,
	B_POP_UP_MENU = 3,
};

enum {
	B_LEFT_MOUSE_BUTTON = 0x01,
	B_RIGHT_MOUSE_BUTTON = 0x02,
	B_MIDDLE_MOUSE_BUTTON = 0x04,
};

enum {
	B_SHIFT_KEY = 0x0001,
	B_CONTROL_KEY = 0x0002,
	B_COMMAND_KEY = 0x0004,
	B_CAPS_LOCK = 0x0008,
	B_SCROLL_LOCK = 0x0010,
	B_NUM_LOCK = 0x0020,
	B_OPTION_KEY = 0x0040,
	B_MENU_KEY = 0x0080,
	B_LEFT_COMMAND_KEY = 0x0004,
	B_RIGHT_COMMAND_KEY = 0x0004,
	B_LEFT_SHIFT_KEY = 0x0001,
	B_RIGHT_SHIFT_KEY = 0x0001,
	B_LEFT_OPTION_KEY = 0x0040,
	B_RIGHT_OPTION_KEY = 0x0040,
	B_LEFT_CONTROL_KEY = 0x0002,
	B_RIGHT_CONTROL_KEY = 0x0002,
};

enum {
	B_KEY_DOWN = 0x400,
	B_KEY_UP = 0x401,
	B_UNMAPPED_KEY_DOWN = 0x404,
	B_MOUSE_DOWN = 0x402,
	B_MOUSE_UP = 0x403,
	B_MOUSE_MOVED = 0x405,
	B_MOUSE_DRAGGED = 0x406,
	B_MOUSE_IDLE = 0x407,
	B_PULSE = 0x408,
	B_WINDOW_ACTIVATED = 0x409,
	B_WORKSPACES_CHANGED = 0x40a,
	B_SCREEN_CHANGED = 0x40b,
	B_MONITOR_CHANGED = 0x40c,
	B_INTERNAL_QUIT = 0x40d,
};

// Standard window/application message constant.
enum {
	B_QUIT_REQUESTED = 0x51524551, // 'QREQ'
};

enum {
	B_BACKSPACE = 0x08,
	B_ENTER = 0x0a,
	B_SPACE = 0x20,
	B_TAB = 0x09,
	B_ESCAPE = 0x1b,
	B_LEFT_ARROW = 0x1c,
	B_RIGHT_ARROW = 0x1d,
	B_UP_ARROW = 0x1e,
	B_DOWN_ARROW = 0x1f,
	B_INSERT = 0x05,
	B_DELETE = 0x7f,
	B_HOME = 0x01,
	B_END = 0x04,
	B_PAGE_UP = 0x0b,
	B_PAGE_DOWN = 0x0c,
	B_FUNCTION_KEY = 0x10,
};

enum {
	B_FOLLOW_LEFT = 0x00000001,
	B_FOLLOW_TOP = 0x00000002,
	B_FOLLOW_RIGHT = 0x00000004,
	B_FOLLOW_BOTTOM = 0x00000008,
	B_FOLLOW_H_CENTER = 0x00000010,
	B_FOLLOW_V_CENTER = 0x00000020,
	B_FOLLOW_LEFT_TOP = B_FOLLOW_LEFT | B_FOLLOW_TOP,
	B_FOLLOW_RIGHT_TOP = B_FOLLOW_RIGHT | B_FOLLOW_TOP,
	B_FOLLOW_LEFT_BOTTOM = B_FOLLOW_LEFT | B_FOLLOW_BOTTOM,
	B_FOLLOW_RIGHT_BOTTOM = B_FOLLOW_RIGHT | B_FOLLOW_BOTTOM,
	B_FOLLOW_ALL = B_FOLLOW_LEFT | B_FOLLOW_TOP | B_FOLLOW_RIGHT | B_FOLLOW_BOTTOM,
	B_FOLLOW_ALL_SIDES = B_FOLLOW_ALL,
	B_FOLLOW_LEFT_RIGHT = B_FOLLOW_LEFT | B_FOLLOW_RIGHT,
	B_FOLLOW_TOP_BOTTOM = B_FOLLOW_TOP | B_FOLLOW_BOTTOM,
	B_FOLLOW_NONE = 0x00000000,
};

enum {
	B_WILL_DRAW = 0x00000001,
	B_NAVIGABLE = 0x00000002,
	B_PULSE_NEEDED = 0x00000004,
	B_REQUESTS_PAINT = 0x00000008,
	B_FRAME_EVENTS = 0x00000020,
};

enum {
	B_FULL_UPDATE_ON_RESIZE = 0x00000008,
	B_DRAW_ON_CHILDREN = 0x00000010,
	B_INPUT_METHOD_AWARE = 0x00001000,
};

enum ui_colors {
	B_PANEL_BACKGROUND_COLOR = 100,
	B_PANEL_TEXT_COLOR = 101,
	B_DOCUMENT_BACKGROUND_COLOR = 102,
	B_DOCUMENT_TEXT_COLOR = 103,
	B_MENU_BACKGROUND_COLOR = 104,
	B_MENU_SELECTED_BACKGROUND_COLOR = 105,
	B_MENU_SELECTED_TEXT_COLOR = 106,
	B_TOOLTIP_BACKGROUND_COLOR = 107,
	B_TOOLTIP_TEXT_COLOR = 108,
	B_SHINE_COLOR = 109,
	B_SCROLLBAR_THUMB_COLOR = 118,
};

enum {
	B_FONT_FAMILY_NAME_LENGTH = 64,
	B_FONT_STYLE_NAME_LENGTH = 64,
};

inline rgb_color ui_color(ui_colors /*which*/) {
	rgb_color c = { 240, 240, 240, 255 };
	return c;
}

inline rgb_color tint_color(const rgb_color& base, float /*tint*/) { return base; }

enum {
	B_TINTED = 0,
	B_DARKENED_TINT = 1,
	B_LIGHTENED_TINT = 2,
};

struct color_table {
	rgb_color background;
	rgb_color line;
	rgb_color shade;
	rgb_color highlight;
	rgb_color border;
	rgb_color shadow;
	rgb_color marker;
	rgb_color selected;
	rgb_color selected_text;
};

// Standard clipboard flavor (real Haiku defines this in InterfaceDefs.h).
#define B_TEXT_FLAVOR "text/plain"

#endif
