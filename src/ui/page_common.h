#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "defines.h"
#include "ui/ui_style.h"

#define TMP_DIR             "/tmp"
#define AUDIO_SEL_SH        "/mnt/app/script/audio_sel.sh"
#define SETTING_INI_VERSION 1
#ifndef EMULATOR_BUILD
#define SETTING_INI "/mnt/app/setting.ini"
#else
#define SETTING_INI "setting.ini"
#endif
#define TEST_INI      "/mnt/extsd/test.ini"
#define REC_START     "/mnt/app/app/record/gogglecmd -rec start"
#define REC_STOP      "/mnt/app/app/record/gogglecmd -rec stop"
#define REC_STOP_LIVE "/mnt/app/app/record/gogglecmd -rec stopl"
#define REC_CONF      "/mnt/app/app/record/confs/record.conf"
#define WIFI_DOWNLOAD "/mnt/app/script/online_downloader.sh"
#define WIFI_OFF      "/mnt/app/script/wlan_stop.sh"
#define WIFI_AP_ON    "/tmp/wlan_start_ap.sh"
#define WIFI_STA_ON   "/tmp/wlan_start_sta.sh"
#define ROOT_PW_SET   "/tmp/root_pw_set.sh"

#define FC_OSD_LOCAL_PATH  "/mnt/app/resource/OSD/FC/"
#define FC_OSD_SDCARD_PATH "/mnt/extsd/resource/OSD/FC/"

#define RESOURCE_PATH        "/mnt/app/resource/OSD/GOGGLE/"
#define RESOURCE_PATH_SDCARD "/mnt/extsd/resource/OSD/GOGGLE/"

#define recording_bmp "recording.bmp"
#define noSdcard_bmp  "noSdcard.bmp"

#define lowBattery_gif   "lowBattery.gif"
#define fan1_bmp         "fan1.bmp"
#define fan2_bmp         "fan2.bmp"
#define fan3_bmp         "fan3.bmp"
#define fan4_bmp         "fan4.bmp"
#define fan5_bmp         "fan5.bmp"
#define fan6_bmp         "fan6.bmp"
#define VtxTemp1_bmp     "VtxTemp1.bmp"
#define VtxTemp2_bmp     "VtxTemp2.bmp"
#define VtxTemp3_bmp     "VtxTemp3.bmp"
#define VtxTemp4_bmp     "VtxTemp4.bmp"
#define VtxTemp5_bmp     "VtxTemp5.bmp"
#define VtxTemp6_bmp     "VtxTemp6.bmp"
#define VtxTemp7_bmp     "VtxTemp7.bmp"
#define VrxTemp7_gif     "VrxTemp7.gif"
#define ant1_bmp         "ant1.bmp"
#define ant2_bmp         "ant2.bmp"
#define ant3_bmp         "ant3.bmp"
#define ant4_bmp         "ant4.bmp"
#define ant5_bmp         "ant5.bmp"
#define ant6_bmp         "ant6.bmp"
#define VLQ1_bmp         "VLQ1.bmp"
#define VLQ2_bmp         "VLQ2.bmp"
#define VLQ3_bmp         "VLQ3.bmp"
#define VLQ4_bmp         "VLQ4.bmp"
#define VLQ5_bmp         "VLQ5.bmp"
#define VLQ6_bmp         "VLQ6.bmp"
#define VLQ7_bmp         "VLQ7.bmp"
#define VLQ8_bmp         "VLQ8.bmp"
#define VLQ9_bmp         "VLQ9.bmp"
#define blank_bmp        "blank36x36.bmp"
#define LLOCK_bmp        "llock.bmp"
#define DEF_VIDEOICON    "videoicon.jpg"
#define FOCUS_CHART_IMG  "focus_chart.png"
#define DIAL_CLICK_IMG   "dial_click.png"
#define DIAL_SCROLL_IMG  "dial_scroll.png"
#define RIGHT_BUTTON_IMG "right_button.png"
#define ALERT_IMG        "alert.png"

#define MAX_PANELS 9

#define FLAG_SELECTABLE LV_OBJ_FLAG_USER_1
#define STATE_DISABLED LV_STATE_USER_1
#define COLOR_DISABLED (lv_color_darken(lv_color_white(), 127))

typedef enum {
    SOURCE_HDZERO = 0,
    SOURCE_HDMI_IN = 1,
    SOURCE_AV_IN = 2,
    SOURCE_EXPANSION = 3
} source_t;

typedef struct _source_info {
    source_t source;
    uint8_t hdmi_in_status; // 0=not detected, 1= detected
    uint8_t av_in_status;   // 0=not detected, 1= detected
    uint8_t av_bay_status;  // 0=not detected, 1= detected
} source_info_t;

typedef struct {
    lv_obj_t *container;
    lv_obj_t *arrow;
    lv_obj_t *btn;
    lv_obj_t *label;
} btn_with_arr_t;

typedef struct {
    btn_with_arr_t btn_a[6];
    int valid;
    int current;
    lv_obj_t *label;
} btn_group_t;

typedef struct {
    lv_obj_t *slider;
    lv_obj_t *name;
    lv_obj_t *label;
} slider_group_t;

typedef struct {
    lv_obj_t *panel[MAX_PANELS];
    int cur;
    int max;
} panel_arr_t;

struct menu_obj_s {
    lv_obj_t *cont;
    lv_obj_t *icon;
};
enum {
    LV_MENU_ITEM_BUILDER_VARIANT_1,
    LV_MENU_ITEM_BUILDER_VARIANT_2
};
typedef uint8_t lv_menu_builder_variant_t;

extern bool g_sdcard_enable;
extern bool g_sdcard_det_req;
extern int g_sdcard_size;
extern bool g_autoscan_exit;
extern bool g_scanning;
extern bool g_latency_locked;
extern source_info_t g_source_info;

int create_text(struct menu_obj_s *s, lv_obj_t *parent, bool is_icon, const char *txt,
                lv_menu_builder_variant_t builder_variant);

void create_slider_item(slider_group_t *slider_group, lv_obj_t *parent, const char *name, int range, int default_value, int row);

void create_slider_item_compact(slider_group_t *slider_group, lv_obj_t *parent, const char *name, int range, int default_value, int row, const lv_font_t *font);

void update_slider_item_with_value(slider_group_t *slider_group, int value);

void create_btn_item(lv_obj_t *parent, const char *name, int col, int row);

lv_obj_t *create_dropdown_item(lv_obj_t *parent, const char *options, int col, int row, int width, int height, int col_span, int pad_top, lv_grid_align_t column_align, const lv_font_t *font);

lv_obj_t *create_label_item_compact(lv_obj_t *parent, const char *name, int col, int row, int cols, int height, lv_text_align_t text_align, lv_grid_align_t col_align, const lv_font_t *font);

lv_obj_t *create_msgbox_item(const char *title, const char *message);

lv_obj_t *create_label_item(lv_obj_t *parent, const char *name, int col, int row, int cols);

lv_obj_t *create_info_item(lv_obj_t *parent, const char *name, int col, int row, int cols);

void create_btn_group_item_compact(btn_group_t *btn_group, lv_obj_t *parent, int count, const char *name, const char *name0, const char *name1,
                                   const char *name2, const char *name3, int row, int height, int arrow_scale_percent, const lv_font_t *font);
void create_btn_group_item(btn_group_t *btn_group, lv_obj_t *parent, int count, const char *name, const char *name0, const char *name1,
                           const char *name2, const char *name3, int row);

void create_btn_group_item2(btn_group_t *btn_group, lv_obj_t *parent, int count, const char *name, const char *name0, const char *name1,
                            const char *name2, const char *name3, const char *name4, const char *name5, int row);

void btn_group_set_sel(btn_group_t *btn_group, int sel);
int btn_group_get_sel(btn_group_t *btn_group);
void btn_group_toggle_sel(btn_group_t *btn_group);

void create_select_item(panel_arr_t *arr, lv_obj_t *parent);
void set_select_item(const panel_arr_t *arr, int row);

void slider_show(slider_group_t *slider_group, bool visible);
void slider_enable(slider_group_t *slider_group, bool enable);
void btn_group_show(btn_group_t *btn_group, bool visible);
void btn_group_enable(btn_group_t *btn_group, bool enable);

#ifdef __cplusplus
}
#endif
