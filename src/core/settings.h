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
    SETTING_AUTOSCAN_STATUS_ON = 0,
    SETTING_AUTOSCAN_STATUS_LAST = 1,
    SETTING_AUTOSCAN_STATUS_OFF = 2
} setting_autoscan_status_t;

typedef enum {
    SETTING_AUTOSCAN_SOURCE_LAST = 0,
    SETTING_AUTOSCAN_SOURCE_HDZERO = 1,
    SETTING_AUTOSCAN_SOURCE_EXPANSION = 2,
    SETTING_AUTOSCAN_SOURCE_AV_IN = 3,
    SETTING_AUTOSCAN_SOURCE_HDMI_IN = 4
} setting_autoscan_source_t;

typedef struct {
    setting_autoscan_status_t status;
    setting_autoscan_source_t last_source;
    setting_autoscan_source_t source;
} setting_autoscan_t;

typedef enum {
    SETTING_POWER_CELL_COUNT_MODE_AUTO = 0,
    SETTING_POWER_CELL_COUNT_MODE_MANUAL = 1
} setting_power_cell_count_mode_t;

typedef enum {
    SETTING_POWER_OSD_DISPLAY_MODE_TOTAL = 0,
    SETTING_POWER_OSD_DISPLAY_MODE_CELL = 1
} setting_power_osd_display_mode_t;

typedef enum {
    SETTING_POWER_WARNING_TYPE_BEEP = 0,
    SETTING_POWER_WARNING_TYPE_VISUAL = 1,
    SETTING_POWER_WARNING_TYPE_BOTH = 2
} setting_power_warning_type_t;

typedef struct {
    int voltage;
    setting_power_warning_type_t warning_type;
    setting_power_cell_count_mode_t cell_count_mode;
    int cell_count;
    setting_power_osd_display_mode_t osd_display_mode;
    bool power_ana;
} setting_power_t;

typedef enum {
    SETTING_RECORD_AUDIO_SOURCE_MIC = 0,
    SETTING_RECORD_AUDIO_SOURCE_LINE_IN = 1,
    SETTING_RECORD_AUDIO_SOURCE_AV_IN = 2
} setting_record_audio_source_t;

typedef struct {
    bool mode_manual;
    bool format_ts;
    bool osd;
    bool audio;
    setting_record_audio_source_t audio_source;
} setting_record_t;

typedef struct {
    uint8_t oled;
    uint8_t brightness;
    uint8_t saturation;
    uint8_t contrast;
    uint8_t auto_off; // 0=1min,1=3min,2=4min,3=5min,4=never,
} setting_image_t;

typedef struct {
    bool enable;
    int max_angle;
    int32_t acc_x;
    int32_t acc_y;
    int32_t acc_z;
    int32_t gyr_x;
    int32_t gyr_y;
    int32_t gyr_z;
} setting_head_tracker_t;

typedef struct {
    bool enable;
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

typedef struct {
    setting_osd_goggle_element_t topfan_speed;
    setting_osd_goggle_element_t latency_lock;
    setting_osd_goggle_element_t vtx_temp;
    setting_osd_goggle_element_t vrx_temp;
    setting_osd_goggle_element_t battery_low;
    setting_osd_goggle_element_t channel;
    setting_osd_goggle_element_t sd_rec;
    setting_osd_goggle_element_t vlq;
    setting_osd_goggle_element_t ant0;
    setting_osd_goggle_element_t ant1;
    setting_osd_goggle_element_t ant2;
    setting_osd_goggle_element_t ant3;
    setting_osd_goggle_element_t osd_tempe[3];
} setting_osd_goggle_elements_t;

typedef struct {
    setting_embedded_mode_t embedded_mode;
    setting_osd_goggle_elements_t elements;
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
    bool enable;
    char ssid[16];   // not implemented yet, load from configure file from sd card, otherwise use default "HDZero"
    char passwd[16]; // default: "divimath"
    uint8_t ip[4];   // default: 192.168.2.122
} wifi_t;

typedef struct {
    uint8_t no_dial; // 1=disable turning channels under video mode
} ease_use_t;

typedef enum {
    SETTING_SOURCES_ANALOG_FORMAT_NTSC = 0,
    SETTING_SOURCES_ANALOG_FORMAT_PAL = 1
} setting_sources_analog_format_t;

typedef struct {
    setting_sources_analog_format_t analog_format; // 0=NTSC, 1= PAL
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

void settings_load(void);
bool settings_get_bool(char* section, char* key, bool default_val);
int settings_put_bool(char* section, char* key, bool value);
