#ifndef _PAGE_VERSION_H
#define _PAGE_VERSION_H


#include "lvgl/lvgl.h"
#include "page_common.h"

lv_obj_t *page_version_create(lv_obj_t *parent, struct panel_arr *arr);
void version_update(int sel);
void process_bar_update(const int value0,
		const int value1);


void bar_update(int sel, int value);
void version_update_title();

void *thread_version(void *ptr);
uint8_t command_monitor(char* cmd);
#endif
