#include "YellowDocumentCore/Command.h"
#include "YellowDocumentCore/YellowDocument.h"

#include <algorithm>

namespace YellowDocumentCore {

// -------------------------------------------------------------- helpers

namespace {

struct RunSpan {
    TextRunNode* run;
    int32 start; // offset within paragraph
    int32 length;
};

void CollectRunSpans(ParagraphNode* paragraph, std::vector<RunSpan>& out)
{
    if (!paragraph) return;
    int32 position = 0;
    for (size_t i = 0; i < paragraph->GetChildCount(); ++i) {
        DocumentNode* child = paragraph->GetChild(i);
        if (!child || child->GetType() != NODE_TYPE_TEXT_RUN) continue;
        TextRunNode* run = static_cast<TextRunNode*>(child);
        RunSpan span;
        span.run = run;
        span.start = position;
        span.length = run->GetTextLength();
        out.push_back(span);
        position += span.length;
    }
}

// Splits the run that contains paragraph offset (splitOffset) so that a run
// boundary exists at exactly that offset. No-op at the very start/end.
void SplitRunAt(ParagraphNode* paragraph, int32 splitOffset)
{
    if (!paragraph || splitOffset <= 0) return;
    std::vector<RunSpan> spans;
    CollectRunSpans(paragraph, spans);
    for (const RunSpan& span : spans) {
        if (splitOffset > span.start && splitOffset < span.start + span.length) {
            int32 local = splitOffset - span.start;
            TextRunNode* right = new TextRunNode(
                BString(span.run->text.String() + local, span.length - local),
                span.run->style);
            right->characterStyleName = span.run->characterStyleName;
            size_t index = paragraph->GetIndexOf(span.run);
            paragraph->InsertChild(index + 1, right);
            span.run->text.Truncate(local);
            return;
        }
    }
}

void ApplyStyleToParagraphRange(ParagraphNode* paragraph, int32 offset,
                                int32 length, const TextStyle& style)
{
    if (!paragraph || length <= 0) return;
    SplitRunAt(paragraph, offset);
    SplitRunAt(paragraph, offset + length);

    std::vector<RunSpan> spans;
    CollectRunSpans(paragraph, spans);
    for (const RunSpan& span : spans) {
        if (span.start >= offset && span.start + span.length <= offset + length) {
            span.run->style = style;
        }
    }
}

BString ParagraphSlice(ParagraphNode* paragraph, int32 offset, int32 length)
{
    if (!paragraph) return BString();
    BString full = paragraph->GetText();
    if (offset < 0 || offset > full.Length()) return BString();
    int32 available = full.Length() - offset;
    if (length < 0 || length > available) length = available;
    return BString(full.String() + offset, length);
}

void CaptureStylesInParagraph(ParagraphNode* paragraph, int32 offset, int32 length,
                              std::vector<TextStyle>& outStyles)
{
    outStyles.clear();
    if (!paragraph || length <= 0) return;
    std::vector<RunSpan> spans;
    CollectRunSpans(paragraph, spans);
    for (const RunSpan& span : spans) {
        int32 overlapStart = std::max(offset, span.start);
        int32 overlapEnd = std::min(offset + length, span.start + span.length);
        for (int32 i = overlapStart; i < overlapEnd; ++i) {
            outStyles.push_back(span.run->style);
        }
    }
}

// Applies styles captured per character back onto a freshly inserted range.
void RestoreStylesInParagraph(ParagraphNode* paragraph, int32 offset,
                              const std::vector<TextStyle>& styles)
{
    if (!paragraph || styles.empty()) return;
    // Group consecutive identical styles and apply per group.
    size_t i = 0;
    while (i < styles.size()) {
        size_t j = i;
        while (j + 1 < styles.size() && styles[j + 1] == styles[i]) {
            ++j;
        }
        ApplyStyleToParagraphRange(paragraph, offset + (int32)i,
                                   (int32)(j - i + 1), styles[i]);
        i = j + 1;
    }
}

std::vector<TextRunNode*> CollectRunsInRange(YellowDocument* document,
                                             const DocumentRange& rangeIn)
{
    std::vector<TextRunNode*> runs;
    if (!document || !document->GetRoot()) return runs;
    DocumentRange range = rangeIn;
    range.Normalize();
    if (!range.IsValid()) return runs;

    std::vector<DocumentNode*> stack;
    stack.push_back(document->GetRoot());
    bool collecting = false;
    while (!stack.empty()) {
        DocumentNode* node = stack.back();
        stack.pop_back();
        if (node->GetType() == NODE_TYPE_TEXT_RUN) {
            TextRunNode* run = static_cast<TextRunNode*>(node);
            DocumentPosition runStart = run->GetStartPosition();
            DocumentPosition runEnd = run->GetEndPosition();
            if (runEnd <= range.GetStart() || runStart >= range.GetEnd()) {
                // no overlap
            } else {
                runs.push_back(run);
            }
            continue;
        }
        for (size_t i = node->GetChildCount(); i-- > 0; ) {
            stack.push_back(node->GetChild(i));
        }
    }
    (void)collecting;
    return runs;
}

} // namespace

// ------------------------------------------------------------------ Command

Command::Command(YellowDocument* document)
    : fDocument(document)
{
}

Command::~Command()
{
}

// ------------------------------------------------------------ CommandHistory

CommandHistory::CommandHistory(YellowDocument* document)
    : fDocument(document)
{
}

CommandHistory::~CommandHistory()
{
    Clear();
}

bool CommandHistory::Execute(Command* command)
{
    if (!command) return false;
    if (!command->Execute()) {
        delete command;
        return false;
    }

    // Try to merge with the previous command for coalescing.
    if (!fUndoStack.empty() && fUndoStack.back()->MergeWith(command)) {
        // MergeWith took ownership.
    } else {
        fUndoStack.push_back(command);
    }

    for (Command* redo : fRedoStack) {
        delete redo;
    }
    fRedoStack.clear();

    if (fDocument) {
        fDocument->SetModified(true);
    }
    return true;
}

bool CommandHistory::CanUndo() const
{
    return !fUndoStack.empty();
}

bool CommandHistory::CanRedo() const
{
    return !fRedoStack.empty();
}

bool CommandHistory::Undo()
{
    if (fUndoStack.empty()) return false;
    Command* command = fUndoStack.back();
    if (!command->Undo()) return false;
    fUndoStack.pop_back();
    fRedoStack.push_back(command);
    if (fDocument) {
        fDocument->SetModified(true);
    }
    return true;
}

bool CommandHistory::Redo()
{
    if (fRedoStack.empty()) return false;
    Command* command = fRedoStack.back();
    if (!command->Execute()) return false;
    fRedoStack.pop_back();
    fUndoStack.push_back(command);
    if (fDocument) {
        fDocument->SetModified(true);
    }
    return true;
}

BString CommandHistory::GetUndoName() const
{
    if (fUndoStack.empty()) return BString();
    return fUndoStack.back()->GetName();
}

BString CommandHistory::GetRedoName() const
{
    if (fRedoStack.empty()) return BString();
    return fRedoStack.back()->GetName();
}

void CommandHistory::Clear()
{
    for (Command* command : fUndoStack) {
        delete command;
    }
    fUndoStack.clear();
    for (Command* command : fRedoStack) {
        delete command;
    }
    fRedoStack.clear();
}

size_t CommandHistory::GetUndoDepth() const
{
    return fUndoStack.size();
}

size_t CommandHistory::GetRedoDepth() const
{
    return fRedoStack.size();
}

// -------------------------------------------------------- InsertTextCommand

InsertTextCommand::InsertTextCommand(YellowDocument* document, ParagraphNode* paragraph,
                                     int32 offset, const BString& text)
    : Command(document),
      fParagraph(paragraph),
      fOffset(offset),
      fText(text)
{
}

bool InsertTextCommand::Execute()
{
    if (!fParagraph || fText.IsEmpty()) return false;
    fParagraph->InsertText(fOffset, fText);
    if (fDocument) {
        fDocument->NotifyContentChanged(DocumentRange(
            fParagraph->GetPositionAt(fOffset),
            fParagraph->GetPositionAt(fOffset + fText.Length())));
        fDocument->SetModified(true);
    }
    return true;
}

bool InsertTextCommand::Undo()
{
    if (!fParagraph) return false;
    fParagraph->DeleteText(fOffset, fText.Length());
    if (fDocument) {
        fDocument->NotifyContentChanged(DocumentRange(
            fParagraph->GetPositionAt(fOffset),
            fParagraph->GetPositionAt(fOffset)));
        fDocument->SetModified(true);
    }
    return true;
}

BString InsertTextCommand::GetName() const
{
    return "Typing";
}

bool InsertTextCommand::MergeWith(Command* next)
{
    InsertTextCommand* other = dynamic_cast<InsertTextCommand*>(next);
    if (!other || other->fParagraph != fParagraph) return false;
    // Only merge if contiguous.
    if (other->fOffset != fOffset + fText.Length()) return false;
    // Don't merge across whitespace boundaries to keep word-level undo.
    if (fText.Length() > 0 && fText[fText.Length() - 1] == ' ') return false;
    if (other->fText.Length() > 0 && other->fText[0] == ' ') return false;
    if (fText.Length() + other->fText.Length() > 64) return false;

    fText += other->fText;
    delete other;
    return true;
}

// ------------------------------------------------------- DeleteTextCommand

DeleteTextCommand::DeleteTextCommand(YellowDocument* document, ParagraphNode* paragraph,
                                     int32 offset, int32 length)
    : Command(document),
      fParagraph(paragraph),
      fOffset(offset),
      fLength(length)
{
}

bool DeleteTextCommand::Execute()
{
    if (!fParagraph || fLength <= 0) return false;
    fDeletedText = ParagraphSlice(fParagraph, fOffset, fLength);
    if (fDeletedText.IsEmpty()) return false;
    CaptureStylesInParagraph(fParagraph, fOffset, fLength, fDeletedStyles);
    fLength = fDeletedText.Length();

    fParagraph->DeleteText(fOffset, fLength);
    if (fDocument) {
        fDocument->NotifyContentChanged(DocumentRange(
            fParagraph->GetPositionAt(fOffset),
            fParagraph->GetPositionAt(fOffset)));
        fDocument->SetModified(true);
    }
    return true;
}

bool DeleteTextCommand::Undo()
{
    if (!fParagraph || fDeletedText.IsEmpty()) return false;
    fParagraph->InsertText(fOffset, fDeletedText);
    RestoreStylesInParagraph(fParagraph, fOffset, fDeletedStyles);
    if (fDocument) {
        fDocument->NotifyContentChanged(DocumentRange(
            fParagraph->GetPositionAt(fOffset),
            fParagraph->GetPositionAt(fOffset + fDeletedText.Length())));
        fDocument->SetModified(true);
    }
    return true;
}

BString DeleteTextCommand::GetName() const
{
    return "Delete";
}

// --------------------------------------------------- SplitParagraphCommand

SplitParagraphCommand::SplitParagraphCommand(YellowDocument* document,
                                             ParagraphNode* paragraph, int32 offset)
    : Command(document),
      fParagraph(paragraph),
      fSecondParagraph(nullptr),
      fOffset(offset),
      fExecuted(false)
{
}

bool SplitParagraphCommand::Execute()
{
    if (!fParagraph || fExecuted) return false;
    DocumentNode* parent = fParagraph->GetParent();
    if (!parent) return false;

    ParagraphNode* second = new ParagraphNode();
    second->style = fParagraph->style;
    second->paragraphStyleName = fParagraph->paragraphStyleName;

    // Move trailing runs (and trailing part of the split run) into second.
    std::vector<RunSpan> spans;
    CollectRunSpans(fParagraph, spans);
    SplitRunAt(fParagraph, fOffset);

    // Collect runs that start at/after fOffset and move them.
    std::vector<TextRunNode*> toMove;
    int32 position = 0;
    for (size_t i = 0; i < fParagraph->GetChildCount(); ++i) {
        DocumentNode* child = fParagraph->GetChild(i);
        if (!child || child->GetType() != NODE_TYPE_TEXT_RUN) continue;
        TextRunNode* run = static_cast<TextRunNode*>(child);
        int32 runLength = run->GetTextLength();
        if (position >= fOffset) {
            toMove.push_back(run);
        }
        position += runLength;
    }
    for (TextRunNode* run : toMove) {
        fParagraph->RemoveChild(run);
        second->AddChild(run);
    }

    size_t index = parent->GetIndexOf(fParagraph);
    parent->InsertChild(index + 1, second);
    fSecondParagraph = second;
    fExecuted = true;

    if (fDocument) {
        fDocument->NotifyStructureChanged();
        fDocument->SetModified(true);
    }
    return true;
}

bool SplitParagraphCommand::Undo()
{
    if (!fParagraph || !fSecondParagraph || !fExecuted) return false;
    DocumentNode* parent = fSecondParagraph->GetParent();
    if (!parent) return false;

    // Append all runs from second back onto first, then remove second.
    std::vector<DocumentNode*> moves;
    for (size_t i = 0; i < fSecondParagraph->GetChildCount(); ++i) {
        moves.push_back(fSecondParagraph->GetChild(i));
    }
    fSecondParagraph->RemoveAllChildren();
    for (DocumentNode* node : moves) {
        fParagraph->AddChild(node);
    }
    parent->RemoveChild(fSecondParagraph);
    delete fSecondParagraph;
    fSecondParagraph = nullptr;
    fExecuted = false;

    if (fDocument) {
        fDocument->NotifyStructureChanged();
        fDocument->SetModified(true);
    }
    return true;
}

BString SplitParagraphCommand::GetName() const
{
    return "Split Paragraph";
}

// ------------------------------------------------- ApplyTextStyleCommand

ApplyTextStyleCommand::ApplyTextStyleCommand(YellowDocument* document,
                                             const DocumentRange& range,
                                             const TextStyle& style,
                                             bool bold, bool italic,
                                             bool underline, bool strikethrough)
    : Command(document),
      fRange(range),
      fNewStyle(style),
      fBold(bold), fItalic(italic), fUnderline(underline), fStrikethrough(strikethrough)
{
    fRange.Normalize();
}

void ApplyTextStyleCommand::Apply(bool toNewStyle)
{
    std::vector<TextRunNode*> runs = CollectRunsInRange(fDocument, fRange);
    for (TextRunNode* run : runs) {
        if (toNewStyle) {
            if (fBold) run->style.bold = fNewStyle.bold;
            if (fItalic) run->style.italic = fNewStyle.italic;
            if (fUnderline) run->style.underline = fNewStyle.underline;
            if (fStrikethrough) run->style.strikethrough = fNewStyle.strikethrough;
        } else {
            for (const RunState& state : fOldStates) {
                if (state.run == run) {
                    run->style = state.oldStyle;
                    break;
                }
            }
        }
    }
}

bool ApplyTextStyleCommand::Execute()
{
    std::vector<TextRunNode*> runs = CollectRunsInRange(fDocument, fRange);
    fOldStates.clear();
    for (TextRunNode* run : runs) {
        RunState state;
        state.run = run;
        state.oldStyle = run->style;
        fOldStates.push_back(state);
    }
    Apply(true);
    if (fDocument) {
        fDocument->NotifyContentChanged(fRange);
        fDocument->SetModified(true);
    }
    return true;
}

bool ApplyTextStyleCommand::Undo()
{
    Apply(false);
    if (fDocument) {
        fDocument->NotifyContentChanged(fRange);
        fDocument->SetModified(true);
    }
    return true;
}

BString ApplyTextStyleCommand::GetName() const
{
    return "Format Text";
}

// ------------------------------------------- ApplyParagraphStyleCommand

ApplyParagraphStyleCommand::ApplyParagraphStyleCommand(YellowDocument* document,
                                                       const DocumentRange& range,
                                                       const ParagraphStyle& style)
    : Command(document),
      fRange(range),
      fNewStyle(style)
{
    fRange.Normalize();
}

bool ApplyParagraphStyleCommand::Execute()
{
    if (!fDocument || !fDocument->GetRoot()) return false;
    fOldStates.clear();

    // Collect all paragraphs whose text intersects the range.
    std::vector<DocumentNode*> stack;
    stack.push_back(fDocument->GetRoot());
    while (!stack.empty()) {
        DocumentNode* node = stack.back();
        stack.pop_back();
        if (node->GetType() == NODE_TYPE_PARAGRAPH) {
            ParagraphNode* paragraph = static_cast<ParagraphNode*>(node);
            DocumentPosition start = paragraph->GetStartPosition();
            DocumentPosition end = paragraph->GetEndPosition();
            if (end >= fRange.GetStart() && start <= fRange.GetEnd()) {
                ParaState state;
                state.paragraph = paragraph;
                state.oldStyle = paragraph->style;
                fOldStates.push_back(state);
                paragraph->style = fNewStyle;
            }
            continue;
        }
        for (size_t i = node->GetChildCount(); i-- > 0; ) {
            stack.push_back(node->GetChild(i));
        }
    }

    if (fDocument) {
        fDocument->NotifyContentChanged(fRange);
        fDocument->SetModified(true);
    }
    return !fOldStates.empty();
}

bool ApplyParagraphStyleCommand::Undo()
{
    for (const ParaState& state : fOldStates) {
        state.paragraph->style = state.oldStyle;
    }
    if (fDocument) {
        fDocument->NotifyContentChanged(fRange);
        fDocument->SetModified(true);
    }
    return true;
}

BString ApplyParagraphStyleCommand::GetName() const
{
    return "Format Paragraph";
}

// ------------------------------------------- SetTextStyleOnRunsCommand

SetTextStyleOnRunsCommand::SetTextStyleOnRunsCommand(YellowDocument* document,
                                                     const DocumentRange& range,
                                                     const TextStyle& newStyle)
    : Command(document),
      fRange(range),
      fNewStyle(newStyle)
{
    fRange.Normalize();
}

bool SetTextStyleOnRunsCommand::Execute()
{
    std::vector<TextRunNode*> runs = CollectRunsInRange(fDocument, fRange);
    fOldStates.clear();
    for (TextRunNode* run : runs) {
        RunState state;
        state.run = run;
        state.oldStyle = run->style;
        fOldStates.push_back(state);
        run->style = fNewStyle;
    }
    if (fDocument) {
        fDocument->NotifyContentChanged(fRange);
        fDocument->SetModified(true);
    }
    return true;
}

bool SetTextStyleOnRunsCommand::Undo()
{
    for (const RunState& state : fOldStates) {
        state.run->style = state.oldStyle;
    }
    if (fDocument) {
        fDocument->NotifyContentChanged(fRange);
        fDocument->SetModified(true);
    }
    return true;
}

BString SetTextStyleOnRunsCommand::GetName() const
{
    return "Change Font";
}

} // namespace YellowDocumentCore
