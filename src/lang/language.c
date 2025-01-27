#include <ctype.h>
#include <log/log.h>
#include <minIni.h>
#include <stdio.h>
#include <unistd.h>

#include "core/common.hh"
#include "language.h"
#include "ui/page_common.h"

#include <assert.h>
#include <stdlib.h>

struct Language {
    lang_e lang;
    const char * const code;
    const char * const name;
    const translate_t *translations;
};

static struct Language languages[] = {
    {LANG_ENGLISH_DEFAULT, "en_us", "English", NULL},
    {LANG_SIMPLIFIED_CHINESE, "zh_hans", "Simplified Chinese", NULL},
    {LANG_RUSSIAN, "ru_ru", "Russian", NULL},
};

const char *language_config_file[] = {
    // uppercase
    "ENG.TXT",
    "CHN.TXT",
    "RUS.TXT",
};

struct CallbackPayload {
    translate_t translations[TRANSLATE_STRING_NUM];
    size_t index;
};

int init_callback(const char* section, const char* key, const char* value, void* userData) {
    struct CallbackPayload* payload = userData;

    assert(key != NULL);
    assert(value != NULL);

    const size_t englishLength = strlen(key);
    const size_t translateLength = strlen(value);

    char* englishBuffer = malloc(englishLength + 1);
    char* translateBuffer = malloc(translateLength + 1);

    strncpy(englishBuffer, key, englishLength);
    strncpy(translateBuffer, value, translateLength);

    englishBuffer[englishLength] = 0;
    translateBuffer[translateLength] = 0;

    payload->translations[payload->index].in_english = englishBuffer;
    payload->translations[payload->index].translate = translateBuffer;

    payload->index++;
    return 1;
}

void language_init() {
    for (size_t i = 0; i < ARRAY_SIZE(languages); i++) {
        char fileName[256];
        sprintf(fileName, "%s/%s.ini", LANG_FOLDER, languages[i].code);

        INI_FILETYPE file;
        if (!ini_openread(fileName, &file)) {
            LOGE("Failed to open %s", fileName);
            languages[i].translations = NULL;
            continue;
        }

        // Load translations
        translate_t* translations = malloc(TRANSLATE_STRING_NUM * sizeof(translate_t));
        struct CallbackPayload payload = {.index = 0};
        ini_browse(init_callback, &payload, fileName);

        // Copy translations
        memcpy(translations, payload.translations, sizeof(payload.translations));
        languages[i].translations = translations;

        for (size_t index = 0; index < TRANSLATE_STRING_NUM; index++) {
            LOGD("%s: %s", languages[i].translations[index].in_english, languages[i].translations[index].translate);
        }

        ini_close(&file);
    }
}

const char *translate_string(const char *str, lang_e lang) {
    if (lang == LANG_ENGLISH_DEFAULT || lang >= LANG_END)
        return str;

    const struct Language * const language = &languages[lang];
    const size_t keyLength = strlen(str);
    // search str translate
    for (int i = 0; i < TRANSLATE_STRING_NUM; i++) {
        const translate_t * const translation = &language->translations[i];

        if (translation->in_english == NULL || translation->in_english[0] == '\0')
            continue;

        if (strlen(translation->in_english) != keyLength)
            continue;

        if (strcmp(str, translation->in_english) == 0)
            return translation->translate;
    }

    return str;
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
        snprintf(buf, sizeof(buf), "/mnt/extsd/%s", language_config_file[i]);
        if (access(buf, F_OK) == 0) {
            LOGI("%s found", language_config_file[i]);
            ini_putl("language", "lang", i, SETTING_INI);
            g_setting.language.lang = i;
            return true;
        }

        to_lowercase(buf);
        if (access(buf, F_OK) == 0) {
            LOGI("%s found", language_config_file[i]);
            ini_putl("language", "lang", i, SETTING_INI);
            g_setting.language.lang = i;
            return true;
        }
    }
    return false;
}