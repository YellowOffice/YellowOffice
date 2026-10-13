// Stub of Haiku <PrintJob.h> - declarations (Interface/Print Kit).
#ifndef YELLOW_HAIKU_STUB_PRINTJOB_H
#define YELLOW_HAIKU_STUB_PRINTJOB_H

#include <SupportDefs.h>
#include <Rect.h>
#include <Message.h>
#include <String.h>

class BView;
class BPicture;
class BBitmap;

class BPrintJob {
public:
	BPrintJob(const char* name);
	virtual ~BPrintJob();

	status_t Config();
	status_t BeginJob();
	status_t BeginPage();
	status_t ConstrainPage(float* width, float* height);
	status_t DrawView(BView* view, BRect rect, BPoint origin);
	status_t DrawBitmap(BBitmap* bitmap, BPoint at);
	status_t SeekPage(int32 page);
	void SetDefaultSettings(BMessage* settings);
	BMessage* Settings() const;
	void SpoolFile(BFile* file);
	status_t Commit();
	status_t AbortJob();
	bool IsRunning() const;
	int32 FirstPage() const;
	int32 LastPage() const;
	int32 CurPage() const;
	void SetPreview(bool on);
	void SetSettings(BMessage* settings);
};

#endif
