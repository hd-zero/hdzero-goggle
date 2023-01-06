#ifndef _PAGE_VERSION_H
#define _PAGE_VERSION_H

#include <lvgl/lvgl.h>

#include "ui/ui_main_menu.h"

#define CURRENT_VER_MAX (64)

typedef struct {
    uint8_t rx;
    uint8_t va;
    uint8_t app;
    char current[CURRENT_VER_MAX];
} sys_version_t;

extern page_pack_t pp_version;

void version_update(int sel);
void process_bar_update(const int value0, const int value1);
void bar_update(int sel, int value);
void version_update_title();

int generate_current_version(sys_version_t *sys_ver);

void *thread_version(void *ptr);
uint8_t command_monitor(char *cmd);
#endif
