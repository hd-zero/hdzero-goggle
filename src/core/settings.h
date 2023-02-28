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
    uint8_t auto_off; // 0=3min,1=4min,2=5min,3=never,
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
    setting_osd_goggle_element_t goggle_temp_top;
    setting_osd_goggle_element_t goggle_temp_left;
    setting_osd_goggle_element_t goggle_temp_right;
} setting_osd_goggle_elements_t;

typedef struct {
    setting_embedded_mode_t embedded_mode;
    setting_osd_goggle_elements_t elements;
} setting_osd_t;

typedef struct {
    setting_scan_t scan;
    setting_fan_t fans;
    setting_autoscan_t autoscan;
    setting_power_t power;
    setting_record_t record;
    setting_image_t image;
    setting_head_tracker_t ht;
    setting_elrs_t elrs;
    setting_osd_t osd;
} setting_t;

extern setting_t g_setting;
extern bool g_test_en;
extern const setting_t g_setting_defaults;

void settings_load(void);