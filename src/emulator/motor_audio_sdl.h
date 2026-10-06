#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* SDL2 audio output feeding the shared motor_audio renderer. Emulator only. */
int motor_audio_sdl_start(uint32_t sample_rate);
void motor_audio_sdl_stop(void);
int motor_audio_sdl_is_running(void);

#ifdef __cplusplus
}
#endif
