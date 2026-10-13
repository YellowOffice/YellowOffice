#ifndef YELLOW_DOCUMENT_LAYOUT_TYPES_H
#define YELLOW_DOCUMENT_LAYOUT_TYPES_H

#include <SupportDefs.h>
#include <Rect.h>
#include <Point.h>
#include <Size.h>
#include <Font.h>
#include <String.h>
#include <map>
#include <vector>

#include <YellowDocumentCore/DocumentNode.h>
#include <YellowDocumentCore/YellowDocument.h>

namespace YellowDocumentLayout {

using YellowDocumentCore::DocumentNode;
using YellowDocumentCore::DocumentPosition;
using YellowDocumentCore::DocumentRange;
using YellowDocumentCore::ParagraphNode;
using YellowDocumentCore::SectionNode;
using YellowDocumentCore::TextRunNode;
using YellowDocumentCore::TextStyle;
using YellowDocumentCore::ParagraphStyle;
using YellowDocumentCore::YellowDocument;

/// One styled piece of a visual line.
struct LineRun {
    TextRunNode* run;
    int32 startInParagraph; // paragraph-relative offset of run start
    int32 offsetInRun;      // first character of this fragment within the run
    int32 length;           // characters in this fragment
    BString text;
    TextStyle style;
    float width;
};

/// One visual line inside a paragraph.
struct LineFragment {
    int32 start;    // paragraph-relative offset of line start
    int32 end;      // paragraph-relative offset one past line end
    float x;        // paragraph-relative x of line start (after alignment)
    float y;        // paragraph-relative y of line top (before paragraph origin)
    float width;    // used width of the line
    float height;
    float baseline; // y of baseline from line top
    bool isLastLineOfParagraph;
    std::vector<LineRun> runs;
};

/// Laid-out paragraph (block) placed on a page.
struct ParagraphLayout {
    ParagraphNode* paragraph;
    int32 pageIndex;
    BPoint origin;      // document coordinates of content top-left (page space)
    float contentWidth; // usable width (after indents)
    float width;        // full block width including indents
    float height;       // total height including spaceBefore/After
    std::vector<LineFragment> lines;
};

/// Placed embedded image.
struct PlacedImage {
    YellowDocumentCore::ImageNode* image;
    int32 pageIndex;
    BRect bounds; // document coordinates
};

/// Simple table geometry cache for rendering.
struct PlacedTable {
    YellowDocumentCore::TableNode* table;
    int32 pageIndex;
    BRect bounds;
    std::vector<float> columnWidths;
    std::vector<float> rowHeights;
};

/// Page description.
struct PageInfo {
    int32 index;
    BRect bounds; // full page in document coordinates
    BRect contentBounds;
};

// Cache key for fonts derived from text styles.
struct FontKey {
    BString family;
    float size;
    bool bold;
    bool italic;

    bool operator<(const FontKey& other) const;
};

class LayoutEngine;

/// Converts a document position inside the paragraph (or its descendants)
/// into a paragraph-relative text offset. Returns -1 when the position does
/// not belong to the paragraph.
int32 ParagraphOffsetOf(ParagraphNode* paragraph, const DocumentPosition& position);

/// Measures text with BFont, caching one BFont per distinct style.
class FontCache {
public:
    FontCache();
    ~FontCache();

    BFont FontForStyle(const TextStyle& style);
    float StringWidth(const BString& text, const TextStyle& style);
    float LineHeight(const TextStyle& style, float paragraphLineSpacing);
    float Ascent(const TextStyle& style);

private:
    std::map<FontKey, BFont> fFonts;
};

} // namespace YellowDocumentLayout

#endif // YELLOW_DOCUMENT_LAYOUT_TYPES_H
