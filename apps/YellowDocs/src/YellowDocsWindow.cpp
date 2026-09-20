#include "YellowDocsWindow.h"

#include <Alert.h>
#include <Application.h>
#include <Box.h>
#include <Button.h>
#include <FilePanel.h>
#include <LayoutBuilder.h>
#include <Menu.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <Message.h>
#include <Path.h>
#include <Screen.h>
#include <SeparatorView.h>
#include <StatusBar.h>
#include <String.h>
#include <ToolBar.h>
#include <View.h>

#include <YellowCommon/Logger.h>
#include <YellowDocumentCore/YellowDocument.h>

// Message constants
const uint32 YellowDocsWindow::kMsgFileNew = 'YDfnew';
const uint32 YellowDocsWindow::kMsgFileOpen = 'YDfopn';
const uint32 YellowDocsWindow::kMsgFileSave = 'YDfsav';
const uint32 YellowDocsWindow::kMsgFileSaveAs = 'YDfsva';
const uint32 YellowDocsWindow::kMsgFileClose = 'YDfcls';
const uint32 YellowDocsWindow::kMsgFileQuit = 'YDfqu';

const uint32 YellowDocsWindow::kMsgEditUndo = 'YDedun';
const uint32 YellowDocsWindow::kMsgEditRedo = 'YDedre';
const uint32 YellowDocsWindow::kMsgEditCut = 'YDedcu';
const uint32 YellowDocsWindow::kMsgEditCopy = 'YDedco';
const uint32 YellowDocsWindow::kMsgEditPaste = 'YDedpa';
const uint32 YellowDocsWindow::kMsgEditSelectAll = 'YDedsa';
const uint32 YellowDocsWindow::kMsgEditFind = 'YDedfi';
const uint32 YellowDocsWindow::kMsgEditReplace = 'YDedre';

const uint32 YellowDocsWindow::kMsgViewZoomIn = 'YDvzmi';
const uint32 YellowDocsWindow::kMsgViewZoomOut = 'YDvzmo';
const uint32 YellowDocsWindow::kMsgViewZoomReset = 'YDvzmr';
const uint32 YellowDocsWindow::kMsgViewToggleToolbar = 'YDvttb';
const uint32 YellowDocsWindow::kMsgViewToggleStatusBar = 'YDvtst';

const uint32 YellowDocsWindow::kMsgInsertPageBreak = 'YDinpb';
const uint32 YellowDocsWindow::kMsgInsertTable = 'YDintb';
const uint32 YellowDocsWindow::kMsgInsertImage = 'YDinim';

const uint32 YellowDocsWindow::kMsgFormatBold = 'YDfmtb';
const uint32 YellowDocsWindow::kMsgFormatItalic = 'YDfmti';
const uint32 YellowDocsWindow::kMsgFormatUnderline = 'YDfmtu';
const uint32 YellowDocsWindow::kMsgFormatAlignLeft = 'YDfmta';
const uint32 YellowDocsWindow::kMsgFormatAlignCenter = 'YDfmtc';
const uint32 YellowDocsWindow::kMsgFormatAlignRight = 'YDfmtr';
const uint32 YellowDocsWindow::kMsgFormatAlignJustify = 'YDfmtj';

const uint32 YellowDocsWindow::kMsgHelpAbout = 'YDhlpa';
const uint32 YellowDocsWindow::kMsgHelpDocumentation = 'YDhlpd';

// Window constants
const char* kWindowTitle = "Untitled - YellowDocs";
const float kDefaultZoom = 1.0f;
const float kZoomIncrement = 0.1f;

// YellowDocsWindow implementation
YellowDocsWindow::YellowDocsWindow(BRect frame)
    : BWindow(frame, kWindowTitle, B_TITLED_WINDOW, B_ASYNCHRONOUS_CONTROLS),
      fMenuBar(nullptr),
      fToolBar(nullptr),
      fMainView(nullptr),
      fStatusBar(nullptr),
      fDocument(new YellowDocumentCore::YellowDocument()),
      fDocumentPath(""),
      fDocumentModified(false),
      fWindowTitle(kWindowTitle),
      fZoomLevel(kDefaultZoom)
{
    YellowCommon::Logger::GetGlobalLogger().Info("Creating YellowDocs window");
    
    // Initialize the window
    InitWindow();
}

YellowDocsWindow::~YellowDocsWindow()
{
    YellowCommon::Logger::GetGlobalLogger().Info("Destroying YellowDocs window");
    
    delete fDocument;
}

void YellowDocsWindow::MessageReceived(BMessage* message)
{
    switch (message->what) {
        // File menu
        case kMsgFileNew:
            NewDocument();
            break;
            
        case kMsgFileOpen:
            ShowOpenFilePanel();
            break;
            
        case kMsgFileSave:
            SaveDocument();
            break;
            
        case kMsgFileSaveAs:
            SaveDocumentAs();
            break;
            
        case kMsgFileClose:
            Quit();
            break;
            
        case kMsgFileQuit:
            be_app->PostMessage(B_QUIT_REQUESTED);
            break;
            
        // Edit menu
        case kMsgEditUndo:
            // TODO: Implement undo
            break;
            
        case kMsgEditRedo:
            // TODO: Implement redo
            break;
            
        case kMsgEditCut:
            // TODO: Implement cut
            break;
            
        case kMsgEditCopy:
            // TODO: Implement copy
            break;
            
        case kMsgEditPaste:
            // TODO: Implement paste
            break;
            
        case kMsgEditSelectAll:
            // TODO: Implement select all
            break;
            
        case kMsgEditFind:
            // TODO: Implement find
            break;
            
        case kMsgEditReplace:
            // TODO: Implement replace
            break;
            
        // View menu
        case kMsgViewZoomIn:
            fZoomLevel += kZoomIncrement;
            UpdateStatusBar();
            break;
            
        case kMsgViewZoomOut:
            fZoomLevel -= kZoomIncrement;
            if (fZoomLevel < 0.1f) fZoomLevel = 0.1f;
            UpdateStatusBar();
            break;
            
        case kMsgViewZoomReset:
            fZoomLevel = kDefaultZoom;
            UpdateStatusBar();
            break;
            
        case kMsgViewToggleToolbar:
            // TODO: Toggle toolbar visibility
            break;
            
        case kMsgViewToggleStatusBar:
            // TODO: Toggle status bar visibility
            break;
            
        // Insert menu
        case kMsgInsertPageBreak:
            // TODO: Insert page break
            break;
            
        case kMsgInsertTable:
            // TODO: Insert table
            break;
            
        case kMsgInsertImage:
            // TODO: Insert image
            break;
            
        // Format menu
        case kMsgFormatBold:
            // TODO: Toggle bold
            break;
            
        case kMsgFormatItalic:
            // TODO: Toggle italic
            break;
            
        case kMsgFormatUnderline:
            // TODO: Toggle underline
            break;
            
        case kMsgFormatAlignLeft:
            // TODO: Set left alignment
            break;
            
        case kMsgFormatAlignCenter:
            // TODO: Set center alignment
            break;
            
        case kMsgFormatAlignRight:
            // TODO: Set right alignment
            break;
            
        case kMsgFormatAlignJustify:
            // TODO: Set justify alignment
            break;
            
        // Help menu
        case kMsgHelpAbout:
            be_app->PostMessage(B_ABOUT_REQUESTED);
            break;
            
        case kMsgHelpDocumentation:
            // TODO: Show documentation
            break;
            
        default:
            BWindow::MessageReceived(message);
            break;
    }
}

bool YellowDocsWindow::QuitRequested()
{
    YellowCommon::Logger::GetGlobalLogger().Info("Window quit requested");
    
    if (fDocumentModified) {
        BString message;
        if (fDocumentPath.IsEmpty()) {
            message = "This document has unsaved changes. Save before closing?";
        } else {
            message.SetToFormat("Document '%s' has unsaved changes. Save before closing?", fDocumentPath.String());
        }
        
        BAlert* alert = new BAlert("Unsaved Changes", message.String(), "Save", "Discard", "Cancel", B_WIDTH_AS_USUAL, B_OFFSET_SPACING, B_WARNING_ALERT);
        int32 result = alert->Go();
        
        switch (result) {
            case 0: // Save
                SaveDocument();
                if (fDocumentModified) {
                    return false; // Still modified, don't quit
                }
                break;
                
            case 1: // Discard
                // Continue with quit
                break;
                
            case 2: // Cancel
            default:
                return false;
        }
    }
    
    return true;
}

void YellowDocsWindow::FrameResized(float width, float height)
{
    BWindow::FrameResized(width, height);
    
    // TODO: Handle window resize
}

void YellowDocsWindow::WorkspacesChanged(uint32 oldWorkspaces, uint32 newWorkspaces)
{
    BWindow::WorkspacesChanged(oldWorkspaces, newWorkspaces);
    
    // TODO: Handle workspace changes
}

void YellowDocsWindow::ScreenChanged(BRect screenSize, color_space depth)
{
    BWindow::ScreenChanged(screenSize, depth);
    
    // TODO: Handle screen changes
}

void YellowDocsWindow::NewDocument()
{
    YellowCommon::Logger::GetGlobalLogger().Info("Creating new document");
    
    // Clear current document
    delete fDocument;
    fDocument = new YellowDocumentCore::YellowDocument();
    fDocumentPath = "";
    fDocumentModified = false;
    
    UpdateTitle();
    UpdateStatusBar();
    UpdateUI();
}

void YellowDocsWindow::LoadDocument(const BString& path)
{
    YellowCommon::Logger::GetGlobalLogger().Info("Loading document: %s", path.String());
    
    // TODO: Implement document loading
    // For now, just set the path
    fDocumentPath = path;
    fDocumentModified = false;
    
    UpdateTitle();
    UpdateStatusBar();
    UpdateUI();
}

void YellowDocsWindow::SaveDocument()
{
    YellowCommon::Logger::GetGlobalLogger().Info("Saving document");
    
    if (fDocumentPath.IsEmpty()) {
        SaveDocumentAs();
    } else {
        // TODO: Implement document saving
        fDocumentModified = false;
        UpdateTitle();
        UpdateStatusBar();
    }
}

void YellowDocsWindow::SaveDocumentAs()
{
    YellowCommon::Logger::GetGlobalLogger().Info("Saving document as...");
    
    ShowSaveFilePanel();
}

void YellowDocsWindow::CloseDocument()
{
    Quit();
}

bool YellowDocsWindow::IsDocumentModified() const
{
    return fDocumentModified;
}

void YellowDocsWindow::SetDocumentModified(bool modified)
{
    fDocumentModified = modified;
    UpdateTitle();
    UpdateStatusBar();
}

YellowDocumentCore::YellowDocument* YellowDocsWindow::GetDocument()
{
    return fDocument;
}

const YellowDocumentCore::YellowDocument* YellowDocsWindow::GetDocument() const
{
    return fDocument;
}

void YellowDocsWindow::UpdateTitle()
{
    BString title;
    
    if (fDocumentPath.IsEmpty()) {
        title = "Untitled";
    } else {
        BPath path(fDocumentPath.String());
        title = path.Leaf();
    }
    
    if (fDocumentModified) {
        title += " *";
    }
    
    title += " - YellowDocs";
    
    SetTitle(title.String());
    fWindowTitle = title;
}

void YellowDocsWindow::UpdateStatusBar()
{
    if (fStatusBar) {
        BString status;
        status.SetToFormat("Zoom: %.0f%% | Page: 1 | Words: 0 | Characters: 0", fZoomLevel * 100);
        fStatusBar->SetText(status.String());
    }
}

void YellowDocsWindow::UpdateUI()
{
    // Update menu items and toolbar buttons based on document state
    // TODO: Implement UI updates
}

void YellowDocsWindow::InitWindow()
{
    YellowCommon::Logger::GetGlobalLogger().Debug("Initializing window");
    
    // Set window properties
    SetSizeLimits(400, 400, 2000, 2000);
    
    // Initialize UI components
    InitMenuBar();
    InitToolBar();
    InitMainView();
    InitStatusBar();
    
    // Update UI
    UpdateTitle();
    UpdateStatusBar();
}

void YellowDocsWindow::InitMenuBar()
{
    YellowCommon::Logger::GetGlobalLogger().Debug("Initializing menu bar");
    
    fMenuBar = new BMenuBar("menubar");
    
    // File menu
    BMenu* fileMenu = new BMenu("File");
    fileMenu->AddItem(new BMenuItem("New", new BMessage(kMsgFileNew), 'N'));
    fileMenu->AddItem(new BMenuItem("Open...", new BMessage(kMsgFileOpen), 'O'));
    fileMenu->AddItem(new BMenuItem("Save", new BMessage(kMsgFileSave), 'S'));
    fileMenu->AddItem(new BMenuItem("Save As...", new BMessage(kMsgFileSaveAs)));
    fileMenu->AddSeparatorItem();
    fileMenu->AddItem(new BMenuItem("Close", new BMessage(kMsgFileClose), 'W'));
    fileMenu->AddItem(new BMenuItem("Quit", new BMessage(kMsgFileQuit), 'Q'));
    
    // Edit menu
    BMenu* editMenu = new BMenu("Edit");
    editMenu->AddItem(new BMenuItem("Undo", new BMessage(kMsgEditUndo), 'Z'));
    editMenu->AddItem(new BMenuItem("Redo", new BMessage(kMsgEditRedo), 'Y'));
    editMenu->AddSeparatorItem();
    editMenu->AddItem(new BMenuItem("Cut", new BMessage(kMsgEditCut), 'X'));
    editMenu->AddItem(new BMenuItem("Copy", new BMessage(kMsgEditCopy), 'C'));
    editMenu->AddItem(new BMenuItem("Paste", new BMessage(kMsgEditPaste), 'V'));
    editMenu->AddItem(new BMenuItem("Select All", new BMessage(kMsgEditSelectAll), 'A'));
    editMenu->AddSeparatorItem();
    editMenu->AddItem(new BMenuItem("Find...", new BMessage(kMsgEditFind), 'F'));
    editMenu->AddItem(new BMenuItem("Replace...", new BMessage(kMsgEditReplace), 'H'));
    
    // View menu
    BMenu* viewMenu = new BMenu("View");
    viewMenu->AddItem(new BMenuItem("Zoom In", new BMessage(kMsgViewZoomIn), '+', B_COMMAND_KEY));
    viewMenu->AddItem(new BMenuItem("Zoom Out", new BMessage(kMsgViewZoomOut), '-', B_COMMAND_KEY));
    viewMenu->AddItem(new BMenuItem("Reset Zoom", new BMessage(kMsgViewZoomReset), '0'));
    viewMenu->AddSeparatorItem();
    viewMenu->AddItem(new BMenuItem("Show Toolbar", new BMessage(kMsgViewToggleToolbar)));
    viewMenu->AddItem(new BMenuItem("Show Status Bar", new BMessage(kMsgViewToggleStatusBar)));
    
    // Insert menu
    BMenu* insertMenu = new BMenu("Insert");
    insertMenu->AddItem(new BMenuItem("Page Break", new BMessage(kMsgInsertPageBreak)));
    insertMenu->AddItem(new BMenuItem("Table...", new BMessage(kMsgInsertTable)));
    insertMenu->AddItem(new BMenuItem("Image...", new BMessage(kMsgInsertImage)));
    
    // Format menu
    BMenu* formatMenu = new BMenu("Format");
    formatMenu->AddItem(new BMenuItem("Bold", new BMessage(kMsgFormatBold), 'B', B_COMMAND_KEY));
    formatMenu->AddItem(new BMenuItem("Italic", new BMessage(kMsgFormatItalic), 'I', B_COMMAND_KEY));
    formatMenu->AddItem(new BMenuItem("Underline", new BMessage(kMsgFormatUnderline), 'U', B_COMMAND_KEY));
    formatMenu->AddSeparatorItem();
    formatMenu->AddItem(new BMenuItem("Align Left", new BMessage(kMsgFormatAlignLeft)));
    formatMenu->AddItem(new BMenuItem("Align Center", new BMessage(kMsgFormatAlignCenter)));
    formatMenu->AddItem(new BMenuItem("Align Right", new BMessage(kMsgFormatAlignRight)));
    formatMenu->AddItem(new BMenuItem("Justify", new BMessage(kMsgFormatAlignJustify)));
    
    // Tools menu
    BMenu* toolsMenu = new BMenu("Tools");
    // TODO: Add tools menu items
    
    // Help menu
    BMenu* helpMenu = new BMenu("Help");
    helpMenu->AddItem(new BMenuItem("About YellowDocs...", new BMessage(kMsgHelpAbout)));
    helpMenu->AddItem(new BMenuItem("Documentation", new BMessage(kMsgHelpDocumentation)));
    
    // Add menus to menu bar
    fMenuBar->AddItem(fileMenu);
    fMenuBar->AddItem(editMenu);
    fMenuBar->AddItem(viewMenu);
    fMenuBar->AddItem(insertMenu);
    fMenuBar->AddItem(formatMenu);
    fMenuBar->AddItem(toolsMenu);
    fMenuBar->AddItem(helpMenu);
    
    AddChild(fMenuBar);
}

void YellowDocsWindow::InitToolBar()
{
    YellowCommon::Logger::GetGlobalLogger().Debug("Initializing toolbar");
    
    // Create toolbar
    fToolBar = new BToolBar(B_HORIZONTAL);
    fToolBar->SetExplicitMinSize(BSize(400, 40));
    
    // Add buttons to toolbar
    // TODO: Add actual toolbar buttons with icons
    
    AddChild(fToolBar);
}

void YellowDocsWindow::InitMainView()
{
    YellowCommon::Logger::GetGlobalLogger().Debug("Initializing main view");
    
    // Create main view
    fMainView = new BView("main_view", B_WILL_DRAW | B_FRAME_EVENTS);
    fMainView->SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
    
    AddChild(fMainView);
}

void YellowDocsWindow::InitStatusBar()
{
    YellowCommon::Logger::GetGlobalLogger().Debug("Initializing status bar");
    
    fStatusBar = new BStatusBar("statusbar");
    fStatusBar->SetText("Ready");
    
    AddChild(fStatusBar);
}

void YellowDocsWindow::ShowOpenFilePanel()
{
    YellowCommon::Logger::GetGlobalLogger().Debug("Showing open file panel");
    
    BFilePanel* panel = new BFilePanel(B_OPEN_PANEL, nullptr, nullptr, B_FILE_NODE, false, new BMessage(kMsgFileOpen));
    panel->SetTarget(this);
    panel->Show();
}

void YellowDocsWindow::ShowSaveFilePanel()
{
    YellowCommon::Logger::GetGlobalLogger().Debug("Showing save file panel");
    
    BFilePanel* panel = new BFilePanel(B_SAVE_PANEL, nullptr, nullptr, B_FILE_NODE, false, new BMessage(kMsgFileSaveAs));
    panel->SetTarget(this);
    panel->Show();
}
