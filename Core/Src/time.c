/*
 * time.c
 *
 *  Created on: Dec 27, 2024
 *      Author: basti
 */

#include "time.h"

RTC_AlarmTypeDef sAlarm;
RTC_TimeTypeDef sTime = {0};

void TIME_SetRTCTime(RTC_HandleTypeDef *hrtc)
{
    /* Définir l'heure : 14:30:45 */
    sTime.Hours = HOUR;
    sTime.Minutes = MIN;        // Minutes
    sTime.Seconds = 40;        // Secondes
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE; // Pas d'ajustement pour l'heure d'été
    sTime.StoreOperation = RTC_STOREOPERATION_SET; // Pas de stockage spécial

    /* Appliquer l'heure */
    if (HAL_RTC_SetTime(hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK) {
        /* Gestion de l'erreur */
        Error_Handler();
    }
    //MX_RTC_SetAlarm(hrtc);
}

RTC_TimeTypeDef TIME_GetTime(RTC_HandleTypeDef *hrtc)
{
    /* Lire l'heure actuelle */
    if (HAL_RTC_GetTime(hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK) {
        /* Gestion d'erreur */
        Error_Handler();
    }
    return sTime;
}

void MX_RTC_SetAlarm(RTC_HandleTypeDef *hrtc) {

    // Obtenir l'heure actuelle
    HAL_RTC_GetTime(hrtc, &sTime, RTC_FORMAT_BIN);

    // Configurer l'alarme pour la prochaine minute
    sAlarm.AlarmTime.Hours = sTime.Hours;
    sAlarm.AlarmTime.Minutes = sTime.Minutes + 1;
    if (sAlarm.AlarmTime.Minutes >= 60) {
        sAlarm.AlarmTime.Minutes = 0;
        sAlarm.AlarmTime.Hours = (sAlarm.AlarmTime.Hours + 1) % 24;
    }
    sAlarm.AlarmTime.Seconds = 0;
    sAlarm.Alarm = RTC_ALARM_A;
    sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
    sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
    sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
    sAlarm.AlarmDateWeekDay = 1;

    // Définir l'alarme avec l'interruption
    if (HAL_RTC_SetAlarm_IT(hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK) {
        // Erreur de configuration de l'alarme
    }
}

/* Interruption RTC Alarm */
void RTC_Alarm_IRQHandler(RTC_HandleTypeDef *hrtc) {
    HAL_RTC_AlarmIRQHandler(hrtc);  // Appeler le gestionnaire d'interruption HAL
}
