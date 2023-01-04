#include "page_playback.h"
#include "../core/playback.h"
#include "page_common.h"
#include "style.h"


static lv_coord_t col_dsc[] = {320,320,320, LV_GRID_TEMPLATE_LAST};
static lv_coord_t row_dsc[] = {150,30,150,30,150,30,30, LV_GRID_TEMPLATE_LAST};

lv_obj_t *page_playback_create(lv_obj_t *parent)
{
    lv_obj_t *page = lv_menu_page_create(parent, NULL);
	lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_size(page, 1164, 900);
	lv_obj_add_style(page, &style_subpage, LV_PART_MAIN);
	lv_obj_set_style_pad_top(page, 94, 0);

    lv_obj_t *section = lv_menu_section_create(page);
	lv_obj_add_style(section, &style_submenu, LV_PART_MAIN);
	lv_obj_set_size(section, 1164, 894);

    create_text(NULL, section, false, "Playback:", LV_MENU_ITEM_BUILDER_VARIANT_2);

    lv_obj_t *cont = lv_obj_create(section);
    lv_obj_set_size(cont, 1164, 700);
    lv_obj_set_layout(cont, LV_LAYOUT_GRID);
	lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_add_style(cont, &style_context, LV_PART_MAIN);

	create_pb_item(cont);
/*
	lv_obj_t *btn = lv_btn_create(cont);
    lv_obj_t *label = lv_label_create(btn);
    lv_label_set_text(label, "<Back");
	lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_LEFT, 0);

	lv_obj_set_style_text_font(btn, &lv_font_montserrat_26, 0);
	lv_obj_set_style_text_align(btn, LV_TEXT_ALIGN_LEFT, 0);
	lv_obj_set_style_bg_color(btn, lv_color_make(19, 19, 19), 0);
	lv_obj_set_style_shadow_width(btn, 0, 0);
	lv_obj_set_style_pad_top(btn, 0, 0);
    lv_obj_set_size(btn, 160, 60);
	lv_obj_set_grid_cell(btn, LV_GRID_ALIGN_START, 0, 1,
						 LV_GRID_ALIGN_CENTER, 6, 1);

*/
	return page;
}

void pb_key(uint8_t key)
{
	onkey(key);
}

