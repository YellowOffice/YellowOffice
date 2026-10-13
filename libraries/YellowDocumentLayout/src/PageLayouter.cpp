#include <YellowDocumentLayout/PageLayouter.h>
#include <YellowDocumentLayout/ParagraphLayouter.h>
#include <YellowDocumentLayout/LayoutEngine.h>

#include <algorithm>

namespace YellowDocumentLayout {

using YellowDocumentCore::ImageNode;
using YellowDocumentCore::ListItemNode;
using YellowDocumentCore::ListNode;
using YellowDocumentCore::PageBreakNode;
using YellowDocumentCore::TableCellNode;
using YellowDocumentCore::TableNode;
using YellowDocumentCore::TableRowNode;

static const float kPageGap = 24.0f;
static const float kTablePadding = 4.0f;

PageLayouter::PageLayouter(LayoutEngine* engine, ParagraphLayouter* paragraphLayouter)
    : fEngine(engine),
      fParagraphLayouter(paragraphLayouter)
{
}

PageLayouter::~PageLayouter()
{
}

void PageLayouter::StartPage(FlowState& state)
{
    SectionNode* section = fEngine->GetSection();
    float pageWidth = section ? section->pageWidth : 8.5f * 72.0f;
    float pageHeight = section ? section->pageHeight : 11.0f * 72.0f;
    float top = section ? section->topMargin : 72.0f;
    float left = section ? section->leftMargin : 72.0f;
    float right = section ? section->rightMargin : 72.0f;
    float bottom = section ? section->bottomMargin : 72.0f;

    int32 index = (int32)fEngine->GetPages().size();
    float topOffset = index * (pageHeight + kPageGap);

    PageInfo page;
    page.index = index;
    page.bounds = BRect(0, topOffset, pageWidth, topOffset + pageHeight);
    page.contentBounds = BRect(left, topOffset + top,
                               pageWidth - right, topOffset + pageHeight - bottom);
    fEngine->GetPages().push_back(page);

    state.pageIndex = index;
    state.y = page.contentBounds.top;
}

bool PageLayouter::NeedsPageBreak(const FlowState& state, float blockHeight) const
{
    const std::vector<PageInfo>& pages = fEngine->GetPages();
    if (pages.empty()) return true;
    const PageInfo& page = pages[state.pageIndex];
    return state.y + blockHeight > page.contentBounds.bottom;
}

void PageLayouter::LayoutParagraphNode(ParagraphNode* paragraph, FlowState& state)
{
    if (!paragraph) return;
    SectionNode* section = fEngine->GetSection();
    float contentWidth = section
        ? section->pageWidth - section->leftMargin - section->rightMargin
        : 8.5f * 72.0f - 144.0f;

    float spaceBefore = paragraph->style.spaceBefore;
    float y = state.y + spaceBefore;

    ParagraphLayout layout = fParagraphLayouter->LayoutParagraph(
        paragraph, BPoint(0, y), contentWidth, state.pageIndex);

    bool pageHasContent = state.y > fEngine->GetPages()[state.pageIndex].contentBounds.top;
    if (layout.height > 0 &&
        NeedsPageBreak(state, spaceBefore + layout.height) && pageHasContent) {
        StartPage(state);
        y = state.y + spaceBefore;
        layout = fParagraphLayouter->LayoutParagraph(
            paragraph, BPoint(0, y), contentWidth, state.pageIndex);
    }

    // Store with correct origin (block top including spaceBefore).
    layout.origin = BPoint(0, y - spaceBefore);
    fEngine->GetParagraphs().push_back(layout);
    state.y = y - spaceBefore + layout.height;
}

void PageLayouter::LayoutTable(TableNode* table, FlowState& state)
{
    if (!table) return;
    SectionNode* section = fEngine->GetSection();
    float contentWidth = section
        ? section->pageWidth - section->leftMargin - section->rightMargin
        : 8.5f * 72.0f - 144.0f;

    int32 columns = table->columnCount;
    if (columns <= 0) {
        for (size_t i = 0; i < table->GetChildCount(); ++i) {
            TableRowNode* row = dynamic_cast<TableRowNode*>(table->GetChild(i));
            if (row) {
                columns = std::max<int32>(columns, (int32)row->GetCellCount());
            }
        }
    }
    if (columns <= 0) columns = 1;

    float columnWidth = contentWidth / (float)columns;

    PlacedTable placed;
    placed.table = table;
    placed.pageIndex = state.pageIndex;
    placed.columnWidths.assign(columns, columnWidth);
    placed.rowHeights.clear();

    float tableTop = state.y;
    float y = state.y;

    for (size_t rowIndex = 0; rowIndex < table->GetChildCount(); ++rowIndex) {
        TableRowNode* row = dynamic_cast<TableRowNode*>(table->GetChild(rowIndex));
        if (!row) continue;

        // Measure the row height by laying out each cell's paragraphs into a
        // scratch layout.
        float rowHeight = 0;
        std::vector<ParagraphLayout> cellParagraphs;

        for (size_t cellIndex = 0; cellIndex < row->GetCellCount(); ++cellIndex) {
            TableCellNode* cell = dynamic_cast<TableCellNode*>(row->GetChild(cellIndex));
            if (!cell) continue;
            float cellX = (float)cellIndex * columnWidth + kTablePadding;
            float cellWidth = columnWidth - 2 * kTablePadding;
            float cellY = y + kTablePadding;

            for (size_t p = 0; p < cell->GetChildCount(); ++p) {
                DocumentNode* child = cell->GetChild(p);
                if (!child || child->GetType() != YellowDocumentCore::NODE_TYPE_PARAGRAPH) {
                    continue;
                }
                ParagraphNode* paragraph = static_cast<ParagraphNode*>(child);
                ParagraphLayout layout = fParagraphLayouter->LayoutParagraph(
                    paragraph, BPoint(cellX, cellY), cellWidth, state.pageIndex);
                cellParagraphs.push_back(layout);
                cellY += layout.height;
            }
            rowHeight = std::max(rowHeight, cellY - y);
        }
        if (row->rowHeight > 0) {
            rowHeight = std::max(rowHeight, row->rowHeight);
        }
        if (rowHeight < 2 * kTablePadding + 12) {
            rowHeight = 2 * kTablePadding + 12;
        }

        bool pageHasContent = y > fEngine->GetPages()[state.pageIndex].contentBounds.top;
        if (y + rowHeight > fEngine->GetPages()[state.pageIndex].contentBounds.bottom
            && pageHasContent) {
            StartPage(state);
            y = state.y;
            // Re-anchor cell paragraph origins to the new page y.
            float delta = y - tableTop;
            for (ParagraphLayout& layout : cellParagraphs) {
                layout.origin.y += delta;
                layout.pageIndex = state.pageIndex;
                for (LineFragment& line : layout.lines) {
                    (void)line;
                }
            }
            placed.pageIndex = state.pageIndex;
            tableTop = y;
        }

        for (ParagraphLayout& layout : cellParagraphs) {
            fEngine->GetParagraphs().push_back(layout);
        }
        placed.rowHeights.push_back(rowHeight);
        y += rowHeight;
    }

    placed.bounds = BRect(0, tableTop, contentWidth, y);
    fEngine->GetTables().push_back(placed);
    state.y = y;
}

void PageLayouter::LayoutImage(ImageNode* image, FlowState& state)
{
    if (!image) return;
    SectionNode* section = fEngine->GetSection();
    float contentWidth = section
        ? section->pageWidth - section->leftMargin - section->rightMargin
        : 8.5f * 72.0f - 144.0f;

    float width = image->displayWidth > 0 ? image->displayWidth
                : (image->intrinsicWidth > 0 ? image->intrinsicWidth : 144.0f);
    float height = image->displayHeight > 0 ? image->displayHeight
                 : (image->intrinsicHeight > 0 ? image->intrinsicHeight : 96.0f);

    if (width > contentWidth) {
        height = height * (contentWidth / width);
        width = contentWidth;
    }

    bool pageHasContent = state.y > fEngine->GetPages()[state.pageIndex].contentBounds.top;
    if (state.y + height > fEngine->GetPages()[state.pageIndex].contentBounds.bottom
        && pageHasContent) {
        StartPage(state);
    }

    PlacedImage placed;
    placed.image = image;
    placed.pageIndex = state.pageIndex;
    placed.bounds = BRect(0, state.y, width, state.y + height);
    fEngine->GetImages().push_back(placed);
    state.y += height + 6.0f;
}

void PageLayouter::LayoutChildren(DocumentNode* node, FlowState& state)
{
    if (!node) return;
    for (size_t i = 0; i < node->GetChildCount(); ++i) {
        DocumentNode* child = node->GetChild(i);
        if (!child) continue;

        switch (child->GetType()) {
            case YellowDocumentCore::NODE_TYPE_SECTION:
                LayoutChildren(child, state);
                break;

            case YellowDocumentCore::NODE_TYPE_PARAGRAPH:
                LayoutParagraphNode(static_cast<ParagraphNode*>(child), state);
                break;

            case YellowDocumentCore::NODE_TYPE_PAGE_BREAK: {
                (void)static_cast<PageBreakNode*>(child);
                bool pageHasContent =
                    state.y > fEngine->GetPages()[state.pageIndex].contentBounds.top;
                if (pageHasContent) {
                    StartPage(state);
                }
                break;
            }

            case YellowDocumentCore::NODE_TYPE_LIST:
                LayoutChildren(child, state);
                break;

            case YellowDocumentCore::NODE_TYPE_LIST_ITEM:
                LayoutChildren(child, state);
                break;

            case YellowDocumentCore::NODE_TYPE_TABLE:
                LayoutTable(static_cast<TableNode*>(child), state);
                break;

            case YellowDocumentCore::NODE_TYPE_IMAGE:
                LayoutImage(static_cast<ImageNode*>(child), state);
                break;

            default:
                // Headers/footers/metadata/text runs are handled elsewhere.
                if (child->GetChildCount() > 0 &&
                    child->GetType() != YellowDocumentCore::NODE_TYPE_TEXT_RUN) {
                    LayoutChildren(child, state);
                }
                break;
        }
    }
}

void PageLayouter::FinishPages()
{
    SectionNode* section = fEngine->GetSection();
    float pageWidth = section ? section->pageWidth : 8.5f * 72.0f;
    float pageHeight = section ? section->pageHeight : 11.0f * 72.0f;

    int32 pageCount = (int32)fEngine->GetPages().size();
    if (pageCount < 1) pageCount = 1;
    float documentHeight = pageCount * (pageHeight + kPageGap) - kPageGap;
    fEngine->FinishLayout(pageCount, BSize(pageWidth, documentHeight));

    if (fEngine->GetDocument()) {
        fEngine->GetDocument()->SetPageCount(pageCount);
    }
}

int32 PageLayouter::LayoutDocument(YellowDocument* document)
{
    fEngine->GetParagraphs().clear();
    fEngine->GetImages().clear();
    fEngine->GetTables().clear();
    fEngine->GetPages().clear();

    if (!document) {
        FinishPages();
        return fEngine->GetPageCount();
    }

    FlowState state;
    StartPage(state);
    LayoutChildren(document->GetRoot(), state);

    FinishPages();
    return fEngine->GetPageCount();
}

} // namespace YellowDocumentLayout
