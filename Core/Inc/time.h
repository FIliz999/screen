/*
 * time.h
 *
 *  Created on: Dec 27, 2024
 *      Author: basti
 */

#ifndef INC_TIME_H_
#define INC_TIME_H_

#include "main.h"
#include "stdio.h"

#define HOUR 23
#define MIN 04

extern RTC_TimeTypeDef sTime;

typedef struct {
	uint8_t uniteMin;
	uint8_t dixMin;
	uint8_t uniteHour;
	uint8_t dixHour;
}TIME;

void TIME_InitRtc();
void TIME_SetRTCTime(RTC_HandleTypeDef *hrtc);
RTC_TimeTypeDef TIME_GetTime(RTC_HandleTypeDef *hrtc);
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc);
void MX_RTC_SetAlarm(RTC_HandleTypeDef *hrtc);
void RTC_Alarm_IRQHandler(RTC_HandleTypeDef *hrtc);
void TIME_AddMin(RTC_HandleTypeDef *hrtc);

#endif /* INC_TIME_H_ */
