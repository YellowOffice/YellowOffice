#ifndef YELLOW_UI_STYLE_PANEL_H
#define YELLOW_UI_STYLE_PANEL_H

#include <SupportDefs.h>
#include <View.h>

namespace YellowUI {

class StylePanel : public BView {
public:
    StylePanel(const char* name);
    virtual ~StylePanel();

    // BView overrides
    virtual void Draw(BRect updateRect);
    virtual void MessageReceived(BMessage* message);

private:
};

} // namespace YellowUI

#endif // YELLOW_UI_STYLE_PANEL_H
