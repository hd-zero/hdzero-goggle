#include <ctype.h>
#include <log/log.h>
#include <minIni.h>
#include <stdio.h>
#include <unistd.h>

#include "language.h"
#include "simplified_chinese.h"
#include "ui/page_common.h"

const translate_t translate_english_default[TRANSLATE_STRING_NUM];

const translate_t *translate_list[LANG_END] = {
    translate_english_default,
    translate_simplified_chinese,
};

const char *language_options[] = {
    "English",
    "Simplified Chinese",
};

const char *language_config_file[] = {
    // uppercase
    "ENG.TXT",
    "CHN.TXT",
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

static void to_lowercase(char *str) {
    while (*str) {
        if (isupper((unsigned char)*str)) {
            *str = tolower((unsigned char)*str);
        }
        str++;
    }
}

bool language_config() {
    char buf[256];
    int i = 0;

    for (i = 0; i < LANG_END; i++) {
        sprintf(buf, "/mnt/extsd/%s", language_config_file[i]);
        if (access(buf, F_OK) == 0) {
            LOGI("%s found", language_config_file[i]);
            ini_putl("language", "lang", i, SETTING_INI);
            g_setting.language.lang = i;
            return 1;
        }

        to_lowercase(buf);
        if (access(buf, F_OK) == 0) {
            LOGI("%s found", language_config_file[i]);
            ini_putl("language", "lang", i, SETTING_INI);
            g_setting.language.lang = i;
            return 1;
        }
    }
    return 0;
}
