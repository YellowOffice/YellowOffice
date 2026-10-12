#ifndef YELLOW_UI_STATUS_BAR_H
#define YELLOW_UI_STATUS_BAR_H

#include <SupportDefs.h>
#include <View.h>

namespace YellowUI {

class StatusBar : public BView {
public:
    StatusBar(const char* name);
    virtual ~StatusBar();

    // BView overrides
    virtual void Draw(BRect updateRect);
    virtual void MessageReceived(BMessage* message);

    // Status updates
    void SetStatusText(const char* text);
    void UpdateStatus();

private:
    BString fStatusText;
};

} // namespace YellowUI

#endif // YELLOW_UI_STATUS_BAR_H
