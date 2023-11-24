/**
 *  RTC Driver Interface.
 *
 *  Provides management interfaces for accessing the
 *  Real-Time-Clock. All date/time representations
 *  are performed in UTC.
 */

#include "rtc.h"

#include <errno.h>
#include <fcntl.h>
#include <linux/rtc.h>
#include <memory.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>

#include <log/log.h>
#include <minIni.h>

#include "core/settings.h"
#include "ui/page_common.h"

/**
 *  Constants
 */
#define RTC_LOG_FORMAT       "%04u-%02u-%02uT%02u_%02u_%02u"
#define RTC_OSD_FORMAT_DATE  "%04u/%02u/%02u"
#define RTC_OSD_FORMAT_TIME  "%02u:%02u:%02u"
#define LEAPS_THRU_END_OF(y) ((y) / 4 - (y) / 100 + (y) / 400)

static const char *RTC_DEV = "/dev/rtc";
static const unsigned char RTC_DAYS_PER_MONTH[] = {
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/**
 *  Globals
 */
static int g_rtc_has_battery = 0;

#ifdef EMULATOR_BUILD
static struct rtc_date g_rtc_date = {1970, 1, 1, 0, 0, 0};
#endif

/**
 * Convert Gregorian date to seconds since 1970-01-01 00:00:00
 */
static inline uint64_t rtc_mktime(const struct rtc_date *rd) {
    unsigned int month = rd->month, year = rd->year;

    if (0 >= (int)(month -= 2)) {
        month += 12;
        year -= 1;
    }

    return ((((year / 4 - year / 100 + year / 400 + 367 * month / 12 + rd->day) +
              year * 365 - 719499) *
                 24 +
             rd->hour) *
                60 +
            rd->min) *
               60 +
           rd->sec;
}

/**
 *  Returns 1 if leap year is detected, otherwise 0.
 */
static inline int rtc_is_leap_year(unsigned int year) {
    return (!(year % 4) && (year % 100)) || !(year % 400);
}

/*
 * Returns number of days per month, month being (base 0).
 */
int rtc_days_per_month(unsigned int year, unsigned int month) {
    return (rtc_is_leap_year(year) && month == 1) + RTC_DAYS_PER_MONTH[month];
}

/*
 * Returns 0 if rtc_date is valid, otherwise -EINVAL (-22).
 */
int rtc_has_valid_date(struct rtc_date *rd) {
    return (rd->year - 1900 < 70 ||
            rd->month - 1 >= 12 ||
            rd->day > rtc_days_per_month(rd->year, rd->month - 1) ||
            rd->hour >= 24 ||
            rd->min >= 60 ||
            rd->sec >= 60)
               ? -EINVAL
               : 0;
}

/**
 *  Return 0 when battery detected, otherwise -ENODEV (-19).
 */
int rtc_has_battery() {
    return g_rtc_has_battery ? 0 : -ENODEV;
}

/**
 *  Log RTC date structure in a standard format
 */
void rtc_print(const struct rtc_date *rd) {
    LOGI(RTC_LOG_FORMAT,
         rd->year,
         rd->month,
         rd->day,
         rd->hour,
         rd->min,
         rd->sec);
}

/**
 *  Conversion Utils used to interact with internal data types.
 */
void rtc_rd2rt(const struct rtc_date *rd, struct rtc_time *rt) {
    rt->tm_year = rd->year - 1900;
    rt->tm_mon = rd->month - 1;
    rt->tm_mday = rd->day;
    rt->tm_hour = rd->hour;
    rt->tm_min = rd->min;
    rt->tm_sec = rd->sec;
}
void rtc_rt2rd(const struct rtc_time *rt, struct rtc_date *rd) {
    rd->year = rt->tm_year + 1900;
    rd->month = rt->tm_mon + 1;
    rd->day = rt->tm_mday;
    rd->hour = rt->tm_hour;
    rd->min = rt->tm_min;
    rd->sec = rt->tm_sec;
}

/**
 *  Conversion Utils used to interact with system data types.
 */
void rtc_rd2tv(const struct rtc_date *rd, struct timeval *tv) {
    tv->tv_sec = rtc_mktime(rd);
    tv->tv_usec = 0;
}
void rtc_tv2rd(const struct timeval *tv, struct rtc_date *rd) {
    struct rtc_time rt;
    unsigned int month, year;
    unsigned long secs;
    int days = tv->tv_sec / 86400;
    secs = tv->tv_sec - (unsigned int)days * 86400;
    rt.tm_wday = (days + 4) % 7;
    year = 1970 + days / 365;
    days -= (year - 1970) * 365 + LEAPS_THRU_END_OF(year - 1) - LEAPS_THRU_END_OF(1970 - 1);
    if (days < 0) {
        year -= 1;
        days += 365 + rtc_is_leap_year(year);
    }
    rt.tm_year = year - 1900;
    rt.tm_yday = days + 1;
    for (month = 0; month < 11; month++) {
        int newdays;

        newdays = days - rtc_days_per_month(year, month);
        if (newdays < 0) {
            break;
        }
        days = newdays;
    }
    rt.tm_mon = month;
    rt.tm_mday = days + 1;
    rt.tm_hour = secs / 3600;
    secs -= rt.tm_hour * 3600;
    rt.tm_min = secs / 60;
    rt.tm_sec = secs - rt.tm_min * 60;
    rt.tm_isdst = 0;
    rtc_rt2rd(&rt, rd);
}

/**
 *  Initialize both the hardware and system clocks.
 */
void rtc_init() {
    struct rtc_date rd;
    rtc_get_clock(&rd);

    // Has time accumulated since the
    // installation of the battery?
    g_rtc_has_battery = rd.year > 1970;

    LOGI("rtc_init %s detected a battery",
         (g_rtc_has_battery ? "has" : "has NOT"));

    if (!g_rtc_has_battery) {
        g_setting.record.naming = SETTING_NAMING_CONTIGUOUS;
        ini_putl("record", "naming", g_setting.record.naming, SETTING_INI);
    }

    if (rd.year == 1970) {
        LOGI("rtc_init updating both clocks via settings");
        rd.year = g_setting.clock.year;
        rd.month = g_setting.clock.month;
        rd.day = g_setting.clock.day;
        rd.hour = g_setting.clock.hour;
        rd.min = g_setting.clock.min;
        rd.sec = g_setting.clock.sec;
        rtc_set_clock(&rd);
    }
}

/**
 *  Log current time to disk.
 */
void rtc_timestamp() {
    struct rtc_date rd;
    rtc_get_clock(&rd);
    rtc_print(&rd);
}

/**
 *  Set Hardware Clock and synchronize OS Clock in UTC.
 */
void rtc_set_clock(const struct rtc_date *rd) {
#ifdef EMULATOR_BUILD
    g_rtc_date = *rd;
#else
    int fd = open(RTC_DEV, O_WRONLY);
    if (fd >= 0) {
        struct rtc_time rt;
        struct timeval tv;
        rtc_rd2rt(rd, &rt);
        rtc_rd2tv(rd, &tv);

        if (settimeofday(&tv, NULL) != 0) {
            LOGE("settimeofday(&tv, NULL) failed with errno(%d)", errno);
        }
        if (ioctl(fd, RTC_SET_TIME, &rt) != 0) {
            LOGE("ioctl(%d,RTC_SET_TIME,&rt) failed with errno(%d)", fd, errno);
        }
        close(fd);
    } else {
        LOGE("rtc_set_clock failed to open(%s, O_RDWR)", RTC_DEV);
    }
#endif
}

/**
 *  Get Hardware Clock
 */
void rtc_get_clock(struct rtc_date *rd) {
#ifdef EMULATOR_BUILD
    *rd = g_rtc_date;
#else
    int fd = open(RTC_DEV, O_RDONLY);
    if (fd >= 0) {
        struct rtc_time rt;
        if (ioctl(fd, RTC_RD_TIME, &rt) == 0) {
            rtc_rt2rd(&rt, rd);
        } else {
            LOGE("ioctl(%d,RTC_RD_TIME,rt) failed with errno(%d)", fd, errno);
        }
        close(fd);
    } else {
        LOGE("rtc_get_clock failed to open(%s, O_RDWR)", RTC_DEV);
    }
#endif
}

/**
 *  Formats buffer to a Filesystem safe UTC string.
 *  Returns the number of characters written.
 */
int rtc_get_clock_log_str(char *buffer, int size) {
    struct rtc_date rd;
    rtc_get_clock(&rd);
    return snprintf(buffer, size, RTC_LOG_FORMAT,
                    rd.year,
                    rd.month,
                    rd.day,
                    rd.hour,
                    rd.min,
                    rd.sec);
}

/**
 *  Updates buffers (Date/Time/Format) for OSD.
 *  Returns total characters written.
 */
int rtc_get_clock_osd_str(char *date, int dsize,
                          char *time, int tsize,
                          char *format, int fsize) {
    int written = 0;
    struct rtc_date rd;
    rtc_get_clock(&rd);

    int hour = rd.hour;
    if (g_setting.clock.format == 0) {
        if (hour > 12) {
            hour -= 12;
        }
        hour = hour == 0 ? 12 : hour;
        written = snprintf(format, fsize, "%s", rd.hour > 11 ? "PM" : "AM");
    } else {
        written = snprintf(format, fsize, "%s", "");
    }

    written += snprintf(date, dsize, RTC_OSD_FORMAT_DATE,
                        rd.year,
                        rd.month,
                        rd.day);

    written += snprintf(time, tsize, RTC_OSD_FORMAT_TIME,
                        hour,
                        rd.min,
                        rd.sec);

    return written;
}
