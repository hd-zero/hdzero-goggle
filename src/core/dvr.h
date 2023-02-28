#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    DVR_TOGGLE,
    DVR_STOP,
    DVR_START,
} osd_dvr_cmd_t;

extern bool dvr_is_recording;

void dvr_update_status();
void dvr_select_audio_source(uint8_t audio_source);
void dvr_enable_line_out(bool enable);
void dvr_cmd(osd_dvr_cmd_t cmd);