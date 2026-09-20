#include "YellowDocumentCore/YellowDocument.h"

#include <time.h>

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
      pageCount(0),
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
    return clone;
}

void DocumentMetadata::UpdateStatistics(YellowDocument* document)
{
    if (!document) {
        return;
    }
    
    // TODO: Implement statistics calculation
    pageCount = 0;
    paragraphCount = 0;
    wordCount = 0;
    characterCount = 0;
}

// SectionNode implementation
SectionNode::SectionNode()
    : DocumentNode(NODE_TYPE_SECTION),
      isContinuous(true),
      pageWidth(8.5f * 72.0f), // 8.5 inches in points
      pageHeight(11.0f * 72.0f), // 11 inches in points
      leftMargin(1.0f * 72.0f), // 1 inch in points
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

YellowDocument* SectionNode::GetDocument()
{
    return nullptr; // Will be implemented when we have document access
}

const YellowDocument* SectionNode::GetDocument() const
{
    return nullptr; // Will be implemented when we have document access
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
    
    // Clone children
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
    if (!text.IsEmpty()) {
        // Create a text run with the text
        TextRunNode* textRun = new TextRunNode(text);
        AddChild(textRun);
    }
}

ParagraphNode::~ParagraphNode()
{
}

NodeType ParagraphNode::GetType() const
{
    return NODE_TYPE_PARAGRAPH;
}

YellowDocument* ParagraphNode::GetDocument()
{
    return nullptr; // Will be implemented when we have document access
}

const YellowDocument* ParagraphNode::GetDocument() const
{
    return nullptr; // Will be implemented when we have document access
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
    // Remove all children and create a new text run
    RemoveAllChildren();
    if (!text.IsEmpty()) {
        TextRunNode* textRun = new TextRunNode(text);
        AddChild(textRun);
    }
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
    return DocumentPosition(const_cast<ParagraphNode*>(this), 0);
}

DocumentPosition ParagraphNode::GetEndPosition() const
{
    return DocumentPosition(const_cast<ParagraphNode*>(this), GetTextLength());
}

DocumentPosition ParagraphNode::GetPositionAt(int32 offset) const
{
    // Find which child contains the offset
    for (size_t i = 0; i < GetChildCount(); ++i) {
        DocumentNode* child = GetChild(i);
        if (child) {
            int32 childLength = child->GetTextLength();
            if (offset < childLength) {
                return child->GetPositionAt(offset);
            }
            offset -= childLength;
        }
    }
    return GetEndPosition();
}

DocumentNode* ParagraphNode::Clone() const
{
    ParagraphNode* clone = new ParagraphNode();
    clone->style = style;
    clone->paragraphStyleName = paragraphStyleName;
    
    // Clone children
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
    if (!text.IsEmpty()) {
        // If we have text runs, append to the last one
        if (GetChildCount() > 0) {
            DocumentNode* lastChild = GetChild(GetChildCount() - 1);
            if (lastChild && lastChild->GetType() == NODE_TYPE_TEXT_RUN) {
                TextRunNode* textRun = static_cast<TextRunNode*>(lastChild);
                textRun->text += text;
                return;
            }
        }
        // Otherwise create a new text run
        TextRunNode* textRun = new TextRunNode(text);
        AddChild(textRun);
    }
}

void ParagraphNode::InsertText(int32 offset, const BString& text)
{
    if (text.IsEmpty()) {
        return;
    }
    
    // Find the text run that contains the offset
    for (size_t i = 0; i < GetChildCount(); ++i) {
        DocumentNode* child = GetChild(i);
        if (child && child->GetType() == NODE_TYPE_TEXT_RUN) {
            TextRunNode* textRun = static_cast<TextRunNode*>(child);
            int32 childLength = textRun->GetTextLength();
            if (offset <= childLength) {
                // Insert into this text run
                textRun->text.Insert(text, offset);
                return;
            }
            offset -= childLength;
        }
    }
    
    // If we get here, append to the end
    AppendText(text);
}

void ParagraphNode::DeleteText(int32 offset, int32 length)
{
    if (length <= 0) {
        return;
    }
    
    // Find the text run that contains the offset
    for (size_t i = 0; i < GetChildCount(); ++i) {
        DocumentNode* child = GetChild(i);
        if (child && child->GetType() == NODE_TYPE_TEXT_RUN) {
            TextRunNode* textRun = static_cast<TextRunNode*>(child);
            int32 childLength = textRun->GetTextLength();
            if (offset < childLength) {
                // Delete from this text run
                int32 deleteLength = std::min(length, childLength - offset);
                textRun->text.Remove(offset, deleteLength);
                length -= deleteLength;
                if (length <= 0) {
                    return;
                }
                offset = 0;
            } else {
                offset -= childLength;
            }
        }
    }
}

BString ParagraphNode::GetText(int32 start, int32 length) const
{
    BString result;
    int32 remaining = length;
    int32 currentStart = start;
    
    for (size_t i = 0; i < GetChildCount() && remaining > 0; ++i) {
        DocumentNode* child = GetChild(i);
        if (child && child->GetType() == NODE_TYPE_TEXT_RUN) {
            TextRunNode* textRun = static_cast<TextRunNode*>(child);
            int32 childLength = textRun->GetTextLength();
            
            if (currentStart < childLength) {
                int32 copyLength = std::min(remaining, childLength - currentStart);
                result += textRun->text.String() + currentStart;
                result.Truncate(result.Length() + copyLength);
                remaining -= copyLength;
                currentStart = 0;
            } else {
                currentStart -= childLength;
            }
        }
    }
    
    return result;
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

// TextRunNode implementation
TextRunNode::TextRunNode()
    : DocumentNode(NODE_TYPE_TEXT_RUN)
{
}

TextRunNode::TextRunNode(const BString& text, const TextStyle& style)
    : DocumentNode(NODE_TYPE_TEXT_RUN),
      text(text),
      style(style)
{
}

TextRunNode::~TextRunNode()
{
}

NodeType TextRunNode::GetType() const
{
    return NODE_TYPE_TEXT_RUN;
}

YellowDocument* TextRunNode::GetDocument()
{
    return nullptr; // Will be implemented when we have document access
}

const YellowDocument* TextRunNode::GetDocument() const
{
    return nullptr; // Will be implemented when we have document access
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
      fCommandHistory(nullptr), // Will be created when needed
      fModified(false),
      fFormatVersion(1)
{
    // Set up root section
    SectionNode* section = new SectionNode();
    section->name = "Default Section";
    fRoot = section;
}

YellowDocument::~YellowDocument()
{
    Clear();
    delete fMetadata;
    delete fStyleManager;
    if (fCommandHistory) {
        delete fCommandHistory;
    }
}

void YellowDocument::Clear()
{
    // Remove all observers
    RemoveAllObservers();
    
    // Delete the document tree
    if (fRoot) {
        delete fRoot;
        fRoot = nullptr;
    }
    
    // Create new root section
    SectionNode* section = new SectionNode();
    section->name = "Default Section";
    fRoot = section;
    
    // Reset metadata
    if (fMetadata) {
        *fMetadata = DocumentMetadata();
    }
    
    // Reset modified state
    fModified = false;
}

bool YellowDocument::IsEmpty() const
{
    // Check if we have any content beyond the root section
    if (fRoot && fRoot->GetChildCount() > 0) {
        return false;
    }
    return true;
}

DocumentNode* YellowDocument::GetRoot() const
{
    return fRoot;
}

void YellowDocument::SetRoot(DocumentNode* root)
{
    if (fRoot != root) {
        if (fRoot) {
            delete fRoot;
        }
        fRoot = root;
        if (fRoot) {
            fRoot->SetParent(nullptr);
        }
    }
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
    if (observer) {
        // Check if already exists
        for (DocumentObserver* obs : fObservers) {
            if (obs == observer) {
                return;
            }
        }
        fObservers.push_back(observer);
    }
}

void YellowDocument::RemoveObserver(DocumentObserver* observer)
{
    if (observer) {
        for (auto it = fObservers.begin(); it != fObservers.end(); ++it) {
            if (*it == observer) {
                fObservers.erase(it);
                return;
            }
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
    for (DocumentObserver* observer : fObservers) {
        observer->DocumentContentChanged(range);
    }
}

void YellowDocument::NotifyStructureChanged()
{
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
        // TODO: Create command history when EditCommand is implemented
    }
    return fCommandHistory;
}

const CommandHistory* YellowDocument::GetCommandHistory() const
{
    return fCommandHistory;
}

bool YellowDocument::SaveToFile(const BString& filename)
{
    // TODO: Implement file saving
    return false;
}

bool YellowDocument::LoadFromFile(const BString& filename)
{
    // TODO: Implement file loading
    return false;
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
