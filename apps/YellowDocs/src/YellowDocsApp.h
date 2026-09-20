#ifndef YELLOW_DOCS_APP_H
#define YELLOW_DOCS_APP_H

#include <Application.h>
#include <String.h>

#include <YellowCommon/Defines.h>

class YellowDocsWindow;

/**
 * Main application class for YellowDocs
 */
class YellowDocsApp : public BApplication {
public:
    YellowDocsApp();
    virtual ~YellowDocsApp();

    // BApplication overrides
    virtual void ReadyToRun();
    virtual void AboutRequested();
    virtual void RefsReceived(BMessage* message);
    virtual void MessageReceived(BMessage* message);
    virtual bool QuitRequested();

    // Application lifecycle
    void OpenNewDocument();
    void OpenDocument(const entry_ref* ref);
    void OpenDocument(const BString& path);
    void CloseAllDocuments();

    // Window management
    YellowDocsWindow* CreateNewWindow();
    YellowDocsWindow* GetFrontWindow() const;
    size_t GetWindowCount() const;

    // Settings
    void LoadSettings();
    void SaveSettings();

private:
    // Message constants
    static const uint32 kMsgNewDocument;
    static const uint32 kMsgOpenDocument;
    static const uint32 kMsgCloseAll;
    static const uint32 kMsgAbout;
    static const uint32 kMsgQuit;

    // Application state
    BString fSettingsPath;
    bool fFirstRun;
};

#endif // YELLOW_DOCS_APP_H
