/*
 * clock.h
 *
 *  Created on: Jan 16, 2025
 *      Author: basti
 */

#ifndef INC_CLOCK_H_
#define INC_CLOCK_H_

#include "timer/timer.h"
#include "screen/ili9341/ili9341.h"

#define LENGHT_TIMER 6

void CLOCK_init();
void CLOCK_start();
void CLOCK_stop();
void CLOCK_clean();


#endif /* INC_CLOCK_H_ */
