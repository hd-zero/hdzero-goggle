#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/*
 * Synthetic motor / propeller audio from mechanical RPM telemetry.
 *
 * Platform-independent PCM generator. Emulator uses SDL2/WAV backends;
 * hardware goggles currently have no general-purpose PCM playback path
 * (beep.c is GPIO only; adec2ao is Softwinner media decode). Wire this
 * module to a real AO/PCM sink when one is available.
 */

#define MOTOR_AUDIO_MOTORS           4
#define MOTOR_AUDIO_CHANNELS         2 /* stereo L/R interleaved */
/* 22.05 kHz is enough for blade-pass + a few harmonics (~<8 kHz); halves CPU vs 48 kHz. */
#define MOTOR_AUDIO_DEFAULT_RATE     22050
#define MOTOR_AUDIO_STALE_MS         500
#define MOTOR_AUDIO_SMOOTH_MS        40

/*
 * Betaflight QUADX motor order (nose forward):
 *   4 (FL)   2 (FR)
 *   3 (RL)   1 (RR)
 * Front motors are louder; left/right are panned in stereo.
 */

void motor_audio_reset(void);

/*
 * Optional clock override for offline replay/tests. When set, motor_audio_now_ms()
 * returns (*fn)() instead of wall time. Pass NULL to restore wall clock.
 */
void motor_audio_set_time_fn(uint32_t (*fn)(void));
uint32_t motor_audio_now_ms(void);
/* Internal PCM timeline used when render(..., timestamp_ms=0). */
uint32_t motor_audio_render_clock_ms(void);

/* Update target RPM for four motors. timestamp_ms is a monotonic clock. */
void motor_audio_set_rpm(uint32_t rpm0, uint32_t rpm1, uint32_t rpm2, uint32_t rpm3, uint32_t timestamp_ms);

/*
 * Render stereo int16 PCM, interleaved L,R into output[frame_count * 2].
 * sample_rate typically MOTOR_AUDIO_DEFAULT_RATE. Uses an internal render clock advanced by
 * frame_count; pass timestamp_ms for stale-fade decisions (0 = use internal).
 */
void motor_audio_render(int16_t *output, uint32_t frame_count, uint32_t sample_rate, uint32_t timestamp_ms);

/* Peak absolute sample seen since last reset (for tests). */
int16_t motor_audio_peak_abs(void);

#ifdef __cplusplus
}
#endif
