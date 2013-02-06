/*
 * main.c
 *
 *  Created on: 24.01.2013
 *      Author: dojoe
 */


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "pins.h"
#include "bt.h"
#include "spi.h"
#include "rtc.h"
#include "tlc.h"
#include "font.h"
#include "uart.h"

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
	rtc_init();
	tlc_init();
	bt_init();

	//for (uint8_t i = 0; i < 4; i++)
	//	display[i] = font_get_digit(i + 1);

	//rtc_set_time(0x12, 0x02, 0x03, 0x15, 0x30, 0x00);

	uint8_t last_button = 0;

	while (1)
	{
		bt_led_on();
		_delay_ms(500);
		bt_led_off();
		_delay_ms(500);
		char str[15] = "YYMMDDhhmmss\r\n";
		str[0] = '0' + (time.year >> 4);
		str[1] = '0' + (time.year & 0xF);
		str[2] = '0' + (time.month >> 4);
		str[3] = '0' + (time.month & 0xF);
		str[4] = '0' + (time.day >> 4);
		str[5] = '0' + (time.day & 0xF);
		str[6] = '0' + (time.hour >> 4);
		str[7] = '0' + (time.hour & 0xF);
		str[8] = '0' + (time.minute >> 4);
		str[9] = '0' + (time.minute & 0xF);
		str[10] = '0' + (time.second >> 4);
		str[11] = '0' + (time.second & 0xF);
		uart_puts(str);

		uint8_t button = !poll_button();
		if (last_button & !button)
		{
			bt_new_pin();
		}
		last_button = button;
	}
	return 0;
}
