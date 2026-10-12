#ifndef YELLOW_DOCUMENT_CORE_STYLE_H
#define YELLOW_DOCUMENT_CORE_STYLE_H

#include <SupportDefs.h>
#include <String.h>
#include <GraphicsDefs.h>
#include <vector>

#include "Defines.h"

namespace YellowDocumentCore {

// Forward declarations
class StyleDefinition;

/**
 * Text style attributes
 */
class TextStyle {
public:
    TextStyle();
    TextStyle(const TextStyle& other);
    ~TextStyle();

    TextStyle& operator=(const TextStyle& other);

    // Font attributes
    BString fontFamily;
    float fontSize;
    uint32 fontFlags; // B_FONT_* flags from GraphicsDefs.h
    
    // Color attributes
    rgb_color textColor;
    rgb_color backgroundColor;
    
    // Text decoration
    bool bold;
    bool italic;
    bool underline;
    bool strikethrough;
    
    // Advanced formatting
    bool superscript;
    bool subscript;
    float baselineShift; // For superscript/subscript
    
    // Spacing
    float letterSpacing;
    float lineSpacing;

    // Comparison
    bool operator==(const TextStyle& other) const;
    bool operator!=(const TextStyle& other) const;
    
    // Check if style has any formatting
    bool HasFormatting() const;
    
    // Reset to default
    void Reset();
};

/**
 * Paragraph style attributes
 */
class ParagraphStyle {
public:
    ParagraphStyle();
    ParagraphStyle(const ParagraphStyle& other);
    ~ParagraphStyle();

    ParagraphStyle& operator=(const ParagraphStyle& other);

    // Alignment
    enum Alignment {
        ALIGN_LEFT,
        ALIGN_CENTER,
        ALIGN_RIGHT,
        ALIGN_JUSTIFY
    };
    
    Alignment alignment;
    
    // Indentation
    float leftIndent;
    float rightIndent;
    float firstLineIndent;
    
    // Spacing
    float spaceBefore;
    float spaceAfter;
    float lineSpacing;
    
    // Margins
    float topMargin;
    float bottomMargin;
    float leftMargin;
    float rightMargin;
    
    // List style
    bool isList;
    bool isNumbered;
    int32 listLevel;
    BString listBulletChar;
    
    // Border
    bool hasBorder;
    rgb_color borderColor;
    float borderWidth;
    
    // Background
    rgb_color backgroundColor;
    
    // Comparison
    bool operator==(const ParagraphStyle& other) const;
    bool operator!=(const ParagraphStyle& other) const;
    
    // Check if style has any formatting
    bool HasFormatting() const;
    
    // Reset to default
    void Reset();
};

/**
 * Style definition with name and inheritance
 */
class StyleDefinition {
public:
    StyleDefinition();
    StyleDefinition(const BString& name, const TextStyle& textStyle = TextStyle(), 
                   const ParagraphStyle& paragraphStyle = ParagraphStyle());
    StyleDefinition(const StyleDefinition& other);
    ~StyleDefinition();

    StyleDefinition& operator=(const StyleDefinition& other);

    // Style properties
    BString name;
    BString displayName;
    BString description;
    BString basedOn; // Parent style name for inheritance
    
    TextStyle textStyle;
    ParagraphStyle paragraphStyle;
    
    // Style type
    enum Type {
        STYLE_TYPE_PARAGRAPH,
        STYLE_TYPE_CHARACTER,
        STYLE_TYPE_TABLE,
        STYLE_TYPE_LIST
    };
    Type type;
    
    // Comparison
    bool operator==(const StyleDefinition& other) const;
    bool operator!=(const StyleDefinition& other) const;
    
    // Check if style is valid
    bool IsValid() const;
    
    // Get effective style (resolving inheritance)
    void GetEffectiveStyle(TextStyle* outTextStyle, ParagraphStyle* outParagraphStyle) const;
};

/**
 * Style manager for the document
 */
class StyleManager {
public:
    StyleManager();
    ~StyleManager();

    // Style management
    void AddStyle(const StyleDefinition& style);
    void RemoveStyle(const BString& name);
    StyleDefinition* GetStyle(const BString& name);
    const StyleDefinition* GetStyle(const BString& name) const;
    
    // Default styles
    void SetDefaultParagraphStyle(const ParagraphStyle& style);
    void SetDefaultTextStyle(const TextStyle& style);
    const ParagraphStyle& GetDefaultParagraphStyle() const;
    const TextStyle& GetDefaultTextStyle() const;
    
    // Style count
    size_t GetStyleCount() const;
    StyleDefinition* GetStyleAt(size_t index);
    const StyleDefinition* GetStyleAt(size_t index) const;
    
    // Find styles by type
    void GetParagraphStyles(std::vector<const StyleDefinition*>& outStyles) const;
    void GetCharacterStyles(std::vector<const StyleDefinition*>& outStyles) const;
    
    // Clear all styles
    void Clear();

private:
    std::vector<StyleDefinition*> fStyles;
    ParagraphStyle fDefaultParagraphStyle;
    TextStyle fDefaultTextStyle;
};

} // namespace YellowDocumentCore

#endif // YELLOW_DOCUMENT_CORE_STYLE_H
