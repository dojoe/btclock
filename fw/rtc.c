/*
 * rtc.c
 *
 *  Created on: 30.01.2013
 *      Author: dojoe
 */

#include <avr/interrupt.h>
#include "rtc.h"
#include "spi.h"
#include "pins.h"
#include "tlc.h"
#include "font.h"

volatile uint8_t update_display_from_rtc;
volatile struct time time;

#define RTC_WRITE_CMD(address) (0x10 | address)
#define RTC_READ_CMD(address) (0x90 | address)

/* Disable interrupts while fiddling with the RTC because
 * the int-driven TLC driver uses SPI, too. */
#define RTC_BEGIN() { cli(); RTCCEPORT |= 1 << RTCCE; }
#define RTC_END() { RTCCEPORT &= ~(1 << RTCCE); sei(); }

ISR(INT0_vect)
{
	uint8_t hour, minute, second;

	/* Read current time from RTC */
	RTC_BEGIN();
	spi_xfer(RTC_READ_CMD(2));
	time.second = second = spi_xfer(0);
	time.minute = minute = spi_xfer(0);
	time.hour = hour = spi_xfer(0);

	/* only read date on midnight */
	if (!second && !minute && !hour)
	{
		time.day = spi_xfer(0);
		spi_xfer(0); /* skip weekday */
		time.month = spi_xfer(0);
		time.year = spi_xfer(0);
	}
	RTC_END();

	/* if time display enabled, set display from BCD time */
	if (update_display_from_rtc)
	{
		display[0] = font_get_digit(hour >> 4);
		uint16_t seg1 = font_get_digit(hour & 0xF);
		if (second & 1)
			seg1 |= DP;
		display[1] = seg1;
		display[2] = font_get_digit(minute >> 4);
		display[3] = font_get_digit(minute & 0xF);
	}
}

void rtc_init()
{
	/* reset RTC by writing magic reset value to CR1 */
	RTC_BEGIN();
	spi_xfer(RTC_WRITE_CMD(0));
	spi_xfer(0x58);
	RTC_END();

	/* Set CLKOUT pin to output 1Hz clock; all other reset values are OK */
	RTC_BEGIN();
	spi_xfer(RTC_WRITE_CMD(0xE));
	spi_xfer(0x63);
	RTC_END();

	/* Enable 1HZ interrupt _after_ setting up RTC CLKOUT */
	MCUCR |= 3 << ISC00;
	GIMSK |= 1 << INT0;
	EIFR  |= 1 << INT0;
}

void rtc_set_time(uint8_t year, uint8_t month, uint8_t day,
		uint8_t hours, uint8_t minutes, uint8_t seconds)
{
	RTC_BEGIN();
	spi_xfer(RTC_WRITE_CMD(2));
	spi_xfer(seconds);
	spi_xfer(minutes);
	spi_xfer(hours);
	spi_xfer(day);
	spi_xfer(0);  /* weekdays mean nothing to us */
	spi_xfer(month);
	spi_xfer(year);
	RTC_END();
}

void rtc_get_time()
{
	RTC_BEGIN();
	spi_xfer(RTC_READ_CMD(2));
	time.second = spi_xfer(0);
	time.minute = spi_xfer(0);
	time.hour = spi_xfer(0);
	time.day = spi_xfer(0);
	spi_xfer(0); /* skip weekday */
	time.month = spi_xfer(0);
	time.year = spi_xfer(0);
	RTC_END();
}
