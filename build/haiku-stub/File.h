// Stub of Haiku <File.h> - functional subset backed by POSIX.
#ifndef YELLOW_HAIKU_STUB_FILE_H
#define YELLOW_HAIKU_STUB_FILE_H

#include <SupportDefs.h>
#include <StorageDefs.h>
#include <FSDefs.h>
#include <Node.h>
#include <Entry.h>
#include <Path.h>
#include <String.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <cstdio>
#include <cstring>
#include <cerrno>

class BFile : public BNode {
public:
	BFile() : fFd(-1), fMode(0) {}
	BFile(const entry_ref* ref, uint32 openMode) : fFd(-1), fMode(0) { SetTo(ref, openMode); }
	BFile(const char* path, uint32 openMode) : fFd(-1), fMode(0) { SetTo(path, openMode); }
	BFile(const BEntry* entry, uint32 openMode) : fFd(-1), fMode(0) { SetTo(entry, openMode); }
	virtual ~BFile() { Unset(); }

	status_t SetTo(const entry_ref* ref, uint32 openMode) {
		if (ref == NULL) return SetTo((const char*)NULL, openMode);
		return SetTo(ref->name, openMode);
	}
	status_t SetTo(const char* path, uint32 openMode) {
		Unset();
		if (path == NULL) { fStatus = B_BAD_VALUE; return fStatus; }
		int flags = 0;
		if ((openMode & B_READ_WRITE) == B_READ_WRITE) flags = O_RDWR;
		else if (openMode & B_WRITE_ONLY) flags = O_WRONLY;
		else flags = O_RDONLY;
		if (openMode & B_CREATE_FILE) flags |= O_CREAT;
		if (openMode & B_TRUNCATE) flags |= O_TRUNC;
		fFd = ::open(path, flags, 0644);
		if (fFd < 0) {
			fStatus = (errno == EACCES) ? B_PERMISSION_DENIED
				: (errno == ENOENT) ? B_ENTRY_NOT_FOUND : B_ERROR;
			return fStatus;
		}
		fMode = openMode;
		fStatus = B_OK;
		snprintf(fPath, sizeof(fPath), "%s", path);
		return B_OK;
	}
	status_t SetTo(const BEntry* entry, uint32 openMode) {
		if (entry == NULL) return B_BAD_VALUE;
		char buffer[1024];
		BPath path;
		if (const_cast<BEntry*>(entry)->GetPath(&path) != B_OK || path.Path() == NULL)
			return B_ERROR;
		snprintf(buffer, sizeof(buffer), "%s", path.Path());
		return SetTo(buffer, openMode);
	}
	using BNode::SetTo;
	void Unset() {
		if (fFd >= 0) { ::close(fFd); fFd = -1; }
	}
	status_t GetSize(off_t* size) const {
		if (fFd < 0 || size == NULL) return B_ERROR;
		struct stat st;
		if (fstat(fFd, &st) != 0) return B_ERROR;
		*size = st.st_size;
		return B_OK;
	}
	ssize_t Read(void* buffer, size_t size) {
		if (fFd < 0 || buffer == NULL) return B_ERROR;
		return ::read(fFd, buffer, size);
	}
	ssize_t Write(const void* buffer, size_t size) {
		if (fFd < 0 || buffer == NULL) return B_ERROR;
		return ::write(fFd, buffer, size);
	}
	off_t Seek(off_t position, uint32 seekMode) {
		if (fFd < 0) return -1;
		int whence = (seekMode == B_SEEK_FROM_END) ? SEEK_END
			: (seekMode == B_SEEK_FROM_CURRENT) ? SEEK_CUR : SEEK_SET;
		return ::lseek(fFd, position, whence);
	}
	off_t Position() const { return fFd >= 0 ? ::lseek(fFd, 0, SEEK_CUR) : -1; }
	status_t SetSize(off_t size) {
		if (fFd < 0) return B_ERROR;
		return ::ftruncate(fFd, size) == 0 ? B_OK : B_ERROR;
	}
	status_t Flush() { return B_OK; }
	status_t Sync() { return fFd >= 0 ? (fsync(fFd) == 0 ? B_OK : B_ERROR) : B_ERROR; }

	enum {
		B_SEEK_FROM_START = 0,
		B_SEEK_FROM_END = 1,
		B_SEEK_FROM_CURRENT = 2,
	};

private:
	int fFd;
	uint32 fMode;
};

#endif
