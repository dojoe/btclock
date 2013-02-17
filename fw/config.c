/*
 * config.c
 *
 *  Created on: 13.02.2013
 *      Author: dojoe
 */

#include "btclock.h"
#include <avr/eeprom.h>

static EEMEM struct {
	uint16_t blank_time_start, blank_time_end;
	struct sequence_entry sequence[MAX_SEQUENCE];
	char lines[TEXT_MAX][NUM_LINES];
} config = {0};

struct sequence_entry sequence[MAX_SEQUENCE];
uint8_t countdown;

static uint8_t sequence_ptr;

void config_init()
{
	eeprom_read_block(sequence, config.sequence, sizeof(config.sequence));
	blank_time_start = eeprom_read_word(&config.blank_time_start);
	blank_time_end = eeprom_read_word(&config.blank_time_end);
	memset(text_line, 0, sizeof(text_line));
	sequence_ptr = 0;
	next_line();
}

void next_line()
{
	uint8_t which = sequence[sequence_ptr].which;
	countdown = sequence[sequence_ptr].duration;

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
		display_mode = STATIC;
		eeprom_read_block(text_line + 3, config.lines[which - 2], TEXT_MAX);
		text_line_offset = 0;
		text_line_length = strlen(text_line + 3);
		display_mode = TEXT;
	}

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

void set_line(uint8_t index, char *buf, uint8_t length)
{
	memset(buf + length, 0, TEXT_MAX - length);
	eeprom_write_block(buf, config.lines[index], TEXT_MAX);
}

void get_line(uint8_t index, char *buf)
{
	eeprom_read_block(buf, config.lines[index], TEXT_MAX);
}

void save_blank_times()
{
	eeprom_write_word(&config.blank_time_start, blank_time_start);
	eeprom_write_word(&config.blank_time_end, blank_time_end);
}
