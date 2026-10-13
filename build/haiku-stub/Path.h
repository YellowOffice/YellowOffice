// Stub of Haiku <Path.h> - functional.
#ifndef YELLOW_HAIKU_STUB_PATH_H
#define YELLOW_HAIKU_STUB_PATH_H

#include <SupportDefs.h>
#include <StorageDefs.h>
#include <Entry.h>
#include <String.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>

class BPath {
public:
	BPath() : fStatus(B_ERROR) { fPath[0] = '\0'; }
	BPath(const char* dir, const char* leaf = NULL, bool = false) : fStatus(B_ERROR) {
		fPath[0] = '\0';
		SetTo(dir, leaf);
	}
	BPath(const entry_ref* dir, const char* leaf = NULL, bool = false) : fStatus(B_ERROR) {
		fPath[0] = '\0';
		if (dir != NULL) {
			// Stub: resolve relative to cwd by leaf name.
			SetTo(dir->name, leaf);
		}
	}
	explicit BPath(const BEntry* entry) : fStatus(B_ERROR) {
		fPath[0] = '\0';
		if (entry != NULL) {
			char buffer[1024];
			// Best effort: reconstruct via GetRef leaf name.
			entry_ref ref;
			if (const_cast<BEntry*>(entry)->GetRef(&ref) == B_OK) {
				SetTo(ref.name, NULL);
			}
			(void)buffer;
		}
	}
	virtual ~BPath() {}

	status_t SetTo(const char* dir, const char* leaf = NULL, bool = false) {
		if (dir == NULL) { fStatus = B_BAD_VALUE; return fStatus; }
		if (leaf != NULL && leaf[0] != '\0') {
			size_t len = strlen(dir);
			if (len > 0 && dir[len - 1] == '/')
				snprintf(fPath, sizeof(fPath), "%s%s", dir, leaf);
			else
				snprintf(fPath, sizeof(fPath), "%s/%s", dir, leaf);
		} else {
			snprintf(fPath, sizeof(fPath), "%s", dir);
		}
		fStatus = B_OK;
		return fStatus;
	}
	status_t SetTo(const entry_ref* dir, const char* leaf = NULL, bool = false) {
		if (dir == NULL) { fStatus = B_BAD_VALUE; return fStatus; }
		return SetTo(dir->name, leaf);
	}
	status_t Unset() { fPath[0] = '\0'; fStatus = B_ERROR; return B_OK; }
	status_t InitCheck() const { return fStatus; }
	const char* Path() const { return fStatus == B_OK ? fPath : NULL; }
	const char* Leaf() const {
		if (fStatus != B_OK) return NULL;
		const char* leaf = strrchr(fPath, '/');
		return leaf ? leaf + 1 : fPath;
	}
	status_t GetParent(BPath* parent) const {
		if (fStatus != B_OK || parent == NULL) return B_ERROR;
		char buffer[1024];
		snprintf(buffer, sizeof(buffer), "%s", fPath);
		char* slash = strrchr(buffer, '/');
		if (slash == NULL || slash == buffer) return B_ERROR;
		*slash = '\0';
		return parent->SetTo(buffer, NULL);
	}
	void Append(const char* leaf, bool = false) {
		if (fStatus != B_OK || leaf == NULL) return;
		size_t len = strlen(fPath);
		if (len > 0 && fPath[len - 1] == '/')
			snprintf(fPath + len, sizeof(fPath) - len, "%s", leaf);
		else
			snprintf(fPath + len, sizeof(fPath) - len, "/%s", leaf);
	}
	void PopParent() {
		if (fStatus != B_OK) return;
		char* slash = strrchr(fPath, '/');
		if (slash != NULL) *slash = '\0';
	}
	bool IsAbsolute() const { return fStatus == B_OK && fPath[0] == '/'; }
	BPath& operator=(const BPath& other) {
		if (this != &other) {
			strncpy(fPath, other.fPath, sizeof(fPath) - 1);
			fPath[sizeof(fPath) - 1] = '\0';
			fStatus = other.fStatus;
		}
		return *this;
	}

private:
	char fPath[1024];
	status_t fStatus;
};

inline status_t BEntry::GetPath(BPath* path) const {
	if (fStatus != B_OK || path == NULL) return B_ERROR;
	return path->SetTo(fPath, NULL);
}

#endif
