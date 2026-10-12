#ifndef YELLOW_DOCUMENT_LAYOUT_TEXT_LAYOUTER_H
#define YELLOW_DOCUMENT_LAYOUT_TEXT_LAYOUTER_H

#include <SupportDefs.h>
#include <Rect.h>

namespace YellowDocumentCore {
    class TextRunNode;
}

namespace YellowDocumentLayout {

class TextLayouter {
public:
    TextLayouter();
    ~TextLayouter();

    // Layout text run
    void LayoutTextRun(YellowDocumentCore::TextRunNode* textRun, const BRect& bounds);
    
    // Measure text
    BSize MeasureText(const char* text, int32 length);
    float MeasureStringWidth(const char* text, int32 length);
    
    // Line breaking
    int32 FindLineBreak(const char* text, int32 length, float maxWidth);
};

} // namespace YellowDocumentLayout

#endif // YELLOW_DOCUMENT_LAYOUT_TEXT_LAYOUTER_H
