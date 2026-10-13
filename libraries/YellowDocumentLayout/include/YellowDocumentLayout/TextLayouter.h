#ifndef YELLOW_DOCUMENT_LAYOUT_TEXT_LAYOUTER_H
#define YELLOW_DOCUMENT_LAYOUT_TEXT_LAYOUTER_H

#include <YellowDocumentLayout/LayoutTypes.h>

namespace YellowDocumentLayout {

class LayoutEngine;

/**
 * TextLayouter breaks a paragraph's styled runs into visual lines using
 * greedy word wrapping against a content width.
 */
class TextLayouter {
public:
    explicit TextLayouter(LayoutEngine* engine);
    ~TextLayouter();

    /// Fills out->lines with wrapped lines for the paragraph. The paragraph
    /// style supplies line spacing; alignment is applied later by
    /// ParagraphLayouter. Line y coordinates are relative to the paragraph's
    /// content top (excluding spaceBefore).
    void LayoutLines(ParagraphNode* paragraph, float contentWidth, ParagraphLayout* out);

    /// Collects the paragraph's text runs into a flat list with cumulative
    /// paragraph offsets and measured widths.
    void CollectStyledRuns(ParagraphNode* paragraph, std::vector<LineRun>& out) const;

private:
    LayoutEngine* fEngine;
};

} // namespace YellowDocumentLayout

#endif
