#include <YellowImportExport/YDFExporter.h>
#include <YellowImportExport/DocumentSerializer.h>
#include <YellowImportExport/ZipArchive.h>
#include <YellowCommon/Defines.h>

#include <stdio.h>

namespace YellowImportExport {

using namespace YellowCommon;
using namespace YellowDocumentCore;

YDFExporter::YDFExporter()
{
}

YDFExporter::~YDFExporter()
{
}

bool YDFExporter::Export(const YellowDocument* document, const BString& filename)
{
    if (!document) return false;

    JsonValue json = DocumentSerializer::DocumentToJson(document);
    BString documentJson = json.Serialize(true);

    ZipWriter zip;
    zip.AddFile("mimetype", BString(YDF_MIME_TYPE), false); // stored, first entry
    zip.AddFile("document.json", documentJson, true);
    JsonValue manifest;
    manifest.Set("application", JsonValue(YELLOWDOCS_APP_SIGNATURE));
    manifest.Set("format", JsonValue("ydf"));
    manifest.Set("version", JsonValue((int32)1));
    zip.AddFile("manifest.json", manifest.Serialize(true), true);

    return zip.Save(filename);
}

bool YDFExporter::ExportToData(const YellowDocument* document, char** data, size_t* size)
{
    if (!document || !data || !size) return false;

    JsonValue json = DocumentSerializer::DocumentToJson(document);
    BString documentJson = json.Serialize(true);

    ZipWriter zip;
    zip.AddFile("mimetype", BString(YDF_MIME_TYPE), false);
    zip.AddFile("document.json", documentJson, true);
    JsonValue manifest;
    manifest.Set("application", JsonValue(YELLOWDOCS_APP_SIGNATURE));
    manifest.Set("format", JsonValue("ydf"));
    manifest.Set("version", JsonValue((int32)1));
    zip.AddFile("manifest.json", manifest.Serialize(true), true);

    std::vector<uint8_t> buffer;
    if (!zip.WriteToMemory(buffer)) return false;

    *data = (char*)malloc(buffer.size() + 1);
    if (!*data) return false;
    memcpy(*data, buffer.data(), buffer.size());
    (*data)[buffer.size()] = '\0';
    *size = buffer.size();
    return true;
}

const char* YDFExporter::GetName() const
{
    return "Yellow Document Format";
}

const char* YDFExporter::GetMimeType() const
{
    return YDF_MIME_TYPE;
}

const char* YDFExporter::GetExtension() const
{
    return YDF_FORMAT_EXTENSION;
}

} // namespace YellowImportExport
