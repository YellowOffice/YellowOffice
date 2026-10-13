// Stub of Haiku <Directory.h> - functional subset backed by POSIX.
#ifndef YELLOW_HAIKU_STUB_DIRECTORY_H
#define YELLOW_HAIKU_STUB_DIRECTORY_H

#include <SupportDefs.h>
#include <StorageDefs.h>
#include <FSDefs.h>
#include <Entry.h>
#include <Node.h>
#include <File.h>
#include <Path.h>
#include <String.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <cerrno>

class BFile;

class BDirectory : public BNode {
public:
	BDirectory() : fDir(NULL) {}
	BDirectory(const BEntry* entry) : fDir(NULL) { if (entry) SetTo(entry); }
	BDirectory(const entry_ref* ref) : fDir(NULL) { if (ref) SetTo(ref); }
	BDirectory(const char* path) : fDir(NULL) { if (path) SetTo(path); }
	BDirectory(const BDirectory& dir) : fDir(NULL) { SetTo(dir); }
	virtual ~BDirectory() { if (fDir) closedir(fDir); }

	using BNode::SetTo;
	using BNode::InitCheck;
	status_t SetTo(const BEntry* entry) {
		if (entry == NULL) return B_BAD_VALUE;
		BPath path;
		status_t result = const_cast<BEntry*>(entry)->GetPath(&path);
		if (result != B_OK) return result;
		return SetTo(path.Path());
	}
	status_t SetTo(const entry_ref* ref) {
		if (ref == NULL) return B_BAD_VALUE;
		return SetTo(ref->name);
	}
	status_t SetTo(const char* path) {
		status_t result = BNode::SetTo(path);
		if (result == B_OK) {
			struct stat st;
			if (stat(path, &st) == 0 && !S_ISDIR(st.st_mode)) result = B_BAD_VALUE;
		}
		fStatus = result;
		return result;
	}
	status_t SetTo(const BDirectory& dir) { return SetTo(dir.fPath); }

	status_t GetEntry(BEntry* entry) const {
		if (fStatus != B_OK || entry == NULL) return B_ERROR;
		return entry->SetTo(fPath, NULL);
	}
	status_t CreateFile(const char* name, BFile* file, bool failIfExists = false) {
		if (fStatus != B_OK || name == NULL) return B_ERROR;
		char fullPath[2048];
		snprintf(fullPath, sizeof(fullPath), "%s/%s", fPath, name);
		struct stat st;
		if (stat(fullPath, &st) == 0 && failIfExists) return B_FILE_EXISTS;
		if (file != NULL) return file->SetTo(fullPath, B_READ_WRITE | B_CREATE_FILE | B_TRUNCATE);
		int fd = ::open(fullPath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (fd < 0) return B_ERROR;
		::close(fd);
		return B_OK;
	}
	status_t CreateDirectory(const char* name, BDirectory* dir) {
		if (fStatus != B_OK || name == NULL) return B_ERROR;
		char fullPath[2048];
		snprintf(fullPath, sizeof(fullPath), "%s/%s", fPath, name);
		if (::mkdir(fullPath, 0755) != 0) return B_ERROR;
		if (dir != NULL) return dir->SetTo(fullPath);
		return B_OK;
	}
	status_t GetNextRef(entry_ref* ref) {
		if (fStatus != B_OK || ref == NULL) return B_ERROR;
		if (fDir == NULL) _InitIterator();
		if (fDir == NULL) return B_ERROR;
		struct dirent* ent = readdir(fDir);
		if (ent == NULL) return B_ENTRY_NOT_FOUND;
		struct stat st;
		char fullPath[2048];
		snprintf(fullPath, sizeof(fullPath), "%s/%s", fPath, ent->d_name);
		if (stat(fullPath, &st) != 0) return B_ERROR;
		*ref = entry_ref(st.st_dev, st.st_ino, ent->d_name);
		return B_OK;
	}
	void Rewind() { if (fDir) { closedir(fDir); fDir = NULL; } }
	int32 CountEntries() {
		if (fStatus != B_OK) return -1;
		DIR* dir = opendir(fPath);
		if (dir == NULL) return -1;
		int32 count = 0;
		while (readdir(dir) != NULL) count++;
		closedir(dir);
		return count;
	}

	static status_t Create(const char* path, bool createIt = true, bool = false) {
		if (path == NULL) return B_BAD_VALUE;
		struct stat st;
		if (stat(path, &st) == 0) return S_ISDIR(st.st_mode) ? B_OK : B_FILE_EXISTS;
		if (!createIt) return B_ENTRY_NOT_FOUND;
		return ::mkdir(path, 0755) == 0 ? B_OK : B_ERROR;
	}

private:
	void _InitIterator() { fDir = opendir(fPath); }
	DIR* fDir;
};

#endif
