#include <YellowCommon/Test.h>
#include <YellowDocumentCore/YellowDocument.h>
#include <YellowDocumentCore/Nodes.h>
#include <YellowDocumentCore/Command.h>

using namespace YellowDocumentCore;

TEST(DocumentCreatesDefaultParagraph)
{
    YellowDocument document;
    CHECK_NOT_NULL(document.GetRoot());
    CHECK_EQUAL(1, (int)document.GetRoot()->GetChildCount());
    DocumentNode* first = document.GetRoot()->GetChild(0);
    CHECK_NOT_NULL(first);
    CHECK(NODE_TYPE_PARAGRAPH == first->GetType());
}

TEST(DocumentStatistics)
{
    YellowDocument document;
    // The document starts with one default paragraph; set its text.
    DocumentNode* root = document.GetRoot();
    ParagraphNode* paragraph = static_cast<ParagraphNode*>(root->GetChild(0));
    paragraph->SetText("Hello brave new world");
    document.UpdateStatistics();
    CHECK_EQUAL(4, document.GetWordCount());
    // 21 characters of text + 1 paragraph mark.
    CHECK_EQUAL(22, document.GetCharacterCount());
    CHECK_EQUAL(1, document.GetParagraphCount());
}

TEST(TextRunOwnership)
{
    ParagraphNode paragraph;
    paragraph.SetText("abc");
    CHECK_EQUAL(3, paragraph.GetTextLength());
    CHECK_STREQ("abc", paragraph.GetText().String());
    CHECK_EQUAL(1, (int)paragraph.GetChildCount());

    paragraph.InsertText(1, "X");
    CHECK_STREQ("aXbc", paragraph.GetText().String());

    paragraph.DeleteText(0, 2);
    CHECK_STREQ("bc", paragraph.GetText().String());
}

TEST(CommandInsertUndoRedo)
{
    YellowDocument document;
    ParagraphNode* paragraph =
        static_cast<ParagraphNode*>(document.GetRoot()->GetChild(0));
    paragraph->SetText("");

    CHECK(document.ExecuteCommand(
        new InsertTextCommand(&document, paragraph, 0, "Hello")));
    CHECK_STREQ("Hello", paragraph->GetText().String());
    CHECK(document.CanUndo());

    CHECK(document.Undo());
    CHECK_STREQ("", paragraph->GetText().String());

    CHECK(document.Redo());
    CHECK_STREQ("Hello", paragraph->GetText().String());
}

TEST(CommandDeleteUndo)
{
    YellowDocument document;
    ParagraphNode* paragraph =
        static_cast<ParagraphNode*>(document.GetRoot()->GetChild(0));
    paragraph->SetText("abcdef");

    CHECK(document.ExecuteCommand(
        new DeleteTextCommand(&document, paragraph, 2, 2)));
    CHECK_STREQ("abef", paragraph->GetText().String());

    CHECK(document.Undo());
    CHECK_STREQ("abcdef", paragraph->GetText().String());
}

TEST(CommandSplitParagraph)
{
    YellowDocument document;
    ParagraphNode* paragraph =
        static_cast<ParagraphNode*>(document.GetRoot()->GetChild(0));
    paragraph->SetText("HelloWorld");

    CHECK(document.ExecuteCommand(
        new SplitParagraphCommand(&document, paragraph, 5)));
    DocumentNode* root = document.GetRoot();
    CHECK_EQUAL(2, (int)root->GetChildCount());
    CHECK_STREQ("Hello", root->GetChild(0)->GetText().String());
    CHECK_STREQ("World", root->GetChild(1)->GetText().String());

    CHECK(document.Undo());
    CHECK_EQUAL(1, (int)root->GetChildCount());
    CHECK_STREQ("HelloWorld", root->GetChild(0)->GetText().String());
}

TEST(NodeOwnershipDeletesChildren)
{
    SectionNode* parent = new SectionNode();
    parent->AddChild(new TextRunNode(BString("child")));
    CHECK_EQUAL(1, (int)parent->GetChildCount());
    delete parent; // must not leak; ASan/valgrind would verify
    CHECK(true);
}

TEST(PositionOrdering)
{
    YellowDocument document;
    DocumentNode* root = document.GetRoot();
    ParagraphNode* first = static_cast<ParagraphNode*>(root->GetChild(0));
    ParagraphNode* second = new ParagraphNode();
    second->SetText("second");
    root->AddChild(second);

    DocumentPosition a(first, 0);
    DocumentPosition b(second, 0);
    CHECK(a < b);
    CHECK(b > a);
    CHECK(a <= a);
    CHECK(a == DocumentPosition(first, 0));
}
