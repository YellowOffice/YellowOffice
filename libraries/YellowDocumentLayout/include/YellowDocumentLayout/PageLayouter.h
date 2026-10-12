#ifndef YELLOW_DOCUMENT_LAYOUT_PAGE_LAYOUTER_H
#define YELLOW_DOCUMENT_LAYOUT_PAGE_LAYOUTER_H

#include <SupportDefs.h>
#include <Rect.h>

namespace YellowDocumentCore {
    class SectionNode;
}

namespace YellowDocumentLayout {

class PageLayouter {
public:
    PageLayouter();
    ~PageLayouter();

    // Layout page
    void LayoutPage(YellowDocumentCore::SectionNode* section, const BRect& bounds);
    
    // Calculate page dimensions
    BSize CalculatePageSize(YellowDocumentCore::SectionNode* section);
};

} // namespace YellowDocumentLayout

#endif // YELLOW_DOCUMENT_LAYOUT_PAGE_LAYOUTER_H
