#ifndef YELLOW_UI_TOOL_BAR_H
#define YELLOW_UI_TOOL_BAR_H

#include <SupportDefs.h>
#include <String.h>
#include <View.h>
#include <vector>

namespace YellowUI {

/**
 * ToolBar - a custom flat button bar (Haiku has no BToolBar). Buttons post
 * their command message (BMessage with what = command) to the parent window
 * when clicked. Separators are visual only.
 */
class ToolBar : public BView {
public:
    ToolBar(const char* name);
    virtual ~ToolBar();

    /// Adds a button; returns its index. command is posted on click.
    int32 AddButton(const char* label, uint32 command, const char* tooltip = nullptr);
    void AddSeparator();

    int32 ButtonCount() const;
    uint32 CommandAt(int32 index) const;
    void SetButtonEnabled(int32 index, bool enabled);
    bool IsButtonEnabled(int32 index) const;

    // BView overrides
    virtual void Draw(BRect updateRect);
    virtual void MouseDown(BPoint where);
    virtual void MouseUp(BPoint where);
    virtual void MouseMoved(BPoint where, uint32 transit, const BMessage* dragMessage);
    virtual void GetPreferredSize(float* _width, float* _height);
    virtual void AttachedToWindow();

private:
    struct Item {
        bool separator;
        BString label;
        BString tooltip;
        uint32 command;
        bool enabled;
        bool pressed;
        bool hovered;
    };

    BRect ItemRect(int32 index) const;
    int32 HitTest(BPoint where) const;
    void SendCommand(int32 index);
    void UpdateHotIndex(int32 index);

    std::vector<Item> fItems;
    int32 fHotIndex;
    int32 fPressIndex;
    float fButtonHeight;
};

} // namespace YellowUI

#endif // YELLOW_UI_TOOL_BAR_H
