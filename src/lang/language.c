#include "language.h"
#include "english.h"
#include "simplified_chinese.h"
#include <log/log.h>

lang_e LANGUAGE = lang_simplified_chinese;

translate_t *translate_list[] = {
    translate_english,
    translate_simplified_chinese,
};

char *translate_string(const char *str, lang_e lang) {
    int i, j;

    // search language
    i = 0;
    while (i != LANGUAGE) {
        if (i < lang_end)
            i++;
    }

    // if language is undefined
    if (i != LANGUAGE)
        return (char *)str;

    // search str translate
    for (j = 0; j < TRANSLATE_STRING_NUM; j++) {
        if (strcmp(str, translate_list[i][j].in_english) == 0)
            return translate_list[i][j].translate;
    }

    // if str is undefined
    return (char *)str;
}

void lv_label_set_text_lang(lv_obj_t *obj, const char *text, lang_e lang) {
    char *text_lang = _str(text, lang);
    lv_label_set_text(obj, text_lang);
}