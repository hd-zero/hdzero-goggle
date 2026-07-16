/**
 *  RTC Driver Interface.
 *
 *  Provides management interfaces for accessing the
 *  Real-Time-Clock. All date/time representations
 *  are performed in UTC.
 *
 *  This file is the shared, hardware-agnostic half: date math, validation and
 *  OSD/log formatting, with NO platform or build #ifdefs. The two functions that
 *  actually touch the clock -- rtc_get_clock / rtc_set_clock -- delegate to the
 *  per-target backend in rtc_hw.h (goggle: /dev/rtc; emulator: in-memory).
 */

#include "rtc.h"
#include "rtc_hw.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

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

static const unsigned char RTC_DAYS_PER_MONTH[] = {
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/**
 *  Globals
 */
static int g_rtc_has_battery = 0;

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
 *  Conversion Utils used to interact with system data types.
 */
void rtc_rd2tv(const struct rtc_date *rd, struct timeval *tv) {
    tv->tv_sec = rtc_mktime(rd);
    tv->tv_usec = 0;
}
void rtc_tv2rd(const struct timeval *tv, struct rtc_date *rd) {
    unsigned int month, year;
    unsigned long secs;
    int days = tv->tv_sec / 86400;
    secs = tv->tv_sec - (unsigned int)days * 86400;
    year = 1970 + days / 365;
    days -= (year - 1970) * 365 + LEAPS_THRU_END_OF(year - 1) - LEAPS_THRU_END_OF(1970 - 1);
    if (days < 0) {
        year -= 1;
        days += 365 + rtc_is_leap_year(year);
    }
    for (month = 0; month < 11; month++) {
        int newdays = days - rtc_days_per_month(year, month);
        if (newdays < 0) {
            break;
        }
        days = newdays;
    }
    rd->year = year;
    rd->month = month + 1;
    rd->day = days + 1;
    rd->hour = secs / 3600;
    secs -= (unsigned long)rd->hour * 3600;
    rd->min = secs / 60;
    rd->sec = secs - rd->min * 60;
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
    rtc_hw_write(rd);
}

/**
 *  Get Hardware Clock
 */
void rtc_get_clock(struct rtc_date *rd) {
    rtc_hw_read(rd);
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
