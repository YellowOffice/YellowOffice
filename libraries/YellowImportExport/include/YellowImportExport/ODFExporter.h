#ifndef YELLOW_IMPORT_EXPORT_ODF_EXPORTER_H
#define YELLOW_IMPORT_EXPORT_ODF_EXPORTER_H

#include <YellowImportExport/Exporter.h>

namespace YellowImportExport {

class ODFExporter : public Exporter {
public:
    ODFExporter();
    virtual ~ODFExporter();

    // Exporter interface
    bool Export(const YellowDocumentCore::YellowDocument* document, const BString& filename) override;
    bool ExportToData(const YellowDocumentCore::YellowDocument* document, char** data, size_t* size) override;
    const char* GetName() const override;
    const char* GetMimeType() const override;
    const char* GetExtension() const override;

private:
};

} // namespace YellowImportExport

#endif // YELLOW_IMPORT_EXPORT_ODF_EXPORTER_H
