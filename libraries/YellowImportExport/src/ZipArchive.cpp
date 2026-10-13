#include <YellowImportExport/ZipArchive.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

namespace YellowImportExport {

// ---------------------------------------------------------------- helpers

namespace {

void Put16(std::vector<uint8_t>& out, uint16 value)
{
    out.push_back((uint8)(value & 0xff));
    out.push_back((uint8)((value >> 8) & 0xff));
}

void Put32(std::vector<uint8_t>& out, uint32 value)
{
    out.push_back((uint8)(value & 0xff));
    out.push_back((uint8)((value >> 8) & 0xff));
    out.push_back((uint8)((value >> 16) & 0xff));
    out.push_back((uint8)((value >> 24) & 0xff));
}

uint16 Get16(const uint8* data)
{
    return (uint16)(data[0] | (data[1] << 8));
}

uint32 Get32(const uint8* data)
{
    return (uint32)data[0] | ((uint32)data[1] << 8) | ((uint32)data[2] << 16) |
           ((uint32)data[3] << 24);
}

uint32 Crc32Of(const void* data, size_t size)
{
    return (uint32)crc32(0L, (const Bytef*)data, (uInt)size);
}

bool DeflateRaw(const std::vector<uint8_t>& input, std::vector<uint8_t>& output)
{
    z_stream stream;
    memset(&stream, 0, sizeof(stream));
    if (deflateInit2(&stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, -15, 8,
                     Z_DEFAULT_STRATEGY) != Z_OK) {
        return false;
    }
    output.resize(compressBound((uLong)input.size()) + 64);
    stream.next_in = (Bytef*)input.data();
    stream.avail_in = (uInt)input.size();
    stream.next_out = output.data();
    stream.avail_out = (uInt)output.size();
    int result = deflate(&stream, Z_FINISH);
    if (result != Z_STREAM_END) {
        deflateEnd(&stream);
        return false;
    }
    output.resize(stream.total_out);
    deflateEnd(&stream);
    return true;
}

bool InflateRaw(const uint8* input, size_t inputSize, std::vector<uint8_t>& output,
                size_t expectedSize)
{
    z_stream stream;
    memset(&stream, 0, sizeof(stream));
    if (inflateInit2(&stream, -15) != Z_OK) {
        return false;
    }
    output.resize(expectedSize > 0 ? expectedSize : inputSize * 4 + 64);
    stream.next_in = (Bytef*)input;
    stream.avail_in = (uInt)inputSize;
    stream.next_out = output.data();
    stream.avail_out = (uInt)output.size();

    while (true) {
        int result = inflate(&stream, Z_NO_FLUSH);
        if (result == Z_STREAM_END) {
            output.resize(stream.total_out);
            inflateEnd(&stream);
            return true;
        }
        if (result != Z_OK) {
            inflateEnd(&stream);
            return false;
        }
        if (stream.avail_out == 0) {
            size_t oldSize = output.size();
            output.resize(oldSize * 2);
            stream.next_out = output.data() + oldSize;
            stream.avail_out = (uInt)oldSize;
        }
    }
}

const uint32 kLocalHeaderSignature = 0x04034b50;
const uint32 kCentralDirSignature = 0x02014b50;
const uint32 kEocdSignature = 0x06054b50;

} // namespace

// ---------------------------------------------------------------- ZipWriter

ZipWriter::ZipWriter()
{
}

ZipWriter::~ZipWriter()
{
}

void ZipWriter::AddFile(const BString& name, const void* data, size_t size, bool useDeflate)
{
    Entry entry;
    entry.name = name;
    if (data && size > 0) {
        entry.data.assign((const uint8*)data, (const uint8*)data + size);
    }
    entry.crc = Crc32Of(entry.data.data(), entry.data.size());
    entry.uncompressedSize = (uint32)entry.data.size();
    entry.deflate = useDeflate && size > 0;

    if (entry.deflate) {
        if (!DeflateRaw(entry.data, entry.compressed) ||
            entry.compressed.size() >= entry.data.size()) {
            // Fall back to store when deflate does not help.
            entry.deflate = false;
            entry.compressed = entry.data;
        }
    } else {
        entry.compressed = entry.data;
    }
    entry.compressedSize = (uint32)entry.compressed.size();
    entry.offset = 0;
    fFiles.push_back(entry);
}

void ZipWriter::AddFile(const BString& name, const BString& text, bool useDeflate)
{
    AddFile(name, text.String(), (size_t)text.Length(), useDeflate);
}

bool ZipWriter::WriteToMemory(std::vector<uint8_t>& out) const
{
    out.clear();

    // Local file headers + data.
    std::vector<Entry> files = fFiles; // copy to fill offsets
    for (Entry& entry : files) {
        entry.offset = (uint32)out.size();
        Put32(out, kLocalHeaderSignature);
        Put16(out, 20);           // version needed
        Put16(out, 0);            // flags
        Put16(out, entry.deflate ? 8 : 0);
        Put16(out, 0);            // mod time
        Put16(out, 0);            // mod date
        Put32(out, entry.crc);
        Put32(out, entry.compressedSize);
        Put32(out, entry.uncompressedSize);
        Put16(out, (uint16)entry.name.Length());
        Put16(out, 0);            // extra length
        out.insert(out.end(), entry.name.String(),
                   entry.name.String() + entry.name.Length());
        out.insert(out.end(), entry.compressed.begin(), entry.compressed.end());
    }

    // Central directory.
    uint32 centralOffset = (uint32)out.size();
    for (const Entry& entry : files) {
        Put32(out, kCentralDirSignature);
        Put16(out, 20);           // version made by
        Put16(out, 20);           // version needed
        Put16(out, 0);            // flags
        Put16(out, entry.deflate ? 8 : 0);
        Put16(out, 0);            // time
        Put16(out, 0);            // date
        Put32(out, entry.crc);
        Put32(out, entry.compressedSize);
        Put32(out, entry.uncompressedSize);
        Put16(out, (uint16)entry.name.Length());
        Put16(out, 0);            // extra
        Put16(out, 0);            // comment
        Put16(out, 0);            // disk start
        Put16(out, 0);            // internal attrs
        Put32(out, 0);            // external attrs
        Put32(out, entry.offset);
        out.insert(out.end(), entry.name.String(),
                   entry.name.String() + entry.name.Length());
    }
    uint32 centralSize = (uint32)out.size() - centralOffset;

    // End of central directory.
    Put32(out, kEocdSignature);
    Put16(out, 0); // this disk
    Put16(out, 0); // disk with central dir
    Put16(out, (uint16)files.size());
    Put16(out, (uint16)files.size());
    Put32(out, centralSize);
    Put32(out, centralOffset);
    Put16(out, 0); // comment length
    return true;
}

bool ZipWriter::Save(const BString& path) const
{
    std::vector<uint8_t> data;
    if (!WriteToMemory(data)) return false;

    FILE* file = fopen(path.String(), "wb");
    if (!file) return false;
    size_t written = fwrite(data.data(), 1, data.size(), file);
    fclose(file);
    return written == data.size();
}

// ---------------------------------------------------------------- ZipReader

ZipReader::ZipReader()
{
}

ZipReader::~ZipReader()
{
}

bool ZipReader::Open(const BString& path)
{
    FILE* file = fopen(path.String(), "rb");
    if (!file) return false;
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    if (size < 22) {
        fclose(file);
        return false;
    }
    fData.resize((size_t)size);
    size_t read = fread(fData.data(), 1, (size_t)size, file);
    fclose(file);
    if (read != (size_t)size) return false;
    return ParseCentralDirectory();
}

bool ZipReader::OpenFromMemory(const void* data, size_t size)
{
    if (!data || size < 22) return false;
    const uint8* bytes = (const uint8*)data;
    fData.assign(bytes, bytes + size);
    return ParseCentralDirectory();
}

bool ZipReader::ParseCentralDirectory()
{
    fCentral.clear();
    fEntries.clear();
    if (fData.size() < 22) return false;

    // Locate EOCD by scanning backwards (comment up to 65535 bytes).
    size_t searchStart = fData.size() >= 22 + 65535 ? fData.size() - 22 - 65535 : 0;
    size_t eocd = (size_t)-1;
    for (size_t i = fData.size() - 22 + 1; i-- > searchStart; ) {
        if (Get32(&fData[i]) == kEocdSignature) {
            eocd = i;
            break;
        }
    }
    if (eocd == (size_t)-1) return false;

    uint16 entryCount = Get16(&fData[eocd + 10]);
    uint32 centralSize = Get32(&fData[eocd + 12]);
    uint32 centralOffset = Get32(&fData[eocd + 16]);
    if ((size_t)centralOffset + centralSize > fData.size()) return false;

    size_t pos = centralOffset;
    for (uint16 i = 0; i < entryCount; ++i) {
        if (pos + 46 > fData.size()) return false;
        if (Get32(&fData[pos]) != kCentralDirSignature) return false;

        CentralEntry entry;
        entry.method = Get16(&fData[pos + 10]);
        entry.crc = Get32(&fData[pos + 16]);
        entry.compressedSize = Get32(&fData[pos + 20]);
        entry.uncompressedSize = Get32(&fData[pos + 24]);
        uint16 nameLength = Get16(&fData[pos + 28]);
        uint16 extraLength = Get16(&fData[pos + 30]);
        uint16 commentLength = Get16(&fData[pos + 32]);
        entry.localHeaderOffset = Get32(&fData[pos + 42]);
        if (pos + 46 + nameLength > fData.size()) return false;
        entry.name.SetTo((const char*)&fData[pos + 46], nameLength);

        fEntries[entry.name] = fCentral.size();
        fCentral.push_back(entry);
        pos += 46 + nameLength + extraLength + commentLength;
    }
    return true;
}

bool ZipReader::HasFile(const BString& name) const
{
    return fEntries.find(name) != fEntries.end();
}

bool ZipReader::ReadFile(const BString& name, std::vector<uint8_t>& out) const
{
    auto it = fEntries.find(name);
    if (it == fEntries.end()) return false;
    const CentralEntry& entry = fCentral[it->second];

    size_t pos = entry.localHeaderOffset;
    if (pos + 30 > fData.size()) return false;
    if (Get32(&fData[pos]) != kLocalHeaderSignature) return false;

    uint16 nameLength = Get16(&fData[pos + 26]);
    uint16 extraLength = Get16(&fData[pos + 28]);
    size_t dataStart = pos + 30 + nameLength + extraLength;
    if (dataStart + entry.compressedSize > fData.size()) return false;

    if (entry.method == 0) {
        out.assign(fData.begin() + dataStart,
                   fData.begin() + dataStart + entry.compressedSize);
        return true;
    }
    if (entry.method == 8) {
        return InflateRaw(&fData[dataStart], entry.compressedSize, out,
                          entry.uncompressedSize);
    }
    return false;
}

bool ZipReader::ReadFileText(const BString& name, BString& out) const
{
    std::vector<uint8_t> data;
    if (!ReadFile(name, data)) return false;
    out.SetTo((const char*)data.data(), (int32)data.size());
    return true;
}

void ZipReader::GetFileNames(std::vector<BString>& out) const
{
    out.clear();
    for (const CentralEntry& entry : fCentral) {
        out.push_back(entry.name);
    }
}

} // namespace YellowImportExport
