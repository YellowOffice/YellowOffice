#include "YellowDocumentLayout/LayoutEngine.h"

// Forward declarations for YellowDocumentCore types
namespace YellowDocumentCore {
    class YellowDocument;
    class DocumentNode;
    class DocumentRange;
    class DocumentPosition;
}

namespace YellowDocumentLayout {

// LayoutRegion implementation
LayoutRegion::LayoutRegion()
    : left(0), top(0), right(0), bottom(0)
{
}

LayoutRegion::LayoutRegion(const BRect& rect)
    : left(rect.left), top(rect.top), right(rect.right), bottom(rect.bottom)
{
}

LayoutRegion::LayoutRegion(float l, float t, float r, float b)
    : left(l), top(t), right(r), bottom(b)
{
}

LayoutRegion::~LayoutRegion()
{
}

LayoutRegion& LayoutRegion::operator=(const LayoutRegion& other)
{
    if (this != &other) {
        left = other.left;
        top = other.top;
        right = other.right;
        bottom = other.bottom;
    }
    return *this;
}

bool LayoutRegion::operator==(const LayoutRegion& other) const
{
    return left == other.left && top == other.top && 
           right == other.right && bottom == other.bottom;
}

bool LayoutRegion::operator!=(const LayoutRegion& other) const
{
    return !(*this == other);
}

BRect LayoutRegion::GetRect() const
{
    return BRect(left, top, right, bottom);
}

void LayoutRegion::SetRect(const BRect& rect)
{
    left = rect.left;
    top = rect.top;
    right = rect.right;
    bottom = rect.bottom;
}

bool LayoutRegion::IsValid() const
{
    return left <= right && top <= bottom;
}

bool LayoutRegion::Contains(const BPoint& point) const
{
    return point.x >= left && point.x <= right && 
           point.y >= top && point.y <= bottom;
}

bool LayoutRegion::Intersects(const LayoutRegion& other) const
{
    return left < other.right && right > other.left &&
           top < other.bottom && bottom > other.top;
}

// LayoutResult implementation
LayoutResult::LayoutResult()
    : bounds(0, 0, 0, 0),
      size(0, 0),
      position(0, 0),
      valid(false),
      needsUpdate(false)
{
}

LayoutResult::~LayoutResult()
{
}

// LayoutPass implementation
LayoutPass::LayoutPass()
    : type(LAYOUT_PASS_FULL),
      documentChanged(false),
      fontChanged(false),
      styleChanged(false)
{
}

LayoutPass::~LayoutPass()
{
}

// LayoutEngine implementation
LayoutEngine::LayoutEngine(YellowDocumentCore::YellowDocument* document)
    : fDocument(document),
      fTextLayouter(nullptr),
      fParagraphLayouter(nullptr),
      fPageLayouter(nullptr),
      fSelectionManager(nullptr),
      fCaretManager(nullptr),
      fHitTester(nullptr),
      fRenderContext(nullptr),
      fViewportSize(0, 0),
      fZoomLevel(1.0f),
      fScrollPosition(0, 0),
      fLayoutValid(false),
      fDocumentSize(0, 0),
      fOwnComponents(true)
{
}

LayoutEngine::~LayoutEngine()
{
    // Clean up components if we own them
    if (fOwnComponents) {
        delete fTextLayouter;
        delete fParagraphLayouter;
        delete fPageLayouter;
        delete fSelectionManager;
        delete fCaretManager;
        delete fHitTester;
        delete fRenderContext;
    }
}

YellowDocumentCore::YellowDocument* LayoutEngine::GetDocument()
{
    return fDocument;
}

const YellowDocumentCore::YellowDocument* LayoutEngine::GetDocument() const
{
    return fDocument;
}

void LayoutEngine::SetDocument(YellowDocumentCore::YellowDocument* document)
{
    fDocument = document;
    fLayoutValid = false;
}

TextLayouter* LayoutEngine::GetTextLayouter()
{
    return fTextLayouter;
}

ParagraphLayouter* LayoutEngine::GetParagraphLayouter()
{
    return fParagraphLayouter;
}

PageLayouter* LayoutEngine::GetPageLayouter()
{
    return fPageLayouter;
}

SelectionManager* LayoutEngine::GetSelectionManager()
{
    return fSelectionManager;
}

CaretManager* LayoutEngine::GetCaretManager()
{
    return fCaretManager;
}

HitTester* LayoutEngine::GetHitTester()
{
    return fHitTester;
}

RenderContext* LayoutEngine::GetRenderContext()
{
    return fRenderContext;
}

void LayoutEngine::LayoutDocument()
{
    LayoutPass pass;
    LayoutDocument(pass);
}

void LayoutEngine::LayoutDocument(const LayoutPass& pass)
{
    // TODO: Implement layout
    fLayoutValid = false;
}

void LayoutEngine::LayoutNode(YellowDocumentCore::DocumentNode* node)
{
    // TODO: Implement node layout
}

void LayoutEngine::LayoutRange(const YellowDocumentCore::DocumentRange& range)
{
    // TODO: Implement range layout
}

void LayoutEngine::InvalidateLayout()
{
    fLayoutValid = false;
}

void LayoutEngine::InvalidateNode(YellowDocumentCore::DocumentNode* node)
{
    // TODO: Implement node invalidation
    fLayoutValid = false;
}

void LayoutEngine::InvalidateRange(const YellowDocumentCore::DocumentRange& range)
{
    // TODO: Implement range invalidation
    fLayoutValid = false;
}

void LayoutEngine::InvalidateRegion(const LayoutRegion& region)
{
    // TODO: Implement region invalidation
    fLayoutValid = false;
}

void LayoutEngine::SetViewportSize(const BSize& size)
{
    fViewportSize = size;
}

BSize LayoutEngine::GetViewportSize() const
{
    return fViewportSize;
}

void LayoutEngine::SetZoomLevel(float zoom)
{
    fZoomLevel = zoom;
}

float LayoutEngine::GetZoomLevel() const
{
    return fZoomLevel;
}

void LayoutEngine::SetScrollPosition(const BPoint& position)
{
    fScrollPosition = position;
}

BPoint LayoutEngine::GetScrollPosition() const
{
    return fScrollPosition;
}

BRect LayoutEngine::GetVisibleRect() const
{
    return BRect(fScrollPosition, fScrollPosition + fViewportSize);
}

void LayoutEngine::RenderDocument(BView* view, const BRect& updateRect)
{
    // TODO: Implement rendering
}

void LayoutEngine::RenderNode(BView* view, YellowDocumentCore::DocumentNode* node, const BRect& bounds)
{
    // TODO: Implement node rendering
}

YellowDocumentCore::DocumentNode* LayoutEngine::HitTest(const BPoint& point)
{
    return nullptr; // TODO: Implement hit testing
}

YellowDocumentCore::DocumentPosition LayoutEngine::HitTestPosition(const BPoint& point)
{
    return YellowDocumentCore::DocumentPosition(); // TODO: Implement position hit testing
}

BPoint LayoutEngine::DocumentToView(const BPoint& documentPoint) const
{
    return BPoint(documentPoint.x * fZoomLevel - fScrollPosition.x,
                  documentPoint.y * fZoomLevel - fScrollPosition.y);
}

BPoint LayoutEngine::ViewToDocument(const BPoint& viewPoint) const
{
    return BPoint((viewPoint.x + fScrollPosition.x) / fZoomLevel,
                  (viewPoint.y + fScrollPosition.y) / fZoomLevel);
}

BRect LayoutEngine::DocumentToView(const BRect& documentRect) const
{
    BPoint leftTop = DocumentToView(documentRect.LeftTop());
    BPoint rightBottom = DocumentToView(documentRect.RightBottom());
    return BRect(leftTop, rightBottom);
}

BRect LayoutEngine::ViewToDocument(const BRect& viewRect) const
{
    BPoint leftTop = ViewToDocument(viewRect.LeftTop());
    BPoint rightBottom = ViewToDocument(viewRect.RightBottom());
    return BRect(leftTop, rightBottom);
}

BSize LayoutEngine::GetDocumentSize() const
{
    return fDocumentSize;
}

BSize LayoutEngine::GetPageSize() const
{
    return BSize(8.5f * 72.0f, 11.0f * 72.0f); // Default page size in points
}

int32 LayoutEngine::GetPageCount() const
{
    return 1; // TODO: Implement page count
}

} // namespace YellowDocumentLayout
