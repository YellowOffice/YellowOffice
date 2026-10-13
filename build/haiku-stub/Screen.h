// Stub of Haiku <Screen.h> - declarations.
#ifndef YELLOW_HAIKU_STUB_SCREEN_H
#define YELLOW_HAIKU_STUB_SCREEN_H

#include <SupportDefs.h>
#include <Rect.h>
#include <GraphicsDefs.h>

class BWindow;

enum screen_id {
	B_MAIN_SCREEN_ID = -1,
	B_NO_SCREEN_ID = -2,
	B_ALL_SCREENS = -3,
};

class BScreen {
public:
	BScreen();
	explicit BScreen(BWindow* window);
	explicit BScreen(screen_id id);
	~BScreen();

	BRect Frame() const;
	rgb_color ColorAt(int32 x, int32 y);
	status_t SetDesktopColor(rgb_color color);
	status_t WaitForRetrace(bigtime_t* period = NULL) const;
	static int32 Number();
	static int32 NumberOfScreens();

private:
	BRect fFrame;
};

#endif
