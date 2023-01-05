#ifndef _PAGE_CONNECTIONS_H
#define _PAGE_CONNECTIONS_H

#include <lvgl/lvgl.h>

#include "page_common.h"

lv_obj_t *page_connections_create(lv_obj_t *parent, panel_arr_t *arr);

void connect_function(int sel);
void page_connections_reset();

#endif
