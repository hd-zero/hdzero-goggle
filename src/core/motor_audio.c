#include "motor_audio.h"

#include <string.h>

#ifdef MOTOR_AUDIO_POC_HOST
float sinf(float);
float expf(float);
float fabsf(float);
long lrintf(float);
#else
#include <math.h>
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/*
 * More realistic (still lightweight) FPV quad sound:
 *  - blade-pass loading pulses (not pure sines)
 *  - shaft-rate blade asymmetry
 *  - BPF-modulated broadband turbulence / "air"
 *  - weak motor electrical buzz
 *  - per-motor detune so the four props don't phase-lock into a tone
 */

typedef struct {
    float phase;       /* blade-pass phase, radians */
    float shaft_phase; /* one revolution phase */
    float rpm_smooth;
    float rpm_target;
    float detune;      /* static fractional detune */
    float buzz_phase;
    float noise_lp;    /* 1-pole LP state for this motor's air noise */
    float noise_hp;    /* DC block / mild HP */
    uint32_t rng;
} motor_osc_t;

static motor_osc_t g_motors[MOTOR_AUDIO_MOTORS];
static uint32_t g_last_rpm_ms;
static uint8_t g_have_rpm;
static float g_master_gain;
static int16_t g_peak_abs;
static uint32_t g_render_ms;
static uint32_t (*g_time_fn)(void);
static float g_mix_lp_l, g_mix_lp_l2;
static float g_mix_lp_r, g_mix_lp_r2;

/* QUADX: motor index -> left/right gain and front/rear level. */
static const float g_pan_l[MOTOR_AUDIO_MOTORS] = {
    0.22f, /* 1 RR */
    0.28f, /* 2 FR */
    0.95f, /* 3 RL */
    1.00f, /* 4 FL */
};
static const float g_pan_r[MOTOR_AUDIO_MOTORS] = {
    0.95f, /* 1 RR */
    1.00f, /* 2 FR */
    0.22f, /* 3 RL */
    0.28f, /* 4 FL */
};
static const float g_front_rear[MOTOR_AUDIO_MOTORS] = {
    0.68f, /* 1 RR rear quieter */
    1.00f, /* 2 FR */
    0.68f, /* 3 RL rear quieter */
    1.00f, /* 4 FL */
};

static float clampf(float x, float lo, float hi) {
    if (x < lo)
        return lo;
    if (x > hi)
        return hi;
    return x;
}

static float softsat(float x) {
    /* Gentle tanh-ish soft clip without libm tanh */
    float a = fabsf(x);
    float y = a / (1.0f + 0.55f * a);
    return (x < 0.0f) ? -y : y;
}

static uint32_t xorshift32(uint32_t *s) {
    uint32_t x = *s;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *s = x ? x : 0xA5A5A5A5u;
    return *s;
}

static float frand(uint32_t *s) {
    /* approx uniform [-1, 1] */
    return ((float)(xorshift32(s) & 0xFFFFFF) * (2.0f / 16777215.0f)) - 1.0f;
}

void motor_audio_set_time_fn(uint32_t (*fn)(void)) {
    g_time_fn = fn;
}

uint32_t motor_audio_now_ms(void) {
    if (g_time_fn)
        return g_time_fn();
    {
        extern uint32_t time_ms(void);
        return time_ms();
    }
}

uint32_t motor_audio_render_clock_ms(void) {
    return g_render_ms;
}

void motor_audio_reset(void) {
    uint32_t m;
    memset(g_motors, 0, sizeof(g_motors));
    /* Spread initial phases / detune so motors don't start as one oscillator. */
    for (m = 0; m < MOTOR_AUDIO_MOTORS; m++) {
        g_motors[m].phase = (float)m * 0.9f;
        g_motors[m].shaft_phase = (float)m * 1.3f;
        g_motors[m].buzz_phase = (float)m * 2.1f;
        /* +/- ~0.6% detune — enough to break beating into a chord */
        g_motors[m].detune = 1.0f + (0.0035f * (float)((int)m - 1) - 0.0015f);
        g_motors[m].rng = 0x12345678u ^ (m * 0x9E3779B9u);
    }
    g_last_rpm_ms = 0;
    g_have_rpm = 0;
    g_master_gain = 0.0f;
    g_peak_abs = 0;
    g_render_ms = 0;
    g_mix_lp_l = g_mix_lp_l2 = 0.0f;
    g_mix_lp_r = g_mix_lp_r2 = 0.0f;
}

void motor_audio_set_rpm(uint32_t rpm0, uint32_t rpm1, uint32_t rpm2, uint32_t rpm3, uint32_t timestamp_ms) {
    g_motors[0].rpm_target = (float)rpm0;
    g_motors[1].rpm_target = (float)rpm1;
    g_motors[2].rpm_target = (float)rpm2;
    g_motors[3].rpm_target = (float)rpm3;
    g_last_rpm_ms = timestamp_ms;
    g_have_rpm = 1;
}

int16_t motor_audio_peak_abs(void) {
    return g_peak_abs;
}

static float motor_audio_sample_one(motor_osc_t *m, float sample_rate) {
    float rpm;
    float shaft_hz;
    float blade_hz;
    float tip;
    float load;
    float asym;
    float tonal;
    float n;
    float air;
    float buzz;
    float env;
    float s;
    float dt_blade;
    float dt_shaft;

    rpm = m->rpm_smooth;
    if (rpm < 80.0f)
        return 0.0f;

    /* Two-blade prop: shaft = rpm/60, blade-pass = 2 * shaft */
    shaft_hz = (rpm / 60.0f) * m->detune;
    blade_hz = shaft_hz * 2.0f;
    if (blade_hz > sample_rate * 0.42f)
        blade_hz = sample_rate * 0.42f;

    /* Normalized tip-speed proxy for loudness (~0..1 around normal FPV RPM). */
    tip = clampf(rpm / 38000.0f, 0.0f, 1.35f);
    tip = tip * tip; /* closer to acoustic power ~ tip^2..tip^4 */

    /* --- Loading / thickness: sharp blade-pass pulse via harmonic series --- */
    {
        float p = m->phase;
        float h1 = sinf(p);
        float h2 = 0.62f * sinf(p * 2.0f);
        float h3 = 0.38f * sinf(p * 3.0f);
        float h4 = 0.22f * sinf(p * 4.0f);
        float h5 = 0.12f * sinf(p * 5.0f);
        /* Soft pulse peaking once per blade pass (more "prop" than sine). */
        float pulse = 0.55f + 0.45f * h1;
        pulse = pulse * pulse;
        load = (h1 + h2 + h3 + h4 + h5) * pulse;
    }

    /* Blade asymmetry / imbalance at shaft rate (one blade slightly louder). */
    asym = 1.0f + 0.12f * sinf(m->shaft_phase);

    tonal = load * asym * (0.55f + 0.45f * tip);

    /* --- Broadband "air" / vortex noise, amplitude-modulated at BPF --- */
    n = frand(&m->rng);
    /* Mild band-limit: one-pole LP ~3–6 kHz depending on tip speed */
    {
        float lp_a = 0.12f + 0.18f * tip;
        m->noise_lp += (n - m->noise_lp) * lp_a;
        m->noise_hp += (m->noise_lp - m->noise_hp) * 0.02f; /* remove rumble DC */
        air = m->noise_lp - m->noise_hp;
    }
    /* Classic propeller AM of self-noise at blade-pass */
    env = 0.55f + 0.45f * (0.5f + 0.5f * sinf(m->phase));
    air *= env * (0.22f + 0.55f * tip);

    /* --- Weak motor electrical buzz (~ poles/2 electrical cycles; approx) --- */
    {
        float buzz_hz = shaft_hz * 7.0f; /* ~14-pole motor electrical-ish */
        if (buzz_hz > sample_rate * 0.4f)
            buzz_hz = sample_rate * 0.4f;
        buzz = 0.08f * tip * sinf(m->buzz_phase);
        buzz += 0.04f * tip * sinf(m->buzz_phase * 2.0f);
        m->buzz_phase += (float)(2.0 * M_PI) * (buzz_hz / sample_rate);
        if (m->buzz_phase >= (float)(2.0 * M_PI))
            m->buzz_phase -= (float)(2.0 * M_PI);
    }

    s = tonal * 0.78f + air * 0.55f + buzz;

    /* Advance continuous phases */
    dt_blade = (float)(2.0 * M_PI) * (blade_hz / sample_rate);
    dt_shaft = (float)(2.0 * M_PI) * (shaft_hz / sample_rate);
    m->phase += dt_blade;
    if (m->phase >= (float)(2.0 * M_PI))
        m->phase -= (float)(2.0 * M_PI);
    m->shaft_phase += dt_shaft;
    if (m->shaft_phase >= (float)(2.0 * M_PI))
        m->shaft_phase -= (float)(2.0 * M_PI);

    /* Idle is quieter / thinner */
    {
        float idle = clampf((rpm - 1500.0f) / 6000.0f, 0.15f, 1.0f);
        s *= idle;
    }

    return s;
}

void motor_audio_render(int16_t *output, uint32_t frame_count, uint32_t sample_rate, uint32_t timestamp_ms) {
    uint32_t i, m;
    float alpha;
    float gain_target;
    float gain_alpha;
    float left, right;
    int32_t pcm_l, pcm_r;
    uint32_t now_ms;
    float mix_lp_a;

    if (!output || frame_count == 0 || sample_rate == 0)
        return;

    if (timestamp_ms != 0)
        now_ms = timestamp_ms;
    else
        now_ms = g_render_ms;

    alpha = 1.0f - expf(-1.0f / ((float)sample_rate * ((float)MOTOR_AUDIO_SMOOTH_MS / 1000.0f)));
    gain_alpha = 1.0f - expf(-1.0f / ((float)sample_rate * 0.05f));
    /* Cabin / soft distance: ~6–8 kHz two-pole LP */
    mix_lp_a = 1.0f - expf(-2.0f * (float)M_PI * 7000.0f / (float)sample_rate);

    for (i = 0; i < frame_count; i++) {
        uint32_t sample_ms = now_ms + (i * 1000u) / sample_rate;

        gain_target = 0.0f;
        if (g_have_rpm) {
            uint32_t age = sample_ms - g_last_rpm_ms;
            if ((int32_t)age < 0)
                age = 0;
            if (age <= MOTOR_AUDIO_STALE_MS) {
                float any = 0.0f;
                for (m = 0; m < MOTOR_AUDIO_MOTORS; m++)
                    any += g_motors[m].rpm_target;
                if (any > 0.5f)
                    gain_target = 1.0f;
            }
        }

        g_master_gain += (gain_target - g_master_gain) * gain_alpha;

        left = 0.0f;
        right = 0.0f;
        for (m = 0; m < MOTOR_AUDIO_MOTORS; m++) {
            float s;
            g_motors[m].rpm_smooth += (g_motors[m].rpm_target - g_motors[m].rpm_smooth) * alpha;
            s = motor_audio_sample_one(&g_motors[m], (float)sample_rate);
            s *= g_front_rear[m];
            left += s * g_pan_l[m];
            right += s * g_pan_r[m];
        }

        left = softsat(left * (0.26f * g_master_gain));
        right = softsat(right * (0.26f * g_master_gain));

        g_mix_lp_l += (left - g_mix_lp_l) * mix_lp_a;
        g_mix_lp_l2 += (g_mix_lp_l - g_mix_lp_l2) * mix_lp_a;
        g_mix_lp_r += (right - g_mix_lp_r) * mix_lp_a;
        g_mix_lp_r2 += (g_mix_lp_r - g_mix_lp_r2) * mix_lp_a;
        left = g_mix_lp_l2;
        right = g_mix_lp_r2;

        pcm_l = (int32_t)lrintf(left * 28000.0f);
        pcm_r = (int32_t)lrintf(right * 28000.0f);
        if (pcm_l > 32767)
            pcm_l = 32767;
        if (pcm_l < -32768)
            pcm_l = -32768;
        if (pcm_r > 32767)
            pcm_r = 32767;
        if (pcm_r < -32768)
            pcm_r = -32768;
        output[i * 2] = (int16_t)pcm_l;
        output[i * 2 + 1] = (int16_t)pcm_r;

        {
            int16_t al = (int16_t)(pcm_l < 0 ? -pcm_l : pcm_l);
            int16_t ar = (int16_t)(pcm_r < 0 ? -pcm_r : pcm_r);
            if (al > g_peak_abs)
                g_peak_abs = al;
            if (ar > g_peak_abs)
                g_peak_abs = ar;
        }
    }

    g_render_ms = now_ms + (frame_count * 1000u) / sample_rate;
}
