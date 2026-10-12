#ifndef YELLOW_DOCUMENT_LAYOUT_RENDER_CONTEXT_H
#define YELLOW_DOCUMENT_LAYOUT_RENDER_CONTEXT_H

#include <SupportDefs.h>
#include <Rect.h>

namespace YellowDocumentLayout {

class RenderContext {
public:
    RenderContext();
    ~RenderContext();

    // Render state
    void SetBounds(const BRect& bounds);
    BRect GetBounds() const;
    
    // Clipping
    void SetClipRect(const BRect& clip);
    BRect GetClipRect() const;
    
    // Drawing state
    void SaveState();
    void RestoreState();

private:
    BRect fBounds;
    BRect fClipRect;
};

} // namespace YellowDocumentLayout

#endif // YELLOW_DOCUMENT_LAYOUT_RENDER_CONTEXT_H
