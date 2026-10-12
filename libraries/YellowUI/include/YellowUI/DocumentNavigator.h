#ifndef YELLOW_UI_DOCUMENT_NAVIGATOR_H
#define YELLOW_UI_DOCUMENT_NAVIGATOR_H

#include <SupportDefs.h>
#include <View.h>

namespace YellowUI {

class DocumentNavigator : public BView {
public:
    DocumentNavigator(const char* name);
    virtual ~DocumentNavigator();

    // BView overrides
    virtual void Draw(BRect updateRect);
    virtual void MessageReceived(BMessage* message);

private:
};

} // namespace YellowUI

#endif // YELLOW_UI_DOCUMENT_NAVIGATOR_H
