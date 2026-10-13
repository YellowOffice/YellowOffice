// Stub of Haiku <Control.h>/<Button.h>/<TextControl.h>/<CheckBox.h>/<MenuField.h> - declarations.
#ifndef YELLOW_HAIKU_STUB_CONTROL_H
#define YELLOW_HAIKU_STUB_CONTROL_H

#include <SupportDefs.h>
#include <Rect.h>
#include <InterfaceDefs.h>
#include <Message.h>

class BMenu;
#include <View.h>
#include <Handler.h>
#include <String.h>

class BControl : public BView {
public:
	BControl(BRect frame, const char* name, const char* label, BMessage* message,
		uint32 resizingMode, uint32 flags);
	BControl(const char* name, const char* label, BMessage* message, uint32 flags);
	virtual ~BControl();

	virtual void SetValue(int32 value);
	int32 Value() const;
	void SetLabel(const char* label);
	const char* Label() const;
	void SetEnabled(bool enabled);
	bool IsEnabled() const;
	virtual void Invoke();
	void SetTarget(BHandler* target);
	void SetValueNoMessage(int32 value);
};

class BButton : public BControl {
public:
	BButton(BRect frame, const char* name, const char* label, BMessage* message,
		uint32 resizingMode, uint32 flags);
	BButton(const char* name, const char* label, BMessage* message, uint32 flags);
	virtual ~BButton();
};

class BCheckBox : public BControl {
public:
	BCheckBox(BRect frame, const char* name, const char* label, BMessage* message,
		uint32 resizingMode, uint32 flags);
	BCheckBox(const char* name, const char* label, BMessage* message, uint32 flags);
	virtual ~BCheckBox();
};

class BTextControl : public BControl {
public:
	BTextControl(BRect frame, const char* name, const char* label, const char* text,
		BMessage* message, uint32 resizingMode, uint32 flags);
	BTextControl(const char* name, const char* label, const char* text,
		BMessage* message, uint32 flags);
	virtual ~BTextControl();

	void SetText(const char* text);
	const char* Text() const;
	void SetModificationMessage(BMessage* message);
	void SetDivider(float divider);
	void HideLabel(bool hide);
};

class BMenuField : public BControl {
public:
	BMenuField(BRect frame, const char* name, const char* label, BMenu* menu,
		uint32 resizingMode, uint32 flags, uint32 border = B_FANCY_BORDER);
	BMenuField(const char* name, const char* label, BMenu* menu, uint32 flags);
	virtual ~BMenuField();

	BMenu* Menu() const;
	void SetDivider(float divider);
	void SetAlignment(uint32 alignment);
};

#endif
