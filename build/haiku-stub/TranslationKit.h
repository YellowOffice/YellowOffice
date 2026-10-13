// Stub of Haiku Translation Kit (<TranslationKit.h>, <TranslatorFormats.h>) - declarations.
#ifndef YELLOW_HAIKU_STUB_TRANSLATIONKIT_H
#define YELLOW_HAIKU_STUB_TRANSLATIONKIT_H

#include <SupportDefs.h>
#include <DataIO.h>
#include <String.h>

class BBitmap;
class BTranslatorRoster;

enum {
	B_TRANSLATOR_BITMAP = 'bits',
	B_TRANSLATOR_PNG = 'png ',
	B_TRANSLATOR_JPEG = 'JPEG',
	B_TRANSLATOR_TIFF = 'TIFF',
	B_TRANSLATOR_GIF = 'GIF ',
	B_TRANSLATOR_WEBP = 'WEBP',
	B_TRANSLATOR_ANY_TYPE = 'ANYT',
};

class BTranslatorRoster {
public:
	BTranslatorRoster();
	virtual ~BTranslatorRoster();

	status_t Translate(BPositionIO* source, BTranslator* translator,
		const uint32* inType, uint32* outType, BPositionIO* target,
		uint32 hintType = 0, const char* extension = NULL, int32 translatorsHint = 0);
	int32 CountTranslators() const;
	status_t GetTranslatorInfo(BPositionIO* source, uint32* outType,
		char* outName, char* outMime);
	static BTranslatorRoster* Default();
};

class BTranslator {
public:
	virtual ~BTranslator() {}
};

#endif
