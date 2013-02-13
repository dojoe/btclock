/*
 * btclock.h
 *
 *  Created on: 13.02.2013
 *      Author: dojoe
 */

#ifndef BTCLOCK_H_
#define BTCLOCK_H_

#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "pins.h"
#include "uart.h"

/* bt.c */

void bt_init();
void bt_new_pin();
void bt_led_on();
void bt_led_off();

/* cmd.c */

void cmd_poll();

/* font.c */

#define DP 0x0001 /* Only switch on one channel for DP */

#define FONTLOW '0'
#define FONTHIGH 'Z'
#define FONTSIZE (FONTHIGH - FONTLOW + 1)

extern const PROGMEM uint16_t font[FONTSIZE];

static inline uint16_t font_get_char(char c)
{
	if (c >= FONTLOW && c <= FONTHIGH)
		return pgm_read_word(font + (c - FONTLOW));
	else if (c >= 'a' && c <= 'z')
		return pgm_read_word(font + (c - ('a' - 'A') - FONTLOW));
	else
		return 0;
}

static inline uint16_t font_get_digit(uint8_t value)
{
	return pgm_read_word(font + value);
}

/* rtc.c */

struct time {
	uint8_t year, month, day, hour, minute, second;
};

extern volatile uint8_t update_display_from_rtc;
extern volatile struct time time;
extern volatile uint8_t tick;

void rtc_init();
void rtc_get_time();
void rtc_set_time(uint8_t year, uint8_t month, uint8_t day,
		uint8_t hours, uint8_t minutes, uint8_t seconds);

/* spi.c */

uint8_t spi_xfer(uint8_t data_out);

/* tlc.c */

extern uint16_t display[4];

void tlc_init();
void tlc_clear();

#endif /* BTCLOCK_H_ */