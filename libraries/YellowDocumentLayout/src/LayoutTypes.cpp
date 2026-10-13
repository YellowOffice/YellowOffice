#include <YellowDocumentLayout/LayoutTypes.h>

namespace YellowDocumentLayout {

int32 ParagraphOffsetOf(ParagraphNode* paragraph, const DocumentPosition& position)
{
    if (!paragraph || !position.IsValid()) return -1;
    if (position.GetNode() == paragraph) {
        int32 length = paragraph->GetTextLength();
        int32 offset = position.GetOffset();
        if (offset < 0) offset = 0;
        if (offset > length) offset = length;
        return offset;
    }

    int32 base = 0;
    for (size_t i = 0; i < paragraph->GetChildCount(); ++i) {
        DocumentNode* child = paragraph->GetChild(i);
        if (!child) continue;
        if (child == position.GetNode() || child->Contains(position.GetNode())) {
            if (child->GetType() == YellowDocumentCore::NODE_TYPE_TEXT_RUN) {
                return base + position.GetOffset();
            }
            // Nested node: approximate by descending (runs are direct children
            // in practice, but handle deeper trees via text length).
            return base + position.GetOffset();
        }
        base += child->GetTextLength();
    }
    return -1;
}

bool FontKey::operator<(const FontKey& other) const
{
    int familyCompare = family.Compare(other.family);
    if (familyCompare != 0) return familyCompare < 0;
    if (size != other.size) return size < other.size;
    if (bold != other.bold) return bold < other.bold;
    if (italic != other.italic) return italic < other.italic;
    return false;
}

FontCache::FontCache()
{
}

FontCache::~FontCache()
{
}

BFont FontCache::FontForStyle(const TextStyle& style)
{
    FontKey key;
    key.family = style.fontFamily;
    key.size = style.fontSize > 0 ? style.fontSize : 12.0f;
    key.bold = style.bold;
    key.italic = style.italic;

    auto it = fFonts.find(key);
    if (it != fFonts.end()) {
        return it->second;
    }

    BFont font;
    if (key.family.Length() > 0 && key.family != "System" && key.family != "Noto Sans") {
        font_family family;
        font_style fontStyle;
        font.GetFamilyAndStyle(family, fontStyle);
        font.SetFamilyAndStyle(key.family.String(), "Regular");
    }
    font.SetSize(key.size);
    uint32 face = B_REGULAR_FACE;
    if (key.bold && key.italic) {
        face = B_BOLD_ITALIC_FACE;
    } else if (key.bold) {
        face = B_BOLD_FACE;
    } else if (key.italic) {
        face = B_ITALIC_FACE;
    }
    font.SetFace(face);

    fFonts[key] = font;
    return font;
}

float FontCache::StringWidth(const BString& text, const TextStyle& style)
{
    BFont font = FontForStyle(style);
    float width = font.StringWidth(text);
    if (style.letterSpacing != 0.0f && text.Length() > 0) {
        width += style.letterSpacing * (float)(text.Length() - 1);
    }
    return width;
}

float FontCache::LineHeight(const TextStyle& style, float paragraphLineSpacing)
{
    BFont font = FontForStyle(style);
    font_height height;
    font.GetHeight(&height);
    float lineSpacing = paragraphLineSpacing > 0 ? paragraphLineSpacing
                                                : (style.lineSpacing > 0 ? style.lineSpacing : 1.0f);
    return (height.ascent + height.descent + height.leading) * lineSpacing;
}

float FontCache::Ascent(const TextStyle& style)
{
    BFont font = FontForStyle(style);
    font_height height;
    font.GetHeight(&height);
    return height.ascent;
}

} // namespace YellowDocumentLayout
