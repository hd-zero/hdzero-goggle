#ifndef _RTC_H
#define _RTC_H

struct rtc_date {
	int year;  // YYYY
	int month; // 1 - 12
	int day;   // 1 - 31
	int hour;  // 0 - 23
	int min;   // 0 - 59 
	int sec;   // 0 - 59
};

void rtc_init_from_file();
void rtc_timestamp();
void rtc_set_clock(const struct rtc_date* rd);
void rtc_get_clock(struct rtc_date* rd);

#endif
