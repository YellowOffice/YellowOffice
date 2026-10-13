#ifndef YELLOW_IMPORT_EXPORT_FORMAT_CAPABILITIES_H
#define YELLOW_IMPORT_EXPORT_FORMAT_CAPABILITIES_H

#include <SupportDefs.h>

namespace YellowImportExport {

enum FormatFeature {
    FEATURE_TEXT = 0,
    FEATURE_FORMATTING,
    FEATURE_STYLES,
    FEATURE_TABLES,
    FEATURE_IMAGES,
    FEATURE_EMBEDDED_OBJECTS,
    FEATURE_HEADERS_FOOTERS,
    FEATURE_PAGE_LAYOUT,
    FEATURE_METADATA,
    FEATURE_COMMENTS,
    FEATURE_TRACK_CHANGES,
    FEATURE_FEATURE_COUNT
};

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
    void SetFeature(uint32 feature, bool supported);
};

} // namespace YellowImportExport

#endif // YELLOW_IMPORT_EXPORT_FORMAT_CAPABILITIES_H
