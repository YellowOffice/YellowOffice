#include <YellowCommon/Test.h>
#include <YellowImportExport/DocumentSerializer.h>
#include <YellowImportExport/ZipArchive.h>
#include <YellowImportExport/Xml.h>
#include <YellowImportExport/FormatRegistry.h>
#include <YellowImportExport/DocumentFormat.h>
#include <YellowImportExport/MarkdownImporter.h>
#include <YellowImportExport/MarkdownExporter.h>
#include <YellowImportExport/PlainText.h>
#include <YellowImportExport/YDFImporter.h>
#include <YellowImportExport/YDFExporter.h>
#include <YellowImportExport/ODFImporter.h>
#include <YellowImportExport/ODFExporter.h>
#include <YellowDocumentCore/YellowDocument.h>
#include <YellowDocumentCore/Nodes.h>

#include <stdlib.h>

using namespace YellowDocumentCore;
using namespace YellowImportExport;

static void EmptyRoot(YellowDocument* document)
{
    DocumentNode* root = document->GetRoot();
    while (root && root->GetChildCount() > 0) {
        DocumentNode* child = root->GetChild(0);
        root->RemoveChild(child);
        delete child;
    }
}

static YellowDocument* MakeSampleDocument()
{
    YellowDocument* document = new YellowDocument();
    EmptyRoot(document);
    DocumentNode* root = document->GetRoot();

    ParagraphNode* heading = new ParagraphNode();
    heading->paragraphStyleName = "Heading 1";
    TextRunNode* headingRun = new TextRunNode(BString("Title"));
    headingRun->style.bold = true;
    headingRun->style.fontSize = 24.0f;
    heading->AddChild(headingRun);
    root->AddChild(heading);

    ParagraphNode* body = new ParagraphNode();
    body->SetText("First paragraph with some words.");
    root->AddChild(body);

    ListNode* list = new ListNode(false);
    ListItemNode* item = new ListItemNode();
    ParagraphNode* itemText = new ParagraphNode();
    itemText->SetText("List item");
    item->AddChild(itemText);
    list->AddChild(item);
    root->AddChild(list);

    document->SetTitle("Test Document");
    return document;
}

TEST(JsonSerializerRoundTrip)
{
    YellowDocument* document = MakeSampleDocument();

    YellowCommon::JsonValue json = DocumentSerializer::DocumentToJson(document);
    BString text = json.Serialize();
    CHECK(text.FindFirst("content") >= 0);

    YellowDocument loaded;
    BString error;
    CHECK(DocumentSerializer::DocumentFromJson(&loaded, json, &error));

    DocumentNode* root = loaded.GetRoot();
    CHECK_NOT_NULL(root);
    CHECK_EQUAL(3, (int)root->GetChildCount());
    CHECK_STREQ("Title", root->GetChild(0)->GetText().String());
    CHECK_STREQ("First paragraph with some words.",
                root->GetChild(1)->GetText().String());
    CHECK(NODE_TYPE_LIST == root->GetChild(2)->GetType());
    CHECK_STREQ("Test Document", loaded.GetTitle().String());

    delete document;
}

TEST(ZipWriteReadRoundTrip)
{
    ZipWriter writer;
    writer.AddFile("a.txt", BString("hello"), true);
    writer.AddFile("sub/b.bin", BString("world"), false);
    std::vector<uint8_t> buffer;
    CHECK(writer.WriteToMemory(buffer));
    CHECK(buffer.size() > 0);

    ZipReader reader;
    CHECK(reader.OpenFromMemory(buffer.data(), buffer.size()));
    BString text;
    CHECK(reader.ReadFileText("a.txt", text));
    CHECK_STREQ("hello", text.String());
    CHECK(reader.ReadFileText("sub/b.bin", text));
    CHECK_STREQ("world", text.String());
}

TEST(XmlParserBasics)
{
    XmlDocument doc;
    BString error;
    CHECK(doc.Parse(
        "<?xml version=\"1.0\"?><root a=\"1\"><child>text &amp; more</child></root>",
        &error));
    CHECK_NOT_NULL(doc.Root());
    CHECK_STREQ("root", doc.Root()->name.String());
    CHECK_STREQ("1", doc.Root()->Attribute("a", "").String());
    XmlElement* child = doc.Root()->Child("child");
    CHECK_NOT_NULL(child);
    CHECK_STREQ("text & more", child->AllText().String());
}

TEST(XmlParserCdataAndEntities)
{
    XmlDocument doc;
    CHECK(doc.Parse("<r>&lt;tag&gt; <![CDATA[<raw>]]> &#65;</r>"));
    CHECK_STREQ("<tag> <raw> A", doc.Root()->AllText().String());
}

TEST(MarkdownRoundTrip)
{
    YellowDocument source;
    DocumentNode* root = source.GetRoot();
    ParagraphNode* heading = new ParagraphNode();
    heading->paragraphStyleName = "Heading 2";
    heading->SetText("Chapter One");
    root->AddChild(heading);
    ParagraphNode* body = new ParagraphNode();
    body->SetText("Some **bold** text.");
    root->AddChild(body);

    MarkdownExporter exporter;
    char* data = nullptr;
    size_t size = 0;
    CHECK(exporter.ExportToData(&source, &data, &size));
    CHECK_NOT_NULL(data);

    YellowDocument loaded;
    MarkdownImporter importer;
    CHECK(importer.ImportFromData(&loaded, data, size));
    free(data);

    DocumentNode* loadedRoot = loaded.GetRoot();
    CHECK_EQUAL(2, (int)loadedRoot->GetChildCount());
    ParagraphNode* loadedHeading =
        dynamic_cast<ParagraphNode*>(loadedRoot->GetChild(0));
    CHECK_NOT_NULL(loadedHeading);
    CHECK_STREQ("Heading 2", loadedHeading->paragraphStyleName.String());
}

TEST(MarkdownHeadingLevel)
{
    YellowDocument document;
    MarkdownImporter importer;
    const char* md = "# Title\n\nBody paragraph.\n";
    CHECK(importer.ImportFromData(&document, md, strlen(md)));
    DocumentNode* root = document.GetRoot();
    CHECK_EQUAL(2, (int)root->GetChildCount());
    ParagraphNode* heading = dynamic_cast<ParagraphNode*>(root->GetChild(0));
    CHECK_NOT_NULL(heading);
    CHECK_STREQ("Heading 1", heading->paragraphStyleName.String());
    CHECK_STREQ("Title", heading->GetText().String());
    ParagraphNode* body = dynamic_cast<ParagraphNode*>(root->GetChild(1));
    CHECK_NOT_NULL(body);
    CHECK_STREQ("Body paragraph.", body->GetText().String());
}

TEST(PlainTextRoundTrip)
{
    YellowDocument document;
    PlainTextImporter importer;
    const char* text = "line one\nline two\n\nline four\n";
    CHECK(importer.ImportFromData(&document, text, strlen(text)));
    CHECK_EQUAL(4, (int)document.GetRoot()->GetChildCount());

    PlainTextExporter exporter;
    char* out = nullptr;
    size_t outSize = 0;
    CHECK(exporter.ExportToData(&document, &out, &outSize));
    CHECK_NOT_NULL(out);
    BString result(out, (int32)outSize);
    free(out);
    CHECK(result.FindFirst("line one") >= 0);
    CHECK(result.FindFirst("line four") >= 0);
}

TEST(YdfContainerRoundTrip)
{
    YellowDocument* source = MakeSampleDocument();

    YDFExporter exporter;
    char* data = nullptr;
    size_t size = 0;
    CHECK(exporter.ExportToData(source, &data, &size));
    CHECK_NOT_NULL(data);
    CHECK(size > 4);
    CHECK((unsigned char)data[0] == 0x50); // 'P'
    CHECK((unsigned char)data[1] == 0x4b); // 'K'

    YellowDocument loaded;
    YDFImporter importer;
    CHECK(importer.ImportFromData(&loaded, data, size));
    free(data);

    CHECK_EQUAL(3, (int)loaded.GetRoot()->GetChildCount());
    CHECK_STREQ("Test Document", loaded.GetTitle().String());
    delete source;
}

TEST(OdfContainerRoundTrip)
{
    YellowDocument* source = MakeSampleDocument();

    ODFExporter exporter;
    char* data = nullptr;
    size_t size = 0;
    CHECK(exporter.ExportToData(source, &data, &size));
    CHECK_NOT_NULL(data);

    YellowDocument loaded;
    ODFImporter importer;
    CHECK(importer.ImportFromData(&loaded, data, size));
    free(data);

    DocumentNode* root = loaded.GetRoot();
    CHECK_NOT_NULL(root);
    CHECK(root->GetChildCount() >= 2);
    // Heading text survived the round trip.
    CHECK_STREQ("Title", root->GetChild(0)->GetText().String());
    delete source;
}

TEST(FormatRegistryDefaults)
{
    FormatRegistry* registry = FormatRegistry::CreateDefault();
    CHECK_NOT_NULL(registry);
    CHECK_NOT_NULL(registry->GetFormatByMimeType(
        BString("application/x-vnd.yellowdocs.ydf")));
    CHECK_NOT_NULL(registry->GetFormatByExtension(BString("md")));
    CHECK_NOT_NULL(registry->GetFormatByExtension(BString("odt")));
    CHECK_NOT_NULL(registry->GetFormatByExtension(BString("txt")));

    std::vector<const DocumentFormat*> formats;
    registry->GetAllFormats(formats);
    CHECK_EQUAL(4, (int)formats.size());
    delete registry;
}
