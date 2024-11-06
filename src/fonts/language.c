#include "language.h"

lang_e LANGUAGE = lang_simplified_chinese;

translate_t simplified_chinese[] = {
    {"Scan Now", "扫频"},
    {"Source", "信号源"},
    {"Image Setting", "图像设置"},
    {"OSD", "OSD"},
    {"Power", "电源"},
    {"Fans", "风扇"},
    {"Record Option", "录像设置"},
    {"Auto Scan", "自动扫频"},
    {"ELRS", "ELRS"},
    {"WiFi Module", "WiFi模块"},
    {"Head Tracker", "头部追踪"},
    {"Playback", "录像回放"},
    {"Storage", "存储"},
    {"Firmware   ", "固件"},
    {"Focus Chart", "焦点图"},
    {"Clock", "时间"},
    {"Input", "按键"},
    {"Go Sleep!", "睡眠"},
};

char *translate_string(const char *str, lang_e lang) {
    int i;
    int string_num = sizeof(simplified_chinese) / sizeof(translate_t);

    if (lang == 0)
        return (char *)str;
    else {
        for (i = 0; i < string_num; i++) {
            if (strcmp(str, simplified_chinese[i].in_english) == 0)
                return simplified_chinese[i].translate;
        }
        return (char *)str;
    }
}

void lv_label_set_text_lang(lv_obj_t *obj, const char *text, lang_e lang) {
    char *text_lang = _str(text, lang);
    lv_label_set_text(obj, text_lang);
}