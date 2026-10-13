#include <YellowImportExport/YDFImporter.h>
#include <YellowImportExport/DocumentSerializer.h>
#include <YellowImportExport/ZipArchive.h>
#include <YellowCommon/Defines.h>
#include <YellowCommon/Logger.h>

#include <stdio.h>
#include <stdlib.h>

namespace YellowImportExport {

using namespace YellowCommon;
using namespace YellowDocumentCore;

YDFImporter::YDFImporter()
{
}

YDFImporter::~YDFImporter()
{
}

bool YDFImporter::Import(YellowDocument* document, const BString& filename)
{
    if (!document) return false;

    ZipReader zip;
    if (!zip.Open(filename)) {
        Logger::GetGlobalLogger().Error("YDF: cannot open %s", filename.String());
        return false;
    }

    BString documentJson;
    if (!zip.ReadFileText("document.json", documentJson)) {
        Logger::GetGlobalLogger().Error("YDF: missing document.json");
        return false;
    }
    return ImportFromData(document, documentJson.String(),
                          (size_t)documentJson.Length());
}

bool YDFImporter::ImportFromData(YellowDocument* document, const char* data, size_t size)
{
    if (!document || !data) return false;

    // The import path receives either raw document.json or a whole .ydf
    // container (detected by the ZIP local header signature).
    BString jsonText;
    if (size >= 4 && (unsigned char)data[0] == 0x50 && (unsigned char)data[1] == 0x4b) {
        ZipReader zip;
        if (!zip.OpenFromMemory(data, size)) return false;
        if (!zip.ReadFileText("document.json", jsonText)) return false;
    } else {
        jsonText.SetTo(data, (int32)size);
    }

    JsonValue json;
    BString error;
    if (!JsonValue::Parse(jsonText, &json, &error)) {
        Logger::GetGlobalLogger().Error("YDF: JSON parse error: %s", error.String());
        return false;
    }
    return DocumentSerializer::DocumentFromJson(document, json, &error);
}

const char* YDFImporter::GetName() const
{
    return "Yellow Document Format";
}

const char* YDFImporter::GetMimeType() const
{
    return YDF_MIME_TYPE;
}

const char* YDFImporter::GetExtension() const
{
    return YDF_FORMAT_EXTENSION;
}

} // namespace YellowImportExport
