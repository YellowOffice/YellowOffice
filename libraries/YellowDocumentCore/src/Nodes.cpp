#include "YellowDocumentCore/Nodes.h"
#include "YellowDocumentCore/YellowDocument.h"

namespace YellowDocumentCore {

// ------------------------------------------------------------------ ListNode

ListNode::ListNode()
    : DocumentNode(NODE_TYPE_LIST),
      ordered(false),
      level(0)
{
}

ListNode::ListNode(bool orderedIn)
    : DocumentNode(NODE_TYPE_LIST),
      ordered(orderedIn),
      level(0)
{
}

ListNode::~ListNode()
{
}

DocumentNode* ListNode::Clone() const
{
    ListNode* clone = new ListNode(ordered);
    clone->level = level;
    for (size_t i = 0; i < GetChildCount(); ++i) {
        DocumentNode* child = GetChild(i);
        if (child) {
            clone->AddChild(child->Clone());
        }
    }
    return clone;
}

// -------------------------------------------------------------- ListItemNode

ListItemNode::ListItemNode()
    : DocumentNode(NODE_TYPE_LIST_ITEM)
{
}

ListItemNode::ListItemNode(ParagraphNode* content)
    : DocumentNode(NODE_TYPE_LIST_ITEM)
{
    if (content) {
        AddChild(content);
    }
}

ListItemNode::~ListItemNode()
{
}

ParagraphNode* ListItemNode::GetContent() const
{
    for (size_t i = 0; i < GetChildCount(); ++i) {
        DocumentNode* child = GetChild(i);
        if (child && child->GetType() == NODE_TYPE_PARAGRAPH) {
            return static_cast<ParagraphNode*>(child);
        }
    }
    return nullptr;
}

DocumentNode* ListItemNode::Clone() const
{
    ListItemNode* clone = new ListItemNode();
    for (size_t i = 0; i < GetChildCount(); ++i) {
        DocumentNode* child = GetChild(i);
        if (child) {
            clone->AddChild(child->Clone());
        }
    }
    return clone;
}

// ----------------------------------------------------------------- TableNode

TableNode::TableNode()
    : DocumentNode(NODE_TYPE_TABLE),
      columnCount(0)
{
}

TableNode::~TableNode()
{
}

size_t TableNode::GetRowCount() const
{
    return GetChildCount();
}

DocumentNode* TableNode::Clone() const
{
    TableNode* clone = new TableNode();
    clone->columnCount = columnCount;
    for (size_t i = 0; i < GetChildCount(); ++i) {
        DocumentNode* child = GetChild(i);
        if (child) {
            clone->AddChild(child->Clone());
        }
    }
    return clone;
}

// -------------------------------------------------------------- TableRowNode

TableRowNode::TableRowNode()
    : DocumentNode(NODE_TYPE_TABLE_ROW),
      rowHeight(0)
{
}

TableRowNode::~TableRowNode()
{
}

size_t TableRowNode::GetCellCount() const
{
    return GetChildCount();
}

DocumentNode* TableRowNode::Clone() const
{
    TableRowNode* clone = new TableRowNode();
    clone->rowHeight = rowHeight;
    for (size_t i = 0; i < GetChildCount(); ++i) {
        DocumentNode* child = GetChild(i);
        if (child) {
            clone->AddChild(child->Clone());
        }
    }
    return clone;
}

// -------------------------------------------------------------- TableCellNode

TableCellNode::TableCellNode()
    : DocumentNode(NODE_TYPE_TABLE_CELL),
      columnSpan(1),
      rowSpan(1)
{
}

TableCellNode::~TableCellNode()
{
}

BString TableCellNode::GetText() const
{
    BString result;
    for (size_t i = 0; i < GetChildCount(); ++i) {
        DocumentNode* child = GetChild(i);
        if (child) {
            if (i > 0) {
                result += "\n";
            }
            result += child->GetText();
        }
    }
    return result;
}

DocumentNode* TableCellNode::Clone() const
{
    TableCellNode* clone = new TableCellNode();
    clone->columnSpan = columnSpan;
    clone->rowSpan = rowSpan;
    for (size_t i = 0; i < GetChildCount(); ++i) {
        DocumentNode* child = GetChild(i);
        if (child) {
            clone->AddChild(child->Clone());
        }
    }
    return clone;
}

// ----------------------------------------------------------------- ImageNode

ImageNode::ImageNode()
    : DocumentNode(NODE_TYPE_IMAGE),
      displayWidth(0),
      displayHeight(0),
      intrinsicWidth(0),
      intrinsicHeight(0)
{
}

ImageNode::~ImageNode()
{
}

BString ImageNode::GetText() const
{
    return altText;
}

bool ImageNode::IsEmpty() const
{
    return data.empty();
}

DocumentNode* ImageNode::Clone() const
{
    ImageNode* clone = new ImageNode();
    clone->mimeType = mimeType;
    clone->altText = altText;
    clone->data = data;
    clone->displayWidth = displayWidth;
    clone->displayHeight = displayHeight;
    clone->intrinsicWidth = intrinsicWidth;
    clone->intrinsicHeight = intrinsicHeight;
    return clone;
}

// ------------------------------------------------------------ PageBreakNode

PageBreakNode::PageBreakNode()
    : DocumentNode(NODE_TYPE_PAGE_BREAK)
{
}

PageBreakNode::~PageBreakNode()
{
}

DocumentNode* PageBreakNode::Clone() const
{
    return new PageBreakNode();
}

// ---------------------------------------------------------- HeaderFooterNode

HeaderFooterNode::HeaderFooterNode()
    : DocumentNode(NODE_TYPE_HEADER_FOOTER),
      isHeader(true)
{
}

HeaderFooterNode::~HeaderFooterNode()
{
}

BString HeaderFooterNode::GetText() const
{
    return text;
}

void HeaderFooterNode::SetText(const BString& newText)
{
    text = newText;
}

DocumentNode* HeaderFooterNode::Clone() const
{
    HeaderFooterNode* clone = new HeaderFooterNode();
    clone->isHeader = isHeader;
    clone->text = text;
    clone->style = style;
    return clone;
}

} // namespace YellowDocumentCore
