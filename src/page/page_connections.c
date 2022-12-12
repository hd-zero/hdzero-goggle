#include "page_connections.h"
#include "page_version.h"
#include "page_common.h"
#include "style.h"
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include "../driver/esp32.h"


static lv_coord_t col_dsc[] = {180,200,160,160,160,160, LV_GRID_TEMPLATE_LAST};
static lv_coord_t row_dsc[] = {60,60,60,60,60,60,60,60,60,60, LV_GRID_TEMPLATE_LAST};
static lv_obj_t *btn_flash;

lv_obj_t *page_connections_create(lv_obj_t *parent, struct panel_arr *arr)
{
    lv_obj_t *page = lv_menu_page_create(parent, NULL);
	lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_size(page, 1053, 900);
	lv_obj_add_style(page, &style_subpage, LV_PART_MAIN);
	lv_obj_set_style_pad_top(page, 94, 0);

    lv_obj_t *section = lv_menu_section_create(page);
	lv_obj_add_style(section, &style_submenu, LV_PART_MAIN);
	lv_obj_set_size(section, 1053, 894);

    create_text(NULL,section, false, "Connect Options:", LV_MENU_ITEM_BUILDER_VARIANT_2);

    lv_obj_t *cont = lv_obj_create(section);
    lv_obj_set_size(cont, 960, 600);
    lv_obj_set_pos(cont, 0, 0);
    lv_obj_set_layout(cont, LV_LAYOUT_GRID);
	lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_add_style(cont, &style_context, LV_PART_MAIN);

    lv_obj_set_style_grid_column_dsc_array(cont, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);

	create_select_item(arr, cont);

	btn_group_t btn_group;
	create_btn_group_item(&btn_group, cont, 2, "Backpack", "On", "Off", "","",  0);
	create_label_item(cont,  "ESP Firmware", 1, 1, 1);
	btn_flash = create_label_item(cont, "Update Via SD", 2, 1, 1);
	create_label_item(cont, "Update Via WIFI", 2, 2, 1);
	create_btn_group_item(&btn_group, cont, 2, "Wifi AP*", "On", "Off", "","",  3);
	create_label_item(cont,  "Wifi Settings", 1, 4, 1);
	create_label_item(cont,  "Configure", 2, 4, 1);
	create_label_item(cont,  "SSD: HDZero", 2, 5, 1);
	//create_label_item(cont,  "Pass: hdzero123", 2, 7, 1);
	create_label_item(cont,  "broadcast ID: Yes", 2, 6, 1);

	create_label_item(cont, "<Back", 1, 7, 1);

	lv_obj_t *label2 = lv_label_create(cont);
   	lv_label_set_text(label2, "*Expension module is required.");
	lv_obj_set_style_text_font(label2, &lv_font_montserrat_16, 0);
	lv_obj_set_style_text_align(label2, LV_TEXT_ALIGN_LEFT, 0);
	lv_obj_set_style_text_color(label2, lv_color_make(255,255,255), 0);
	lv_obj_set_style_pad_top(label2, 12, 0);
	lv_label_set_long_mode(label2, LV_LABEL_LONG_WRAP);
	lv_obj_set_grid_cell(label2, LV_GRID_ALIGN_START, 1, 4,
						 LV_GRID_ALIGN_START, 8, 2);

	return page;
}

static bool flash_elrs(void)
{
	bool ret;
	ret = (command_monitor("/mnt/app/script/update_elrs.sh") == 1);
	if(ret) init_esp32();
	return ret;
}

void connect_function(int sel)
{
	if(sel == 1)
	{
		lv_label_set_text(btn_flash, "flash...");
		lv_timer_handler();
		bool ret = flash_elrs();
		if(ret)
			lv_label_set_text(btn_flash, "Success");
		else
			lv_label_set_text(btn_flash, "Failed");
	}
}
