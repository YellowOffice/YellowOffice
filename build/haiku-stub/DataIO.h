// Stub of Haiku <DataIO.h> - declarations + functional BMemoryIO.
#ifndef YELLOW_HAIKU_STUB_DATAIO_H
#define YELLOW_HAIKU_STUB_DATAIO_H

#include <SupportDefs.h>
#include <String.h>
#include <cstring>
#include <cstdlib>

class BDataIO {
public:
	virtual ~BDataIO() {}
	virtual ssize_t Read(void* buffer, size_t size) = 0;
	virtual ssize_t Write(const void* buffer, size_t size) = 0;
};

class BPositionIO : public BDataIO {
public:
	virtual ~BPositionIO() {}
	virtual off_t Seek(off_t position, uint32 seekMode) = 0;
	virtual off_t Position() const = 0;
	virtual status_t SetSize(off_t size) = 0;
	virtual status_t GetSize(off_t* size) const = 0;

	enum {
		B_SEEK_FROM_START = 0,
		B_SEEK_FROM_END = 1,
		B_SEEK_FROM_CURRENT = 2,
	};
};

class BMemoryIO : public BPositionIO {
public:
	BMemoryIO(void* buffer, size_t maxSize)
		: fBuffer((char*)buffer), fSize(maxSize), fLength(0), fPosition(0),
		  fReadOnly(false), fOwnsBuffer(false) {}
	BMemoryIO(const void* buffer, size_t size)
		: fBuffer((char*)buffer), fSize(size), fLength(size), fPosition(0),
		  fReadOnly(true), fOwnsBuffer(false) {}
	virtual ~BMemoryIO() { if (fOwnsBuffer) free(fBuffer); }

	virtual ssize_t Read(void* buffer, size_t size) {
		if (fReadOnly && buffer == NULL) return B_BAD_VALUE;
		size_t available = (fPosition < (off_t)fLength) ? fLength - (size_t)fPosition : 0;
		size_t toRead = size < available ? size : available;
		if (buffer != NULL && toRead > 0) memcpy(buffer, fBuffer + fPosition, toRead);
		fPosition += toRead;
		return (ssize_t)toRead;
	}
	virtual ssize_t Write(const void* buffer, size_t size) {
		if (fReadOnly || buffer == NULL) return B_BAD_VALUE;
		if ((size_t)fPosition + size > fSize) return B_BUFFER_OVERFLOW;
		memcpy(fBuffer + fPosition, buffer, size);
		fPosition += size;
		if ((size_t)fPosition > fLength) fLength = (size_t)fPosition;
		return (ssize_t)size;
	}
	virtual off_t Seek(off_t position, uint32 seekMode) {
		off_t base = (seekMode == B_SEEK_FROM_END) ? (off_t)fLength
			: (seekMode == B_SEEK_FROM_CURRENT) ? fPosition : 0;
		off_t target = base + position;
		if (target < 0) target = 0;
		fPosition = target;
		return fPosition;
	}
	virtual off_t Position() const { return fPosition; }
	virtual status_t SetSize(off_t size) {
		if (fReadOnly) return B_BAD_VALUE;
		if (size < 0 || (size_t)size > fSize) return B_BAD_VALUE;
		fLength = (size_t)size;
		if (fPosition > fLength) fPosition = fLength;
		return B_OK;
	}
	virtual status_t GetSize(off_t* size) const {
		if (size) *size = (off_t)fLength;
		return B_OK;
	}
	ssize_t ReadAt(off_t position, void* buffer, size_t size) {
		if (position < 0 || (size_t)position >= fLength) return 0;
		size_t available = fLength - (size_t)position;
		size_t toRead = size < available ? size : available;
		if (buffer != NULL && toRead > 0) memcpy(buffer, fBuffer + position, toRead);
		return (ssize_t)toRead;
	}
	ssize_t WriteAt(off_t position, const void* buffer, size_t size) {
		if (fReadOnly || buffer == NULL || position < 0) return B_BAD_VALUE;
		if ((size_t)position + size > fSize) return B_BUFFER_OVERFLOW;
		memcpy(fBuffer + position, buffer, size);
		if ((size_t)position + size > fLength) fLength = (size_t)position + size;
		return (ssize_t)size;
	}
	size_t BufferLength() const { return fLength; }
	void* Buffer() const { return fBuffer; }

private:
	char* fBuffer;
	size_t fSize;
	size_t fLength;
	off_t fPosition;
	bool fReadOnly;
	bool fOwnsBuffer;
};

#endif
