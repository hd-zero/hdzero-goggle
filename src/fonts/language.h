#ifndef _LANGUAGE_H_
#define _LANGUAGE_H_

#include <lvgl/lvgl.h>

typedef enum {
    lang_english = 0,
    lang_simplified_chinese = 1,
} lang_e;

typedef struct {
    char *in_english;
    char *translate;
} translate_t;

char *translate_string(const char *str, lang_e lang);

#define _str(string, lang) translate_string(string, lang)

void lv_label_set_text_lang(lv_obj_t *obj, const char *text, lang_e lang);

extern lang_e LANGUAGE;
#endif