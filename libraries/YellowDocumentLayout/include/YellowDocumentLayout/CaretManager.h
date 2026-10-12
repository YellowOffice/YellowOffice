#ifndef YELLOW_DOCUMENT_LAYOUT_CARET_MANAGER_H
#define YELLOW_DOCUMENT_LAYOUT_CARET_MANAGER_H

#include <SupportDefs.h>
#include <Point.h>

namespace YellowDocumentCore {
    class DocumentPosition;
}

namespace YellowDocumentLayout {

class CaretManager {
public:
    CaretManager();
    ~CaretManager();

    // Caret operations
    void SetCaretPosition(const YellowDocumentCore::DocumentPosition& position);
    YellowDocumentCore::DocumentPosition GetCaretPosition() const;
    
    // Caret visibility
    void ShowCaret();
    void HideCaret();
    bool IsCaretVisible() const;
    
    // Caret drawing
    void DrawCaret(BView* view, const BPoint& position);
    
    // Blink timer
    void StartBlinking();
    void StopBlinking();

private:
    bool fVisible;
    bool fBlinking;
};

} // namespace YellowDocumentLayout

#endif // YELLOW_DOCUMENT_LAYOUT_CARET_MANAGER_H
