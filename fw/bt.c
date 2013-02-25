/*
 * bt.c
 *
 *  Created on: 27.01.2013
 *      Author: dojoe
 */

#include "btclock.h"

static void bt_send(const char *str)
{
	uart_puts(str);
}

static void bt_send_p(const char *str)
{
	uart_puts_p(str);
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
	BTATPORT |= 1 << BTATENABLE;

	/* set up UART */
	uart_init(UART_BAUD_SELECT(9600, F_CPU));

	bt_send_P("AT+NAME=\"BT Clock\"\r\n");
}

static const PROGMEM char setpin_tmpl[] = "AT+PSWD=XXXX\r\n";

void bt_new_pin()
{
	char setpin[sizeof(setpin_tmpl)];
	memcpy_P(setpin, setpin_tmpl, sizeof(setpin_tmpl));
	uint16_t rnd = random_number;
	for (int i = 0; i < 4; i++)
	{
		uint8_t digit = rnd & 0xF;
		rnd >>= 4;
		if (digit >= 10)
			digit -= 10;
		setpin[8 + i] = digit + '0';
		display[i] = font_get_digit(digit);
		display_mode = BTPIN;
		countdown = 30;
		cur_line = SEQ_NOTHING;
	}

	bt_send(setpin);
}

#if 0
void bt_led_on()
{
	//bt_send(setpin);
	bt_send_P("AT+PIO=7,1\r\n");
}

void bt_led_off()
{
	bt_send_P("AT+PIO=7,0\r\n");
}

void bt_poll_gpio()
{
	bt_send_P("AT+MPIO?\r\n");
}
#endif
