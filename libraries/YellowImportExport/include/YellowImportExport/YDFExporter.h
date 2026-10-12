#ifndef YELLOW_IMPORT_EXPORT_YDF_EXPORTER_H
#define YELLOW_IMPORT_EXPORT_YDF_EXPORTER_H

#include <YellowImportExport/Exporter.h>

namespace YellowImportExport {

class YDFExporter : public Exporter {
public:
    YDFExporter();
    virtual ~YDFExporter();

    // Exporter interface
    bool Export(const YellowDocumentCore::YellowDocument* document, const BString& filename) override;
    bool ExportToData(const YellowDocumentCore::YellowDocument* document, char** data, size_t* size) override;
    const char* GetName() const override;
    const char* GetMimeType() const override;
    const char* GetExtension() const override;

private:
};

} // namespace YellowImportExport

#endif // YELLOW_IMPORT_EXPORT_YDF_EXPORTER_H
