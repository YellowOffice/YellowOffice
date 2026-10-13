// Stub of Haiku <String.h> - functional, header-only, C-storage based.
//
// NOTE: this file is also picked up when libc++ does #include <string.h>
// on case-insensitive filesystems, so it must (a) pull in the real C string
// header via include_next and (b) avoid including any C++ standard headers
// (they may be mid-include when this file is reached).
#ifndef YELLOW_HAIKU_STUB_STRING_H
#define YELLOW_HAIKU_STUB_STRING_H

#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
#include_next <string.h>
#elif defined(__linux__)
#include_next <string.h>
#endif

#include <SupportDefs.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

class BString {
public:
	BString() : fBuffer(NULL), fLength(0), fCapacity(0) { SetTo(""); }
	BString(const char* string) : fBuffer(NULL), fLength(0), fCapacity(0) { SetTo(string); }
	BString(const BString& other) : fBuffer(NULL), fLength(0), fCapacity(0) { SetTo(other.String()); }
	BString(const char* string, int32 maxLength) : fBuffer(NULL), fLength(0), fCapacity(0) {
		SetTo(string, maxLength);
	}
	~BString() { free(fBuffer); }

	BString& operator=(const BString& other) { if (this != &other) SetTo(other.String()); return *this; }
	BString& operator=(const char* string) { SetTo(string); return *this; }

	status_t SetTo(const char* string) {
		if (!string) string = "";
		return Adopt(string, strlen(string));
	}
	status_t SetTo(const char* string, int32 maxLength) {
		if (!string) string = "";
		if (maxLength < 0) return B_BAD_VALUE;
		size_t len = strlen(string);
		if ((size_t)maxLength < len) len = (size_t)maxLength;
		return Adopt(string, len);
	}

	const char* String() const { return fBuffer ? fBuffer : ""; }
	const char* CharAt(int32 index) const {
		if (index < 0 || (size_t)index >= fLength) return NULL;
		return fBuffer + index;
	}
	char operator[](int32 index) const {
		if (index < 0 || (size_t)index >= fLength) return '\0';
		return fBuffer[index];
	}

	int32 Length() const { return (int32)fLength; }
	int32 CountChars() const { return (int32)fLength; }
	bool IsEmpty() const { return fLength == 0; }
	bool IsNull() const { return fBuffer == NULL; }

	status_t Append(const char* string) { return Insert(string, -1); }
	status_t Append(const char* string, int32 length) {
		if (!string) return B_BAD_VALUE;
		if (length < 0) length = (int32)strlen(string);
		return InsertBytes(fLength, string, length);
	}
	status_t Append(const BString& other) { return Append(other.String()); }
	status_t Append(char c) { return InsertBytes(fLength, &c, 1); }
	status_t Prepend(const char* string) { return Insert(string, 0); }
	status_t Prepend(char c) { return InsertBytes(0, &c, 1); }
	status_t Insert(const char* string, int32 at) {
		if (!string) return B_BAD_VALUE;
		return InsertBytes(at, string, (int32)strlen(string));
	}
	status_t Insert(const BString& other, int32 at) { return Insert(other.String(), at); }
	status_t Insert(char c, int32 at) { return InsertBytes(at, &c, 1); }

	status_t InsertBytes(int32 at, const char* bytes, int32 length) {
		if (length < 0) return B_BAD_VALUE;
		if (at < 0) at = (int32)fLength + at + 1;
		if (at < 0 || (size_t)at > fLength) return B_BAD_VALUE;
		if (length == 0) return B_OK;
		Reserve(fLength + (size_t)length + 1);
		memmove(fBuffer + at + length, fBuffer + at, fLength - (size_t)at);
		memmove(fBuffer + at, bytes, (size_t)length);
		fLength += (size_t)length;
		fBuffer[fLength] = '\0';
		return B_OK;
	}

	status_t Remove(int32 from, int32 length) {
		if (from < 0 || length < 0 || (size_t)from > fLength) return B_BAD_VALUE;
		if ((size_t)(from + length) > fLength) length = (int32)(fLength - from);
		memmove(fBuffer + from, fBuffer + from + length, fLength - (size_t)(from + length));
		fLength -= (size_t)length;
		fBuffer[fLength] = '\0';
		return B_OK;
	}
	status_t Truncate(int32 length, bool min = false) {
		if (length < 0) return B_BAD_VALUE;
		if ((size_t)length > fLength) {
			if (!min) {
				Reserve((size_t)length + 1);
				memset(fBuffer + fLength, ' ', (size_t)length - fLength);
				fLength = (size_t)length;
				fBuffer[fLength] = '\0';
			}
			return B_OK;
		}
		fLength = (size_t)length;
		if (fBuffer) fBuffer[fLength] = '\0';
		return B_OK;
	}

	status_t SetToFormat(const char* format, ...) {
		va_list args;
		va_start(args, format);
		status_t result = B_OK;
		va_list copy;
		va_copy(copy, args);
		int needed = vsnprintf(NULL, 0, format, copy);
		va_end(copy);
		if (needed < 0) {
			result = B_BAD_VALUE;
		} else {
			Reserve((size_t)needed + 1);
			vsnprintf(fBuffer, (size_t)needed + 1, format, args);
			fLength = (size_t)needed;
		}
		va_end(args);
		return result;
	}

	int32 Compare(const char* other) const { return strcmp(String(), other ? other : ""); }
	int32 Compare(const BString& other) const { return Compare(other.String()); }
	int32 CompareIgnoreCase(const char* other) const {
		return strcasecmp(String(), other ? other : "");
	}
	int32 ICompare(const char* other) const { return CompareIgnoreCase(other); }
	int32 ICompare(const BString& other) const { return CompareIgnoreCase(other.String()); }
	bool operator==(const BString& other) const { return Compare(other) == 0; }
	bool operator==(const char* other) const { return Compare(other) == 0; }
	bool operator!=(const BString& other) const { return Compare(other) != 0; }
	bool operator!=(const char* other) const { return Compare(other) != 0; }
	bool operator<(const BString& other) const { return Compare(other) < 0; }
	bool operator>(const BString& other) const { return Compare(other) > 0; }

	BString& operator+=(const BString& other) { Append(other); return *this; }
	BString& operator+=(const char* other) { Append(other); return *this; }
	BString& operator+=(char c) { Append(c); return *this; }
	friend BString operator+(const BString& a, const BString& b) {
		BString result(a);
		result.Append(b);
		return result;
	}
	friend BString operator+(const BString& a, const char* b) {
		BString result(a);
		result.Append(b);
		return result;
	}

	int32 FindFirst(const char* string) const {
		if (!string) return -1;
		const char* found = strstr(String(), string);
		return found ? (int32)(found - fBuffer) : -1;
	}
	int32 FindFirst(char c) const {
		const char* found = strchr(String(), c);
		return found ? (int32)(found - fBuffer) : -1;
	}
	int32 FindFirst(char c, int32 fromOffset) const {
		if (fromOffset < 0 || (size_t)fromOffset >= fLength) return -1;
		const char* found = strchr(String() + fromOffset, c);
		return found ? (int32)(found - fBuffer) : -1;
	}
	int32 FindFirst(const char* string, int32 fromOffset) const {
		if (!string || fromOffset < 0 || (size_t)fromOffset >= fLength) return -1;
		const char* found = strstr(String() + fromOffset, string);
		return found ? (int32)(found - fBuffer) : -1;
	}
	int32 FindFirst(const BString& other) const { return FindFirst(other.String()); }
	int32 ReverseFind(char c) const {
		const char* found = strrchr(String(), c);
		return found ? (int32)(found - fBuffer) : -1;
	}
	int32 FindLast(char c) const { return ReverseFind(c); }
	int32 FindLast(const char* string) const {
		if (!string) return -1;
		const char* found = strstr(String(), string);
		if (!found) return -1;
		int32 last = -1;
		while (found) {
			last = (int32)(found - fBuffer);
			found = strstr(found + 1, string);
		}
		return last;
	}

	bool IContains(const BString& other) const {
		const char* found = strcasestr(String(), other.String());
		return found != NULL;
	}
	bool IContains(const char* other) const {
		const char* found = strcasestr(String(), other);
		return found != NULL;
	}

	BString& operator<<(const BString& other) { Append(other); return *this; }
	BString& operator<<(const char* other) { Append(other); return *this; }
	BString& operator<<(char c) { Append(c); return *this; }

private:
	// Real BString has no Adopt(const char*, int32); SetTo() copies inline.
	status_t Adopt(const char* string, int32 maxLength) {
		if (maxLength < 0) return B_BAD_VALUE;
		Reserve((size_t)maxLength + 1);
		if (string && maxLength > 0) memmove(fBuffer, string, (size_t)maxLength);
		fBuffer[maxLength > 0 ? maxLength : 0] = '\0';
		fLength = maxLength > 0 ? (size_t)maxLength : 0;
		return B_OK;
	}

	status_t Reserve(size_t capacity) {
		if (capacity <= fCapacity && fBuffer) return B_OK;
		size_t newCap = fCapacity ? fCapacity : 32;
		while (newCap < capacity) newCap *= 2;
		char* buffer = (char*)realloc(fBuffer, newCap);
		if (!buffer) return B_NO_MEMORY;
		fBuffer = buffer;
		fCapacity = newCap;
		return B_OK;
	}

	char* fBuffer;
	size_t fLength;
	size_t fCapacity;
};

#endif
