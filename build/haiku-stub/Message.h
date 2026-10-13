// Stub of Haiku <Message.h> - functional-enough for host-side tests.
#ifndef YELLOW_HAIKU_STUB_MESSAGE_H
#define YELLOW_HAIKU_STUB_MESSAGE_H

#include <SupportDefs.h>
#include <String.h>
#include <Rect.h>
#include <StorageDefs.h>
#include <map>
#include <string>
#include <vector>

class BMessage {
public:
	BMessage() : what(0) {}
	explicit BMessage(uint32 aWhat) : what(aWhat) {}
	BMessage(const BMessage& other) : what(other.what), fStrings(other.fStrings), fInt32s(other.fInt32s), fFloats(other.fFloats), fRefs(other.fRefs) {}
	~BMessage() {}

	BMessage& operator=(const BMessage& other) {
		if (this != &other) {
			what = other.what;
			fStrings = other.fStrings;
			fInt32s = other.fInt32s;
			fFloats = other.fFloats;
			fRefs = other.fRefs;
		}
		return *this;
	}

	status_t AddString(const char* name, const char* value) {
		fStrings[name].push_back(value ? value : "");
		return B_OK;
	}
	status_t AddString(const char* name, const BString& value) { return AddString(name, value.String()); }
	status_t AddInt32(const char* name, int32 value) { fInt32s[name].push_back(value); return B_OK; }
	status_t AddFloat(const char* name, float value) { fFloats[name].push_back(value); return B_OK; }
	status_t AddBool(const char* name, bool value) { fInt32s[name].push_back(value ? 1 : 0); return B_OK; }
	status_t AddRef(const char* name, const entry_ref* ref) {
		if (ref == NULL) return B_BAD_VALUE;
		fRefs[name].push_back(*ref);
		return B_OK;
	}

	status_t FindString(const char* name, BString* out) const {
		std::map<std::string, std::vector<std::string> >::const_iterator it = fStrings.find(name);
		if (it == fStrings.end() || it->second.empty()) return B_ENTRY_NOT_FOUND;
		if (out) *out = it->second[0].c_str();
		return B_OK;
	}
	status_t FindInt32(const char* name, int32* out) const {
		std::map<std::string, std::vector<int32> >::const_iterator it = fInt32s.find(name);
		if (it == fInt32s.end() || it->second.empty()) return B_ENTRY_NOT_FOUND;
		if (out) *out = it->second[0];
		return B_OK;
	}
	status_t FindFloat(const char* name, float* out) const {
		std::map<std::string, std::vector<float> >::const_iterator it = fFloats.find(name);
		if (it == fFloats.end() || it->second.empty()) return B_ENTRY_NOT_FOUND;
		if (out) *out = it->second[0];
		return B_OK;
	}
	status_t FindBool(const char* name, bool* out) const {
		int32 value = 0;
		status_t result = FindInt32(name, &value);
		if (result == B_OK && out) *out = (value != 0);
		return result;
	}
	status_t FindRef(const char* name, entry_ref* ref) const { return FindRef(name, 0, ref); }
	status_t FindRef(const char* name, int32 index, entry_ref* ref) const {
		std::map<std::string, std::vector<entry_ref> >::const_iterator it = fRefs.find(name);
		if (it == fRefs.end() || index < 0 || (size_t)index >= it->second.size())
			return B_ENTRY_NOT_FOUND;
		if (ref) *ref = it->second[(size_t)index];
		return B_OK;
	}

	status_t GetInfo(const char* name, type_code* typeFound, int32* countFound = NULL) const {
		std::map<std::string, std::vector<entry_ref> >::const_iterator refIt = fRefs.find(name);
		if (refIt != fRefs.end()) {
			if (typeFound) *typeFound = 'REFS';
			if (countFound) *countFound = (int32)refIt->second.size();
			return B_OK;
		}
		std::map<std::string, std::vector<std::string> >::const_iterator sIt = fStrings.find(name);
		if (sIt != fStrings.end()) {
			if (typeFound) *typeFound = 'STR';
			if (countFound) *countFound = (int32)sIt->second.size();
			return B_OK;
		}
		std::map<std::string, std::vector<int32> >::const_iterator iIt = fInt32s.find(name);
		if (iIt != fInt32s.end()) {
			if (typeFound) *typeFound = 'LONG';
			if (countFound) *countFound = (int32)iIt->second.size();
			return B_OK;
		}
		return B_NAME_NOT_FOUND;
	}
	status_t GetInfo(const char* name, type_code* typeFound, bool* fixedSize) const {
		status_t result = GetInfo(name, typeFound, (int32*)NULL);
		if (result == B_OK && fixedSize) *fixedSize = true;
		return result;
	}

	bool HasString(const char* name) const { return fStrings.count(name) > 0; }
	bool HasInt32(const char* name) const { return fInt32s.count(name) > 0; }

	uint32 what;

private:
	std::map<std::string, std::vector<std::string> > fStrings;
	std::map<std::string, std::vector<int32> > fInt32s;
	std::map<std::string, std::vector<float> > fFloats;
	std::map<std::string, std::vector<entry_ref> > fRefs;
};

#endif
