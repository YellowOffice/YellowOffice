#include <YellowDocumentLayout/RenderContext.h>
#include <YellowDocumentLayout/LayoutEngine.h>
#include <YellowDocumentLayout/HitTester.h>
#include <YellowDocumentLayout/CaretManager.h>
#include <YellowDocumentLayout/SelectionManager.h>

#include <View.h>
#include <Font.h>

#include <stdio.h>

namespace YellowDocumentLayout {

RenderContext::RenderContext(LayoutEngine* engine)
    : fEngine(engine)
{
}

RenderContext::~RenderContext()
{
}

void RenderContext::ApplyTextStyle(BView* view, const TextStyle& style)
{
    if (!view) return;
    BFont font;
    view->GetFont(&font);
    if (style.fontFamily.Length() > 0 && style.fontFamily != "System") {
        font.SetFamilyAndStyle(style.fontFamily.String(), "Regular");
    }
    font.SetSize(style.fontSize > 0 ? style.fontSize : 12.0f);
    uint32 face = B_REGULAR_FACE;
    if (style.bold && style.italic) face = B_BOLD_ITALIC_FACE;
    else if (style.bold) face = B_BOLD_FACE;
    else if (style.italic) face = B_ITALIC_FACE;
    font.SetFace(face);
    view->SetFont(font);
    view->SetHighColor(style.textColor);
}

void RenderContext::RenderPageFrame(BView* view, const PageInfo& page, const BRect& updateRect)
{
    if (!view) return;
    BRect viewRect = fEngine->DocumentToView(page.bounds);
    if (!viewRect.Intersects(updateRect)) return;

    // Drop shadow.
    BRect shadow(viewRect.left + 3, viewRect.top + 3,
                 viewRect.right + 3, viewRect.bottom + 3);
    view->SetHighColor(180, 180, 180, 255);
    view->FillRect(shadow);

    // Page background.
    view->SetHighColor(255, 255, 255, 255);
    view->FillRect(viewRect);
    view->SetHighColor(200, 200, 200, 255);
    view->StrokeRect(viewRect);
}

void RenderContext::DrawDecorations(BView* view, const LineRun& run, float x, float baseline)
{
    if (!view) return;
    if (run.style.underline) {
        float y = baseline + 1.5f;
        view->StrokeLine(BPoint(x, y), BPoint(x + run.width, y));
    }
    if (run.style.strikethrough) {
        float y = baseline - run.style.fontSize * 0.25f;
        view->StrokeLine(BPoint(x, y), BPoint(x + run.width, y));
    }
}

void RenderContext::RenderParagraph(BView* view, const ParagraphLayout& layout,
                                    const BRect& updateRect)
{
    if (!view || !layout.paragraph) return;

    BRect bounds(layout.origin.x, layout.origin.y,
                 layout.origin.x + layout.width, layout.origin.y + layout.height);
    BRect viewBounds = fEngine->DocumentToView(bounds);
    if (!viewBounds.Intersects(updateRect)) return;

    // Paragraph background.
    const ParagraphStyle& style = layout.paragraph->style;
    if (style.backgroundColor.alpha > 0) {
        view->SetHighColor(style.backgroundColor);
        view->FillRect(viewBounds);
    }

    for (const LineFragment& line : layout.lines) {
        float x = layout.origin.x + line.x;
        float lineTop = layout.origin.y + layout.paragraph->style.spaceBefore + line.y;

        for (const LineRun& run : line.runs) {
            if (run.length <= 0) continue;
            BPoint viewPoint = fEngine->DocumentToView(BPoint(x, lineTop + line.baseline));
            ApplyTextStyle(view, run.style);
            view->DrawString(run.text.String(), run.length, viewPoint);
            DrawDecorations(view, run, viewPoint.x, viewPoint.y);
            x += run.width;
        }
    }
}

void RenderContext::RenderListMarker(BView* view, const ParagraphLayout& layout)
{
    if (!view || !layout.paragraph) return;
    BString marker = ParagraphLayouter::ListMarkerFor(layout.paragraph);
    if (marker.IsEmpty()) return;

    ParagraphLayouter* layouter = fEngine->GetParagraphLayouter();
    float listIndent = ParagraphLayouter::ListIndentFor(layout.paragraph);
    (void)layouter;
    float markerX = layout.origin.x + layout.paragraph->style.leftIndent +
                    layout.paragraph->style.leftMargin + listIndent - 16.0f;
    float lineTop = layout.origin.y + layout.paragraph->style.spaceBefore;
    float baseline = layout.lines.empty() ? 12.0f : layout.lines[0].baseline;

    BPoint viewPoint = fEngine->DocumentToView(BPoint(markerX, lineTop + baseline));
    TextStyle markerStyle; // markers use the default UI font metrics
    ApplyTextStyle(view, markerStyle);
    view->DrawString(marker.String(), viewPoint);
}

void RenderContext::RenderSelection(BView* view, const BRect& updateRect)
{
    if (!view) return;
    SelectionManager* selection = fEngine->GetSelectionManager();
    if (selection->IsEmpty()) return;

    std::vector<BRect> rects;
    fEngine->GetHitTester()->SelectionRects(selection->GetRange(), rects);

    view->SetDrawingMode(B_OP_ALPHA);
    for (const BRect& rect : rects) {
        BRect viewRect = fEngine->DocumentToView(rect);
        if (!viewRect.Intersects(updateRect)) continue;
        view->SetHighColor(120, 170, 255, 110);
        view->FillRect(viewRect);
    }
    view->SetDrawingMode(B_OP_COPY);
}

void RenderContext::RenderTable(BView* view, const PlacedTable& table, const BRect& updateRect)
{
    if (!view) return;
    BRect viewBounds = fEngine->DocumentToView(table.bounds);
    if (!viewBounds.Intersects(updateRect)) return;

    view->SetHighColor(60, 60, 60, 255);
    view->StrokeRect(fEngine->DocumentToView(table.bounds));

    // Vertical lines.
    float x = table.bounds.left;
    for (size_t i = 0; i + 1 < table.columnWidths.size(); ++i) {
        x += table.columnWidths[i];
        BPoint top = fEngine->DocumentToView(BPoint(x, table.bounds.top));
        BPoint bottom = fEngine->DocumentToView(BPoint(x, table.bounds.bottom));
        view->StrokeLine(top, bottom);
    }

    // Horizontal lines.
    float y = table.bounds.top;
    for (size_t i = 0; i < table.rowHeights.size(); ++i) {
        y += table.rowHeights[i];
        BPoint left = fEngine->DocumentToView(BPoint(table.bounds.left, y));
        BPoint right = fEngine->DocumentToView(BPoint(table.bounds.right, y));
        view->StrokeLine(left, right);
    }
}

void RenderContext::RenderImage(BView* view, const PlacedImage& image, const BRect& updateRect)
{
    if (!view || !image.image) return;
    BRect viewRect = fEngine->DocumentToView(image.bounds);
    if (!viewRect.Intersects(updateRect)) return;

    view->SetHighColor(245, 245, 245, 255);
    view->FillRect(viewRect);
    view->SetHighColor(140, 140, 140, 255);
    view->SetPenSize(1.0f);
    view->StrokeRect(viewRect);

    BString label = image.image->altText;
    if (label.IsEmpty()) {
        label = image.image->mimeType.Length() > 0 ? image.image->mimeType : "Image";
    }
    BFont font;
    view->GetFont(&font);
    font.SetSize(10.0f);
    view->SetFont(font);
    view->SetHighColor(100, 100, 100, 255);
    BPoint textPoint(viewRect.left + 6, viewRect.top + 14);
    view->DrawString(label.String(), textPoint);
}

void RenderContext::RenderDocument(BView* view, const BRect& updateRect)
{
    if (!view) return;
    fEngine->EnsureLayout();

    // Canvas background.
    view->SetHighColor(220, 220, 220, 255);
    view->FillRect(updateRect);

    for (const PageInfo& page : fEngine->GetPages()) {
        RenderPageFrame(view, page, updateRect);
    }
    for (const ParagraphLayout& layout : fEngine->GetParagraphs()) {
        RenderParagraph(view, layout, updateRect);
        RenderListMarker(view, layout);
    }
    for (const PlacedTable& table : fEngine->GetTables()) {
        RenderTable(view, table, updateRect);
    }
    for (const PlacedImage& image : fEngine->GetImages()) {
        RenderImage(view, image, updateRect);
    }
    RenderSelection(view, updateRect);
    RenderCaret(view);
}

void RenderContext::RenderCaret(BView* view)
{
    if (!view) return;
    CaretManager* caret = fEngine->GetCaretManager();
    if (!caret->IsVisible() || !caret->GetPosition().IsValid()) return;

    BRect rect = fEngine->GetHitTester()->CaretRect(caret->GetPosition());
    if (rect.left < 0) return;

    BRect viewRect = fEngine->DocumentToView(rect);
    view->SetHighColor(0, 0, 0, 255);
    view->SetPenSize(1.0f);
    view->StrokeLine(BPoint(viewRect.left, viewRect.top),
                     BPoint(viewRect.left, viewRect.bottom));
}

} // namespace YellowDocumentLayout
