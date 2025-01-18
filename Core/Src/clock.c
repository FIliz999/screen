/*
 * clock.c
 *	Ici on va faire l'init et le lancement de tous ce qu'il faut pour laisser au main que 4 methodes.
 *  Created on: Jan 16, 2025
 *      Author: basti
 */

#include "clock.h"


void CLOCK_init()
{
	ILI9341_Init();
	char timerInit[LENGHT_TIMER];
	TIMER_init(timerInit);
	ILI9341_InitDrawString(timerInit);
}

void CLOCK_start()
{
	uint8_t lastMinutes = TIMER_getTime().Minutes;
	while(1)
	{
		if (TIMER_updateTime(&lastMinutes))
		{
			//Return true => change time
		}
		HAL_Delay(500);
	}
}

void CLOCK_stop()
{

}

void CLOCK_clean()
{

}
