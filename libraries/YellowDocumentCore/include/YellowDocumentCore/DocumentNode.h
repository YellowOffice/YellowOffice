#ifndef YELLOW_DOCUMENT_CORE_DOCUMENT_NODE_H
#define YELLOW_DOCUMENT_CORE_DOCUMENT_NODE_H

#include <SupportDefs.h>
#include <String.h>
#include <vector>

#include <YellowCommon/Defines.h>

namespace YellowDocumentCore {

// Forward declarations
class DocumentNode;
class YellowDocument;

/**
 * DocumentPosition represents a position in the document
 */
class DocumentPosition {
public:
    DocumentPosition(DocumentNode* node = nullptr, int32 offset = 0);
    DocumentPosition(const DocumentPosition& other);
    ~DocumentPosition();

    DocumentPosition& operator=(const DocumentPosition& other);
    bool operator==(const DocumentPosition& other) const;
    bool operator!=(const DocumentPosition& other) const;
    bool operator<(const DocumentPosition& other) const;
    bool operator<=(const DocumentPosition& other) const;
    bool operator>(const DocumentPosition& other) const;
    bool operator>=(const DocumentPosition& other) const;

    DocumentNode* GetNode() const;
    int32 GetOffset() const;
    void SetNode(DocumentNode* node);
    void SetOffset(int32 offset);

    bool IsValid() const;
    void Invalidate();

private:
    DocumentNode* fNode;
    int32 fOffset;
};

/**
 * DocumentRange represents a range in the document
 */
class DocumentRange {
public:
    DocumentRange();
    DocumentRange(const DocumentPosition& start, const DocumentPosition& end);
    DocumentRange(const DocumentRange& other);
    ~DocumentRange();

    DocumentRange& operator=(const DocumentRange& other);

    const DocumentPosition& GetStart() const;
    const DocumentPosition& GetEnd() const;
    void SetStart(const DocumentPosition& start);
    void SetEnd(const DocumentPosition& end);

    bool IsValid() const;
    bool IsCollapsed() const;
    bool Contains(const DocumentPosition& position) const;

private:
    DocumentPosition fStart;
    DocumentPosition fEnd;
};

/**
 * NodeType enum for document node types
 */
enum NodeType {
    NODE_TYPE_DOCUMENT,
    NODE_TYPE_SECTION,
    NODE_TYPE_PARAGRAPH,
    NODE_TYPE_TEXT_RUN,
    NODE_TYPE_LIST,
    NODE_TYPE_LIST_ITEM,
    NODE_TYPE_TABLE,
    NODE_TYPE_TABLE_ROW,
    NODE_TYPE_TABLE_CELL,
    NODE_TYPE_IMAGE,
    NODE_TYPE_PAGE_BREAK,
    NODE_TYPE_HEADER_FOOTER,
    NODE_TYPE_METADATA
};

/**
 * Base class for all document nodes
 */
class DocumentNode {
public:
    DocumentNode(NodeType type);
    virtual ~DocumentNode();

    NodeType GetType() const;
    
    // Hierarchy
    DocumentNode* GetParent() const;
    void SetParent(DocumentNode* parent);
    
    size_t GetChildCount() const;
    DocumentNode* GetChild(size_t index) const;
    void AddChild(DocumentNode* child);
    void InsertChild(size_t index, DocumentNode* child);
    void RemoveChild(size_t index);
    void RemoveChild(DocumentNode* child);
    void RemoveAllChildren();
    
    // Find operations
    size_t GetIndexOf(const DocumentNode* child) const;
    bool Contains(const DocumentNode* child) const;
    
    // Document access
    virtual YellowDocument* GetDocument();
    virtual const YellowDocument* GetDocument() const;

    // Text operations (for text-containing nodes)
    virtual BString GetText() const;
    virtual void SetText(const BString& text);
    virtual int32 GetTextLength() const;
    
    // Position operations
    virtual DocumentPosition GetStartPosition() const;
    virtual DocumentPosition GetEndPosition() const;
    virtual DocumentPosition GetPositionAt(int32 offset) const;
    
    // Clone
    virtual DocumentNode* Clone() const = 0;

protected:
    NodeType fType;
    DocumentNode* fParent;
    std::vector<DocumentNode*> fChildren;
};

} // namespace YellowDocumentCore

#endif // YELLOW_DOCUMENT_CORE_DOCUMENT_NODE_H
