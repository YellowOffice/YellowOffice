#ifndef YELLOW_DOCUMENT_CORE_COMMAND_H
#define YELLOW_DOCUMENT_CORE_COMMAND_H

#include <SupportDefs.h>
#include <String.h>
#include <vector>

#include <YellowDocumentCore/DocumentNode.h>
#include <YellowDocumentCore/Style.h>

namespace YellowDocumentCore {

class YellowDocument;
class ParagraphNode;
class TextRunNode;

/**
 * Command - base class for undoable operations on a document.
 */
class Command {
public:
    virtual ~Command();

    /// Executes (or re-executes after undo). Returns false on failure.
    virtual bool Execute() = 0;
    /// Reverts a previous Execute(). Returns false on failure.
    virtual bool Undo() = 0;
    virtual BString GetName() const = 0;

    /// Commands may merge with the previous command for coalescing
    /// (e.g. consecutive typed characters). Takes ownership of *this when
    /// returning true.
    virtual bool MergeWith(Command* /*next*/) { return false; }

protected:
    explicit Command(YellowDocument* document);

    YellowDocument* fDocument;
};

/**
 * CommandHistory - undo/redo stack owned by the document.
 */
class CommandHistory {
public:
    explicit CommandHistory(YellowDocument* document);
    ~CommandHistory();

    /// Executes the command, pushing it onto the undo stack. Takes
    /// ownership of the command. Clears the redo stack. Merges with the
    /// top undo command when MergeWith() succeeds.
    bool Execute(Command* command);

    bool CanUndo() const;
    bool CanRedo() const;
    bool Undo();
    bool Redo();

    BString GetUndoName() const;
    BString GetRedoName() const;

    void Clear();
    size_t GetUndoDepth() const;
    size_t GetRedoDepth() const;

private:
    YellowDocument* fDocument;
    std::vector<Command*> fUndoStack;
    std::vector<Command*> fRedoStack;
};

// ------------------------------------------------------------------ commands

/**
 * InsertTextCommand - inserts text into a paragraph at a text offset.
 */
class InsertTextCommand : public Command {
public:
    InsertTextCommand(YellowDocument* document, ParagraphNode* paragraph,
                      int32 offset, const BString& text);

    bool Execute() override;
    bool Undo() override;
    BString GetName() const override;
    bool MergeWith(Command* next) override;

private:
    ParagraphNode* fParagraph;
    int32 fOffset;
    BString fText;
};

/**
 * DeleteTextCommand - deletes a text range from a paragraph.
 */
class DeleteTextCommand : public Command {
public:
    DeleteTextCommand(YellowDocument* document, ParagraphNode* paragraph,
                      int32 offset, int32 length);

    bool Execute() override;
    bool Undo() override;
    BString GetName() const override;

private:
    ParagraphNode* fParagraph;
    int32 fOffset;
    int32 fLength;
    BString fDeletedText;           // full deleted text (for undo)
    std::vector<TextStyle> fDeletedStyles; // per-character styles of deleted text
};

/**
 * SplitParagraphCommand - splits a paragraph at a text offset.
 */
class SplitParagraphCommand : public Command {
public:
    SplitParagraphCommand(YellowDocument* document, ParagraphNode* paragraph,
                          int32 offset);

    bool Execute() override;
    bool Undo() override;
    BString GetName() const override;

private:
    ParagraphNode* fParagraph;
    ParagraphNode* fSecondParagraph; // created by Execute, owned by document after Execute
    int32 fOffset;
    bool fExecuted;
};

/**
 * ApplyTextStyleCommand - applies a character style delta to a range.
 */
class ApplyTextStyleCommand : public Command {
public:
    ApplyTextStyleCommand(YellowDocument* document, const DocumentRange& range,
                          const TextStyle& style, bool bold, bool italic,
                          bool underline, bool strikethrough);

    bool Execute() override;
    bool Undo() override;
    BString GetName() const override;

private:
    void Apply(bool toNewStyle);

    DocumentRange fRange;
    TextStyle fNewStyle;
    bool fBold, fItalic, fUnderline, fStrikethrough;

    struct RunState {
        TextRunNode* run;
        TextStyle oldStyle;
    };
    std::vector<RunState> fOldStates;
};

/**
 * ApplyParagraphStyleCommand - applies paragraph formatting to the
 * paragraph containing the range.
 */
class ApplyParagraphStyleCommand : public Command {
public:
    ApplyParagraphStyleCommand(YellowDocument* document, const DocumentRange& range,
                               const ParagraphStyle& style);

    bool Execute() override;
    bool Undo() override;
    BString GetName() const override;

private:
    DocumentRange fRange;
    ParagraphStyle fNewStyle;
    struct ParaState {
        ParagraphNode* paragraph;
        ParagraphStyle oldStyle;
    };
    std::vector<ParaState> fOldStates;
};

/**
 * SetTextStyleOnRunsCommand - replaces style wholesale on all runs in range
 * (used by font family/size pickers).
 */
class SetTextStyleOnRunsCommand : public Command {
public:
    SetTextStyleOnRunsCommand(YellowDocument* document, const DocumentRange& range,
                              const TextStyle& newStyle);

    bool Execute() override;
    bool Undo() override;
    BString GetName() const override;

private:
    DocumentRange fRange;
    TextStyle fNewStyle;
    struct RunState {
        TextRunNode* run;
        TextStyle oldStyle;
    };
    std::vector<RunState> fOldStates;
};

} // namespace YellowDocumentCore

#endif // YELLOW_DOCUMENT_CORE_COMMAND_H
