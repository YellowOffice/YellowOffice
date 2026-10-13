#ifndef YELLOW_IMPORT_EXPORT_PLAIN_TEXT_H
#define YELLOW_IMPORT_EXPORT_PLAIN_TEXT_H

#include <YellowImportExport/Importer.h>
#include <YellowImportExport/Exporter.h>

namespace YellowImportExport {

/**
 * PlainTextImporter - reads UTF-8 text files; blank-line-separated
 * paragraphs, single newlines inside a paragraph are preserved as line
 * breaks within the paragraph text (rendered as separate paragraphs on
 * round-trip for simplicity).
 */
class PlainTextImporter : public Importer {
public:
    PlainTextImporter();
    ~PlainTextImporter() override;

    bool Import(YellowDocumentCore::YellowDocument* document, const BString& filename) override;
    bool ImportFromData(YellowDocumentCore::YellowDocument* document, const char* data, size_t size) override;
    const char* GetName() const override;
    const char* GetMimeType() const override;
    const char* GetExtension() const override;
};

/**
 * PlainTextExporter - writes the document's text content as UTF-8,
 * one paragraph per line.
 */
class PlainTextExporter : public Exporter {
public:
    PlainTextExporter();
    ~PlainTextExporter() override;

    bool Export(const YellowDocumentCore::YellowDocument* document, const BString& filename) override;
    bool ExportToData(const YellowDocumentCore::YellowDocument* document, char** data, size_t* size) override;
    const char* GetName() const override;
    const char* GetMimeType() const override;
    const char* GetExtension() const override;
};

} // namespace YellowImportExport

#endif
