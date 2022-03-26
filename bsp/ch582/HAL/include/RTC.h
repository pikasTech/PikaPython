/********************************** (C) COPYRIGHT *******************************
 * File Name          : RTC.h
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2016/04/12
 * Description        :
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

/******************************************************************************/
#ifndef __RTC_H
#define __RTC_H

#ifdef __cplusplus
extern "C" {
#endif


#define RTC_TIMER_MAX_VALUE    0xa8c00000

extern volatile uint32_t RTCTigFlag;

/**
 * @brief   Initialize time Service.
 */
void HAL_TimeInit(void);

/**
 * @brief   配置RTC触发时间
 *
 * @param   time    - 触发时间.
 */
extern void RTC_SetTignTime(uint32_t time);

#ifdef __cplusplus
}
#endif

#endif
