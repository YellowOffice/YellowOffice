#include <YellowImportExport/FormatRegistry.h>
#include <YellowImportExport/DocumentFormat.h>
#include <YellowCommon/Defines.h>

#include <string.h>

namespace YellowImportExport {

FormatRegistry::FormatRegistry()
{
}

FormatRegistry::~FormatRegistry()
{
    for (const DocumentFormat* format : fFormats) {
        delete format;
    }
    fFormats.clear();
}

FormatRegistry* FormatRegistry::CreateDefault()
{
    FormatRegistry* registry = new FormatRegistry();

    DocumentFormat* ydf = new DocumentFormat(
        YDF_FORMAT_NAME, YDF_MIME_TYPE, YDF_FORMAT_EXTENSION, true, true);
    ydf->description = "YellowDocs native document";
    ydf->supportsStyles = true;
    ydf->supportsTables = true;
    ydf->supportsImages = true;
    ydf->priority = 100;
    registry->RegisterFormat(ydf);

    DocumentFormat* markdown = new DocumentFormat(
        "Markdown", "text/markdown", "md", true, true);
    markdown->description = "Markdown text";
    markdown->priority = 50;
    registry->RegisterFormat(markdown);

    DocumentFormat* plain = new DocumentFormat(
        "Plain Text", "text/plain", "txt", true, true);
    plain->description = "Plain UTF-8 text";
    plain->priority = 10;
    registry->RegisterFormat(plain);

    DocumentFormat* odf = new DocumentFormat(
        ODF_FORMAT_NAME, ODF_MIME_TYPE, ODF_FORMAT_EXTENSION, true, true);
    odf->description = "OpenDocument Text";
    odf->supportsStyles = true;
    odf->supportsTables = true;
    odf->priority = 60;
    registry->RegisterFormat(odf);

    return registry;
}

void FormatRegistry::RegisterFormat(const DocumentFormat* format)
{
    if (!format) return;
    // Replace an existing format with the same MIME type.
    for (size_t i = 0; i < fFormats.size(); ++i) {
        if (fFormats[i] && fFormats[i]->mimeType == format->mimeType) {
            delete fFormats[i];
            fFormats[i] = format;
            return;
        }
    }
    fFormats.push_back(format);
}

void FormatRegistry::UnregisterFormat(const char* mimeType)
{
    if (!mimeType) return;
    for (size_t i = 0; i < fFormats.size(); ++i) {
        if (fFormats[i] && fFormats[i]->mimeType == mimeType) {
            delete fFormats[i];
            fFormats.erase(fFormats.begin() + i);
            return;
        }
    }
}

const DocumentFormat* FormatRegistry::GetFormatByMimeType(const BString& mimeType) const
{
    for (const DocumentFormat* format : fFormats) {
        if (format && format->mimeType == mimeType) return format;
    }
    return nullptr;
}

const DocumentFormat* FormatRegistry::GetFormatByExtension(const BString& extension) const
{
    for (const DocumentFormat* format : fFormats) {
        if (format && format->MatchesExtension(extension)) return format;
    }
    return nullptr;
}

void FormatRegistry::GetAllFormats(std::vector<const DocumentFormat*>& formats) const
{
    formats = fFormats;
}

void FormatRegistry::GetImporters(std::vector<const DocumentFormat*>& formats) const
{
    for (const DocumentFormat* format : fFormats) {
        if (format && format->canImport) formats.push_back(format);
    }
}

void FormatRegistry::GetExporters(std::vector<const DocumentFormat*>& formats) const
{
    for (const DocumentFormat* format : fFormats) {
        if (format && format->canExport) formats.push_back(format);
    }
}

} // namespace YellowImportExport
