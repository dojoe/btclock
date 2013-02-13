/*
 * cmd.c
 *
 *  Created on: 11.02.2013
 *      Author: dojoe
 */

#include "btclock.h"

static char cmd_buf[16];
static uint8_t cmd_buf_ptr = 0;

static const PROGMEM char help[] =
		"     H A L P !\n\n"
		"Set time: T=YYMMDDhhmmss\n"
		"Get time: T?\n"
		"    all digits 0..9\n"
		"    HH is 24hr format\n\n";

static uint8_t set_time()
{
	uint8_t i, data[6];

	for (i = 0; i < 6; i++)
	{
		char *text = cmd_buf + 2 + (2 * i);
		if (text[0] < '0' || text[0] > '9' || text[1] < '0' || text[1] > '9')
		{
			return 0;
		}
		data[i] = ((text[0] - '0') << 4) | (text[1] - '0');
	}

	if (data[1] > 0x12 || data[2] > 0x31 || data[3] > 0x23 || data[4] > 0x59 || data[5] > 0x59)
	{
		return 0;
	}

	rtc_set_time(data[0], data[1], data[2], data[3], data[4], data[5]);

	return 1;
}

static void print_bcd(uint8_t value, char extra_char)
{
	uart_putc('0' + (value >> 4));
	uart_putc('0' + (value & 0xF));
	uart_putc(extra_char);
}

static void get_time()
{
	uart_putc('2');
	uart_putc('0');
	print_bcd(time.year, '-');
	print_bcd(time.month, '-');
	print_bcd(time.day, ' ');
	print_bcd(time.hour, ':');
	print_bcd(time.minute, ':');
	print_bcd(time.second, '\n');
}

static uint8_t cmd_parse()
{
	if (cmd_buf[0] == '?')
	{
		uart_puts_p(help);
		return 1;
	}
	else if (cmd_buf[0] == 'T')
	{
		if (cmd_buf_ptr == 14 && cmd_buf[1] == '=')
			return set_time();
		else if (cmd_buf_ptr == 2 && cmd_buf[1] == '?')
		{
			get_time();
			return 1;
		}
	}
	return 0;
}

void cmd_poll()
{
	while (1)
	{
		unsigned int rc = uart_getc();
		char c = rc & 0xFF;
		if (rc & UART_NO_DATA)
			return;
		if (c == '\n')
		{
			/* filter out empty commands and "OK" responses from BT module */
			if (cmd_buf_ptr &&
					!(cmd_buf_ptr == 2 && cmd_buf[0] == 'O' && cmd_buf[1] == 'K'))
				uart_puts_p(cmd_parse() ? PSTR("OK\n") : PSTR("Nope\n"));
			cmd_buf_ptr = 0;
		}
		else if (c != '\r' && cmd_buf_ptr < sizeof(cmd_buf))
		{
			cmd_buf[cmd_buf_ptr++] = c;
		}
	}
}
