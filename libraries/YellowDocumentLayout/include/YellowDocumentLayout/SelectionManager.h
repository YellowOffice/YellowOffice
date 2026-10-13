#ifndef YELLOW_DOCUMENT_LAYOUT_SELECTION_MANAGER_H
#define YELLOW_DOCUMENT_LAYOUT_SELECTION_MANAGER_H

#include <YellowDocumentLayout/LayoutTypes.h>

namespace YellowDocumentLayout {

class LayoutEngine;

/**
 * SelectionManager tracks the current selection range in the document.
 */
class SelectionManager {
public:
    SelectionManager();
    ~SelectionManager();

    bool IsEmpty() const;
    const DocumentRange& GetRange() const;
    void SetRange(const DocumentRange& range);
    void SetRange(const DocumentPosition& anchor, const DocumentPosition& other);
    void Collapse(const DocumentPosition& position);
    void CollapseToStart();
    void CollapseToEnd();
    void Clear();
    void SelectAll(YellowDocument* document);

    /// True if the given paragraph-level position is inside the selection.
    bool Contains(const DocumentPosition& position) const;

    /// Plain text of the selection (paragraphs joined with '\n').
    BString GetSelectedText(YellowDocument* document) const;

    /// Extends the selection end to the given position.
    void ExtendTo(const DocumentPosition& position);

private:
    DocumentRange fRange;
    bool fEmpty;
};

} // namespace YellowDocumentLayout

#endif
