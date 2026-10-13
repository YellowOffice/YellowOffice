#ifndef YELLOW_DOCS_WINDOW_H
#define YELLOW_DOCS_WINDOW_H

#include <Window.h>
#include <String.h>

#include <YellowCommon/Defines.h>

class BMenuBar;
class BView;
class BStatusBar;
class BScrollView;
class BFilePanel;
class DocumentCanvas;

namespace YellowUI {
    class ToolBar;
}

namespace YellowDocumentCore {
    class YellowDocument;
}

/**
 * Main window for YellowDocs: menu bar + tool bar + document canvas +
 * status bar. Owns the YellowDocument and drives import/export.
 */
class YellowDocsWindow : public BWindow {
public:
    YellowDocsWindow(BRect frame);
    virtual ~YellowDocsWindow();

    // BWindow overrides
    virtual void MessageReceived(BMessage* message);
    virtual bool QuitRequested();
    virtual void FrameResized(float width, float height);

    // Document operations
    void NewDocument();
    bool LoadDocument(const BString& path);
    bool SaveDocument();
    void SaveDocumentAs();
    void CloseDocument();

    bool IsDocumentModified() const;
    void SetDocumentModified(bool modified);

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

    // Sent by DocumentCanvas after each edit.
    static const uint32 kMsgDocumentEdited;

private:
    void InitWindow();
    void InitMenuBar();
    void InitToolBar();
    void InitMainView();
    void InitStatusBar();
    void LayoutChildren();

    void ShowOpenFilePanel();
    void ShowSaveFilePanel();
    bool ImportFromPath(const BString& path);
    bool ExportToPath(const BString& path);

    BMenuBar* fMenuBar;
    YellowUI::ToolBar* fToolBar;
    BScrollView* fScrollView;
    DocumentCanvas* fCanvas;
    BStatusBar* fStatusBar;
    BFilePanel* fOpenPanel;
    BFilePanel* fSavePanel;

    YellowDocumentCore::YellowDocument* fDocument;
    BString fDocumentPath;
    bool fDocumentModified;
    float fZoomLevel;
};

#endif // YELLOW_DOCS_WINDOW_H
