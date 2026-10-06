#include "motor_audio_sdl.h"

#include "core/motor_audio.h"
#include "core/SDLaccess.h"

#include <stdio.h>
#include <string.h>

static SDL_AudioDeviceID g_dev;
static uint32_t g_rate;

static void motor_audio_sdl_callback(void *userdata, Uint8 *stream, int len) {
    int16_t *out = (int16_t *)stream;
    uint32_t frames = (uint32_t)(len / (int)(sizeof(int16_t) * MOTOR_AUDIO_CHANNELS));
    (void)userdata;
    motor_audio_render(out, frames, g_rate, 0);
}

int motor_audio_sdl_start(uint32_t sample_rate) {
    SDL_AudioSpec want, have;

    if (g_dev)
        return 0;

    if (sample_rate == 0)
        sample_rate = MOTOR_AUDIO_DEFAULT_RATE;
    g_rate = sample_rate;

    if (SDL_WasInit(SDL_INIT_AUDIO) == 0) {
        if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
            fprintf(stderr, "motor_audio_sdl: SDL_InitSubSystem AUDIO failed: %s\n", SDL_GetError());
            return -1;
        }
    }

    memset(&want, 0, sizeof(want));
    want.freq = (int)sample_rate;
    want.format = AUDIO_S16SYS;
    want.channels = MOTOR_AUDIO_CHANNELS;
    want.samples = 1024;
    want.callback = motor_audio_sdl_callback;

    g_dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
    if (!g_dev) {
        fprintf(stderr, "motor_audio_sdl: OpenAudioDevice failed: %s\n", SDL_GetError());
        return -1;
    }

    motor_audio_reset();
    SDL_PauseAudioDevice(g_dev, 0);
    return 0;
}

void motor_audio_sdl_stop(void) {
    if (!g_dev)
        return;
    SDL_CloseAudioDevice(g_dev);
    g_dev = 0;
}

int motor_audio_sdl_is_running(void) {
    return g_dev != 0;
}
