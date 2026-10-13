#include <YellowImportExport/PlainText.h>
#include <YellowDocumentCore/YellowDocument.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace YellowImportExport {

using namespace YellowDocumentCore;

// ----------------------------------------------------------- PlainTextImporter

PlainTextImporter::PlainTextImporter()
{
}

PlainTextImporter::~PlainTextImporter()
{
}

bool PlainTextImporter::Import(YellowDocument* document, const BString& filename)
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

bool PlainTextImporter::ImportFromData(YellowDocument* document, const char* data, size_t size)
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
    while (pos < content.Length()) {
        int32 lineEnd = pos;
        while (lineEnd < content.Length() && content[lineEnd] != '\n') ++lineEnd;
        BString line(content.String() + pos, lineEnd - pos);
        if (line.Length() > 0 && line[line.Length() - 1] == '\r') {
            line.Truncate(line.Length() - 1);
        }
        pos = lineEnd + 1;

        ParagraphNode* paragraph = new ParagraphNode();
        if (line.Length() > 0) {
            paragraph->SetText(line);
        } else {
            paragraph->EnsureTextRun();
        }
        root->AddChild(paragraph);
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

const char* PlainTextImporter::GetName() const
{
    return "Plain Text";
}

const char* PlainTextImporter::GetMimeType() const
{
    return "text/plain";
}

const char* PlainTextImporter::GetExtension() const
{
    return "txt";
}

// ----------------------------------------------------------- PlainTextExporter

PlainTextExporter::PlainTextExporter()
{
}

PlainTextExporter::~PlainTextExporter()
{
}

bool PlainTextExporter::Export(const YellowDocument* document, const BString& filename)
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

bool PlainTextExporter::ExportToData(const YellowDocument* document, char** data, size_t* size)
{
    if (!document || !data || !size) return false;

    BString out;
    struct Collector {
        static void Collect(const DocumentNode* node, BString& out) {
            if (!node) return;
            if (node->GetType() == NODE_TYPE_PARAGRAPH) {
                out += node->GetText();
                out += "\n";
                return;
            }
            if (node->GetType() == NODE_TYPE_TABLE) {
                for (size_t r = 0; r < node->GetChildCount(); ++r) {
                    DocumentNode* row = node->GetChild(r);
                    if (!row) continue;
                    for (size_t c = 0; c < row->GetChildCount(); ++c) {
                        DocumentNode* cell = row->GetChild(c);
                        if (!cell) continue;
                        if (c > 0) out += "\t";
                        out += cell->GetText();
                    }
                    out += "\n";
                }
                return;
            }
            for (size_t i = 0; i < node->GetChildCount(); ++i) {
                Collect(node->GetChild(i), out);
            }
        }
    };
    Collector::Collect(document->GetRoot(), out);

    *data = (char*)malloc((size_t)out.Length() + 1);
    if (!*data) return false;
    memcpy(*data, out.String(), (size_t)out.Length() + 1);
    *size = (size_t)out.Length();
    return true;
}

const char* PlainTextExporter::GetName() const
{
    return "Plain Text";
}

const char* PlainTextExporter::GetMimeType() const
{
    return "text/plain";
}

const char* PlainTextExporter::GetExtension() const
{
    return "txt";
}

} // namespace YellowImportExport
