// Stub of Haiku <Alert.h> - declarations.
#ifndef YELLOW_HAIKU_STUB_ALERT_H
#define YELLOW_HAIKU_STUB_ALERT_H

#include <SupportDefs.h>
#include <InterfaceDefs.h>
#include <Window.h>
#include <String.h>

class BAlert : public BWindow {
public:
	BAlert(const char* title, const char* text, const char* button1,
		const char* button2 = NULL, const char* button3 = NULL,
		button_width width = B_WIDTH_AS_USUAL, alert_type type = B_STOP_ALERT);
	BAlert(const char* title, const char* text, const char* button1,
		const char* button2, const char* button3, button_width width,
		button_spacing spacing, alert_type type = B_STOP_ALERT);
	virtual ~BAlert();

	virtual void MessageReceived(BMessage* message);
	int32 Go(bool modal = true);
	void SetShortcut(int32 index, char key, uint32 modifiers = 0);
	void SetText(const char* text);
	void SetButtons(const char* button1, const char* button2 = NULL, const char* button3 = NULL);
	void SetType(alert_type type);
	int32 CountButtons() const;
};

#endif
