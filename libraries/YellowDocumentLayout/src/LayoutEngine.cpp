#include <YellowDocumentLayout/LayoutEngine.h>

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
LayoutEngine::LayoutEngine(YellowDocument* document)
    : fDocument(document),
      fFontCache(new FontCache()),
      fTextLayouter(new TextLayouter(this)),
      fParagraphLayouter(new ParagraphLayouter(fTextLayouter)),
      fPageLayouter(new PageLayouter(this, fParagraphLayouter)),
      fSelectionManager(new SelectionManager()),
      fCaretManager(new CaretManager(this)),
      fHitTester(new HitTester(this)),
      fRenderContext(new RenderContext(this)),
      fViewportSize(612, 792),
      fZoomLevel(1.0f),
      fScrollPosition(0, 0),
      fLayoutValid(false),
      fDocumentSize(0, 0),
      fPageCount(0)
{
}

LayoutEngine::~LayoutEngine()
{
    delete fFontCache;
    delete fTextLayouter;
    delete fParagraphLayouter;
    delete fPageLayouter;
    delete fSelectionManager;
    delete fCaretManager;
    delete fHitTester;
    delete fRenderContext;
}

YellowDocument* LayoutEngine::GetDocument()
{
    return fDocument;
}

const YellowDocument* LayoutEngine::GetDocument() const
{
    return fDocument;
}

void LayoutEngine::SetDocument(YellowDocument* document)
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

FontCache* LayoutEngine::GetFontCache()
{
    return fFontCache;
}

void LayoutEngine::LayoutDocument()
{
    LayoutPass pass;
    LayoutDocument(pass);
}

void LayoutEngine::LayoutDocument(const LayoutPass& pass)
{
    (void)pass;
    if (!fDocument) {
        fLayoutValid = false;
        return;
    }
    fPageLayouter->LayoutDocument(fDocument);
    fLayoutValid = true;
}

void LayoutEngine::LayoutNode(DocumentNode* node)
{
    (void)node;
    InvalidateLayout();
}

void LayoutEngine::LayoutRange(const DocumentRange& range)
{
    (void)range;
    InvalidateLayout();
}

void LayoutEngine::InvalidateLayout()
{
    fLayoutValid = false;
}

void LayoutEngine::InvalidateNode(DocumentNode* node)
{
    (void)node;
    fLayoutValid = false;
}

void LayoutEngine::InvalidateRange(const DocumentRange& range)
{
    (void)range;
    fLayoutValid = false;
}

void LayoutEngine::InvalidateRegion(const LayoutRegion& region)
{
    (void)region;
    fLayoutValid = false;
}

bool LayoutEngine::EnsureLayout()
{
    if (!fLayoutValid) {
        LayoutDocument();
    }
    return fLayoutValid;
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
    if (zoom < 0.1f) zoom = 0.1f;
    if (zoom > 8.0f) zoom = 8.0f;
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
    BPoint bottomRight(fScrollPosition.x + fViewportSize.width / fZoomLevel,
                       fScrollPosition.y + fViewportSize.height / fZoomLevel);
    return BRect(fScrollPosition, bottomRight);
}

void LayoutEngine::RenderDocument(BView* view, const BRect& updateRect)
{
    EnsureLayout();
    fRenderContext->RenderDocument(view, updateRect);
}

void LayoutEngine::RenderNode(BView* view, DocumentNode* node, const BRect& bounds)
{
    (void)node;
    // Full-document rendering is the supported path; node-level rendering
    // reuses the paragraph cache.
    fRenderContext->RenderDocument(view, bounds);
}

DocumentNode* LayoutEngine::HitTest(const BPoint& point)
{
    EnsureLayout();
    BPoint documentPoint = ViewToDocument(point);
    return fHitTester->HitTestParagraph(documentPoint);
}

DocumentPosition LayoutEngine::HitTestPosition(const BPoint& point)
{
    EnsureLayout();
    BPoint documentPoint = ViewToDocument(point);
    return fHitTester->HitTest(documentPoint);
}

BPoint LayoutEngine::DocumentToView(const BPoint& documentPoint) const
{
    return BPoint((documentPoint.x - fScrollPosition.x) * fZoomLevel,
                  (documentPoint.y - fScrollPosition.y) * fZoomLevel);
}

BPoint LayoutEngine::ViewToDocument(const BPoint& viewPoint) const
{
    return BPoint(viewPoint.x / fZoomLevel + fScrollPosition.x,
                  viewPoint.y / fZoomLevel + fScrollPosition.y);
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
    SectionNode* section = GetSection();
    if (section) {
        return BSize(section->pageWidth, section->pageHeight);
    }
    return BSize(8.5f * 72.0f, 11.0f * 72.0f);
}

int32 LayoutEngine::GetPageCount() const
{
    return fPageCount;
}

const std::vector<ParagraphLayout>& LayoutEngine::GetParagraphs() const
{
    return fParagraphs;
}

std::vector<ParagraphLayout>& LayoutEngine::GetParagraphs()
{
    return fParagraphs;
}

const std::vector<PlacedImage>& LayoutEngine::GetImages() const
{
    return fImages;
}

std::vector<PlacedImage>& LayoutEngine::GetImages()
{
    return fImages;
}

const std::vector<PlacedTable>& LayoutEngine::GetTables() const
{
    return fTables;
}

std::vector<PlacedTable>& LayoutEngine::GetTables()
{
    return fTables;
}

const std::vector<PageInfo>& LayoutEngine::GetPages() const
{
    return fPages;
}

std::vector<PageInfo>& LayoutEngine::GetPages()
{
    return fPages;
}

const ParagraphLayout* LayoutEngine::GetParagraphLayout(ParagraphNode* paragraph) const
{
    for (const ParagraphLayout& layout : fParagraphs) {
        if (layout.paragraph == paragraph) {
            return &layout;
        }
    }
    return nullptr;
}

void LayoutEngine::FinishLayout(int32 pageCount, const BSize& documentSize)
{
    fPageCount = pageCount;
    fDocumentSize = documentSize;
    fLayoutValid = true;
}

SectionNode* LayoutEngine::GetSection() const
{
    if (!fDocument) return nullptr;
    SectionNode* section = fDocument->GetRootSection();
    if (section) return section;
    // Fallback: search for the first section in the tree.
    DocumentNode* node = fDocument->GetRoot();
    // Sections are only at the root level in this model.
    (void)node;
    return nullptr;
}

} // namespace YellowDocumentLayout
