/*
 * time.c
 *
 *  Created on: Dec 27, 2024
 *      Author: basti
 */

#include "timer/timer.h"

static void MX_RTC_Init(void);
extern RTC_HandleTypeDef hrtc;
RTC_DateTypeDef Date;
RTC_TimeTypeDef Time;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;


int __io_putchar(int ch)
{
	HAL_UART_Transmit(&huart1, (uint8_t*)&ch, 1, HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, (uint8_t*)&ch, 1, HAL_MAX_DELAY);
	return ch;
}

RTC_TimeTypeDef TIMER_getTime()
{
	return Time;
}

void TIMER_init(char timeChar[])
{
	MX_USART1_UART_Init();
	MX_USART2_UART_Init();
	MX_RTC_Init();
	HAL_RTC_GetTime(&hrtc, &Time, RTC_FORMAT_BIN);
	sprintf(timeChar, "%02d:%02d", (char)Time.Hours, (char)Time.Minutes);
}

bool TIMER_updateTime(uint8_t *lastMinutes)
{
	printf("Salut toi\n");
	HAL_RTC_GetTime(&hrtc, &Time, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &Date, RTC_FORMAT_BIN);
	if (lastMinutes != &Time.Minutes)
	{
		ILI9341_ChangeTime(Time.Hours, Time.Minutes);
		lastMinutes = &Time.Minutes;
		return true;
	}
	else {
		return false;
	}
}
