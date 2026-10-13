#ifndef YELLOW_COMMON_JSON_H
#define YELLOW_COMMON_JSON_H

#include <SupportDefs.h>
#include <String.h>
#include <vector>
#include <utility>

namespace YellowCommon {

namespace detail {
struct JsonMember;
}

// Minimal JSON DOM used by the YDF document format. Supports the full
// JSON grammar (objects, arrays, strings with escapes, numbers, true,
// false, null) with a straightforward parser; not optimized for huge
// documents.

class JsonValue {
public:
    enum Type {
        JSON_NULL,
        JSON_BOOL,
        JSON_NUMBER,
        JSON_STRING,
        JSON_ARRAY,
        JSON_OBJECT
    };

    JsonValue();
    explicit JsonValue(bool value);
    explicit JsonValue(double value);
    explicit JsonValue(int32 value);
    explicit JsonValue(const char* value);
    explicit JsonValue(const BString& value);
    JsonValue(const JsonValue& other);
    ~JsonValue();

    JsonValue& operator=(const JsonValue& other);

    Type GetType() const { return fType; }
    bool IsNull() const { return fType == JSON_NULL; }
    bool IsBool() const { return fType == JSON_BOOL; }
    bool IsNumber() const { return fType == JSON_NUMBER; }
    bool IsString() const { return fType == JSON_STRING; }
    bool IsArray() const { return fType == JSON_ARRAY; }
    bool IsObject() const { return fType == JSON_OBJECT; }

    bool AsBool(bool defaultValue = false) const;
    double AsNumber(double defaultValue = 0.0) const;
    int32 AsInt(int32 defaultValue = 0) const;
    const BString& AsString() const;
    BString AsString(const BString& defaultValue) const;

    // Array access
    void Append(const JsonValue& value);
    int32 CountItems() const;
    const JsonValue& At(int32 index) const;
    JsonValue& At(int32 index);

    // Object access
    void Set(const BString& key, const JsonValue& value);
    bool Has(const BString& key) const;
    const JsonValue& Get(const BString& key) const; // returns static null if missing
    bool Remove(const BString& key);
    void GetKeys(std::vector<BString>& outKeys) const;

    // Serialization
    BString Serialize(bool pretty = false, int32 indentLevel = 0) const;

    // Parsing. Returns true on success; on failure returns false and sets
    // *outError (if given) to a human-readable message.
    static bool Parse(const BString& text, JsonValue* outValue, BString* outError = nullptr);
    static bool Parse(const char* text, size_t length, JsonValue* outValue, BString* outError = nullptr);

    static const JsonValue& Null();

private:
    void CopyFrom(const JsonValue& other);
    void Reset();
    BString SerializeInternal(bool pretty, int32 indentLevel) const;

    Type fType;
    bool fBool;
    double fNumber;
    BString* fString;
    std::vector<JsonValue>* fArray;
    std::vector<detail::JsonMember>* fObject;
};

} // namespace YellowCommon

#endif // YELLOW_COMMON_JSON_H
