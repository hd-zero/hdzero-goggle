#ifndef _LANGUAGE_H_
#define _LANGUAGE_H_

#include "core/settings.h"

#define TRANSLATE_STRING_NUM 287
#define LANG_FOLDER          "/mnt/app/language"

typedef enum {
    LANG_ENGLISH_DEFAULT = 0,
    LANG_SIMPLIFIED_CHINESE,
    LANG_RUSSIAN,
    LANG_END,
} lang_e;

typedef struct {
    const char *in_english;
    const char *translate;
} translate_t;

void language_init();
const char *translate_string(const char *str, lang_e lang);
bool language_config();

#define _str(string, lang) translate_string(string, lang)
#define _(string)          _str(string, g_setting.language.lang)
#define _lang(string)      _((string))

#endif