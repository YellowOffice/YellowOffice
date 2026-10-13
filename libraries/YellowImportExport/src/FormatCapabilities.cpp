#include <YellowImportExport/FormatCapabilities.h>

namespace YellowImportExport {

FormatCapabilities::FormatCapabilities()
    : supportsText(true),
      supportsFormatting(false),
      supportsStyles(false),
      supportsTables(false),
      supportsImages(false),
      supportsEmbeddedObjects(false),
      supportsHeadersFooters(false),
      supportsPageLayout(false),
      supportsMetadata(false),
      supportsComments(false),
      supportsTrackChanges(false)
{
}

FormatCapabilities::~FormatCapabilities()
{
}

bool FormatCapabilities::SupportsFeature(uint32 feature) const
{
    switch (feature) {
        case FEATURE_TEXT: return supportsText;
        case FEATURE_FORMATTING: return supportsFormatting;
        case FEATURE_STYLES: return supportsStyles;
        case FEATURE_TABLES: return supportsTables;
        case FEATURE_IMAGES: return supportsImages;
        case FEATURE_EMBEDDED_OBJECTS: return supportsEmbeddedObjects;
        case FEATURE_HEADERS_FOOTERS: return supportsHeadersFooters;
        case FEATURE_PAGE_LAYOUT: return supportsPageLayout;
        case FEATURE_METADATA: return supportsMetadata;
        case FEATURE_COMMENTS: return supportsComments;
        case FEATURE_TRACK_CHANGES: return supportsTrackChanges;
        default: return false;
    }
}

void FormatCapabilities::SetFeature(uint32 feature, bool supported)
{
    switch (feature) {
        case FEATURE_TEXT: supportsText = supported; break;
        case FEATURE_FORMATTING: supportsFormatting = supported; break;
        case FEATURE_STYLES: supportsStyles = supported; break;
        case FEATURE_TABLES: supportsTables = supported; break;
        case FEATURE_IMAGES: supportsImages = supported; break;
        case FEATURE_EMBEDDED_OBJECTS: supportsEmbeddedObjects = supported; break;
        case FEATURE_HEADERS_FOOTERS: supportsHeadersFooters = supported; break;
        case FEATURE_PAGE_LAYOUT: supportsPageLayout = supported; break;
        case FEATURE_METADATA: supportsMetadata = supported; break;
        case FEATURE_COMMENTS: supportsComments = supported; break;
        case FEATURE_TRACK_CHANGES: supportsTrackChanges = supported; break;
        default: break;
    }
}

} // namespace YellowImportExport
