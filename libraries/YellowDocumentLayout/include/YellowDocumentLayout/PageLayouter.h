#ifndef YELLOW_DOCUMENT_LAYOUT_PAGE_LAYOUTER_H
#define YELLOW_DOCUMENT_LAYOUT_PAGE_LAYOUTER_H

#include <YellowDocumentLayout/LayoutTypes.h>

namespace YellowDocumentLayout {

class LayoutEngine;
class ParagraphLayouter;

/**
 * PageLayouter walks the document tree, flows blocks onto pages, handles
 * explicit page breaks, tables and images, and fills the layout cache
 * owned by the LayoutEngine.
 */
class PageLayouter {
public:
    PageLayouter(LayoutEngine* engine, ParagraphLayouter* paragraphLayouter);
    ~PageLayouter();

    /// Rebuilds the entire layout cache. Returns the page count.
    int32 LayoutDocument(YellowDocument* document);

private:
    struct FlowState {
        int32 pageIndex;
        float y; // current y within the page content area (document coords)
    };

    void StartPage(FlowState& state);
    bool NeedsPageBreak(const FlowState& state, float blockHeight) const;
    void LayoutParagraphNode(ParagraphNode* paragraph, FlowState& state);
    void LayoutTable(YellowDocumentCore::TableNode* table, FlowState& state);
    void LayoutImage(YellowDocumentCore::ImageNode* image, FlowState& state);
    void LayoutChildren(DocumentNode* node, FlowState& state);
    void FinishPages();

    LayoutEngine* fEngine;
    ParagraphLayouter* fParagraphLayouter;
};

} // namespace YellowDocumentLayout

#endif
