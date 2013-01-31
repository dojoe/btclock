/*
 * bt.c
 *
 *  Created on: 27.01.2013
 *      Author: dojoe
 */

#include "bt.h"
#include "pins.h"
#include "uart.h"
#include <util/delay.h>
#include <avr/pgmspace.h>

void bt_init()
{
	/* reset BT module */
	BTRESETDDR |= 1 << BTRESET;
	BTRESETPORT &= ~(1 << BTRESET);
	_delay_ms(10);
	BTRESETPORT |= 1 << BTRESET;

	/* enable AT mode */
	BTATDDR |= 1 << BTATENABLE;
	BTATPORT |= 1 << BTATENABLE;

	/* set up UART */
	uart_init(UART_BAUD_SELECT(9600, F_CPU));
}

void bt_led_on()
{
	uart_puts_P("AT+PIO=7,1\r\n");
}

void bt_led_off()
{
	uart_puts_P("AT+PIO=7,0\r\n");
}

void bt_poll_gpio()
{
	uart_puts_P("AT+MPIO?\r\n");
}
