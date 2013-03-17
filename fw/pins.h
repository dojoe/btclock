/*
 * pins.h
 *
 *  Created on: 27.01.2013
 *      Author: dojoe
 */

#ifndef PINS_H_
#define PINS_H_

#include <avr/io.h>

/*
 * PD0, PD1 - UART
 * PD2 - 1HZ input / INT0
 * PD3-PD6 - Display Enable
 * PB0 - BT Reset
 * PB1 - BT AT Enable / Button
 * PB2 - RTC CE
 * PB3 - LATCH / OC1A
 * PB4 - BLANK / OC1B
 * PB5..PB7 - SPI (TLC, RTC)
 */

#define BTRESETPORT PORTB
#define BTRESETDDR  DDRB
#define BTRESET     PB0

#define BTATPORT    PORTB
#define BTATDDR     DDRB
#define BTATENABLE  PB1

#define BUTTONPIN   PINB
#define BUTTON      BTATENABLE

#define SPISCK      PB7
#define SPIDO       PB6

#define RTCCEPORT   PORTB
#define RTCCEDDR    DDRB
#define RTCCE       PB2

#define SEGPORT     PORTD
#define SEGDDR      DDRD
#define SEGBASE     BOARD_SEGBASE
#define SEGMASK     (0xF << SEGBASE)

#define LATCHPORT   PORTB
#define LATCHDDR    DDRB
#define LATCH       PB3
#define BLANK       PB4

#define HZPORT      PORTD
#define HZDDR       DDRD
#define HZPIN       BOARD_HZPIN

#define PORTB_INIT  (1 << BTRESET) | (1 << BLANK)
#define DDRB_INIT   (1 << BTRESET) | (1 << RTCCE) | \
					(1 << LATCH) | (1 << BLANK) | (1 << SPISCK) | (1 << SPIDO)

#define PORTD_INIT  (1 << HZPIN)
#define DDRD_INIT   SEGMASK

#endif /* PINS_H_ */
