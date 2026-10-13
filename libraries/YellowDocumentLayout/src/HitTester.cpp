#include <YellowDocumentLayout/HitTester.h>
#include <YellowDocumentLayout/LayoutEngine.h>

#include <math.h>

namespace YellowDocumentLayout {

HitTester::HitTester(LayoutEngine* engine)
    : fEngine(engine)
{
}

HitTester::~HitTester()
{
}

ParagraphNode* HitTester::HitTestParagraph(const BPoint& documentPoint) const
{
    const std::vector<ParagraphLayout>& paragraphs = fEngine->GetParagraphs();
    for (const ParagraphLayout& layout : paragraphs) {
        float bottom = layout.origin.y + layout.height;
        // Use the widest line as the horizontal extent.
        float right = layout.origin.x + layout.width;
        if (documentPoint.y >= layout.origin.y && documentPoint.y <= bottom &&
            documentPoint.x >= layout.origin.x && documentPoint.x <= right) {
            return layout.paragraph;
        }
    }
    return nullptr;
}

int32 HitTester::OffsetInLine(const LineFragment& line, float x,
                              ParagraphNode* paragraph) const
{
    if (!paragraph) return line.start;
    FontCache* fonts = fEngine->GetFontCache();

    float currentX = line.x;
    for (const LineRun& run : line.runs) {
        for (int32 i = 0; i < run.length; ++i) {
            BString character(run.text.String() + i, 1);
            float charWidth = fonts->StringWidth(character, run.style);
            if (x < currentX + charWidth / 2.0f) {
                int32 offset = run.startInParagraph + run.offsetInRun + i;
                return offset;
            }
            currentX += charWidth;
        }
    }
    return line.end;
}

DocumentPosition HitTester::HitTest(const BPoint& documentPoint) const
{
    fEngine->EnsureLayout();
    const std::vector<ParagraphLayout>& paragraphs = fEngine->GetParagraphs();
    if (paragraphs.empty()) return DocumentPosition();

    // Find the closest paragraph vertically, then the closest line.
    const ParagraphLayout* best = nullptr;
    float bestDistance = 1e12f;
    for (const ParagraphLayout& layout : paragraphs) {
        float top = layout.origin.y;
        float bottom = layout.origin.y + layout.height;
        float distance = 0;
        if (documentPoint.y < top) {
            distance = top - documentPoint.y;
        } else if (documentPoint.y > bottom) {
            distance = documentPoint.y - bottom;
        }
        if (distance < bestDistance) {
            bestDistance = distance;
            best = &layout;
        }
    }
    if (!best) return DocumentPosition();

    // Choose the line whose vertical span contains (or is nearest to) the y.
    const LineFragment* bestLine = nullptr;
    float lineBest = 1e12f;
    for (const LineFragment& line : best->lines) {
        float top = best->origin.y + best->paragraph->style.spaceBefore + line.y;
        float bottom = top + line.height;
        float distance = 0;
        if (documentPoint.y < top) distance = top - documentPoint.y;
        else if (documentPoint.y > bottom) distance = documentPoint.y - bottom;
        if (distance < lineBest) {
            lineBest = distance;
            bestLine = &line;
        }
    }
    if (!bestLine) return best->paragraph->GetStartPosition();

    int32 offset = OffsetInLine(*bestLine, documentPoint.x - best->origin.x, best->paragraph);
    return best->paragraph->GetPositionAt(offset);
}

BRect HitTester::CaretRect(const DocumentPosition& position) const
{
    if (!position.IsValid()) return BRect(-1, -1, -1, -1);
    fEngine->EnsureLayout();

    DocumentNode* node = position.GetNode();
    ParagraphNode* paragraph = nullptr;
    while (node) {
        if (node->GetType() == YellowDocumentCore::NODE_TYPE_PARAGRAPH) {
            paragraph = static_cast<ParagraphNode*>(node);
            break;
        }
        node = node->GetParent();
    }
    if (!paragraph) return BRect(-1, -1, -1, -1);

    const ParagraphLayout* layout = fEngine->GetParagraphLayout(paragraph);
    if (!layout || layout->lines.empty()) return BRect(-1, -1, -1, -1);

    int32 offset = ParagraphOffsetOf(paragraph, position);
    if (offset < 0) offset = 0;

    const LineFragment* line = nullptr;
    for (const LineFragment& candidate : layout->lines) {
        if (offset >= candidate.start && offset <= candidate.end) {
            line = &candidate;
            break;
        }
    }
    if (!line) line = &layout->lines.front();

    FontCache* fonts = fEngine->GetFontCache();
    float x = layout->origin.x + line->x;
    // Advance through runs up to the offset.
    for (const LineRun& run : line->runs) {
        int32 runStart = run.startInParagraph + run.offsetInRun;
        int32 runEnd = runStart + run.length;
        if (offset <= runStart) break;
        int32 chars = offset - runStart;
        if (chars > run.length) chars = run.length;
        BString prefix(run.text.String(), chars);
        x += fonts->StringWidth(prefix, run.style);
        if (offset <= runEnd) break;
    }

    float top = layout->origin.y + paragraph->style.spaceBefore + line->y;
    return BRect(x, top, x + 1.0f, top + line->height);
}

void HitTester::SelectionRects(const DocumentRange& rangeIn, std::vector<BRect>& out) const
{
    out.clear();
    DocumentRange range = rangeIn;
    range.Normalize();
    if (!range.IsValid() || range.IsCollapsed()) return;

    fEngine->EnsureLayout();
    FontCache* fonts = fEngine->GetFontCache();
    const std::vector<ParagraphLayout>& paragraphs = fEngine->GetParagraphs();

    for (const ParagraphLayout& layout : paragraphs) {
        ParagraphNode* paragraph = layout.paragraph;
        int32 length = paragraph->GetTextLength();
        DocumentPosition pStart = paragraph->GetStartPosition();
        DocumentPosition pEnd = paragraph->GetEndPosition();

        if (pEnd <= range.GetStart() || pStart >= range.GetEnd()) {
            // Allow end-of-paragraph caret positions to match boundary.
            bool touches = (pEnd == range.GetStart() || pStart == range.GetEnd());
            if (!touches) continue;
            if (range.GetStart().GetNode() != paragraph &&
                range.GetEnd().GetNode() != paragraph &&
                !paragraph->Contains(range.GetStart().GetNode()) &&
                !paragraph->Contains(range.GetEnd().GetNode())) {
                continue;
            }
        }

        int32 startOffset = 0;
        int32 endOffset = length;
        if (pStart < range.GetStart()) {
            int32 candidate = ParagraphOffsetOf(paragraph, range.GetStart());
            if (candidate >= 0) startOffset = candidate;
        }
        if (pEnd > range.GetEnd()) {
            int32 candidate = ParagraphOffsetOf(paragraph, range.GetEnd());
            if (candidate >= 0) endOffset = candidate;
        }
        if (endOffset <= startOffset) continue;

        for (const LineFragment& line : layout.lines) {
            if (line.end <= startOffset || line.start >= endOffset) continue;

            int32 lineSelStart = startOffset > line.start ? startOffset : line.start;
            int32 lineSelEnd = endOffset < line.end ? endOffset : line.end;

            float x = layout.origin.x + line.x;
            float startX = x;
            float endX = x;
            bool startFound = lineSelStart <= line.start;
            bool endFound = false;

            for (const LineRun& run : line.runs) {
                int32 runStart = run.startInParagraph + run.offsetInRun;
                for (int32 i = 0; i < run.length; ++i) {
                    int32 offset = runStart + i;
                    BString character(run.text.String() + i, 1);
                    float charWidth = fonts->StringWidth(character, run.style);
                    if (!startFound && offset >= lineSelStart) {
                        startX = x;
                        startFound = true;
                    }
                    if (offset >= lineSelEnd) {
                        endX = x;
                        endFound = true;
                        break;
                    }
                    x += charWidth;
                }
                if (endFound) break;
            }
            if (!startFound) startX = x;
            if (!endFound) endX = x;

            if (endX <= startX) {
                // Empty trailing selection on the line: draw a small caret-
                // width marker so the user sees the selection.
                endX = startX + 2.0f;
            }
            float top = layout.origin.y + paragraph->style.spaceBefore + line.y;
            out.push_back(BRect(startX, top, endX, top + line.height));
        }
    }
}

} // namespace YellowDocumentLayout
