#ifndef YELLOW_DOCUMENT_LAYOUT_LAYOUT_ENGINE_H
#define YELLOW_DOCUMENT_LAYOUT_LAYOUT_ENGINE_H

#include <SupportDefs.h>
#include <Rect.h>
#include <Point.h>
#include <Size.h>
#include <vector>

#include <YellowCommon/Defines.h>
#include <YellowDocumentLayout/LayoutTypes.h>
#include <YellowDocumentLayout/TextLayouter.h>
#include <YellowDocumentLayout/ParagraphLayouter.h>
#include <YellowDocumentLayout/PageLayouter.h>
#include <YellowDocumentLayout/SelectionManager.h>
#include <YellowDocumentLayout/CaretManager.h>
#include <YellowDocumentLayout/HitTester.h>
#include <YellowDocumentLayout/RenderContext.h>

class BView;

namespace YellowDocumentCore {
    class YellowDocument;
    class DocumentNode;
}

namespace YellowDocumentLayout {

using YellowDocumentCore::DocumentNode;
using YellowDocumentCore::DocumentPosition;
using YellowDocumentCore::DocumentRange;
using YellowDocumentCore::ParagraphNode;
using YellowDocumentCore::SectionNode;
using YellowDocumentCore::YellowDocument;

/**
 * Layout region represents a rectangular area in the document
 */
class LayoutRegion {
public:
    LayoutRegion();
    LayoutRegion(const BRect& rect);
    LayoutRegion(float left, float top, float right, float bottom);
    ~LayoutRegion();

    LayoutRegion& operator=(const LayoutRegion& other);
    bool operator==(const LayoutRegion& other) const;
    bool operator!=(const LayoutRegion& other) const;

    BRect GetRect() const;
    void SetRect(const BRect& rect);

    float left;
    float top;
    float right;
    float bottom;

    bool IsValid() const;
    bool Contains(const BPoint& point) const;
    bool Intersects(const LayoutRegion& other) const;
};

/**
 * Layout result for a node
 */
class LayoutResult {
public:
    LayoutResult();
    ~LayoutResult();

    BRect bounds;
    BSize size;
    BPoint position;
    bool valid;
    bool needsUpdate;
};

/**
 * Layout pass information
 */
class LayoutPass {
public:
    LayoutPass();
    ~LayoutPass();

    enum Type {
        LAYOUT_PASS_FULL,
        LAYOUT_PASS_INCREMENTAL,
        LAYOUT_PASS_FORCE
    };

    Type type;
    LayoutRegion dirtyRegion;
    bool documentChanged;
    bool fontChanged;
    bool styleChanged;
};

/**
 * Main layout engine: owns the layout cache and component objects.
 */
class LayoutEngine {
public:
    explicit LayoutEngine(YellowDocument* document);
    ~LayoutEngine();

    // Document access
    YellowDocument* GetDocument();
    const YellowDocument* GetDocument() const;
    void SetDocument(YellowDocument* document);

    // Layout components
    TextLayouter* GetTextLayouter();
    ParagraphLayouter* GetParagraphLayouter();
    PageLayouter* GetPageLayouter();
    SelectionManager* GetSelectionManager();
    CaretManager* GetCaretManager();
    HitTester* GetHitTester();
    RenderContext* GetRenderContext();
    FontCache* GetFontCache();

    // Layout operations
    void LayoutDocument();
    void LayoutDocument(const LayoutPass& pass);
    void LayoutNode(DocumentNode* node);
    void LayoutRange(const DocumentRange& range);

    // Invalidation
    void InvalidateLayout();
    void InvalidateNode(DocumentNode* node);
    void InvalidateRange(const DocumentRange& range);
    void InvalidateRegion(const LayoutRegion& region);

    /// True when the cache is valid; performs layout if stale.
    bool EnsureLayout();

    // Viewport
    void SetViewportSize(const BSize& size);
    BSize GetViewportSize() const;
    void SetZoomLevel(float zoom);
    float GetZoomLevel() const;

    // Scrolling
    void SetScrollPosition(const BPoint& position);
    BPoint GetScrollPosition() const;
    BRect GetVisibleRect() const;

    // Rendering
    void RenderDocument(BView* view, const BRect& updateRect);
    void RenderNode(BView* view, DocumentNode* node, const BRect& bounds);

    // Hit testing
    DocumentNode* HitTest(const BPoint& point);
    DocumentPosition HitTestPosition(const BPoint& point);

    // Coordinate conversion (document points <-> view points)
    BPoint DocumentToView(const BPoint& documentPoint) const;
    BPoint ViewToDocument(const BPoint& viewPoint) const;
    BRect DocumentToView(const BRect& documentRect) const;
    BRect ViewToDocument(const BRect& viewRect) const;

    // Document dimensions (unscaled document points)
    BSize GetDocumentSize() const;
    BSize GetPageSize() const;
    int32 GetPageCount() const;

    // Layout cache access (used by components and the canvas)
    const std::vector<ParagraphLayout>& GetParagraphs() const;
    std::vector<ParagraphLayout>& GetParagraphs();
    const std::vector<PlacedImage>& GetImages() const;
    std::vector<PlacedImage>& GetImages();
    const std::vector<PlacedTable>& GetTables() const;
    std::vector<PlacedTable>& GetTables();
    const std::vector<PageInfo>& GetPages() const;
    std::vector<PageInfo>& GetPages();

    const ParagraphLayout* GetParagraphLayout(ParagraphNode* paragraph) const;

    /// Called by PageLayouter when layout finishes.
    void FinishLayout(int32 pageCount, const BSize& documentSize);

    SectionNode* GetSection() const;

private:
    YellowDocument* fDocument;

    // Components
    FontCache* fFontCache;
    TextLayouter* fTextLayouter;
    ParagraphLayouter* fParagraphLayouter;
    PageLayouter* fPageLayouter;
    SelectionManager* fSelectionManager;
    CaretManager* fCaretManager;
    HitTester* fHitTester;
    RenderContext* fRenderContext;

    // Viewport state
    BSize fViewportSize;
    float fZoomLevel;
    BPoint fScrollPosition;

    // Layout cache
    bool fLayoutValid;
    BSize fDocumentSize;
    int32 fPageCount;
    std::vector<ParagraphLayout> fParagraphs;
    std::vector<PlacedImage> fImages;
    std::vector<PlacedTable> fTables;
    std::vector<PageInfo> fPages;
};

} // namespace YellowDocumentLayout

#endif // YELLOW_DOCUMENT_LAYOUT_LAYOUT_ENGINE_H
