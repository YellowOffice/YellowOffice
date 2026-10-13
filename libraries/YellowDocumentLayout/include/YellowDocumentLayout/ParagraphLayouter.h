#ifndef YELLOW_DOCUMENT_LAYOUT_PARAGRAPH_LAYOUTER_H
#define YELLOW_DOCUMENT_LAYOUT_PARAGRAPH_LAYOUTER_H

#include <YellowDocumentLayout/LayoutTypes.h>

namespace YellowDocumentLayout {

class LayoutEngine;
class TextLayouter;

/**
 * ParagraphLayouter produces a complete ParagraphLayout for one paragraph:
 * line breaking (via TextLayouter), alignment, indentation, spacing and
 * list markers.
 */
class ParagraphLayouter {
public:
    explicit ParagraphLayouter(TextLayouter* textLayouter);
    ~ParagraphLayouter();

    /// Lays out the paragraph at the given origin (content top-left of the
    /// block, before spaceBefore). contentWidth is the usable width.
    ParagraphLayout LayoutParagraph(ParagraphNode* paragraph, BPoint origin,
                                    float contentWidth, int32 pageIndex);

    /// Height the paragraph will occupy (including spaceBefore/After).
    float MeasureHeight(ParagraphNode* paragraph, float contentWidth);

    /// Text of the list marker for this paragraph ("" if not in a list).
    static BString ListMarkerFor(ParagraphNode* paragraph);

    /// Indent (points) added on the left for list nesting.
    static float ListIndentFor(ParagraphNode* paragraph);

private:
    void ApplyAlignment(ParagraphLayout& layout, float contentWidth,
                        ParagraphStyle::Alignment alignment);
    void ComputeLines(ParagraphLayout& layout, float contentWidth);

    TextLayouter* fTextLayouter;
};

} // namespace YellowDocumentLayout

#endif
