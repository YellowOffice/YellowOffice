#include <YellowImportExport/ODFImporter.h>
#include <YellowImportExport/Xml.h>
#include <YellowImportExport/ZipArchive.h>
#include <YellowCommon/Defines.h>
#include <YellowCommon/Logger.h>
#include <YellowDocumentCore/YellowDocument.h>
#include <YellowDocumentCore/Nodes.h>

#include <stdio.h>
#include <stdlib.h>
#include <map>

namespace YellowImportExport {

using namespace YellowCommon;
using namespace YellowDocumentCore;

ODFImporter::ODFImporter()
{
}

ODFImporter::~ODFImporter()
{
}

namespace {

// Applies ODF text:style-name span attributes via a flat style table.
struct OdfStyle {
    bool bold = false;
    bool italic = false;
    bool underline = false;
    float fontSize = 0.0f;
    BString fontName;
    ParagraphStyle::Alignment alignment = ParagraphStyle::ALIGN_LEFT;
    bool hasAlignment = false;
};

struct OdfParagraphStyle {
    ParagraphStyle style;
    bool hasAlignment = false;
};

struct OdfContext {
    YellowDocument* document = nullptr;
    std::map<BString, OdfStyle> styles;
    std::map<BString, OdfParagraphStyle> paragraphStyles;
};

TextStyle ToTextStyle(const OdfStyle& style, const TextStyle& base)
{
    TextStyle result = base;
    result.bold = style.bold;
    result.italic = style.italic;
    result.underline = style.underline;
    if (style.fontSize > 0.0f) result.fontSize = style.fontSize;
    if (style.fontName.Length() > 0) result.fontFamily = style.fontName;
    return result;
}

void ParseStyleFamily(const XmlElement* styleEl, OdfContext& context)
{
    BString family = styleEl->Attribute("style:family", "");
    if (family == "text") {
        OdfStyle style;
        if (const XmlElement* textProps = styleEl->Child("style:text-properties")) {
            BString weight = textProps->Attribute("fo:font-weight", "");
            style.bold = (weight == "bold" || weight == "700");
            BString posture = textProps->Attribute("fo:font-style", "");
            style.italic = (posture == "italic" || posture == "oblique");
            BString u = textProps->Attribute("style:text-underline-style", "");
            style.underline = (u != "" && u != "none");
            BString size = textProps->Attribute("fo:font-size", "");
            if (size.Length() > 0) {
                // Values look like "12pt".
                style.fontSize = (float)atof(size.String());
            }
            style.fontName = textProps->Attribute("fo:font-family", "");
            if (style.fontName.Length() > 1 &&
                style.fontName[0] == '"' &&
                style.fontName[style.fontName.Length() - 1] == '"') {
                style.fontName.Remove(0, 1);
                style.fontName.Truncate(style.fontName.Length() - 1);
            }
        }
        BString name = styleEl->Attribute("style:name", "");
        if (name.Length() > 0) context.styles[name] = style;
    } else if (family == "paragraph") {
        OdfParagraphStyle style;
        BString name = styleEl->Attribute("style:name", "");
        if (const XmlElement* paraProps = styleEl->Child("style:paragraph-properties")) {
            BString align = paraProps->Attribute("fo:text-align", "");
            if (align == "center") { style.style.alignment = ParagraphStyle::ALIGN_CENTER; style.hasAlignment = true; }
            else if (align == "end" || align == "right") { style.style.alignment = ParagraphStyle::ALIGN_RIGHT; style.hasAlignment = true; }
        }
        if (name.Length() > 0) context.paragraphStyles[name] = style;
    }
}

void ParseTextContent(OdfContext& context, DocumentNode* parent, const XmlElement* element);

void ParseParagraph(OdfContext& context, DocumentNode* parent, const XmlElement* element)
{
    ParagraphNode* paragraph = new ParagraphNode();
    BString styleName = element->Attribute("text:style-name", "");
    auto it = context.paragraphStyles.find(styleName);
    if (it != context.paragraphStyles.end() && it->second.hasAlignment) {
        paragraph->style.alignment = it->second.style.alignment;
    }

    // Walk children: text and <text:span> runs.
    struct Walker {
        static void Walk(OdfContext& context, ParagraphNode* paragraph,
                         const XmlElement* element, const TextStyle& baseStyle) {
            for (const XmlElement* child : element->children) {
                if (!child) continue;
                if (child->name == "text:span") {
                    BString spanStyleName = child->Attribute("text:style-name", "");
                    OdfStyle odfStyle;
                    auto sit = context.styles.find(spanStyleName);
                    if (sit != context.styles.end()) odfStyle = sit->second;
                    TextStyle style = ToTextStyle(odfStyle, baseStyle);
                    BString text = child->AllText();
                    if (text.Length() > 0) {
                        paragraph->AddChild(new TextRunNode(text, style));
                    }
                } else if (child->name == "text:s") {
                    int32 spaces = (int32)atoi(child->Attribute("text:c", "1").String());
                    BString text;
                    for (int32 i = 0; i < spaces; ++i) text += ' ';
                    paragraph->AddChild(new TextRunNode(text, baseStyle));
                } else if (child->name == "text:tab") {
                    paragraph->AddChild(new TextRunNode(BString("\t"), baseStyle));
                } else if (child->name == "text:line-break") {
                    paragraph->AddChild(new TextRunNode(BString("\n"), baseStyle));
                } else {
                    // Recurse into unknown inline wrappers.
                    Walk(context, paragraph, child, baseStyle);
                }
            }
            if (!element->text.IsEmpty()) {
                paragraph->AddChild(new TextRunNode(element->text, baseStyle));
            }
        }
    };
    Walker::Walk(context, paragraph, element, TextStyle());
    if (paragraph->GetChildCount() == 0) {
        paragraph->EnsureTextRun();
    }
    parent->AddChild(paragraph);
}

void ParseTextContent(OdfContext& context, DocumentNode* parent, const XmlElement* element)
{
    for (const XmlElement* child : element->children) {
        if (!child) continue;
        if (child->name == "text:p" || child->name == "text:h") {
            ParseParagraph(context, parent, child);
        } else if (child->name == "text:list") {
            ListNode* list = new ListNode(child->Attribute("text:style-name", "").FindFirst("Number") >= 0);
            ParseTextContent(context, list, child);
            parent->AddChild(list);
        } else if (child->name == "text:list-item") {
            ListItemNode* item = new ListItemNode();
            ParseTextContent(context, item, child);
            if (item->GetChildCount() == 0) {
                ParagraphNode* paragraph = new ParagraphNode();
                paragraph->EnsureTextRun();
                item->AddChild(paragraph);
            }
            parent->AddChild(item);
        } else if (child->name == "table:table") {
            TableNode* table = new TableNode();
            ParseTextContent(context, table, child);
            parent->AddChild(table);
        } else if (child->name == "table:table-row") {
            TableRowNode* row = new TableRowNode();
            ParseTextContent(context, row, child);
            parent->AddChild(row);
        } else if (child->name == "table:table-cell") {
            TableCellNode* cell = new TableCellNode();
            ParseTextContent(context, cell, child);
            if (cell->GetChildCount() == 0) {
                ParagraphNode* paragraph = new ParagraphNode();
                paragraph->EnsureTextRun();
                cell->AddChild(paragraph);
            }
            parent->AddChild(cell);
        } else if (child->name == "text:soft-page-break") {
            parent->AddChild(new PageBreakNode());
        } else if (!child->children.empty()) {
            // Recurse into unknown structural wrappers (e.g. text:body).
            ParseTextContent(context, parent, child);
        }
    }
    if (!element->text.IsEmpty() && parent->GetType() == NODE_TYPE_SECTION) {
        // Stray text outside paragraphs: make a paragraph of it.
        ParagraphNode* paragraph = new ParagraphNode();
        paragraph->SetText(element->text);
        parent->AddChild(paragraph);
    }
}

} // namespace

bool ODFImporter::Import(YellowDocument* document, const BString& filename)
{
    if (!document) return false;

    ZipReader zip;
    if (!zip.Open(filename)) {
        Logger::GetGlobalLogger().Error("ODF: cannot open %s", filename.String());
        return false;
    }
    BString contentXml;
    if (!zip.ReadFileText("content.xml", contentXml)) {
        Logger::GetGlobalLogger().Error("ODF: missing content.xml");
        return false;
    }
    return ImportFromData(document, contentXml.String(), (size_t)contentXml.Length());
}

bool ODFImporter::ImportFromData(YellowDocument* document, const char* data, size_t size)
{
    if (!document || !data) return false;

    BString text(data, (int32)size);

    // Detect whether this is a whole .odt package (ZIP) or raw content.xml.
    if (size >= 4 && (unsigned char)data[0] == 0x50 && (unsigned char)data[1] == 0x4b) {
        ZipReader zip;
        if (!zip.OpenFromMemory(data, size)) return false;
        BString contentXml;
        if (!zip.ReadFileText("content.xml", contentXml)) return false;
        text = contentXml;
    }

    XmlDocument xml;
    BString error;
    if (!xml.Parse(text, &error)) {
        Logger::GetGlobalLogger().Error("ODF: XML parse error: %s", error.String());
        return false;
    }
    if (!xml.Root() || !xml.Root()->IsNamed("office:document-content")) {
        Logger::GetGlobalLogger().Error("ODF: not an office:document-content document");
        return false;
    }

    document->Clear();
    DocumentNode* root = document->GetRoot();
    if (!root) return false;

    // Clear() seeds a default paragraph; start from an empty root.
    while (root->GetChildCount() > 0) {
        DocumentNode* child = root->GetChild(0);
        root->RemoveChild(child);
        delete child;
    }

    OdfContext context;
    context.document = document;

    // Collect automatic styles.
    if (const XmlElement* autoStyles = xml.Root()->Child("office:automatic-styles")) {
        for (const XmlElement* style : autoStyles->children) {
            if (style && style->name == "style:style") {
                ParseStyleFamily(style, context);
            }
        }
    }

    if (const XmlElement* body = xml.Root()->Child("office:body")) {
        if (const XmlElement* textBody = body->Child("office:text")) {
            ParseTextContent(context, root, textBody);
        } else {
            ParseTextContent(context, root, body);
        }
    }

    if (root->GetChildCount() == 0) {
        ParagraphNode* paragraph = new ParagraphNode();
        paragraph->EnsureTextRun();
        root->AddChild(paragraph);
    }

    root->SetDocument(document);
    document->SetModified(false);
    document->UpdateStatistics();
    return true;
}

const char* ODFImporter::GetName() const
{
    return "OpenDocument Text";
}

const char* ODFImporter::GetMimeType() const
{
    return ODF_MIME_TYPE;
}

const char* ODFImporter::GetExtension() const
{
    return ODF_FORMAT_EXTENSION;
}

} // namespace YellowImportExport
