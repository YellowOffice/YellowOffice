#ifndef YELLOW_DOCUMENT_LAYOUT_RENDER_CONTEXT_H
#define YELLOW_DOCUMENT_LAYOUT_RENDER_CONTEXT_H

#include <YellowDocumentLayout/LayoutTypes.h>

class BView;

namespace YellowDocumentLayout {

class LayoutEngine;

/**
 * RenderContext draws the laid-out document into a BView: pages, text,
 * selection, caret, tables and image placeholders.
 */
class RenderContext {
public:
    explicit RenderContext(LayoutEngine* engine);
    ~RenderContext();

    void RenderDocument(BView* view, const BRect& updateRect);
    void RenderCaret(BView* view);

    /// Draw a page background/shadow if the page intersects updateRect.
    void RenderPageFrame(BView* view, const PageInfo& page, const BRect& updateRect);

    void RenderParagraph(BView* view, const ParagraphLayout& layout,
                         const BRect& updateRect);
    void RenderSelection(BView* view, const BRect& updateRect);
    void RenderTable(BView* view, const PlacedTable& table, const BRect& updateRect);
    void RenderImage(BView* view, const PlacedImage& image, const BRect& updateRect);
    void RenderListMarker(BView* view, const ParagraphLayout& layout);

    /// Sets the view font/color from a text style.
    static void ApplyTextStyle(BView* view, const TextStyle& style);
    /// Draws underline/strikethrough decorations for a laid-out run.
    void DrawDecorations(BView* view, const LineRun& run, float x, float baseline);

private:
    LayoutEngine* fEngine;
};

} // namespace YellowDocumentLayout

#endif
