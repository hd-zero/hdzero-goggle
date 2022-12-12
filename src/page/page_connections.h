#ifndef _PAGE_CONNECTIONS_H
#define _PAGE_CONNECTIONS_H


#include "lvgl/lvgl.h"
#include "page_common.h"
lv_obj_t *page_connections_create(lv_obj_t *parent, struct panel_arr *arr);
void connect_function(int sel);

#endif
