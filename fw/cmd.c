/*
 * cmd.c
 *
 *  Created on: 11.02.2013
 *      Author: dojoe
 */

#include "btclock.h"

static char cmd_buf[TEXT_MAX + 2]; /* for "N=<up to TEXT_MAX chars>" */
static uint8_t cmd_buf_ptr = 0;

static uint8_t parse_bcd(char *text, uint8_t *data, uint8_t size)
{
	while (size--)
	{
		if (text[0] < '0' || text[0] > '9' || text[1] < '0' || text[1] > '9')
		{
			return 0;
		}
		*data++ = ((text[0] - '0') << 4) | (text[1] - '0');
		text += 2;
	}
	return 1;
}

static uint8_t set_time()
{
	uint8_t data[6];

	if (cmd_buf_ptr != 14)
		return 0;

	if (!parse_bcd(cmd_buf + 2, data, 6))
		return 0;

	if (data[1] > 0x12 || data[2] > 0x31 || data[3] > 0x23 || data[4] > 0x59 || data[5] > 0x59)
	{
		return 0;
	}

	rtc_set_time(data[0], data[1], data[2], data[3], data[4], data[5]);

	return 1;
}

static void print_bcd(uint8_t value)
{
	uart_putc('0' + (value >> 4));
	uart_putc('0' + (value & 0xF));
}

static void print_bcd_plus_char(uint8_t value, char extra_char)
{
	print_bcd(value);
	uart_putc(extra_char);
}

static uint8_t get_time()
{
	uart_putc('2');
	uart_putc('0');
	print_bcd_plus_char(time.year, '-');
	print_bcd_plus_char(time.month, '-');
	print_bcd_plus_char(time.day, ' ');
	print_bcd_plus_char(time.hour, ':');
	print_bcd_plus_char(time.minute, ':');
	print_bcd_plus_char(time.second, '\n');
	return 1;
}

static uint8_t set_sequence()
{
	struct sequence_entry tmp_sequence[MAX_SEQUENCE];
	uint8_t which, sequence_ptr, cmd_ptr = 2;

	memset(tmp_sequence, 0, sizeof(tmp_sequence));

	for (sequence_ptr = 0; sequence_ptr < MAX_SEQUENCE; sequence_ptr++)
	{
		char c;
		uint8_t duration = 0;

		if (cmd_ptr >= cmd_buf_ptr)
			break;

		c = cmd_buf[cmd_ptr++];

		if (c >= '1' && c < '1' + NUM_LINES)
			which = c - '1';
		else if (c == 'T')
			which = SEQ_TIME;
		else if (c == 'D')
			which = SEQ_DATE;
		else
			return 0;
		tmp_sequence[sequence_ptr].which = which;

		while (1)
		{
			if (cmd_ptr >= cmd_buf_ptr)
				break;

			c = cmd_buf[cmd_ptr++];

			if (c == ',')
				break;

			if (c < '0' || c > '9')
				return 0;

			{   /* multiply duration by 10 */
				uint8_t tmp = duration;
				duration <<= 2;
				duration += tmp;
				duration <<= 1;
			}
			duration += c - '0';
		}

		if (!duration)
			return 0;

		tmp_sequence[sequence_ptr].duration = duration;
	}

	if (!sequence_ptr)
		return 0;

	memcpy(sequence, tmp_sequence, sizeof(tmp_sequence));
	save_sequence();
	return 1;
}

static uint8_t print_digit(uint8_t value, uint8_t divisor)
{
	uint8_t digit;
	for (digit = 0; value >= divisor; value -= divisor, digit++);
	if (digit)
		uart_putc(digit + '0');
	return value;
}

static uint8_t get_sequence()
{
	uint8_t sequence_ptr, value, which;

	for (sequence_ptr = 0; sequence_ptr < MAX_SEQUENCE; sequence_ptr++)
	{
		value = sequence[sequence_ptr].duration;
		if (sequence_ptr)
		{
			if (value)
				uart_putc(',');
			else
				break;
		}
		which = sequence[sequence_ptr].which;
		uart_putc(which == SEQ_TIME ? 'T' : (which == SEQ_DATE ? 'D' : which + '1'));
		value = print_digit(value, 100);
		value = print_digit(value, 10);
		uart_putc(value + '0');
	}

	uart_putc('\n');
	return 1;
}

static uint8_t parse_timespan(char *text, uint8_t len, struct timespan *span)
{
	uint16_t start, end;
	uint8_t b[2];

	if (len < 9 || text[4] != '-')
		return 0;

	if (!parse_bcd(text, b, 2) || b[0] > 0x23 || b[1] > 0x59)
		return 0;

	start = b[0] << 8 | b[1];

	if (!parse_bcd(text + 5, b, 2) || b[0] > 0x23 || b[1] > 0x59)
		return 0;

	end = b[0] << 8 | b[1];

	span->start = start;
	span->end = end;
	return 1;
}

static uint8_t set_special(uint8_t index)
{
	struct timespan when;
	uint8_t what = 0;

	if (!parse_timespan(cmd_buf + 2, cmd_buf_ptr - 2, &when))
		return 0;

	if (index)
	{
		if (cmd_buf_ptr < 13 || cmd_buf[11] != ',' || cmd_buf[12] < '1' || cmd_buf[12] > '0' + NUM_LINES)
			return 0;
		what = cmd_buf[12] - '1';
	}

	set_special_time(index, &when, what);
	return 1;
}

static uint8_t get_special(uint8_t index)
{
	struct timespan span;
	uint8_t which = get_special_time(index, &span);
	print_bcd(span.start >> 8);
	print_bcd_plus_char(span.start & 0xFF, '-');
	print_bcd(span.end >> 8);
	print_bcd(span.end & 0xFF);
	if (index)
	{
		uart_putc(',');
		uart_putc('1' + which);
	}
	uart_putc('\n');
	return 1;
}

static uint8_t cmd_parse()
{
	char type = cmd_buf[0], op = cmd_buf[1];
	uint8_t set = ('=' == op);

	if (cmd_buf_ptr == 1 || !(set || op == '?'))
		return 0;

	if (type == 'T')
			return set ? set_time() : get_time();
	else if (type == 'S')
		return set ? set_sequence() : get_sequence();
	else if (type >= 'B' && type <= 'B' + NUM_SPECIALS)
	{
		uint8_t index = type - 'B';
		return set ? set_special(index) : get_special(index);
	}
	else if (type >= '1' && type <= '0' + NUM_LINES)
	{
		type -= '1';
		if (set)
		{
			uint8_t mode;
			if (cmd_buf_ptr - 2 > TEXT_MAX - 1)
				return 0;
			mode = '!' == cmd_buf[2] ? 1 : 0;
			set_line(type, cmd_buf + 2 + mode, cmd_buf_ptr - 2 - mode, mode);
		}
		else
		{
			get_line(type, cmd_buf);
			uart_puts(cmd_buf);
			uart_putc('\n');
		}
		return 1;
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
