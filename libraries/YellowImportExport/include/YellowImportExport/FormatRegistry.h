#ifndef YELLOW_IMPORT_EXPORT_FORMAT_REGISTRY_H
#define YELLOW_IMPORT_EXPORT_FORMAT_REGISTRY_H

#include <SupportDefs.h>
#include <String.h>
#include <vector>

namespace YellowImportExport {

class DocumentFormat;
class Importer;
class Exporter;

class FormatRegistry {
public:
    FormatRegistry();
    ~FormatRegistry();

    // Register formats
    void RegisterFormat(const DocumentFormat* format);
    void UnregisterFormat(const char* mimeType);
    
    // Get format by MIME type
    const DocumentFormat* GetFormatByMimeType(const BString& mimeType) const;
    const DocumentFormat* GetFormatByExtension(const BString& extension) const;
    
    // Get all formats
    void GetAllFormats(std::vector<const DocumentFormat*>& formats) const;
    void GetImporters(std::vector<const DocumentFormat*>& importers) const;
    void GetExporters(std::vector<const DocumentFormat*>& exporters) const;

private:
    std::vector<const DocumentFormat*> fFormats;
};

} // namespace YellowImportExport

#endif // YELLOW_IMPORT_EXPORT_FORMAT_REGISTRY_H
