// Stub of Haiku <Entry.h> - functional subset backed by POSIX.
#ifndef YELLOW_HAIKU_STUB_ENTRY_H
#define YELLOW_HAIKU_STUB_ENTRY_H

#include <SupportDefs.h>
#include <StorageDefs.h>
#include <FSDefs.h>
#include <Node.h>
#include <String.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <cerrno>

class BPath;

class BStatable {
public:
	virtual ~BStatable() {}
	virtual status_t GetStat(struct stat* stat) const = 0;
	bool IsFile() const { struct stat st; return GetStat(&st) == B_OK && S_ISREG(st.st_mode); }
	bool IsDirectory() const { struct stat st; return GetStat(&st) == B_OK && S_ISDIR(st.st_mode); }
	bool IsSymLink() const { struct stat st; return GetStat(&st) == B_OK && S_ISLNK(st.st_mode); }
	status_t GetCreationTime(time_t* time) const {
		struct stat st;
		if (GetStat(&st) != B_OK) return B_ERROR;
		if (time) *time = st.st_ctime;
		return B_OK;
	}
	status_t GetModificationTime(time_t* time) const {
		struct stat st;
		if (GetStat(&st) != B_OK) return B_ERROR;
		if (time) *time = st.st_mtime;
		return B_OK;
	}
	status_t GetAccessTime(time_t* time) const {
		struct stat st;
		if (GetStat(&st) != B_OK) return B_ERROR;
		if (time) *time = st.st_atime;
		return B_OK;
	}
};

class BEntry {
public:
	BEntry() : fStatus(B_ERROR) { fPath[0] = '\0'; }
	BEntry(const char* path, const char* name = NULL, bool = false)
		: fStatus(B_ERROR) {
		fPath[0] = '\0';
		SetTo(path, name);
	}
	// Real Haiku: BEntry(const entry_ref* ref, bool traverse = false)
	explicit BEntry(const entry_ref* ref, bool traverse = false) : fStatus(B_ERROR) {
		fPath[0] = '\0';
		if (ref != NULL) SetTo(ref, traverse);
	}
	virtual ~BEntry() {}

	status_t SetTo(const char* path, const char* name = NULL, bool = false) {
		if (path == NULL) { fStatus = B_BAD_VALUE; return fStatus; }
		if (name != NULL) {
			snprintf(fPath, sizeof(fPath), "%s/%s", path, name);
		} else {
			snprintf(fPath, sizeof(fPath), "%s", path);
		}
		struct stat st;
		fStatus = (lstat(fPath, &st) == 0) ? B_OK : B_ENTRY_NOT_FOUND;
		return fStatus;
	}
	// Simplified: uses ref.name only (no directory resolution).
	status_t SetTo(const entry_ref* ref, bool traverse = false) {
		if (ref == NULL) { fStatus = B_BAD_VALUE; return fStatus; }
		(void)traverse;
		return SetTo(ref->name);
	}
	status_t Unset() { fPath[0] = '\0'; fStatus = B_ERROR; return B_OK; }
	status_t InitCheck() const { return fStatus; }
	bool Exists() const {
		struct stat st;
		return fStatus == B_OK && lstat(fPath, &st) == 0;
	}
	status_t GetRef(entry_ref* ref) const {
		if (fStatus != B_OK || ref == NULL) return B_ERROR;
		struct stat st;
		if (stat(fPath, &st) != 0) return B_ENTRY_NOT_FOUND;
		const char* leaf = strrchr(fPath, '/');
		leaf = leaf ? leaf + 1 : fPath;
		*ref = entry_ref(st.st_dev, st.st_ino, leaf);
		return B_OK;
	}
	// Defined in Path.h (needs complete BPath).
	status_t GetPath(BPath* path) const;
	status_t GetName(char* name) const {
		if (fStatus != B_OK || name == NULL) return B_ERROR;
		const char* leaf = strrchr(fPath, '/');
		leaf = leaf ? leaf + 1 : fPath;
		strcpy(name, leaf);
		return B_OK;
	}
	status_t GetLeafName(BString* name) const {
		if (fStatus != B_OK || name == NULL) return B_ERROR;
		const char* leaf = strrchr(fPath, '/');
		leaf = leaf ? leaf + 1 : fPath;
		*name = leaf;
		return B_OK;
	}
	status_t Rename(const char* path, bool = false) {
		if (fStatus != B_OK || path == NULL) return B_ERROR;
		if (::rename(fPath, path) != 0) return B_ERROR;
		snprintf(fPath, sizeof(fPath), "%s", path);
		return B_OK;
	}
	status_t Remove() const {
		if (fStatus != B_OK) return B_ERROR;
		return ::unlink(fPath) == 0 ? B_OK : B_ERROR;
	}
	status_t GetModificationTime(time_t* time) const {
		struct stat st;
		if (fStatus != B_OK || stat(fPath, &st) != 0) return B_ERROR;
		if (time) *time = st.st_mtime;
		return B_OK;
	}
	status_t GetCreationTime(time_t* time) const {
		struct stat st;
		if (fStatus != B_OK || stat(fPath, &st) != 0) return B_ERROR;
		if (time) *time = st.st_ctime;
		return B_OK;
	}
	BEntry& operator=(const BEntry& other) {
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

#endif
