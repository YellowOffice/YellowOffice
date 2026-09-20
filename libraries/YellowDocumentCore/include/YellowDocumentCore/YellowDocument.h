#ifndef YELLOW_DOCUMENT_CORE_YELLOW_DOCUMENT_H
#define YELLOW_DOCUMENT_CORE_YELLOW_DOCUMENT_H

#include <SupportDefs.h>
#include <String.h>
#include <vector>
#include <memory>

#include <YellowCommon/Defines.h>
#include <YellowDocumentCore/DocumentNode.h>
#include <YellowDocumentCore/Style.h>

namespace YellowDocumentCore {

// Forward declarations
class DocumentChange;
class DocumentObserver;
class EditCommand;
class CommandHistory;

/**
 * DocumentChange represents a change in the document
 */
class DocumentChange {
public:
    enum Type {
        CHANGE_TYPE_INSERT,
        CHANGE_TYPE_DELETE,
        CHANGE_TYPE_FORMAT,
        CHANGE_TYPE_STYLE,
        CHANGE_TYPE_STRUCTURE
    };

    DocumentChange(Type type, const DocumentRange& range = DocumentRange());
    ~DocumentChange();

    Type GetType() const;
    const DocumentRange& GetRange() const;
    void SetRange(const DocumentRange& range);

private:
    Type fType;
    DocumentRange fRange;
};

/**
 * DocumentObserver interface for observing document changes
 */
class DocumentObserver {
public:
    virtual ~DocumentObserver();

    virtual void DocumentChanged(const DocumentChange& change) = 0;
    virtual void DocumentContentChanged(const DocumentRange& range) = 0;
    virtual void DocumentStructureChanged() = 0;
    virtual void DocumentStylesChanged() = 0;
};

/**
 * DocumentMetadata contains document metadata
 */
class DocumentMetadata : public DocumentNode {
public:
    DocumentMetadata();
    ~DocumentMetadata();

    // Metadata properties
    BString title;
    BString author;
    BString subject;
    BString keywords;
    BString description;
    BString generator;
    
    time_t created;
    time_t modified;
    time_t lastSaved;
    
    // Document statistics
    int32 pageCount;
    int32 paragraphCount;
    int32 wordCount;
    int32 characterCount;
    
    // Node interface
    NodeType GetType() const override;
    DocumentNode* Clone() const override;
    
    // Metadata access
    void UpdateStatistics(YellowDocument* document);
};

/**
 * SectionNode represents a section in the document
 */
class SectionNode : public DocumentNode {
public:
    SectionNode();
    ~SectionNode();

    // Section properties
    BString name;
    bool isContinuous;
    
    // Page properties
    float pageWidth;
    float pageHeight;
    float leftMargin;
    float rightMargin;
    float topMargin;
    float bottomMargin;
    
    // Header and footer
    bool hasHeader;
    bool hasFooter;
    
    // Node interface
    NodeType GetType() const override;
    YellowDocument* GetDocument() override;
    const YellowDocument* GetDocument() const override;
    DocumentNode* Clone() const override;
};

/**
 * ParagraphNode represents a paragraph in the document
 */
class ParagraphNode : public DocumentNode {
public:
    ParagraphNode();
    ParagraphNode(const BString& text);
    ~ParagraphNode();

    // Paragraph properties
    ParagraphStyle style;
    BString paragraphStyleName;
    
    // Node interface
    NodeType GetType() const override;
    YellowDocument* GetDocument() override;
    const YellowDocument* GetDocument() const override;
    BString GetText() const override;
    void SetText(const BString& text) override;
    int32 GetTextLength() const override;
    DocumentPosition GetStartPosition() const override;
    DocumentPosition GetEndPosition() const override;
    DocumentPosition GetPositionAt(int32 offset) const override;
    DocumentNode* Clone() const override;
    
    // Text content
    void AppendText(const BString& text);
    void InsertText(int32 offset, const BString& text);
    void DeleteText(int32 offset, int32 length);
    BString GetText(int32 start, int32 length) const;
    
    // Child management with text
    void AddTextRun(DocumentNode* textRun);
    
    // Style
    void SetStyle(const ParagraphStyle& newStyle);
    const ParagraphStyle& GetStyle() const;
    void SetStyleName(const BString& styleName);
    const BString& GetStyleName() const;
};

/**
 * TextRunNode represents a run of text with consistent formatting
 */
class TextRunNode : public DocumentNode {
public:
    TextRunNode();
    TextRunNode(const BString& text, const TextStyle& style = TextStyle());
    ~TextRunNode();

    // TextRun properties
    BString text;
    TextStyle style;
    BString characterStyleName;
    
    // Node interface
    NodeType GetType() const override;
    YellowDocument* GetDocument() override;
    const YellowDocument* GetDocument() const override;
    BString GetText() const override;
    void SetText(const BString& newText) override;
    int32 GetTextLength() const override;
    DocumentPosition GetStartPosition() const override;
    DocumentPosition GetEndPosition() const override;
    DocumentPosition GetPositionAt(int32 offset) const override;
    DocumentNode* Clone() const override;
    
    // Style
    void SetStyle(const TextStyle& newStyle);
    const TextStyle& GetStyle() const;
    void SetStyleName(const BString& styleName);
    const BString& GetStyleName() const;
};

/**
 * Main YellowDocument class
 */
class YellowDocument {
public:
    YellowDocument();
    ~YellowDocument();

    // Document management
    void Clear();
    bool IsEmpty() const;
    
    // Root node access
    DocumentNode* GetRoot() const;
    void SetRoot(DocumentNode* root);
    
    // Metadata
    DocumentMetadata* GetMetadata();
    const DocumentMetadata* GetMetadata() const;
    void SetMetadata(DocumentMetadata* metadata);
    
    // Style management
    StyleManager* GetStyleManager();
    const StyleManager* GetStyleManager() const;
    void SetStyleManager(StyleManager* styleManager);
    
    // Observer management
    void AddObserver(DocumentObserver* observer);
    void RemoveObserver(DocumentObserver* observer);
    void RemoveAllObservers();
    
    // Document modification
    void NotifyDocumentChanged(const DocumentChange& change);
    void NotifyContentChanged(const DocumentRange& range);
    void NotifyStructureChanged();
    void NotifyStylesChanged();
    
    // Command history
    CommandHistory* GetCommandHistory();
    const CommandHistory* GetCommandHistory() const;
    
    // File operations (will be implemented in import/export)
    // These are placeholders for now
    bool SaveToFile(const BString& filename);
    bool LoadFromFile(const BString& filename);
    
    // Document properties
    BString GetTitle() const;
    void SetTitle(const BString& title);
    
    BString GetAuthor() const;
    void SetAuthor(const BString& author);
    
    // Statistics
    void UpdateStatistics();
    int32 GetPageCount() const;
    int32 GetParagraphCount() const;
    int32 GetWordCount() const;
    int32 GetCharacterCount() const;
    
    // Document state
    bool IsModified() const;
    void SetModified(bool modified);
    void MarkSaved();
    
    // Document version
    int32 GetFormatVersion() const;
    void SetFormatVersion(int32 version);

private:
    DocumentNode* fRoot;
    DocumentMetadata* fMetadata;
    StyleManager* fStyleManager;
    CommandHistory* fCommandHistory;
    std::vector<DocumentObserver*> fObservers;
    
    bool fModified;
    int32 fFormatVersion;
};

} // namespace YellowDocumentCore

#endif // YELLOW_DOCUMENT_CORE_YELLOW_DOCUMENT_H
