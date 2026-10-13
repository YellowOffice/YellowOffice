#include <YellowDocumentLayout/SelectionManager.h>
#include <YellowDocumentLayout/LayoutEngine.h>

namespace YellowDocumentLayout {

SelectionManager::SelectionManager()
    : fEmpty(true)
{
}

SelectionManager::~SelectionManager()
{
}

bool SelectionManager::IsEmpty() const
{
    return fEmpty || fRange.IsCollapsed() || !fRange.IsValid();
}

const DocumentRange& SelectionManager::GetRange() const
{
    return fRange;
}

void SelectionManager::SetRange(const DocumentRange& range)
{
    fRange = range;
    fRange.Normalize();
    fEmpty = fRange.IsCollapsed() || !fRange.IsValid();
}

void SelectionManager::SetRange(const DocumentPosition& anchor, const DocumentPosition& other)
{
    DocumentRange range(anchor, other);
    range.Normalize();
    SetRange(range);
}

void SelectionManager::Collapse(const DocumentPosition& position)
{
    fRange = DocumentRange(position, position);
    fEmpty = true;
}

void SelectionManager::CollapseToStart()
{
    Collapse(fRange.GetStart());
}

void SelectionManager::CollapseToEnd()
{
    Collapse(fRange.GetEnd());
}

void SelectionManager::Clear()
{
    fRange = DocumentRange();
    fEmpty = true;
}

void SelectionManager::SelectAll(YellowDocument* document)
{
    if (!document || !document->GetRoot()) {
        Clear();
        return;
    }
    DocumentPosition start = document->GetRoot()->GetStartPosition();
    DocumentPosition end = document->GetRoot()->GetEndPosition();

    // Prefer the first/last leaf positions.
    DocumentNode* node = document->GetRoot();
    while (node && node->GetChildCount() > 0) {
        node = node->GetChild(0);
    }
    if (node) start = node->GetStartPosition();

    node = document->GetRoot();
    while (node && node->GetChildCount() > 0) {
        node = node->GetChild(node->GetChildCount() - 1);
    }
    if (node) end = node->GetEndPosition();

    SetRange(DocumentRange(start, end));
}

bool SelectionManager::Contains(const DocumentPosition& position) const
{
    if (IsEmpty()) return false;
    return fRange.Contains(position);
}

void SelectionManager::ExtendTo(const DocumentPosition& position)
{
    if (!fRange.IsValid()) {
        SetRange(position, position);
        return;
    }
    DocumentRange range(fRange.GetStart(), position);
    range.Normalize();
    SetRange(range);
}

namespace {

void CollectParagraphs(DocumentNode* node, std::vector<ParagraphNode*>& out)
{
    if (!node) return;
    if (node->GetType() == YellowDocumentCore::NODE_TYPE_PARAGRAPH) {
        out.push_back(static_cast<ParagraphNode*>(node));
        return;
    }
    for (size_t i = 0; i < node->GetChildCount(); ++i) {
        CollectParagraphs(node->GetChild(i), out);
    }
}

} // namespace

BString SelectionManager::GetSelectedText(YellowDocument* document) const
{
    BString result;
    if (!document || IsEmpty()) return result;

    std::vector<ParagraphNode*> paragraphs;
    CollectParagraphs(document->GetRoot(), paragraphs);

    bool first = true;
    for (ParagraphNode* paragraph : paragraphs) {
        int32 length = paragraph->GetTextLength();
        DocumentPosition paragraphStart = paragraph->GetStartPosition();
        DocumentPosition paragraphEnd = paragraph->GetEndPosition();

        if (paragraphEnd <= fRange.GetStart() || paragraphStart >= fRange.GetEnd()) {
            continue;
        }

        int32 startOffset = 0;
        if (paragraphStart < fRange.GetStart()) {
            DocumentPosition clipped = fRange.GetStart();
            if (clipped.GetNode() != paragraph &&
                clipped.GetNode() && paragraph->Contains(clipped.GetNode())) {
                startOffset = ParagraphOffsetOf(paragraph, clipped);
            } else if (clipped.GetNode() == paragraph) {
                startOffset = clipped.GetOffset();
            }
        }
        int32 endOffset = length;
        if (paragraphEnd > fRange.GetEnd()) {
            DocumentPosition clipped = fRange.GetEnd();
            if (clipped.GetNode() != paragraph &&
                clipped.GetNode() && paragraph->Contains(clipped.GetNode())) {
                endOffset = ParagraphOffsetOf(paragraph, clipped);
            } else if (clipped.GetNode() == paragraph) {
                endOffset = clipped.GetOffset();
            }
        }
        if (endOffset < startOffset) endOffset = startOffset;

        if (!first) {
            result += "\n";
        }
        first = false;
        result += paragraph->GetText(startOffset, endOffset - startOffset);
    }
    return result;
}

} // namespace YellowDocumentLayout
