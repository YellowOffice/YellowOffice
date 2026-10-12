#ifndef YELLOW_UI_FORMATTING_PANEL_H
#define YELLOW_UI_FORMATTING_PANEL_H

#include <SupportDefs.h>
#include <View.h>

namespace YellowUI {

class FormattingPanel : public BView {
public:
    FormattingPanel(const char* name);
    virtual ~FormattingPanel();

    // BView overrides
    virtual void Draw(BRect updateRect);
    virtual void MessageReceived(BMessage* message);

private:
};

} // namespace YellowUI

#endif // YELLOW_UI_FORMATTING_PANEL_H
