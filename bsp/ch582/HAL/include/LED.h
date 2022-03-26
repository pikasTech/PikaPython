/********************************** (C) COPYRIGHT *******************************
 * File Name          : LED.h
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2016/04/12
 * Description        :
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

/******************************************************************************/
#ifndef __LED_H
#define __LED_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************
 * CONSTANTS
 */

/* LEDS - The LED number is the same as the bit position */
#define HAL_LED_1                      0x01
#define HAL_LED_2                      0x02
#define HAL_LED_3                      0x04
#define HAL_LED_4                      0x08
#define HAL_LED_ALL                    (HAL_LED_1 | HAL_LED_2 | HAL_LED_3 | HAL_LED_4)

/* Modes */
#define HAL_LED_MODE_OFF               0x00
#define HAL_LED_MODE_ON                0x01
#define HAL_LED_MODE_BLINK             0x02
#define HAL_LED_MODE_FLASH             0x04
#define HAL_LED_MODE_TOGGLE            0x08

/* Defaults */
#define HAL_LED_DEFAULT_MAX_LEDS       4
#define HAL_LED_DEFAULT_DUTY_CYCLE     5
#define HAL_LED_DEFAULT_FLASH_COUNT    50
#define HAL_LED_DEFAULT_FLASH_TIME     1000

/*********************************************************************
 * TYPEDEFS
 */

/* 连接一个LED用于监控演示程序的进度,低电平LED亮 */

/* 1 - LED */
#define LED1_BV                 BV(15)
#define LED2_BV
#define LED3_BV

#define LED1_OUT                (R32_PB_OUT)
#define LED2_OUT                0
#define LED3_OUT                0
#define LED4_OUT                0

#define LED1_DDR                (R32_PB_DIR |= LED1_BV)
#define LED2_DDR                0
#define LED3_DDR                0

#define HAL_TURN_OFF_LED1()     (LED1_OUT |= LED1_BV)
#define HAL_TURN_OFF_LED2()
#define HAL_TURN_OFF_LED3()
#define HAL_TURN_OFF_LED4()

#define HAL_TURN_ON_LED1()      (LED1_OUT &= (~LED1_BV))
#define HAL_TURN_ON_LED2()
#define HAL_TURN_ON_LED3()
#define HAL_TURN_ON_LED4()

#define HAL_STATE_LED1()        0
#define HAL_STATE_LED2()        0
#define HAL_STATE_LED3()        0
#define HAL_STATE_LED4()        0

/*********************************************************************
 * GLOBAL VARIABLES
 */

/**
 * @brief   Initialize LED Service.
 */
void HAL_LedInit(void);

/**
 * @brief   update time LED Service.
 */
void HalLedUpdate(void);

/**
 * @brief   Turn ON/OFF/TOGGLE given LEDs
 *
 * @param   led     - bit mask value of leds to be turned ON/OFF/TOGGLE
 * @param   mode    - BLINK, FLASH, TOGGLE, ON, OFF
 */
extern uint8_t HalLedSet(uint8_t led, uint8_t mode);

/**
 * @brief   Blink the leds
 *
 * @param   led         - bit mask value of leds to be turned ON/OFF/TOGGLE
 * @param   numBlinks   - number of blinks
 * @param   percent     - the percentage in each period where the led will be on
 * @param   period      - length of each cycle in milliseconds
 */
extern void HalLedBlink(uint8_t leds, uint8_t cnt, uint8_t duty, uint16_t time);

/**
 * @brief   Put LEDs in sleep state - store current values
 */
extern void HalLedEnterSleep(void);

/**
 * @brief   Retore LEDs from sleep state
 */
extern void HalLedExitSleep(void);

/**
 * @brief   Return LED state
 */
extern uint8_t HalLedGetState(void);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif
