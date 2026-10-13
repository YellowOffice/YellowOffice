#include "YellowCommon/Json.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

namespace YellowCommon {

namespace detail {
struct JsonMember {
    BString key;
    JsonValue value;
};
} // namespace detail

using detail::JsonMember;

static void AppendIndent(BString& out, int32 level)
{
    for (int32 i = 0; i < level; ++i) {
        out += "  ";
    }
}

static void AppendEscapedString(BString& out, const BString& value)
{
    out += '"';
    const char* s = value.String();
    for (int32 i = 0; i < value.Length(); ++i) {
        unsigned char c = (unsigned char)s[i];
        switch (c) {
            case '"': out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\b': out += "\\b"; break;
            case '\f': out += "\\f"; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            default:
                if (c < 0x20) {
                    char buffer[8];
                    snprintf(buffer, sizeof(buffer), "\\u%04x", c);
                    out += buffer;
                } else {
                    out += (char)c;
                }
                break;
        }
    }
    out += '"';
}

static void AppendNumber(BString& out, double number)
{
    if (number == (double)(int64)number && fabs(number) < 1e15) {
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%lld", (long long)(int64)number);
        out += buffer;
    } else {
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "%.17g", number);
        out += buffer;
    }
}

// ---------------------------------------------------------------- JsonValue

JsonValue::JsonValue()
    : fType(JSON_NULL), fBool(false), fNumber(0), fString(nullptr),
      fArray(nullptr), fObject(nullptr)
{
}

JsonValue::JsonValue(bool value)
    : fType(JSON_BOOL), fBool(value), fNumber(0), fString(nullptr),
      fArray(nullptr), fObject(nullptr)
{
}

JsonValue::JsonValue(double value)
    : fType(JSON_NUMBER), fBool(false), fNumber(value), fString(nullptr),
      fArray(nullptr), fObject(nullptr)
{
}

JsonValue::JsonValue(int32 value)
    : fType(JSON_NUMBER), fBool(false), fNumber(value), fString(nullptr),
      fArray(nullptr), fObject(nullptr)
{
}

JsonValue::JsonValue(const char* value)
    : fType(value ? JSON_STRING : JSON_NULL), fBool(false), fNumber(0),
      fString(nullptr), fArray(nullptr), fObject(nullptr)
{
    if (value) {
        fString = new BString(value);
    }
}

JsonValue::JsonValue(const BString& value)
    : fType(JSON_STRING), fBool(false), fNumber(0), fString(new BString(value)),
      fArray(nullptr), fObject(nullptr)
{
}

JsonValue::JsonValue(const JsonValue& other)
    : fType(JSON_NULL), fBool(false), fNumber(0), fString(nullptr),
      fArray(nullptr), fObject(nullptr)
{
    CopyFrom(other);
}

JsonValue::~JsonValue()
{
    Reset();
}

JsonValue& JsonValue::operator=(const JsonValue& other)
{
    if (this != &other) {
        Reset();
        CopyFrom(other);
    }
    return *this;
}

void JsonValue::CopyFrom(const JsonValue& other)
{
    fType = other.fType;
    fBool = other.fBool;
    fNumber = other.fNumber;
    if (other.fString) fString = new BString(*other.fString);
    if (other.fArray) fArray = new std::vector<JsonValue>(*other.fArray);
    if (other.fObject) {
        fObject = new std::vector<JsonMember>(*other.fObject);
    }
}

void JsonValue::Reset()
{
    delete fString;
    delete fArray;
    delete fObject;
    fString = nullptr;
    fArray = nullptr;
    fObject = nullptr;
    fType = JSON_NULL;
    fBool = false;
    fNumber = 0;
}

bool JsonValue::AsBool(bool defaultValue) const
{
    if (fType == JSON_BOOL) return fBool;
    if (fType == JSON_NUMBER) return fNumber != 0.0;
    return defaultValue;
}

double JsonValue::AsNumber(double defaultValue) const
{
    if (fType == JSON_NUMBER) return fNumber;
    if (fType == JSON_BOOL) return fBool ? 1.0 : 0.0;
    return defaultValue;
}

int32 JsonValue::AsInt(int32 defaultValue) const
{
    if (fType == JSON_NUMBER) return (int32)fNumber;
    return defaultValue;
}

const BString& JsonValue::AsString() const
{
    static const BString empty;
    if (fType == JSON_STRING && fString) return *fString;
    return empty;
}

BString JsonValue::AsString(const BString& defaultValue) const
{
    if (fType == JSON_STRING && fString) return *fString;
    return defaultValue;
}

void JsonValue::Append(const JsonValue& value)
{
    if (fType == JSON_NULL) {
        fType = JSON_ARRAY;
        fArray = new std::vector<JsonValue>();
    }
    if (fType == JSON_ARRAY && fArray) {
        fArray->push_back(value);
    }
}

int32 JsonValue::CountItems() const
{
    if (fType == JSON_ARRAY && fArray) return (int32)fArray->size();
    if (fType == JSON_OBJECT && fObject) return (int32)fObject->size();
    return 0;
}

const JsonValue& JsonValue::At(int32 index) const
{
    static const JsonValue null;
    if (fType == JSON_ARRAY && fArray && index >= 0 && (size_t)index < fArray->size()) {
        return (*fArray)[index];
    }
    return null;
}

JsonValue& JsonValue::At(int32 index)
{
    static JsonValue null;
    if (fType == JSON_ARRAY && fArray && index >= 0 && (size_t)index < fArray->size()) {
        return (*fArray)[index];
    }
    return null;
}

void JsonValue::Set(const BString& key, const JsonValue& value)
{
    if (fType == JSON_NULL) {
        fType = JSON_OBJECT;
        fObject = new std::vector<JsonMember>();
    }
    if (fType != JSON_OBJECT || !fObject) return;
    for (JsonMember& member : *fObject) {
        if (member.key == key) {
            member.value = value;
            return;
        }
    }
    JsonMember member;
    member.key = key;
    member.value = value;
    fObject->push_back(member);
}

bool JsonValue::Has(const BString& key) const
{
    if (fType != JSON_OBJECT || !fObject) return false;
    for (const JsonMember& member : *fObject) {
        if (member.key == key) return true;
    }
    return false;
}

const JsonValue& JsonValue::Get(const BString& key) const
{
    if (fType == JSON_OBJECT && fObject) {
        for (const JsonMember& member : *fObject) {
            if (member.key == key) return member.value;
        }
    }
    return Null();
}

bool JsonValue::Remove(const BString& key)
{
    if (fType != JSON_OBJECT || !fObject) return false;
    for (auto it = fObject->begin(); it != fObject->end(); ++it) {
        if (it->key == key) {
            fObject->erase(it);
            return true;
        }
    }
    return false;
}

void JsonValue::GetKeys(std::vector<BString>& outKeys) const
{
    outKeys.clear();
    if (fType == JSON_OBJECT && fObject) {
        for (const JsonMember& member : *fObject) {
            outKeys.push_back(member.key);
        }
    }
}

BString JsonValue::Serialize(bool pretty, int32 indentLevel) const
{
    return SerializeInternal(pretty, indentLevel);
}

BString JsonValue::SerializeInternal(bool pretty, int32 indentLevel) const
{
    BString out;
    switch (fType) {
        case JSON_NULL:
            out += "null";
            break;
        case JSON_BOOL:
            out += fBool ? "true" : "false";
            break;
        case JSON_NUMBER:
            AppendNumber(out, fNumber);
            break;
        case JSON_STRING:
            AppendEscapedString(out, fString ? *fString : BString());
            break;
        case JSON_ARRAY: {
            if (!fArray || fArray->empty()) {
                out += "[]";
                break;
            }
            out += '[';
            if (pretty) out += '\n';
            for (size_t i = 0; i < fArray->size(); ++i) {
                if (pretty) AppendIndent(out, indentLevel + 1);
                out += (*fArray)[i].SerializeInternal(pretty, indentLevel + 1);
                if (i + 1 < fArray->size()) out += ',';
                if (pretty) out += '\n';
            }
            if (pretty) AppendIndent(out, indentLevel);
            out += ']';
            break;
        }
        case JSON_OBJECT: {
            if (!fObject || fObject->empty()) {
                out += "{}";
                break;
            }
            out += '{';
            if (pretty) out += '\n';
            for (size_t i = 0; i < fObject->size(); ++i) {
                if (pretty) AppendIndent(out, indentLevel + 1);
                AppendEscapedString(out, (*fObject)[i].key);
                out += pretty ? ": " : ":";
                out += (*fObject)[i].value.SerializeInternal(pretty, indentLevel + 1);
                if (i + 1 < fObject->size()) out += ',';
                if (pretty) out += '\n';
            }
            if (pretty) AppendIndent(out, indentLevel);
            out += '}';
            break;
        }
    }
    return out;
}

const JsonValue& JsonValue::Null()
{
    static const JsonValue null;
    return null;
}

// ---------------------------------------------------------------- Parser

namespace {

class Parser {
public:
    Parser(const char* text, size_t length)
        : fText(text), fLength(length), fPos(0), fError(false)
    {
    }

    bool Parse(JsonValue* outValue) {
        SkipWhitespace();
        if (!ParseValue(outValue)) return false;
        SkipWhitespace();
        if (fPos != fLength) {
            fError = true;
            fErrorMessage = "Trailing characters after JSON value";
            return false;
        }
        return true;
    }

    bool HasError() const { return fError; }
    const BString& ErrorMessage() const { return fErrorMessage; }

private:
    void SkipWhitespace() {
        while (fPos < fLength) {
            char c = fText[fPos];
            if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
                ++fPos;
            } else {
                break;
            }
        }
    }

    bool Expect(char c) {
        if (fPos >= fLength || fText[fPos] != c) {
            fError = true;
            BString message("Expected '");
            message += c;
            message += "'";
            fErrorMessage = message;
            return false;
        }
        ++fPos;
        return true;
    }

    bool ParseValue(JsonValue* outValue) {
        if (fPos >= fLength) {
            fError = true;
            fErrorMessage = "Unexpected end of input";
            return false;
        }
        char c = fText[fPos];
        switch (c) {
            case '{': return ParseObject(outValue);
            case '[': return ParseArray(outValue);
            case '"': {
                BString string;
                if (!ParseString(&string)) return false;
                *outValue = JsonValue(string);
                return true;
            }
            case 't':
                if (MatchLiteral("true")) { *outValue = JsonValue(true); return true; }
                break;
            case 'f':
                if (MatchLiteral("false")) { *outValue = JsonValue(false); return true; }
                break;
            case 'n':
                if (MatchLiteral("null")) { *outValue = JsonValue(); return true; }
                break;
            default:
                if (c == '-' || (c >= '0' && c <= '9')) {
                    return ParseNumber(outValue);
                }
                break;
        }
        fError = true;
        fErrorMessage = "Invalid JSON value";
        return false;
    }

    bool MatchLiteral(const char* literal) {
        size_t len = strlen(literal);
        if (fPos + len > fLength) return false;
        if (strncmp(fText + fPos, literal, len) != 0) return false;
        fPos += len;
        return true;
    }

    bool ParseNumber(JsonValue* outValue) {
        size_t start = fPos;
        if (fPos < fLength && fText[fPos] == '-') ++fPos;
        while (fPos < fLength && fText[fPos] >= '0' && fText[fPos] <= '9') ++fPos;
        if (fPos < fLength && fText[fPos] == '.') {
            ++fPos;
            while (fPos < fLength && fText[fPos] >= '0' && fText[fPos] <= '9') ++fPos;
        }
        if (fPos < fLength && (fText[fPos] == 'e' || fText[fPos] == 'E')) {
            ++fPos;
            if (fPos < fLength && (fText[fPos] == '+' || fText[fPos] == '-')) ++fPos;
            while (fPos < fLength && fText[fPos] >= '0' && fText[fPos] <= '9') ++fPos;
        }
        BString number(fText + start, (int32)(fPos - start));
        *outValue = JsonValue(strtod(number.String(), nullptr));
        return true;
    }

    bool ParseString(BString* out) {
        if (!Expect('"')) return false;
        out->Truncate(0);
        while (fPos < fLength) {
            char c = fText[fPos++];
            if (c == '"') return true;
            if (c == '\\') {
                if (fPos >= fLength) break;
                char escape = fText[fPos++];
                switch (escape) {
                    case '"': *out += '"'; break;
                    case '\\': *out += '\\'; break;
                    case '/': *out += '/'; break;
                    case 'b': *out += '\b'; break;
                    case 'f': *out += '\f'; break;
                    case 'n': *out += '\n'; break;
                    case 'r': *out += '\r'; break;
                    case 't': *out += '\t'; break;
                    case 'u': {
                        if (fPos + 4 > fLength) {
                            fError = true;
                            fErrorMessage = "Invalid \\u escape";
                            return false;
                        }
                        unsigned int code = 0;
                        for (int i = 0; i < 4; ++i) {
                            char h = fText[fPos++];
                            code <<= 4;
                            if (h >= '0' && h <= '9') code += h - '0';
                            else if (h >= 'a' && h <= 'f') code += h - 'a' + 10;
                            else if (h >= 'A' && h <= 'F') code += h - 'A' + 10;
                            else {
                                fError = true;
                                fErrorMessage = "Invalid hex digit in \\u escape";
                                return false;
                            }
                        }
                        // Encode as UTF-8 (basic multilingual plane only).
                        if (code < 0x80) {
                            *out += (char)code;
                        } else if (code < 0x800) {
                            *out += (char)(0xC0 | (code >> 6));
                            *out += (char)(0x80 | (code & 0x3F));
                        } else {
                            *out += (char)(0xE0 | (code >> 12));
                            *out += (char)(0x80 | ((code >> 6) & 0x3F));
                            *out += (char)(0x80 | (code & 0x3F));
                        }
                        break;
                    }
                    default:
                        fError = true;
                        fErrorMessage = "Invalid escape sequence";
                        return false;
                }
            } else {
                *out += c;
            }
        }
        fError = true;
        fErrorMessage = "Unterminated string";
        return false;
    }

    bool ParseArray(JsonValue* outValue) {
        if (!Expect('[')) return false;
        outValue->Append(JsonValue()); // convert to array
        // Clear the placeholder: rebuild properly
        JsonValue array;
        SkipWhitespace();
        if (fPos < fLength && fText[fPos] == ']') {
            ++fPos;
            *outValue = array;
            return true;
        }
        while (true) {
            SkipWhitespace();
            JsonValue item;
            if (!ParseValue(&item)) return false;
            array.Append(item);
            SkipWhitespace();
            if (fPos < fLength && fText[fPos] == ',') {
                ++fPos;
                continue;
            }
            if (fPos < fLength && fText[fPos] == ']') {
                ++fPos;
                *outValue = array;
                return true;
            }
            fError = true;
            fErrorMessage = "Expected ',' or ']' in array";
            return false;
        }
    }

    bool ParseObject(JsonValue* outValue) {
        if (!Expect('{')) return false;
        JsonValue object;
        SkipWhitespace();
        if (fPos < fLength && fText[fPos] == '}') {
            ++fPos;
            object.Set(BString(), JsonValue()); // ensure object type with no members
            // Remove the dummy member; Set() made it an object already.
            object.Remove(BString());
            *outValue = object;
            return true;
        }
        while (true) {
            SkipWhitespace();
            BString key;
            if (!ParseString(&key)) return false;
            SkipWhitespace();
            if (!Expect(':')) return false;
            SkipWhitespace();
            JsonValue value;
            if (!ParseValue(&value)) return false;
            object.Set(key, value);
            SkipWhitespace();
            if (fPos < fLength && fText[fPos] == ',') {
                ++fPos;
                continue;
            }
            if (fPos < fLength && fText[fPos] == '}') {
                ++fPos;
                *outValue = object;
                return true;
            }
            fError = true;
            fErrorMessage = "Expected ',' or '}' in object";
            return false;
        }
    }

    const char* fText;
    size_t fLength;
    size_t fPos;
    bool fError;
    BString fErrorMessage;
};

} // namespace

bool JsonValue::Parse(const BString& text, JsonValue* outValue, BString* outError)
{
    return Parse(text.String(), (size_t)text.Length(), outValue, outError);
}

bool JsonValue::Parse(const char* text, size_t length, JsonValue* outValue, BString* outError)
{
    if (!text || !outValue) {
        if (outError) *outError = "Invalid arguments";
        return false;
    }
    Parser parser(text, length);
    if (!parser.Parse(outValue)) {
        if (outError) *outError = parser.ErrorMessage();
        return false;
    }
    return true;
}

} // namespace YellowCommon
