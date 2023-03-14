#ifndef _RTC_H
#define _RTC_H

/**
 *  RTC Driver Interface.
 *
 *  Provides management interfaces for accessing the
 *  Real-Time-Clock. All date/time representations
 *  are performed in UTC.
 */

#include <sys/time.h>

/**
 *  Structure representing the Date/Time of the RTC clock.
 */
struct rtc_date {
    unsigned short year; // YYYY
    unsigned char month; // 1 - 12
    unsigned char day;   // 1 - 31
    unsigned char hour;  // 0 - 23
    unsigned char min;   // 0 - 59
    unsigned char sec;   // 0 - 59
};

/**
 *  Return 0 when battery detected, otherwise -ENODEV (-19).
 */
int rtc_has_battery();

/**
 *  Conversion Utils used to interact with system data types.
 */
void rtc_rd2tv(const struct rtc_date *rd, struct timeval *tv);
void rtc_tv2rd(const struct timeval *tv, struct rtc_date *rd);

/**
 *  Initialize RTC and OS Clock from file if detected,
 *  contents of file must match the following UTC format:
 *  YYYY-MM-DDTHH_MM_SS
 *  2023-03-10T01_05_15
 */
void rtc_init();

/**
 *  Log RTC date structure in a standard format
 */
void rtc_timestamp();

/**
 *  Set Hardware Clock and synchronize OS Clock in UTC.
 */
void rtc_set_clock(const struct rtc_date *rd);

/**
 *  Get Hardware Clock in UTC.
 */
void rtc_get_clock(struct rtc_date *rd);

/**
 *  Formats buffer to a Filesystem safe UTC string.
 *  Returns the number of characters written.
 */
int rtc_get_clock_log_str(char *buffer, int size);

/**
 *  Formats buffer to an OSD pretty UTC string.
 *  Returns the number of characters written.
 */
int rtc_get_clock_osd_str(char *buffer, int size);

#endif
