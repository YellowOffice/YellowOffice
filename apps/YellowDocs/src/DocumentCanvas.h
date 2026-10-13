#ifndef YELLOW_DOCS_DOCUMENT_CANVAS_H
#define YELLOW_DOCS_DOCUMENT_CANVAS_H

#include <View.h>
#include <String.h>

#include <YellowDocumentCore/YellowDocument.h>
#include <YellowDocumentLayout/LayoutEngine.h>

/**
 * DocumentCanvas - the editing view. Owns the layout engine and routes
 * mouse/keyboard input to the document through the command history.
 */
class DocumentCanvas : public BView {
public:
    DocumentCanvas(BRect frame);
    virtual ~DocumentCanvas();

    void SetDocument(YellowDocumentCore::YellowDocument* document);
    YellowDocumentCore::YellowDocument* GetDocument() const;

    // Zoom
    void SetZoom(float zoom);
    float GetZoom() const;
    void ZoomIn();
    void ZoomOut();
    void ZoomReset();

    // Editing operations (all undoable where applicable)
    void InsertText(const BString& text);
    void DeleteSelectionOrBackspace();
    void SplitParagraph();
    void SelectAll();
    void Undo();
    void Redo();

    void CopyToClipboard();
    void CutToClipboard();
    void PasteFromClipboard();

    void ToggleBold();
    void ToggleItalic();
    void ToggleUnderline();
    void SetAlignment(YellowDocumentCore::ParagraphStyle::Alignment alignment);

    bool CanUndo() const;
    bool CanRedo() const;
    bool HasSelection() const;

    // Find: returns true if found and moved the caret/selection to the match.
    bool FindNext(const BString& text, bool forward = true);

    // BView overrides
    virtual void Draw(BRect updateRect);
    virtual void MouseDown(BPoint where);
    virtual void MouseUp(BPoint where);
    virtual void MouseMoved(BPoint where, uint32 transit, const BMessage* dragMessage);
    virtual void KeyDown(const char* bytes, int32 numBytes);
    virtual void FrameResized(float newWidth, float newHeight);
    virtual void AttachedToWindow();
    virtual void GetPreferredSize(float* _width, float* _height);

private:
    using DocumentPosition = YellowDocumentCore::DocumentPosition;
    using DocumentRange = YellowDocumentCore::DocumentRange;
    using ParagraphNode = YellowDocumentCore::ParagraphNode;

    void InvalidateAndNotify();
    void MoveCaretTo(const DocumentPosition& position, bool extendSelection);
    void DeleteTextRange(ParagraphNode* paragraph, int32 offset, int32 length);
    bool SelectionToRange(DocumentRange* outRange, ParagraphNode** outParagraph,
                          int32* outStart, int32* outEnd) const;
    void HandleCommandKey(char key);
    void InsertCharacter(const char* bytes, int32 numBytes);
    void EnsureCaretVisible();

    YellowDocumentLayout::LayoutEngine* fEngine;
    YellowDocumentCore::YellowDocument* fDocument;
    bool fDragging;
    DocumentPosition fDragAnchor;
};

#endif // YELLOW_DOCS_DOCUMENT_CANVAS_H
