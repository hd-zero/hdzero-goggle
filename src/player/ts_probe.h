#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Derive the video frame rate of an MPEG-TS file from the PTS spacing of
 * its video PES packets. The platform demuxer reports 0 fps for TS files
 * whose stream carries no timing info, which silently disables the DVR
 * playback display retiming - this probe is the fallback.
 *
 * Returns fps * 1000 (e.g. 90000 for 90fps), or 0 if it cannot be
 * determined.
 */
int ts_probe_fps_x1000(const char *path);

#ifdef __cplusplus
}
#endif
