// Stub of Haiku <Node.h> - functional subset backed by POSIX.
#ifndef YELLOW_HAIKU_STUB_NODE_H
#define YELLOW_HAIKU_STUB_NODE_H

#include <SupportDefs.h>
#include <StorageDefs.h>
#include <FSDefs.h>
#include <String.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <fcntl.h>

struct attr_info {
	type_code type;
	size_t size;
};

class BNode {
public:
	BNode() : fStatus(B_ERROR) { fPath[0] = '\0'; }
	BNode(const char* path) : fStatus(B_ERROR) { fPath[0] = '\0'; if (path) SetTo(path); }
	explicit BNode(const entry_ref& ref) : fStatus(B_ERROR) { fPath[0] = '\0'; SetTo(ref); }
	virtual ~BNode() {}

	status_t SetTo(const char* path) {
		if (path == NULL) { fStatus = B_BAD_VALUE; return fStatus; }
		struct stat st;
		if (stat(path, &st) == 0) {
			snprintf(fPath, sizeof(fPath), "%s", path);
			fStatus = B_OK;
		} else {
			fStatus = B_ENTRY_NOT_FOUND;
		}
		return fStatus;
	}
	status_t SetTo(const entry_ref& ref) {
		// Best-effort for stub: only name-only refs in cwd are resolvable.
		return SetTo(ref.name);
	}
	status_t InitCheck() const { return fStatus; }
	status_t Unset() { fPath[0] = '\0'; fStatus = B_ERROR; return B_OK; }
	bool Exists() const {
		struct stat st;
		return fStatus == B_OK && stat(fPath, &st) == 0;
	}
	status_t GetNodeRef(node_ref* ref) const {
		if (fStatus != B_OK || ref == NULL) return B_ERROR;
		struct stat st;
		if (stat(fPath, &st) != 0) return B_ENTRY_NOT_FOUND;
		ref->device = st.st_dev;
		ref->node = st.st_ino;
		return B_OK;
	}
	status_t GetAttrInfo(const char* /*name*/, attr_info* info) const {
		if (info == NULL) return B_BAD_VALUE;
		info->type = 0;
		info->size = 0;
		return B_ENTRY_NOT_FOUND;
	}
	ssize_t ReadAttr(const char* /*name*/, type_code /*type*/, int32 /*index*/,
		void* /*buffer*/, size_t /*maxLength*/, uint32 /*flags*/ = 0) {
		return B_ENTRY_NOT_FOUND;
	}
	ssize_t WriteAttr(const char* /*name*/, type_code /*type*/, int32 /*index*/,
		const void* /*buffer*/, size_t /*length*/, uint32 /*flags*/ = 0) {
		return B_NOT_SUPPORTED;
	}
	status_t RemoveAttr(const char* /*name*/) { return B_NOT_SUPPORTED; }
	BNode& operator=(const BNode& other) {
		if (this != &other) {
			strncpy(fPath, other.fPath, sizeof(fPath) - 1);
			fPath[sizeof(fPath) - 1] = '\0';
			fStatus = other.fStatus;
		}
		return *this;
	}

protected:
	char fPath[1024];
	status_t fStatus;
};

#endif
