#include "rtc.h"

#include <errno.h>
#include <fcntl.h>
#include <linux/rtc.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <unistd.h>

#include <log/log.h>

#include "util/file.h"

/**
 *  Constants
 */
static const char *RTC_FILE = "/mnt/extsd/rtc.txt";

/**
 *  Conversion Utils
 */
void rd2rt(const struct rtc_date *rd, struct rtc_time *rt) {
    rt->tm_year = rd->year - 1900;
    rt->tm_mon = rd->month - 1;
    rt->tm_mday = rd->day;
    rt->tm_hour = rd->hour;
    rt->tm_min = rd->min;
    rt->tm_sec = rd->sec;
}
void rt2rd(const struct rtc_time *rt, struct rtc_date *rd) {
    rd->year = rt->tm_year + 1900;
    rd->month = rt->tm_mon + 1;
    rd->day = rt->tm_mday;
    rd->hour = rt->tm_hour;
    rd->min = rt->tm_min;
    rd->sec = rt->tm_sec;
}

/**
 *  Initialize RTC from file if detected,
 *  must be formatted to ISO-8601:
 *  YYYY-MM-DDTHH:MM:SS
 *  2023-03-10T01:05:15
 */
void rtc_init() {
    if (file_exists(RTC_FILE)) {
        FILE *fp = fopen(RTC_FILE, "r");
        if (fp) {
            static char buffer[32];
            while (fgets(buffer, sizeof(buffer), fp)) {
                struct rtc_date rd, rd_now;
                sscanf(buffer,
                       "%04d-%02d-%02dT%02d:%02d:%02d",
                       &rd.year,
                       &rd.month,
                       &rd.day,
                       &rd.hour,
                       &rd.min,
                       &rd.sec);

                // Wishes to reset the clock
                if (rd.year == 1970) {
                    rtc_set_clock(&rd);
                } else {
                    rtc_get_clock(&rd);

                    // Update if file contains a later date
                    if (rd_now.year < rd.year ||
                        rd_now.month < rd.month ||
                        rd_now.day < rd.day ||
                        rd_now.hour < rd.hour ||
                        rd_now.min < rd.min ||
                        rd_now.sec < rd.sec) {
                        rtc_set_clock(&rd);
                    }
                }
            }
            fclose(fp);
        }
    }

    rtc_timestamp();
}

/**
 *  Format RTC in a standard format
 */
static inline void rtc_print(const struct rtc_date *rd) {
    LOGI("RTC: %04d-%02d-%02dT%02d:%02d:%02d",
         rd->year,
         rd->month,
         rd->day,
         rd->hour,
         rd->min,
         rd->sec);
}

/**
 *  Log RTC in a standard format
 */
void rtc_timestamp() {
    struct rtc_date rd;
    rtc_get_clock(&rd);
    rtc_print(&rd);
}

/**
 *  Set Hardware Clock
 */
void rtc_set_clock(const struct rtc_date *rd) {
    int fd = open("/dev/rtc", O_WRONLY);
    if (fd != -1) {
        struct rtc_time rt;
        rd2rt(rd, &rt);
        if (ioctl(fd, RTC_SET_TIME, &rt) != 0) {
            LOGE("ioctl(%d,RTC_SET_TIME,rt) failed with errno(%d)", fd, errno);
        }
        close(fd);
    } else {
        LOGE("Failed to open(/dev/rtc, O_WRONLY)");
    }
}

/**
 *  Get Hardware Clock
 */
void rtc_get_clock(struct rtc_date *rd) {
    int fd = open("/dev/rtc", O_RDONLY);
    if (fd != -1) {
        struct rtc_time rt;
        if (ioctl(fd, RTC_RD_TIME, &rt) == 0) {
            rt2rd(&rt, rd);
        } else {
            LOGE("ioctl(%d,RTC_RD_TIME,rt) failed with errno(%d)", fd, errno);
        }
        close(fd);
    } else {
        LOGE("Failed to open(/dev/rtc, O_RDONLY)");
    }
}
