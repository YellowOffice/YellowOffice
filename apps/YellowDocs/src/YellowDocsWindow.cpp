#include "YellowDocsWindow.h"
#include "DocumentCanvas.h"

#include <MenuBar.h>
#include <Menu.h>
#include <MenuItem.h>
#include <ScrollView.h>
#include <StatusBar.h>
#include <FilePanel.h>
#include <Entry.h>
#include <Path.h>
#include <Alert.h>
#include <Message.h>
#include <InterfaceDefs.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <YellowUI/ToolBar.h>
#include <YellowDocumentCore/YellowDocument.h>
#include <YellowImportExport/YDFImporter.h>
#include <YellowImportExport/YDFExporter.h>
#include <YellowImportExport/MarkdownImporter.h>
#include <YellowImportExport/MarkdownExporter.h>
#include <YellowImportExport/PlainText.h>
#include <YellowImportExport/ODFImporter.h>
#include <YellowImportExport/ODFExporter.h>

using namespace YellowDocumentCore;
using namespace YellowImportExport;

// ------------------------------------------------------------- message constants

const uint32 YellowDocsWindow::kMsgFileNew = 'YDnw';
const uint32 YellowDocsWindow::kMsgFileOpen = 'YDop';
const uint32 YellowDocsWindow::kMsgFileSave = 'YDsv';
const uint32 YellowDocsWindow::kMsgFileSaveAs = 'YDsa';
const uint32 YellowDocsWindow::kMsgFileClose = 'YDcl';
const uint32 YellowDocsWindow::kMsgFileQuit = 'YDqt';

const uint32 YellowDocsWindow::kMsgEditUndo = 'YDun';
const uint32 YellowDocsWindow::kMsgEditRedo = 'YDrd';
const uint32 YellowDocsWindow::kMsgEditCut = 'YDct';
const uint32 YellowDocsWindow::kMsgEditCopy = 'YDcp';
const uint32 YellowDocsWindow::kMsgEditPaste = 'YDpa';
const uint32 YellowDocsWindow::kMsgEditSelectAll = 'YDsl';
const uint32 YellowDocsWindow::kMsgEditFind = 'YDfi';
const uint32 YellowDocsWindow::kMsgEditReplace = 'YDrp';

const uint32 YellowDocsWindow::kMsgViewZoomIn = 'YDzi';
const uint32 YellowDocsWindow::kMsgViewZoomOut = 'YDzo';
const uint32 YellowDocsWindow::kMsgViewZoomReset = 'YDzr';
const uint32 YellowDocsWindow::kMsgViewToggleToolbar = 'YDtt';
const uint32 YellowDocsWindow::kMsgViewToggleStatusBar = 'YDts';

const uint32 YellowDocsWindow::kMsgInsertPageBreak = 'YDpb';
const uint32 YellowDocsWindow::kMsgInsertTable = 'YDt1';
const uint32 YellowDocsWindow::kMsgInsertImage = 'YDim';

const uint32 YellowDocsWindow::kMsgFormatBold = 'YDbd';
const uint32 YellowDocsWindow::kMsgFormatItalic = 'YDit';
const uint32 YellowDocsWindow::kMsgFormatUnderline = 'YDul';
const uint32 YellowDocsWindow::kMsgFormatAlignLeft = 'YDfl';
const uint32 YellowDocsWindow::kMsgFormatAlignCenter = 'YDfc';
const uint32 YellowDocsWindow::kMsgFormatAlignRight = 'YDfr';
const uint32 YellowDocsWindow::kMsgFormatAlignJustify = 'YDfj';

const uint32 YellowDocsWindow::kMsgHelpAbout = 'YDab';
const uint32 YellowDocsWindow::kMsgHelpDocumentation = 'YDhd';

const uint32 YellowDocsWindow::kMsgDocumentEdited = 'YDed';

// ------------------------------------------------------------------- window

YellowDocsWindow::YellowDocsWindow(BRect frame)
    : BWindow(frame, "Untitled - YellowDocs", B_TITLED_WINDOW,
              B_ASYNCHRONOUS_CONTROLS | B_NOT_ZOOMABLE | B_NOT_MINIMIZABLE),
      fMenuBar(nullptr),
      fToolBar(nullptr),
      fScrollView(nullptr),
      fCanvas(nullptr),
      fStatusBar(nullptr),
      fOpenPanel(nullptr),
      fSavePanel(nullptr),
      fDocument(nullptr),
      fDocumentModified(false),
      fZoomLevel(1.0f)
{
    fDocument = new YellowDocument();
    InitWindow();
    UpdateTitle();
    UpdateStatusBar();
}

YellowDocsWindow::~YellowDocsWindow()
{
    delete fOpenPanel;
    delete fSavePanel;
    fOpenPanel = nullptr;
    fSavePanel = nullptr;
    // fCanvas and fMenuBar are children of this window (owned by it).
    // fDocument is owned by us.
    delete fDocument;
}

void YellowDocsWindow::InitWindow()
{
    InitMenuBar();
    InitToolBar();
    InitMainView();
    InitStatusBar();
    LayoutChildren();
}

void YellowDocsWindow::InitMenuBar()
{
    fMenuBar = new BMenuBar("MenuBar");

    // File
    BMenu* fileMenu = new BMenu("File");
    fileMenu->AddItem(new BMenuItem("New", new BMessage(kMsgFileNew), 'N'));
    fileMenu->AddItem(new BMenuItem("Open...", new BMessage(kMsgFileOpen), 'O'));
    fileMenu->AddSeparatorItem();
    fileMenu->AddItem(new BMenuItem("Save", new BMessage(kMsgFileSave), 'S'));
    fileMenu->AddItem(new BMenuItem("Save As...", new BMessage(kMsgFileSaveAs), 'S',
                                    B_SHIFT_KEY | B_COMMAND_KEY));
    fileMenu->AddSeparatorItem();
    fileMenu->AddItem(new BMenuItem("Close", new BMessage(kMsgFileClose), 'W'));
    fileMenu->AddItem(new BMenuItem("Quit", new BMessage(kMsgFileQuit), 'Q'));
    fMenuBar->AddItem(fileMenu);

    // Edit
    BMenu* editMenu = new BMenu("Edit");
    editMenu->AddItem(new BMenuItem("Undo", new BMessage(kMsgEditUndo), 'Z'));
    editMenu->AddItem(new BMenuItem("Redo", new BMessage(kMsgEditRedo), 'Z',
                                    B_SHIFT_KEY | B_COMMAND_KEY));
    editMenu->AddSeparatorItem();
    editMenu->AddItem(new BMenuItem("Cut", new BMessage(kMsgEditCut), 'X'));
    editMenu->AddItem(new BMenuItem("Copy", new BMessage(kMsgEditCopy), 'C'));
    editMenu->AddItem(new BMenuItem("Paste", new BMessage(kMsgEditPaste), 'V'));
    editMenu->AddSeparatorItem();
    editMenu->AddItem(new BMenuItem("Select All", new BMessage(kMsgEditSelectAll), 'A'));
    editMenu->AddItem(new BMenuItem("Find...", new BMessage(kMsgEditFind), 'F'));
    fMenuBar->AddItem(editMenu);

    // View
    BMenu* viewMenu = new BMenu("View");
    viewMenu->AddItem(new BMenuItem("Zoom In", new BMessage(kMsgViewZoomIn), '+'));
    viewMenu->AddItem(new BMenuItem("Zoom Out", new BMessage(kMsgViewZoomOut), '-'));
    viewMenu->AddItem(new BMenuItem("Normal Size", new BMessage(kMsgViewZoomReset), '0'));
    viewMenu->AddSeparatorItem();
    viewMenu->AddItem(new BMenuItem("Toggle Tool Bar", new BMessage(kMsgViewToggleToolbar)));
    viewMenu->AddItem(new BMenuItem("Toggle Status Bar", new BMessage(kMsgViewToggleStatusBar)));
    fMenuBar->AddItem(viewMenu);

    // Insert
    BMenu* insertMenu = new BMenu("Insert");
    insertMenu->AddItem(new BMenuItem("Page Break", new BMessage(kMsgInsertPageBreak)));
    insertMenu->AddItem(new BMenuItem("Table", new BMessage(kMsgInsertTable)));
    insertMenu->AddItem(new BMenuItem("Image...", new BMessage(kMsgInsertImage)));
    fMenuBar->AddItem(insertMenu);

    // Format
    BMenu* formatMenu = new BMenu("Format");
    formatMenu->AddItem(new BMenuItem("Bold", new BMessage(kMsgFormatBold), 'B'));
    formatMenu->AddItem(new BMenuItem("Italic", new BMessage(kMsgFormatItalic), 'I'));
    formatMenu->AddItem(new BMenuItem("Underline", new BMessage(kMsgFormatUnderline), 'U'));
    formatMenu->AddSeparatorItem();
    BMenu* alignMenu = new BMenu("Align");
    alignMenu->AddItem(new BMenuItem("Left", new BMessage(kMsgFormatAlignLeft)));
    alignMenu->AddItem(new BMenuItem("Center", new BMessage(kMsgFormatAlignCenter)));
    alignMenu->AddItem(new BMenuItem("Right", new BMessage(kMsgFormatAlignRight)));
    alignMenu->AddItem(new BMenuItem("Justify", new BMessage(kMsgFormatAlignJustify)));
    formatMenu->AddItem(alignMenu);
    fMenuBar->AddItem(formatMenu);

    // Help
    BMenu* helpMenu = new BMenu("Help");
    helpMenu->AddItem(new BMenuItem("About YellowDocs", new BMessage(kMsgHelpAbout)));
    helpMenu->AddItem(new BMenuItem("Documentation", new BMessage(kMsgHelpDocumentation)));
    fMenuBar->AddItem(helpMenu);

    fMenuBar->SetTargetForItems(this);
    AddChild(fMenuBar);
}

void YellowDocsWindow::InitToolBar()
{
    fToolBar = new YellowUI::ToolBar("ToolBar");
    fToolBar->AddButton("New", kMsgFileNew);
    fToolBar->AddButton("Open", kMsgFileOpen);
    fToolBar->AddButton("Save", kMsgFileSave);
    fToolBar->AddSeparator();
    fToolBar->AddButton("Bold", kMsgFormatBold);
    fToolBar->AddButton("Italic", kMsgFormatItalic);
    fToolBar->AddButton("Underline", kMsgFormatUnderline);
    fToolBar->AddSeparator();
    fToolBar->AddButton("Zoom +", kMsgViewZoomIn);
    fToolBar->AddButton("Zoom -", kMsgViewZoomOut);
    AddChild(fToolBar);
}

void YellowDocsWindow::InitMainView()
{
    BRect canvasFrame(0, 0, Bounds().Width(), Bounds().Height());
    fCanvas = new DocumentCanvas(canvasFrame);
    fScrollView = new BScrollView("ScrollView", fCanvas,
                                  B_FOLLOW_ALL_SIDES, 0, true, true);
    AddChild(fScrollView);
    fCanvas->SetDocument(fDocument);
}

void YellowDocsWindow::InitStatusBar()
{
    fStatusBar = new BStatusBar("StatusBar", "YellowDocs");
    AddChild(fStatusBar);
}

void YellowDocsWindow::LayoutChildren()
{
    float width = Bounds().Width();
    float height = Bounds().Height();
    float y = 0;

    if (fMenuBar) {
        float menuHeight = 25.0f;
        fMenuBar->MoveTo(0, y);
        fMenuBar->ResizeTo(width, menuHeight);
        y += menuHeight;
    }
    if (fToolBar && fToolBar->IsHidden() == false) {
        float toolHeight = 40.0f;
        fToolBar->MoveTo(0, y);
        fToolBar->ResizeTo(width, toolHeight);
        y += toolHeight;
    }
    float statusHeight = 22.0f;
    if (fStatusBar && !fStatusBar->IsHidden()) {
        fStatusBar->MoveTo(0, height - statusHeight);
        fStatusBar->ResizeTo(width, statusHeight);
        height -= statusHeight;
    }
    if (fScrollView) {
        fScrollView->MoveTo(0, y);
        fScrollView->ResizeTo(width, height - y);
    }
}

void YellowDocsWindow::FrameResized(float width, float height)
{
    (void)width;
    (void)height;
    LayoutChildren();
}

// ------------------------------------------------------------- document ops

YellowDocument* YellowDocsWindow::GetDocument()
{
    return fDocument;
}

const YellowDocument* YellowDocsWindow::GetDocument() const
{
    return fDocument;
}

bool YellowDocsWindow::IsDocumentModified() const
{
    return fDocumentModified || (fDocument && fDocument->IsModified());
}

void YellowDocsWindow::SetDocumentModified(bool modified)
{
    fDocumentModified = modified;
    if (fDocument) fDocument->SetModified(modified);
    UpdateTitle();
}

void YellowDocsWindow::NewDocument()
{
    delete fDocument;
    fDocument = new YellowDocument();
    fDocumentPath.Truncate(0);
    if (fCanvas) fCanvas->SetDocument(fDocument);
    SetDocumentModified(false);
    UpdateTitle();
    UpdateStatusBar();
}

bool YellowDocsWindow::ImportFromPath(const BString& path)
{
    // Pick the importer based on extension.
    Importer* importer = nullptr;
    YDFImporter ydf;
    MarkdownImporter markdown;
    PlainTextImporter plain;
    ODFImporter odf;

    if (path.Length() > 4 && path.FindFirst(".ydf") >= 0) {
        importer = &ydf;
    } else if (path.Length() > 3 && path.FindFirst(".md") >= 0) {
        importer = &markdown;
    } else if (path.Length() > 4 && path.FindFirst(".odt") >= 0) {
        importer = &odf;
    } else {
        importer = &plain;
    }
    return importer->Import(fDocument, path);
}

bool YellowDocsWindow::LoadDocument(const BString& path)
{
    if (!fDocument) return false;
    if (!ImportFromPath(path)) {
        BAlert* alert = new BAlert("Open failed",
                                   "Could not open the document.", "OK");
        alert->Go();
        return false;
    }
    fDocumentPath = path;
    if (fCanvas) fCanvas->SetDocument(fDocument);
    SetDocumentModified(false);
    UpdateTitle();
    UpdateStatusBar();
    return true;
}

bool YellowDocsWindow::ExportToPath(const BString& path)
{
    Exporter* exporter = nullptr;
    YDFExporter ydf;
    MarkdownExporter markdown;
    PlainTextExporter plain;
    ODFExporter odf;

    if (path.Length() > 4 && path.FindFirst(".ydf") >= 0) {
        exporter = &ydf;
    } else if (path.Length() > 3 && path.FindFirst(".md") >= 0) {
        exporter = &markdown;
    } else if (path.Length() > 4 && path.FindFirst(".odt") >= 0) {
        exporter = &odf;
    } else {
        exporter = &plain;
    }
    return exporter->Export(fDocument, path);
}

bool YellowDocsWindow::SaveDocument()
{
    if (fDocumentPath.IsEmpty()) {
        SaveDocumentAs();
        return false;
    }
    if (!ExportToPath(fDocumentPath)) {
        BAlert* alert = new BAlert("Save failed",
                                   "Could not save the document.", "OK");
        alert->Go();
        return false;
    }
    SetDocumentModified(false);
    return true;
}

void YellowDocsWindow::SaveDocumentAs()
{
    if (!fSavePanel) {
        fSavePanel = new BFilePanel(B_SAVE_PANEL, new BMessenger(this));
        fSavePanel->SetSaveText("Untitled.ydf");
    }
    fSavePanel->Show();
}

void YellowDocsWindow::CloseDocument()
{
    if (IsDocumentModified()) {
        // Simple approach: just close; a real app would prompt.
    }
    PostMessage(B_QUIT_REQUESTED);
}

void YellowDocsWindow::UpdateTitle()
{
    BString title;
    if (fDocumentPath.Length() > 0) {
        int32 slash = fDocumentPath.FindLast('/');
        if (slash >= 0 && slash + 1 < fDocumentPath.Length()) {
            title.SetTo(fDocumentPath.String() + slash + 1);
        } else {
            title = fDocumentPath;
        }
    } else {
        title = "Untitled";
    }
    title += " - YellowDocs";
    if (IsDocumentModified()) {
        title += " *";
    }
    SetTitle(title.String());
}

void YellowDocsWindow::UpdateStatusBar()
{
    if (!fStatusBar || !fDocument) return;
    char buffer[128];
    snprintf(buffer, sizeof(buffer), "%d words, %d characters, %d pages",
             fDocument->GetWordCount(), fDocument->GetCharacterCount(),
             fDocument->GetPageCount());
    fStatusBar->SetTrailingText(buffer);
}

void YellowDocsWindow::UpdateUI()
{
    UpdateTitle();
    UpdateStatusBar();
}

// ------------------------------------------------------------- file panels

void YellowDocsWindow::ShowOpenFilePanel()
{
    if (!fOpenPanel) {
        fOpenPanel = new BFilePanel(B_OPEN_PANEL, new BMessenger(this));
    }
    fOpenPanel->Show();
}

// ------------------------------------------------------------- messages

void YellowDocsWindow::MessageReceived(BMessage* message)
{
    if (!message) {
        BWindow::MessageReceived(message);
        return;
    }

    switch (message->what) {
        case kMsgDocumentEdited:
            SetDocumentModified(true);
            UpdateStatusBar();
            return;

        case kMsgFileNew:
            NewDocument();
            return;

        case kMsgFileOpen:
            ShowOpenFilePanel();
            return;

        case kMsgFileSave:
            SaveDocument();
            return;

        case kMsgFileSaveAs:
            SaveDocumentAs();
            return;

        case kMsgFileClose:
            CloseDocument();
            return;

        case kMsgFileQuit:
            PostMessage(B_QUIT_REQUESTED);
            return;

        case kMsgEditUndo:
            if (fCanvas) fCanvas->Undo();
            return;
        case kMsgEditRedo:
            if (fCanvas) fCanvas->Redo();
            return;
        case kMsgEditCut:
            if (fCanvas) fCanvas->CutToClipboard();
            return;
        case kMsgEditCopy:
            if (fCanvas) fCanvas->CopyToClipboard();
            return;
        case kMsgEditPaste:
            if (fCanvas) fCanvas->PasteFromClipboard();
            return;
        case kMsgEditSelectAll:
            if (fCanvas) fCanvas->SelectAll();
            return;

        case kMsgViewZoomIn:
            if (fCanvas) fCanvas->ZoomIn();
            return;
        case kMsgViewZoomOut:
            if (fCanvas) fCanvas->ZoomOut();
            return;
        case kMsgViewZoomReset:
            if (fCanvas) fCanvas->ZoomReset();
            return;

        case kMsgViewToggleToolbar:
            if (fToolBar) {
                fToolBar->Hide();
                LayoutChildren();
            }
            return;
        case kMsgViewToggleStatusBar:
            if (fStatusBar) {
                fStatusBar->Hide();
                LayoutChildren();
            }
            return;

        case kMsgFormatBold:
            if (fCanvas) fCanvas->ToggleBold();
            return;
        case kMsgFormatItalic:
            if (fCanvas) fCanvas->ToggleItalic();
            return;
        case kMsgFormatUnderline:
            if (fCanvas) fCanvas->ToggleUnderline();
            return;
        case kMsgFormatAlignLeft:
            if (fCanvas) {
                fCanvas->SetAlignment(ParagraphStyle::ALIGN_LEFT);
            }
            return;
        case kMsgFormatAlignCenter:
            if (fCanvas) {
                fCanvas->SetAlignment(ParagraphStyle::ALIGN_CENTER);
            }
            return;
        case kMsgFormatAlignRight:
            if (fCanvas) {
                fCanvas->SetAlignment(ParagraphStyle::ALIGN_RIGHT);
            }
            return;
        case kMsgFormatAlignJustify:
            if (fCanvas) {
                fCanvas->SetAlignment(ParagraphStyle::ALIGN_JUSTIFY);
            }
            return;

        case kMsgHelpAbout: {
            BAlert* alert = new BAlert(
                "About YellowDocs",
                "YellowDocs " YELLOWOFFICE_VERSION "\n"
                "A native word processor for Haiku.\n"
                "Part of the YellowOffice suite.",
                "OK");
            alert->Go();
            return;
        }

        case B_SAVE_REQUESTED: {
            // Save notifications carry the target directory in "directory"
            // (entry_ref) and the typed file name in "name" (string).
            entry_ref dirRef;
            BString name;
            if (message->FindRef("directory", &dirRef) == B_OK
                && message->FindString("name", &name) == B_OK
                && name.Length() > 0) {
                BEntry entry(&dirRef);
                BPath path;
                if (entry.GetPath(&path) == B_OK && path.Path()) {
                    fDocumentPath = path.Path();
                    fDocumentPath += "/";
                    fDocumentPath += name;
                    SaveDocument();
                }
            }
            return;
        }

        case B_REFS_RECEIVED: {
            entry_ref ref;
            if (message->FindRef("refs", &ref) == B_OK) {
                BEntry entry(&ref);
                BPath path;
                if (entry.GetPath(&path) == B_OK && path.Path()) {
                    LoadDocument(path.Path());
                }
            }
            return;
        }

        default:
            BWindow::MessageReceived(message);
            return;
    }
}

bool YellowDocsWindow::QuitRequested()
{
    // TODO: prompt to save if modified.
    return true;
}
