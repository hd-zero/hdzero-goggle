#include "language.h"
#include "simplified_chinese.h"
#include <log/log.h>

translate_t translate_english_default[TRANSLATE_STRING_NUM];

translate_t *translate_list[LANG_END] = {
    translate_english_default,
    translate_simplified_chinese,
};

const char *language_options[] = {
    "English",
    "Simplified Chinese",
};

char *translate_string(const char *str, lang_e lang) {
    int i, j;

    // search language
    i = 0;
    while (i != lang) {
        if (i < LANG_END)
            i++;
    }

    // if language is undefined
    if (i != lang)
        return (char *)str;

    // search str translate
    for (j = 0; j < TRANSLATE_STRING_NUM; j++) {
        if (translate_list[i][j].in_english == NULL || translate_list[i][j].in_english[0] == '\0')
            continue;

        if (strcmp(str, translate_list[i][j].in_english) == 0)
            return translate_list[i][j].translate;
    }

    // if str is undefined
    return (char *)str;
}