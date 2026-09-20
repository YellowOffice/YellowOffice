#ifndef YELLOW_DOCUMENT_LAYOUT_LAYOUT_ENGINE_H
#define YELLOW_DOCUMENT_LAYOUT_LAYOUT_ENGINE_H

#include <SupportDefs.h>
#include <Rect.h>
#include <Point.h>
#include <Size.h>
#include <vector>

#include <YellowCommon/Defines.h>

namespace YellowDocumentCore {
    class YellowDocument;
    class DocumentNode;
}

namespace YellowDocumentLayout {

// Forward declarations
class TextLayouter;
class ParagraphLayouter;
class PageLayouter;
class SelectionManager;
class CaretManager;
class HitTester;
class RenderContext;

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
 * Main layout engine class
 */
class LayoutEngine {
public:
    LayoutEngine(YellowDocumentCore::YellowDocument* document);
    ~LayoutEngine();

    // Document access
    YellowDocumentCore::YellowDocument* GetDocument();
    const YellowDocumentCore::YellowDocument* GetDocument() const;
    void SetDocument(YellowDocumentCore::YellowDocument* document);

    // Layout components
    TextLayouter* GetTextLayouter();
    ParagraphLayouter* GetParagraphLayouter();
    PageLayouter* GetPageLayouter();
    SelectionManager* GetSelectionManager();
    CaretManager* GetCaretManager();
    HitTester* GetHitTester();
    RenderContext* GetRenderContext();

    // Layout operations
    void LayoutDocument();
    void LayoutDocument(const LayoutPass& pass);
    void LayoutNode(YellowDocumentCore::DocumentNode* node);
    void LayoutRange(const YellowDocumentCore::DocumentRange& range);

    // Invalidation
    void InvalidateLayout();
    void InvalidateNode(YellowDocumentCore::DocumentNode* node);
    void InvalidateRange(const YellowDocumentCore::DocumentRange& range);
    void InvalidateRegion(const LayoutRegion& region);

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
    void RenderNode(BView* view, YellowDocumentCore::DocumentNode* node, const BRect& bounds);

    // Hit testing
    YellowDocumentCore::DocumentNode* HitTest(const BPoint& point);
    YellowDocumentCore::DocumentPosition HitTestPosition(const BPoint& point);

    // Coordinate conversion
    BPoint DocumentToView(const BPoint& documentPoint) const;
    BPoint ViewToDocument(const BPoint& viewPoint) const;
    BRect DocumentToView(const BRect& documentRect) const;
    BRect ViewToDocument(const BRect& viewRect) const;

    // Document dimensions
    BSize GetDocumentSize() const;
    BSize GetPageSize() const;
    int32 GetPageCount() const;

private:
    YellowDocumentCore::YellowDocument* fDocument;
    
    // Layout components
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
    
    // Own the components
    bool fOwnComponents;
};

} // namespace YellowDocumentLayout

#endif // YELLOW_DOCUMENT_LAYOUT_LAYOUT_ENGINE_H
