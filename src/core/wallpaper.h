#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void load_wallpaper(char *file_path);
uint8_t wallpaper_is_used();

extern lv_img_dsc_t img_menu_bg;

#ifdef __cplusplus
}
#endif