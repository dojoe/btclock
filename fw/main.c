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

int main(void)
{
	sei();
	spi_init();
	rtc_init();
	tlc_init();
	bt_init();

	for (uint8_t i = 0; i < 4; i++)
		display[i] = font_get_digit(i + 1);

	DDRD = 1 << PD3;
	while (1)
	{
		PORTD = 1 << PD3;
		bt_led_on();
		_delay_ms(500);
		PORTD = 0;
		bt_led_off();
		_delay_ms(500);
	}
	return 0;
}
