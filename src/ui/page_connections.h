#ifndef _PAGE_CONNECTIONS_H
#define _PAGE_CONNECTIONS_H

#include <lvgl/lvgl.h>

#include "ui/ui_main_menu.h"

extern page_pack_t pp_connections;

lv_obj_t *page_connections_create(lv_obj_t *parent, panel_arr_t *arr);

#endif
