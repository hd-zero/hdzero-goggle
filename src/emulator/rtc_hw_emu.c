// rtc_hw_emu.c - Emulator backend for the physical clock (see rtc_hw.h).
//
// No /dev/rtc on a dev box, so the clock is just an in-memory value the app sets
// and reads back. Starts at the epoch so rtc_init() treats it as "no battery" and
// seeds it from settings, exactly like a fresh goggle. Compiled instead of
// src/driver/rtc_hw.c when EMULATOR_BUILD.
#ifdef EMULATOR_BUILD

#include "rtc_hw.h"

static struct rtc_date g_rtc_date = {1970, 1, 1, 0, 0, 0};

void rtc_hw_read(struct rtc_date *rd) {
    *rd = g_rtc_date;
}

void rtc_hw_write(const struct rtc_date *rd) {
    g_rtc_date = *rd;
}

#endif // EMULATOR_BUILD
