#ifndef YELLOW_IMPORT_EXPORT_ZIP_ARCHIVE_H
#define YELLOW_IMPORT_EXPORT_ZIP_ARCHIVE_H

#include <SupportDefs.h>
#include <String.h>
#include <vector>
#include <map>

namespace YellowImportExport {

/**
 * Minimal ZIP container reader/writer (subset of APPNOTE.TXT).
 * Writer supports store and deflate; reader supports both. Used by the
 * YDF and ODF containers.
 */
class ZipWriter {
public:
    ZipWriter();
    ~ZipWriter();

    /// Adds a file. useDeflate=false stores the entry uncompressed
    /// (required for the ODF "mimetype" first entry).
    void AddFile(const BString& name, const void* data, size_t size,
                 bool useDeflate = true);
    void AddFile(const BString& name, const BString& text, bool useDeflate = true);

    /// Serializes the archive into memory.
    bool WriteToMemory(std::vector<uint8_t>& out) const;
    bool Save(const BString& path) const;

    size_t CountFiles() const { return fFiles.size(); }

private:
    struct Entry {
        BString name;
        std::vector<uint8_t> data;       // uncompressed
        std::vector<uint8_t> compressed; // compressed payload
        uint32 crc;
        uint32 compressedSize;
        uint32 uncompressedSize;
        bool deflate;
        uint32 offset; // local header offset (filled during write)
    };

    std::vector<Entry> fFiles;
};

class ZipReader {
public:
    ZipReader();
    ~ZipReader();

    bool Open(const BString& path);
    bool OpenFromMemory(const void* data, size_t size);

    bool HasFile(const BString& name) const;
    bool ReadFile(const BString& name, std::vector<uint8_t>& out) const;
    bool ReadFileText(const BString& name, BString& out) const;
    void GetFileNames(std::vector<BString>& out) const;

private:
    bool ParseCentralDirectory();

    std::vector<uint8_t> fData;
    std::map<BString, size_t> fEntries; // name -> central directory index

    struct CentralEntry {
        BString name;
        uint32 crc;
        uint32 compressedSize;
        uint32 uncompressedSize;
        uint32 localHeaderOffset;
        uint16 method; // 0 store, 8 deflate
    };
    std::vector<CentralEntry> fCentral;
};

} // namespace YellowImportExport

#endif
