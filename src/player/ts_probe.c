#include "ts_probe.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TS_PKT_SIZE     188
#define TS_SYNC_BYTE    0x47
#define PROBE_MAX_BYTES (8 * 1024 * 1024) // plenty for >64 video PES at any bitrate
#define PROBE_MAX_PTS   64
#define PROBE_MAX_PIDS  8

typedef struct {
    uint16_t pid;
    int count;
    uint64_t pts[PROBE_MAX_PTS];
} pid_pts_t;

static int cmp_u64(const void *a, const void *b) {
    uint64_t x = *(const uint64_t *)a, y = *(const uint64_t *)b;
    return (x > y) - (x < y);
}

int ts_probe_fps_x1000(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) {
        return 0;
    }

    pid_pts_t pids[PROBE_MAX_PIDS];
    int npids = 0;
    memset(pids, 0, sizeof(pids));

    uint8_t pkt[TS_PKT_SIZE];
    long consumed = 0;
    bool full = false;

    while (!full && consumed < PROBE_MAX_BYTES && fread(pkt, 1, TS_PKT_SIZE, f) == TS_PKT_SIZE) {
        consumed += TS_PKT_SIZE;

        if (pkt[0] != TS_SYNC_BYTE) {
            // resync: scan forward one byte at a time
            fseek(f, 1 - TS_PKT_SIZE, SEEK_CUR);
            continue;
        }

        bool const pusi = pkt[1] & 0x40;
        if (!pusi) {
            continue;
        }
        uint16_t const pid = ((pkt[1] & 0x1F) << 8) | pkt[2];
        uint8_t const afc = (pkt[3] >> 4) & 0x3;
        int off = 4;
        if (afc & 0x2) { // adaptation field present
            off += 1 + pkt[4];
        }
        if (!(afc & 0x1) || off + 14 > TS_PKT_SIZE) {
            continue;
        }

        // PES start: 00 00 01 <stream_id>, video stream ids are 0xE0-0xEF
        const uint8_t *pes = pkt + off;
        if (pes[0] != 0x00 || pes[1] != 0x00 || pes[2] != 0x01 || (pes[3] & 0xF0) != 0xE0) {
            continue;
        }
        if ((pes[7] & 0x80) == 0) { // no PTS
            continue;
        }
        uint64_t const pts = ((uint64_t)((pes[9] >> 1) & 0x07) << 30) |
                             ((uint64_t)pes[10] << 22) |
                             ((uint64_t)(pes[11] >> 1) << 15) |
                             ((uint64_t)pes[12] << 7) |
                             ((uint64_t)pes[13] >> 1);

        pid_pts_t *slot = NULL;
        for (int i = 0; i < npids; ++i) {
            if (pids[i].pid == pid) {
                slot = &pids[i];
                break;
            }
        }
        if (!slot && npids < PROBE_MAX_PIDS) {
            slot = &pids[npids++];
            slot->pid = pid;
        }
        if (slot && slot->count < PROBE_MAX_PTS) {
            slot->pts[slot->count++] = pts;
            if (slot->count == PROBE_MAX_PTS) {
                full = true;
            }
        }
    }
    fclose(f);

    // The video stream is the PID with the most PES starts
    pid_pts_t *best = NULL;
    for (int i = 0; i < npids; ++i) {
        if (!best || pids[i].count > best->count) {
            best = &pids[i];
        }
    }
    if (!best || best->count < 8) {
        return 0;
    }

    // PES packets arrive in decode order; with B-frames the PTS jumps
    // around, so sort into presentation order first, then take the median
    // of successive deltas (90kHz ticks) - robust against discontinuities.
    qsort(best->pts, best->count, sizeof(best->pts[0]), cmp_u64);
    uint64_t deltas[PROBE_MAX_PTS];
    int ndeltas = 0;
    for (int i = 1; i < best->count; ++i) {
        uint64_t const d = best->pts[i] - best->pts[i - 1];
        if (d > 0 && d < 90000) { // ignore duplicates and >1s jumps
            deltas[ndeltas++] = d;
        }
    }
    if (ndeltas < 4) {
        return 0;
    }
    qsort(deltas, ndeltas, sizeof(deltas[0]), cmp_u64);
    uint64_t const med = deltas[ndeltas / 2];

    int const fps_x1000 = (int)((90000ULL * 1000 + med / 2) / med);
    if (fps_x1000 < 10000 || fps_x1000 > 200000) { // sane range 10..200 fps
        return 0;
    }
    return fps_x1000;
}
