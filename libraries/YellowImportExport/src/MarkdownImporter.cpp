#include <YellowImportExport/MarkdownImporter.h>
#include <YellowDocumentCore/YellowDocument.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

namespace YellowImportExport {

using namespace YellowDocumentCore;

MarkdownImporter::MarkdownImporter()
{
}

MarkdownImporter::~MarkdownImporter()
{
}

namespace {

// Parses inline emphasis: **bold**, *italic*. Produces text runs.
void AppendInlineRuns(ParagraphNode* paragraph, const char* text, int32 length,
                      const TextStyle& baseStyle)
{
    int32 i = 0;
    BString plain;
    TextStyle style = baseStyle;

    auto flushPlain = [&]() {
        if (!plain.IsEmpty()) {
            TextRunNode* run = new TextRunNode(plain, style);
            paragraph->AddChild(run);
            plain.Truncate(0);
        }
    };

    while (i < length) {
        if (i + 1 < length && text[i] == '*' && text[i + 1] == '*') {
            flushPlain();
            style = baseStyle;
            style.bold = !style.bold;
            i += 2;
            continue;
        }
        if (text[i] == '*') {
            flushPlain();
            style = baseStyle;
            style.italic = !style.italic;
            i += 1;
            continue;
        }
        plain += text[i];
        ++i;
    }
    flushPlain();
}

void AppendParagraph(YellowDocument* document, DocumentNode* parent, const BString& line,
                     const TextStyle& runStyle, const BString& styleName,
                     float fontSize, bool bold)
{
    ParagraphNode* paragraph = new ParagraphNode();
    paragraph->paragraphStyleName = styleName;
    TextStyle style = runStyle;
    style.fontSize = fontSize;
    style.bold = bold;
    if (line.Length() > 0) {
        AppendInlineRuns(paragraph, line.String(), line.Length(), style);
    } else {
        paragraph->EnsureTextRun();
    }
    parent->AddChild(paragraph);
    (void)document;
}

} // namespace

bool MarkdownImporter::Import(YellowDocument* document, const BString& filename)
{
    if (!document) return false;
    FILE* file = fopen(filename.String(), "rb");
    if (!file) return false;
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    if (size < 0) {
        fclose(file);
        return false;
    }
    BString text;
    char* buffer = (char*)malloc((size_t)size + 1);
    if (!buffer) {
        fclose(file);
        return false;
    }
    size_t read = fread(buffer, 1, (size_t)size, file);
    fclose(file);
    buffer[read] = '\0';
    text.SetTo(buffer, (int32)read);
    free(buffer);
    return ImportFromData(document, text.String(), (size_t)text.Length());
}

bool MarkdownImporter::ImportFromData(YellowDocument* document, const char* data, size_t size)
{
    if (!document || !data) return false;

    document->Clear();
    DocumentNode* root = document->GetRoot();
    if (!root) return false;

    // Clear() seeds a default paragraph; start from an empty root.
    while (root->GetChildCount() > 0) {
        DocumentNode* child = root->GetChild(0);
        root->RemoveChild(child);
        delete child;
    }

    BString content(data, (int32)size);
    int32 pos = 0;
    DocumentNode* list = nullptr;

    while (pos <= content.Length()) {
        // Extract one line.
        int32 lineEnd = pos;
        while (lineEnd < content.Length() && content[lineEnd] != '\n') ++lineEnd;
        BString line(content.String() + pos, lineEnd - pos);
        if (line.Length() > 0 && line[line.Length() - 1] == '\r') {
            line.Truncate(line.Length() - 1);
        }
        pos = lineEnd + 1;
        if (pos > content.Length() && line.IsEmpty()) break;

        BString stripped = line;
        // Trim leading spaces (but keep list markers which need one leading space check).
        while (stripped.Length() > 0 && stripped[0] == ' ') {
            stripped.Remove(0, 1);
        }

        if (stripped.IsEmpty()) {
            list = nullptr; // blank line ends a list
            continue;
        }

        // Headings.
        if (stripped[0] == '#') {
            int32 level = 0;
            while (level < stripped.Length() && stripped[level] == '#') ++level;
            if (level > 0 && level < stripped.Length() && stripped[level] == ' ') {
                BString text(stripped.String() + level + 1,
                             stripped.Length() - level - 1);
                char styleName[32];
                snprintf(styleName, sizeof(styleName), "Heading %d", level);
                float fontSize = 28.0f - (float)(level - 1) * 3.0f;
                if (fontSize < 12.0f) fontSize = 12.0f;
                AppendParagraph(document, root, text, TextStyle(), styleName,
                                fontSize, true);
                continue;
            }
        }

        // Horizontal rule.
        if (stripped == "---" || stripped == "***" || stripped == "___") {
            list = nullptr;
            continue;
        }

        // Lists.
        bool ordered = false;
        int32 markerLength = 0;
        if (stripped.Length() >= 2 && stripped[0] == '-' && stripped[1] == ' ') {
            markerLength = 2;
        } else if (stripped.Length() >= 2 && stripped[0] == '*' && stripped[1] == ' ') {
            markerLength = 2;
        } else {
            // "N. "
            int32 i = 0;
            while (i < stripped.Length() && stripped[i] >= '0' && stripped[i] <= '9') ++i;
            if (i > 0 && i + 1 < stripped.Length() && stripped[i] == '.' &&
                stripped[i + 1] == ' ') {
                ordered = true;
                markerLength = i + 2;
            }
        }
        if (markerLength > 0) {
            if (!list) {
                list = new ListNode(ordered);
                root->AddChild(list);
            }
            ListItemNode* item = new ListItemNode();
            list->AddChild(item);
            BString text(stripped.String() + markerLength,
                         stripped.Length() - markerLength);
            ParagraphNode* paragraph = new ParagraphNode();
            paragraph->style.isList = true;
            paragraph->style.isNumbered = ordered;
            if (text.Length() > 0) {
                AppendInlineRuns(paragraph, text.String(), text.Length(), TextStyle());
            } else {
                paragraph->EnsureTextRun();
            }
            item->AddChild(paragraph);
            continue;
        }

        // Blockquote (kept as plain paragraph with '>' stripped).
        if (stripped[0] == '>') {
            BString text;
            if (stripped.Length() > 1 && stripped[1] == ' ') {
                text.SetTo(stripped.String() + 2, stripped.Length() - 2);
            } else {
                text.SetTo(stripped.String() + 1, stripped.Length() - 1);
            }
            AppendParagraph(document, root, text, TextStyle(), "", 12.0f, false);
            continue;
        }

        // Regular paragraph.
        list = nullptr;
        AppendParagraph(document, root, stripped, TextStyle(), "", 12.0f, false);
    }

    if (root->GetChildCount() == 0) {
        ParagraphNode* paragraph = new ParagraphNode();
        paragraph->EnsureTextRun();
        root->AddChild(paragraph);
    }

    root->SetDocument(document);
    document->SetModified(false);
    document->UpdateStatistics();
    return true;
}

const char* MarkdownImporter::GetName() const
{
    return "Markdown";
}

const char* MarkdownImporter::GetMimeType() const
{
    return "text/markdown";
}

const char* MarkdownImporter::GetExtension() const
{
    return "md";
}

} // namespace YellowImportExport
