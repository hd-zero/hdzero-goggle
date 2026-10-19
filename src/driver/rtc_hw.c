// rtc_hw.c - Goggle backend for the physical clock (see rtc_hw.h).
//
// Reads/writes /dev/rtc via the kernel rtc ioctls and mirrors the wall clock into
// the OS via settimeofday(). The struct rtc_time <-> struct rtc_date conversions
// live here because only this hardware path needs the kernel type; the shared
// rtc.c deals purely in struct rtc_date. The emulator supplies rtc_hw_emu.c instead.
#include "rtc_hw.h"

#include <errno.h>
#include <fcntl.h>
#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <sys/time.h> // settimeofday, struct timeval
#include <unistd.h>

#include <log/log.h>

#include "rtc.h" // rtc_rd2tv

static const char *RTC_DEV = "/dev/rtc";

static void rd2rt(const struct rtc_date *rd, struct rtc_time *rt) {
    rt->tm_year = rd->year - 1900;
    rt->tm_mon = rd->month - 1;
    rt->tm_mday = rd->day;
    rt->tm_hour = rd->hour;
    rt->tm_min = rd->min;
    rt->tm_sec = rd->sec;
}

static void rt2rd(const struct rtc_time *rt, struct rtc_date *rd) {
    rd->year = rt->tm_year + 1900;
    rd->month = rt->tm_mon + 1;
    rd->day = rt->tm_mday;
    rd->hour = rt->tm_hour;
    rd->min = rt->tm_min;
    rd->sec = rt->tm_sec;
}

void rtc_hw_read(struct rtc_date *rd) {
    int fd = open(RTC_DEV, O_RDONLY);
    if (fd >= 0) {
        struct rtc_time rt;
        if (ioctl(fd, RTC_RD_TIME, &rt) == 0) {
            rt2rd(&rt, rd);
        } else {
            LOGE("ioctl(%d,RTC_RD_TIME,rt) failed with errno(%d)", fd, errno);
        }
        close(fd);
    } else {
        LOGE("rtc_get_clock failed to open(%s, O_RDONLY)", RTC_DEV);
    }
}

void rtc_hw_write(const struct rtc_date *rd) {
    int fd = open(RTC_DEV, O_WRONLY);
    if (fd >= 0) {
        struct rtc_time rt;
        struct timeval tv;
        rd2rt(rd, &rt);
        rtc_rd2tv(rd, &tv);

        if (settimeofday(&tv, NULL) != 0) {
            LOGE("settimeofday(&tv, NULL) failed with errno(%d)", errno);
        }
        if (ioctl(fd, RTC_SET_TIME, &rt) != 0) {
            LOGE("ioctl(%d,RTC_SET_TIME,&rt) failed with errno(%d)", fd, errno);
        }
        close(fd);
    } else {
        LOGE("rtc_set_clock failed to open(%s, O_WRONLY)", RTC_DEV);
    }
}
