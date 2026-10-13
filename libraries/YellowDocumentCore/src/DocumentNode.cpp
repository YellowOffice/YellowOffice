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

namespace {

// Compares two nodes by document tree order (a node precedes its
// descendants; siblings compare by child index; unrelated roots compare by
// pointer address as a stable fallback).
int CompareNodesInTree(DocumentNode* a, DocumentNode* b)
{
    if (a == b) return 0;
    if (a == nullptr) return -1;
    if (b == nullptr) return 1;

    DocumentNode* parentA = a->GetParent();
    DocumentNode* parentB = b->GetParent();

    if (parentA == nullptr && parentB == nullptr) {
        return a < b ? -1 : 1;
    }
    if (parentA == nullptr) {
        // a is either an ancestor of b or an unrelated root.
        return CompareNodesInTree(a, parentB) <= 0 ? -1 : 1;
    }
    if (parentB == nullptr) {
        return CompareNodesInTree(parentA, b) >= 0 ? 1 : -1;
    }

    int parentOrder = CompareNodesInTree(parentA, parentB);
    if (parentOrder != 0) {
        return parentOrder;
    }
    size_t indexA = parentA->GetIndexOf(a);
    size_t indexB = parentB->GetIndexOf(b);
    if (indexA < indexB) return -1;
    if (indexA > indexB) return 1;
    return 0;
}

} // namespace

int DocumentPosition::CompareInTree(const DocumentPosition& a, const DocumentPosition& b)
{
    if (a.fNode == b.fNode) {
        if (a.fOffset < b.fOffset) return -1;
        if (a.fOffset > b.fOffset) return 1;
        return 0;
    }
    int nodeOrder = CompareNodesInTree(a.fNode, b.fNode);
    if (nodeOrder != 0) {
        return nodeOrder;
    }
    // Same node reached differently (shouldn't happen), compare offsets.
    if (a.fOffset < b.fOffset) return -1;
    if (a.fOffset > b.fOffset) return 1;
    return 0;
}

bool DocumentPosition::operator<(const DocumentPosition& other) const
{
    return CompareInTree(*this, other) < 0;
}

bool DocumentPosition::operator<=(const DocumentPosition& other) const
{
    return CompareInTree(*this, other) <= 0;
}

bool DocumentPosition::operator>(const DocumentPosition& other) const
{
    return CompareInTree(*this, other) > 0;
}

bool DocumentPosition::operator>=(const DocumentPosition& other) const
{
    return CompareInTree(*this, other) >= 0;
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

DocumentPosition DocumentPosition::NextPosition() const
{
    if (!IsValid()) return DocumentPosition();

    DocumentNode* node = fNode;
    int32 offset = fOffset;

    // Move within the current text-bearing node if possible.
    if (node->GetType() == NODE_TYPE_TEXT_RUN || node->GetTextLength() > 0) {
        int32 length = node->GetTextLength();
        if (offset < length) {
            return DocumentPosition(node, offset + 1);
        }
    }

    // Move to the next text-bearing node in document order.
    DocumentNode* current = node;
    while (current != nullptr) {
        DocumentNode* parent = current->GetParent();
        if (parent == nullptr) return DocumentPosition();
        size_t index = parent->GetIndexOf(current);
        for (size_t i = index + 1; i < parent->GetChildCount(); ++i) {
            DocumentNode* sibling = parent->GetChild(i);
            if (sibling == nullptr) continue;
            if (sibling->GetTextLength() > 0 ||
                sibling->GetType() == NODE_TYPE_PARAGRAPH) {
                // Descend to first leaf.
                DocumentNode* leaf = sibling;
                while (leaf->GetChildCount() > 0 &&
                       leaf->GetType() != NODE_TYPE_TEXT_RUN) {
                    leaf = leaf->GetChild(0);
                }
                if (leaf->GetType() == NODE_TYPE_TEXT_RUN) {
                    return DocumentPosition(leaf, 0);
                }
            }
        }
        current = parent;
    }
    return DocumentPosition();
}

DocumentPosition DocumentPosition::PreviousPosition() const
{
    if (!IsValid()) return DocumentPosition();

    DocumentNode* node = fNode;
    int32 offset = fOffset;

    if (offset > 0) {
        return DocumentPosition(node, offset - 1);
    }

    // Move to the previous text-bearing node.
    DocumentNode* current = node;
    while (current != nullptr) {
        DocumentNode* parent = current->GetParent();
        if (parent == nullptr) return DocumentPosition();
        size_t index = parent->GetIndexOf(current);
        for (size_t i = index; i-- > 0; ) {
            DocumentNode* sibling = parent->GetChild(i);
            if (sibling == nullptr) continue;
            if (sibling->GetTextLength() > 0) {
                // Descend to last leaf.
                DocumentNode* leaf = sibling;
                while (leaf->GetChildCount() > 0 &&
                       leaf->GetType() != NODE_TYPE_TEXT_RUN) {
                    leaf = leaf->GetChild(leaf->GetChildCount() - 1);
                }
                if (leaf->GetType() == NODE_TYPE_TEXT_RUN) {
                    return DocumentPosition(leaf, leaf->GetTextLength());
                }
            }
        }
        current = parent;
    }
    return DocumentPosition();
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

void DocumentRange::Normalize()
{
    if (fStart > fEnd) {
        DocumentPosition temp = fStart;
        fStart = fEnd;
        fEnd = temp;
    }
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
      fParent(nullptr),
      fDocument(nullptr)
{
}

DocumentNode::~DocumentNode()
{
    for (DocumentNode* child : fChildren) {
        if (child) {
            child->fParent = nullptr;
            delete child;
        }
    }
    fChildren.clear();
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
        // Detach from previous parent.
        if (child->fParent) {
            child->fParent->RemoveChild(child);
        }
        fChildren.push_back(child);
        child->fParent = this;
        if (fDocument) {
            child->SetDocument(fDocument);
        }
    }
}

void DocumentNode::InsertChild(size_t index, DocumentNode* child)
{
    if (child && child != this && index <= fChildren.size()) {
        if (child->fParent) {
            child->fParent->RemoveChild(child);
        }
        fChildren.insert(fChildren.begin() + index, child);
        child->fParent = this;
        if (fDocument) {
            child->SetDocument(fDocument);
        }
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
        if (child) {
            child->SetParent(nullptr);
        }
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

    for (DocumentNode* c : fChildren) {
        if (c == child || (c && c->Contains(child))) {
            return true;
        }
    }
    return false;
}

YellowDocument* DocumentNode::GetDocument()
{
    return fDocument;
}

const YellowDocument* DocumentNode::GetDocument() const
{
    return fDocument;
}

void DocumentNode::SetDocument(YellowDocument* document)
{
    fDocument = document;
    for (DocumentNode* child : fChildren) {
        if (child) {
            child->SetDocument(document);
        }
    }
}

YellowDocument* DocumentNode::GetOwningDocument() const
{
    const DocumentNode* node = this;
    while (node) {
        if (node->fDocument) {
            return node->fDocument;
        }
        node = node->fParent;
    }
    return nullptr;
}

void DocumentNode::PropagateDocument(YellowDocument* document)
{
    SetDocument(document);
}

BString DocumentNode::GetText() const
{
    return BString();
}

void DocumentNode::SetText(const BString& text)
{
    (void)text;
}

int32 DocumentNode::GetTextLength() const
{
    return 0;
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
