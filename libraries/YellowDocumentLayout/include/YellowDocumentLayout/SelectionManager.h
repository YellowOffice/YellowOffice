#ifndef YELLOW_DOCUMENT_LAYOUT_SELECTION_MANAGER_H
#define YELLOW_DOCUMENT_LAYOUT_SELECTION_MANAGER_H

#include <SupportDefs.h>
#include <Rect.h>

namespace YellowDocumentCore {
    class DocumentRange;
    class DocumentPosition;
}

namespace YellowDocumentLayout {

class SelectionManager {
public:
    SelectionManager();
    ~SelectionManager();

    // Selection operations
    void SetSelection(const YellowDocumentCore::DocumentRange& range);
    void ClearSelection();
    bool HasSelection() const;
    
    // Get selection
    YellowDocumentCore::DocumentRange GetSelection() const;
    
    // Draw selection
    void DrawSelection(BView* view, const BRect& bounds);
};

} // namespace YellowDocumentLayout

#endif // YELLOW_DOCUMENT_LAYOUT_SELECTION_MANAGER_H
