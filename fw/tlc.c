/*
 * tlc.c
 *
 *  Created on: 31.01.2013
 *      Author: dojoe
 */

#include "btclock.h"

uint16_t random_number = 0xACE1;

uint16_t display[4];
static uint8_t int_counter;

uint8_t blank;

volatile uint8_t text_line_offset;
uint8_t text_line_length;
char text_line[TEXT_MAX + 8];

ISR(TIMER0_COMPA_vect)
{
#ifdef BOARD_SEGS_ARE_ACTIVE_LOW
	uint8_t segport_clear = SEGPORT | SEGMASK;
#else
	uint8_t segport_clear = SEGPORT & ~SEGMASK;
#endif

	if (blank && BTPIN != display_mode)
	{
		LATCHPORT |= 1 << BLANK;
		SEGPORT = segport_clear;
	}
	else
	{
		/* prepare new values for TLC register and segment enables */
		uint8_t cur_segment = int_counter & 3;
		uint16_t bits = display[cur_segment];
#ifdef BOARD_SEGS_ARE_ACTIVE_LOW
		uint8_t new_segport = (segport_clear & ~SEGMASK) | (~((8 << SEGBASE) >> cur_segment) & SEGMASK);
#else
		uint8_t new_segport = segport_clear | ((8 << SEGBASE) >> cur_segment);
#endif

		/* Clear the segment drivers early to account for slow recovery time */
		SEGPORT = segport_clear;

		/* shift new value into TLC */
		spi_xfer(bits >> 8);
		spi_xfer(bits & 0xFF);

		/* blank, latch, switch segment, unblank */
		LATCHPORT |= 1 << BLANK;
		LATCHPORT |= 1 << LATCH;
		SEGPORT = new_segport;
		/* Resetting the bits in two lines generates two instructions (cbi, cbi),
		 * whereas doing it in one line generates three: in, andi, out
		 */
		LATCHPORT &= ~(1 << LATCH);
		LATCHPORT &= ~(1 << BLANK);
	}

	/* next time, next segment */
	int_counter++;

	if (0 == (int_counter & 0x1F))
	{
		/* update random value */
		uint8_t rand_low = random_number;
		rand_low = ((rand_low) ^ (rand_low >> 2) ^ (rand_low >> 3) ^ (rand_low >> 5)) & 1;
		random_number = random_number >> 1 | rand_low << 15;
	}

	if ((0 == (int_counter & 0x7F)) && (TEXT_1 == display_mode || TEXT_2 == display_mode))
	{
		uint8_t offset, i;
		if (text_line_length > 4)
		{
			offset = text_line_offset++;
			if (offset >= text_line_length + 2)
				text_line_offset = 0;

			if ((offset < 3) && (TEXT_1 == display_mode))
				offset = 3;
			else if ((offset >= text_line_length) && (TEXT_1 == display_mode))
				offset = text_line_length - 1;
		}
		else
		{
			offset = 3;
		}
		for (i = 0; i < 4; i++)
			display[i] = font_get_char(text_line[offset++]);
	}
}

void tlc_init()
{
	/* Set up T/C 0 to run at fCLK/64 and count to 125,
	 * giving us interrupts at 500Hz */
	OCR0A = 124;
	TCNT0 = 0;
	TCCR0A = 2 << WGM00;
	TIMSK = 1 << OCIE0A;
	TIFR = 1 << OCF0A;
	TCCR0B = 3 << CS00;
}

void tlc_show_time()
{
	display[0] = font_get_digit(time.hour >> 4);
	uint16_t seg1 = font_get_digit(time.hour & 0xF);
	if (time.second & 1)
		seg1 |= DP;
	display[1] = seg1;
	display[2] = font_get_digit(time.minute >> 4);
	display[3] = font_get_digit(time.minute & 0xF);
}

void tlc_show_date()
{
	display[0] = font_get_digit(time.day >> 4);
	display[1] = font_get_digit(time.day & 0xF) | DP;
	display[2] = font_get_digit(time.month >> 4);
	display[3] = font_get_digit(time.month & 0xF) | DP;
}
