/*
 * tlc.c
 *
 *  Created on: 31.01.2013
 *      Author: dojoe
 */

#include "btclock.h"

uint16_t display[4];
static uint8_t int_counter;

volatile uint8_t text_line_offset;
char text_line[TEXT_MAX + 3];

ISR(TIMER0_COMPA_vect)
{
	/* prepare new values for TLC register and segment enables */
	uint8_t cur_segment = int_counter & 3;
	uint16_t bits = display[cur_segment];
	uint8_t segport_clear = SEGPORT | SEGMASK;
	uint8_t new_segport = (segport_clear & ~SEGMASK) | (~((8 << SEGBASE) >> cur_segment) & SEGMASK);

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

	/* next time, next segment */
	int_counter++;
	if (0 == (int_counter & 0x7F) && !update_display_from_rtc)
	{
		uint8_t offset = text_line_offset++;
		uint8_t i;
		if (offset < 3)
			offset = 3;
		offset -= 3;
		if (0 == text_line[offset])
			text_line_offset = offset = 0;
		for (i = 0; i < 4; i++)
		{
			display[i] = font_get_char(text_line[offset++]);
			if ('.' == text_line[offset])
			{
				display[i] |= DP;
				offset++;
			}
		}
	}
}

void tlc_init()
{
	int_counter = 0;
	tlc_clear();

	/* Set up T/C 0 to run at fCLK/64 and count to 125,
	 * giving us interrupts at 500Hz */
	OCR0A = 124;
	TCNT0 = 0;
	TCCR0A = 2 << WGM00;
	TIMSK = 1 << OCIE0A;
	TIFR = 1 << OCF0A;
	TCCR0B = 3 << CS00;
}

void tlc_clear()
{
//	display[0] = display[1] = display[2] = display[3] = 0;
	memset(display, 0, sizeof(display));
}
