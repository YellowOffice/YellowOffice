#include <YellowUI/ToolBar.h>

#include <Message.h>
#include <Window.h>
#include <GraphicsDefs.h>

namespace YellowUI {

static const float kButtonPaddingH = 10.0f;
static const float kButtonPaddingV = 6.0f;
static const float kSeparatorWidth = 9.0f;
static const float kGap = 4.0f;

ToolBar::ToolBar(const char* name)
    : BView(BRect(0, 0, 100, 40), name ? name : "ToolBar", B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP,
            B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE),
      fHotIndex(-1),
      fPressIndex(-1),
      fButtonHeight(28.0f)
{
    SetViewColor(235, 235, 235);
}

ToolBar::~ToolBar()
{
}

int32 ToolBar::AddButton(const char* label, uint32 command, const char* tooltip)
{
    Item item;
    item.separator = false;
    item.label = label ? label : "";
    item.tooltip = tooltip ? tooltip : "";
    item.command = command;
    item.enabled = true;
    item.pressed = false;
    item.hovered = false;
    fItems.push_back(item);
    Invalidate();
    return (int32)fItems.size() - 1;
}

void ToolBar::AddSeparator()
{
    Item item;
    item.separator = true;
    item.command = 0;
    item.enabled = false;
    item.pressed = false;
    item.hovered = false;
    fItems.push_back(item);
    Invalidate();
}

int32 ToolBar::ButtonCount() const
{
    return (int32)fItems.size();
}

uint32 ToolBar::CommandAt(int32 index) const
{
    if (index < 0 || index >= (int32)fItems.size()) return 0;
    return fItems[index].command;
}

void ToolBar::SetButtonEnabled(int32 index, bool enabled)
{
    if (index < 0 || index >= (int32)fItems.size()) return;
    if (fItems[index].separator) return;
    fItems[index].enabled = enabled;
    if (!enabled) {
        fItems[index].pressed = false;
        fItems[index].hovered = false;
    }
    Invalidate(ItemRect(index));
}

bool ToolBar::IsButtonEnabled(int32 index) const
{
    if (index < 0 || index >= (int32)fItems.size()) return false;
    return fItems[index].enabled;
}

BRect ToolBar::ItemRect(int32 index) const
{
    float x = kGap;
    for (int32 i = 0; i < index && i < (int32)fItems.size(); ++i) {
        const Item& item = fItems[i];
        if (item.separator) {
            x += kSeparatorWidth + kGap;
        } else {
            // Approximate label width: 7px per character plus padding.
            x += item.label.Length() * 7.0f + 2.0f * kButtonPaddingH + kGap;
        }
    }
    const Item& item = fItems[index];
    float width = item.separator
        ? kSeparatorWidth
        : item.label.Length() * 7.0f + 2.0f * kButtonPaddingH;
    float top = (fButtonHeight > 0 ? (Bounds().Height() - fButtonHeight) / 2.0f : 4.0f);
    if (top < 2.0f) top = 2.0f;
    return BRect(x, top, x + width, top + fButtonHeight);
}

int32 ToolBar::HitTest(BPoint where) const
{
    for (int32 i = 0; i < (int32)fItems.size(); ++i) {
        if (!fItems[i].separator && ItemRect(i).Contains(where)) return i;
    }
    return -1;
}

void ToolBar::SendCommand(int32 index)
{
    if (index < 0 || index >= (int32)fItems.size()) return;
    if (fItems[index].separator || !fItems[index].enabled) return;
    BMessage message(fItems[index].command);
    if (Window()) {
        Window()->PostMessage(&message);
    }
}

void ToolBar::UpdateHotIndex(int32 index)
{
    if (fHotIndex == index) return;
    int32 old = fHotIndex;
    fHotIndex = index;
    if (old >= 0 && old < (int32)fItems.size()) {
        fItems[old].hovered = false;
        Invalidate(ItemRect(old));
    }
    if (fHotIndex >= 0) {
        fItems[fHotIndex].hovered = true;
        Invalidate(ItemRect(fHotIndex));
    }
}

void ToolBar::AttachedToWindow()
{
    SetViewColor(235, 235, 235);
    Invalidate();
}

void ToolBar::Draw(BRect updateRect)
{
    (void)updateRect;

    // Top and bottom hairlines.
    rgb_color line = { 190, 190, 190, 255 };
    rgb_color shadow = { 255, 255, 255, 255 };
    SetHighColor(line);
    StrokeLine(BPoint(0, 0), BPoint(Bounds().right, 0));
    SetHighColor(shadow);
    StrokeLine(BPoint(0, Bounds().bottom), BPoint(Bounds().right, Bounds().bottom));

    for (int32 i = 0; i < (int32)fItems.size(); ++i) {
        const Item& item = fItems[i];
        BRect rect = ItemRect(i);

        if (item.separator) {
            float midX = rect.left + rect.Width() / 2.0f;
            SetHighColor(line);
            StrokeLine(BPoint(midX, rect.top + 4.0f),
                       BPoint(midX, rect.bottom - 4.0f));
            continue;
        }

        // Background.
        rgb_color fill = { 235, 235, 235, 255 };
        if (!item.enabled) {
            fill = { 235, 235, 235, 255 };
        } else if (item.pressed) {
            fill = { 180, 180, 180, 255 };
        } else if (item.hovered) {
            fill = { 210, 210, 210, 255 };
        }
        SetHighColor(fill);
        FillRect(rect);

        // Border when hovered or pressed.
        if (item.enabled && (item.hovered || item.pressed)) {
            rgb_color border = { 120, 120, 120, 255 };
            SetHighColor(border);
            StrokeRect(rect.InsetByCopy(0.5f, 0.5f));
        }

        // Label.
        rgb_color textColor = item.enabled
            ? rgb_color{ 20, 20, 20, 255 }
            : rgb_color{ 150, 150, 150, 255 };
        SetHighColor(textColor);
        // Rough centering: 7px per character, 12px baseline.
        float textWidth = item.label.Length() * 7.0f;
        float x = rect.left + (rect.Width() - textWidth) / 2.0f;
        float y = rect.top + rect.Height() / 2.0f + 4.0f;
        if (!item.label.IsEmpty()) {
            DrawString(item.label.String(), BPoint(x, y));
        }
    }
}

void ToolBar::MouseDown(BPoint where)
{
    int32 index = HitTest(where);
    if (index < 0 || !fItems[index].enabled) return;
    fPressIndex = index;
    fItems[index].pressed = true;
    Invalidate(ItemRect(index));
}

void ToolBar::MouseUp(BPoint where)
{
    if (fPressIndex < 0) return;
    int32 pressed = fPressIndex;
    fPressIndex = -1;
    fItems[pressed].pressed = false;
    Invalidate(ItemRect(pressed));

    int32 index = HitTest(where);
    if (index == pressed) {
        SendCommand(index);
    }
}

void ToolBar::MouseMoved(BPoint where, uint32 transit, const BMessage* dragMessage)
{
    (void)transit;
    (void)dragMessage;
    UpdateHotIndex(HitTest(where));
}

void ToolBar::GetPreferredSize(float* _width, float* _height)
{
    float width = 2 * kGap;
    for (size_t i = 0; i < fItems.size(); ++i) {
        if (fItems[i].separator) {
            width += kSeparatorWidth + kGap;
        } else {
            width += fItems[i].label.Length() * 7.0f + 2.0f * kButtonPaddingH + kGap;
        }
    }
    if (_width) *_width = width;
    if (_height) *_height = fButtonHeight + 2.0f * kButtonPaddingV;
}

} // namespace YellowUI
