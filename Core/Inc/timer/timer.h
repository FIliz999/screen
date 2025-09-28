/*
 * time.h
 *
 *  Created on: Dec 27, 2024
 *      Author: basti
 */

#ifndef INC_TIME_H_
#define INC_TIME_H_

#include <stdbool.h>
#include "main.h"
#include "stdio.h"
#include "stm32f4xx_hal_rtc.h"

void TIMER_init(char timeChar[]);
RTC_TimeTypeDef TIMER_getTime();
bool TIMER_updateTime(uint8_t *lastMinutes);
int __io_putchar(int ch);
#endif /* INC_TIME_H_ */
