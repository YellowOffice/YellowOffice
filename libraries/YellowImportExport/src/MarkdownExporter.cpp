#include <YellowImportExport/MarkdownExporter.h>
#include <YellowDocumentCore/YellowDocument.h>
#include <YellowDocumentCore/Nodes.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace YellowImportExport {

using namespace YellowDocumentCore;

MarkdownExporter::MarkdownExporter()
{
}

MarkdownExporter::~MarkdownExporter()
{
}

namespace {

void AppendRunMarkdown(BString& out, const TextRunNode* run)
{
    if (!run) return;
    BString text = run->text;
    // Escape markdown special characters conservatively.
    BString escaped;
    for (int32 i = 0; i < text.Length(); ++i) {
        char c = text[i];
        if (c == '*' || c == '_' || c == '#' || c == '[' || c == ']' ||
            c == '\\' || c == '`') {
            escaped += '\\';
        }
        escaped += c;
    }
    if (run->style.bold && run->style.italic) {
        out += "***";
        out += escaped;
        out += "***";
    } else if (run->style.bold) {
        out += "**";
        out += escaped;
        out += "**";
    } else if (run->style.italic) {
        out += '*';
        out += escaped;
        out += '*';
    } else {
        out += escaped;
    }
}

void AppendParagraphMarkdown(BString& out, const ParagraphNode* paragraph, int32 headingLevel)
{
    if (headingLevel > 0 && headingLevel <= 6) {
        for (int32 i = 0; i < headingLevel; ++i) out += '#';
        out += ' ';
    }
    for (size_t i = 0; i < paragraph->GetChildCount(); ++i) {
        DocumentNode* child = paragraph->GetChild(i);
        if (child && child->GetType() == NODE_TYPE_TEXT_RUN) {
            AppendRunMarkdown(out, static_cast<const TextRunNode*>(child));
        }
    }
    out += "\n\n";
}

int32 HeadingLevelFor(const ParagraphNode* paragraph)
{
    const BString& name = paragraph->paragraphStyleName;
    if (name.Length() >= 8 && strncmp(name.String(), "Heading ", 8) == 0) {
        int level = atoi(name.String() + 8);
        if (level >= 1 && level <= 6) return level;
    }
    if (paragraph->style.alignment == ParagraphStyle::ALIGN_LEFT &&
        paragraph->GetTextLength() > 0) {
        // Font-size based fallback.
        // (paragraph style doesn't carry font size; skip)
    }
    return 0;
}

void AppendNodeMarkdown(BString& out, const DocumentNode* node, int32 listDepth);

void AppendListMarkdown(BString& out, const ListNode* list, int32 depth)
{
    int32 index = 1;
    for (size_t i = 0; i < list->GetChildCount(); ++i) {
        DocumentNode* child = list->GetChild(i);
        if (!child || child->GetType() != NODE_TYPE_LIST_ITEM) continue;
        ListItemNode* item = static_cast<ListItemNode*>(child);
        for (int32 d = 0; d < depth; ++d) out += "  ";
        if (list->ordered) {
            char buffer[32];
            snprintf(buffer, sizeof(buffer), "%d. ", index);
            out += buffer;
        } else {
            out += "- ";
        }
        ++index;

        ParagraphNode* paragraph = item->GetContent();
        if (paragraph) {
            for (size_t r = 0; r < paragraph->GetChildCount(); ++r) {
                DocumentNode* run = paragraph->GetChild(r);
                if (run && run->GetType() == NODE_TYPE_TEXT_RUN) {
                    AppendRunMarkdown(out, static_cast<TextRunNode*>(run));
                }
            }
        }
        out += "\n";

        // Nested lists inside the item.
        for (size_t c = 0; c < item->GetChildCount(); ++c) {
            DocumentNode* nested = item->GetChild(c);
            if (nested && nested->GetType() == NODE_TYPE_LIST) {
                AppendListMarkdown(out, static_cast<ListNode*>(nested), depth + 1);
            }
        }
    }
    out += "\n";
}

void AppendTableMarkdown(BString& out, const TableNode* table)
{
    bool firstRow = true;
    for (size_t r = 0; r < table->GetChildCount(); ++r) {
        TableRowNode* row = dynamic_cast<TableRowNode*>(table->GetChild(r));
        if (!row) continue;
        out += '|';
        for (size_t c = 0; c < row->GetCellCount(); ++c) {
            TableCellNode* cell = dynamic_cast<TableCellNode*>(row->GetChild(c));
            BString text = cell ? cell->GetText() : BString();
            // Single-line cells only.
            BString single;
            for (int32 i = 0; i < text.Length(); ++i) {
                single += (text[i] == '\n') ? ' ' : text[i];
            }
            out += ' ';
            out += single;
            out += " |";
        }
        out += "\n";
        if (firstRow) {
            firstRow = false;
            out += '|';
            for (size_t c = 0; c < row->GetCellCount(); ++c) out += " --- |";
            out += "\n";
        }
    }
    out += "\n";
}

void AppendNodeMarkdown(BString& out, const DocumentNode* node, int32 listDepth)
{
    if (!node) return;
    switch (node->GetType()) {
        case NODE_TYPE_SECTION:
        case NODE_TYPE_LIST_ITEM:
            for (size_t i = 0; i < node->GetChildCount(); ++i) {
                AppendNodeMarkdown(out, node->GetChild(i), listDepth);
            }
            break;
        case NODE_TYPE_PARAGRAPH: {
            const ParagraphNode* paragraph = static_cast<const ParagraphNode*>(node);
            AppendParagraphMarkdown(out, paragraph, HeadingLevelFor(paragraph));
            break;
        }
        case NODE_TYPE_LIST:
            AppendListMarkdown(out, static_cast<const ListNode*>(node), listDepth);
            break;
        case NODE_TYPE_TABLE:
            AppendTableMarkdown(out, static_cast<const TableNode*>(node));
            break;
        case NODE_TYPE_PAGE_BREAK:
            out += "---\n\n";
            break;
        case NODE_TYPE_IMAGE: {
            const ImageNode* image = static_cast<const ImageNode*>(node);
            out += "![";
            out += image->altText;
            out += "](image)\n\n";
            break;
        }
        default:
            break;
    }
}

} // namespace

bool MarkdownExporter::Export(const YellowDocument* document, const BString& filename)
{
    if (!document) return false;
    char* data = nullptr;
    size_t size = 0;
    if (!ExportToData(document, &data, &size)) return false;
    FILE* file = fopen(filename.String(), "wb");
    if (!file) {
        free(data);
        return false;
    }
    size_t written = fwrite(data, 1, size, file);
    fclose(file);
    free(data);
    return written == size;
}

bool MarkdownExporter::ExportToData(const YellowDocument* document, char** data, size_t* size)
{
    if (!document || !data || !size) return false;

    BString out;
    if (document->GetMetadata() && document->GetMetadata()->title.Length() > 0) {
        out += "# ";
        out += document->GetMetadata()->title;
        out += "\n\n";
    }
    AppendNodeMarkdown(out, document->GetRoot(), 0);

    *data = (char*)malloc((size_t)out.Length() + 1);
    if (!*data) return false;
    memcpy(*data, out.String(), (size_t)out.Length() + 1);
    *size = (size_t)out.Length();
    return true;
}

const char* MarkdownExporter::GetName() const
{
    return "Markdown";
}

const char* MarkdownExporter::GetMimeType() const
{
    return "text/markdown";
}

const char* MarkdownExporter::GetExtension() const
{
    return "md";
}

} // namespace YellowImportExport
