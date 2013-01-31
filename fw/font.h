/*
 * font.h
 *
 *  Created on: 31.01.2013
 *      Author: dojoe
 */

#ifndef FONT_H_
#define FONT_H_

#include <avr/pgmspace.h>

#define DP 0x0003

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

#endif /* FONT_H_ */
