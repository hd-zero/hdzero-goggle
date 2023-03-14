#ifndef _RTC_H
#define _RTC_H

struct rtc_date {
    unsigned short year; // YYYY
    unsigned char month; // 1 - 12
    unsigned char day;   // 1 - 31
    unsigned char hour;  // 0 - 23
    unsigned char min;   // 0 - 59
    unsigned char sec;   // 0 - 59
};

void rtc_init_old();
void rtc_init();
int rtc_has_battery();
void rtc_timestamp();
void rtc_set_clock(const struct rtc_date *rd);
void rtc_get_clock(struct rtc_date *rd);
int rtc_get_clock_iso_str(char *buffer, int size);
int rtc_get_clock_osd_str(char *buffer, int size);

#endif
