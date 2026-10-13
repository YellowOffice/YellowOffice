#ifndef YELLOW_DOCUMENT_LAYOUT_CARET_MANAGER_H
#define YELLOW_DOCUMENT_LAYOUT_CARET_MANAGER_H

#include <YellowDocumentLayout/LayoutTypes.h>

namespace YellowDocumentLayout {

class LayoutEngine;

/**
 * CaretManager tracks the insertion caret position and visibility.
 */
class CaretManager {
public:
    explicit CaretManager(LayoutEngine* engine);
    ~CaretManager();

    const DocumentPosition& GetPosition() const;
    void SetPosition(const DocumentPosition& position);

    bool IsVisible() const;
    void SetVisible(bool visible);
    void ToggleVisible();

    /// Caret x within the view for the current position (invalid position
    /// yields BPoint(-1, -1)).
    BPoint GetViewPoint() const;

    /// Simple caret motion.
    void MoveLeft();
    void MoveRight();
    void MoveToLineStart();
    void MoveToLineEnd();

private:
    LayoutEngine* fEngine;
    DocumentPosition fPosition;
    bool fVisible;
};

} // namespace YellowDocumentLayout

#endif
