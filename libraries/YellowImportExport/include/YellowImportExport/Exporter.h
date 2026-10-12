#ifndef YELLOW_IMPORT_EXPORT_EXPORTER_H
#define YELLOW_IMPORT_EXPORTER_H

#include <SupportDefs.h>
#include <String.h>

namespace YellowDocumentCore {
    class YellowDocument;
}

namespace YellowImportExport {

class Exporter {
public:
    Exporter();
    virtual ~Exporter();

    // Export document to file
    virtual bool Export(const YellowDocumentCore::YellowDocument* document, const BString& filename) = 0;
    
    // Export to data
    virtual bool ExportToData(const YellowDocumentCore::YellowDocument* document, char** data, size_t* size) = 0;
    
    // Get exporter name
    virtual const char* GetName() const = 0;
    virtual const char* GetMimeType() const = 0;
    virtual const char* GetExtension() const = 0;
};

} // namespace YellowImportExport

#endif // YELLOW_IMPORT_EXPORT_EXPORTER_H
