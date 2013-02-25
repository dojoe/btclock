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

#define _STRINGIFY(s) #s
#define STRINGIFY(x) _STRINGIFY(x)

/* bt.c */

void bt_init();
void bt_new_pin();
void bt_led_on();
void bt_led_off();

/* cmd.c */

void cmd_poll();

/* config.c */

#include "config.h"

/* font.c */

#define DP 0x0001 /* Only switch on one channel for DP */

uint16_t font_get_char(char c);
uint16_t font_get_digit(uint8_t value);

/* main.c */

enum display_mode_e { STATIC, TIME, DATE, TEXT_1, TEXT_2, BTPIN };
extern volatile enum display_mode_e display_mode;

/* rtc.c */

struct time {
	uint8_t year, month, day, hour, minute, second, weekday;
};

extern volatile struct time time;
extern volatile uint8_t tick;

void rtc_init();
void rtc_get_time();
void rtc_set_time(uint8_t year, uint8_t month, uint8_t day,
		uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t weekday);

/* spi.c */

uint8_t spi_xfer(uint8_t data_out);

/* tlc.c */

extern uint8_t  blank;

extern uint16_t random_number;

extern uint16_t display[4];

extern uint8_t text_line_length;
extern volatile uint8_t text_line_offset;
extern char text_line[TEXT_MAX + 8];

void tlc_init();
void tlc_show_time();
void tlc_show_date();

#endif /* BTCLOCK_H_ */
