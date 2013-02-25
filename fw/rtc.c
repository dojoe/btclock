/*
 * rtc.c
 *
 *  Created on: 30.01.2013
 *      Author: dojoe
 */

#include "btclock.h"

volatile struct time time;
volatile uint8_t tick;

#define RTC_WRITE_CMD(address) (0x10 | address)
#define RTC_READ_CMD(address) (0x90 | address)

/* Disable interrupts while fiddling with the RTC because
 * the int-driven TLC driver uses SPI, too. */
#define RTC_BEGIN() { cli(); RTCCEPORT |= 1 << RTCCE; }
#define RTC_END() { RTCCEPORT &= ~(1 << RTCCE); sei(); }

static const PROGMEM char jocki[] = "Jocki hat Geburtstag";

ISR(INT0_vect)
{
	rtc_get_time();

	if (!time.minute && !time.second && 0x03 == time.month && 0x19 == time.day)
	{
		memset(text_line, 0, sizeof(text_line));
		memcpy_P(text_line + 3, jocki, sizeof(jocki));
		text_line_length = strlen(text_line + 3);
		text_line_offset = 0;
		display_mode = TEXT_2;
		countdown = 60;
		cur_line = SEQ_NOTHING;
	}
	else if (TIME == display_mode)
	{
		tlc_show_time();
	}
	else if (DATE == display_mode)
	{
		tlc_show_date();
	}

	tick = 1;
}

void rtc_init()
{
	/* check whether the RTC oscillator has run through */
	RTC_BEGIN();
	spi_xfer(RTC_READ_CMD(2));
	uint8_t tmp = spi_xfer(0);
	RTC_END();

	if (tmp & 0x80)
	{
		/* oscillator has been interrupted: reset RTC and time */

		/* reset RTC by writing magic reset value to CR1 */
		RTC_BEGIN();
		spi_xfer(RTC_WRITE_CMD(0));
		spi_xfer(0x58);
		RTC_END();

		_delay_ms(100);

		/* Set CLKOUT pin to output 1Hz clock; all other reset values are OK */
		RTC_BEGIN();
		spi_xfer(RTC_WRITE_CMD(0xE));
		spi_xfer(0x63);
		RTC_END();

		/* Clear time values */
		rtc_set_time(0x13, 0x01, 0x01, 0x00, 0x00, 0x00);
	}
	else
	{
		/* read full time once */
		rtc_get_time();
	}

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

	/* read what we just wrote */
	rtc_get_time();
}

void rtc_get_time()
{
	RTC_BEGIN();
	spi_xfer(RTC_READ_CMD(2));
	time.second = spi_xfer(0) & 0x7F;
	time.minute = spi_xfer(0) & 0x7F;
	time.hour = spi_xfer(0) & 0x3F;
	time.day = spi_xfer(0) & 0x3F;
	spi_xfer(0); /* skip weekday */
	time.month = spi_xfer(0) & 0x1F;
	time.year = spi_xfer(0);
	RTC_END();
}
