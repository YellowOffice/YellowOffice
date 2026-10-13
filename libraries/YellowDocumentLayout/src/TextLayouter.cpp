#include <YellowDocumentLayout/TextLayouter.h>
#include <YellowDocumentLayout/LayoutEngine.h>

#include <algorithm>
#include <string.h>

namespace YellowDocumentLayout {

TextLayouter::TextLayouter(LayoutEngine* engine)
    : fEngine(engine)
{
}

TextLayouter::~TextLayouter()
{
}

void TextLayouter::CollectStyledRuns(ParagraphNode* paragraph, std::vector<LineRun>& out) const
{
    out.clear();
    if (!paragraph) return;
    FontCache* fonts = fEngine->GetFontCache();

    int32 position = 0;
    for (size_t i = 0; i < paragraph->GetChildCount(); ++i) {
        DocumentNode* child = paragraph->GetChild(i);
        if (!child || child->GetType() != YellowDocumentCore::NODE_TYPE_TEXT_RUN) continue;
        TextRunNode* run = static_cast<TextRunNode*>(child);
        LineRun lineRun;
        lineRun.run = run;
        lineRun.startInParagraph = position;
        lineRun.offsetInRun = 0;
        lineRun.length = run->GetTextLength();
        lineRun.text = run->text;
        lineRun.style = run->style;
        lineRun.width = fonts->StringWidth(lineRun.text, lineRun.style);
        out.push_back(lineRun);
        position += lineRun.length;
    }
}

namespace {

bool IsSpace(char c)
{
    return c == ' ' || c == '\t';
}

// Splits a word (non-space sequence) from a run starting at localOffset.
int32 WordLengthAt(const BString& text, int32 localOffset)
{
    int32 length = 0;
    const char* s = text.String();
    while (localOffset + length < text.Length() && !IsSpace(s[localOffset + length])) {
        ++length;
    }
    return length;
}

int32 SpaceLengthAt(const BString& text, int32 localOffset)
{
    int32 length = 0;
    const char* s = text.String();
    while (localOffset + length < text.Length() && IsSpace(s[localOffset + length])) {
        ++length;
    }
    return length;
}

} // namespace

void TextLayouter::LayoutLines(ParagraphNode* paragraph, float contentWidth, ParagraphLayout* out)
{
    if (!paragraph || !out) return;
    if (contentWidth < 20.0f) contentWidth = 20.0f;

    std::vector<LineRun> sourceRuns;
    CollectStyledRuns(paragraph, sourceRuns);

    out->lines.clear();

    // Empty paragraph still needs one caret line.
    if (sourceRuns.empty() || (sourceRuns.size() == 1 && sourceRuns[0].length == 0)) {
        LineFragment line;
        line.start = 0;
        line.end = 0;
        line.x = 0;
        line.y = 0;
        line.width = 0;
        TextStyle defaultStyle = sourceRuns.empty() ? TextStyle() : sourceRuns[0].style;
        line.height = fEngine->GetFontCache()->LineHeight(defaultStyle, paragraph->style.lineSpacing);
        line.baseline = fEngine->GetFontCache()->Ascent(defaultStyle);
        line.isLastLineOfParagraph = true;
        out->lines.push_back(line);
        return;
    }

    FontCache* fonts = fEngine->GetFontCache();

    // Current line state
    LineFragment current;
    current.start = 0;
    current.y = 0;
    current.width = 0;
    current.height = 0;
    current.baseline = 0;
    current.isLastLineOfParagraph = false;

    float maxHeight = 0;
    float maxBaseline = 0;

    auto flushLine = [&](bool last) {
        current.height = maxHeight > 0 ? maxHeight
            : fonts->LineHeight(current.runs.empty() ? TextStyle() : current.runs[0].style,
                                paragraph->style.lineSpacing);
        current.baseline = maxBaseline > 0 ? maxBaseline : fonts->Ascent(
            current.runs.empty() ? TextStyle() : current.runs[0].style);
        current.isLastLineOfParagraph = last;
        out->lines.push_back(current);
        current = LineFragment();
        current.y = out->lines.back().y + out->lines.back().height;
        current.width = 0;
        maxHeight = 0;
        maxBaseline = 0;
    };

    auto appendFragment = [&](const LineRun& fragment) {
        current.runs.push_back(fragment);
        current.width += fragment.width;
        float height = fonts->LineHeight(fragment.style, paragraph->style.lineSpacing);
        float ascent = fonts->Ascent(fragment.style);
        if (height > maxHeight) maxHeight = height;
        if (ascent > maxBaseline) maxBaseline = ascent;
    };

    for (const LineRun& source : sourceRuns) {
        int32 local = 0;
        while (local < source.length) {
            const char* s = source.text.String();
            if (IsSpace(s[local])) {
                int32 spaceLen = SpaceLengthAt(source.text, local);
                float spaceWidth = fonts->StringWidth(
                    BString(s + local, spaceLen), source.style);
                // Spaces attach to the current line unless the line is empty.
                if (!current.runs.empty()) {
                    LineRun fragment = source;
                    fragment.offsetInRun = local;
                    fragment.length = spaceLen;
                    fragment.text = BString(s + local, spaceLen);
                    fragment.width = spaceWidth;
                    appendFragment(fragment);
                }
                local += spaceLen;
                continue;
            }

            int32 wordLen = WordLengthAt(source.text, local);
            float wordWidth = fonts->StringWidth(BString(s + local, wordLen), source.style);

            if (!current.runs.empty() && current.width + wordWidth > contentWidth) {
                // Trim trailing spaces from the line before wrapping.
                while (!current.runs.empty()) {
                    LineRun& last = current.runs.back();
                    if (last.length > 0 && IsSpace(last.text[last.length - 1])) {
                        last.length -= 1;
                        last.text.Truncate(last.length);
                        last.width = fonts->StringWidth(last.text, last.style);
                        current.width -= fonts->StringWidth(
                            BString(" ", 1), last.style);
                        if (last.length == 0) {
                            current.runs.pop_back();
                        }
                    } else {
                        break;
                    }
                }
                int32 lineEnd = current.start;
                if (!current.runs.empty()) {
                    lineEnd = current.runs.back().startInParagraph +
                              current.runs.back().offsetInRun + current.runs.back().length;
                }
                current.end = lineEnd;
                flushLine(false);
                current.start = lineEnd;
            }

            // Hard-break words that cannot fit on an empty line.
            while (wordLen > 0) {
                float available = contentWidth - current.width;
                int32 fit = wordLen;
                if (fonts->StringWidth(BString(s + local, wordLen), source.style) > available) {
                    // Find how many chars fit.
                    fit = 0;
                    float width = 0;
                    while (fit < wordLen) {
                        float next = fonts->StringWidth(
                            BString(s + local + fit, 1), source.style);
                        if (width + next > available && fit > 0) break;
                        width += next;
                        ++fit;
                    }
                    if (fit == 0) fit = 1; // always make progress
                }

                LineRun fragment = source;
                fragment.offsetInRun = local;
                fragment.length = fit;
                fragment.text = BString(s + local, fit);
                fragment.width = fonts->StringWidth(fragment.text, source.style);
                appendFragment(fragment);

                local += fit;
                wordLen -= fit;
                if (wordLen > 0) {
                    int32 lineEnd = current.start;
                    if (!current.runs.empty()) {
                        lineEnd = current.runs.back().startInParagraph +
                                  current.runs.back().offsetInRun + current.runs.back().length;
                    }
                    current.end = lineEnd;
                    flushLine(false);
                    current.start = lineEnd;
                }
            }
        }
    }

    // Flush the final line.
    int32 paragraphLength = paragraph->GetTextLength();
    if (!current.runs.empty() || out->lines.empty()) {
        current.start = out->lines.empty() ? 0 : out->lines.back().start;
        // Recompute start as end of previous line.
        if (!out->lines.empty()) {
            current.start = out->lines.back().end;
        }
        current.end = paragraphLength;
        flushLine(true);
    } else {
        out->lines.back().isLastLineOfParagraph = true;
    }
}

} // namespace YellowDocumentLayout
