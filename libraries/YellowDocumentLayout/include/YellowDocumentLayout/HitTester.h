#ifndef YELLOW_DOCUMENT_LAYOUT_HIT_TESTER_H
#define YELLOW_DOCUMENT_LAYOUT_HIT_TESTER_H

#include <YellowDocumentLayout/LayoutTypes.h>

namespace YellowDocumentLayout {

class LayoutEngine;

/**
 * HitTester maps points in document coordinates to document positions.
 */
class HitTester {
public:
    explicit HitTester(LayoutEngine* engine);
    ~HitTester();

    /// Nearest caret position for a point in document coordinates.
    DocumentPosition HitTest(const BPoint& documentPoint) const;

    /// Paragraph whose bounds contain the point (or null).
    ParagraphNode* HitTestParagraph(const BPoint& documentPoint) const;

    /// Given a line fragment and a point's x, returns the paragraph offset
    /// of the nearest character boundary.
    int32 OffsetInLine(const LineFragment& line, float x,
                       ParagraphNode* paragraph) const;

    /// Document-coordinate rect of the caret (thin) at a position.
    BRect CaretRect(const DocumentPosition& position) const;

    /// Document-coordinate rects covering a range (one per intersecting line).
    void SelectionRects(const DocumentRange& range, std::vector<BRect>& out) const;

private:
    LayoutEngine* fEngine;
};

} // namespace YellowDocumentLayout

#endif
