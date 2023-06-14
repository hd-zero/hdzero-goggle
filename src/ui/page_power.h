#ifndef _PAGE_POWER_H
#define _PAGE_POWER_H

#include "ui/ui_main_menu.h"

extern page_pack_t pp_power;

void power_cell_count_inc(void);
void power_cell_count_dec(void);
void power_warning_voltage_inc(void);
void power_warning_voltage_dec(void);

#endif
