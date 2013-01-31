/*
 * spi.c
 *
 *  Created on: 30.01.2013
 *      Author: dojoe
 */

#include "spi.h"

uint8_t spi_xfer(uint8_t data_out)
{
	USICR = (1 << USIWM0) | // Three-Wire mode
			(2 << USICS0) | // Use external clock
			(1 << USICLK);  // Strobe counter through USITC
	USIDR = data_out;
	uint8_t i = 16;
	do
	{
		USICR = (1 << USIWM0) | (2 << USICS0) | (1 << USICLK) | (1 << USITC);
	}
	while (--i);
	return USIDR;
}
