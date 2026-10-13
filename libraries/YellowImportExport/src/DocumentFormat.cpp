#include <YellowImportExport/DocumentFormat.h>

namespace YellowImportExport {

DocumentFormat::DocumentFormat()
    : canImport(false),
      canExport(false),
      supportsStyles(false),
      supportsTables(false),
      supportsImages(false),
      priority(0)
{
}

DocumentFormat::DocumentFormat(const char* name, const char* mimeType,
                               const char* extension, bool canImport, bool canExport)
    : name(name ? name : ""),
      mimeType(mimeType ? mimeType : ""),
      extension(extension ? extension : ""),
      canImport(canImport),
      canExport(canExport),
      supportsStyles(false),
      supportsTables(false),
      supportsImages(false),
      priority(0)
{
}

DocumentFormat::~DocumentFormat()
{
}

bool DocumentFormat::MatchesFile(const BString& filename) const
{
    if (extension.IsEmpty()) return false;
    if (filename.Length() <= extension.Length() + 1) return false;
    if (filename[filename.Length() - extension.Length() - 1] != '.') return false;
    return filename.ICompare(BString(".") + extension) == 0;
}

bool DocumentFormat::MatchesExtension(const BString& other) const
{
    return extension.ICompare(other) == 0;
}

} // namespace YellowImportExport
