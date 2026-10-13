#include <YellowImportExport/Xml.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

namespace YellowImportExport {

// -------------------------------------------------------------- XmlElement

XmlElement::XmlElement()
{
}

XmlElement::~XmlElement()
{
    for (XmlElement* child : children) {
        delete child;
    }
    children.clear();
}

const BString* XmlElement::Attribute(const BString& name) const
{
    auto it = attributes.find(name);
    if (it == attributes.end()) return nullptr;
    return &it->second;
}

BString XmlElement::Attribute(const BString& name, const BString& fallback) const
{
    const BString* value = Attribute(name);
    return value ? *value : fallback;
}

bool XmlElement::IsNamed(const BString& name) const
{
    return this->name == name;
}

XmlElement* XmlElement::Child(const BString& name) const
{
    for (XmlElement* child : children) {
        if (child && child->name == name) return child;
    }
    return nullptr;
}

void XmlElement::ChildrenNamed(const BString& name, std::vector<XmlElement*>& out) const
{
    for (XmlElement* child : children) {
        if (child && child->name == name) out.push_back(child);
    }
}

BString XmlElement::AllText() const
{
    BString out = text;
    for (XmlElement* child : children) {
        if (child) out += child->AllText();
    }
    return out;
}

// -------------------------------------------------------------- XmlDocument

XmlDocument::XmlDocument()
    : fRoot(nullptr)
{
}

XmlDocument::~XmlDocument()
{
    delete fRoot;
}

namespace {

class XmlParser {
public:
    XmlParser(const char* text, size_t length)
        : fText(text), fLength(length), fPos(0), fError(false)
    {
    }

    XmlElement* Parse(BString* outError) {
        SkipMisc();
        if (fPos >= fLength) {
            Fail("Empty document", outError);
            return nullptr;
        }
        XmlElement* root = ParseElement(outError);
        return root;
    }

    bool HasError() const { return fError; }

private:
    void Fail(const char* message, BString* outError) {
        fError = true;
        if (outError) {
            BString text(message);
            text += " at offset ";
            char buffer[32];
            snprintf(buffer, sizeof(buffer), "%zu", fPos);
            text += buffer;
            *outError = text;
        }
    }

    bool StartsWith(const char* token) const {
        size_t len = strlen(token);
        return fPos + len <= fLength && strncmp(fText + fPos, token, len) == 0;
    }

    void SkipWhitespace() {
        while (fPos < fLength) {
            char c = fText[fPos];
            if (c == ' ' || c == '\t' || c == '\n' || c == '\r') ++fPos;
            else break;
        }
    }

    void SkipMisc() {
        while (fPos < fLength) {
            SkipWhitespace();
            if (StartsWith("<?")) {
                const char* end = strstr(fText + fPos, "?>");
                if (!end) { fPos = fLength; return; }
                fPos = (size_t)(end - fText) + 2;
            } else if (StartsWith("<!--")) {
                const char* end = strstr(fText + fPos, "-->");
                if (!end) { fPos = fLength; return; }
                fPos = (size_t)(end - fText) + 3;
            } else if (StartsWith("<!")) {
                const char* end = strstr(fText + fPos, ">");
                if (!end) { fPos = fLength; return; }
                fPos = (size_t)(end - fText) + 1;
            } else {
                return;
            }
        }
    }

    BString ParseName() {
        size_t start = fPos;
        while (fPos < fLength) {
            char c = fText[fPos];
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
                (c >= '0' && c <= '9') || c == '_' || c == '-' || c == ':' ||
                c == '.') {
                ++fPos;
            } else {
                break;
            }
        }
        BString name;
        name.SetTo(fText + start, (int32)(fPos - start));
        return name;
    }

    BString DecodeEntities(const BString& raw) {
        BString out;
        int32 i = 0;
        while (i < raw.Length()) {
            if (raw[i] == '&') {
                int32 semi = raw.FindFirst(';', i);
                if (semi > i && semi - i < 12) {
                    BString entity(raw.String() + i + 1, semi - i - 1);
                    if (entity == "amp") out += '&';
                    else if (entity == "lt") out += '<';
                    else if (entity == "gt") out += '>';
                    else if (entity == "quot") out += '"';
                    else if (entity == "apos") out += '\'';
                    else if (entity.Length() > 1 && entity[0] == '#') {
                        long code = 0;
                        if (entity[1] == 'x' || entity[1] == 'X') {
                            code = strtol(entity.String() + 2, nullptr, 16);
                        } else {
                            code = strtol(entity.String() + 1, nullptr, 10);
                        }
                        if (code > 0 && code < 0x80) {
                            out += (char)code;
                        } else if (code >= 0x80 && code < 0x800) {
                            out += (char)(0xC0 | (code >> 6));
                            out += (char)(0x80 | (code & 0x3F));
                        } else if (code >= 0x800 && code < 0x10000) {
                            out += (char)(0xE0 | (code >> 12));
                            out += (char)(0x80 | ((code >> 6) & 0x3F));
                            out += (char)(0x80 | (code & 0x3F));
                        }
                    } else {
                        out += '&';
                        out += entity;
                        out += ';';
                    }
                    i = semi + 1;
                    continue;
                }
            }
            out += raw[i];
            ++i;
        }
        return out;
    }

    XmlElement* ParseElement(BString* outError) {
        if (fPos >= fLength || fText[fPos] != '<') {
            Fail("Expected '<'", outError);
            return nullptr;
        }
        ++fPos;
        XmlElement* element = new XmlElement();
        element->name = ParseName();
        if (element->name.IsEmpty()) {
            delete element;
            Fail("Expected element name", outError);
            return nullptr;
        }

        // Attributes.
        while (true) {
            SkipWhitespace();
            if (fPos >= fLength) {
                delete element;
                Fail("Unterminated start tag", outError);
                return nullptr;
            }
            if (fText[fPos] == '>') {
                ++fPos;
                break;
            }
            if (fText[fPos] == '/' && fPos + 1 < fLength && fText[fPos + 1] == '>') {
                fPos += 2;
                return element; // empty element
            }
            BString attrName = ParseName();
            if (attrName.IsEmpty()) {
                delete element;
                Fail("Expected attribute name", outError);
                return nullptr;
            }
            SkipWhitespace();
            if (fPos >= fLength || fText[fPos] != '=') {
                delete element;
                Fail("Expected '='", outError);
                return nullptr;
            }
            ++fPos;
            SkipWhitespace();
            if (fPos >= fLength || (fText[fPos] != '"' && fText[fPos] != '\'')) {
                delete element;
                Fail("Expected quoted attribute value", outError);
                return nullptr;
            }
            char quote = fText[fPos++];
            size_t valueStart = fPos;
            while (fPos < fLength && fText[fPos] != quote) ++fPos;
            if (fPos >= fLength) {
                delete element;
                Fail("Unterminated attribute value", outError);
                return nullptr;
            }
            BString raw;
            raw.SetTo(fText + valueStart, (int32)(fPos - valueStart));
            ++fPos; // closing quote
            element->attributes[attrName] = DecodeEntities(raw);
        }

        // Content until matching close tag.
        while (fPos < fLength) {
            if (StartsWith("<!--")) {
                const char* end = strstr(fText + fPos, "-->");
                if (!end) break;
                fPos = (size_t)(end - fText) + 3;
                continue;
            }
            if (StartsWith("<![CDATA[")) {
                const char* end = strstr(fText + fPos, "]]>");
                if (!end) break;
                BString cdata;
                cdata.SetTo(fText + fPos + 9, (int32)(end - (fText + fPos + 9)));
                element->text += cdata;
                fPos = (size_t)(end - fText) + 3;
                continue;
            }
            if (StartsWith("</")) {
                fPos += 2;
                BString closeName = ParseName();
                SkipWhitespace();
                if (fPos < fLength && fText[fPos] == '>') ++fPos;
                if (closeName != element->name) {
                    // Mismatched close tag: be lenient and accept.
                }
                return element;
            }
            if (fText[fPos] == '<') {
                XmlElement* child = ParseElement(outError);
                if (!child) {
                    delete element;
                    return nullptr;
                }
                element->children.push_back(child);
                continue;
            }
            // Text run.
            size_t start = fPos;
            while (fPos < fLength && fText[fPos] != '<') ++fPos;
            BString raw;
            raw.SetTo(fText + start, (int32)(fPos - start));
            element->text += DecodeEntities(raw);
        }

        delete element;
        Fail("Unterminated element", outError);
        return nullptr;
    }

    const char* fText;
    size_t fLength;
    size_t fPos;
    bool fError;
};

} // namespace

bool XmlDocument::Parse(const BString& text, BString* outError)
{
    delete fRoot;
    fRoot = nullptr;
    XmlParser parser(text.String(), (size_t)text.Length());
    fRoot = parser.Parse(outError);
    return fRoot != nullptr;
}

BString XmlEscape(const BString& text)
{
    BString out;
    for (int32 i = 0; i < text.Length(); ++i) {
        char c = text[i];
        switch (c) {
            case '&': out += "&amp;"; break;
            case '<': out += "&lt;"; break;
            case '>': out += "&gt;"; break;
            case '"': out += "&quot;"; break;
            case '\'': out += "&apos;"; break;
            default: out += c; break;
        }
    }
    return out;
}

} // namespace YellowImportExport
