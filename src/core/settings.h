#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint8_t top_speed;
    bool auto_mode;
    uint8_t left_speed;
    uint8_t right_speed;
} setting_fan_t;

typedef struct {
    int channel;
} setting_scan_t;

typedef enum {
    SETTING_AUTOSCAN_SCAN = 0,
    SETTING_AUTOSCAN_LAST = 1,
    SETTING_AUTOSCAN_MENU = 2
} setting_status_t;

typedef enum {
    SETTING_SOURCE_LAST = 0,
    SETTING_SOURCE_HDZERO = 1,
    SETTING_SOURCE_EXPANSION = 2,
    SETTING_SOURCE_AV_IN = 3,
    SETTING_SOURCE_HDMI_IN = 4
} setting_source_t;

typedef struct {
    setting_status_t status;
    setting_source_t last_source;
    setting_source_t source;
} setting_autoscan_t;

typedef enum {
    SETTING_POWER_CELL_COUNT_MODE_AUTO = 0,
    SETTING_POWER_CELL_COUNT_MODE_MANUAL = 1
} setting_power_cell_count_mode_t;

typedef enum {
    SETTING_POWER_OSD_DISPLAY_MODE_TOTAL = 0,
    SETTING_POWER_OSD_DISPLAY_MODE_CELL = 1
} setting_power_osd_display_mode_t;

typedef struct {
    int voltage;
    bool display_voltage;
    int warning_type; // 0=beep,1=visual,2=both
    setting_power_cell_count_mode_t cell_count_mode;
    int cell_count;
    setting_power_osd_display_mode_t osd_display_mode;
    bool power_ana;
} setting_power_t;

typedef struct {
    bool mode_manual;
    bool format_ts;
    bool osd;
    bool audio;
    int audio_source; // 0=MIC,1=Line in,2=AV in
} setting_record_t;

typedef struct {
    uint8_t oled;
    uint8_t brightness;
    uint8_t saturation;
    uint8_t contrast;
    uint8_t auto_off; // 0=1min,1=3min,2=4min,3=5min,4=never,
} setting_image_t;

typedef struct {
    int enable;
    int max_angle;
    int32_t acc_x;
    int32_t acc_y;
    int32_t acc_z;
    int32_t gyr_x;
    int32_t gyr_y;
    int32_t gyr_z;
} setting_head_tracker_t;

typedef struct {
    int enable;
} setting_elrs_t;

typedef enum {
    EMBEDDED_4x3,
    EMBEDDED_16x9
} setting_embedded_mode_t;

typedef struct {
    int x;
    int y;
} setting_osd_goggle_element_position_t;

typedef struct {
    setting_osd_goggle_element_position_t mode_4_3;
    setting_osd_goggle_element_position_t mode_16_9;
} setting_osd_goggle_element_positions_t;

typedef struct {
    bool show;
    setting_osd_goggle_element_positions_t position;
} setting_osd_goggle_element_t;

typedef enum {
    OSD_GOGGLE_TOPFAN_SPEED = 0,
    OSD_GOGGLE_LATENCY_LOCK,
    OSD_GOGGLE_VTX_TEMP,
    OSD_GOGGLE_VRX_TEMP,
    OSD_GOGGLE_BATTERY_LOW,
    OSD_GOGGLE_CHANNEL,
    OSD_GOGGLE_SD_REC,
    OSD_GOGGLE_VLQ,
    OSD_GOGGLE_ANT0,
    OSD_GOGGLE_ANT1,
    OSD_GOGGLE_ANT2,
    OSD_GOGGLE_ANT3,
    OSD_GOGGLE_TEMP_TOP,
    OSD_GOGGLE_TEMP_LEFT,
    OSD_GOGGLE_TEMP_RIGHT,
    OSD_GOGGLE_NUM,
} osd_goggle_element_e;

typedef struct {
    setting_embedded_mode_t embedded_mode;
    setting_osd_goggle_element_t element[OSD_GOGGLE_NUM];
} setting_osd_t;

typedef struct {
    int year;
    int month;
    int day;
    int hour;
    int min;
    int sec;
    int format;
} setting_clock_t;

typedef struct {
    int enable;
    char ssid[16];   // loaded from configure file from sd card, otherwise use default "HDZero"
    char passwd[16]; // default: "divimath"
    uint8_t ip[4];   // not used, default: 192.168.2.122
    int rate;        // default: 2500Kbps
    int channel;     // default: 6
    char std[2];     // g or n
} wifi_t;

typedef struct {
    uint8_t no_dial; // 1=disable turning channels under video mode
} ease_use_t;
typedef struct {
    uint8_t analog_format; // 0=NTSC, 1= PAL
} setting_sources_t;

typedef struct {
    setting_scan_t scan;
    setting_fan_t fans;
    setting_autoscan_t autoscan;
    setting_power_t power;
    setting_sources_t source;
    setting_record_t record;
    setting_image_t image;
    setting_head_tracker_t ht;
    setting_elrs_t elrs;
    wifi_t wifi;
    setting_osd_t osd;
    setting_clock_t clock;
    ease_use_t ease;
} setting_t;

extern setting_t g_setting;
extern bool g_test_en;
extern const setting_t g_setting_defaults;

void settings_reset(void);
void settings_init(void);
void settings_load(void);
bool settings_get_bool(char *section, char *key, bool default_val);
int settings_put_bool(char *section, char *key, bool value);
int settings_put_osd_element(const setting_osd_goggle_element_t *element, char *config_name);
int settings_put_osd_element_pos_y(const setting_osd_goggle_element_positions_t *pos, char *config_name);
int settings_put_osd_element_pos_x(const setting_osd_goggle_element_positions_t *pos, char *config_name);
int settings_put_osd_element_shown(bool show, char *config_name);