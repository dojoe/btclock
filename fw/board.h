/*
 * board.h
 *
 *  Created on: 17.03.2013
 *      Author: dojoe
 */

#ifndef BOARD_H_
#define BOARD_H_

#if BOARD_VERSION == 1
#define BOARD_SEGBASE PD3
#define BOARD_HZBIT PD2
#define BOARD_SEGS_ARE_ACTIVE_LOW
#elif BOARD_VERSION == 2
#define BOARD_SEGBASE PD2
#define BOARD_HZBIT PD6
#define BOARD_USE_HZ_PCINT
#else
#error Must define board version!
#endif

#endif /* BOARD_H_ */
