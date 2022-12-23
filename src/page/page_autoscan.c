#include "page_autoscan.h"

#include "page_common.h"
#include "style.h"
#include "../minIni/minIni.h"


static lv_coord_t col_dsc[] = {100,150,180,220,180,160, LV_GRID_TEMPLATE_LAST};
static lv_coord_t row_dsc[] = {60,60,60,60,60,60,60,60,60,60, LV_GRID_TEMPLATE_LAST};

static btn_group_t btn_group0;
static btn_group_t btn_group1;

lv_obj_t *page_autoscan_create(lv_obj_t *parent, struct panel_arr *arr)
{
    lv_obj_t *page = lv_menu_page_create(parent, NULL);
	lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_size(page, 1053, 900);
	lv_obj_add_style(page, &style_subpage, LV_PART_MAIN);
	lv_obj_set_style_pad_top(page, 94, 0);

    lv_obj_t *section = lv_menu_section_create(page);
	lv_obj_add_style(section, &style_submenu, LV_PART_MAIN);
	lv_obj_set_size(section, 1053, 894);

    create_text(NULL, section, false, "Auto Scan:", LV_MENU_ITEM_BUILDER_VARIANT_2);

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
	create_btn_group_item(&btn_group0, cont, 2, "Auto Scan", "On", "Off", "","",  0);
	create_btn_group_item2(&btn_group1, cont, 5, "Default", "Last","HDZero", "Expansion", "AV In", "HDMI In", " ",  1); //2 rows
	create_label_item(cont, "<Back", 1, 3, 1);

	lv_obj_t *label2 = lv_label_create(cont);
   	lv_label_set_text(label2, "*if Auto Scan is off, goggles will default to show last tuned channel");
	lv_obj_set_style_text_font(label2, &lv_font_montserrat_16, 0);
	lv_obj_set_style_text_align(label2, LV_TEXT_ALIGN_LEFT, 0);
	lv_obj_set_style_text_color(label2, lv_color_make(255,255,255), 0);
	lv_obj_set_style_pad_top(label2, 12, 0);
	lv_label_set_long_mode(label2, LV_LABEL_LONG_WRAP);
	lv_obj_set_grid_cell(label2, LV_GRID_ALIGN_START, 1, 3,
						 LV_GRID_ALIGN_START, 4, 2);

	btn_group_set_sel(&btn_group0, !g_setting.autoscan.status);		
	btn_group_set_sel(&btn_group1, g_setting.autoscan.source);		
	return page;
}


void autoscan_toggle(int sel)
{
	int value = 0;
	
	if(sel == 0) {
		btn_group_toggle_sel(&btn_group0);		

		value = btn_group_get_sel(&btn_group0) == 0 ? 1 : 0;
		if(value){
			ini_puts("autoscan", "status", "enable", SETTING_INI);
		}else{
			ini_puts("autoscan", "status", "disable", SETTING_INI);
		}
		g_setting.autoscan.status = value;
	}
	else if(sel < 3) {
		btn_group_toggle_sel(&btn_group1);	
		value = btn_group_get_sel(&btn_group1);
		ini_putl("autoscan", "source", value, SETTING_INI);
		g_setting.autoscan.source = value;
	}
}
