/*
 * config.c
 *
 *  Created on: 13.02.2013
 *      Author: dojoe
 */

#include "btclock.h"
#include <avr/eeprom.h>

static EEMEM struct {
	struct sequence_entry sequence[MAX_SEQUENCE];
	char lines[TEXT_MAX][NUM_LINES];
} config = {{{0}}};

struct sequence_entry sequence[MAX_SEQUENCE];
uint8_t countdown;

static uint8_t sequence_ptr;

void config_init()
{
	eeprom_read_block(sequence, config.sequence, sizeof(config.sequence));
	sequence_ptr = 0;
	next_line();
}

void next_line()
{
	uint8_t which = sequence[sequence_ptr].which;
	countdown = sequence[sequence_ptr].duration;

	if (0 == which)
	{
		update_display_from_rtc = 1;
	}
	else
	{
		eeprom_read_block(text_line, config.lines[which - 1], TEXT_MAX);
		text_line_offset = 0;
		update_display_from_rtc = 0;
	}

	sequence_ptr++;
	if (MAX_SEQUENCE == sequence_ptr || 0 == sequence[sequence_ptr].duration)
		sequence_ptr = 0;
}

void save_sequence()
{
	eeprom_write_block(sequence, config.sequence, sizeof(sequence));
}

void set_line(uint8_t index, char *buf, uint8_t length)
{
	memset(buf + length, 0, TEXT_MAX - length);
	eeprom_write_block(buf, config.lines[index], TEXT_MAX);
}

void get_line(uint8_t index, char *buf)
{
	eeprom_read_block(buf, config.lines[index], TEXT_MAX);
}
