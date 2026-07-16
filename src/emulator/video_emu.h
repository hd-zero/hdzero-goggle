#pragma once
#ifdef EMULATOR_BUILD
#include <stdbool.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
// Start a background decode thread for `path` (a .ts / mp4). Playback loops forever.
// Frames are converted+scaled to w x h, packed RGB24 (3 bytes/pixel, tightly packed, no row padding).
// Returns 0 on success; non-zero on error or if path is NULL/empty (feature then inert).
int  video_emu_start(const char *path, int w, int h);
// Copy the latest decoded frame into dst (caller-owned, must hold w*h*3 bytes). Returns true if a frame was copied.
bool video_emu_get(uint8_t *dst, int w, int h);
// Pause/resume decoding. While paused the decode thread idles (no decode, no CPU),
// mirroring the real receiver being closed when the app leaves the video view.
void video_emu_set_paused(bool paused);
void video_emu_stop(void);
#ifdef __cplusplus
}
#endif
#endif
