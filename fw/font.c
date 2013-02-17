/*
 * font.c
 *
 *  Created on: 31.01.2013
 *      Author: dojoe
 */

#include "btclock.h"

#define FONTLOW '0'
#define FONTHIGH 'Z'
#define FONTSIZE (FONTHIGH - FONTLOW + 1)

static const PROGMEM uint16_t font[FONTSIZE];

uint16_t font_get_char(char c)
{
	if (c >= FONTLOW && c <= FONTHIGH)
		return pgm_read_word(font + (c - FONTLOW));
	else if (c >= 'a' && c <= 'z')
		return pgm_read_word(font + (c - ('a' - 'A') - FONTLOW));
	else
		return 0;
}

uint16_t font_get_digit(uint8_t value)
{
	return pgm_read_word(font + value);
}

#define a 0x3000
#define b 0xC000
#define c 0x000C
#define d 0x0030
#define e 0x00C0
#define f 0x0C00
#define g 0x0300

static const PROGMEM uint16_t font[FONTSIZE] = {
		/* 0 */ a | b | c | d | e | f,
		/* 1 */ b | c,
		/* 2 */ a | b | g | e | d,
		/* 3 */ a | b | g | c | d,
		/* 4 */ f | g | b | c,
		/* 5 */ a | f | g | c | d,
		/* 6 */ a | f | g | c | d | e,
		/* 7 */ a | b | c,
		/* 8 */ a | b | c | d | e | f | g,
		/* 9 */ a | b | c | d | f | g,
		/* : */ 0,
		/* ; */ 0,
		/* < */ a | d | e | f,
		/* = */ g,
		/* > */ 0,
		/* ? */ 0,
		/* @ */ a | b | f | g,
		/* A */ a | b | c | e | f | g,
		/* B */ c | d | e | f | g,
		/* C */ d | e | g,
		/* D */ b | c | d | e | g,
		/* E */ a | d | e | f | g,
		/* F */ a | e | f | g,
		/* G */ a | c | d | e | f,
		/* H */ c | e | f | g,
		/* I */ b | c,
		/* J */ b | c | d | e,
		/* K */ b | c | e | f | g,
		/* L */ d | e | f,
		/* M */ a | b | c | e | f,
		/* N */ c | e | g,
		/* O */ c | d | e | g,
		/* P */ a | b | e | f | g,
		/* Q */ a | b | c | f | g,
		/* R */ e | g,
		/* S */ a | c | d | f | g,
		/* T */ d | e | f | g,
		/* U */ c | d | e,
		/* V */ c | d | e,
		/* W */ b | c | d | e | f,
		/* X */ b | c | e | f | g,
		/* Y */ b | e | f | g,
		/* Z */ a | b | d | e | g
};
