/*
 * config.h
 *
 *  Created on: 25.02.2013
 *      Author: dojoe
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <avr/eeprom.h>

#define NUM_SPECIALS 2
#define MAX_SEQUENCE 8
#define NUM_LINES 5
#define TEXT_MAX 45

#define SEQ_TIME 255
#define SEQ_DATE 254
#define SEQ_NOTHING 253

struct sequence_entry {
	uint8_t which;    /* SEQ_TIME = time, SEQ_DATE = date, 0..NUM_LINES-1 = text */
	uint8_t duration; /* in seconds, 0 = skip */
};

struct timespan {
	uint16_t start, end;
};

struct special {
	struct timespan when;
	uint8_t what;
};

struct config_s {
	union {
		struct special specials[NUM_SPECIALS + 1];
		struct {
			struct timespan blank_time;
			uint8_t line_modes;
		};
	};
	struct sequence_entry sequence[MAX_SEQUENCE];
	char lines[NUM_LINES][TEXT_MAX];
};

extern EEMEM struct config_s config;
extern struct sequence_entry sequence[MAX_SEQUENCE];
extern uint8_t countdown;
extern uint8_t sequence_ptr;
extern uint8_t cur_line;

void next_line();
void check_timespans();
void set_line(uint8_t index, char *buf, uint8_t length, uint8_t mode);

static inline void config_init()
{
	eeprom_read_block(sequence, config.sequence, sizeof(config.sequence));
	next_line();
	check_timespans();
}

static inline void save_sequence()
{
	eeprom_update_block(sequence, config.sequence, sizeof(sequence));
	sequence_ptr = 0;
	countdown = 0;
}

static inline void get_line(uint8_t index, char *buf)
{
	eeprom_read_block(buf, config.lines[index], TEXT_MAX);
}

static inline void set_special_time(uint8_t index, struct timespan *span, uint8_t what)
{
	eeprom_update_block(span, &config.specials[index].when, sizeof(struct timespan));
	if (index)
		eeprom_update_byte(&config.specials[index].what, what);
	check_timespans();
}

static inline uint8_t get_special_time(uint8_t index, struct timespan *span)
{
	eeprom_read_block(span, &config.specials[index].when, sizeof(struct timespan));
	return eeprom_read_byte(&config.specials[index].what);
}

#endif /* CONFIG_H_ */
