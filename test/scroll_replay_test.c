// Offline replay of SCROLL_RAW waveforms through candidate scroll policies.
//
// Build:
//   gcc -std=gnu11 -Wall -Wextra -O2 -I../src/core scroll_replay_test.c
//       ../src/core/scroll_filter.c -o scroll_replay_test -lm
//   ./scroll_replay_test /path/to/HDZGOGGLE-diag.log
//
// Phase 1: compare policies only. Does not wire production burst gates.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "scroll_burst.h"
#include "scroll_filter.h"

#define MAX_RAW 8192
#define MAX_OUT 4096
#define DIAL_SENSITIVITY 1

typedef struct {
    long long t_us;
    int sign; // +1 or -1
} raw_event_t;

typedef struct {
    long long t_us;
    int sign;
} pulse_t;

typedef struct {
    const char *name;
    int filter_inputs;
    int logical_outputs;
    int same_episode_multi; // episodes that produced >1 logical
    int direction_changes;  // logical direction flips
    double first_latency_ms_sum;
    int first_latency_count;
    int lead_vs_majority_disagree;
    int episodes;
    int reversal_confirm; // 0 = compiled default; 1/2 for A/B
} metrics_t;

static raw_event_t g_raw[MAX_RAW];
static int g_raw_n;

static struct timeval tv_from_us(long long t_us) {
    struct timeval t;
    t.tv_sec = (time_t)(t_us / 1000000LL);
    t.tv_usec = (suseconds_t)(t_us % 1000000LL);
    return t;
}

static int load_scroll_raw(const char *path) {
    FILE *fp = fopen(path, "r");
    if (!fp) {
        perror(path);
        return -1;
    }

    char line[1024];
    g_raw_n = 0;
    while (fgets(line, sizeof(line), fp)) {
        const char *p = strstr(line, "SCROLL_RAW ");
        if (!p)
            continue;

        long sec = 0, usec = 0;
        int sign = 0;
        // ... ts_ev=%ld.%06ld ... sign=%d
        if (sscanf(p, "SCROLL_RAW %*s %*s ts_ev=%ld.%06ld %*s sign=%d", &sec, &usec, &sign) != 3) {
            // tolerate alternate token order: eventN= fd= ts_ev= val= sign=
            if (sscanf(p, "SCROLL_RAW eventN=%*d fd=%*d ts_ev=%ld.%06ld val=%*d sign=%d",
                       &sec, &usec, &sign) != 3)
                continue;
        }
        if (sign != 1 && sign != -1)
            continue;
        if (g_raw_n >= MAX_RAW) {
            fprintf(stderr, "raw buffer full\n");
            break;
        }
        g_raw[g_raw_n].t_us = (long long)sec * 1000000LL + (long long)usec;
        g_raw[g_raw_n].sign = sign;
        g_raw_n++;
    }
    fclose(fp);
    return g_raw_n;
}

static void run_filter_pulses(const pulse_t *pulses, int n, metrics_t *m,
                              const int *episode_of_pulse, const long long *ep_first_raw_us) {
    scroll_filter_t f;
    scroll_filter_init(&f);
    f.reversal_confirm = m->reversal_confirm;

    int up_acc = 0, down_acc = 0;
    int last_logical_sign = 0;
    int *logical_per_ep = calloc((size_t)m->episodes + 1, sizeof(int));
    int *ep_has_logical = calloc((size_t)m->episodes + 1, sizeof(int));
    if (!logical_per_ep || !ep_has_logical) {
        free(logical_per_ep);
        free(ep_has_logical);
        return;
    }

    m->filter_inputs = n;
    m->logical_outputs = 0;
    m->direction_changes = 0;
    m->same_episode_multi = 0;
    m->first_latency_ms_sum = 0;
    m->first_latency_count = 0;

    for (int i = 0; i < n; i++) {
        int ep = episode_of_pulse[i];

        struct timeval t = tv_from_us(pulses[i].t_us);
        scroll_filter_event_t out = scroll_filter_step(&f, &t, pulses[i].sign);
        int filtered = 0;
        if (out == SCROLL_FILTER_UP)
            filtered = 1;
        else if (out == SCROLL_FILTER_DOWN)
            filtered = -1;

        if (filtered == 1) {
            up_acc++;
            down_acc = 0;
        } else if (filtered == -1) {
            down_acc++;
            up_acc = 0;
        } else {
            continue;
        }

        if (up_acc == DIAL_SENSITIVITY || down_acc == DIAL_SENSITIVITY) {
            int logical_sign = (up_acc == DIAL_SENSITIVITY) ? 1 : -1;
            up_acc = 0;
            down_acc = 0;
            m->logical_outputs++;
            if (last_logical_sign != 0 && last_logical_sign != logical_sign)
                m->direction_changes++;
            last_logical_sign = logical_sign;

            if (ep >= 0 && ep < m->episodes) {
                logical_per_ep[ep]++;
                if (!ep_has_logical[ep]) {
                    ep_has_logical[ep] = 1;
                    double lat = (double)(pulses[i].t_us - ep_first_raw_us[ep]) / 1000.0;
                    if (lat < 0)
                        lat = 0;
                    m->first_latency_ms_sum += lat;
                    m->first_latency_count++;
                }
            }
        }
    }

    for (int e = 0; e < m->episodes; e++) {
        if (logical_per_ep[e] > 1)
            m->same_episode_multi++;
    }

    free(logical_per_ep);
    free(ep_has_logical);
}

static void fill_ep_first_raw(const int *ep_of_raw, int ep_count, long long *ep_first_raw_us) {
    for (int e = 0; e < ep_count; e++)
        ep_first_raw_us[e] = -1;
    for (int i = 0; i < g_raw_n; i++) {
        int ep = ep_of_raw[i];
        if (ep_first_raw_us[ep] < 0)
            ep_first_raw_us[ep] = g_raw[i].t_us;
    }
}

// Assign each raw index to an episode id using quiet_ms gap on ALL raws.
static int assign_episodes(long long quiet_us, int *ep_of_raw, int *ep_count_out) {
    int ep = -1;
    int count = 0;
    long long last_t = 0;
    int have = 0;
    for (int i = 0; i < g_raw_n; i++) {
        if (!have || g_raw[i].t_us - last_t > quiet_us) {
            ep = count++;
        }
        ep_of_raw[i] = ep;
        last_t = g_raw[i].t_us;
        have = 1;
    }
    *ep_count_out = count;
    return 0;
}

static int episode_majority(int ep, const int *ep_of_raw, int *lead_sign_out) {
    int sum = 0;
    int lead = 0;
    int have_lead = 0;
    for (int i = 0; i < g_raw_n; i++) {
        if (ep_of_raw[i] != ep)
            continue;
        if (!have_lead) {
            lead = g_raw[i].sign;
            have_lead = 1;
        }
        sum += g_raw[i].sign;
    }
    if (lead_sign_out)
        *lead_sign_out = lead;
    if (sum > 0)
        return 1;
    if (sum < 0)
        return -1;
    return lead; // tie: leading sign
}

static void count_lead_majority_disagree(metrics_t *m, long long quiet_us) {
    int *ep_of_raw = calloc((size_t)g_raw_n, sizeof(int));
    int ep_count = 0;
    assign_episodes(quiet_us, ep_of_raw, &ep_count);
    m->lead_vs_majority_disagree = 0;
    for (int e = 0; e < ep_count; e++) {
        int lead = 0;
        int maj = episode_majority(e, ep_of_raw, &lead);
        if (lead != maj)
            m->lead_vs_majority_disagree++;
    }
    free(ep_of_raw);
}

// A/B: fixed throttle then filter. Episode labeling uses quiet_us for metrics only.
static void policy_throttle(metrics_t *m, long long throttle_us, long long quiet_us) {
    pulse_t pulses[MAX_OUT];
    int ep_of_pulse[MAX_OUT];
    int n = 0;

    int *ep_of_raw = calloc((size_t)g_raw_n, sizeof(int));
    int ep_count = 0;
    assign_episodes(quiet_us, ep_of_raw, &ep_count);
    m->episodes = ep_count;
    count_lead_majority_disagree(m, quiet_us);

    long long *ep_first = calloc((size_t)ep_count + 1, sizeof(long long));
    fill_ep_first_raw(ep_of_raw, ep_count, ep_first);

    long long next_scroll = 0;
    int have_next = 0;
    for (int i = 0; i < g_raw_n; i++) {
        long long t = g_raw[i].t_us;
        if (!have_next || t > next_scroll) {
            next_scroll = t + throttle_us;
            have_next = 1;
            if (n < MAX_OUT) {
                pulses[n].t_us = t;
                pulses[n].sign = g_raw[i].sign;
                ep_of_pulse[n] = ep_of_raw[i];
                n++;
            }
        }
    }
    run_filter_pulses(pulses, n, m, ep_of_pulse, ep_first);
    free(ep_of_raw);
    free(ep_first);
}

// C: leading-edge + raw quiet suppress
static void policy_leading_quiet(metrics_t *m, long long quiet_us) {
    pulse_t pulses[MAX_OUT];
    int ep_of_pulse[MAX_OUT];
    int n = 0;

    int *ep_of_raw = calloc((size_t)g_raw_n, sizeof(int));
    int ep_count = 0;
    assign_episodes(quiet_us, ep_of_raw, &ep_count);
    m->episodes = ep_count;
    count_lead_majority_disagree(m, quiet_us);

    long long *ep_first = calloc((size_t)ep_count + 1, sizeof(long long));
    fill_ep_first_raw(ep_of_raw, ep_count, ep_first);

    int last_ep = -1;
    for (int i = 0; i < g_raw_n; i++) {
        int ep = ep_of_raw[i];
        if (ep != last_ep) {
            if (n < MAX_OUT) {
                pulses[n].t_us = g_raw[i].t_us;
                pulses[n].sign = g_raw[i].sign;
                ep_of_pulse[n] = ep;
                n++;
            }
            last_ep = ep;
        }
    }
    run_filter_pulses(pulses, n, m, ep_of_pulse, ep_first);
    free(ep_of_raw);
    free(ep_first);
}

// D/E: micro-majority over first window_ms of each episode, then suppress
static void policy_micro_majority(metrics_t *m, long long window_us, long long quiet_us) {
    pulse_t pulses[MAX_OUT];
    int ep_of_pulse[MAX_OUT];
    int n = 0;

    int *ep_of_raw = calloc((size_t)g_raw_n, sizeof(int));
    int ep_count = 0;
    assign_episodes(quiet_us, ep_of_raw, &ep_count);
    m->episodes = ep_count;
    count_lead_majority_disagree(m, quiet_us);

    long long *ep_first = calloc((size_t)ep_count + 1, sizeof(long long));
    fill_ep_first_raw(ep_of_raw, ep_count, ep_first);

    for (int e = 0; e < ep_count; e++) {
        long long t0 = -1;
        long long t_emit = -1;
        int sum = 0;
        int lead = 0;
        int have = 0;
        for (int i = 0; i < g_raw_n; i++) {
            if (ep_of_raw[i] != e)
                continue;
            if (!have) {
                t0 = g_raw[i].t_us;
                lead = g_raw[i].sign;
                have = 1;
            }
            if (g_raw[i].t_us - t0 < window_us) {
                sum += g_raw[i].sign;
                t_emit = g_raw[i].t_us; // last sample inside window
            }
        }
        if (!have)
            continue;
        if (t_emit < 0)
            t_emit = t0;
        long long t_window_end = t0 + window_us;
        int past_window = 0;
        for (int i = 0; i < g_raw_n; i++) {
            if (ep_of_raw[i] == e && g_raw[i].t_us >= t_window_end) {
                past_window = 1;
                break;
            }
        }
        if (past_window)
            t_emit = t_window_end;

        int maj = (sum > 0) ? 1 : (sum < 0) ? -1
                                              : lead;
        if (n < MAX_OUT) {
            pulses[n].t_us = t_emit;
            pulses[n].sign = maj;
            ep_of_pulse[n] = e;
            n++;
        }
    }
    run_filter_pulses(pulses, n, m, ep_of_pulse, ep_first);
    free(ep_of_raw);
    free(ep_first);
}

// F: full-burst majority, emit at last raw of episode (reference)
static void policy_full_majority(metrics_t *m, long long quiet_us) {
    pulse_t pulses[MAX_OUT];
    int ep_of_pulse[MAX_OUT];
    int n = 0;

    int *ep_of_raw = calloc((size_t)g_raw_n, sizeof(int));
    int ep_count = 0;
    assign_episodes(quiet_us, ep_of_raw, &ep_count);
    m->episodes = ep_count;
    count_lead_majority_disagree(m, quiet_us);

    long long *ep_first = calloc((size_t)ep_count + 1, sizeof(long long));
    fill_ep_first_raw(ep_of_raw, ep_count, ep_first);

    for (int e = 0; e < ep_count; e++) {
        long long t_last = 0;
        int lead = 0;
        int maj = episode_majority(e, ep_of_raw, &lead);
        for (int i = 0; i < g_raw_n; i++) {
            if (ep_of_raw[i] == e)
                t_last = g_raw[i].t_us;
        }
        if (n < MAX_OUT) {
            pulses[n].t_us = t_last;
            pulses[n].sign = maj;
            ep_of_pulse[n] = e;
            n++;
        }
    }
    run_filter_pulses(pulses, n, m, ep_of_pulse, ep_first);
    free(ep_of_raw);
    free(ep_first);
}

static void print_metrics(const metrics_t *m) {
    double avg_lat = m->first_latency_count
                         ? m->first_latency_ms_sum / (double)m->first_latency_count
                         : 0.0;
    printf("%-36s  ep=%4d  filt_in=%4d  logical=%4d  multi_ep=%4d  dir_chg=%4d  "
           "avg_1st_lat_ms=%6.2f  lead_vs_maj_disagree=%4d\n",
           m->name, m->episodes, m->filter_inputs, m->logical_outputs, m->same_episode_multi,
           m->direction_changes, avg_lat, m->lead_vs_majority_disagree);
}

static int cmp_ll(const void *a, const void *b) {
    long long x = *(const long long *)a;
    long long y = *(const long long *)b;
    return (x > y) - (x < y);
}

static void report_quiet_gaps(void) {
    if (g_raw_n < 2) {
        printf("Not enough raw events for gap distribution.\n");
        return;
    }

    long long *gaps = calloc((size_t)g_raw_n - 1, sizeof(long long));
    int ng = 0;
    for (int i = 1; i < g_raw_n; i++) {
        gaps[ng++] = g_raw[i].t_us - g_raw[i - 1].t_us;
    }
    qsort(gaps, (size_t)ng, sizeof(long long), cmp_ll);

    printf("\n=== Raw quiet-gap distribution (us between consecutive SCROLL_RAW) ===\n");
    printf("count=%d  min=%lld  p25=%lld  p50=%lld  p75=%lld  p90=%lld  p99=%lld  max=%lld\n",
           ng, gaps[0], gaps[ng / 4], gaps[ng / 2], gaps[(ng * 3) / 4],
           gaps[(ng * 9) / 10], gaps[(ng * 99) / 100], gaps[ng - 1]);

    // Histogram of gaps in ms buckets
    int b0_1 = 0, b1_5 = 0, b5_10 = 0, b10_20 = 0, b20_25 = 0, b25_30 = 0, b30_50 = 0, b50_100 = 0, b100p = 0;
    for (int i = 0; i < ng; i++) {
        double ms = (double)gaps[i] / 1000.0;
        if (ms < 1)
            b0_1++;
        else if (ms < 5)
            b1_5++;
        else if (ms < 10)
            b5_10++;
        else if (ms < 20)
            b10_20++;
        else if (ms < 25)
            b20_25++;
        else if (ms < 30)
            b25_30++;
        else if (ms < 50)
            b30_50++;
        else if (ms < 100)
            b50_100++;
        else
            b100p++;
    }
    printf("hist_ms: <1=%d  1-5=%d  5-10=%d  10-20=%d  20-25=%d  25-30=%d  30-50=%d  50-100=%d  >=100=%d\n",
           b0_1, b1_5, b5_10, b10_20, b20_25, b25_30, b30_50, b50_100, b100p);

    printf("\n=== Episodes vs quiet threshold (activity episodes from ALL raw) ===\n");
    int qlist[] = {20, 25, 30};
    int ep20 = 0;
    for (int qi = 0; qi < 3; qi++) {
        int *ep_of_raw = calloc((size_t)g_raw_n, sizeof(int));
        int ep_count = 0;
        assign_episodes((long long)qlist[qi] * 1000LL, ep_of_raw, &ep_count);
        if (qi == 0)
            ep20 = ep_count;
        int merged = ep20 - ep_count; // vs 20ms baseline: how many episodes disappear (merged)
        printf("quiet=%d ms: episodes=%d  merged_vs_20ms=%d\n", qlist[qi], ep_count, merged);
        free(ep_of_raw);
    }
    free(gaps);
}

static const char *sign_name(int s) {
    if (s > 0)
        return "+";
    if (s < 0)
        return "-";
    return "0";
}

// Window majority over raws with t - t0 < window_us.
// Tie-break: leading sign of the episode.
// Returns majority sign; sets *tie if sum==0 with >=1 sample; *n_win = samples in window.
static int window_majority(int ep, const int *ep_of_raw, long long window_us,
                           int *tie_out, int *n_win_out, long long *t_emit_out) {
    long long t0 = -1;
    int lead = 0;
    int sum = 0;
    int n_win = 0;
    long long t_last_in = -1;
    int have = 0;

    for (int i = 0; i < g_raw_n; i++) {
        if (ep_of_raw[i] != ep)
            continue;
        if (!have) {
            t0 = g_raw[i].t_us;
            lead = g_raw[i].sign;
            have = 1;
        }
        if (g_raw[i].t_us - t0 < window_us) {
            sum += g_raw[i].sign;
            n_win++;
            t_last_in = g_raw[i].t_us;
        }
    }
    if (!have) {
        if (tie_out)
            *tie_out = 0;
        if (n_win_out)
            *n_win_out = 0;
        if (t_emit_out)
            *t_emit_out = 0;
        return 0;
    }

    long long t_window_end = t0 + window_us;
    int past = 0;
    for (int i = 0; i < g_raw_n; i++) {
        if (ep_of_raw[i] == ep && g_raw[i].t_us >= t_window_end) {
            past = 1;
            break;
        }
    }
    long long t_emit = past ? t_window_end : (t_last_in >= 0 ? t_last_in : t0);
    if (t_emit_out)
        *t_emit_out = t_emit;
    if (n_win_out)
        *n_win_out = n_win;

    if (sum == 0) {
        if (tie_out)
            *tie_out = 1;
        return lead; // tie-break: leading sign
    }
    if (tie_out)
        *tie_out = 0;
    return (sum > 0) ? 1 : -1;
}

static void print_latency_dist(const char *label, long long *lats_us, int n) {
    if (n <= 0) {
        printf("%s: no samples\n", label);
        return;
    }
    qsort(lats_us, (size_t)n, sizeof(long long), cmp_ll);
    int i50 = n / 2;
    int i90 = (n * 9) / 10;
    int i95 = (n * 95) / 100;
    if (i90 >= n)
        i90 = n - 1;
    if (i95 >= n)
        i95 = n - 1;
    printf("%s (n=%d): min=%.3f  p50=%.3f  p90=%.3f  p95=%.3f  max=%.3f ms\n",
           label, n,
           (double)lats_us[0] / 1000.0,
           (double)lats_us[i50] / 1000.0,
           (double)lats_us[i90] / 1000.0,
           (double)lats_us[i95] / 1000.0,
           (double)lats_us[n - 1] / 1000.0);
}

static void report_q20_direction_quality(void) {
    const long long quiet_us = 20000;
    int *ep_of_raw = calloc((size_t)g_raw_n, sizeof(int));
    int ep_count = 0;
    assign_episodes(quiet_us, ep_of_raw, &ep_count);

    printf("\n=== Q20 direction-quality refinement (comparison signal only) ===\n");
    printf("Episodes: %d (quiet=20ms on ALL raw REL_Y)\n", ep_count);
    printf("Tie-break rule for 3ms/5ms micro-majority: if window sum==0, use leading sign.\n");
    printf("Full-burst majority is NOT physical ground truth.\n\n");

    int lead_vs_full = 0;
    int m3_vs_full = 0;
    int m5_vs_full = 0;
    int m3_vs_m5 = 0;
    int ties_3 = 0;
    int ties_5 = 0;
    int single_raw = 0;
    int sign_changes_along_chain = 0; // lead→3→5→full has at least one step change

    int lead_diff_both_micro = 0; // lead differs from both 3ms and 5ms

    long long *lat_d = calloc((size_t)ep_count, sizeof(long long));
    long long *lat_e = calloc((size_t)ep_count, sizeof(long long));
    int n_lat_d = 0, n_lat_e = 0;

    int *flag_ep = calloc((size_t)ep_count, sizeof(int));
    int n_flag = 0;

    for (int e = 0; e < ep_count; e++) {
        long long t0 = -1, t_last = -1;
        int n_raw = 0, n_plus = 0, n_minus = 0;
        int lead = 0;
        int have = 0;
        for (int i = 0; i < g_raw_n; i++) {
            if (ep_of_raw[i] != e)
                continue;
            if (!have) {
                t0 = g_raw[i].t_us;
                lead = g_raw[i].sign;
                have = 1;
            }
            t_last = g_raw[i].t_us;
            n_raw++;
            if (g_raw[i].sign > 0)
                n_plus++;
            else
                n_minus++;
        }
        if (!have)
            continue;
        if (n_raw == 1)
            single_raw++;

        int full_sum = n_plus - n_minus;
        int full = (full_sum > 0) ? 1 : (full_sum < 0) ? -1
                                                         : lead;

        int tie3 = 0, tie5 = 0, n3 = 0, n5 = 0;
        long long t_emit3 = 0, t_emit5 = 0;
        int m3 = window_majority(e, ep_of_raw, 3000, &tie3, &n3, &t_emit3);
        int m5 = window_majority(e, ep_of_raw, 5000, &tie5, &n5, &t_emit5);
        if (tie3)
            ties_3++;
        if (tie5)
            ties_5++;

        if (lead != full)
            lead_vs_full++;
        if (m3 != full)
            m3_vs_full++;
        if (m5 != full)
            m5_vs_full++;
        if (m3 != m5)
            m3_vs_m5++;

        if (lead != m3 || m3 != m5 || m5 != full)
            sign_changes_along_chain++;

        lat_d[n_lat_d++] = t_emit3 - t0;
        lat_e[n_lat_e++] = t_emit5 - t0;

        int interesting = 0;
        if (lead != m3 && lead != m5)
            interesting = 1;
        if (m3 != m5)
            interesting = 1;
        if (m5 != full)
            interesting = 1;
        if (lead != m3 && lead != m5)
            lead_diff_both_micro++;

        if (interesting)
            flag_ep[n_flag++] = e;

        (void)t_last;
    }

    printf("--- Aggregate (q20, n=%d episodes) ---\n", ep_count);
    printf("lead vs full-majority disagreements:     %d\n", lead_vs_full);
    printf("3ms micro-maj vs full-majority:          %d\n", m3_vs_full);
    printf("5ms micro-maj vs full-majority:          %d\n", m5_vs_full);
    printf("3ms vs 5ms disagreements:                %d\n", m3_vs_m5);
    printf("3ms ties (sum==0, break→lead):           %d\n", ties_3);
    printf("5ms ties (sum==0, break→lead):           %d\n", ties_5);
    printf("single-raw episodes:                     %d\n", single_raw);
    printf("episodes with any sign change along\n");
    printf("  lead → 3ms → 5ms → full-majority:      %d\n", sign_changes_along_chain);
    printf("lead differs from BOTH 3ms and 5ms:      %d\n", lead_diff_both_micro);

    printf("\n--- First-output latency of micro-majority decision (ms from first raw) ---\n");
    print_latency_dist("D 3ms microMaj q20", lat_d, n_lat_d);
    print_latency_dist("E 5ms microMaj q20", lat_e, n_lat_e);

    printf("\n--- Flagged episodes (lead≠both micro | 3ms≠5ms | 5ms≠full) count=%d ---\n", n_flag);
    for (int fi = 0; fi < n_flag; fi++) {
        int e = flag_ep[fi];
        long long t0 = -1, t_last = -1;
        int n_raw = 0, n_plus = 0, n_minus = 0;
        int lead = 0;
        int have = 0;
        for (int i = 0; i < g_raw_n; i++) {
            if (ep_of_raw[i] != e)
                continue;
            if (!have) {
                t0 = g_raw[i].t_us;
                lead = g_raw[i].sign;
                have = 1;
            }
            t_last = g_raw[i].t_us;
            if (g_raw[i].sign > 0)
                n_plus++;
            else
                n_minus++;
            n_raw++;
        }
        int full_sum = n_plus - n_minus;
        int full = (full_sum > 0) ? 1 : (full_sum < 0) ? -1
                                                         : lead;
        int tie3 = 0, tie5 = 0, n3 = 0, n5 = 0;
        long long t_emit3 = 0, t_emit5 = 0;
        int m3 = window_majority(e, ep_of_raw, 3000, &tie3, &n3, &t_emit3);
        int m5 = window_majority(e, ep_of_raw, 5000, &tie5, &n5, &t_emit5);
        double dur_ms = (double)(t_last - t0) / 1000.0;

        printf("\nep#%d  start=%lld.%06lld  dur_ms=%.3f  raw=%d  +=%d  -=%d\n",
               e, t0 / 1000000LL, t0 % 1000000LL, dur_ms, n_raw, n_plus, n_minus);
        printf("  decisions: lead=%s  3ms=%s%s  5ms=%s%s  full=%s\n",
               sign_name(lead),
               sign_name(m3), tie3 ? "(tie→lead)" : "",
               sign_name(m5), tie5 ? "(tie→lead)" : "",
               sign_name(full));
        printf("  flags:");
        if (lead != m3 && lead != m5)
            printf(" lead≠3&5");
        if (m3 != m5)
            printf(" 3≠5");
        if (m5 != full)
            printf(" 5≠full");
        if (lead != full)
            printf(" lead≠full");
        printf("\n  first_raws:");
        int show = n_raw < 10 ? n_raw : 10;
        int shown = 0;
        for (int i = 0; i < g_raw_n && shown < show; i++) {
            if (ep_of_raw[i] != e)
                continue;
            printf(" %+d@+%.3fms", g_raw[i].sign, (double)(g_raw[i].t_us - t0) / 1000.0);
            shown++;
        }
        printf("\n");
    }

    printf("\n=== Data-based recommendation (D q20 vs E q20) ===\n");
    printf("Priority: (1) direction robustness (2) preserve rapid detents (3) low latency\n");
    printf("Quiet 25/30 not recommended unless these numbers force it — they do not:\n");
    printf("  q20 keeps all 145 episodes; q25/q30 only merge 6–7.\n");
    printf("Direction vs full-majority (lower disagreement = closer to full-burst signal):\n");
    printf("  lead: %d   3ms: %d   5ms: %d   (of %d)\n", lead_vs_full, m3_vs_full, m5_vs_full, ep_count);
    printf("3ms vs 5ms differ on %d episodes; 5ms vs full differ on %d.\n", m3_vs_m5, m5_vs_full);

    free(ep_of_raw);
    free(lat_d);
    free(lat_e);
    free(flag_ep);
}

// CAUSAL micro-majority: always decide at t0+window_us (no hindsight early emit).
// Include raws with t in [t0, t0+window_us] (closed interval on event timestamps).
static int causal_window_majority(int ep, const int *ep_of_raw, long long window_us,
                                  int *tie_out, int *n_win_out) {
    long long t0 = -1;
    int lead = 0;
    int sum = 0;
    int n_win = 0;
    int have = 0;

    for (int i = 0; i < g_raw_n; i++) {
        if (ep_of_raw[i] != ep)
            continue;
        if (!have) {
            t0 = g_raw[i].t_us;
            lead = g_raw[i].sign;
            have = 1;
        }
        long long dt = g_raw[i].t_us - t0;
        if (dt >= 0 && dt <= window_us) {
            sum += g_raw[i].sign;
            n_win++;
        }
    }
    if (!have) {
        if (tie_out)
            *tie_out = 0;
        if (n_win_out)
            *n_win_out = 0;
        return 0;
    }
    if (n_win_out)
        *n_win_out = n_win;
    if (sum == 0) {
        if (tie_out)
            *tie_out = 1;
        return lead;
    }
    if (tie_out)
        *tie_out = 0;
    return (sum > 0) ? 1 : -1;
}

static void causal_policy_metrics(const char *name, long long window_us, int use_lead,
                                  long long quiet_us) {
    int *ep_of_raw = calloc((size_t)g_raw_n, sizeof(int));
    int ep_count = 0;
    assign_episodes(quiet_us, ep_of_raw, &ep_count);

    pulse_t pulses[MAX_OUT];
    int ep_of_pulse[MAX_OUT];
    long long *ep_first = calloc((size_t)ep_count + 1, sizeof(long long));
    long long *lats = calloc((size_t)ep_count, sizeof(long long));
    int n = 0;
    int n_lat = 0;
    int vs_full = 0;
    int ties = 0;

    fill_ep_first_raw(ep_of_raw, ep_count, ep_first);

    for (int e = 0; e < ep_count; e++) {
        long long t0 = ep_first[e];
        int lead = 0;
        int n_plus = 0, n_minus = 0;
        for (int i = 0; i < g_raw_n; i++) {
            if (ep_of_raw[i] != e)
                continue;
            if (lead == 0)
                lead = g_raw[i].sign;
            if (g_raw[i].sign > 0)
                n_plus++;
            else
                n_minus++;
        }
        int full_sum = n_plus - n_minus;
        int full = (full_sum > 0) ? 1 : (full_sum < 0) ? -1
                                                         : lead;

        int sign;
        long long t_emit;
        if (use_lead) {
            sign = lead;
            t_emit = t0; // immediate leading-edge is causal
        } else {
            int tie = 0, n_win = 0;
            sign = causal_window_majority(e, ep_of_raw, window_us, &tie, &n_win);
            if (tie)
                ties++;
            t_emit = t0 + window_us; // ALWAYS wait full window — no hindsight
        }

        if (sign != full)
            vs_full++;

        if (n < MAX_OUT) {
            pulses[n].t_us = t_emit;
            pulses[n].sign = sign;
            ep_of_pulse[n] = e;
            n++;
        }
        lats[n_lat++] = t_emit - t0;
    }

    metrics_t m;
    memset(&m, 0, sizeof(m));
    m.name = name;
    m.episodes = ep_count;
    m.lead_vs_majority_disagree = vs_full; // here: policy sign vs full-burst
    run_filter_pulses(pulses, n, &m, ep_of_pulse, ep_first);

    printf("%-32s  ep=%3d  filt_in=%3d  logical=%3d  multi_ep=%3d  dir_chg=%3d  "
           "vs_full=%3d  ties=%3d\n",
           name, m.episodes, m.filter_inputs, m.logical_outputs, m.same_episode_multi,
           m.direction_changes, vs_full, ties);
    print_latency_dist(name, lats, n_lat);

    free(ep_of_raw);
    free(ep_first);
    free(lats);
}

// Production-equivalent: scroll_burst module + secondary 10ms next_scroll + filter.
static void report_production_equiv_q20(void) {
    printf("\n=== PRODUCTION-EQUIVALENT (scroll_burst W=10ms quiet=20ms) ===\n");
    printf("Raw-scroll path only (burst + next_scroll + filter). Does NOT cover SYN/key dispatch.\n");

    scroll_burst_t burst;
    scroll_burst_init(&burst);

    struct timeval next_scroll = {0, 0};
    const struct timeval scroll_time_diff = {0, 10000};

    int *ep_of_raw = calloc((size_t)g_raw_n, sizeof(int));
    int ep_count = 0;
    assign_episodes(20000, ep_of_raw, &ep_count);

    pulse_t pulses[MAX_OUT];
    int ep_of_pulse[MAX_OUT];
    int n = 0;
    int drop10 = 0;

    for (int i = 0; i < g_raw_n; i++) {
        struct timeval et = tv_from_us(g_raw[i].t_us);
        scroll_burst_result_t r;

        // Due decisions before this event (wall ≈ event time when stream is live).
        scroll_burst_poll_due(&burst, &et, &r);
        if (r.action == SCROLL_BURST_EMIT) {
            if (timercmp(&r.emit_event_time, &next_scroll, >)) {
                timeradd(&r.emit_event_time, &scroll_time_diff, &next_scroll);
                if (n < MAX_OUT) {
                    pulses[n].t_us = (long long)r.emit_event_time.tv_sec * 1000000LL +
                                     r.emit_event_time.tv_usec;
                    pulses[n].sign = r.candidate;
                    ep_of_pulse[n] = ep_of_raw[i > 0 ? i - 1 : 0];
                    n++;
                }
            } else {
                drop10++;
            }
        }

        scroll_burst_on_rel_y(&burst, &et, g_raw[i].sign, &et, &r);
        if (r.action == SCROLL_BURST_EMIT) {
            if (timercmp(&r.emit_event_time, &next_scroll, >)) {
                timeradd(&r.emit_event_time, &scroll_time_diff, &next_scroll);
                if (n < MAX_OUT) {
                    pulses[n].t_us = (long long)r.emit_event_time.tv_sec * 1000000LL +
                                     r.emit_event_time.tv_usec;
                    pulses[n].sign = r.candidate;
                    ep_of_pulse[n] = ep_of_raw[i];
                    n++;
                }
            } else {
                drop10++;
            }
        }
    }

    if (g_raw_n > 0) {
        long long last = g_raw[g_raw_n - 1].t_us + 10000;
        struct timeval et = tv_from_us(last);
        scroll_burst_result_t r;
        scroll_burst_poll_due(&burst, &et, &r);
        if (r.action == SCROLL_BURST_EMIT) {
            if (timercmp(&r.emit_event_time, &next_scroll, >)) {
                timeradd(&r.emit_event_time, &scroll_time_diff, &next_scroll);
                if (n < MAX_OUT) {
                    pulses[n].t_us = (long long)r.emit_event_time.tv_sec * 1000000LL +
                                     r.emit_event_time.tv_usec;
                    pulses[n].sign = r.candidate;
                    ep_of_pulse[n] = ep_of_raw[g_raw_n - 1];
                    n++;
                }
            } else {
                drop10++;
            }
        }
    }

    metrics_t m;
    memset(&m, 0, sizeof(m));
    m.name = "PROD burst 10ms+q20 + next_scroll + confirm=1";
    m.episodes = ep_count;
    m.reversal_confirm = 1;
    long long *ep_first = calloc((size_t)ep_count + 1, sizeof(long long));
    fill_ep_first_raw(ep_of_raw, ep_count, ep_first);
    run_filter_pulses(pulses, n, &m, ep_of_pulse, ep_first);

    printf("%-40s ep=%d filt_in=%d logical=%d multi_ep=%d dir_chg=%d drop10_secondary=%d\n",
           m.name, m.episodes, m.filter_inputs, m.logical_outputs, m.same_episode_multi,
           m.direction_changes, drop10);

    // Compare signs to an independently-computed causal window majority at
    // the SAME window as production (self-consistency check, not a fixed
    // dataset expectation -- production window is SCROLL_MICRO_MAJORITY_MS).
    int disagree_e = 0;
    int matched = 0;
    for (int e = 0; e < ep_count; e++) {
        int ee = causal_window_majority(e, ep_of_raw, SCROLL_MICRO_MAJORITY_MS * 1000LL, NULL, NULL);
        int got = 0;
        int have = 0;
        for (int i = 0; i < n; i++) {
            if (ep_of_pulse[i] == e) {
                got = pulses[i].sign;
                have = 1;
                break;
            }
        }
        if (have) {
            matched++;
            if (got != ee)
                disagree_e++;
        }
    }
    printf("vs causal window majority (same %dms window): matched_eps=%d disagree=%d (expect 0)\n",
           SCROLL_MICRO_MAJORITY_MS, matched, disagree_e);
    if (m.same_episode_multi != 0 || disagree_e != 0) {
        printf("NOTE: expected 0 multi_ep, 0 disagree with causal window majority at production window.\n");
    } else {
        printf("OK: matches causal window-majority expectations at production window (multi_ep=0, disagree=0).\n");
    }

    free(ep_of_raw);
    free(ep_first);
}

typedef struct {
    long long t_us;
    int sign;
    int ep;
} logical_out_t;

static void filter_pulses_confirm(const pulse_t *pulses, int n, const int *ep_of_pulse,
                                  int reversal_confirm,
                                  logical_out_t *outs, int *n_out,
                                  int *dir_changes, int *swallowed_opposite) {
    scroll_filter_t f;
    scroll_filter_init(&f);
    f.reversal_confirm = reversal_confirm;

    int last_logical = 0;
    int up_acc = 0, down_acc = 0;
    *n_out = 0;
    *dir_changes = 0;
    *swallowed_opposite = 0;

    for (int i = 0; i < n; i++) {
        struct timeval t = tv_from_us(pulses[i].t_us);
        int committed_before = f.dir;
        scroll_filter_event_t fo = scroll_filter_step(&f, &t, pulses[i].sign);
        if (committed_before != 0 && pulses[i].sign != committed_before && fo == SCROLL_FILTER_NONE)
            (*swallowed_opposite)++;

        int filtered = 0;
        if (fo == SCROLL_FILTER_UP)
            filtered = 1;
        else if (fo == SCROLL_FILTER_DOWN)
            filtered = -1;
        else
            continue;

        if (filtered == 1) {
            up_acc++;
            down_acc = 0;
        } else {
            down_acc++;
            up_acc = 0;
        }
        if (up_acc != DIAL_SENSITIVITY && down_acc != DIAL_SENSITIVITY)
            continue;

        int logical = (up_acc == DIAL_SENSITIVITY) ? 1 : -1;
        up_acc = 0;
        down_acc = 0;
        if (last_logical != 0 && last_logical != logical)
            (*dir_changes)++;
        last_logical = logical;
        if (*n_out < MAX_OUT) {
            outs[*n_out].t_us = pulses[i].t_us;
            outs[*n_out].sign = logical;
            outs[*n_out].ep = ep_of_pulse[i];
            (*n_out)++;
        }
    }
}

static void report_reversal_confirm_ab(void) {
    printf("\n=== REVERSAL CONFIRM A/B at episode granularity ===\n");
    printf("Filter inputs are production burst candidates (10ms µmaj + q20 + next_scroll).\n");
    printf("A: reversal_confirm=2 (compiled default). B: reversal_confirm=1.\n");
    printf("This is burst-candidate replay, not full SYN/key dispatch equivalence.\n\n");

    scroll_burst_t burst;
    scroll_burst_init(&burst);
    struct timeval next_scroll = {0, 0};
    const struct timeval scroll_time_diff = {0, 10000};

    int *ep_of_raw = calloc((size_t)g_raw_n, sizeof(int));
    int ep_count = 0;
    assign_episodes(20000, ep_of_raw, &ep_count);

    pulse_t pulses[MAX_OUT];
    int ep_of_pulse[MAX_OUT];
    int n = 0;

    for (int i = 0; i < g_raw_n; i++) {
        struct timeval et = tv_from_us(g_raw[i].t_us);
        scroll_burst_result_t r;
        scroll_burst_poll_due(&burst, &et, &r);
        if (r.action == SCROLL_BURST_EMIT) {
            if (timercmp(&r.emit_event_time, &next_scroll, >)) {
                timeradd(&r.emit_event_time, &scroll_time_diff, &next_scroll);
                if (n < MAX_OUT) {
                    pulses[n].t_us = (long long)r.emit_event_time.tv_sec * 1000000LL +
                                     r.emit_event_time.tv_usec;
                    pulses[n].sign = r.candidate;
                    ep_of_pulse[n] = ep_of_raw[i > 0 ? i - 1 : 0];
                    n++;
                }
            }
        }
        scroll_burst_on_rel_y(&burst, &et, g_raw[i].sign, &et, &r);
        if (r.action == SCROLL_BURST_EMIT) {
            if (timercmp(&r.emit_event_time, &next_scroll, >)) {
                timeradd(&r.emit_event_time, &scroll_time_diff, &next_scroll);
                if (n < MAX_OUT) {
                    pulses[n].t_us = (long long)r.emit_event_time.tv_sec * 1000000LL +
                                     r.emit_event_time.tv_usec;
                    pulses[n].sign = r.candidate;
                    ep_of_pulse[n] = ep_of_raw[i];
                    n++;
                }
            }
        }
    }
    if (g_raw_n > 0) {
        long long last = g_raw[g_raw_n - 1].t_us + 10000;
        struct timeval et = tv_from_us(last);
        scroll_burst_result_t r;
        scroll_burst_poll_due(&burst, &et, &r);
        if (r.action == SCROLL_BURST_EMIT) {
            if (timercmp(&r.emit_event_time, &next_scroll, >)) {
                timeradd(&r.emit_event_time, &scroll_time_diff, &next_scroll);
                if (n < MAX_OUT) {
                    pulses[n].t_us = (long long)r.emit_event_time.tv_sec * 1000000LL +
                                     r.emit_event_time.tv_usec;
                    pulses[n].sign = r.candidate;
                    ep_of_pulse[n] = ep_of_raw[g_raw_n - 1];
                    n++;
                }
            }
        }
    }

    logical_out_t a[MAX_OUT], b[MAX_OUT];
    int na = 0, nb = 0, dirc_a = 0, dirc_b = 0, sw_a = 0, sw_b = 0;
    filter_pulses_confirm(pulses, n, ep_of_pulse, 2, a, &na, &dirc_a, &sw_a);
    filter_pulses_confirm(pulses, n, ep_of_pulse, 1, b, &nb, &dirc_b, &sw_b);

    printf("candidates (filter inputs): %d  episodes(q20): %d\n", n, ep_count);
    printf("A confirm=2: logical=%d  dir_chg=%d  opposite_episodes_swallowed=%d\n",
           na, dirc_a, sw_a);
    printf("B confirm=1: logical=%d  dir_chg=%d  opposite_episodes_swallowed=%d\n",
           nb, dirc_b, sw_b);

    printf("\n--- Cases where confirm=1 output differs from confirm=2 ---\n");
    int ia = 0, ib = 0, ndiff = 0;
    while (ia < na || ib < nb) {
        int a_done = ia >= na;
        int b_done = ib >= nb;
        if (!a_done && !b_done && a[ia].t_us == b[ib].t_us && a[ia].sign == b[ib].sign) {
            ia++;
            ib++;
            continue;
        }
        ndiff++;
        if (a_done || (!b_done && (b[ib].t_us < a[ia].t_us ||
                                   (b[ib].t_us == a[ia].t_us && b[ib].sign != a[ia].sign)))) {
            int ep = b[ib].ep;
            printf("B-only  ep=%d t=%lld.%06lld sign=%s  (confirm=1 extra/changed)\n",
                   ep, b[ib].t_us / 1000000LL, b[ib].t_us % 1000000LL, sign_name(b[ib].sign));
            if (ep >= 0 && ep < ep_count) {
                long long t0 = -1, tlast = -1;
                int nraw = 0, np = 0, nm = 0;
                for (int i = 0; i < g_raw_n; i++) {
                    if (ep_of_raw[i] != ep)
                        continue;
                    if (t0 < 0)
                        t0 = g_raw[i].t_us;
                    tlast = g_raw[i].t_us;
                    nraw++;
                    if (g_raw[i].sign > 0)
                        np++;
                    else
                        nm++;
                }
                printf("  episode: start=%lld.%06lld dur_ms=%.3f raw=%d +=%d -=%d\n",
                       t0 / 1000000LL, t0 % 1000000LL,
                       t0 >= 0 ? (double)(tlast - t0) / 1000.0 : 0.0, nraw, np, nm);
            }
            ib++;
            continue;
        }
        {
            int ep = a[ia].ep;
            printf("A-only  ep=%d t=%lld.%06lld sign=%s  (confirm=2 extra/changed)\n",
                   ep, a[ia].t_us / 1000000LL, a[ia].t_us % 1000000LL, sign_name(a[ia].sign));
            ia++;
        }
    }
    printf("differing logical events: %d\n", ndiff);
    if (ndiff == 0)
        printf("No output differences: confirm=1 and confirm=2 agree on this capture.\n");

    free(ep_of_raw);
}

static void report_causal_q20(void) {
    printf("\n=== CAUSAL q20 replay (no future knowledge) ===\n");
    printf("Micro-majority: collect [t0, t0+W], decide/emit ALWAYS at t0+W.\n");
    printf("Leading-edge: emit at t0 (causal).\n");
    printf("Quiet unlock: next episode only after >20ms raw silence (partition on all RAW).\n");
    printf("Tie-break: window sum==0 → leading sign.\n");
    printf("Full-burst majority is comparison-only.\n\n");

    printf("--- Causal candidate table ---\n");
    causal_policy_metrics("C' lead+q20 (causal)", 0, 1, 20000);
    causal_policy_metrics("D' causal 3ms+q20", 3000, 0, 20000);
    causal_policy_metrics("E' causal 5ms+q20", 5000, 0, 20000);

    // Compare causal 3 vs 5 vs lead agreement with full, and 3 vs 5
    int *ep_of_raw = calloc((size_t)g_raw_n, sizeof(int));
    int ep_count = 0;
    assign_episodes(20000, ep_of_raw, &ep_count);
    int d_vs_e = 0, lead_vs_d = 0, lead_vs_e = 0;
    int d_vs_full = 0, e_vs_full = 0, lead_vs_full = 0;
    for (int e = 0; e < ep_count; e++) {
        int lead = 0, n_plus = 0, n_minus = 0;
        for (int i = 0; i < g_raw_n; i++) {
            if (ep_of_raw[i] != e)
                continue;
            if (lead == 0)
                lead = g_raw[i].sign;
            if (g_raw[i].sign > 0)
                n_plus++;
            else
                n_minus++;
        }
        int full = (n_plus - n_minus > 0) ? 1 : (n_plus - n_minus < 0) ? -1
                                                                        : lead;
        int d = causal_window_majority(e, ep_of_raw, 3000, NULL, NULL);
        int ee = causal_window_majority(e, ep_of_raw, 5000, NULL, NULL);
        if (lead != full)
            lead_vs_full++;
        if (d != full)
            d_vs_full++;
        if (ee != full)
            e_vs_full++;
        if (d != ee)
            d_vs_e++;
        if (lead != d)
            lead_vs_d++;
        if (lead != ee)
            lead_vs_e++;
    }
    printf("\n--- Causal sign disagreements (of %d eps) ---\n", ep_count);
    printf("lead vs full: %d\n", lead_vs_full);
    printf("causal3 vs full: %d\n", d_vs_full);
    printf("causal5 vs full: %d\n", e_vs_full);
    printf("causal3 vs causal5: %d\n", d_vs_e);
    printf("lead vs causal3: %d\n", lead_vs_d);
    printf("lead vs causal5: %d\n", lead_vs_e);

    // Extreme burst check
    printf("\n--- Extreme burst ep (89 raw / ~22.4ms) under CAUSAL windows ---\n");
    for (int e = 0; e < ep_count; e++) {
        int n_raw = 0;
        long long t0 = -1, t_last = -1;
        for (int i = 0; i < g_raw_n; i++) {
            if (ep_of_raw[i] != e)
                continue;
            if (t0 < 0)
                t0 = g_raw[i].t_us;
            t_last = g_raw[i].t_us;
            n_raw++;
        }
        if (n_raw < 80)
            continue;
        int lead = 0;
        for (int i = 0; i < g_raw_n; i++) {
            if (ep_of_raw[i] == e) {
                lead = g_raw[i].sign;
                break;
            }
        }
        int n_plus = 0, n_minus = 0;
        for (int i = 0; i < g_raw_n; i++) {
            if (ep_of_raw[i] != e)
                continue;
            if (g_raw[i].sign > 0)
                n_plus++;
            else
                n_minus++;
        }
        int full = (n_plus - n_minus > 0) ? 1 : -1;
        int d = causal_window_majority(e, ep_of_raw, 3000, NULL, NULL);
        int ee = causal_window_majority(e, ep_of_raw, 5000, NULL, NULL);
        printf("ep#%d raw=%d dur_ms=%.3f lead=%s causal3=%s causal5=%s full=%s\n",
               e, n_raw, (double)(t_last - t0) / 1000.0,
               sign_name(lead), sign_name(d), sign_name(ee), sign_name(full));
    }

    free(ep_of_raw);
}

int main(int argc, char **argv) {
    const char *path = (argc > 1) ? argv[1] : "HDZGOGGLE-diag.log";
    int n = load_scroll_raw(path);
    if (n <= 0) {
        fprintf(stderr, "No SCROLL_RAW events loaded from %s\n", path);
        return 1;
    }
    printf("Loaded %d SCROLL_RAW events from %s\n", n, path);

    report_quiet_gaps();

    printf("\n=== Candidate replay (filter = scroll_filter_step, DIAL_SENSITIVITY=1) ===\n");
    printf("Note: lead_vs_maj_disagree uses that row's quiet threshold episode partition.\n");
    printf("      Burst majority is a comparison signal only, not physical truth.\n\n");

    metrics_t m;

    // A: current 10ms — report under quiet=20 for episode metrics (common separator)
    memset(&m, 0, sizeof(m));
    m.name = "A current throttle 10ms (ep@20)";
    policy_throttle(&m, 10000, 20000);
    print_metrics(&m);

    int th[] = {15, 20, 25, 30};
    for (int i = 0; i < 4; i++) {
        char name[64];
        snprintf(name, sizeof(name), "B throttle %dms (ep@20)", th[i]);
        memset(&m, 0, sizeof(m));
        // keep name stable: print via temporary buffer copied into metrics
        static char names_b[4][64];
        snprintf(names_b[i], sizeof(names_b[i]), "B throttle %dms (ep@20)", th[i]);
        m.name = names_b[i];
        policy_throttle(&m, (long long)th[i] * 1000LL, 20000);
        print_metrics(&m);
        (void)name;
    }

    int q[] = {20, 25, 30};
    for (int i = 0; i < 3; i++) {
        static char names_c[3][64];
        snprintf(names_c[i], sizeof(names_c[i]), "C leading-edge quiet %dms", q[i]);
        memset(&m, 0, sizeof(m));
        m.name = names_c[i];
        policy_leading_quiet(&m, (long long)q[i] * 1000LL);
        print_metrics(&m);
    }

    for (int i = 0; i < 3; i++) {
        static char names_d[3][64];
        snprintf(names_d[i], sizeof(names_d[i]), "D microMaj 3ms quiet %dms", q[i]);
        memset(&m, 0, sizeof(m));
        m.name = names_d[i];
        policy_micro_majority(&m, 3000, (long long)q[i] * 1000LL);
        print_metrics(&m);
    }

    for (int i = 0; i < 3; i++) {
        static char names_e[3][64];
        snprintf(names_e[i], sizeof(names_e[i]), "E microMaj 5ms quiet %dms", q[i]);
        memset(&m, 0, sizeof(m));
        m.name = names_e[i];
        policy_micro_majority(&m, 5000, (long long)q[i] * 1000LL);
        print_metrics(&m);
    }

    for (int i = 0; i < 3; i++) {
        static char names_f[3][64];
        snprintf(names_f[i], sizeof(names_f[i]), "F full-burst maj quiet %dms", q[i]);
        memset(&m, 0, sizeof(m));
        m.name = names_f[i];
        policy_full_majority(&m, (long long)q[i] * 1000LL);
        print_metrics(&m);
    }

    // Sanity: current pipeline filter-input count should be near measured 220
    printf("\nSanity: measured hardware run had ~220 filter inputs / ~174 logical.\n");

    report_q20_direction_quality();

    report_causal_q20();

    report_production_equiv_q20();

    report_reversal_confirm_ab();

    printf("\nPhase 2 raw-scroll replay complete (not full SYN/key input-path equivalence).\n");
    return 0;
}
