#include <stdio.h>
#include "main_menu.h"
#include "lvgl/lvgl.h"

#include "style.h"
#include "porting.h"
#include "common.hh"
#include "../page/page_scannow.h"
#include "../page/page_source.h"
#include "../page/page_imagesettings.h"
#include "../page/page_power.h"
#include "../page/page_fans.h"
#include "../page/page_record.h"
#include "../page/page_autoscan.h"
#include "../page/page_connections.h"
#include "../page/page_headtracker.h"
#include "../page/page_playback.h"
#include "../page/page_version.h"
#include "../page/page_common.h"
#include "../driver/hardware.h"
#include "../driver/mcp3021.h"
#include "../driver/oled.h"
#include "imagesetting.h"
#include "playback.h"

lv_obj_t * menu;
lv_obj_t * root_page;
progress_bar_t progress_bar;

page_pack_t pp_scannow;
page_pack_t pp_source = {
	.p_arr = {
		.cur = 0,
		.max = 4,
	}
};
page_pack_t pp_imagesettings = {
	.p_arr = {
		.cur = 0,
		.max = 6,
	}
};
page_pack_t pp_power = {
	.p_arr = {
		.cur = 0,
		.max = 4,
	}
};
page_pack_t pp_fans = {
	.p_arr = {
		.cur = 0,
		.max = 4,
	}
};
page_pack_t pp_record = {
	.p_arr = {
		.cur = 0,
		.max = 7,
	}
};
page_pack_t pp_autoscan = {
	.p_arr = {
		.cur = 0,
		.max = 4,
	}
};
page_pack_t pp_connections = {
	.p_arr = {
		.cur = 0,
		.max = 9,
	}
};
page_pack_t pp_headtracker = {
	.p_arr = {
		.cur = 0,
		.max = 3,
	}
};
page_pack_t pp_playback;

page_pack_t pp_version = {
	.p_arr = {
		.cur = 0,
		.max = 4,
	}
};


LV_IMG_DECLARE(img_arrow);

page_pack_t * find_pp(lv_obj_t *page)
{
	if(pp_scannow.page == page)
		return &pp_scannow;

	if(pp_source.page == page)
		return &pp_source;

	if(pp_imagesettings.page == page)
		return &pp_imagesettings;

	if(pp_power.page == page)
		return &pp_power;

	if(pp_fans.page == page)
		return &pp_fans;

	if(pp_record.page == page)
		return &pp_record;

	if(pp_autoscan.page == page)
		return &pp_autoscan;

	if(pp_connections.page == page)
		return &pp_connections;

	if(pp_headtracker.page == page)
		return &pp_headtracker;

	if(pp_playback.page == page)
		return &pp_playback;

	if(pp_version.page == page)
		return &pp_version;

	return NULL;
}

static void clear_all_icon(void)
{
	lv_img_set_src(pp_scannow.icon, LV_SYMBOL_DUMMY);
	lv_img_set_src(pp_source.icon, LV_SYMBOL_DUMMY);
	lv_img_set_src(pp_imagesettings.icon, LV_SYMBOL_DUMMY);
	lv_img_set_src(pp_power.icon, LV_SYMBOL_DUMMY);
	lv_img_set_src(pp_fans.icon, LV_SYMBOL_DUMMY);
	lv_img_set_src(pp_record.icon, LV_SYMBOL_DUMMY);
	lv_img_set_src(pp_autoscan.icon, LV_SYMBOL_DUMMY);
	lv_img_set_src(pp_connections.icon, LV_SYMBOL_DUMMY);
	lv_img_set_src(pp_headtracker.icon, LV_SYMBOL_DUMMY);
	lv_img_set_src(pp_playback.icon, LV_SYMBOL_DUMMY);
	lv_img_set_src(pp_version.icon, LV_SYMBOL_DUMMY);
}
static void menu_event_handler(lv_event_t * e)
{

	clear_all_icon();

	page_pack_t *pp = find_pp(lv_menu_get_cur_main_page(menu));
	if(!pp)
		return;

	lv_img_set_src(pp->icon, &img_arrow);

}

static int auto_scaned_cnt = 0;
void submenu_enter(void)
{
	page_pack_t *pp = find_pp(lv_menu_get_cur_main_page(menu));
	if(!pp) return;

	if(pp == &pp_scannow) {
		auto_scaned_cnt = scan();
		Printf("scan return :%d\n", auto_scaned_cnt);

		if(auto_scaned_cnt == 1) {
			if(!g_autoscan_exit)
				g_autoscan_exit = true;

			g_menu_op = OPLEVEL_VIDEO;
			switch_to_video(false);
		}

		if(auto_scaned_cnt == -1)
			submenu_exit();

		return;
	}
	else if(pp == &pp_playback) {
		if(!init_pb())
			submenu_exit(); //won't enter sub menu if no video file is found
		return;
	}
	else if(pp == &pp_source) {
		in_sourcepage = true;
	}
	else if(pp == &pp_version) {
		version_update_title();
	}
	else if(pp==&pp_imagesettings) {
		page_ims_click();
	}

	pp->p_arr.cur = 0;
	set_select_item(&pp->p_arr, pp->p_arr.cur);
}

void submenu_nav(uint8_t key)
{
	page_pack_t *pp = find_pp(lv_menu_get_cur_main_page(menu));
	if(!pp) return;

	if(pp == &pp_scannow) user_select(key);
	else if(pp == &pp_playback) pb_key(key);
	else {
		if(pp == &pp_version)
			version_update_title();

		if(pp == &pp_record)
			formatsd_negtive();

		if(key == DIAL_KEY_UP) {
			if(pp->p_arr.cur < pp->p_arr.max - 1)
				pp->p_arr.cur++;
			else
				pp->p_arr.cur = 0;
		}
		else if(key == DIAL_KEY_DOWN) {
			if(pp->p_arr.cur > 0)
				pp->p_arr.cur--;
			else
				pp->p_arr.cur = pp->p_arr.max -1;
		}
		set_select_item(&pp->p_arr, pp->p_arr.cur);
	}
}

void submenu_exit()
{
	Printf("submenu_exit\n");
	g_menu_op = OPLEVEL_MAINMENU;

	page_pack_t *pp = find_pp(lv_menu_get_cur_main_page(menu));
	if(!pp) return;
	pp->p_arr.cur = 0;

	if(pp == &pp_playback)
		pb_key(DIAL_KEY_PRESS);
	else if(pp == &pp_scannow)
		HDZero_Close();
	else if(pp == &pp_source) {
		pp_source_exit();
		in_sourcepage = false;
	}
	else if(pp == &pp_record) {
		formatsd_negtive();
	}

	//No need to remove the selected bar on ScanNow and Playback page
	if((pp != &pp_scannow) && (pp != &pp_playback))
		set_select_item(&pp->p_arr, -1);
}

void submenu_fun(void)
{
	page_pack_t *pp = find_pp(lv_menu_get_cur_main_page(menu));
	if(!pp) return;

	if(pp == &pp_scannow) {
		g_menu_op = OPLEVEL_VIDEO;
		switch_to_video(false);
	}
	else if(pp == &pp_playback) {
		pb_key(DIAL_KEY_CLICK);
	}
	else
	{
		if((pp == &pp_fans) )
		{
			fans_mode_toggle(pp->p_arr.cur);
		}
		if((pp == &pp_source) )
		{
			source_mode_set(pp->p_arr.cur);
		}
		if((pp == &pp_autoscan) )
		{
			autoscan_toggle(pp->p_arr.cur);
		}
		if((pp == &pp_power) )
		{
			power_set_toggle(pp->p_arr.cur);
		}
		if((pp == &pp_connections) )
		{
			connect_function(pp->p_arr.cur);
		}
		if((pp == &pp_record) )
		{
			record_set_toggle(pp->p_arr.cur);
		}

		if(pp == &pp_headtracker)
		{
			headtracker_set_toggle(pp->p_arr.cur);
		}

		if((pp == &pp_version) )
		{
			version_update(pp->p_arr.cur);
		}


		if(pp->p_arr.cur ==  pp->p_arr.max - 1)
		{
				submenu_exit();
		}
		else
		{

		}

	}
}

void menu_nav(uint8_t key)
{
	static int8_t selected = 0;
	printf("menu_nav: key = %d,sel = %d\n", key, selected);
	if(key == DIAL_KEY_DOWN) {
		selected--;
		if(selected <0)
			selected += MAIN_MENU_ITEMS;
	}
	else if(key == DIAL_KEY_UP) {
		selected++;
		if(selected >= MAIN_MENU_ITEMS)
			selected -= MAIN_MENU_ITEMS;
	}
	lv_event_send(lv_obj_get_child(lv_obj_get_child(lv_menu_get_cur_sidebar_page(menu), 0), selected), LV_EVENT_CLICKED, NULL);
}

static void ui_create_rootpage(lv_obj_t * parent)
{
    lv_obj_t * cont;
    lv_obj_t * section;

    /*Create a root page*/
    root_page = lv_menu_page_create(parent, "aaa");
    section = lv_menu_section_create(root_page);
	lv_obj_clear_flag(section, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_add_event_cb(parent, menu_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

	struct menu_obj_s s;

    create_text(&s, section, true, "Scan Now", LV_MENU_ITEM_BUILDER_VARIANT_1);
	pp_scannow.icon = s.icon;
	cont = s.cont;
	lv_obj_set_style_text_font(cont, &lv_font_montserrat_26, 0);
    lv_menu_set_load_page_event(parent, cont, pp_scannow.page);

    create_text(&s, section, true, "Source", LV_MENU_ITEM_BUILDER_VARIANT_1);
	pp_source.icon = s.icon;
	cont = s.cont;
	lv_obj_set_style_text_font(cont, &lv_font_montserrat_26, 0);
    lv_menu_set_load_page_event(parent, cont, pp_source.page);

    create_text(&s, section, true, "Image Settings", LV_MENU_ITEM_BUILDER_VARIANT_1);
	pp_imagesettings.icon = s.icon;
	cont = s.cont;
	lv_obj_set_style_text_font(cont, &lv_font_montserrat_26, 0);
    lv_menu_set_load_page_event(parent, cont, pp_imagesettings.page);

    create_text(&s, section, true, "Power", LV_MENU_ITEM_BUILDER_VARIANT_1);
	pp_power.icon = s.icon;
	cont = s.cont;
	lv_obj_set_style_text_font(cont, &lv_font_montserrat_26, 0);
    lv_menu_set_load_page_event(parent, cont, pp_power.page);

    create_text(&s, section, true, "Fans", LV_MENU_ITEM_BUILDER_VARIANT_1);
	pp_fans.icon = s.icon;
	cont = s.cont;
	lv_obj_set_style_text_font(cont, &lv_font_montserrat_26, 0);
    lv_menu_set_load_page_event(parent, cont, pp_fans.page);

    create_text(&s, section, true, "Record options", LV_MENU_ITEM_BUILDER_VARIANT_1);
	pp_record.icon = s.icon;
	cont = s.cont;
	lv_obj_set_style_text_font(cont, &lv_font_montserrat_26, 0);
    lv_menu_set_load_page_event(parent, cont, pp_record.page);

    create_text(&s, section, true, "Auto Scan", LV_MENU_ITEM_BUILDER_VARIANT_1);
	pp_autoscan.icon = s.icon;
	cont = s.cont;
	lv_obj_set_style_text_font(cont, &lv_font_montserrat_26, 0);
    lv_menu_set_load_page_event(parent, cont, pp_autoscan.page);

    create_text(&s, section, true, "Connections", LV_MENU_ITEM_BUILDER_VARIANT_1);
	pp_connections.icon = s.icon;
	cont = s.cont;
	lv_obj_set_style_text_font(cont, &lv_font_montserrat_26, 0);
    lv_menu_set_load_page_event(parent, cont, pp_connections.page);

    create_text(&s, section, true, "Head Tracker", LV_MENU_ITEM_BUILDER_VARIANT_1);
	pp_headtracker.icon = s.icon;
	cont = s.cont;
	lv_obj_set_style_text_font(cont, &lv_font_montserrat_26, 0);
    lv_menu_set_load_page_event(parent, cont, pp_headtracker.page);

    create_text(&s, section, true, "Playback", LV_MENU_ITEM_BUILDER_VARIANT_1);
	pp_playback.icon = s.icon;
	cont = s.cont;
	lv_obj_set_style_text_font(cont, &lv_font_montserrat_26, 0);
    lv_menu_set_load_page_event(parent, cont, pp_playback.page);

    create_text(&s, section, true, "Firmware", LV_MENU_ITEM_BUILDER_VARIANT_1);
	pp_version.icon = s.icon;
	cont = s.cont;
	lv_obj_set_style_text_font(cont, &lv_font_montserrat_26, 0);
    lv_menu_set_load_page_event(parent, cont, pp_version.page);

	lv_obj_add_style(section, &style_rootmenu, LV_PART_MAIN);
	lv_obj_set_size(section, 250, 975);
	lv_obj_set_pos(section, 0,0);

	lv_obj_set_size(root_page, 250, 975);
	lv_obj_set_pos(root_page, 0,0);
	lv_obj_set_style_border_width(root_page, 0, 0);
	lv_obj_set_style_radius(root_page, 0, 0);

    lv_menu_set_sidebar_page(parent, root_page);
    lv_event_send(lv_obj_get_child(lv_obj_get_child(lv_menu_get_cur_sidebar_page(parent), 0), 0), LV_EVENT_CLICKED, NULL);
	lv_obj_add_flag(lv_menu_get_sidebar_header(parent), LV_OBJ_FLAG_HIDDEN);
	lv_obj_clear_flag(lv_menu_get_cur_sidebar_page(parent), LV_OBJ_FLAG_SCROLLABLE);

}

static void menu_reinit(void)
{
	Printf("reinit\n");
	page_pack_t *pp = find_pp(lv_menu_get_cur_main_page(menu));
	if(!pp)
		return;

	//Printf("set select item\n");

	if((pp == &pp_scannow) )
	{
		scan_reinit();
	}
	else if((pp == &pp_source) )
	{
		set_select_item(&pp->p_arr, -1);
	}
	else if((pp == &pp_playback) )
	{

	}
	else
	{
		pp->p_arr.cur = -1;
		set_select_item(&pp->p_arr, pp->p_arr.cur);
	}
}

bool main_menu_isshow(void)
{
	return !lv_obj_has_flag(menu , LV_OBJ_FLAG_HIDDEN);
}

void main_menu_show(bool is_show)
{
	if(is_show){
		menu_reinit();
		lv_obj_clear_flag(menu, LV_OBJ_FLAG_HIDDEN);
	}
	else{
		lv_obj_add_flag(menu, LV_OBJ_FLAG_HIDDEN);
	}
}

void main_menu_toggle(void)
{

	if(lv_obj_has_flag(menu , LV_OBJ_FLAG_HIDDEN)){
		lv_obj_clear_flag(menu, LV_OBJ_FLAG_HIDDEN);
		menu_reinit();
	}
	else{
		lv_obj_add_flag(menu, LV_OBJ_FLAG_HIDDEN);
	}
}

void main_menu_init(void)
{
    menu = lv_menu_create(lv_scr_act());
	//lv_obj_add_flag(menu, LV_OBJ_FLAG_HIDDEN);
	lv_obj_clear_flag(menu, LV_OBJ_FLAG_SCROLLABLE);

	lv_obj_set_style_bg_color(menu, lv_color_make(32, 32, 32), 0);
	lv_obj_set_style_border_width(menu, 3, 0);
	lv_obj_set_style_border_color(menu, lv_color_make(255, 0, 0), 0);
	lv_obj_set_style_border_side(menu, LV_BORDER_SIDE_LEFT|LV_BORDER_SIDE_RIGHT, 0);
    lv_obj_set_size(menu, lv_disp_get_hor_res(NULL)-500, lv_disp_get_ver_res(NULL)-96);
	lv_obj_set_pos(menu, 250,96);

	pp_scannow.page = page_scannow_create(menu);

	if(g_test_en)
		pp_source.p_arr.max = 6;
	else
		pp_source.p_arr.max = 5;
	pp_source.page = page_source_create(menu, &pp_source.p_arr);

	pp_imagesettings.page = page_imagesettings_create(menu, &pp_imagesettings.p_arr);
	pp_power.page = page_power_create(menu, &pp_power.p_arr);
	pp_fans.page = page_fans_create(menu, &pp_fans.p_arr);
	pp_record.page = page_record_create(menu, &pp_record.p_arr);
	pp_autoscan.page = page_autoscan_create(menu, &pp_autoscan.p_arr);
	pp_connections.page = page_connections_create(menu, &pp_connections.p_arr);
	pp_headtracker.page = page_headtracker_create(menu, &pp_headtracker.p_arr);
	pp_playback.page = page_playback_create(menu);
	pp_version.page = page_version_create(menu, &pp_version.p_arr);

	ui_create_rootpage(menu);

	progress_bar.bar = lv_bar_create(lv_scr_act());
    lv_obj_set_size(progress_bar.bar, 320, 20);
	lv_obj_align(progress_bar.bar, LV_ALIGN_CENTER, 0, 0);
	lv_obj_add_flag(progress_bar.bar, LV_OBJ_FLAG_HIDDEN);
	progress_bar.start = 0;
	progress_bar.val = 0;
}

void progress_bar_update()
{
	static uint8_t state = 0; //0=idle, 1= in process

	switch(state) {
		case 0:
			if(progress_bar.start) { //to start the progress bar
				state = 1;
				lv_obj_add_flag(menu, LV_OBJ_FLAG_HIDDEN);
				lv_obj_clear_flag(progress_bar.bar, LV_OBJ_FLAG_HIDDEN);
				progress_bar.val = 0;
				//Printf("Progress bar start\n");
			}
			break;

		case 1:
			if(progress_bar.start == 0) { //to end end progress bar
				state = 0;
				lv_obj_clear_flag(menu, LV_OBJ_FLAG_HIDDEN);
				lv_obj_add_flag(progress_bar.bar, LV_OBJ_FLAG_HIDDEN);
				progress_bar.val = 0;
				//Printf("Progress bar end\n");
			}
			break;
	}

	if(state == 1) {
		if(progress_bar.val <100)
			progress_bar.val+=4;
		lv_bar_set_value(progress_bar.bar, progress_bar.val, LV_ANIM_OFF);
		lv_timer_handler();
	}
}

///////////////////////////////////////////////////////////////////////////////
//
void autoscan_exit(void)
{
	if(!g_autoscan_exit)
	{
		Printf("autoscan_exit, lelve=1\n");
		g_autoscan_exit = true;
		if(auto_scaned_cnt >1)
			g_menu_op = OPLEVEL_SUBMENU;
		else
			g_menu_op = OPLEVEL_MAINMENU;
	}
}
