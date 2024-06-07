#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <lvgl/lvgl.h>

#include "ui/ui_main_menu.h"

extern page_pack_t pp_source;

void source_status_timer();
void page_source_select_hdzero();
void page_source_select_expansion();
void page_source_select_av_in();
void page_source_select_hdmi();

#ifdef __cplusplus
}
#endif
