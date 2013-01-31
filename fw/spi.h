/*
 * spi.h
 *
 *  Created on: 30.01.2013
 *      Author: dojoe
 */

#ifndef SPI_H_
#define SPI_H_

#include <avr/io.h>

static inline void spi_init()
{
	DDRB |= (1 << PB6) /* DO */ | (1 << PB7) /* USCK */;
}

uint8_t spi_xfer(uint8_t data_out);

#endif /* SPI_H_ */
