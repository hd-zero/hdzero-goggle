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
#include <stdio.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>

#include <log/log.h>

#include "util/file.h"

/**
 *  Constants
 */
#define RTC_LOG_FORMAT "%04hu-%02u-%02uT%02u_%02u_%02u"
#define RTC_OSD_FORMAT "%04hu/%02u/%02u %02u:%02u:%02u"
static const char *RTC_DEV = "/dev/rtc";
static const char *RTC_FILE = "/mnt/extsd/rtc.txt";
static const unsigned char RTC_DAYS_PER_MONTH[] = {
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/**
 *  Globals
 */
static int g_rtc_has_battery = 0;

/**
 *  Returns 1 if leap year is detected, otherwise 0.
 */
static inline int rtc_is_leap_year(unsigned int year) {
    return (!(year % 4) && (year % 100)) || !(year % 400);
}

/*
 * The number of days per month.
 */
static inline int rtc_days_per_month(unsigned int year, unsigned int month) {
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
    struct rtc_time rt;
    rtc_rd2rt(rd, &rt);
    tv->tv_sec = mktime((struct tm *)&rt);
    tv->tv_usec = 0;
}
void rtc_tv2rd(const struct timeval *tv, struct rtc_date *rd) {
    struct rtc_time *rt = (struct rtc_time *)gmtime(&tv->tv_sec);
    rtc_rt2rd(rt, rd);
}

/**
 *  Initialize RTC and OS Clock from file if detected,
 *  contents of file must match the following:
 *  YYYY-MM-DDTHH_MM_SS
 *  2023-03-10T01_05_15
 */
void rtc_init() {
    struct rtc_date rd_now;
    rtc_get_clock(&rd_now);

    // Has time has accumulated since the
    // the installation of the battery?
    g_rtc_has_battery =
        !(rd_now.year == 1970 &&
          rd_now.month == 1 &&
          rd_now.day == 1 &&
          rd_now.hour == 0 &&
          rd_now.min == 0);

    LOGI("rtc_init %s detected a battery",
         (g_rtc_has_battery ? "has" : "has NOT"));

    // Load file and set RTC if found.
    if (file_exists(RTC_FILE)) {
        FILE *fp = fopen(RTC_FILE, "r");
        if (fp) {
            static char buffer[32];

            while (fgets(buffer, sizeof(buffer), fp)) {
                struct rtc_date rd_file;
                sscanf(buffer, RTC_LOG_FORMAT,
                       &rd_file.year,
                       &rd_file.month,
                       &rd_file.day,
                       &rd_file.hour,
                       &rd_file.min,
                       &rd_file.sec);

                if (rtc_has_valid_date(&rd_file) == 0) {
                    // Wishes to reset the clock
                    if (rd_file.year == 1970) {
                        LOGI("rtc_init is resetting year back to 1970");
                        rtc_set_clock(&rd_file);
                    } else {
                        // Update if file contains a later date
                        if (rd_now.year < rd_file.year ||
                            rd_now.month < rd_file.month ||
                            rd_now.day < rd_file.day ||
                            rd_now.hour < rd_file.hour ||
                            rd_now.min < rd_file.min ||
                            rd_now.sec < rd_file.sec) {
                            LOGI("rtc_init is updating clock to a later date");
                            rtc_set_clock(&rd_file);
                        }
                    }
                } else {
                    LOGE("rtc_init detected an invalid date within %s", RTC_FILE);
                }
            }
            fclose(fp);
        }
    }

    rtc_timestamp();
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
}

/**
 *  Get Hardware Clock
 */
void rtc_get_clock(struct rtc_date *rd) {
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
 *  Formats buffer to an OSD pretty UTC string.
 *  Returns the number of characters written.
 */
int rtc_get_clock_osd_str(char *buffer, int size) {
    struct rtc_date rd;
    rtc_get_clock(&rd);
    return snprintf(buffer, size, RTC_OSD_FORMAT,
                    rd.year,
                    rd.month,
                    rd.day,
                    rd.hour,
                    rd.min,
                    rd.sec);
}
