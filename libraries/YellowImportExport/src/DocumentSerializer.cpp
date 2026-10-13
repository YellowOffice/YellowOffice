#include <YellowImportExport/DocumentSerializer.h>

#include <YellowDocumentCore/Nodes.h>
#include <time.h>

namespace YellowImportExport {

using namespace YellowCommon;
using namespace YellowDocumentCore;

// ---------------------------------------------------------------- base64

namespace {

const char kBase64Table[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int Base64Value(char c)
{
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= 'a' && c <= 'z') return c - 'a' + 26;
    if (c >= '0' && c <= '9') return c - '0' + 52;
    if (c == '+') return 62;
    if (c == '/') return 63;
    return -1;
}

BString ColorToHex(const rgb_color& color, rgb_color = { 0, 0, 0, 0 })
{
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "#%02x%02x%02x%02x",
             color.red, color.green, color.blue, color.alpha);
    return BString(buffer);
}

rgb_color HexToColor(const BString& text, rgb_color fallback = { 0, 0, 0, 255 })
{
    rgb_color color = fallback;
    const char* s = text.String();
    if (text.Length() >= 7 && s[0] == '#') {
        unsigned int r, g, b, a = 255;
        if (sscanf(s + 1, "%02x%02x%02x", &r, &g, &b) == 3) {
            if (text.Length() >= 9) {
                unsigned int alpha;
                if (sscanf(s + 7, "%02x", &alpha) == 1) a = alpha;
            }
            color.red = (uint8)r;
            color.green = (uint8)g;
            color.blue = (uint8)b;
            color.alpha = (uint8)a;
        }
    }
    return color;
}

JsonValue TextStyleToJson(const TextStyle& style)
{
    JsonValue json;
    json.Set("fontFamily", JsonValue(style.fontFamily));
    json.Set("fontSize", JsonValue((double)style.fontSize));
    json.Set("bold", JsonValue(style.bold));
    json.Set("italic", JsonValue(style.italic));
    json.Set("underline", JsonValue(style.underline));
    json.Set("strikethrough", JsonValue(style.strikethrough));
    json.Set("superscript", JsonValue(style.superscript));
    json.Set("subscript", JsonValue(style.subscript));
    json.Set("baselineShift", JsonValue((double)style.baselineShift));
    json.Set("letterSpacing", JsonValue((double)style.letterSpacing));
    json.Set("lineSpacing", JsonValue((double)style.lineSpacing));
    json.Set("fontFlags", JsonValue((int32)style.fontFlags));
    json.Set("textColor", JsonValue(ColorToHex(style.textColor)));
    json.Set("backgroundColor", JsonValue(ColorToHex(style.backgroundColor,
                                                     { 0, 0, 0, 0 })));
    return json;
}

TextStyle TextStyleFromJson(const JsonValue& json)
{
    TextStyle style;
    if (json.Has("fontFamily")) style.fontFamily = json.Get("fontFamily").AsString();
    if (json.Has("fontSize")) style.fontSize = (float)json.Get("fontSize").AsNumber(12.0);
    if (json.Has("bold")) style.bold = json.Get("bold").AsBool();
    if (json.Has("italic")) style.italic = json.Get("italic").AsBool();
    if (json.Has("underline")) style.underline = json.Get("underline").AsBool();
    if (json.Has("strikethrough")) style.strikethrough = json.Get("strikethrough").AsBool();
    if (json.Has("superscript")) style.superscript = json.Get("superscript").AsBool();
    if (json.Has("subscript")) style.subscript = json.Get("subscript").AsBool();
    if (json.Has("baselineShift")) style.baselineShift = (float)json.Get("baselineShift").AsNumber();
    if (json.Has("letterSpacing")) style.letterSpacing = (float)json.Get("letterSpacing").AsNumber();
    if (json.Has("lineSpacing")) style.lineSpacing = (float)json.Get("lineSpacing").AsNumber(1.0);
    if (json.Has("fontFlags")) style.fontFlags = (uint32)json.Get("fontFlags").AsInt();
    if (json.Has("textColor")) style.textColor = HexToColor(json.Get("textColor").AsString());
    if (json.Has("backgroundColor")) {
        style.backgroundColor = HexToColor(json.Get("backgroundColor").AsString(),
                                           { 0, 0, 0, 0 });
    }
    return style;
}

JsonValue ParagraphStyleToJson(const ParagraphStyle& style)
{
    JsonValue json;
    json.Set("alignment", JsonValue((int32)style.alignment));
    json.Set("leftIndent", JsonValue((double)style.leftIndent));
    json.Set("rightIndent", JsonValue((double)style.rightIndent));
    json.Set("firstLineIndent", JsonValue((double)style.firstLineIndent));
    json.Set("spaceBefore", JsonValue((double)style.spaceBefore));
    json.Set("spaceAfter", JsonValue((double)style.spaceAfter));
    json.Set("lineSpacing", JsonValue((double)style.lineSpacing));
    json.Set("topMargin", JsonValue((double)style.topMargin));
    json.Set("bottomMargin", JsonValue((double)style.bottomMargin));
    json.Set("leftMargin", JsonValue((double)style.leftMargin));
    json.Set("rightMargin", JsonValue((double)style.rightMargin));
    json.Set("isList", JsonValue(style.isList));
    json.Set("isNumbered", JsonValue(style.isNumbered));
    json.Set("listLevel", JsonValue(style.listLevel));
    json.Set("listBulletChar", JsonValue(style.listBulletChar));
    json.Set("hasBorder", JsonValue(style.hasBorder));
    json.Set("borderWidth", JsonValue((double)style.borderWidth));
    json.Set("borderColor", JsonValue(ColorToHex(style.borderColor)));
    json.Set("backgroundColor", JsonValue(ColorToHex(style.backgroundColor,
                                                     { 0, 0, 0, 0 })));
    return json;
}

ParagraphStyle ParagraphStyleFromJson(const JsonValue& json)
{
    ParagraphStyle style;
    if (json.Has("alignment")) {
        style.alignment = (ParagraphStyle::Alignment)json.Get("alignment").AsInt();
    }
    if (json.Has("leftIndent")) style.leftIndent = (float)json.Get("leftIndent").AsNumber();
    if (json.Has("rightIndent")) style.rightIndent = (float)json.Get("rightIndent").AsNumber();
    if (json.Has("firstLineIndent")) {
        style.firstLineIndent = (float)json.Get("firstLineIndent").AsNumber();
    }
    if (json.Has("spaceBefore")) style.spaceBefore = (float)json.Get("spaceBefore").AsNumber();
    if (json.Has("spaceAfter")) style.spaceAfter = (float)json.Get("spaceAfter").AsNumber();
    if (json.Has("lineSpacing")) style.lineSpacing = (float)json.Get("lineSpacing").AsNumber(1.0);
    if (json.Has("topMargin")) style.topMargin = (float)json.Get("topMargin").AsNumber();
    if (json.Has("bottomMargin")) style.bottomMargin = (float)json.Get("bottomMargin").AsNumber();
    if (json.Has("leftMargin")) style.leftMargin = (float)json.Get("leftMargin").AsNumber();
    if (json.Has("rightMargin")) style.rightMargin = (float)json.Get("rightMargin").AsNumber();
    if (json.Has("isList")) style.isList = json.Get("isList").AsBool();
    if (json.Has("isNumbered")) style.isNumbered = json.Get("isNumbered").AsBool();
    if (json.Has("listLevel")) style.listLevel = json.Get("listLevel").AsInt();
    if (json.Has("listBulletChar")) style.listBulletChar = json.Get("listBulletChar").AsString();
    if (json.Has("hasBorder")) style.hasBorder = json.Get("hasBorder").AsBool();
    if (json.Has("borderWidth")) style.borderWidth = (float)json.Get("borderWidth").AsNumber();
    if (json.Has("borderColor")) style.borderColor = HexToColor(json.Get("borderColor").AsString());
    if (json.Has("backgroundColor")) {
        style.backgroundColor = HexToColor(json.Get("backgroundColor").AsString(),
                                           { 0, 0, 0, 0 });
    }
    return style;
}

JsonValue NodeToJson(const DocumentNode* node);
bool NodeFromJson(YellowDocument* document, DocumentNode* parent,
                  const JsonValue& json, BString* outError);

JsonValue ChildrenToJson(const DocumentNode* node)
{
    JsonValue array;
    for (size_t i = 0; i < node->GetChildCount(); ++i) {
        array.Append(NodeToJson(node->GetChild(i)));
    }
    return array;
}

void ChildrenFromJson(YellowDocument* document, DocumentNode* parent,
                      const JsonValue& json, BString* outError)
{
    for (int32 i = 0; i < json.CountItems(); ++i) {
        NodeFromJson(document, parent, json.At(i), outError);
    }
}

JsonValue NodeToJson(const DocumentNode* node)
{
    JsonValue json;
    if (!node) return json;

    switch (node->GetType()) {
        case NODE_TYPE_SECTION: {
            const SectionNode* section = static_cast<const SectionNode*>(node);
            json.Set("type", JsonValue("section"));
            json.Set("name", JsonValue(section->name));
            json.Set("pageWidth", JsonValue((double)section->pageWidth));
            json.Set("pageHeight", JsonValue((double)section->pageHeight));
            json.Set("leftMargin", JsonValue((double)section->leftMargin));
            json.Set("rightMargin", JsonValue((double)section->rightMargin));
            json.Set("topMargin", JsonValue((double)section->topMargin));
            json.Set("bottomMargin", JsonValue((double)section->bottomMargin));
            json.Set("hasHeader", JsonValue(section->hasHeader));
            json.Set("hasFooter", JsonValue(section->hasFooter));
            json.Set("children", ChildrenToJson(node));
            break;
        }
        case NODE_TYPE_PARAGRAPH: {
            const ParagraphNode* paragraph = static_cast<const ParagraphNode*>(node);
            json.Set("type", JsonValue("paragraph"));
            json.Set("styleName", JsonValue(paragraph->paragraphStyleName));
            json.Set("style", ParagraphStyleToJson(paragraph->style));
            json.Set("children", ChildrenToJson(node));
            break;
        }
        case NODE_TYPE_TEXT_RUN: {
            const TextRunNode* run = static_cast<const TextRunNode*>(node);
            json.Set("type", JsonValue("textRun"));
            json.Set("text", JsonValue(run->text));
            json.Set("styleName", JsonValue(run->characterStyleName));
            json.Set("style", TextStyleToJson(run->style));
            break;
        }
        case NODE_TYPE_LIST: {
            const ListNode* list = static_cast<const ListNode*>(node);
            json.Set("type", JsonValue("list"));
            json.Set("ordered", JsonValue(list->ordered));
            json.Set("level", JsonValue(list->level));
            json.Set("children", ChildrenToJson(node));
            break;
        }
        case NODE_TYPE_LIST_ITEM: {
            json.Set("type", JsonValue("listItem"));
            json.Set("children", ChildrenToJson(node));
            break;
        }
        case NODE_TYPE_TABLE: {
            const TableNode* table = static_cast<const TableNode*>(node);
            json.Set("type", JsonValue("table"));
            json.Set("columns", JsonValue(table->columnCount));
            json.Set("children", ChildrenToJson(node));
            break;
        }
        case NODE_TYPE_TABLE_ROW: {
            json.Set("type", JsonValue("tableRow"));
            json.Set("children", ChildrenToJson(node));
            break;
        }
        case NODE_TYPE_TABLE_CELL: {
            json.Set("type", JsonValue("tableCell"));
            json.Set("children", ChildrenToJson(node));
            break;
        }
        case NODE_TYPE_IMAGE: {
            const ImageNode* image = static_cast<const ImageNode*>(node);
            json.Set("type", JsonValue("image"));
            json.Set("mimeType", JsonValue(image->mimeType));
            json.Set("alt", JsonValue(image->altText));
            json.Set("displayWidth", JsonValue((double)image->displayWidth));
            json.Set("displayHeight", JsonValue((double)image->displayHeight));
            json.Set("intrinsicWidth", JsonValue((double)image->intrinsicWidth));
            json.Set("intrinsicHeight", JsonValue((double)image->intrinsicHeight));
            json.Set("data", JsonValue(DocumentSerializer::Base64Encode(image->data.data(), image->data.size())));
            break;
        }
        case NODE_TYPE_PAGE_BREAK:
            json.Set("type", JsonValue("pageBreak"));
            break;
        case NODE_TYPE_HEADER_FOOTER: {
            const HeaderFooterNode* hf = static_cast<const HeaderFooterNode*>(node);
            json.Set("type", JsonValue("headerFooter"));
            json.Set("isHeader", JsonValue(hf->isHeader));
            json.Set("text", JsonValue(hf->text));
            break;
        }
        default:
            json.Set("type", JsonValue("unknown"));
            break;
    }
    return json;
}

bool NodeFromJson(YellowDocument* document, DocumentNode* parent,
                  const JsonValue& json, BString* outError)
{
    BString type = json.Get("type").AsString();
    DocumentNode* node = nullptr;

    if (type == "section") {
        SectionNode* section = new SectionNode();
        section->name = json.Get("name").AsString();
        section->pageWidth = (float)json.Get("pageWidth").AsNumber(section->pageWidth);
        section->pageHeight = (float)json.Get("pageHeight").AsNumber(section->pageHeight);
        section->leftMargin = (float)json.Get("leftMargin").AsNumber(section->leftMargin);
        section->rightMargin = (float)json.Get("rightMargin").AsNumber(section->rightMargin);
        section->topMargin = (float)json.Get("topMargin").AsNumber(section->topMargin);
        section->bottomMargin = (float)json.Get("bottomMargin").AsNumber(section->bottomMargin);
        section->hasHeader = json.Get("hasHeader").AsBool();
        section->hasFooter = json.Get("hasFooter").AsBool();
        node = section;
    } else if (type == "paragraph") {
        ParagraphNode* paragraph = new ParagraphNode();
        paragraph->paragraphStyleName = json.Get("styleName").AsString();
        paragraph->style = ParagraphStyleFromJson(json.Get("style"));
        node = paragraph;
    } else if (type == "textRun") {
        TextRunNode* run = new TextRunNode();
        run->text = json.Get("text").AsString();
        run->characterStyleName = json.Get("styleName").AsString();
        run->style = TextStyleFromJson(json.Get("style"));
        node = run;
    } else if (type == "list") {
        ListNode* list = new ListNode(json.Get("ordered").AsBool());
        list->level = json.Get("level").AsInt();
        node = list;
    } else if (type == "listItem") {
        node = new ListItemNode();
    } else if (type == "table") {
        TableNode* table = new TableNode();
        table->columnCount = json.Get("columns").AsInt();
        node = table;
    } else if (type == "tableRow") {
        node = new TableRowNode();
    } else if (type == "tableCell") {
        node = new TableCellNode();
    } else if (type == "image") {
        ImageNode* image = new ImageNode();
        image->mimeType = json.Get("mimeType").AsString();
        image->altText = json.Get("alt").AsString();
        image->displayWidth = (float)json.Get("displayWidth").AsNumber();
        image->displayHeight = (float)json.Get("displayHeight").AsNumber();
        image->intrinsicWidth = (float)json.Get("intrinsicWidth").AsNumber();
        image->intrinsicHeight = (float)json.Get("intrinsicHeight").AsNumber();
        BString encoded = json.Get("data").AsString();
        if (encoded.Length() > 0) {
            DocumentSerializer::Base64Decode(encoded, image->data);
        }
        node = image;
    } else if (type == "pageBreak") {
        node = new PageBreakNode();
    } else if (type == "headerFooter") {
        HeaderFooterNode* hf = new HeaderFooterNode();
        hf->isHeader = json.Get("isHeader").AsBool(true);
        hf->text = json.Get("text").AsString();
        node = hf;
    } else {
        if (outError) {
            BString message("Unknown node type: ");
            message += type;
            *outError = message;
        }
        return false;
    }

    if (json.Has("children")) {
        ChildrenFromJson(document, node, json.Get("children"), outError);
    }

    if (parent) {
        parent->AddChild(node);
    } else {
        delete node;
    }
    return true;
}

} // namespace

// ---------------------------------------------------------------- DocumentSerializer

BString DocumentSerializer::Base64Encode(const void* data, size_t size)
{
    BString out;
    const uint8* bytes = (const uint8*)data;
    size_t i = 0;
    for (; i + 2 < size; i += 3) {
        uint32 value = ((uint32)bytes[i] << 16) | ((uint32)bytes[i + 1] << 8) | bytes[i + 2];
        out += kBase64Table[(value >> 18) & 63];
        out += kBase64Table[(value >> 12) & 63];
        out += kBase64Table[(value >> 6) & 63];
        out += kBase64Table[value & 63];
    }
    if (i < size) {
        uint32 value = (uint32)bytes[i] << 16;
        bool hasSecond = i + 1 < size;
        if (hasSecond) value |= (uint32)bytes[i + 1] << 8;
        out += kBase64Table[(value >> 18) & 63];
        out += kBase64Table[(value >> 12) & 63];
        out += hasSecond ? kBase64Table[(value >> 6) & 63] : '=';
        out += '=';
    }
    return out;
}

bool DocumentSerializer::Base64Decode(const BString& text, std::vector<uint8_t>& out)
{
    out.clear();
    uint32 buffer = 0;
    int32 bits = 0;
    for (int32 i = 0; i < text.Length(); ++i) {
        char c = text[i];
        if (c == '=' || c == '\n' || c == '\r') continue;
        int value = Base64Value(c);
        if (value < 0) return false;
        buffer = (buffer << 6) | (uint32)value;
        bits += 6;
        if (bits >= 8) {
            bits -= 8;
            out.push_back((uint8)((buffer >> bits) & 0xff));
        }
    }
    return true;
}

JsonValue DocumentSerializer::DocumentToJson(const YellowDocument* document)
{
    JsonValue json;
    if (!document) return json;

    json.Set("version", JsonValue((int32)1));
    json.Set("generator", JsonValue("YellowDocs " YELLOWOFFICE_VERSION));

    if (document->GetMetadata()) {
        const DocumentMetadata* metadata = document->GetMetadata();
        JsonValue meta;
        meta.Set("title", JsonValue(metadata->title));
        meta.Set("author", JsonValue(metadata->author));
        meta.Set("subject", JsonValue(metadata->subject));
        meta.Set("keywords", JsonValue(metadata->keywords));
        meta.Set("description", JsonValue(metadata->description));
        meta.Set("created", JsonValue((double)metadata->created));
        meta.Set("modified", JsonValue((double)metadata->modified));
        json.Set("metadata", meta);
    }

    if (document->GetRoot()) {
        json.Set("content", NodeToJson(document->GetRoot()));
    }
    return json;
}

bool DocumentSerializer::DocumentFromJson(YellowDocument* document,
                                          const JsonValue& json,
                                          BString* outError)
{
    if (!document) {
        if (outError) *outError = "No document";
        return false;
    }

    document->Clear();
    document->RemoveAllObservers();

    if (json.Has("metadata")) {
        const JsonValue& meta = json.Get("metadata");
        DocumentMetadata* metadata = document->GetMetadata();
        if (metadata) {
            metadata->title = meta.Get("title").AsString();
            metadata->author = meta.Get("author").AsString();
            metadata->subject = meta.Get("subject").AsString();
            metadata->keywords = meta.Get("keywords").AsString();
            metadata->description = meta.Get("description").AsString();
            metadata->created = (time_t)meta.Get("created").AsNumber((double)time(nullptr));
            metadata->modified = (time_t)meta.Get("modified").AsNumber((double)time(nullptr));
        }
    }

    if (!json.Has("content")) {
        document->SetModified(false);
        return true;
    }

    // Parse into a temporary root adopter, then promote the parsed tree.
    struct RootAdopter : DocumentNode {
        RootAdopter() : DocumentNode(NODE_TYPE_SECTION) {}
        DocumentNode* Clone() const override { return nullptr; }
    };

    RootAdopter* adopter = new RootAdopter();
    if (!NodeFromJson(document, adopter, json.Get("content"), outError)) {
        delete adopter;
        document->Clear();
        return false;
    }

    if (adopter->GetChildCount() == 1) {
        DocumentNode* root = adopter->GetChild(0);
        adopter->RemoveChild(root);
        delete adopter;
        document->SetRoot(root);
    } else {
        document->SetRoot(adopter);
    }

    if (document->GetRoot()) {
        document->GetRoot()->SetDocument(document);
    }
    document->SetModified(false);
    document->UpdateStatistics();
    return true;
}

} // namespace YellowImportExport
