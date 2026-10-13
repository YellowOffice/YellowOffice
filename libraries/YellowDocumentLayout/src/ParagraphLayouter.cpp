#include <YellowDocumentLayout/ParagraphLayouter.h>
#include <YellowDocumentLayout/TextLayouter.h>
#include <YellowDocumentLayout/LayoutEngine.h>

#include <stdio.h>

namespace YellowDocumentLayout {

using YellowDocumentCore::ListItemNode;
using YellowDocumentCore::ListNode;

ParagraphLayouter::ParagraphLayouter(TextLayouter* textLayouter)
    : fTextLayouter(textLayouter)
{
}

ParagraphLayouter::~ParagraphLayouter()
{
}

namespace {

// Walks up from the paragraph looking for a list item / list context.
struct ListContext {
    bool inList;
    bool ordered;
    int32 level;
    int32 index; // 1-based index within the list
};

ListContext FindListContext(ParagraphNode* paragraph)
{
    ListContext context = { false, false, 0, 0 };
    DocumentNode* node = paragraph;
    DocumentNode* parent = node ? node->GetParent() : nullptr;
    while (parent) {
        if (parent->GetType() == YellowDocumentCore::NODE_TYPE_LIST_ITEM) {
            ListItemNode* item = static_cast<ListItemNode*>(parent);
            DocumentNode* list = item->GetParent();
            if (list && list->GetType() == YellowDocumentCore::NODE_TYPE_LIST) {
                ListNode* listNode = static_cast<ListNode*>(list);
                context.inList = true;
                context.ordered = listNode->ordered;
                context.level = listNode->level;
                context.index = (int32)list->GetIndexOf(item) + 1;
                return context;
            }
        }
        node = parent;
        parent = parent->GetParent();
    }
    return context;
}

} // namespace

BString ParagraphLayouter::ListMarkerFor(ParagraphNode* paragraph)
{
    ListContext context = FindListContext(paragraph);
    if (!context.inList) return BString();
    if (context.ordered) {
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%d. ", context.index);
        return BString(buffer);
    }
    static const char* bullets[] = { "* ", "+ ", "- ", ". " };
    return BString(bullets[context.level % 4]);
}

float ParagraphLayouter::ListIndentFor(ParagraphNode* paragraph)
{
    ListContext context = FindListContext(paragraph);
    if (!context.inList) return 0.0f;
    return 18.0f + context.level * 18.0f;
}

void ParagraphLayouter::ComputeLines(ParagraphLayout& layout, float contentWidth)
{
    fTextLayouter->LayoutLines(layout.paragraph, contentWidth, &layout);
}

void ParagraphLayouter::ApplyAlignment(ParagraphLayout& layout, float contentWidth,
                                       ParagraphStyle::Alignment alignment)
{
    if (alignment == ParagraphStyle::ALIGN_LEFT) {
        return;
    }
    for (LineFragment& line : layout.lines) {
        float extra = contentWidth - line.width;
        if (extra <= 0) continue;
        switch (alignment) {
            case ParagraphStyle::ALIGN_CENTER:
                line.x += extra / 2.0f;
                break;
            case ParagraphStyle::ALIGN_RIGHT:
                line.x += extra;
                break;
            case ParagraphStyle::ALIGN_JUSTIFY:
                if (!line.isLastLineOfParagraph && line.runs.size() > 1) {
                    // Distribute extra space among word gaps (spaces).
                    int32 spaceGaps = 0;
                    for (const LineRun& run : line.runs) {
                        for (int32 i = 0; i < run.length; ++i) {
                            if (run.text[i] == ' ') ++spaceGaps;
                        }
                    }
                    if (spaceGaps > 0) {
                        float perSpace = extra / (float)spaceGaps;
                        // Expand each space character's share by widening the
                        // run width; rendering uses per-run widths so we also
                        // grow the space fragments.
                        float remaining = extra;
                        for (LineRun& run : line.runs) {
                            for (int32 i = 0; i < run.length; ++i) {
                                if (run.text[i] == ' ') {
                                    run.width += perSpace;
                                    remaining -= perSpace;
                                }
                            }
                        }
                        line.width += extra - remaining;
                    }
                }
                break;
            case ParagraphStyle::ALIGN_LEFT:
            default:
                break;
        }
    }
}

ParagraphLayout ParagraphLayouter::LayoutParagraph(ParagraphNode* paragraph, BPoint origin,
                                                   float contentWidth, int32 pageIndex)
{
    ParagraphLayout layout;
    layout.paragraph = paragraph;
    layout.pageIndex = pageIndex;
    layout.origin = origin;
    layout.width = contentWidth;
    layout.height = 0;

    if (!paragraph) return layout;

    const ParagraphStyle& style = paragraph->style;
    float listIndent = ListIndentFor(paragraph);
    float leftIndent = style.leftIndent + listIndent + style.leftMargin;
    float rightIndent = style.rightIndent + style.rightMargin;
    float usable = contentWidth - leftIndent - rightIndent;
    if (usable < 20.0f) usable = 20.0f;

    layout.contentWidth = usable;
    // Lines are computed at x=0; alignment shifts them inside [0, usable].
    ComputeLines(layout, usable);
    ApplyAlignment(layout, usable, style.alignment);

    // Store line positions relative to content top (after spaceBefore).
    float y = 0;
    for (LineFragment& line : layout.lines) {
        line.y = y;
        y += line.height;
    }
    float contentHeight = y;

    layout.height = style.spaceBefore + contentHeight + style.spaceAfter;
    return layout;
}

float ParagraphLayouter::MeasureHeight(ParagraphNode* paragraph, float contentWidth)
{
    if (!paragraph) return 0;
    const ParagraphStyle& style = paragraph->style;
    float listIndent = ListIndentFor(paragraph);
    float leftIndent = style.leftIndent + listIndent + style.leftMargin;
    float rightIndent = style.rightIndent + style.rightMargin;
    float usable = contentWidth - leftIndent - rightIndent;
    if (usable < 20.0f) usable = 20.0f;

    ParagraphLayout scratch;
    scratch.paragraph = paragraph;
    fTextLayouter->LayoutLines(paragraph, usable, &scratch);

    float contentHeight = 0;
    for (const LineFragment& line : scratch.lines) {
        contentHeight += line.height;
    }
    return style.spaceBefore + contentHeight + style.spaceAfter;
}

} // namespace YellowDocumentLayout
