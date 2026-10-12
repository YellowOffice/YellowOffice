#ifndef YELLOW_DOCUMENT_LAYOUT_PARAGRAPH_LAYOUTER_H
#define YELLOW_DOCUMENT_LAYOUT_PARAGRAPH_LAYOUTER_H

#include <SupportDefs.h>
#include <Rect.h>

namespace YellowDocumentCore {
    class ParagraphNode;
}

namespace YellowDocumentLayout {

class ParagraphLayouter {
public:
    ParagraphLayouter();
    ~ParagraphLayouter();

    // Layout paragraph
    void LayoutParagraph(YellowDocumentCore::ParagraphNode* paragraph, const BRect& bounds);
    
    // Calculate paragraph dimensions
    BSize CalculateParagraphSize(YellowDocumentCore::ParagraphNode* paragraph);
};

} // namespace YellowDocumentLayout

#endif // YELLOW_DOCUMENT_LAYOUT_PARAGRAPH_LAYOUTER_H
