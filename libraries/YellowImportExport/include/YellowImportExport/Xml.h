#ifndef YELLOW_IMPORT_EXPORT_XML_H
#define YELLOW_IMPORT_EXPORT_XML_H

#include <SupportDefs.h>
#include <String.h>
#include <map>
#include <vector>

namespace YellowImportExport {

/**
 * Minimal non-validating XML parser sufficient for ODF content documents:
 * elements, attributes, text, comments, processing instructions, CDATA,
 * and the five predefined entities plus numeric character references.
 */
struct XmlElement {
    BString name;
    std::map<BString, BString> attributes;
    BString text;
    std::vector<XmlElement*> children;

    XmlElement();
    ~XmlElement();

    const BString* Attribute(const BString& name) const;
    BString Attribute(const BString& name, const BString& fallback) const;
    bool IsNamed(const BString& name) const;
    /// Returns the first direct child with the given (possibly prefixed) name.
    XmlElement* Child(const BString& name) const;
    void ChildrenNamed(const BString& name, std::vector<XmlElement*>& out) const;
    /// Concatenated text of this element and all descendants.
    BString AllText() const;
};

class XmlDocument {
public:
    XmlDocument();
    ~XmlDocument();

    bool Parse(const BString& text, BString* outError = nullptr);
    XmlElement* Root() const { return fRoot; }

private:
    XmlElement* fRoot;
};

/// Escapes XML special characters.
BString XmlEscape(const BString& text);

} // namespace YellowImportExport

#endif
