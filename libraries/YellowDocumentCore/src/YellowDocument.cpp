#include "YellowDocumentCore/YellowDocument.h"

#include <algorithm>
#include <string.h>
#include <time.h>
#include <vector>

namespace YellowDocumentCore {

// DocumentChange implementation
DocumentChange::DocumentChange(Type type, const DocumentRange& range)
    : fType(type),
      fRange(range)
{
}

DocumentChange::~DocumentChange()
{
}

DocumentChange::Type DocumentChange::GetType() const
{
    return fType;
}

const DocumentRange& DocumentChange::GetRange() const
{
    return fRange;
}

void DocumentChange::SetRange(const DocumentRange& range)
{
    fRange = range;
}

// DocumentObserver implementation
DocumentObserver::~DocumentObserver()
{
}

// DocumentMetadata implementation
DocumentMetadata::DocumentMetadata()
    : DocumentNode(NODE_TYPE_METADATA),
      created(time(nullptr)),
      modified(time(nullptr)),
      lastSaved(time(nullptr)),
      pageCount(1),
      paragraphCount(0),
      wordCount(0),
      characterCount(0)
{
    generator = "YellowDocs 0.1.0";
}

DocumentMetadata::~DocumentMetadata()
{
}

NodeType DocumentMetadata::GetType() const
{
    return NODE_TYPE_METADATA;
}

DocumentNode* DocumentMetadata::Clone() const
{
    DocumentMetadata* clone = new DocumentMetadata();
    clone->title = title;
    clone->author = author;
    clone->subject = subject;
    clone->keywords = keywords;
    clone->description = description;
    clone->generator = generator;
    clone->created = created;
    clone->modified = modified;
    clone->lastSaved = lastSaved;
    clone->pageCount = pageCount;
    clone->paragraphCount = paragraphCount;
    clone->wordCount = wordCount;
    clone->characterCount = characterCount;
    return clone;
}

namespace {

void CountDocument(DocumentNode* node, int32& paragraphs, int32& words, int32& chars)
{
    if (!node) return;
    if (node->GetType() == NODE_TYPE_PARAGRAPH) {
        ++paragraphs;
        BString text = node->GetText();
        chars += text.Length();

        // Count words: sequences of non-whitespace characters.
        const char* s = text.String();
        bool inWord = false;
        for (int32 i = 0; i < text.Length(); ++i) {
            bool isSpace = (s[i] == ' ' || s[i] == '\t' || s[i] == '\n' || s[i] == '\r');
            if (!isSpace && !inWord) {
                inWord = true;
                ++words;
            } else if (isSpace) {
                inWord = false;
            }
        }
        // A paragraph's text does not include its line break; still count
        // paragraph mark as a character for parity with common editors.
        ++chars;
        return;
    }
    for (size_t i = 0; i < node->GetChildCount(); ++i) {
        CountDocument(node->GetChild(i), paragraphs, words, chars);
    }
}

} // namespace

void DocumentMetadata::UpdateStatistics(YellowDocument* document)
{
    if (!document) {
        return;
    }
    paragraphCount = 0;
    wordCount = 0;
    characterCount = 0;
    CountDocument(document->GetRoot(), paragraphCount, wordCount, characterCount);
    if (pageCount < 1) {
        pageCount = 1;
    }
}

// SectionNode implementation
SectionNode::SectionNode()
    : DocumentNode(NODE_TYPE_SECTION),
      isContinuous(true),
      pageWidth(8.5f * 72.0f), // US Letter in points
      pageHeight(11.0f * 72.0f),
      leftMargin(1.0f * 72.0f),
      rightMargin(1.0f * 72.0f),
      topMargin(1.0f * 72.0f),
      bottomMargin(1.0f * 72.0f),
      hasHeader(false),
      hasFooter(false)
{
}

SectionNode::~SectionNode()
{
}

NodeType SectionNode::GetType() const
{
    return NODE_TYPE_SECTION;
}

DocumentNode* SectionNode::Clone() const
{
    SectionNode* clone = new SectionNode();
    clone->name = name;
    clone->isContinuous = isContinuous;
    clone->pageWidth = pageWidth;
    clone->pageHeight = pageHeight;
    clone->leftMargin = leftMargin;
    clone->rightMargin = rightMargin;
    clone->topMargin = topMargin;
    clone->bottomMargin = bottomMargin;
    clone->hasHeader = hasHeader;
    clone->hasFooter = hasFooter;

    for (size_t i = 0; i < GetChildCount(); ++i) {
        DocumentNode* child = GetChild(i);
        if (child) {
            clone->AddChild(child->Clone());
        }
    }
    return clone;
}

// ParagraphNode implementation
ParagraphNode::ParagraphNode()
    : DocumentNode(NODE_TYPE_PARAGRAPH)
{
}

ParagraphNode::ParagraphNode(const BString& text)
    : DocumentNode(NODE_TYPE_PARAGRAPH)
{
    TextRunNode* textRun = new TextRunNode(text);
    AddChild(textRun);
}

ParagraphNode::~ParagraphNode()
{
}

NodeType ParagraphNode::GetType() const
{
    return NODE_TYPE_PARAGRAPH;
}

BString ParagraphNode::GetText() const
{
    BString result;
    for (size_t i = 0; i < GetChildCount(); ++i) {
        DocumentNode* child = GetChild(i);
        if (child) {
            result += child->GetText();
        }
    }
    return result;
}

void ParagraphNode::SetText(const BString& text)
{
    RemoveAllChildren();
    TextRunNode* textRun = new TextRunNode(text);
    AddChild(textRun);
}

int32 ParagraphNode::GetTextLength() const
{
    int32 length = 0;
    for (size_t i = 0; i < GetChildCount(); ++i) {
        DocumentNode* child = GetChild(i);
        if (child) {
            length += child->GetTextLength();
        }
    }
    return length;
}

DocumentPosition ParagraphNode::GetStartPosition() const
{
    for (size_t i = 0; i < GetChildCount(); ++i) {
        DocumentNode* child = GetChild(i);
        if (child && child->GetTextLength() > 0) {
            return child->GetStartPosition();
        }
    }
    return DocumentPosition(const_cast<ParagraphNode*>(this), 0);
}

DocumentPosition ParagraphNode::GetEndPosition() const
{
    for (size_t i = GetChildCount(); i-- > 0; ) {
        DocumentNode* child = GetChild(i);
        if (child && child->GetTextLength() > 0) {
            return child->GetEndPosition();
        }
    }
    return DocumentPosition(const_cast<ParagraphNode*>(this), 0);
}

DocumentPosition ParagraphNode::GetPositionAt(int32 offset) const
{
    if (offset <= 0) {
        return GetStartPosition();
    }
    for (size_t i = 0; i < GetChildCount(); ++i) {
        DocumentNode* child = GetChild(i);
        if (!child) continue;
        int32 childLength = child->GetTextLength();
        if (offset < childLength) {
            return child->GetPositionAt(offset);
        }
        if (offset == childLength && child->GetType() == NODE_TYPE_TEXT_RUN) {
            return child->GetPositionAt(offset);
        }
        offset -= childLength;
    }
    return GetEndPosition();
}

DocumentNode* ParagraphNode::Clone() const
{
    ParagraphNode* clone = new ParagraphNode();
    clone->style = style;
    clone->paragraphStyleName = paragraphStyleName;

    for (size_t i = 0; i < GetChildCount(); ++i) {
        DocumentNode* child = GetChild(i);
        if (child) {
            clone->AddChild(child->Clone());
        }
    }
    return clone;
}

void ParagraphNode::AppendText(const BString& text)
{
    if (text.IsEmpty()) return;
    if (GetChildCount() > 0) {
        DocumentNode* lastChild = GetChild(GetChildCount() - 1);
        if (lastChild && lastChild->GetType() == NODE_TYPE_TEXT_RUN) {
            TextRunNode* textRun = static_cast<TextRunNode*>(lastChild);
            textRun->text += text;
            return;
        }
    }
    TextRunNode* textRun = new TextRunNode(text);
    AddChild(textRun);
}

void ParagraphNode::InsertText(int32 offset, const BString& text)
{
    if (text.IsEmpty()) return;
    if (offset < 0) offset = 0;

    if (GetChildCount() == 0) {
        TextRunNode* textRun = new TextRunNode(text);
        AddChild(textRun);
        return;
    }

    for (size_t i = 0; i < GetChildCount(); ++i) {
        DocumentNode* child = GetChild(i);
        if (child && child->GetType() == NODE_TYPE_TEXT_RUN) {
            TextRunNode* textRun = static_cast<TextRunNode*>(child);
            int32 childLength = textRun->GetTextLength();
            if (offset <= childLength) {
                textRun->text.Insert(text, offset);
                return;
            }
            offset -= childLength;
        }
    }

    AppendText(text);
}

void ParagraphNode::DeleteText(int32 offset, int32 length)
{
    if (length <= 0 || offset < 0) return;

    for (size_t i = 0; i < GetChildCount() && length > 0; ++i) {
        DocumentNode* child = GetChild(i);
        if (child && child->GetType() == NODE_TYPE_TEXT_RUN) {
            TextRunNode* textRun = static_cast<TextRunNode*>(child);
            int32 childLength = textRun->GetTextLength();
            if (offset < childLength) {
                int32 deleteLength = std::min(length, childLength - offset);
                textRun->text.Remove(offset, deleteLength);
                length -= deleteLength;
                offset = 0;
            } else {
                offset -= childLength;
            }
        }
    }

    // Remove empty runs (but keep at least one run so the paragraph stays
    // caret-addressable).
    for (size_t i = GetChildCount(); i-- > 0; ) {
        DocumentNode* child = GetChild(i);
        if (child && child->GetType() == NODE_TYPE_TEXT_RUN &&
            child->GetTextLength() == 0 && GetChildCount() > 1) {
            RemoveChild(i);
            delete child;
        }
    }
}

BString ParagraphNode::GetText(int32 start, int32 length) const
{
    BString full = GetText();
    if (start < 0 || start > full.Length() || length <= 0) {
        return BString();
    }
    int32 available = full.Length() - start;
    if (length > available) {
        length = available;
    }
    return BString(full.String() + start, length);
}

void ParagraphNode::AddTextRun(DocumentNode* textRun)
{
    if (textRun && textRun->GetType() == NODE_TYPE_TEXT_RUN) {
        AddChild(textRun);
    }
}

void ParagraphNode::SetStyle(const ParagraphStyle& newStyle)
{
    style = newStyle;
}

const ParagraphStyle& ParagraphNode::GetStyle() const
{
    return style;
}

void ParagraphNode::SetStyleName(const BString& styleName)
{
    paragraphStyleName = styleName;
}

const BString& ParagraphNode::GetStyleName() const
{
    return paragraphStyleName;
}

void ParagraphNode::EnsureTextRun()
{
    if (GetChildCount() == 0) {
        AddChild(new TextRunNode(BString()));
    }
}

// TextRunNode implementation
TextRunNode::TextRunNode()
    : DocumentNode(NODE_TYPE_TEXT_RUN)
{
}

TextRunNode::TextRunNode(const BString& textIn, const TextStyle& styleIn)
    : DocumentNode(NODE_TYPE_TEXT_RUN),
      text(textIn),
      style(styleIn)
{
}

TextRunNode::~TextRunNode()
{
}

NodeType TextRunNode::GetType() const
{
    return NODE_TYPE_TEXT_RUN;
}

BString TextRunNode::GetText() const
{
    return text;
}

void TextRunNode::SetText(const BString& newText)
{
    text = newText;
}

int32 TextRunNode::GetTextLength() const
{
    return text.Length();
}

DocumentPosition TextRunNode::GetStartPosition() const
{
    return DocumentPosition(const_cast<TextRunNode*>(this), 0);
}

DocumentPosition TextRunNode::GetEndPosition() const
{
    return DocumentPosition(const_cast<TextRunNode*>(this), text.Length());
}

DocumentPosition TextRunNode::GetPositionAt(int32 offset) const
{
    if (offset < 0) offset = 0;
    if (offset > text.Length()) offset = text.Length();
    return DocumentPosition(const_cast<TextRunNode*>(this), offset);
}

DocumentNode* TextRunNode::Clone() const
{
    TextRunNode* clone = new TextRunNode(text, style);
    clone->characterStyleName = characterStyleName;
    return clone;
}

void TextRunNode::SetStyle(const TextStyle& newStyle)
{
    style = newStyle;
}

const TextStyle& TextRunNode::GetStyle() const
{
    return style;
}

void TextRunNode::SetStyleName(const BString& styleName)
{
    characterStyleName = styleName;
}

const BString& TextRunNode::GetStyleName() const
{
    return characterStyleName;
}

// YellowDocument implementation
YellowDocument::YellowDocument()
    : fRoot(nullptr),
      fMetadata(new DocumentMetadata()),
      fStyleManager(new StyleManager()),
      fCommandHistory(nullptr),
      fModified(false),
      fFormatVersion(1)
{
    SectionNode* section = new SectionNode();
    section->name = "Default Section";
    section->SetDocument(this);
    fRoot = section;

    // A new document always has one empty paragraph so the caret has
    // somewhere to live.
    ParagraphNode* paragraph = new ParagraphNode();
    paragraph->EnsureTextRun();
    section->AddChild(paragraph);
}

YellowDocument::~YellowDocument()
{
    RemoveAllObservers();
    delete fRoot;
    fRoot = nullptr;
    delete fMetadata;
    fMetadata = nullptr;
    delete fStyleManager;
    fStyleManager = nullptr;
    delete fCommandHistory;
    fCommandHistory = nullptr;
}

void YellowDocument::Clear()
{
    RemoveAllObservers();

    if (fCommandHistory) {
        fCommandHistory->Clear();
    }

    delete fRoot;
    fRoot = nullptr;

    SectionNode* section = new SectionNode();
    section->name = "Default Section";
    section->SetDocument(this);
    fRoot = section;

    ParagraphNode* paragraph = new ParagraphNode();
    paragraph->EnsureTextRun();
    section->AddChild(paragraph);

    if (fMetadata) {
        *fMetadata = DocumentMetadata();
    }

    fModified = false;
}

bool YellowDocument::IsEmpty() const
{
    if (!fRoot) return true;
    // Empty means: a single empty paragraph and nothing else.
    if (fRoot->GetChildCount() != 1) return false;
    DocumentNode* only = fRoot->GetChild(0);
    if (!only || only->GetType() != NODE_TYPE_PARAGRAPH) return false;
    return only->GetTextLength() == 0;
}

DocumentNode* YellowDocument::GetRoot() const
{
    return fRoot;
}

void YellowDocument::SetRoot(DocumentNode* root)
{
    if (fRoot != root) {
        delete fRoot;
        fRoot = root;
        if (fRoot) {
            fRoot->SetParent(nullptr);
            fRoot->SetDocument(this);
        }
    }
}

SectionNode* YellowDocument::GetRootSection() const
{
    if (fRoot && fRoot->GetType() == NODE_TYPE_SECTION) {
        return static_cast<SectionNode*>(fRoot);
    }
    return nullptr;
}

DocumentMetadata* YellowDocument::GetMetadata()
{
    return fMetadata;
}

const DocumentMetadata* YellowDocument::GetMetadata() const
{
    return fMetadata;
}

void YellowDocument::SetMetadata(DocumentMetadata* metadata)
{
    if (fMetadata != metadata) {
        delete fMetadata;
        fMetadata = metadata;
    }
}

StyleManager* YellowDocument::GetStyleManager()
{
    return fStyleManager;
}

const StyleManager* YellowDocument::GetStyleManager() const
{
    return fStyleManager;
}

void YellowDocument::SetStyleManager(StyleManager* styleManager)
{
    if (fStyleManager != styleManager) {
        delete fStyleManager;
        fStyleManager = styleManager;
    }
}

void YellowDocument::AddObserver(DocumentObserver* observer)
{
    if (!observer) return;
    for (DocumentObserver* existing : fObservers) {
        if (existing == observer) return;
    }
    fObservers.push_back(observer);
}

void YellowDocument::RemoveObserver(DocumentObserver* observer)
{
    for (auto it = fObservers.begin(); it != fObservers.end(); ++it) {
        if (*it == observer) {
            fObservers.erase(it);
            return;
        }
    }
}

void YellowDocument::RemoveAllObservers()
{
    fObservers.clear();
}

void YellowDocument::NotifyDocumentChanged(const DocumentChange& change)
{
    for (DocumentObserver* observer : fObservers) {
        observer->DocumentChanged(change);
    }
}

void YellowDocument::NotifyContentChanged(const DocumentRange& range)
{
    if (fMetadata) {
        fMetadata->modified = time(nullptr);
    }
    for (DocumentObserver* observer : fObservers) {
        observer->DocumentContentChanged(range);
    }
}

void YellowDocument::NotifyStructureChanged()
{
    if (fMetadata) {
        fMetadata->modified = time(nullptr);
    }
    for (DocumentObserver* observer : fObservers) {
        observer->DocumentStructureChanged();
    }
}

void YellowDocument::NotifyStylesChanged()
{
    for (DocumentObserver* observer : fObservers) {
        observer->DocumentStylesChanged();
    }
}

CommandHistory* YellowDocument::GetCommandHistory()
{
    if (!fCommandHistory) {
        fCommandHistory = new CommandHistory(this);
    }
    return fCommandHistory;
}

const CommandHistory* YellowDocument::GetCommandHistory() const
{
    return fCommandHistory;
}

bool YellowDocument::ExecuteCommand(Command* command)
{
    return GetCommandHistory()->Execute(command);
}

bool YellowDocument::Undo()
{
    CommandHistory* history = GetCommandHistory();
    if (!history->CanUndo()) return false;
    return history->Undo();
}

bool YellowDocument::Redo()
{
    CommandHistory* history = GetCommandHistory();
    if (!history->CanRedo()) return false;
    return history->Redo();
}

bool YellowDocument::CanUndo() const
{
    return fCommandHistory && fCommandHistory->CanUndo();
}

bool YellowDocument::CanRedo() const
{
    return fCommandHistory && fCommandHistory->CanRedo();
}

BString YellowDocument::GetTitle() const
{
    if (fMetadata) {
        return fMetadata->title;
    }
    return BString();
}

void YellowDocument::SetTitle(const BString& title)
{
    if (fMetadata) {
        fMetadata->title = title;
    }
}

BString YellowDocument::GetAuthor() const
{
    if (fMetadata) {
        return fMetadata->author;
    }
    return BString();
}

void YellowDocument::SetAuthor(const BString& author)
{
    if (fMetadata) {
        fMetadata->author = author;
    }
}

void YellowDocument::UpdateStatistics()
{
    if (fMetadata) {
        fMetadata->UpdateStatistics(this);
    }
}

int32 YellowDocument::GetPageCount() const
{
    if (fMetadata) {
        return fMetadata->pageCount;
    }
    return 0;
}

void YellowDocument::SetPageCount(int32 count)
{
    if (fMetadata) {
        fMetadata->pageCount = count < 1 ? 1 : count;
    }
}

int32 YellowDocument::GetParagraphCount() const
{
    if (fMetadata) {
        return fMetadata->paragraphCount;
    }
    return 0;
}

int32 YellowDocument::GetWordCount() const
{
    if (fMetadata) {
        return fMetadata->wordCount;
    }
    return 0;
}

int32 YellowDocument::GetCharacterCount() const
{
    if (fMetadata) {
        return fMetadata->characterCount;
    }
    return 0;
}

bool YellowDocument::IsModified() const
{
    return fModified;
}

void YellowDocument::SetModified(bool modified)
{
    fModified = modified;
    if (modified && fMetadata) {
        fMetadata->modified = time(nullptr);
    }
}

void YellowDocument::MarkSaved()
{
    fModified = false;
    if (fMetadata) {
        fMetadata->lastSaved = time(nullptr);
    }
}

int32 YellowDocument::GetFormatVersion() const
{
    return fFormatVersion;
}

void YellowDocument::SetFormatVersion(int32 version)
{
    fFormatVersion = version;
}

} // namespace YellowDocumentCore
