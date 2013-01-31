/*
 * rtc.h
 *
 *  Created on: 30.01.2013
 *      Author: dojoe
 */

#ifndef RTC_H_
#define RTC_H_

struct time {
	uint8_t year, month, day, hour, minute, second;
};

extern volatile uint8_t update_display_from_rtc;
extern volatile struct time time;

void rtc_init();
void rtc_set_time(uint8_t year, uint8_t month, uint8_t day,
		uint8_t hours, uint8_t minutes, uint8_t seconds);

#endif /* RTC_H_ */
