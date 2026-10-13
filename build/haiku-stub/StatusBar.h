// Stub of Haiku <StatusBar.h> - declarations.
#ifndef YELLOW_HAIKU_STUB_STATUSBAR_H
#define YELLOW_HAIKU_STUB_STATUSBAR_H

#include <SupportDefs.h>
#include <Rect.h>
#include <View.h>
#include <String.h>

class BStatusBar : public BView {
public:
	BStatusBar(BRect frame, const char* name, const char* label = NULL, const char* trailing = NULL);
	explicit BStatusBar(const char* name, const char* label = NULL, const char* trailing = NULL);
	virtual ~BStatusBar();

	virtual void SetText(const char* string);
	void SetValue(float value);
	float Value() const;
	void Update(float delta);
	void Reset(const char* label = NULL, const char* trailing = NULL);
	void SetTrailingText(const char* text);
	void HideBarText(bool hide);
};

#endif
