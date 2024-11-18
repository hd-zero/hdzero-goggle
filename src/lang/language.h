#ifndef _LANGUAGE_H_
#define _LANGUAGE_H_

#include <lvgl/lvgl.h>

#include "core/settings.h"

#define TRANSLATE_STRING_NUM 273

typedef enum {
    LANG_ENGLISH_DEFAULT = 0,
    LANG_SIMPLIFIED_CHINESE,
    LANG_END,
} lang_e;

typedef struct {
    const char * const in_english;
    const char * const translate;
} translate_t;

const char *translate_string(const char *str, lang_e lang);
bool language_config();

#define _str(string, lang) translate_string(string, lang)
#define _lang(string)      _str(string, g_setting.language.lang)

extern const char *language_options[];
#endif