/*
 * config.c
 *
 *  Created on: 13.02.2013
 *      Author: dojoe
 */

#include "btclock.h"
#include <avr/eeprom.h>

static EEMEM struct {
	struct timespan blank_time;
	struct sequence_entry sequence[MAX_SEQUENCE];
	char lines[TEXT_MAX][NUM_LINES];
	uint8_t line_modes;
} config = {{0}};

struct sequence_entry sequence[MAX_SEQUENCE];

uint8_t countdown;

static uint8_t sequence_ptr;

void config_init()
{
	eeprom_read_block(sequence, config.sequence, sizeof(config.sequence));
	eeprom_read_block(&blank_time, &config.blank_time, sizeof(config.blank_time));
	memset(text_line, 0, sizeof(text_line));
	sequence_ptr = 0;
	next_line();
}

static void set_display(uint8_t which)
{
	if (0 == which)
	{
		display_mode = TIME;
		tlc_show_time();
	}
	else if (1 == which)
	{
		display_mode = DATE;
		tlc_show_date();
	}
	else
	{
		which -= 2;
		display_mode = STATIC;
		eeprom_read_block(text_line + 3, config.lines[which], TEXT_MAX);
		text_line_offset = 0;
		text_line_length = strlen(text_line + 3);
		display_mode = (eeprom_read_byte(&config.line_modes) & (1 << which)) ? TEXT_2 : TEXT_1;
	}
}

void next_line()
{
	uint8_t which = sequence[sequence_ptr].which;
	countdown = sequence[sequence_ptr].duration;

	set_display(which);

	sequence_ptr++;
	if (MAX_SEQUENCE == sequence_ptr || 0 == sequence[sequence_ptr].duration)
		sequence_ptr = 0;
}

void save_sequence()
{
	eeprom_write_block(sequence, config.sequence, sizeof(sequence));
	sequence_ptr = 0;
	countdown = 0;
}

void set_line(uint8_t index, char *buf, uint8_t length, uint8_t mode)
{
	uint8_t line_modes, mode_mask;

	memset(buf + length, 0, TEXT_MAX - length);
	eeprom_write_block(buf, config.lines[index], TEXT_MAX);
	line_modes = eeprom_read_byte(&config.line_modes);
	mode_mask = 1 << index;
	if (mode)
		line_modes |= mode_mask;
	else
		line_modes &= ~mode_mask;
	eeprom_write_byte(&config.line_modes, line_modes);
}

void get_line(uint8_t index, char *buf)
{
	eeprom_read_block(buf, config.lines[index], TEXT_MAX);
}

void save_blank_times()
{
	eeprom_write_block(&blank_time, &config.blank_time, sizeof(config.blank_time));
}
