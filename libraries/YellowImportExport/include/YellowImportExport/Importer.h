#ifndef YELLOW_IMPORT_EXPORT_IMPORTER_H
#define YELLOW_IMPORT_EXPORT_IMPORTER_H

#include <SupportDefs.h>
#include <String.h>

namespace YellowDocumentCore {
    class YellowDocument;
}

namespace YellowImportExport {

class Importer {
public:
    Importer();
    virtual ~Importer();

    // Import document from file
    virtual bool Import(YellowDocumentCore::YellowDocument* document, const BString& filename) = 0;
    
    // Import from data
    virtual bool ImportFromData(YellowDocumentCore::YellowDocument* document, const char* data, size_t size) = 0;
    
    // Get importer name
    virtual const char* GetName() const = 0;
    virtual const char* GetMimeType() const = 0;
    virtual const char* GetExtension() const = 0;
};

} // namespace YellowImportExport

#endif // YELLOW_IMPORT_EXPORT_IMPORTER_H
