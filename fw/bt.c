/*
 * bt.c
 *
 *  Created on: 27.01.2013
 *      Author: dojoe
 */

#include "bt.h"
#include "pins.h"
#include "uart.h"
#include "font.h"
#include "tlc.h"
#include <stdlib.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

void bt_send(const char *str)
{
	uart_wait();
	_delay_ms(10);
	BTATPORT |= 1 << BTATENABLE;
	_delay_ms(50);
	uart_puts(str);
	uart_wait();
	BTATPORT &= ~(1 << BTATENABLE);
	_delay_ms(50);
}

void bt_send_p(const char *str)
{
	uart_wait();
	_delay_ms(10);
	BTATPORT |= 1 << BTATENABLE;
	_delay_ms(50);
	uart_puts_p(str);
	uart_wait();
	BTATPORT &= ~(1 << BTATENABLE);
	_delay_ms(50);
}

#define bt_send_P(str) bt_send_p(PSTR(str))

void bt_init()
{
	/* reset BT module */
	BTATPORT &= ~(1 << BTATENABLE);
	BTATDDR |= 1 << BTATENABLE;
	BTRESETPORT &= ~(1 << BTRESET);
	_delay_ms(10);
	BTRESETPORT |= 1 << BTRESET;

	/* After reset, we have to wait for the module to boot before */
	/* we can enable AT mode, or the module will land in "perpetual AT mode" */
	_delay_ms(1000);

	/* enable AT mode */
	//BTATPORT |= 1 << BTATENABLE;

	/* set up UART */
	uart_init(UART_BAUD_SELECT(9600, F_CPU));

	bt_send_P("AT+NAME=\"DoJoe's BT Clock\"\r\n");
}

static char setpin[] = "BT+PSWD=XXXX\r\n";

void bt_new_pin()
{
	for (int i = 0; i < 4; i++)
	{
		uint8_t digit = rand() % 10;
		setpin[8 + i] = digit + '0';
		display[i] = font_get_digit(digit);
	}

	/* reset BT module again, to be sure we get into AT mode and are not connected */
	//bt_init();

	uart_puts(setpin);
}

void bt_led_on()
{
	//bt_send(setpin);
	bt_send_P("AT+PIO=7,1\r\n");
}

void bt_led_off()
{
	bt_send_P("AT+PIO=7,0\r\n");
}

#if 0
void bt_poll_gpio()
{
	bt_send_P("AT+MPIO?\r\n");
}
#endif
