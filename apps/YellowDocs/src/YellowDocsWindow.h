#ifndef YELLOW_DOCS_WINDOW_H
#define YELLOW_DOCS_WINDOW_H

#include <Window.h>
#include <String.h>

#include <YellowCommon/Defines.h>

class BMenuBar;
class BToolBar;
class BView;
class BStatusBar;

namespace YellowDocumentCore {
    class YellowDocument;
}

/**
 * Main window class for YellowDocs
 */
class YellowDocsWindow : public BWindow {
public:
    YellowDocsWindow(BRect frame);
    virtual ~YellowDocsWindow();

    // BWindow overrides
    virtual void MessageReceived(BMessage* message);
    virtual bool QuitRequested();
    virtual void FrameResized(float width, float height);
    virtual void WorkspacesChanged(uint32 oldWorkspaces, uint32 newWorkspaces);
    virtual void ScreenChanged(BRect screenSize, color_space depth);

    // Document operations
    void NewDocument();
    void LoadDocument(const BString& path);
    void SaveDocument();
    void SaveDocumentAs();
    void CloseDocument();
    
    bool IsDocumentModified() const;
    void SetDocumentModified(bool modified);
    
    // Document access
    YellowDocumentCore::YellowDocument* GetDocument();
    const YellowDocumentCore::YellowDocument* GetDocument() const;

    // UI updates
    void UpdateTitle();
    void UpdateStatusBar();
    void UpdateUI();

    // Message constants
    static const uint32 kMsgFileNew;
    static const uint32 kMsgFileOpen;
    static const uint32 kMsgFileSave;
    static const uint32 kMsgFileSaveAs;
    static const uint32 kMsgFileClose;
    static const uint32 kMsgFileQuit;
    
    static const uint32 kMsgEditUndo;
    static const uint32 kMsgEditRedo;
    static const uint32 kMsgEditCut;
    static const uint32 kMsgEditCopy;
    static const uint32 kMsgEditPaste;
    static const uint32 kMsgEditSelectAll;
    static const uint32 kMsgEditFind;
    static const uint32 kMsgEditReplace;
    
    static const uint32 kMsgViewZoomIn;
    static const uint32 kMsgViewZoomOut;
    static const uint32 kMsgViewZoomReset;
    static const uint32 kMsgViewToggleToolbar;
    static const uint32 kMsgViewToggleStatusBar;
    
    static const uint32 kMsgInsertPageBreak;
    static const uint32 kMsgInsertTable;
    static const uint32 kMsgInsertImage;
    
    static const uint32 kMsgFormatBold;
    static const uint32 kMsgFormatItalic;
    static const uint32 kMsgFormatUnderline;
    static const uint32 kMsgFormatAlignLeft;
    static const uint32 kMsgFormatAlignCenter;
    static const uint32 kMsgFormatAlignRight;
    static const uint32 kMsgFormatAlignJustify;
    
    static const uint32 kMsgHelpAbout;
    static const uint32 kMsgHelpDocumentation;

private:
    // UI components
    BMenuBar* fMenuBar;
    BToolBar* fToolBar;
    BView* fMainView;
    BStatusBar* fStatusBar;
    
    // Document
    YellowDocumentCore::YellowDocument* fDocument;
    BString fDocumentPath;
    bool fDocumentModified;
    
    // Window state
    BString fWindowTitle;
    float fZoomLevel;
    
    // Initialize UI
    void InitWindow();
    void InitMenuBar();
    void InitToolBar();
    void InitMainView();
    void InitStatusBar();
    
    // Helper methods
    void ShowOpenFilePanel();
    void ShowSaveFilePanel();
};

#endif // YELLOW_DOCS_WINDOW_H
