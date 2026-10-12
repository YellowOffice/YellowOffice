#ifndef YELLOW_IMPORT_EXPORT_DOCUMENT_FORMAT_H
#define YELLOW_IMPORT_EXPORT_DOCUMENT_FORMAT_H

#include <SupportDefs.h>
#include <String.h>

namespace YellowImportExport {

class DocumentFormat {
public:
    DocumentFormat();
    DocumentFormat(const char* name, const char* mimeType, const char* extension, 
                   bool canImport, bool canExport);
    ~DocumentFormat();

    // Format information
    BString name;
    BString mimeType;
    BString extension;
    BString description;
    
    // Capabilities
    bool canImport;
    bool canExport;
    bool supportsStyles;
    bool supportsTables;
    bool supportsImages;
    
    // Priority (higher = preferred)
    int32 priority;

    // Check if format matches file
    bool MatchesFile(const BString& filename) const;
    bool MatchesExtension(const BString& extension) const;
};

} // namespace YellowImportExport

#endif // YELLOW_IMPORT_EXPORT_DOCUMENT_FORMAT_H
