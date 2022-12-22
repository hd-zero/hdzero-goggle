#include "page_fans.h"

#include "style.h"

#include <stdio.h>
#include "fans.h"
#include "../minIni/minIni.h"
#include "../page/page_common.h"
#include "../page/page_fans.h"
#include "../driver/nct75.h"
#include "../driver/fans.h"
#include "ui_attribute.h"
#include "../core/common.hh"

typedef enum {
	FANS_MODE_TOP = 0,
	FANS_MODE_SIDE,
} fans_mode_t;

static lv_coord_t col_dsc[] = {160,160,160,160,140,160, LV_GRID_TEMPLATE_LAST};
static lv_coord_t row_dsc[] = {60,60,60,60,60,60,60,60,60,60, LV_GRID_TEMPLATE_LAST};

fans_mode_t fans_mode = FANS_MODE_TOP;


static btn_group_t btn_group_fans;

static slider_group_t slider_group[2];


lv_obj_t *page_fans_create(lv_obj_t *parent, struct panel_arr *arr)
{
    lv_obj_t *page = lv_menu_page_create(parent, NULL);
	lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_size(page, 1053, 900);
	lv_obj_add_style(page, &style_subpage, LV_PART_MAIN);
	lv_obj_set_style_pad_top(page, 94, 0);

    lv_obj_t *section = lv_menu_section_create(page);
	lv_obj_add_style(section, &style_submenu, LV_PART_MAIN);
	lv_obj_set_size(section, 1053, 894);

    create_text(NULL,section, false, "Fans:", LV_MENU_ITEM_BUILDER_VARIANT_2);

    lv_obj_t *cont = lv_obj_create(section);
    lv_obj_set_size(cont, 960, 600);
    lv_obj_set_pos(cont, 0, 0);
    lv_obj_set_layout(cont, LV_LAYOUT_GRID);
	lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_add_style(cont, &style_context, LV_PART_MAIN);

    lv_obj_set_style_grid_column_dsc_array(cont, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);

	create_select_item(arr, cont);

	create_btn_group_item(&btn_group_fans, cont, 2, "Auto Control", "On", "Off", "","",  0);
	create_slider_item(&slider_group[0], cont, "Top Fan", 5, 2, 1);
	create_slider_item(&slider_group[1], cont, "Side Fans", 5, 2, 2);

	create_label_item(cont, "<Back", 1, 3,1);
	
	btn_group_set_sel(&btn_group_fans, !g_setting.fans.auto_mode);
	
	lv_slider_set_value(slider_group[0].slider, g_setting.fans.top_speed, LV_ANIM_OFF);	
	lv_slider_set_value(slider_group[1].slider, g_setting.fans.left_speed, LV_ANIM_OFF);	

	char buf[5];
	sprintf(buf, "%d", g_setting.fans.top_speed);
	lv_label_set_text(slider_group[0].label, buf);
	sprintf(buf, "%d", g_setting.fans.left_speed);
	lv_label_set_text(slider_group[1].label, buf);

	return page;
}

void fans_speed_inc(void)
{
	int32_t value = 0;
	char buf[5];

	if( fans_mode == FANS_MODE_TOP)
	{
		 value = lv_slider_get_value(slider_group[0].slider);
		if(value <= 4)
			value += 1;

		lv_slider_set_value(slider_group[0].slider, value, LV_ANIM_OFF);	

		sprintf(buf, "%d", value);
		lv_label_set_text(slider_group[0].label, buf);

		fans_top_setspeed(value);

		g_setting.fans.top_speed = value;
  		ini_putl("fans", "top_speed", value, SETTING_INI);
	}
	else if( fans_mode == FANS_MODE_SIDE)
	{
		 value = lv_slider_get_value(slider_group[1].slider);
		if(value <= 4)
			value += 1;

		lv_slider_set_value(slider_group[1].slider, value, LV_ANIM_OFF);	
		
		sprintf(buf, "%d", value);
		lv_label_set_text(slider_group[1].label, buf);

		g_setting.fans.left_speed = value;
  		ini_putl("fans", "left_speed", value, SETTING_INI);
		g_setting.fans.right_speed = value;
  		ini_putl("fans", "right_speed", value, SETTING_INI);
	}
}
void fans_speed_dec(void)
{
	int32_t value = 0;
	char buf[5];
	
	if( fans_mode == FANS_MODE_TOP)
	{
		value = lv_slider_get_value(slider_group[0].slider);

		if(value >= 1)
			value -= 1;

		lv_slider_set_value(slider_group[0].slider, value, LV_ANIM_OFF);	
		sprintf(buf, "%d", value);
		lv_label_set_text(slider_group[0].label, buf);

		fans_top_setspeed(value);

		g_setting.fans.top_speed = (uint8_t)value;
  		ini_putl("fans", "top_speed", value, SETTING_INI);
	}
	else if( fans_mode == FANS_MODE_SIDE)
	{
		value = lv_slider_get_value(slider_group[1].slider);

		if(value >= 1)
			value -= 1;

		lv_slider_set_value(slider_group[1].slider, value, LV_ANIM_OFF);	
		sprintf(buf, "%d", value);
		lv_label_set_text(slider_group[1].label, buf);

		g_setting.fans.left_speed = value;
  		ini_putl("fans", "left_speed", value, SETTING_INI);
		g_setting.fans.right_speed = value;
  		ini_putl("fans", "right_speed", value, SETTING_INI);
	}
}

void fans_mode_toggle(int sel)
{
	lv_obj_t *slider = slider_group[0].slider;
	int value = 0;

	if(sel == 0)
	{
		btn_group_toggle_sel(&btn_group_fans);		

		value = btn_group_get_sel(&btn_group_fans) == 0 ? 1 : 0;
		if(value)
			ini_puts("fans", "auto", "enable", SETTING_INI);
		else
			ini_puts("fans", "auto", "disable", SETTING_INI);
		
		g_setting.fans.auto_mode = value;
		return;
	}
	else if(sel == 1)
	{
		slider = slider_group[0].slider;
		fans_mode = FANS_MODE_TOP;
	}
	else if(sel == 2)
	{
		slider = slider_group[1].slider;
		fans_mode = FANS_MODE_SIDE;
	}
	else{
		return;
	}

	if(g_menu_op == PAGE_FAN_SLIDE)
	{
		g_menu_op = OPLEVEL_SUBMENU;
		lv_obj_add_style(slider, &style_silder_main, LV_PART_MAIN);
	}else{
		g_menu_op = PAGE_FAN_SLIDE;
		lv_obj_add_style(slider, &style_silder_select, LV_PART_MAIN);
	}

}

void page_fans_key(uint8_t key)
{
	
}

void step_topfan()
{
	char str[10];
	
	g_setting.fans.top_speed++;
	if(g_setting.fans.top_speed == 6)  g_setting.fans.top_speed = 0;

	fans_top_setspeed(g_setting.fans.top_speed);
	ini_putl("fans", "top_speed", g_setting.fans.top_speed, SETTING_INI);
}

///////////////////////////////////////////////////////////////////////////////
// Auto control side fans
static uint16_t respeed_cnt[2] = {0,0}; //[0]=right,[1]=left
static bool respeeding[2] = {false,false}; 

uint8_t adj_speed(uint8_t cur_speed, int tempe,uint8_t is_left)
{
	uint8_t new_speed = cur_speed;

	if(tempe > FAN_TEMPERATURE_THR_H) {
		if(new_speed != 5) {
			new_speed++;
			respeeding[is_left] = true;  respeed_cnt[is_left] = 0;
		}
	}
	else if(tempe < FAN_TEMPERATURE_THR_L) {
		if(new_speed != 0) {
			new_speed--;
			respeeding[is_left] = true;  respeed_cnt[is_left] = 0;
		}
	}

	if(cur_speed != new_speed)
		Printf("%s Fan speed: %d (T=%d)\n", is_left? "Left": "Right",new_speed,tempe);

	return new_speed;
}

void fans_auto_ctrl_core(bool is_left,int tempe, bool binit)
{
	static uint8_t speed[2]={1,1};
	uint8_t new_spd;

	//////////////////////////////////////////////////////////////////////////////////
	//reinit auto speed
	if(binit) {
		speed[0] = speed[1] = 3;
		respeed_cnt[0] = respeed_cnt[1] = 0;
		respeeding[0] = respeeding[1] = false;
		fans_right_setspeed(speed[0]);
		fans_left_setspeed(speed[1]);
	}

	if(respeeding[is_left]) {
		respeed_cnt[is_left]++;
		if(respeed_cnt[is_left] == RESPEED_WAIT_TIME) {
			respeeding[is_left] = false;
			respeed_cnt[is_left] = 0;
		}
		return;
	}

	new_spd = adj_speed(speed[is_left],tempe, is_left);
	if(new_spd != speed[is_left]) {
		speed[is_left] = new_spd;
		if(is_left)
			fans_left_setspeed(speed[1]);
		else	
			fans_right_setspeed(speed[0]);
	}
}

bool rescue_from_hot()
{
	static uint8_t speeds_saved[3];
	static bool    respeeding[3] = {false,false,false};

	//Right
	if(g_temperature.right > SIDE_TEMPERATURE_RISKH) {
		if(!respeeding[0]) {
			speeds_saved[0] = fan_speeds[0];
			respeeding[0] = true;
			fans_right_setspeed(5);
			Printf("Right fan: rescue ON.\n");
		}
	}
	else if(respeeding[0] &&(g_temperature.right < FAN_TEMPERATURE_THR_L)) {
		fans_right_setspeed(speeds_saved[0]);
		respeeding[0] = false;
		Printf("Right fan: rescue OFF.\n");
	}

	//Left
	if(g_temperature.left > SIDE_TEMPERATURE_RISKH) {
		if(!respeeding[1]) {
			speeds_saved[1] = fan_speeds[1];
			respeeding[1] = true;
			fans_left_setspeed(5);
			Printf("Left fan: rescue ON.\n");
		}
	}
	else if(respeeding[1] &&(g_temperature.left < FAN_TEMPERATURE_THR_L)) {
		fans_left_setspeed(speeds_saved[1]);
		respeeding[1] = false;
		Printf("Left fan: rescue OFF.\n");
	}

	//Top 
	if(g_temperature.top > TOP_TEMPERATURE_RISKH) {
		if(!respeeding[2]) {
			speeds_saved[2] = fan_speeds[2];
			respeeding[2] = true;
			fans_top_setspeed(5);
			Printf("Top fan: rescue ON.\n");
		}
	}
	else if(respeeding[2] &&(g_temperature.top < TOP_TEMPERATURE_NORM)) {
		fans_top_setspeed(speeds_saved[2]);
		respeeding[2] = false;
		Printf("Top fan: rescue OFF.\n");
	}

	g_temperature.is_rescuing = respeeding[0] || respeeding[1] || respeeding[2];
	return  g_temperature.is_rescuing;
}

void fans_auto_ctrl()
{
	static uint8_t auto_mode_d;
	static uint8_t speeds[3];
	uint8_t binit_r,binit_f;

	if(rescue_from_hot()) return;

	binit_r = (auto_mode_d == 0) && (g_setting.fans.auto_mode == 1);  //Manual mode -> Auto
	binit_f = (auto_mode_d == 1) && (g_setting.fans.auto_mode == 0);  //Auto   mode -> manual
	auto_mode_d = g_setting.fans.auto_mode;
	
	if(g_setting.fans.auto_mode) {
		fans_auto_ctrl_core(false, g_temperature.right, binit_r);
		fans_auto_ctrl_core(true, g_temperature.left, binit_r);
	}
	else {
		if(binit_f) 
			speeds[0] = speeds[1] = speeds[2] = 0xFF;

		if(speeds[0] != g_setting.fans.right_speed) {
			fans_right_setspeed(g_setting.fans.right_speed);
			speeds[0] = g_setting.fans.right_speed;
		}

		if(speeds[1] != g_setting.fans.left_speed) {
			fans_left_setspeed(g_setting.fans.left_speed);
			speeds[1] = g_setting.fans.left_speed;
		}

		if(speeds[2] != g_setting.fans.top_speed) {
			fans_top_setspeed(g_setting.fans.top_speed);
			speeds[2] = g_setting.fans.top_speed;
		}
	}
}
