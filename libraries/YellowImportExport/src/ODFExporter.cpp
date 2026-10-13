#include <YellowImportExport/ODFExporter.h>
#include <YellowImportExport/Xml.h>
#include <YellowImportExport/ZipArchive.h>
#include <YellowCommon/Defines.h>
#include <YellowDocumentCore/YellowDocument.h>
#include <YellowDocumentCore/Nodes.h>

#include <stdio.h>
#include <stdlib.h>

namespace YellowImportExport {

using namespace YellowDocumentCore;

ODFExporter::ODFExporter()
{
}

ODFExporter::~ODFExporter()
{
}

namespace {

BString StylesXml()
{
    BString xml;
    xml += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    xml += "<office:document-styles "
           "xmlns:office=\"urn:oasis:names:tc:opendocument:xmlns:office:1.0\" "
           "xmlns:style=\"urn:oasis:names:tc:opendocument:xmlns:style:1.0\" "
           "xmlns:text=\"urn:oasis:names:tc:opendocument:xmlns:text:1.0\" "
           "xmlns:fo=\"urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0\" "
           "office:version=\"1.2\">\n";
    xml += "  <office:styles>\n";
    xml += "    <style:default-style style:family=\"paragraph\">\n";
    xml += "      <style:text-properties fo:font-size=\"12pt\"/>\n";
    xml += "    </style:default-style>\n";
    xml += "    <style:style style:name=\"Standard\" style:family=\"paragraph\" "
           "style:class=\"text\"/>\n";
    xml += "  </office:styles>\n";
    xml += "</office:document-styles>\n";
    return xml;
}

BString MetaXml(const YellowDocument* document)
{
    const DocumentMetadata* metadata = document ? document->GetMetadata() : nullptr;
    BString xml;
    xml += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    xml += "<office:document-meta "
           "xmlns:office=\"urn:oasis:names:tc:opendocument:xmlns:office:1.0\" "
           "xmlns:meta=\"urn:oasis:names:tc:opendocument:xmlns:meta:1.0\" "
           "xmlns:dc=\"http://purl.org/dc/elements/1.1/\" "
           "office:version=\"1.2\">\n";
    xml += "  <office:meta>\n";
    if (metadata) {
        if (metadata->title.Length() > 0) {
            xml += "    <dc:title>";
            xml += XmlEscape(metadata->title);
            xml += "</dc:title>\n";
        }
        if (metadata->author.Length() > 0) {
            xml += "    <meta:initial-creator>";
            xml += XmlEscape(metadata->author);
            xml += "</meta:initial-creator>\n";
        }
        if (metadata->subject.Length() > 0) {
            xml += "    <dc:subject>";
            xml += XmlEscape(metadata->subject);
            xml += "</dc:subject>\n";
        }
    }
    xml += "  </office:meta>\n";
    xml += "</office:document-meta>\n";
    return xml;
}

void AppendTextProperties(BString& xml, const TextStyle& style)
{
    if (style.bold) xml += " fo:font-weight=\"bold\"";
    if (style.italic) xml += " fo:font-style=\"italic\"";
    if (style.underline) xml += " style:text-underline-style=\"solid\"";
    if (style.fontSize > 0.0f) {
        char buffer[32];
        snprintf(buffer, sizeof(buffer), " fo:font-size=\"%.1fpt\"", style.fontSize);
        xml += buffer;
    }
    if (style.fontFamily.Length() > 0) {
        xml += " fo:font-family=\"";
        xml += XmlEscape(style.fontFamily);
        xml += "\"";
    }
}

void AppendParagraphContent(BString& xml, const ParagraphNode* paragraph,
                            int32& styleIndex)
{
    for (size_t i = 0; i < paragraph->GetChildCount(); ++i) {
        DocumentNode* child = paragraph->GetChild(i);
        if (!child || child->GetType() != NODE_TYPE_TEXT_RUN) continue;
        const TextRunNode* run = static_cast<const TextRunNode*>(child);
        if (run->text.IsEmpty()) continue;

        if (run->style.HasFormatting()) {
            xml += "<text:span text:style-name=\"T";
            char buffer[16];
            snprintf(buffer, sizeof(buffer), "%d", styleIndex++);
            xml += buffer;
            xml += "\">";
            xml += XmlEscape(run->text);
            xml += "</text:span>";
        } else {
            xml += XmlEscape(run->text);
        }
    }
    if (paragraph->GetChildCount() == 0) {
        xml += "<text:s/>";
    }
}

void CollectSpanStyles(BString& xml, const ParagraphNode* paragraph,
                       int32& styleIndex)
{
    for (size_t i = 0; i < paragraph->GetChildCount(); ++i) {
        DocumentNode* child = paragraph->GetChild(i);
        if (!child || child->GetType() != NODE_TYPE_TEXT_RUN) continue;
        const TextRunNode* run = static_cast<const TextRunNode*>(child);
        if (!run->style.HasFormatting() || run->text.IsEmpty()) continue;
        xml += "    <style:style style:name=\"T";
        char buffer[16];
        snprintf(buffer, sizeof(buffer), "%d", styleIndex++);
        xml += buffer;
        xml += "\" style:family=\"text\">\n";
        xml += "      <style:text-properties";
        AppendTextProperties(xml, run->style);
        xml += "/>\n";
        xml += "    </style:style>\n";
    }
}

void AppendNode(BString& xml, const DocumentNode* node, int32& styleIndex);

void AppendParagraph(BString& xml, const ParagraphNode* paragraph, int32& styleIndex)
{
    xml += "      <text:p text:style-name=\"Standard\">";
    AppendParagraphContent(xml, paragraph, styleIndex);
    xml += "</text:p>\n";
    (void)styleIndex;
}

void AppendList(BString& xml, const ListNode* list, int32& styleIndex)
{
    xml += "      <text:list>\n";
    for (size_t i = 0; i < list->GetChildCount(); ++i) {
        DocumentNode* child = list->GetChild(i);
        if (!child || child->GetType() != NODE_TYPE_LIST_ITEM) continue;
        xml += "        <text:list-item>\n";
        for (size_t c = 0; c < child->GetChildCount(); ++c) {
            AppendNode(xml, child->GetChild(c), styleIndex);
        }
        xml += "        </text:list-item>\n";
    }
    xml += "      </text:list>\n";
}

void AppendTable(BString& xml, const TableNode* table, int32& styleIndex)
{
    xml += "      <table:table table:name=\"Table";
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%zu", (size_t)table);
    xml += buffer;
    xml += "\">\n";
    for (size_t r = 0; r < table->GetChildCount(); ++r) {
        DocumentNode* rowNode = table->GetChild(r);
        if (!rowNode || rowNode->GetType() != NODE_TYPE_TABLE_ROW) continue;
        xml += "        <table:table-row>\n";
        for (size_t c = 0; c < rowNode->GetChildCount(); ++c) {
            DocumentNode* cellNode = rowNode->GetChild(c);
            if (!cellNode || cellNode->GetType() != NODE_TYPE_TABLE_CELL) continue;
            xml += "          <table:table-cell>\n";
            for (size_t p = 0; p < cellNode->GetChildCount(); ++p) {
                AppendNode(xml, cellNode->GetChild(p), styleIndex);
            }
            xml += "          </table:table-cell>\n";
        }
        xml += "        </table:table-row>\n";
    }
    xml += "      </table:table>\n";
}

void AppendNode(BString& xml, const DocumentNode* node, int32& styleIndex)
{
    if (!node) return;
    switch (node->GetType()) {
        case NODE_TYPE_SECTION:
            for (size_t i = 0; i < node->GetChildCount(); ++i) {
                AppendNode(xml, node->GetChild(i), styleIndex);
            }
            break;
        case NODE_TYPE_PARAGRAPH:
            AppendParagraph(xml, static_cast<const ParagraphNode*>(node), styleIndex);
            break;
        case NODE_TYPE_LIST:
            AppendList(xml, static_cast<const ListNode*>(node), styleIndex);
            break;
        case NODE_TYPE_LIST_ITEM:
            for (size_t i = 0; i < node->GetChildCount(); ++i) {
                AppendNode(xml, node->GetChild(i), styleIndex);
            }
            break;
        case NODE_TYPE_TABLE:
            AppendTable(xml, static_cast<const TableNode*>(node), styleIndex);
            break;
        case NODE_TYPE_PAGE_BREAK:
            xml += "      <text:soft-page-break/>\n";
            break;
        default:
            break;
    }
}

BString ContentXml(const YellowDocument* document)
{
    // Automatic styles for runs with formatting. The same running counter
    // is reused for the content pass so style names match.
    BString automaticStyles;
    int32 styleIndex = 0;
    struct StyleCollector {
        static void Collect(const DocumentNode* node, BString& out, int32& index) {
            if (!node) return;
            if (node->GetType() == NODE_TYPE_PARAGRAPH) {
                CollectSpanStyles(out, static_cast<const ParagraphNode*>(node), index);
            }
            for (size_t i = 0; i < node->GetChildCount(); ++i) {
                Collect(node->GetChild(i), out, index);
            }
        }
    };
    StyleCollector::Collect(document->GetRoot(), automaticStyles, styleIndex);

    BString xml;
    xml += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    xml += "<office:document-content "
           "xmlns:office=\"urn:oasis:names:tc:opendocument:xmlns:office:1.0\" "
           "xmlns:text=\"urn:oasis:names:tc:opendocument:xmlns:text:1.0\" "
           "xmlns:table=\"urn:oasis:names:tc:opendocument:xmlns:table:1.0\" "
           "xmlns:style=\"urn:oasis:names:tc:opendocument:xmlns:style:1.0\" "
           "xmlns:fo=\"urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0\" "
           "office:version=\"1.2\">\n";
    xml += "  <office:automatic-styles>\n";
    xml += automaticStyles;
    xml += "  </office:automatic-styles>\n";
    xml += "  <office:body>\n";
    xml += "    <office:text>\n";
    styleIndex = 0;
    AppendNode(xml, document->GetRoot(), styleIndex);
    xml += "    </office:text>\n";
    xml += "  </office:body>\n";
    xml += "</office:document-content>\n";
    return xml;
}

BString ManifestXml()
{
    BString xml;
    xml += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    xml += "<manifest:manifest "
           "xmlns:manifest=\"urn:oasis:names:tc:opendocument:xmlns:manifest:1.0\" "
           "manifest:version=\"1.2\">\n";
    xml += "  <manifest:file-entry manifest:full-path=\"/\" "
           "manifest:media-type=\"application/vnd.oasis.opendocument.text\"/>\n";
    xml += "  <manifest:file-entry manifest:full-path=\"content.xml\" "
           "manifest:media-type=\"text/xml\"/>\n";
    xml += "  <manifest:file-entry manifest:full-path=\"styles.xml\" "
           "manifest:media-type=\"text/xml\"/>\n";
    xml += "  <manifest:file-entry manifest:full-path=\"meta.xml\" "
           "manifest:media-type=\"text/xml\"/>\n";
    xml += "</manifest:manifest>\n";
    return xml;
}

} // namespace

bool ODFExporter::Export(const YellowDocument* document, const BString& filename)
{
    if (!document) return false;

    ZipWriter zip;
    zip.AddFile("mimetype", BString(ODF_MIME_TYPE), false); // stored uncompressed, first
    zip.AddFile("content.xml", ContentXml(document), true);
    zip.AddFile("styles.xml", StylesXml(), true);
    zip.AddFile("meta.xml", MetaXml(document), true);
    zip.AddFile("META-INF/manifest.xml", ManifestXml(), true);
    return zip.Save(filename);
}

bool ODFExporter::ExportToData(const YellowDocument* document, char** data, size_t* size)
{
    if (!document || !data || !size) return false;

    ZipWriter zip;
    zip.AddFile("mimetype", BString(ODF_MIME_TYPE), false);
    zip.AddFile("content.xml", ContentXml(document), true);
    zip.AddFile("styles.xml", StylesXml(), true);
    zip.AddFile("meta.xml", MetaXml(document), true);
    zip.AddFile("META-INF/manifest.xml", ManifestXml(), true);

    std::vector<uint8_t> buffer;
    if (!zip.WriteToMemory(buffer)) return false;

    *data = (char*)malloc(buffer.size() + 1);
    if (!*data) return false;
    memcpy(*data, buffer.data(), buffer.size());
    (*data)[buffer.size()] = '\0';
    *size = buffer.size();
    return true;
}

const char* ODFExporter::GetName() const
{
    return "OpenDocument Text";
}

const char* ODFExporter::GetMimeType() const
{
    return ODF_MIME_TYPE;
}

const char* ODFExporter::GetExtension() const
{
    return ODF_FORMAT_EXTENSION;
}

} // namespace YellowImportExport
