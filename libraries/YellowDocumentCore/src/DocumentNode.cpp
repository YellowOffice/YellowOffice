#include "YellowDocumentCore/DocumentNode.h"

#include <algorithm>

namespace YellowDocumentCore {

// DocumentPosition implementation
DocumentPosition::DocumentPosition(DocumentNode* node, int32 offset)
    : fNode(node),
      fOffset(offset)
{
}

DocumentPosition::DocumentPosition(const DocumentPosition& other)
    : fNode(other.fNode),
      fOffset(other.fOffset)
{
}

DocumentPosition::~DocumentPosition()
{
}

DocumentPosition& DocumentPosition::operator=(const DocumentPosition& other)
{
    if (this != &other) {
        fNode = other.fNode;
        fOffset = other.fOffset;
    }
    return *this;
}

bool DocumentPosition::operator==(const DocumentPosition& other) const
{
    return fNode == other.fNode && fOffset == other.fOffset;
}

bool DocumentPosition::operator!=(const DocumentPosition& other) const
{
    return !(*this == other);
}

bool DocumentPosition::operator<(const DocumentPosition& other) const
{
    // Simple comparison - could be enhanced with document tree traversal
    if (fNode == other.fNode) {
        return fOffset < other.fOffset;
    }
    // For different nodes, we'd need to compare their positions in the tree
    // For now, just compare pointer addresses (not ideal, but works for basic cases)
    return fNode < other.fNode;
}

bool DocumentPosition::operator<=(const DocumentPosition& other) const
{
    return *this < other || *this == other;
}

bool DocumentPosition::operator>(const DocumentPosition& other) const
{
    return !(*this <= other);
}

bool DocumentPosition::operator>=(const DocumentPosition& other) const
{
    return !(*this < other);
}

DocumentNode* DocumentPosition::GetNode() const
{
    return fNode;
}

int32 DocumentPosition::GetOffset() const
{
    return fOffset;
}

void DocumentPosition::SetNode(DocumentNode* node)
{
    fNode = node;
}

void DocumentPosition::SetOffset(int32 offset)
{
    fOffset = offset;
}

bool DocumentPosition::IsValid() const
{
    return fNode != nullptr && fOffset >= 0;
}

void DocumentPosition::Invalidate()
{
    fNode = nullptr;
    fOffset = -1;
}

// DocumentRange implementation
DocumentRange::DocumentRange()
    : fStart(),
      fEnd()
{
}

DocumentRange::DocumentRange(const DocumentPosition& start, const DocumentPosition& end)
    : fStart(start),
      fEnd(end)
{
}

DocumentRange::DocumentRange(const DocumentRange& other)
    : fStart(other.fStart),
      fEnd(other.fEnd)
{
}

DocumentRange::~DocumentRange()
{
}

DocumentRange& DocumentRange::operator=(const DocumentRange& other)
{
    if (this != &other) {
        fStart = other.fStart;
        fEnd = other.fEnd;
    }
    return *this;
}

const DocumentPosition& DocumentRange::GetStart() const
{
    return fStart;
}

const DocumentPosition& DocumentRange::GetEnd() const
{
    return fEnd;
}

void DocumentRange::SetStart(const DocumentPosition& start)
{
    fStart = start;
}

void DocumentRange::SetEnd(const DocumentPosition& end)
{
    fEnd = end;
}

bool DocumentRange::IsValid() const
{
    return fStart.IsValid() && fEnd.IsValid();
}

bool DocumentRange::IsCollapsed() const
{
    return fStart == fEnd;
}

bool DocumentRange::Contains(const DocumentPosition& position) const
{
    return position >= fStart && position <= fEnd;
}

// DocumentNode implementation
DocumentNode::DocumentNode(NodeType type)
    : fType(type),
      fParent(nullptr)
{
}

DocumentNode::~DocumentNode()
{
    // Remove all children
    RemoveAllChildren();
}

NodeType DocumentNode::GetType() const
{
    return fType;
}

DocumentNode* DocumentNode::GetParent() const
{
    return fParent;
}

void DocumentNode::SetParent(DocumentNode* parent)
{
    fParent = parent;
}

size_t DocumentNode::GetChildCount() const
{
    return fChildren.size();
}

DocumentNode* DocumentNode::GetChild(size_t index) const
{
    if (index < fChildren.size()) {
        return fChildren[index];
    }
    return nullptr;
}

void DocumentNode::AddChild(DocumentNode* child)
{
    if (child && child != this) {
        fChildren.push_back(child);
        child->SetParent(this);
    }
}

void DocumentNode::InsertChild(size_t index, DocumentNode* child)
{
    if (child && child != this && index <= fChildren.size()) {
        fChildren.insert(fChildren.begin() + index, child);
        child->SetParent(this);
    }
}

void DocumentNode::RemoveChild(size_t index)
{
    if (index < fChildren.size()) {
        DocumentNode* child = fChildren[index];
        child->SetParent(nullptr);
        fChildren.erase(fChildren.begin() + index);
    }
}

void DocumentNode::RemoveChild(DocumentNode* child)
{
    if (child) {
        auto it = std::find(fChildren.begin(), fChildren.end(), child);
        if (it != fChildren.end()) {
            child->SetParent(nullptr);
            fChildren.erase(it);
        }
    }
}

void DocumentNode::RemoveAllChildren()
{
    for (DocumentNode* child : fChildren) {
        child->SetParent(nullptr);
    }
    fChildren.clear();
}

size_t DocumentNode::GetIndexOf(const DocumentNode* child) const
{
    if (child) {
        for (size_t i = 0; i < fChildren.size(); ++i) {
            if (fChildren[i] == child) {
                return i;
            }
        }
    }
    return (size_t)-1; // Not found
}

bool DocumentNode::Contains(const DocumentNode* child) const
{
    if (!child) {
        return false;
    }
    
    // Check direct children
    for (DocumentNode* c : fChildren) {
        if (c == child) {
            return true;
        }
    }
    
    // Check recursively
    for (DocumentNode* c : fChildren) {
        if (c->Contains(child)) {
            return true;
        }
    }
    
    return false;
}

YellowDocument* DocumentNode::GetDocument()
{
    return nullptr; // Default implementation - should be overridden by YellowDocument
}

const YellowDocument* DocumentNode::GetDocument() const
{
    return nullptr; // Default implementation - should be overridden by YellowDocument
}

BString DocumentNode::GetText() const
{
    return BString(); // Default implementation - empty string
}

void DocumentNode::SetText(const BString& text)
{
    // Default implementation - do nothing
}

int32 DocumentNode::GetTextLength() const
{
    return 0; // Default implementation - no text
}

DocumentPosition DocumentNode::GetStartPosition() const
{
    return DocumentPosition(const_cast<DocumentNode*>(this), 0);
}

DocumentPosition DocumentNode::GetEndPosition() const
{
    return DocumentPosition(const_cast<DocumentNode*>(this), 0);
}

DocumentPosition DocumentNode::GetPositionAt(int32 offset) const
{
    return DocumentPosition(const_cast<DocumentNode*>(this), offset);
}

} // namespace YellowDocumentCore
