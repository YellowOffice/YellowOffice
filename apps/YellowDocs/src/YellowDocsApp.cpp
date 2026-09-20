#include "YellowDocsApp.h"

#include <Alert.h>
#include <Application.h>
#include <Autolock.h>
#include <Directory.h>
#include <Entry.h>
#include <File.h>
#include <FindDirectory.h>
#include <Message.h>
#include <Path.h>
#include <Roster.h>
#include <String.h>
#include <Window.h>

#include <YellowCommon/Logger.h>

#include "YellowDocsWindow.h"

// Message constants
const uint32 YellowDocsApp::kMsgNewDocument = 'YDnew';
const uint32 YellowDocsApp::kMsgOpenDocument = 'YDopen';
const uint32 YellowDocsApp::kMsgCloseAll = 'YDclsa';
const uint32 YellowDocsApp::kMsgAbout = 'YDabou';
const uint32 YellowDocsApp::kMsgQuit = 'YDquit';

// Application signature
const char* kAppSignature = "application/x-vnd.yellowdocs";

// Application info
const char* kAppName = "YellowDocs";
const char* kAppVersion = "0.1.0";
const char* kAppDescription = "YellowOffice word processor and document editor";

// YellowDocsApp implementation
YellowDocsApp::YellowDocsApp()
    : BApplication(kAppSignature),
      fFirstRun(true)
{
    YellowCommon::Logger::GetGlobalLogger().Info("YellowDocs starting up...");
    
    // Set up application info
    BString appInfo;
    appInfo.SetToFormat("%s %s", kAppName, kAppVersion);
    SetPulseRate(500000); // 0.5 seconds
}

YellowDocsApp::~YellowDocsApp()
{
    YellowCommon::Logger::GetGlobalLogger().Info("YellowDocs shutting down...");
    
    SaveSettings();
}

void YellowDocsApp::ReadyToRun()
{
    YellowCommon::Logger::GetGlobalLogger().Info("Application ready to run");
    
    // Load settings
    LoadSettings();
    
    // Create initial window if no files to open
    if (CountWindows() == 0) {
        OpenNewDocument();
    }
    
    // Inherit from BApplication
    BApplication::ReadyToRun();
}

void YellowDocsApp::AboutRequested()
{
    BString aboutText;
    aboutText.SetToFormat(
        "%s\n\n"
        "Version: %s\n\n"
        "A native word processor for Haiku OS\n"
        "Part of the YellowOffice suite\n\n"
        "Copyright (c) 2024 YellowOffice Contributors\n\n"
        "Licensed under the MIT License",
        kAppName, kAppVersion
    );
    
    BAlert* alert = new BAlert("About YellowDocs", aboutText.String(), "OK");
    alert->Go();
}

void YellowDocsApp::RefsReceived(BMessage* message)
{
    YellowCommon::Logger::GetGlobalLogger().Info("Received RefsReceived message");
    
    int32 count;
    if (message->GetInfo("refs", &count) == B_OK && count > 0) {
        entry_ref ref;
        for (int32 i = 0; i < count; i++) {
            if (message->FindRef("refs", i, &ref) == B_OK) {
                OpenDocument(&ref);
            }
        }
    }
}

void YellowDocsApp::MessageReceived(BMessage* message)
{
    switch (message->what) {
        case kMsgNewDocument:
            OpenNewDocument();
            break;
            
        case kMsgOpenDocument:
            // Open file dialog will be handled by the window
            break;
            
        case kMsgCloseAll:
            CloseAllDocuments();
            break;
            
        case kMsgAbout:
            AboutRequested();
            break;
            
        case kMsgQuit:
            Quit();
            break;
            
        default:
            BApplication::MessageReceived(message);
            break;
    }
}

bool YellowDocsApp::QuitRequested()
{
    YellowCommon::Logger::GetGlobalLogger().Info("Quit requested");
    
    // Check if any documents are modified
    for (int32 i = 0; i < CountWindows(); i++) {
        YellowDocsWindow* window = dynamic_cast<YellowDocsWindow*>(WindowAt(i));
        if (window && window->IsDocumentModified()) {
            BString message;
            message.SetToFormat("Document in window %ld has unsaved changes. Quit anyway?", i + 1);
            
            BAlert* alert = new BAlert("Unsaved Changes", message.String(), "Quit", "Cancel", nullptr, B_WIDTH_AS_USUAL, B_OFFSET_SPACING, B_WARNING_ALERT);
            int32 result = alert->Go();
            
            if (result == 1) { // Cancel
                return false;
            }
        }
    }
    
    return true;
}

void YellowDocsApp::OpenNewDocument()
{
    YellowCommon::Logger::GetGlobalLogger().Info("Creating new document");
    
    YellowDocsWindow* window = CreateNewWindow();
    if (window) {
        window->Show();
        window->Activate();
    }
}

void YellowDocsApp::OpenDocument(const entry_ref* ref)
{
    if (!ref) {
        return;
    }
    
    YellowCommon::Logger::GetGlobalLogger().Info("Opening document: %s", ref->name);
    
    BPath path(ref);
    OpenDocument(path.Path());
}

void YellowDocsApp::OpenDocument(const BString& path)
{
    if (path.IsEmpty()) {
        return;
    }
    
    YellowCommon::Logger::GetGlobalLogger().Info("Opening document from path: %s", path.String());
    
    // Check if file exists
    BEntry entry(path.String());
    if (!entry.Exists()) {
        BString errorMsg;
        errorMsg.SetToFormat("File not found: %s", path.String());
        BAlert* alert = new BAlert("Error", errorMsg.String(), "OK");
        alert->Go();
        return;
    }
    
    // Create new window and load document
    YellowDocsWindow* window = CreateNewWindow();
    if (window) {
        window->Show();
        window->LoadDocument(path);
    }
}

void YellowDocsApp::CloseAllDocuments()
{
    YellowCommon::Logger::GetGlobalLogger().Info("Closing all documents");
    
    // Close all windows
    for (int32 i = CountWindows() - 1; i >= 0; i--) {
        BWindow* window = WindowAt(i);
        if (window) {
            window->Quit();
        }
    }
}

YellowDocsWindow* YellowDocsApp::CreateNewWindow()
{
    YellowCommon::Logger::GetGlobalLogger().Debug("Creating new YellowDocs window");
    
    YellowDocsWindow* window = new YellowDocsWindow(BRect(100, 100, 800, 600));
    return window;
}

YellowDocsWindow* YellowDocsApp::GetFrontWindow() const
{
    for (int32 i = 0; i < CountWindows(); i++) {
        YellowDocsWindow* window = dynamic_cast<YellowDocsWindow*>(WindowAt(i));
        if (window && window->IsActive()) {
            return window;
        }
    }
    return nullptr;
}

size_t YellowDocsApp::GetWindowCount() const
{
    size_t count = 0;
    for (int32 i = 0; i < CountWindows(); i++) {
        if (dynamic_cast<YellowDocsWindow*>(WindowAt(i))) {
            count++;
        }
    }
    return count;
}

void YellowDocsApp::LoadSettings()
{
    YellowCommon::Logger::GetGlobalLogger().Info("Loading settings");
    
    // Try to find settings directory
    BPath settingsPath;
    if (find_directory(B_USER_SETTINGS_DIRECTORY, &settingsPath) == B_OK) {
        settingsPath.Append("YellowDocs");
        
        // Create directory if it doesn't exist
        BDirectory dir(settingsPath.Path());
        if (!dir.Exists()) {
            create_directory(settingsPath.Path(), 0755);
        }
        
        fSettingsPath = settingsPath.Path();
    }
    
    fFirstRun = false;
}

void YellowDocsApp::SaveSettings()
{
    YellowCommon::Logger::GetGlobalLogger().Info("Saving settings");
    
    // Settings will be saved by individual components
    // This is a placeholder for future settings management
}

// Application entry point
int main()
{
    YellowDocsApp app;
    app.Run();
    return 0;
}
