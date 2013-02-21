/*
 * config.c
 *
 *  Created on: 13.02.2013
 *      Author: dojoe
 */

#include "btclock.h"
#include <avr/eeprom.h>

static EEMEM struct {
	union {
		struct special specials[NUM_SPECIALS + 1];
		struct {
			struct timespan blank_time;
			uint8_t line_modes;
		};
	};
	struct sequence_entry sequence[MAX_SEQUENCE];
	char lines[NUM_LINES][TEXT_MAX];
} config = { .sequence = {{ SEQ_TIME, 1}}};

struct sequence_entry sequence[MAX_SEQUENCE];

uint8_t countdown;

static uint8_t sequence_ptr;
static uint8_t cur_line = SEQ_NOTHING;

void config_init()
{
	eeprom_read_block(sequence, config.sequence, sizeof(config.sequence));
	next_line();
}

static void set_display(uint8_t which)
{
	if (cur_line == which)
		return;

	if (SEQ_TIME == which)
	{
		display_mode = TIME;
		tlc_show_time();
	}
	else if (SEQ_DATE == which)
	{
		display_mode = DATE;
		tlc_show_date();
	}
	else
	{
		display_mode = STATIC;
		eeprom_read_block(text_line + 3, config.lines[which], TEXT_MAX);
		text_line_offset = 0;
		text_line_length = strlen(text_line + 3);
		display_mode = (eeprom_read_byte(&config.line_modes) & (1 << which)) ? TEXT_2 : TEXT_1;
		cur_line = which;
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

static uint8_t in_timespan(struct timespan *span)
{
	uint16_t cur_time   = time.hour << 8 | time.minute;
	uint16_t span_start = eeprom_read_word(&span->start);
	uint16_t span_end   = eeprom_read_word(&span->end);
	uint8_t after_start = (cur_time >= span_start);
	uint8_t before_end  = (cur_time < span_end);
	if (span_start <= span_end)
	{
		return after_start && before_end;
	}
	else
	{
		return after_start || before_end;
	}
}

void check_timespans()
{
	uint8_t i = NUM_SPECIALS;
	blank = in_timespan(&config.blank_time);
	while (i)
	{
		if (in_timespan(&config.specials[i].when))
		{
			set_display(eeprom_read_byte(&config.specials[i].what));
			countdown = 60; // so we get a normal update the next minute _if_ we're not still in the timespan
		}
		i--;
	}
}

void save_sequence()
{
	eeprom_update_block(sequence, config.sequence, sizeof(sequence));
	sequence_ptr = 0;
	countdown = 0;
}

void set_line(uint8_t index, char *buf, uint8_t length, uint8_t mode)
{
	uint8_t line_modes, mode_mask;

	memset(buf + length, 0, TEXT_MAX - length);
	eeprom_update_block(buf, config.lines[index], TEXT_MAX);

	line_modes = eeprom_read_byte(&config.line_modes);
	mode_mask = 1 << index;
	if (mode)
		line_modes |= mode_mask;
	else
		line_modes &= ~mode_mask;
	eeprom_update_byte(&config.line_modes, line_modes);

	cur_line = SEQ_NOTHING; // make sure we copy the line at next update
}

void get_line(uint8_t index, char *buf)
{
	eeprom_read_block(buf, config.lines[index], TEXT_MAX);
}

void set_special_time(uint8_t index, struct timespan *span, uint8_t what)
{
	eeprom_update_block(span, &config.specials[index].when, sizeof(struct timespan));
	if (index)
		eeprom_update_byte(&config.specials[index].what, what);
}

uint8_t get_special_time(uint8_t index, struct timespan *span)
{
	eeprom_read_block(span, &config.specials[index].when, sizeof(struct timespan));
	return eeprom_read_byte(&config.specials[index].what);
}
