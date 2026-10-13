#include "YellowDocsApp.h"
#include "YellowDocsWindow.h"

#include <Alert.h>
#include <Entry.h>
#include <Path.h>
#include <Message.h>
#include <Roster.h>
#include <Screen.h>
#include <vector>

const uint32 YellowDocsApp::kMsgNewDocument = 'YDnw';
const uint32 YellowDocsApp::kMsgOpenDocument = 'YDop';
const uint32 YellowDocsApp::kMsgCloseAll = 'YDca';
const uint32 YellowDocsApp::kMsgAbout = 'YDab';
const uint32 YellowDocsApp::kMsgQuit = 'YDqt';

YellowDocsApp::YellowDocsApp()
    : BApplication(YELLOWDOCS_APP_SIGNATURE)
{
}

YellowDocsApp::~YellowDocsApp()
{
}

void YellowDocsApp::ReadyToRun()
{
    OpenNewDocument();
}

void YellowDocsApp::AboutRequested()
{
    BAlert* alert = new BAlert(
        "About YellowDocs",
        "YellowDocs " YELLOWOFFICE_VERSION "\n"
        "A native word processor for Haiku.\n"
        "Part of the YellowOffice suite.",
        "OK");
    alert->Go();
}

void YellowDocsApp::RefsReceived(BMessage* message)
{
    if (!message) return;
    entry_ref ref;
    if (message->FindRef("refs", &ref) == B_OK) {
        OpenDocument(&ref);
    }
}

void YellowDocsApp::MessageReceived(BMessage* message)
{
    if (!message) {
        BApplication::MessageReceived(message);
        return;
    }
    switch (message->what) {
        case kMsgNewDocument:
            OpenNewDocument();
            return;
        case kMsgOpenDocument:
            // Handled by window's open panel; nothing to do at app level.
            return;
        case kMsgAbout:
            AboutRequested();
            return;
        case kMsgQuit:
            PostMessage(B_QUIT_REQUESTED);
            return;
        default:
            BApplication::MessageReceived(message);
            return;
    }
}

bool YellowDocsApp::QuitRequested()
{
    CloseAllDocuments();
    return true;
}

void YellowDocsApp::OpenNewDocument()
{
    CreateNewWindow();
}

void YellowDocsApp::OpenDocument(const entry_ref* ref)
{
    if (!ref) return;
    BEntry entry(*ref);
    BPath path;
    if (entry.GetPath(&path) != B_OK || !path.Path()) return;
    OpenDocument(BString(path.Path()));
}

void YellowDocsApp::OpenDocument(const BString& path)
{
    YellowDocsWindow* window = CreateNewWindow();
    if (window) {
        window->LoadDocument(path);
    }
}

void YellowDocsApp::CloseAllDocuments()
{
    std::vector<YellowDocsWindow*> windows = fWindows;
    for (YellowDocsWindow* window : windows) {
        if (window) {
            window->PostMessage(B_QUIT_REQUESTED);
        }
    }
    fWindows.clear();
}

YellowDocsWindow* YellowDocsApp::CreateNewWindow()
{
    BRect frame(80, 80, 80 + 800, 80 + 600);
    // Place the window on the main screen if possible.
    BScreen screen(B_MAIN_SCREEN_ID);
    BRect screenFrame = screen.Frame();
    frame.OffsetBy(screenFrame.left + 40 - frame.left,
                   screenFrame.top + 40 - frame.top);

    YellowDocsWindow* window = new YellowDocsWindow(frame);
    fWindows.push_back(window);
    window->Show();
    return window;
}

YellowDocsWindow* YellowDocsApp::GetFrontWindow() const
{
    if (fWindows.empty()) return nullptr;
    return fWindows.back();
}

size_t YellowDocsApp::GetWindowCount() const
{
    return fWindows.size();
}

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;
    YellowDocsApp app;
    app.Run();
    return 0;
}
