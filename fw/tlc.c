/*
 * tlc.c
 *
 *  Created on: 31.01.2013
 *      Author: dojoe
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "pins.h"
#include "spi.h"
#include "tlc.h"

uint16_t display[4];
static uint8_t cur_segment;

ISR(TIMER1_COMPA_vect)
{
	/* prepare new values for TLC register and segment enables */
	uint16_t bits = display[cur_segment];
	uint8_t new_segport = (SEGPORT & ~SEGMASK) | ((8 << SEGBASE) >> cur_segment);

	/* shift new value into TLC */
	spi_xfer(bits >> 8);
	spi_xfer(bits & 0xFF);

	/* blank, latch, switch segment, unblank */
	LATCHPORT |= 1 << BLANK;
	LATCHPORT |= 1 << LATCH;
	SEGPORT = new_segport;
	LATCHPORT &= ~((1 << BLANK) | (1 << LATCH));

	/* next time, next segment */
	cur_segment = (cur_segment + 1) & 3;
}

void tlc_init()
{
	cur_segment = 0;
	tlc_clear();

	/* Set BLANK high before setting pin direction to output
	 * to make sure the TLC stays blanked. */
	LATCHPORT |= 1 << BLANK;
	LATCHDDR |= (1 << LATCH) | (1 << BLANK);

	SEGDDR |= SEGMASK;

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
	display[0] = display[1] = display[2] = display[3] = 0;
}
