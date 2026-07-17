// video_emu.c - Emulator-only mock of the HDZero video device.
//
// On the goggle, the DM5680 receiver decodes the RF and the display engine
// overlays the decoded video *under* the LVGL UI in hardware -- the app never
// touches the pixels. The desktop emulator has no receiver, so this module stands
// in for that hardware at the same seam: it decodes a small looping MPEG-1 clip
// and hands out the most recent frame as tightly-packed RGB24 for the SDL
// compositor to draw behind the FPV/OSD view, exactly where the overlay would go.
//
// Decoding uses pl_mpeg (a single-header MPEG-1 decoder), so the module compiles
// and runs identically on Linux / macOS / Windows with NO ffmpeg in the app. The
// clip is produced offline (a build step runs ffmpeg once to turn a test video
// into loop.mpg); ffmpeg is never linked into any app binary.
//
// Design:
//   * Decode runs on its own pthread: plm_decode_video() -> plm_frame_to_rgb()
//     straight into an RGB24 back buffer.
//   * Double buffer: the decoder writes the "back" buffer, then takes the mutex
//     only to flip the front index. video_emu_get() copies the front buffer out.
//   * Looping is handled by pl_mpeg (plm_set_loop), so playback repeats forever.
//   * Pacing: sleep the remainder of the frame interval (from the clip's rate).

#ifdef EMULATOR_BUILD

#include "video_emu.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define PL_MPEG_IMPLEMENTATION
#include <pl_mpeg/pl_mpeg.h>

#include <log/log.h>

typedef struct {
    int w; // frame geometry (taken from the clip)
    int h;

    plm_t *plm;

    // RGB24 double buffer, each w*h*3 bytes, no row padding
    uint8_t *buf[2];
    int front_idx;
    bool has_frame;

    pthread_t tid;
    pthread_mutex_t mutex;
    volatile bool running;
    volatile bool paused; // true = decode thread idles (not in the video view)
    bool started;

    int64_t frame_interval_ns;
} video_emu_ctx_t;

static video_emu_ctx_t g_video_emu;

// Release the decoder + buffers. Idempotent; does not touch the mutex.
static void video_emu_free_resources(video_emu_ctx_t *s) {
    if (s->plm) {
        plm_destroy(s->plm);
        s->plm = NULL;
    }
    for (int i = 0; i < 2; i++) {
        free(s->buf[i]);
        s->buf[i] = NULL;
    }
}

// Decode loop: pull frames forever (pl_mpeg loops for us), convert to RGB into
// the back buffer, flip to front, and pace to the clip's frame rate.
static void *video_emu_thread(void *arg) {
    video_emu_ctx_t *s = (video_emu_ctx_t *)arg;

    while (s->running) {
        // Not in the video view: idle instead of decoding. No frames are pulled,
        // converted, or paced -- the "receiver" is effectively off, so the menu
        // costs no video CPU. A short sleep keeps resume latency low.
        if (s->paused) {
            usleep(20000);
            continue;
        }

        struct timespec t_start;
        clock_gettime(CLOCK_MONOTONIC, &t_start);

        plm_frame_t *frame = plm_decode_video(s->plm);
        if (frame == NULL) {
            // With looping enabled this shouldn't occur; back off to avoid a spin.
            usleep(10000);
            continue;
        }

        int back = 1 - s->front_idx;
        plm_frame_to_rgb(frame, s->buf[back], s->w * 3);

        pthread_mutex_lock(&s->mutex);
        s->front_idx = back;
        s->has_frame = true;
        pthread_mutex_unlock(&s->mutex);

        struct timespec t_now;
        clock_gettime(CLOCK_MONOTONIC, &t_now);
        int64_t elapsed_ns = (int64_t)(t_now.tv_sec - t_start.tv_sec) * 1000000000LL +
                             (int64_t)(t_now.tv_nsec - t_start.tv_nsec);
        int64_t sleep_ns = s->frame_interval_ns - elapsed_ns;
        if (sleep_ns > 0)
            usleep((useconds_t)(sleep_ns / 1000));
    }

    return NULL;
}

int video_emu_start(const char *path, int w, int h) {
    video_emu_ctx_t *s = &g_video_emu;

    if (path == NULL || path[0] == '\0' || w <= 0 || h <= 0)
        return -1;
    if (s->started) // already playing
        return -1;

    memset(s, 0, sizeof(*s));

    s->plm = plm_create_with_filename(path);
    if (s->plm == NULL) {
        // Missing/invalid clip: the feature is simply absent (gray FPV), not broken.
        LOGE("video_emu: cannot open '%s'", path);
        return -1;
    }
    plm_set_audio_enabled(s->plm, 0);
    plm_set_loop(s->plm, 1);

    // The clip is authored at display resolution (the build step encodes it that
    // way), so its frames drop straight into the compositor with no scaling.
    s->w = plm_get_width(s->plm);
    s->h = plm_get_height(s->plm);
    if (s->w <= 0 || s->h <= 0) {
        s->w = w;
        s->h = h;
    }
    if (s->w != w || s->h != h)
        LOGI("video_emu: clip is %dx%d but compositor wants %dx%d (re-encode to match for video)", s->w, s->h, w, h);

    size_t buf_size = (size_t)s->w * (size_t)s->h * 3;
    s->buf[0] = (uint8_t *)calloc(1, buf_size);
    s->buf[1] = (uint8_t *)calloc(1, buf_size);
    if (s->buf[0] == NULL || s->buf[1] == NULL) {
        video_emu_free_resources(s);
        return -1;
    }

    double fps = plm_get_framerate(s->plm);
    if (fps <= 0.0)
        fps = 30.0;
    s->frame_interval_ns = (int64_t)(1000000000.0 / fps);

    if (pthread_mutex_init(&s->mutex, NULL) != 0) {
        video_emu_free_resources(s);
        return -1;
    }

    s->running = true;
    s->paused = true; // start idle; the flush path resumes us once the video view is shown
    if (pthread_create(&s->tid, NULL, video_emu_thread, s) != 0) {
        s->running = false;
        pthread_mutex_destroy(&s->mutex);
        video_emu_free_resources(s);
        return -1;
    }

    s->started = true;
    LOGI("video_emu: playing '%s' -> %dx%d RGB24 @ %.2f fps", path, s->w, s->h, fps);
    return 0;
}

bool video_emu_get(uint8_t *dst, int w, int h) {
    video_emu_ctx_t *s = &g_video_emu;

    if (dst == NULL || !s->started)
        return false;
    // Geometry must match what playback produced; refuse rather than garble.
    if (w != s->w || h != s->h)
        return false;

    bool copied = false;
    pthread_mutex_lock(&s->mutex);
    if (s->has_frame) {
        memcpy(dst, s->buf[s->front_idx], (size_t)s->w * (size_t)s->h * 3);
        copied = true;
    }
    pthread_mutex_unlock(&s->mutex);
    return copied;
}

void video_emu_set_paused(bool paused) {
    video_emu_ctx_t *s = &g_video_emu;
    if (s->started)
        s->paused = paused;
}

void video_emu_stop(void) {
    video_emu_ctx_t *s = &g_video_emu;

    if (!s->started)
        return;

    s->running = false;
    pthread_join(s->tid, NULL);

    video_emu_free_resources(s);
    pthread_mutex_destroy(&s->mutex);

    s->started = false;
    s->has_frame = false;
}

#endif // EMULATOR_BUILD
