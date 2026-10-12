#ifndef YELLOW_UI_TOOL_BAR_H
#define YELLOW_UI_TOOL_BAR_H

#include <SupportDefs.h>
#include <View.h>

namespace YellowUI {

class ToolBar : public BView {
public:
    ToolBar(const char* name);
    virtual ~ToolBar();

    // BView overrides
    virtual void Draw(BRect updateRect);
    virtual void MessageReceived(BMessage* message);

private:
};

} // namespace YellowUI

#endif // YELLOW_UI_TOOL_BAR_H
