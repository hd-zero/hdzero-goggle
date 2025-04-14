#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "ui/ui_main_menu.h"

extern page_pack_t pp_storage;

bool page_storage_was_sd_repair_invoked();
bool page_storage_is_sd_repair_active();
void page_storage_init_auto_sd_repair();

#ifdef __cplusplus
}
#endif
