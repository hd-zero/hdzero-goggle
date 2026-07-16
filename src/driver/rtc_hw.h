#pragma once
// Internal per-target backend for the physical clock. rtc.c owns all the shared
// date math + formatting with no build/platform #ifdefs; these two calls are the
// only hardware-touching seam:
//   goggle   -> src/driver/rtc_hw.c        (/dev/rtc ioctls + settimeofday)
//   emulator -> src/emulator/rtc_hw_emu.c  (in-memory clock)
// CMake compiles exactly one of the two.
#include "rtc.h" // struct rtc_date

#ifdef __cplusplus
extern "C" {
#endif

// Read the hardware clock into *rd (leaves *rd untouched on failure).
void rtc_hw_read(struct rtc_date *rd);
// Write *rd to the hardware clock (and, on the goggle, the OS clock).
void rtc_hw_write(const struct rtc_date *rd);

#ifdef __cplusplus
}
#endif
