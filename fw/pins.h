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
 * PD2 - 1HZ input / INT0
 * PD3-PD6 - Display Enable
 * PB0 - BT Reset
 * PB1 - BT AT Enable
 * PB2 - RTC CE
 * PB3 - LATCH / OC1A
 * PB4 - BLANK / OC1B
 */

#define BTRESETPORT PORTB
#define BTRESETDDR  DDRB
#define BTRESET     PB0

#define BTATPORT    PORTB
#define BTATDDR     DDRB
#define BTATENABLE  PB1

#define SPISCK      PB7
#define SPIMOSI     PB5

#define RTCCEPORT   PORTB
#define RTCCEDDR    DDRB
#define RTCCE       PB2

#define SEGPORT     PORTD
#define SEGDDR      DDRD
#define SEGBASE     PD3
#define SEGMASK     (0xF << SEGBASE)

#define LATCHPORT   PORTB
#define LATCHDDR    DDRB
#define LATCH       PB3
#define BLANK       PB4

#define HZPORT      PORTD
#define HZDDR       DDRD
#define HZPIN       PD2

#define PORTB_INIT  (1 << BTRESET) | (1 << BLANK)
#define DDRB_INIT   (1 << BTRESET) | (1 << BTATENABLE) | (1 << RTCCE) | \\
					(1 << LATCH) | (1 << BLANK) | (1 << SPISCK) | (1 << SPIMOSI)

#define DDRD_INIT   (1 << TXD_BIT) | (1 << )

#endif /* PINS_H_ */
