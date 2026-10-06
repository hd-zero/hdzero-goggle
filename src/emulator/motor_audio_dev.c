#include "motor_audio_dev.h"

#include "core/motor_audio.h"
#include "core/motor_rpm_proto.h"
#include "core/msp_displayport.h"
#include "motor_audio_sdl.h"
#include "util/time.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef MOTOR_AUDIO_POC_HOST
float sinf(float);
float cosf(float);
#else
#include <math.h>
#endif

#define SAMPLE_RATE MOTOR_AUDIO_DEFAULT_RATE
#define TELEMETRY_PERIOD_MS 125

typedef struct {
    int32_t time_us;
    int32_t erpm[4];
} bbl_sample_t;

typedef struct {
    FILE *fp;
    uint32_t sample_rate;
    uint32_t data_bytes;
} wav_writer_t;

static int g_drop_every;
static uint32_t g_pkt_count;
static uint32_t g_pkt_dropped;
static uint32_t g_clip_count;

static int wav_open(wav_writer_t *w, const char *path, uint32_t sample_rate) {
    memset(w, 0, sizeof(*w));
    w->fp = fopen(path, "wb");
    if (!w->fp)
        return -1;
    w->sample_rate = sample_rate;
    /* Placeholder header; rewritten on close. */
    uint8_t hdr[44] = {0};
    if (fwrite(hdr, 1, 44, w->fp) != 44)
        return -1;
    return 0;
}

static int wav_write(wav_writer_t *w, const int16_t *samples, uint32_t count) {
    size_t n = fwrite(samples, sizeof(int16_t), count, w->fp);
    if (n != count)
        return -1;
    w->data_bytes += (uint32_t)(n * sizeof(int16_t));
    return 0;
}

static int wav_close(wav_writer_t *w) {
    uint32_t sr = w->sample_rate;
    uint16_t channels = MOTOR_AUDIO_CHANNELS;
    uint16_t block_align = (uint16_t)(channels * 2);
    uint32_t byte_rate = sr * block_align;
    uint32_t data_sz = w->data_bytes;
    uint32_t riff_sz = 36 + data_sz;
    uint8_t hdr[44];

    if (!w->fp)
        return -1;

    memcpy(hdr + 0, "RIFF", 4);
    hdr[4] = (uint8_t)(riff_sz);
    hdr[5] = (uint8_t)(riff_sz >> 8);
    hdr[6] = (uint8_t)(riff_sz >> 16);
    hdr[7] = (uint8_t)(riff_sz >> 24);
    memcpy(hdr + 8, "WAVEfmt ", 8);
    hdr[16] = 16;
    hdr[17] = 0;
    hdr[18] = 0;
    hdr[19] = 0; /* fmt chunk size */
    hdr[20] = 1;
    hdr[21] = 0; /* PCM */
    hdr[22] = (uint8_t)channels;
    hdr[23] = 0;
    hdr[24] = (uint8_t)(sr);
    hdr[25] = (uint8_t)(sr >> 8);
    hdr[26] = (uint8_t)(sr >> 16);
    hdr[27] = (uint8_t)(sr >> 24);
    hdr[28] = (uint8_t)(byte_rate);
    hdr[29] = (uint8_t)(byte_rate >> 8);
    hdr[30] = (uint8_t)(byte_rate >> 16);
    hdr[31] = (uint8_t)(byte_rate >> 24);
    hdr[32] = (uint8_t)block_align;
    hdr[33] = 0;
    hdr[34] = 16;
    hdr[35] = 0; /* bits */
    memcpy(hdr + 36, "data", 4);
    hdr[40] = (uint8_t)(data_sz);
    hdr[41] = (uint8_t)(data_sz >> 8);
    hdr[42] = (uint8_t)(data_sz >> 16);
    hdr[43] = (uint8_t)(data_sz >> 24);

    if (fseek(w->fp, 0, SEEK_SET) != 0)
        return -1;
    if (fwrite(hdr, 1, 44, w->fp) != 44)
        return -1;
    fclose(w->fp);
    w->fp = NULL;
    return 0;
}

/* Build a complete HDZero 0xFF service frame with optional 6-byte RPM tail. */
static uint8_t build_service_frame(uint8_t *frame, const uint32_t rpm[4], int with_rpm, uint8_t lq) {
    uint8_t payload_len = with_rpm ? MOTOR_RPM_SERVICE_LEN : MOTOR_RPM_SERVICE_LEGACY;
    uint8_t i;
    uint8_t crc0 = 0, crc1 = 0;
    uint8_t len;

    frame[0] = HEADER0;
    frame[1] = HEADER1;
    frame[2] = 0xff;
    frame[3] = payload_len;
    frame[4] = 0x99; /* 720P60 */
    frame[5] = 'B';
    frame[6] = 'T';
    frame[7] = 'F';
    frame[8] = 'L';
    frame[9] = lq;
    frame[10] = 0;
    frame[11] = 0;
    frame[12] = 0;
    frame[13] = 0;
    frame[14] = 0x03;
    frame[15] = 0x55;
    frame[16] = 1;
    frame[17] = 0;
    frame[18] = 0;

    if (with_rpm) {
        uint16_t q[4];
        for (i = 0; i < 4; i++) {
            q[i] = motor_rpm_quantize(rpm[i]);
            if (rpm[i] > MOTOR_RPM_MAX_REPR)
                g_clip_count++;
        }
        motor_rpm_pack(q, &frame[19]);
    }

    len = (uint8_t)(4 + payload_len); /* header+index+len + payload */
    for (i = 0; i < len; i++) {
        crc0 ^= frame[i];
        crc1 = crc8tab[crc1 ^ frame[i]];
    }
    frame[len] = crc0;
    frame[len + 1] = crc1;
    return (uint8_t)(len + 2);
}

static void feed_service_rpm(const uint32_t rpm[4], int with_rpm, uint8_t lq) {
    uint8_t frame[64];
    uint8_t n;

    g_pkt_count++;
    if (g_drop_every > 0 && (g_pkt_count % (uint32_t)g_drop_every) == 0) {
        g_pkt_dropped++;
        return;
    }

    n = build_service_frame(frame, rpm, with_rpm, lq);
    recive_one_frame(frame, n);
}

static int write_pcm_seconds(wav_writer_t *wav, float seconds, int use_sdl_realtime) {
    uint32_t total = (uint32_t)(seconds * SAMPLE_RATE);
    uint32_t chunk = 1024;
    int16_t buf[1024 * MOTOR_AUDIO_CHANNELS];
    uint32_t done = 0;

    while (done < total) {
        uint32_t n = total - done;
        if (n > chunk)
            n = chunk;
        motor_audio_render(buf, n, SAMPLE_RATE, 0);
        if (wav && wav->fp) {
            if (wav_write(wav, buf, n * MOTOR_AUDIO_CHANNELS) != 0)
                return -1;
        }
        if (use_sdl_realtime) {
            /* SDL callback also renders; for realtime replay we sleep wall time. */
            usleep((useconds_t)((n * 1000000ull) / SAMPLE_RATE));
        }
        done += n;
    }
    return 0;
}

/* ---------------- unit tests ---------------- */

static int expect_true(int cond, const char *msg) {
    if (!cond) {
        fprintf(stderr, "FAIL: %s\n", msg);
        return 1;
    }
    return 0;
}

static int test_roundtrip_rpm(uint32_t rpm, uint32_t max_err) {
    uint16_t q = motor_rpm_quantize(rpm);
    uint32_t back = motor_rpm_dequantize(q);
    uint32_t err = (back > rpm) ? (back - rpm) : (rpm - back);
    if (rpm > MOTOR_RPM_MAX_REPR) {
        if (q != MOTOR_RPM_Q_MAX || back != MOTOR_RPM_MAX_REPR)
            return 1;
        return 0;
    }
    return err > max_err;
}

static int run_unit_tests(void) {
    int fails = 0;
    uint16_t q[4], q2[4];
    uint8_t packed[6];
    uint32_t rpm[4];
    uint8_t msp[53];
    uint8_t frame[64];
    uint8_t n;
    int16_t *pcm;
    int i;

    pcm = (int16_t *)malloc(SAMPLE_RATE * MOTOR_AUDIO_CHANNELS * sizeof(int16_t));
    if (!pcm) {
        fprintf(stderr, "oom\n");
        return 1;
    }
    fails += expect_true(test_roundtrip_rpm(0, 0) == 0, "RPM 0 round-trip");
    fails += expect_true(test_roundtrip_rpm(3200, 16) == 0, "RPM 3200 within 16");
    fails += expect_true(test_roundtrip_rpm(25000, 16) == 0, "RPM 25000 within 16");
    fails += expect_true(test_roundtrip_rpm(45000, 16) == 0, "RPM 45000 within 16");
    fails += expect_true(test_roundtrip_rpm(MOTOR_RPM_MAX_REPR, 0) == 0, "max 131040 round-trip");
    fails += expect_true(test_roundtrip_rpm(200000, 0) == 0, "above max clamps");
    q[0] = 0;
    q[1] = 100;
    q[2] = 2048;
    q[3] = 4095;
    motor_rpm_pack(q, packed);
    motor_rpm_unpack(packed, q2);
    fails += expect_true(q[0] == q2[0] && q[1] == q2[1] && q[2] == q2[2] && q[3] == q2[3],
                         "12-bit pack/unpack exact");

    /* Golden vector: q = {1, 16, 256, 4095} */
    q[0] = 1;
    q[1] = 16;
    q[2] = 256;
    q[3] = 4095;
    motor_rpm_pack(q, packed);
    fails += expect_true(packed[0] == 0x01 && packed[1] == 0x00 && packed[2] == 0x01 &&
                             packed[3] == 0x00 && packed[4] == 0xf1 && packed[5] == 0xff,
                         "golden six-byte vector");
    motor_rpm_unpack(packed, q2);
    fails += expect_true(q2[0] == 1 && q2[1] == 16 && q2[2] == 256 && q2[3] == 4095,
                         "golden unpack");
    memset(msp, 0, sizeof(msp));
    msp[0] = 4;
    for (i = 0; i < 4; i++) {
        uint32_t r = 1000u * (uint32_t)(i + 1);
        uint8_t *p = &msp[1 + i * 13];
        p[0] = (uint8_t)(r);
        p[1] = (uint8_t)(r >> 8);
        p[2] = (uint8_t)(r >> 16);
        p[3] = (uint8_t)(r >> 24);
    }
    fails += expect_true(motor_rpm_from_msp_telemetry(msp, 53, rpm) == 1, "msp extract ok");
    fails += expect_true(rpm[0] == 1000 && rpm[1] == 2000 && rpm[2] == 3000 && rpm[3] == 4000,
                         "msp rpm values");
    fails += expect_true(motor_rpm_from_msp_telemetry(msp, 40, rpm) == 0, "short msp ignored");
    msp[0] = 3;
    fails += expect_true(motor_rpm_from_msp_telemetry(msp, 53, rpm) == 0, "motor count < 4 ignored");
    motor_audio_reset();
    rpm[0] = rpm[1] = rpm[2] = rpm[3] = 20000;
    n = build_service_frame(frame, rpm, 0, 0);
    recive_one_frame(frame, n);
    motor_audio_render(pcm, 4800, SAMPLE_RATE, 1000);
    {
        int silent = 1;
        for (i = 0; i < 4800 * MOTOR_AUDIO_CHANNELS; i++) {
            if (pcm[i] != 0) {
                silent = 0;
                break;
            }
        }
        fails += expect_true(silent, "old 15-byte service keeps audio silent");
    }
    motor_audio_reset();
    n = build_service_frame(frame, rpm, 1, 1);
    recive_one_frame(frame, n);
    /* Advance internal clock past smoothing; feed timestamp via render */
    motor_audio_render(pcm, 4800, SAMPLE_RATE, time_ms() ? time_ms() : 100);
    {
        int any = 0;
        for (i = 0; i < 4800 * MOTOR_AUDIO_CHANNELS; i++) {
            if (pcm[i] != 0) {
                any = 1;
                break;
            }
        }
        fails += expect_true(any, "21-byte service enables audio");
    }

    /* Stale telemetry fades to silence */
    motor_audio_reset();
    motor_audio_set_rpm(30000, 30000, 30000, 30000, 0);
    motor_audio_render(pcm, SAMPLE_RATE / 10, SAMPLE_RATE, 0); /* 100ms audible */
    motor_audio_render(pcm, SAMPLE_RATE, SAMPLE_RATE, 600); /* stale > 500ms */
    {
        int end_silent = 1;
        for (i = ((int)SAMPLE_RATE - 1000) * MOTOR_AUDIO_CHANNELS; i < (int)SAMPLE_RATE * MOTOR_AUDIO_CHANNELS; i++) {
            if (pcm[i] != 0) {
                end_silent = 0;
                break;
            }
        }
        fails += expect_true(end_silent, "stale telemetry fades to zero");
    }

    /* Zero RPM fades to silence */
    motor_audio_reset();
    motor_audio_set_rpm(25000, 25000, 25000, 25000, 0);
    motor_audio_render(pcm, SAMPLE_RATE / 5, SAMPLE_RATE, 0);
    motor_audio_set_rpm(0, 0, 0, 0, 200);
    motor_audio_render(pcm, SAMPLE_RATE, SAMPLE_RATE, 200);
    {
        int end_silent = 1;
        for (i = ((int)SAMPLE_RATE - 2000) * MOTOR_AUDIO_CHANNELS; i < (int)SAMPLE_RATE * MOTOR_AUDIO_CHANNELS; i++) {
            if (pcm[i] < -50 || pcm[i] > 50) {
                end_silent = 0;
                break;
            }
        }
        fails += expect_true(end_silent, "zero RPM fades to silence");
    }
    fails += expect_true(motor_audio_peak_abs() <= 32767, "samples within int16 range");

    free(pcm);

    if (fails == 0)
        printf("motor_audio unit tests: PASS\n");
    else
        printf("motor_audio unit tests: %d FAIL\n", fails);
    return fails ? 1 : 0;
}

/* ---------------- synthetic timeline ---------------- */

static void synthetic_rpm_at(float t_sec, uint32_t rpm[4]) {
    float thr;
    float base;

    if (t_sec < 1.0f) {
        /* idle */
        base = 3000.0f;
    } else if (t_sec < 4.0f) {
        /* gradual throttle increase */
        thr = (t_sec - 1.0f) / 3.0f;
        base = 3000.0f + thr * 22000.0f;
    } else if (t_sec < 7.0f) {
        /* roll/yaw maneuver: diverge motors */
        thr = (t_sec - 4.0f) / 3.0f;
        base = 25000.0f;
        rpm[0] = (uint32_t)(base + 8000.0f * sinf(thr * 6.0f));
        rpm[1] = (uint32_t)(base - 6000.0f * sinf(thr * 5.0f));
        rpm[2] = (uint32_t)(base + 5000.0f * cosf(thr * 4.0f));
        rpm[3] = (uint32_t)(base - 7000.0f * cosf(thr * 7.0f));
        return;
    } else if (t_sec < 9.0f) {
        base = 40000.0f; /* full throttle */
    } else if (t_sec < 12.0f) {
        thr = (t_sec - 9.0f) / 3.0f;
        base = 40000.0f - thr * 35000.0f;
    } else {
        base = 0.0f; /* disarm */
    }

    rpm[0] = rpm[1] = rpm[2] = rpm[3] = (uint32_t)base;
}

static int run_synthetic(const char *wav_path, int realtime) {
    wav_writer_t wav;
    float t;
    uint32_t rpm[4];
    uint8_t lq = 0;
    int use_wav = wav_path && wav_path[0];

    g_pkt_count = 0;
    g_pkt_dropped = 0;
    g_clip_count = 0;
    motor_audio_reset();
    /* Offline WAV must share one clock: parser RPM stamps vs PCM render timeline. */
    motor_audio_set_time_fn(motor_audio_render_clock_ms);

    if (use_wav && wav_open(&wav, wav_path, SAMPLE_RATE) != 0) {
        fprintf(stderr, "cannot open wav %s\n", wav_path);
        return 1;
    }

    /* Realtime SDL playback only when not writing WAV (avoid double-render). */
    if (realtime && !use_wav) {
        if (motor_audio_sdl_start(SAMPLE_RATE) != 0)
            fprintf(stderr, "warning: SDL audio unavailable\n");
    }

    printf("synthetic flight: idle -> throttle -> maneuver -> full -> down -> disarm\n");

    for (t = 0.0f; t <= 14.0f; t += TELEMETRY_PERIOD_MS / 1000.0f) {
        synthetic_rpm_at(t, rpm);
        feed_service_rpm(rpm, 1, lq++);
        if (use_wav) {
            if (write_pcm_seconds(&wav, TELEMETRY_PERIOD_MS / 1000.0f, 0) != 0)
                return 1;
            if (realtime)
                usleep(TELEMETRY_PERIOD_MS * 1000);
        } else if (realtime) {
            usleep(TELEMETRY_PERIOD_MS * 1000);
        } else {
            int16_t buf[(SAMPLE_RATE / 8) * MOTOR_AUDIO_CHANNELS];
            motor_audio_render(buf, SAMPLE_RATE / 8, SAMPLE_RATE, 0);
        }
    }

    if (realtime)
        motor_audio_sdl_stop();
    if (use_wav) {
        wav_close(&wav);
        printf("wrote %s (%u packets, %u clipped)\n", wav_path, g_pkt_count, g_clip_count);
    }
    motor_audio_set_time_fn(NULL);
    return 0;
}

/* ---------------- blackbox CSV replay ---------------- */

static char *trim(char *s) {
    char *e;
    while (*s == ' ' || *s == '\t' || *s == '"')
        s++;
    e = s + strlen(s);
    while (e > s && (e[-1] == '\n' || e[-1] == '\r' || e[-1] == ' ' || e[-1] == '\t' || e[-1] == '"'))
        *--e = 0;
    return s;
}

static int header_col(char **cols, int n, const char *name) {
    int i;
    for (i = 0; i < n; i++) {
        if (strcmp(cols[i], name) == 0)
            return i;
    }
    return -1;
}

/*
 * Match eRPM[0] / eRPM(/100)[0] headers from blackbox_decode.
 * *out_div100 is set when the matched column is already divided by 100.
 */
static int header_col_erpm(char **cols, int n, int motor, int *out_div100) {
    char exact[32];
    char alt[40];
    int i;

    snprintf(exact, sizeof(exact), "eRPM[%d]", motor);
    snprintf(alt, sizeof(alt), "eRPM(/100)[%d]", motor);
    i = header_col(cols, n, exact);
    if (i >= 0) {
        if (out_div100)
            *out_div100 = 0;
        return i;
    }
    i = header_col(cols, n, alt);
    if (i >= 0) {
        if (out_div100)
            *out_div100 = 1;
        return i;
    }
    for (i = 0; i < n; i++) {
        const char *p = strstr(cols[i], "eRPM");
        char bracket[8];
        if (!p)
            continue;
        snprintf(bracket, sizeof(bracket), "[%d]", motor);
        if (!strstr(p, bracket))
            continue;
        if (out_div100)
            *out_div100 = (strstr(cols[i], "/100") != NULL);
        return i;
    }
    return -1;
}

/* mechanical RPM = eRPM * 2 / poles; blackbox may store eRPM/100. */
static uint32_t erpm_to_mech(int32_t erpm_field, int motor_poles, int div100) {
    int64_t erpm = div100 ? ((int64_t)erpm_field * 100) : (int64_t)erpm_field;
    if (erpm < 0)
        erpm = 0;
    return (uint32_t)(erpm * 2 / motor_poles);
}

static int split_csv_line(char *line, char **cols, int max_cols) {
    int n = 0;
    char *p = line;
    while (n < max_cols) {
        cols[n++] = p;
        p = strchr(p, ',');
        if (!p)
            break;
        *p++ = 0;
    }
    return n;
}

static int run_blackbox_replay(const char *csv_path, int motor_poles, const char *wav_path, int realtime) {
    FILE *fp;
    char line[4096];
    char *cols[256];
    int ncols;
    int idx_time = -1;
    int idx_erpm[4] = {-1, -1, -1, -1};
    int erpm_div100 = 0;
    bbl_sample_t *samples = NULL;
    size_t sample_count = 0, sample_cap = 0;
    size_t i;
    int32_t t0 = 0;
    uint32_t rpm_min[4], rpm_max[4];
    wav_writer_t wav;
    int use_wav = wav_path && wav_path[0];
    uint8_t lq = 0;
    int64_t next_tx_us;
    uint32_t latest[4] = {0, 0, 0, 0};
    size_t si = 0;

    if (motor_poles < 2) {
        fprintf(stderr, "error: --motor-poles N is required (N >= 2)\n");
        return 1;
    }

    fp = fopen(csv_path, "r");
    if (!fp) {
        fprintf(stderr, "error: cannot open %s\n", csv_path);
        return 1;
    }

    if (!fgets(line, sizeof(line), fp)) {
        fprintf(stderr, "error: empty CSV\n");
        fclose(fp);
        return 1;
    }

    ncols = split_csv_line(line, cols, 256);
    for (i = 0; i < (size_t)ncols; i++)
        cols[i] = trim(cols[i]);

    idx_time = header_col(cols, ncols, "time");
    if (idx_time < 0)
        idx_time = header_col(cols, ncols, "time (us)");
    idx_erpm[0] = header_col_erpm(cols, ncols, 0, &erpm_div100);
    idx_erpm[1] = header_col_erpm(cols, ncols, 1, NULL);
    idx_erpm[2] = header_col_erpm(cols, ncols, 2, NULL);
    idx_erpm[3] = header_col_erpm(cols, ncols, 3, NULL);

    if (idx_time < 0 || idx_erpm[0] < 0 || idx_erpm[1] < 0 || idx_erpm[2] < 0 || idx_erpm[3] < 0) {
        fprintf(stderr,
                "error: CSV missing required fields (need time/time (us) and eRPM[0..3] or eRPM(/100)[0..3]).\n"
                "This log was recorded without RPM fields and cannot reproduce actual motor RPM.\n");
        fclose(fp);
        return 1;
    }

    while (fgets(line, sizeof(line), fp)) {
        char *c[256];
        int nc = split_csv_line(line, c, 256);
        bbl_sample_t s;
        if (nc <= idx_erpm[3])
            continue;
        s.time_us = (int32_t)atoi(trim(c[idx_time]));
        s.erpm[0] = atoi(trim(c[idx_erpm[0]]));
        s.erpm[1] = atoi(trim(c[idx_erpm[1]]));
        s.erpm[2] = atoi(trim(c[idx_erpm[2]]));
        s.erpm[3] = atoi(trim(c[idx_erpm[3]]));
        if (sample_count == sample_cap) {
            sample_cap = sample_cap ? sample_cap * 2 : 4096;
            samples = realloc(samples, sample_cap * sizeof(*samples));
            if (!samples) {
                fprintf(stderr, "oom\n");
                fclose(fp);
                return 1;
            }
        }
        samples[sample_count++] = s;
    }
    fclose(fp);

    if (sample_count == 0) {
        fprintf(stderr, "error: no CSV rows\n");
        return 1;
    }

    t0 = samples[0].time_us;
    for (i = 0; i < 4; i++) {
        rpm_min[i] = UINT32_MAX;
        rpm_max[i] = 0;
    }
    for (i = 0; i < sample_count; i++) {
        int m;
        for (m = 0; m < 4; m++) {
            uint32_t mech = erpm_to_mech(samples[i].erpm[m], motor_poles, erpm_div100);
            if (mech < rpm_min[m])
                rpm_min[m] = mech;
            if (mech > rpm_max[m])
                rpm_max[m] = mech;
        }
    }

    {
        double dur = (samples[sample_count - 1].time_us - t0) / 1e6;
        uint32_t est_pkts = (uint32_t)(dur / 0.125) + 1;
        printf("blackbox replay diagnostics:\n");
        printf("  motor poles: %d\n", motor_poles);
        printf("  eRPM column scale: %s\n", erpm_div100 ? "eRPM/100" : "eRPM");
        printf("  flight duration: %.2f s\n", dur);
        printf("  samples: %zu\n", sample_count);
        for (i = 0; i < 4; i++)
            printf("  motor%zu mechanical RPM min/max: %u / %u\n", i, rpm_min[i], rpm_max[i]);
        printf("  estimated 8 Hz packets: %u\n", est_pkts);
    }

    g_pkt_count = 0;
    g_pkt_dropped = 0;
    g_clip_count = 0;
    motor_audio_reset();
    motor_audio_set_time_fn(motor_audio_render_clock_ms);

    if (use_wav && wav_open(&wav, wav_path, SAMPLE_RATE) != 0) {
        fprintf(stderr, "cannot open wav %s\n", wav_path);
        free(samples);
        return 1;
    }
    if (realtime && !use_wav) {
        if (motor_audio_sdl_start(SAMPLE_RATE) != 0)
            fprintf(stderr, "warning: SDL audio unavailable\n");
    }

    next_tx_us = samples[0].time_us;
    while (si < sample_count || next_tx_us <= samples[sample_count - 1].time_us) {
        while (si < sample_count && samples[si].time_us <= next_tx_us) {
            int m;
            for (m = 0; m < 4; m++)
                latest[m] = erpm_to_mech(samples[si].erpm[m], motor_poles, erpm_div100);
            si++;
        }
        feed_service_rpm(latest, 1, lq++);
        if (use_wav) {
            if (write_pcm_seconds(&wav, TELEMETRY_PERIOD_MS / 1000.0f, 0) != 0) {
                free(samples);
                return 1;
            }
            if (realtime)
                usleep(TELEMETRY_PERIOD_MS * 1000);
        } else if (realtime) {
            usleep(TELEMETRY_PERIOD_MS * 1000);
        } else {
            int16_t buf[(SAMPLE_RATE / 8) * MOTOR_AUDIO_CHANNELS];
            motor_audio_render(buf, SAMPLE_RATE / 8, SAMPLE_RATE, 0);
        }
        next_tx_us += TELEMETRY_PERIOD_MS * 1000;
        if (si >= sample_count && next_tx_us > samples[sample_count - 1].time_us + TELEMETRY_PERIOD_MS * 1000)
            break;
    }

    printf("  generated 8 Hz packets: %u (dropped %u)\n", g_pkt_count, g_pkt_dropped);
    printf("  values clipped at 131040 RPM: %u\n", g_clip_count);

    if (realtime)
        motor_audio_sdl_stop();
    if (use_wav) {
        wav_close(&wav);
        printf("wrote %s\n", wav_path);
    }
    motor_audio_set_time_fn(NULL);
    free(samples);
    return 0;
}

static void print_usage(const char *argv0) {
    printf("Motor audio PoC (emulator/dev):\n");
    printf("  %s --motor-test\n", argv0);
    printf("  %s --motor-synthetic [--motor-wav out.wav] [--motor-realtime]\n", argv0);
    printf("  %s --motor-replay <csv> --motor-poles <n> [--motor-wav out.wav]\n", argv0);
    printf("                 [--motor-realtime] [--motor-drop-every N]\n");
}

int motor_audio_dev_main(int argc, char **argv) {
    const char *replay = NULL;
    const char *wav = NULL;
    int poles = 0;
    int do_test = 0;
    int do_synth = 0;
    int realtime = 0;
    int i;

    if (argc < 2)
        return 0;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--motor-test") == 0) {
            do_test = 1;
        } else if (strcmp(argv[i], "--motor-synthetic") == 0) {
            do_synth = 1;
        } else if (strcmp(argv[i], "--motor-replay") == 0 && i + 1 < argc) {
            replay = argv[++i];
        } else if (strcmp(argv[i], "--motor-poles") == 0 && i + 1 < argc) {
            poles = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--motor-wav") == 0 && i + 1 < argc) {
            wav = argv[++i];
        } else if (strcmp(argv[i], "--motor-realtime") == 0) {
            realtime = 1;
        } else if (strcmp(argv[i], "--motor-drop-every") == 0 && i + 1 < argc) {
            g_drop_every = atoi(argv[++i]);
        } else if (strcmp(argv[i], "--motor-help") == 0) {
            print_usage(argv[0]);
            return 1;
        } else if (strncmp(argv[i], "--motor-", 8) == 0) {
            fprintf(stderr, "unknown motor option: %s\n", argv[i]);
            print_usage(argv[0]);
            return 1;
        } else {
            /* not a motor-audio option; let normal emulator run */
            return 0;
        }
    }

    if (!do_test && !do_synth && !replay)
        return 0;

    if (do_test) {
        int rc = run_unit_tests();
        if (rc)
            exit(rc);
    }
    if (do_synth) {
        const char *out = wav ? wav : "tmp/motor_synthetic.wav";
        int rc = run_synthetic(out, realtime);
        if (rc)
            exit(rc);
    }
    if (replay) {
        int rc = run_blackbox_replay(replay, poles, wav, realtime);
        exit(rc);
    }

    exit(0);
}
