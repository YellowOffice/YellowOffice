#include "YellowDocumentCore/Style.h"

#include <algorithm>
#include <string.h>
#include <vector>

namespace YellowDocumentCore {

// TextStyle implementation
TextStyle::TextStyle()
    : fontFamily("System"),
      fontSize(12.0f),
      fontFlags(0),
      textColor({255, 255, 255, 255}), // White
      backgroundColor({0, 0, 0, 0}), // Transparent
      bold(false),
      italic(false),
      underline(false),
      strikethrough(false),
      superscript(false),
      subscript(false),
      baselineShift(0.0f),
      letterSpacing(0.0f),
      lineSpacing(1.0f)
{
}

TextStyle::TextStyle(const TextStyle& other)
    : fontFamily(other.fontFamily),
      fontSize(other.fontSize),
      fontFlags(other.fontFlags),
      textColor(other.textColor),
      backgroundColor(other.backgroundColor),
      bold(other.bold),
      italic(other.italic),
      underline(other.underline),
      strikethrough(other.strikethrough),
      superscript(other.superscript),
      subscript(other.subscript),
      baselineShift(other.baselineShift),
      letterSpacing(other.letterSpacing),
      lineSpacing(other.lineSpacing)
{
}

TextStyle::~TextStyle()
{
}

TextStyle& TextStyle::operator=(const TextStyle& other)
{
    if (this != &other) {
        fontFamily = other.fontFamily;
        fontSize = other.fontSize;
        fontFlags = other.fontFlags;
        textColor = other.textColor;
        backgroundColor = other.backgroundColor;
        bold = other.bold;
        italic = other.italic;
        underline = other.underline;
        strikethrough = other.strikethrough;
        superscript = other.superscript;
        subscript = other.subscript;
        baselineShift = other.baselineShift;
        letterSpacing = other.letterSpacing;
        lineSpacing = other.lineSpacing;
    }
    return *this;
}

bool TextStyle::operator==(const TextStyle& other) const
{
    return fontFamily == other.fontFamily &&
           fontSize == other.fontSize &&
           fontFlags == other.fontFlags &&
           textColor == other.textColor &&
           backgroundColor == other.backgroundColor &&
           bold == other.bold &&
           italic == other.italic &&
           underline == other.underline &&
           strikethrough == other.strikethrough &&
           superscript == other.superscript &&
           subscript == other.subscript &&
           baselineShift == other.baselineShift &&
           letterSpacing == other.letterSpacing &&
           lineSpacing == other.lineSpacing;
}

bool TextStyle::operator!=(const TextStyle& other) const
{
    return !(*this == other);
}

bool TextStyle::HasFormatting() const
{
    // Check if any formatting is applied beyond defaults
    return bold || italic || underline || strikethrough ||
           superscript || subscript ||
           baselineShift != 0.0f ||
           letterSpacing != 0.0f ||
           lineSpacing != 1.0f ||
           fontSize != 12.0f ||
           fontFamily != "System" ||
           textColor != rgb_color({255, 255, 255, 255}) ||
           backgroundColor != rgb_color({0, 0, 0, 0});
}

void TextStyle::Reset()
{
    *this = TextStyle();
}

// ParagraphStyle implementation
ParagraphStyle::ParagraphStyle()
    : alignment(ALIGN_LEFT),
      leftIndent(0.0f),
      rightIndent(0.0f),
      firstLineIndent(0.0f),
      spaceBefore(0.0f),
      spaceAfter(0.0f),
      lineSpacing(1.0f),
      topMargin(0.0f),
      bottomMargin(0.0f),
      leftMargin(0.0f),
      rightMargin(0.0f),
      isList(false),
      isNumbered(false),
      listLevel(0),
      listBulletChar("•"),
      hasBorder(false),
      borderColor({0, 0, 0, 255}),
      borderWidth(1.0f),
      backgroundColor({0, 0, 0, 0})
{
}

ParagraphStyle::ParagraphStyle(const ParagraphStyle& other)
    : alignment(other.alignment),
      leftIndent(other.leftIndent),
      rightIndent(other.rightIndent),
      firstLineIndent(other.firstLineIndent),
      spaceBefore(other.spaceBefore),
      spaceAfter(other.spaceAfter),
      lineSpacing(other.lineSpacing),
      topMargin(other.topMargin),
      bottomMargin(other.bottomMargin),
      leftMargin(other.leftMargin),
      rightMargin(other.rightMargin),
      isList(other.isList),
      isNumbered(other.isNumbered),
      listLevel(other.listLevel),
      listBulletChar(other.listBulletChar),
      hasBorder(other.hasBorder),
      borderColor(other.borderColor),
      borderWidth(other.borderWidth),
      backgroundColor(other.backgroundColor)
{
}

ParagraphStyle::~ParagraphStyle()
{
}

ParagraphStyle& ParagraphStyle::operator=(const ParagraphStyle& other)
{
    if (this != &other) {
        alignment = other.alignment;
        leftIndent = other.leftIndent;
        rightIndent = other.rightIndent;
        firstLineIndent = other.firstLineIndent;
        spaceBefore = other.spaceBefore;
        spaceAfter = other.spaceAfter;
        lineSpacing = other.lineSpacing;
        topMargin = other.topMargin;
        bottomMargin = other.bottomMargin;
        leftMargin = other.leftMargin;
        rightMargin = other.rightMargin;
        isList = other.isList;
        isNumbered = other.isNumbered;
        listLevel = other.listLevel;
        listBulletChar = other.listBulletChar;
        hasBorder = other.hasBorder;
        borderColor = other.borderColor;
        borderWidth = other.borderWidth;
        backgroundColor = other.backgroundColor;
    }
    return *this;
}

bool ParagraphStyle::operator==(const ParagraphStyle& other) const
{
    return alignment == other.alignment &&
           leftIndent == other.leftIndent &&
           rightIndent == other.rightIndent &&
           firstLineIndent == other.firstLineIndent &&
           spaceBefore == other.spaceBefore &&
           spaceAfter == other.spaceAfter &&
           lineSpacing == other.lineSpacing &&
           topMargin == other.topMargin &&
           bottomMargin == other.bottomMargin &&
           leftMargin == other.leftMargin &&
           rightMargin == other.rightMargin &&
           isList == other.isList &&
           isNumbered == other.isNumbered &&
           listLevel == other.listLevel &&
           listBulletChar == other.listBulletChar &&
           hasBorder == other.hasBorder &&
           borderColor == other.borderColor &&
           borderWidth == other.borderWidth &&
           backgroundColor == other.backgroundColor;
}

bool ParagraphStyle::operator!=(const ParagraphStyle& other) const
{
    return !(*this == other);
}

bool ParagraphStyle::HasFormatting() const
{
    // Check if any formatting is applied beyond defaults
    return alignment != ALIGN_LEFT ||
           leftIndent != 0.0f ||
           rightIndent != 0.0f ||
           firstLineIndent != 0.0f ||
           spaceBefore != 0.0f ||
           spaceAfter != 0.0f ||
           lineSpacing != 1.0f ||
           topMargin != 0.0f ||
           bottomMargin != 0.0f ||
           leftMargin != 0.0f ||
           rightMargin != 0.0f ||
           isList ||
           isNumbered ||
           listLevel != 0 ||
           hasBorder ||
           borderWidth != 1.0f ||
           backgroundColor != rgb_color({0, 0, 0, 0});
}

void ParagraphStyle::Reset()
{
    *this = ParagraphStyle();
}

// StyleDefinition implementation
StyleDefinition::StyleDefinition()
    : type(STYLE_TYPE_PARAGRAPH)
{
}

StyleDefinition::StyleDefinition(const BString& name, const TextStyle& textStyle, 
                                   const ParagraphStyle& paragraphStyle)
    : name(name),
      displayName(name),
      textStyle(textStyle),
      paragraphStyle(paragraphStyle),
      type(STYLE_TYPE_PARAGRAPH)
{
}

StyleDefinition::StyleDefinition(const StyleDefinition& other)
    : name(other.name),
      displayName(other.displayName),
      description(other.description),
      basedOn(other.basedOn),
      textStyle(other.textStyle),
      paragraphStyle(other.paragraphStyle),
      type(other.type)
{
}

StyleDefinition::~StyleDefinition()
{
}

StyleDefinition& StyleDefinition::operator=(const StyleDefinition& other)
{
    if (this != &other) {
        name = other.name;
        displayName = other.displayName;
        description = other.description;
        basedOn = other.basedOn;
        textStyle = other.textStyle;
        paragraphStyle = other.paragraphStyle;
        type = other.type;
    }
    return *this;
}

bool StyleDefinition::operator==(const StyleDefinition& other) const
{
    return name == other.name &&
           displayName == other.displayName &&
           description == other.description &&
           basedOn == other.basedOn &&
           textStyle == other.textStyle &&
           paragraphStyle == other.paragraphStyle &&
           type == other.type;
}

bool StyleDefinition::operator!=(const StyleDefinition& other) const
{
    return !(*this == other);
}

bool StyleDefinition::IsValid() const
{
    return !name.IsEmpty();
}

void StyleDefinition::GetEffectiveStyle(TextStyle* outTextStyle, ParagraphStyle* outParagraphStyle) const
{
    if (outTextStyle) {
        *outTextStyle = textStyle;
    }
    if (outParagraphStyle) {
        *outParagraphStyle = paragraphStyle;
    }
    // TODO: Implement inheritance resolution
}

// StyleManager implementation
StyleManager::StyleManager()
{
}

StyleManager::~StyleManager()
{
    Clear();
}

void StyleManager::AddStyle(const StyleDefinition& style)
{
    if (style.IsValid()) {
        // Check if style with same name already exists
        for (StyleDefinition* existing : fStyles) {
            if (existing->name == style.name) {
                // Replace existing style
                *existing = style;
                return;
            }
        }
        // Add new style
        fStyles.push_back(new StyleDefinition(style));
    }
}

void StyleManager::RemoveStyle(const BString& name)
{
    for (auto it = fStyles.begin(); it != fStyles.end(); ++it) {
        if ((*it)->name == name) {
            delete *it;
            fStyles.erase(it);
            return;
        }
    }
}

StyleDefinition* StyleManager::GetStyle(const BString& name)
{
    for (StyleDefinition* style : fStyles) {
        if (style->name == name) {
            return style;
        }
    }
    return nullptr;
}

const StyleDefinition* StyleManager::GetStyle(const BString& name) const
{
    for (const StyleDefinition* style : fStyles) {
        if (style->name == name) {
            return style;
        }
    }
    return nullptr;
}

void StyleManager::SetDefaultParagraphStyle(const ParagraphStyle& style)
{
    fDefaultParagraphStyle = style;
}

void StyleManager::SetDefaultTextStyle(const TextStyle& style)
{
    fDefaultTextStyle = style;
}

const ParagraphStyle& StyleManager::GetDefaultParagraphStyle() const
{
    return fDefaultParagraphStyle;
}

const TextStyle& StyleManager::GetDefaultTextStyle() const
{
    return fDefaultTextStyle;
}

size_t StyleManager::GetStyleCount() const
{
    return fStyles.size();
}

StyleDefinition* StyleManager::GetStyleAt(size_t index)
{
    if (index < fStyles.size()) {
        return fStyles[index];
    }
    return nullptr;
}

const StyleDefinition* StyleManager::GetStyleAt(size_t index) const
{
    if (index < fStyles.size()) {
        return fStyles[index];
    }
    return nullptr;
}

void StyleManager::GetParagraphStyles(std::vector<const StyleDefinition*>& outStyles) const
{
    outStyles.clear();
    for (const StyleDefinition* style : fStyles) {
        if (style->type == StyleDefinition::STYLE_TYPE_PARAGRAPH) {
            outStyles.push_back(style);
        }
    }
}

void StyleManager::GetCharacterStyles(std::vector<const StyleDefinition*>& outStyles) const
{
    outStyles.clear();
    for (const StyleDefinition* style : fStyles) {
        if (style->type == StyleDefinition::STYLE_TYPE_CHARACTER) {
            outStyles.push_back(style);
        }
    }
}

void StyleManager::Clear()
{
    for (StyleDefinition* style : fStyles) {
        delete style;
    }
    fStyles.clear();
}

} // namespace YellowDocumentCore
