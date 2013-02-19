/*
 * main.c
 *
 *  Created on: 24.01.2013
 *      Author: dojoe
 */

#include "btclock.h"
#include <avr/sleep.h>

/* TODO:
 *   - scrolling with fullstops in text
 *   - alternate scroll mode
 */

volatile enum display_mode_e display_mode;

uint8_t in_timespan(struct timespan span)
{
	uint16_t cur_time = time.hour << 8 | time.minute;
	uint8_t after_start = (cur_time >= span.start);
	uint8_t before_end  = (cur_time < span.end);
	if (span.start <= span.end)
	{
		return after_start && before_end;
	}
	else
	{
		return after_start || before_end;
	}
}

uint8_t poll_button()
{
	uint8_t retval = BUTTONPIN & _BV(BUTTON);
	return retval;
}

int main(void)
{
	/* Set BLANK high before setting pin direction to output
	 * to make sure the TLC stays blanked. */
	PORTB = PORTB_INIT;
	DDRB = DDRB_INIT;
	PORTD = PORTD_INIT;
	DDRD = DDRD_INIT;

	sei();
	config_init();
	rtc_init();
	tlc_init();
	bt_init();

	set_sleep_mode(SLEEP_MODE_IDLE);
	while (1)
	{
		if (tick)  /* happens once every second */
		{
			tick = 0;

			if (0 == countdown)
				next_line();
			else
				countdown--;

			static uint8_t last_button = 0;
			uint8_t button = !poll_button();
			if (last_button & !button)
			{
				bt_new_pin();
			}
			last_button = button;
		}

		cmd_poll();

		sleep_mode();
	}
	return 0;
}
