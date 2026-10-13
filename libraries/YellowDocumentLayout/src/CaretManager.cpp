#include <YellowDocumentLayout/CaretManager.h>
#include <YellowDocumentLayout/LayoutEngine.h>
#include <YellowDocumentLayout/HitTester.h>

namespace YellowDocumentLayout {

CaretManager::CaretManager(LayoutEngine* engine)
    : fEngine(engine),
      fVisible(true)
{
}

CaretManager::~CaretManager()
{
}

const DocumentPosition& CaretManager::GetPosition() const
{
    return fPosition;
}

void CaretManager::SetPosition(const DocumentPosition& position)
{
    fPosition = position;
}

bool CaretManager::IsVisible() const
{
    return fVisible;
}

void CaretManager::SetVisible(bool visible)
{
    fVisible = visible;
}

void CaretManager::ToggleVisible()
{
    fVisible = !fVisible;
}

BPoint CaretManager::GetViewPoint() const
{
    if (!fPosition.IsValid()) {
        return BPoint(-1, -1);
    }
    BRect rect = fEngine->GetHitTester()->CaretRect(fPosition);
    if (rect.Width() < 0 || rect.left < 0) {
        return BPoint(-1, -1);
    }
    return fEngine->DocumentToView(BPoint(rect.left, rect.top));
}

void CaretManager::MoveLeft()
{
    DocumentPosition previous = fPosition.PreviousPosition();
    if (previous.IsValid()) {
        fPosition = previous;
    }
}

void CaretManager::MoveRight()
{
    DocumentPosition next = fPosition.NextPosition();
    if (next.IsValid()) {
        fPosition = next;
    }
}

void CaretManager::MoveToLineStart()
{
    if (!fPosition.IsValid()) return;
    // Find the paragraph containing the caret.
    DocumentNode* node = fPosition.GetNode();
    ParagraphNode* paragraph = nullptr;
    while (node) {
        if (node->GetType() == YellowDocumentCore::NODE_TYPE_PARAGRAPH) {
            paragraph = static_cast<ParagraphNode*>(node);
            break;
        }
        node = node->GetParent();
    }
    if (!paragraph) return;

    const ParagraphLayout* layout = fEngine->GetParagraphLayout(paragraph);
    if (!layout || layout->lines.empty()) return;

    int32 paragraphOffset = ParagraphOffsetOf(paragraph, fPosition);
    if (paragraphOffset < 0) paragraphOffset = 0;

    const LineFragment* line = nullptr;
    for (const LineFragment& candidate : layout->lines) {
        if (paragraphOffset >= candidate.start && paragraphOffset <= candidate.end) {
            line = &candidate;
            break;
        }
    }
    if (!line) line = &layout->lines.front();

    fPosition = paragraph->GetPositionAt(line->start);
}

void CaretManager::MoveToLineEnd()
{
    if (!fPosition.IsValid()) return;
    DocumentNode* node = fPosition.GetNode();
    ParagraphNode* paragraph = nullptr;
    while (node) {
        if (node->GetType() == YellowDocumentCore::NODE_TYPE_PARAGRAPH) {
            paragraph = static_cast<ParagraphNode*>(node);
            break;
        }
        node = node->GetParent();
    }
    if (!paragraph) return;

    const ParagraphLayout* layout = fEngine->GetParagraphLayout(paragraph);
    if (!layout || layout->lines.empty()) return;

    int32 paragraphOffset = ParagraphOffsetOf(paragraph, fPosition);
    if (paragraphOffset < 0) paragraphOffset = 0;

    const LineFragment* line = nullptr;
    for (const LineFragment& candidate : layout->lines) {
        if (paragraphOffset >= candidate.start && paragraphOffset <= candidate.end) {
            line = &candidate;
            break;
        }
    }
    if (!line) line = &layout->lines.back();

    fPosition = paragraph->GetPositionAt(line->end);
}

} // namespace YellowDocumentLayout
