#include "language.h"
#include "simplified_chinese.h"
#include <log/log.h>

const translate_t translate_english_default[TRANSLATE_STRING_NUM];

const translate_t *translate_list[LANG_END] = {
    translate_english_default,
    translate_simplified_chinese,
};

const char *language_options[] = {
    "English",
    "Simplified Chinese",
};

char *translate_string(const char *str, lang_e lang) {
    int i;

    if (lang == LANG_ENGLISH_DEFAULT || lang >= LANG_END)
        return (char *)str;

    // search str translate
    for (i = 0; i < TRANSLATE_STRING_NUM; i++) {
        if (translate_list[lang][i].in_english == NULL || translate_list[lang][i].in_english[0] == '\0')
            continue;

        if (strcmp(str, translate_list[lang][i].in_english) == 0)
            return translate_list[lang][i].translate;
    }

    // if str is undefined
    return (char *)str;
}