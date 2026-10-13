#ifndef YELLOW_IMPORT_EXPORT_DOCUMENT_SERIALIZER_H
#define YELLOW_IMPORT_EXPORT_DOCUMENT_SERIALIZER_H

#include <SupportDefs.h>
#include <String.h>
#include <vector>
#include <YellowCommon/Json.h>
#include <YellowDocumentCore/YellowDocument.h>

namespace YellowImportExport {

/**
 * DocumentSerializer converts a YellowDocument tree to/from a JSON DOM
 * (the payload of the YDF container and used by tests).
 */
class DocumentSerializer {
public:
    static YellowCommon::JsonValue DocumentToJson(
        const YellowDocumentCore::YellowDocument* document);

    static bool DocumentFromJson(YellowDocumentCore::YellowDocument* document,
                                 const YellowCommon::JsonValue& json,
                                 BString* outError = nullptr);

    // Base64 helpers (for image payloads).
    static BString Base64Encode(const void* data, size_t size);
    static bool Base64Decode(const BString& text, std::vector<uint8_t>& out);
};

} // namespace YellowImportExport

#endif
