#ifndef YELLOW_IMPORT_EXPORT_YDF_IMPORTER_H
#define YELLOW_IMPORT_EXPORT_YDF_IMPORTER_H

#include <YellowImportExport/Importer.h>

namespace YellowImportExport {

class YDFImporter : public Importer {
public:
    YDFImporter();
    virtual ~YDFImporter();

    // Importer interface
    bool Import(YellowDocumentCore::YellowDocument* document, const BString& filename) override;
    bool ImportFromData(YellowDocumentCore::YellowDocument* document, const char* data, size_t size) override;
    const char* GetName() const override;
    const char* GetMimeType() const override;
    const char* GetExtension() const override;

private:
};

} // namespace YellowImportExport

#endif // YELLOW_IMPORT_EXPORT_YDF_IMPORTER_H
