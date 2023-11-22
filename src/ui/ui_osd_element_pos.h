#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include "core/settings.h"

void ui_osd_element_pos_init(void);
void ui_osd_element_pos_update();
void ui_osd_element_pos_on_enter();
void ui_osd_element_pos_cancel_and_hide();
int ui_osd_element_pos_handle_input(int key);

#ifdef __cplusplus
}
#endif
