/*
 * main.c
 *
 *  Created on: 24.01.2013
 *      Author: dojoe
 */

#include "btclock.h"
#include <avr/sleep.h>

/* TODO:
 *   - proper button reaction
 *   - scrolling with fullstops in text
 *   - switchover between clock and text
 *   - text <= 4 chars
 *   - dark times
 */

uint8_t poll_button()
{
	/* If we're transmitting something, wait until done. */
	uart_wait();
	_delay_ms(10);
	BTATPORT |= _BV(BTATENABLE);
	BTATDDR &= ~_BV(BTATENABLE);
	uint8_t retval = BUTTONPIN & _BV(BUTTON);
	BTATPORT &= ~_BV(BTATENABLE);
	BTATDDR |= _BV(BTATENABLE);
	_delay_ms(50);
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
