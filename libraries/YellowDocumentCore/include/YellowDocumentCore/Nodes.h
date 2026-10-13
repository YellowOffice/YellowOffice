#ifndef YELLOW_DOCUMENT_CORE_NODES_H
#define YELLOW_DOCUMENT_CORE_NODES_H

#include <SupportDefs.h>
#include <String.h>
#include <Size.h>
#include <vector>

#include <YellowDocumentCore/DocumentNode.h>
#include <YellowDocumentCore/Style.h>

namespace YellowDocumentCore {

class YellowDocument;
class ParagraphNode;

/**
 * ListNode - a bulleted or numbered list. Children are ListItemNodes.
 */
class ListNode : public DocumentNode {
public:
    ListNode();
    explicit ListNode(bool ordered);
    ~ListNode();

    bool ordered;
    int32 level;

    DocumentNode* Clone() const override;
};

/**
 * ListItemNode - one item in a ListNode. Contains a single ParagraphNode
 * child holding the item's content.
 */
class ListItemNode : public DocumentNode {
public:
    ListItemNode();
    explicit ListItemNode(ParagraphNode* content);
    ~ListItemNode();

    /// Convenience access to the content paragraph (may be null).
    ParagraphNode* GetContent() const;

    DocumentNode* Clone() const override;
};

/**
 * TableNode - a simple table. Children are TableRowNodes.
 */
class TableNode : public DocumentNode {
public:
    TableNode();
    ~TableNode();

    int32 columnCount;

    size_t GetRowCount() const;
    DocumentNode* Clone() const override;
};

/**
 * TableRowNode - one row of a TableNode. Children are TableCellNodes.
 */
class TableRowNode : public DocumentNode {
public:
    TableRowNode();
    ~TableRowNode();

    float rowHeight;

    size_t GetCellCount() const;
    DocumentNode* Clone() const override;
};

/**
 * TableCellNode - one cell of a TableRowNode. Children are ParagraphNodes.
 */
class TableCellNode : public DocumentNode {
public:
    TableCellNode();
    ~TableCellNode();

    int32 columnSpan;
    int32 rowSpan;

    BString GetText() const override;
    DocumentNode* Clone() const override;
};

/**
 * ImageNode - an embedded raster image. Pixel data is kept in memory so
 * the document is self-contained.
 */
class ImageNode : public DocumentNode {
public:
    ImageNode();
    ~ImageNode();

    BString mimeType;      // e.g. "image/png"
    BString altText;
    std::vector<uint8_t> data;
    float displayWidth;    // points; 0 = use intrinsic size
    float displayHeight;   // points; 0 = use intrinsic size
    float intrinsicWidth;  // pixels (as declared by the importer)
    float intrinsicHeight;

    BString GetText() const override; // returns alt text
    bool IsEmpty() const;
    DocumentNode* Clone() const override;
};

/**
 * PageBreakNode - forces a page break before the following content.
 */
class PageBreakNode : public DocumentNode {
public:
    PageBreakNode();
    ~PageBreakNode();

    DocumentNode* Clone() const override;
};

/**
 * HeaderFooterNode - repeating header or footer content for a section.
 */
class HeaderFooterNode : public DocumentNode {
public:
    HeaderFooterNode();
    ~HeaderFooterNode();

    bool isHeader;   // false = footer
    BString text;    // may contain simple placeholders: &P (page), &N (pages), &T (title)
    ParagraphStyle style;

    BString GetText() const override;
    void SetText(const BString& newText) override;
    DocumentNode* Clone() const override;
};

} // namespace YellowDocumentCore

#endif // YELLOW_DOCUMENT_CORE_NODES_H
