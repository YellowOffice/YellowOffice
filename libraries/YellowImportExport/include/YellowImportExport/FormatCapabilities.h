#ifndef YELLOW_IMPORT_EXPORT_FORMAT_CAPABILITIES_H
#define YELLOW_IMPORT_EXPORT_FORMAT_CAPABILITIES_H

#include <SupportDefs.h>

namespace YellowImportExport {

class FormatCapabilities {
public:
    FormatCapabilities();
    ~FormatCapabilities();

    // Feature flags
    bool supportsText;
    bool supportsFormatting;
    bool supportsStyles;
    bool supportsTables;
    bool supportsImages;
    bool supportsEmbeddedObjects;
    bool supportsHeadersFooters;
    bool supportsPageLayout;
    bool supportsMetadata;
    bool supportsComments;
    bool supportsTrackChanges;

    // Check if format supports a feature
    bool SupportsFeature(uint32 feature) const;
};

} // namespace YellowImportExport

#endif // YELLOW_IMPORT_EXPORT_FORMAT_CAPABILITIES_H
