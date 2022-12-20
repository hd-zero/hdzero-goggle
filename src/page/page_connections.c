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
#include "../minIni/minIni.h"
#include "../driver/esp32.h"
#include "../core/common.hh"
#include "../core/esp32_flash.h"
#include "../core/elrs.h"


static lv_coord_t col_dsc[] = {180,200,160,160,160,160, LV_GRID_TEMPLATE_LAST};
static lv_coord_t row_dsc[] = {60,60,40,40,40,60,40,40,60,60, LV_GRID_TEMPLATE_LAST};
static lv_obj_t *btn_flash;
static lv_obj_t *btn_wifi;
static lv_obj_t *btn_bind;
static btn_group_t elrs_group;
static lv_obj_t *elrs_bar = NULL;

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

	create_btn_group_item(&elrs_group, cont, 2, "Backpack", "On", "Off", "","",  0);
	btn_group_set_sel(&elrs_group, !g_setting.elrs.enable);
	create_label_item(cont,  "ESP Firmware", 1, 1, 1);
	btn_group_t btn_group;
	btn_flash = create_label_item(cont, "Update Via SD", 2, 1, 1);
	btn_wifi = create_label_item(cont, "Start WIFI", 2, 2, 1);
	btn_bind = create_label_item(cont, "Start Binding", 2, 3, 1);
	create_btn_group_item(&btn_group, cont, 2, "Wifi AP*", "On", "Off", "","",  4);
	create_label_item(cont,  "Wifi Settings", 1, 5, 1);
	create_label_item(cont,  "Configure", 2, 5, 1);
	create_label_item(cont,  "SSD: HDZero", 2, 6, 1);
	//create_label_item(cont,  "Pass: hdzero123", 2, 7, 1);
	create_label_item(cont,  "broadcast ID: Yes", 2, 7, 1);

	create_label_item(cont, "<Back", 1, 8, 1);

	lv_obj_t *label2 = lv_label_create(cont);
   	lv_label_set_text(label2, "*Expansion module is required.");
	lv_obj_set_style_text_font(label2, &lv_font_montserrat_16, 0);
	lv_obj_set_style_text_align(label2, LV_TEXT_ALIGN_LEFT, 0);
	lv_obj_set_style_text_color(label2, lv_color_make(255,255,255), 0);
	lv_obj_set_style_pad_top(label2, 12, 0);
	lv_label_set_long_mode(label2, LV_LABEL_LONG_WRAP);
	lv_obj_set_grid_cell(label2, LV_GRID_ALIGN_START, 1, 4,
						 LV_GRID_ALIGN_START, 9, 2);

	elrs_bar = lv_bar_create(cont);
    lv_obj_set_size(elrs_bar, 260, 20);
	lv_obj_set_grid_cell(elrs_bar, LV_GRID_ALIGN_CENTER, 3, 3,
						 LV_GRID_ALIGN_CENTER, 1, 1);
	lv_obj_add_flag(elrs_bar, LV_OBJ_FLAG_HIDDEN);

	return page;
}

#undef RETURN_ON_ERROR
#define RETURN_ON_ERROR(m, x) do {      \
    esp_loader_error_t _err_ = (x);     \
    if (_err_ != ESP_LOADER_SUCCESS) {  \
		Printf("ERR %s: %d\n", m, _err_);   \
        return _err_;                   \
    }                                   \
} while(0)

static esp_loader_error_t flash_esp32_file(char *path, uint32_t offset)
{
	char fpath[80];
	strcpy(fpath, "/mnt/extsd/ELRS/");
	strcat(fpath, path);

	FILE *image = fopen(fpath, "r");
    if (image == NULL) {
        Printf("Firmware file does not exist %s\n", fpath);
        return ESP_LOADER_SUCCESS;
    }

	lv_label_set_text(btn_flash, path);
	lv_timer_handler();

    fseek(image, 0L, SEEK_END);
    size_t size = ftell(image);
    rewind(image);

	uint8_t buffer[4096];
	RETURN_ON_ERROR("start", esp_loader_flash_start(offset, size, sizeof(buffer)));
	uint32_t read, current = 0;
	while((read = fread(buffer, 1, sizeof(buffer), image))) {
		RETURN_ON_ERROR("write", esp_loader_flash_write(buffer, read));
		current += read;
		int percent = (current * 100) / size;
		lv_bar_set_value(elrs_bar, percent, LV_ANIM_OFF);
		lv_timer_handler();
		Printf("%d %d %d\n", current, size, percent);
	}
	return ESP_LOADER_SUCCESS;
}

static esp_loader_error_t flash_esp32()
{
	disable_esp32();

	esp_loader_connect_args_t config = ESP_LOADER_CONNECT_DEFAULT();
	RETURN_ON_ERROR("init", loader_port_init());
	RETURN_ON_ERROR("connect", esp_loader_connect(&config));
	RETURN_ON_ERROR("get_target", esp_loader_get_target() == ESP32_CHIP ? ESP_LOADER_SUCCESS : ESP_LOADER_ERROR_UNSUPPORTED_CHIP);

	RETURN_ON_ERROR("flash", flash_esp32_file("bootloader.bin", 0x1000));
	RETURN_ON_ERROR("flash", flash_esp32_file("partitions.bin", 0x8000));
	RETURN_ON_ERROR("flash", flash_esp32_file("boot_app0.bin", 0xE000));
	RETURN_ON_ERROR("flash", flash_esp32_file("firmware.bin", 0x10000));

	RETURN_ON_ERROR("finish", esp_loader_flash_finish(true));
	loader_port_close();

	if (g_setting.elrs.enable)
		enable_esp32();

	return ESP_LOADER_SUCCESS;
}

static bool flash_elrs()
{
	esp_loader_error_t ret = flash_esp32();
	loader_port_close();

	if (g_setting.elrs.enable)
		enable_esp32();

	return ret;
}

void connect_function(int sel)
{
	if(sel == 0)
	{
		btn_group_toggle_sel(&elrs_group);
		g_setting.elrs.enable = btn_group_get_sel(&elrs_group) == 0 ? 1 : 0;
		ini_putl("elrs", "enable", g_setting.elrs.enable, SETTING_INI);
		if(g_setting.elrs.enable)
			enable_esp32();
		else
			disable_esp32();
	}
	else if(sel == 1) // flash ESP via SD
	{
		lv_obj_clear_flag(elrs_bar, LV_OBJ_FLAG_HIDDEN);
		lv_label_set_text(btn_flash, "Flashing...");
		lv_timer_handler();
		esp_loader_error_t ret = flash_elrs();
		lv_obj_add_flag(elrs_bar, LV_OBJ_FLAG_HIDDEN);
		if(ret == ESP_LOADER_SUCCESS)
			lv_label_set_text(btn_flash, "Success");
		else
			lv_label_set_text(btn_flash, "Failed");
	}
	else if(sel == 2) // start ESP Wifi
	{
		msp_send_packet(MSP_SET_MODE, MSP_PACKET_COMMAND, 1, (uint8_t *)"W");
		lv_label_set_text(btn_wifi, "Starting...");
		lv_timer_handler();
		if (!msp_await_resposne(MSP_SET_MODE, 1, (uint8_t *)"P", 1000)) {
			lv_label_set_text(btn_wifi, "Failed");
			return;
		}
		lv_label_set_text(btn_wifi, "Success");
	}
	else if(sel == 3) // start ESP bind
	{
		msp_send_packet(MSP_SET_MODE, MSP_PACKET_COMMAND, 1, (uint8_t *)"B");
		lv_label_set_text(btn_bind, "Binding...");
		lv_timer_handler();
		if (!msp_await_resposne(MSP_SET_MODE, 1, (uint8_t *)"P", 1000)) {
			lv_label_set_text(btn_bind, "Failed");
			return;
		}
		if(!msp_await_resposne(MSP_SET_MODE, 1, (uint8_t *)"O", 120000)) {
			lv_label_set_text(btn_bind, "Timeout");
		} else {
			lv_label_set_text(btn_bind, "Success");
		}
	}
}
