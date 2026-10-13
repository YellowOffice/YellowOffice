#include "DocumentCanvas.h"

#include <Message.h>
#include <Window.h>
#include <Clipboard.h>
#include <InterfaceDefs.h>
#include <stdio.h>
#include <string.h>

#include <YellowDocumentCore/Command.h>
#include <YellowDocumentCore/Nodes.h>

using namespace YellowDocumentCore;
using namespace YellowDocumentLayout;

// Posted to the window after every document mutation.
static const uint32 kMsgDocumentEdited = 'YDed';

DocumentCanvas::DocumentCanvas(BRect frame)
    : BView(frame, "DocumentCanvas",
            B_FOLLOW_ALL_SIDES,
            B_WILL_DRAW | B_FRAME_EVENTS | B_NAVIGABLE | B_INPUT_METHOD_AWARE),
      fEngine(nullptr),
      fDocument(nullptr),
      fDragging(false)
{
    SetViewColor(200, 200, 200);
}

DocumentCanvas::~DocumentCanvas()
{
    delete fEngine;
}

void DocumentCanvas::SetDocument(YellowDocument* document)
{
    if (fDocument == document) return;
    fDocument = document;
    delete fEngine;
    fEngine = nullptr;
    if (fDocument) {
        fEngine = new LayoutEngine(fDocument);
        fEngine->SetViewportSize(BSize(Bounds().Width(), Bounds().Height()));
        fEngine->LayoutDocument();
        // Put the caret at the start of the first paragraph.
        YellowDocumentLayout::SelectionManager* selection =
            fEngine->GetSelectionManager();
        YellowDocumentLayout::CaretManager* caret = fEngine->GetCaretManager();
        if (selection && caret) {
            selection->SelectAll(fDocument);
            selection->Clear();
            DocumentNode* root = fDocument->GetRoot();
            if (root && root->GetChildCount() > 0) {
                DocumentNode* first = root->GetChild(0);
                caret->SetPosition(DocumentPosition(first, 0));
                caret->SetVisible(true);
            }
        }
    }
    Invalidate();
}

YellowDocument* DocumentCanvas::GetDocument() const
{
    return fDocument;
}

void DocumentCanvas::SetZoom(float zoom)
{
    if (!fEngine) return;
    if (zoom < 0.25f) zoom = 0.25f;
    if (zoom > 4.0f) zoom = 4.0f;
    fEngine->SetZoomLevel(zoom);
    fEngine->InvalidateLayout();
    Invalidate();
}

float DocumentCanvas::GetZoom() const
{
    return fEngine ? fEngine->GetZoomLevel() : 1.0f;
}

void DocumentCanvas::ZoomIn()
{
    SetZoom(GetZoom() * 1.1f);
}

void DocumentCanvas::ZoomOut()
{
    SetZoom(GetZoom() / 1.1f);
}

void DocumentCanvas::ZoomReset()
{
    SetZoom(1.0f);
}

void DocumentCanvas::InvalidateAndNotify()
{
    if (fEngine) {
        fEngine->InvalidateLayout();
        fEngine->LayoutDocument();
    }
    Invalidate();
    if (Window()) {
        BMessage message(kMsgDocumentEdited);
        Window()->PostMessage(&message);
    }
}

void DocumentCanvas::MoveCaretTo(const DocumentPosition& position, bool extendSelection)
{
    if (!fEngine) return;
    SelectionManager* selection = fEngine->GetSelectionManager();
    CaretManager* caret = fEngine->GetCaretManager();
    if (!selection || !caret) return;

    if (extendSelection) {
        selection->ExtendTo(position);
    } else {
        selection->Collapse(position);
    }
    caret->SetPosition(position);
    caret->SetVisible(true);
    Invalidate();
}

bool DocumentCanvas::SelectionToRange(DocumentRange* outRange,
                                      ParagraphNode** outParagraph,
                                      int32* outStart, int32* outEnd) const
{
    if (!fEngine || !outRange || !outParagraph || !outStart || !outEnd) return false;
    SelectionManager* selection = fEngine->GetSelectionManager();
    if (!selection || selection->IsEmpty()) return false;

    DocumentRange range = selection->GetRange();
    range.Normalize();
    *outRange = range;

    DocumentNode* startNode = range.GetStart().GetNode();
    DocumentNode* endNode = range.GetEnd().GetNode();
    if (!startNode || startNode != endNode) return false; // multi-paragraph: not handled here

    ParagraphNode* paragraph = dynamic_cast<ParagraphNode*>(startNode);
    if (!paragraph) return false;

    *outParagraph = paragraph;
    *outStart = range.GetStart().GetOffset();
    *outEnd = range.GetEnd().GetOffset();
    if (*outEnd < *outStart) {
        int32 tmp = *outStart;
        *outStart = *outEnd;
        *outEnd = tmp;
    }
    return true;
}

void DocumentCanvas::DeleteTextRange(ParagraphNode* paragraph, int32 offset, int32 length)
{
    if (!fDocument || !paragraph || length <= 0) return;
    fDocument->ExecuteCommand(new DeleteTextCommand(fDocument, paragraph, offset, length));
}

void DocumentCanvas::InsertText(const BString& text)
{
    if (!fDocument || !fEngine || text.IsEmpty()) return;

    SelectionManager* selection = fEngine->GetSelectionManager();
    CaretManager* caret = fEngine->GetCaretManager();
    if (!selection || !caret) return;

    // Replace an active selection first.
    DocumentRange range;
    ParagraphNode* paragraph = nullptr;
    int32 start = 0;
    int32 end = 0;
    if (SelectionToRange(&range, &paragraph, &start, &end) && end > start) {
        DeleteTextRange(paragraph, start, end - start);
        selection->Collapse(DocumentPosition(paragraph, start));
        caret->SetPosition(DocumentPosition(paragraph, start));
    }

    DocumentPosition position = caret->GetPosition();
    paragraph = dynamic_cast<ParagraphNode*>(position.GetNode());
    if (!paragraph) return;

    fDocument->ExecuteCommand(
        new InsertTextCommand(fDocument, paragraph, position.GetOffset(), text));
    DocumentPosition after(paragraph, position.GetOffset() + text.Length());
    selection->Collapse(after);
    caret->SetPosition(after);
    InvalidateAndNotify();
}

void DocumentCanvas::DeleteSelectionOrBackspace()
{
    if (!fDocument || !fEngine) return;
    SelectionManager* selection = fEngine->GetSelectionManager();
    CaretManager* caret = fEngine->GetCaretManager();
    if (!selection || !caret) return;

    DocumentRange range;
    ParagraphNode* paragraph = nullptr;
    int32 start = 0;
    int32 end = 0;
    if (SelectionToRange(&range, &paragraph, &start, &end) && end > start) {
        DeleteTextRange(paragraph, start, end - start);
        DocumentPosition caretPos(paragraph, start);
        selection->Collapse(caretPos);
        caret->SetPosition(caretPos);
        InvalidateAndNotify();
        return;
    }

    // Backspace: delete the character before the caret.
    DocumentPosition position = caret->GetPosition();
    paragraph = dynamic_cast<ParagraphNode*>(position.GetNode());
    if (!paragraph) return;
    if (position.GetOffset() > 0) {
        DeleteTextRange(paragraph, position.GetOffset() - 1, 1);
        DocumentPosition after(paragraph, position.GetOffset() - 1);
        selection->Collapse(after);
        caret->SetPosition(after);
        InvalidateAndNotify();
    } else {
        // At paragraph start: join with the previous paragraph if possible.
        DocumentNode* parent = paragraph->GetParent();
        if (!parent) return;
        size_t index = parent->GetIndexOf(paragraph);
        if (index == 0 || index == (size_t)-1) return;
        DocumentNode* prevNode = parent->GetChild(index - 1);
        ParagraphNode* prev = dynamic_cast<ParagraphNode*>(prevNode);
        if (!prev) return;
        int32 prevLength = prev->GetTextLength();
        BString text = paragraph->GetText();
        if (!text.IsEmpty()) {
            fDocument->ExecuteCommand(
                new InsertTextCommand(fDocument, prev, prevLength, text));
        }
        parent->RemoveChild(paragraph);
        delete paragraph;
        DocumentPosition caretPos(prev, prevLength);
        selection->Collapse(caretPos);
        caret->SetPosition(caretPos);
        InvalidateAndNotify();
    }
}

void DocumentCanvas::SplitParagraph()
{
    if (!fDocument || !fEngine) return;
    CaretManager* caret = fEngine->GetCaretManager();
    SelectionManager* selection = fEngine->GetSelectionManager();
    if (!caret || !selection) return;

    DocumentPosition position = caret->GetPosition();
    ParagraphNode* paragraph = dynamic_cast<ParagraphNode*>(position.GetNode());
    if (!paragraph) return;

    fDocument->ExecuteCommand(
        new SplitParagraphCommand(fDocument, paragraph, position.GetOffset()));
    // After the split the caret belongs at the start of the new paragraph.
    DocumentNode* parent = paragraph->GetParent();
    if (parent) {
        size_t index = parent->GetIndexOf(paragraph);
        if (index != (size_t)-1 && index + 1 < parent->GetChildCount()) {
            DocumentNode* next = parent->GetChild(index + 1);
            DocumentPosition caretPos(next, 0);
            selection->Collapse(caretPos);
            caret->SetPosition(caretPos);
        }
    }
    InvalidateAndNotify();
}

void DocumentCanvas::SelectAll()
{
    if (!fDocument || !fEngine) return;
    SelectionManager* selection = fEngine->GetSelectionManager();
    if (!selection) return;
    selection->SelectAll(fDocument);
    Invalidate();
}

void DocumentCanvas::Undo()
{
    if (!fDocument) return;
    fDocument->Undo();
    InvalidateAndNotify();
}

void DocumentCanvas::Redo()
{
    if (!fDocument) return;
    fDocument->Redo();
    InvalidateAndNotify();
}

bool DocumentCanvas::CanUndo() const
{
    return fDocument && fDocument->CanUndo();
}

bool DocumentCanvas::CanRedo() const
{
    return fDocument && fDocument->CanRedo();
}

bool DocumentCanvas::HasSelection() const
{
    if (!fEngine) return false;
    SelectionManager* selection = fEngine->GetSelectionManager();
    return selection && !selection->IsEmpty();
}

void DocumentCanvas::CopyToClipboard()
{
    if (!fDocument || !fEngine) return;
    SelectionManager* selection = fEngine->GetSelectionManager();
    if (!selection || selection->IsEmpty()) return;

    BString text = selection->GetSelectedText(fDocument);
    if (text.IsEmpty()) return;

    if (!be_clipboard) return;
    be_clipboard->Lock();
    be_clipboard->Clear();
    BMessage message;
    message.AddString("data", text);
    be_clipboard->AddData(B_TEXT_FLAVOR, &message);
    be_clipboard->Commit();
    be_clipboard->Unlock();
}

void DocumentCanvas::CutToClipboard()
{
    if (!HasSelection()) return;
    CopyToClipboard();
    DeleteSelectionOrBackspace();
}

void DocumentCanvas::PasteFromClipboard()
{
    if (!be_clipboard) return;
    be_clipboard->Lock();
    void* data = nullptr;
    size_t length = 0;
    if (be_clipboard->FindData(B_TEXT_FLAVOR, &data, &length) == B_OK && data) {
        BString text;
        text.SetTo((const char*)data, (int32)length);
        be_clipboard->Unlock();
        InsertText(text);
        return;
    }
    be_clipboard->Unlock();
}

void DocumentCanvas::ToggleBold()
{
    if (!fDocument || !fEngine) return;
    SelectionManager* selection = fEngine->GetSelectionManager();
    if (!selection || selection->IsEmpty()) return;

    // Detect current bold state from the first run in the selection.
    bool bold = false;
    DocumentRange range = selection->GetRange();
    range.Normalize();
    DocumentNode* startNode = range.GetStart().GetNode();
    ParagraphNode* paragraph = dynamic_cast<ParagraphNode*>(startNode);
    if (paragraph) {
        for (size_t i = 0; i < paragraph->GetChildCount(); ++i) {
            DocumentNode* child = paragraph->GetChild(i);
            if (child && child->GetType() == NODE_TYPE_TEXT_RUN) {
                bold = static_cast<TextRunNode*>(child)->style.bold;
                break;
            }
        }
    }
    fDocument->ExecuteCommand(new ApplyTextStyleCommand(
        fDocument, range, TextStyle(), !bold, false, false, false));
    InvalidateAndNotify();
}

void DocumentCanvas::ToggleItalic()
{
    if (!fDocument || !fEngine) return;
    SelectionManager* selection = fEngine->GetSelectionManager();
    if (!selection || selection->IsEmpty()) return;
    DocumentRange range = selection->GetRange();
    range.Normalize();
    bool italic = false;
    ParagraphNode* paragraph = dynamic_cast<ParagraphNode*>(range.GetStart().GetNode());
    if (paragraph) {
        for (size_t i = 0; i < paragraph->GetChildCount(); ++i) {
            DocumentNode* child = paragraph->GetChild(i);
            if (child && child->GetType() == NODE_TYPE_TEXT_RUN) {
                italic = static_cast<TextRunNode*>(child)->style.italic;
                break;
            }
        }
    }
    fDocument->ExecuteCommand(new ApplyTextStyleCommand(
        fDocument, range, TextStyle(), false, !italic, false, false));
    InvalidateAndNotify();
}

void DocumentCanvas::ToggleUnderline()
{
    if (!fDocument || !fEngine) return;
    SelectionManager* selection = fEngine->GetSelectionManager();
    if (!selection || selection->IsEmpty()) return;
    DocumentRange range = selection->GetRange();
    range.Normalize();
    bool underline = false;
    ParagraphNode* paragraph = dynamic_cast<ParagraphNode*>(range.GetStart().GetNode());
    if (paragraph) {
        for (size_t i = 0; i < paragraph->GetChildCount(); ++i) {
            DocumentNode* child = paragraph->GetChild(i);
            if (child && child->GetType() == NODE_TYPE_TEXT_RUN) {
                underline = static_cast<TextRunNode*>(child)->style.underline;
                break;
            }
        }
    }
    fDocument->ExecuteCommand(new ApplyTextStyleCommand(
        fDocument, range, TextStyle(), false, false, !underline, false));
    InvalidateAndNotify();
}

void DocumentCanvas::SetAlignment(ParagraphStyle::Alignment alignment)
{
    if (!fDocument || !fEngine) return;
    SelectionManager* selection = fEngine->GetSelectionManager();
    CaretManager* caret = fEngine->GetCaretManager();
    if (!selection || !caret) return;

    DocumentRange range;
    if (!selection->IsEmpty()) {
        range = selection->GetRange();
        range.Normalize();
    } else {
        DocumentNode* node = caret->GetPosition().GetNode();
        if (!node) return;
        range = DocumentRange(caret->GetPosition(), caret->GetPosition());
        (void)node;
    }
    ParagraphStyle style;
    style.alignment = alignment;
    fDocument->ExecuteCommand(
        new ApplyParagraphStyleCommand(fDocument, range, style));
    InvalidateAndNotify();
}

bool DocumentCanvas::FindNext(const BString& text, bool forward)
{
    if (!fDocument || !fEngine || text.IsEmpty()) return false;
    CaretManager* caret = fEngine->GetCaretManager();
    SelectionManager* selection = fEngine->GetSelectionManager();
    if (!caret || !selection) return false;

    // Search paragraph by paragraph from the caret.
    DocumentNode* root = fDocument->GetRoot();
    if (!root) return false;

    DocumentNode* startNode = caret->GetPosition().GetNode();
    bool found = false;

    struct Finder {
        static bool Search(DocumentNode* node, const BString& text, bool fromStart,
                           DocumentPosition* matchStart, DocumentPosition* matchEnd) {
            if (!node) return false;
            if (node->GetType() == NODE_TYPE_PARAGRAPH) {
                if (!fromStart) {
                    // Skip paragraphs before the caret: caller handles order;
                    // here we just search every paragraph.
                }
                BString content = node->GetText();
                int32 index = content.FindFirst(text);
                if (index >= 0) {
                    *matchStart = DocumentPosition(node, index);
                    *matchEnd = DocumentPosition(node, index + text.Length());
                    return true;
                }
                return false;
            }
            for (size_t i = 0; i < node->GetChildCount(); ++i) {
                if (Search(node->GetChild(i), text, fromStart, matchStart, matchEnd)) {
                    return true;
                }
            }
            return false;
        }
    };

    // Simple full-document search (forward only for now).
    (void)forward;
    (void)startNode;
    DocumentPosition matchStart;
    DocumentPosition matchEnd;
    if (Finder::Search(root, text, true, &matchStart, &matchEnd)) {
        selection->SetRange(matchStart, matchEnd);
        caret->SetPosition(matchEnd);
        found = true;
        Invalidate();
    }
    return found;
}

void DocumentCanvas::Draw(BRect updateRect)
{
    (void)updateRect;
    if (fEngine) {
        fEngine->RenderDocument(this, updateRect);
    } else {
        SetHighColor(255, 255, 255);
        FillRect(Bounds());
    }
}

void DocumentCanvas::MouseDown(BPoint where)
{
    if (!fEngine) return;
    MakeFocus();
    DocumentPosition position = fEngine->HitTestPosition(where);
    if (!position.IsValid()) return;

    uint32 modifiers = 0;
    if (Window() && Window()->CurrentMessage()) {
        int32 mods = 0;
        Window()->CurrentMessage()->FindInt32("modifiers", &mods);
        modifiers = (uint32)mods;
    }
    bool shift = (modifiers & B_SHIFT_KEY) != 0;
    MoveCaretTo(position, shift);
    if (!shift) {
        fDragging = true;
        fDragAnchor = position;
    } else {
        fDragging = true;
    }
}

void DocumentCanvas::MouseUp(BPoint where)
{
    (void)where;
    fDragging = false;
}

void DocumentCanvas::MouseMoved(BPoint where, uint32 transit, const BMessage* dragMessage)
{
    (void)transit;
    (void)dragMessage;
    if (!fDragging || !fEngine) return;
    DocumentPosition position = fEngine->HitTestPosition(where);
    if (position.IsValid()) {
        MoveCaretTo(position, true);
    }
}

void DocumentCanvas::HandleCommandKey(char key)
{
    switch (key) {
        case 'c':
        case 'C':
            CopyToClipboard();
            break;
        case 'x':
        case 'X':
            CutToClipboard();
            break;
        case 'v':
        case 'V':
            PasteFromClipboard();
            break;
        case 'z':
        case 'Z':
            Undo();
            break;
        case 'y':
        case 'Y':
            Redo();
            break;
        case 'a':
        case 'A':
            SelectAll();
            break;
        case 'b':
        case 'B':
            ToggleBold();
            break;
        case 'i':
        case 'I':
            ToggleItalic();
            break;
        case 'u':
        case 'U':
            ToggleUnderline();
            break;
        case '+':
        case '=':
            ZoomIn();
            break;
        case '-':
        case '_':
            ZoomOut();
            break;
        case '0':
            ZoomReset();
            break;
        default:
            break;
    }
}

void DocumentCanvas::InsertCharacter(const char* bytes, int32 numBytes)
{
    if (numBytes <= 0) return;
    BString text;
    text.SetTo(bytes, numBytes);
    InsertText(text);
}

void DocumentCanvas::KeyDown(const char* bytes, int32 numBytes)
{
    if (!fEngine || numBytes <= 0) return;

    uint32 modifiers = 0;
    if (Window() && Window()->CurrentMessage()) {
        int32 mods = 0;
        Window()->CurrentMessage()->FindInt32("modifiers", &mods);
        modifiers = (uint32)mods;
    }
    bool shift = (modifiers & B_SHIFT_KEY) != 0;

    if (modifiers & B_COMMAND_KEY) {
        HandleCommandKey(bytes[0]);
        return;
    }

    CaretManager* caret = fEngine->GetCaretManager();
    if (!caret) return;

    unsigned char key = (unsigned char)bytes[0];
    switch (key) {
        case B_BACKSPACE:
            DeleteSelectionOrBackspace();
            return;
        case B_ENTER:
            SplitParagraph();
            return;
        case B_LEFT_ARROW:
            caret->MoveLeft();
            MoveCaretTo(caret->GetPosition(), shift);
            EnsureCaretVisible();
            return;
        case B_RIGHT_ARROW:
            caret->MoveRight();
            MoveCaretTo(caret->GetPosition(), shift);
            EnsureCaretVisible();
            return;
        case B_HOME:
            caret->MoveToLineStart();
            MoveCaretTo(caret->GetPosition(), shift);
            EnsureCaretVisible();
            return;
        case B_END:
            caret->MoveToLineEnd();
            MoveCaretTo(caret->GetPosition(), shift);
            EnsureCaretVisible();
            return;
        case B_TAB:
            InsertCharacter("    ", 4);
            return;
        default:
            break;
    }

    // Ignore other control characters.
    if (key < 0x20 && key != B_ENTER && key != B_TAB) return;

    InsertCharacter(bytes, numBytes);
    EnsureCaretVisible();
}

void DocumentCanvas::EnsureCaretVisible()
{
    // Minimal: full repaint; a real implementation scrolls the BScrollView.
    Invalidate();
}

void DocumentCanvas::FrameResized(float newWidth, float newHeight)
{
    if (fEngine) {
        fEngine->SetViewportSize(BSize(newWidth, newHeight));
        fEngine->InvalidateLayout();
        fEngine->LayoutDocument();
    }
    Invalidate();
}

void DocumentCanvas::AttachedToWindow()
{
    SetViewColor(200, 200, 200);
    MakeFocus();
    Invalidate();
}

void DocumentCanvas::GetPreferredSize(float* _width, float* _height)
{
    if (fEngine) {
        BSize size = fEngine->GetDocumentSize();
        if (_width) *_width = size.width + 40.0f;
        if (_height) *_height = size.height + 40.0f;
        return;
    }
    if (_width) *_width = 400.0f;
    if (_height) *_height = 400.0f;
}
