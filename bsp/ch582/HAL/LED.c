/********************************** (C) COPYRIGHT *******************************
 * File Name          : LED.c
 * Author             : WCH
 * Version            : V1.2
 * Date               : 2022/01/18
 * Description        :
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

/******************************************************************************/
/* 头文件包含 */
#include "HAL.h"

/* LED control structure */
typedef struct
{
    uint8_t  mode;  /* Operation mode */
    uint8_t  todo;  /* Blink cycles left */
    uint8_t  onPct; /* On cycle percentage */
    uint16_t time;  /* On/off cycle time (msec) */
    uint32_t next;  /* Time for next change */
} HalLedControl_t;

typedef struct
{
    HalLedControl_t HalLedControlTable[HAL_LED_DEFAULT_MAX_LEDS];
    uint8_t         sleepActive;
} HalLedStatus_t;

/***************************************************************************************************
 *                                           GLOBAL VARIABLES
 ***************************************************************************************************/
static uint8_t HalLedState; // LED state at last set/clr/blink update

static uint8_t preBlinkState; // Original State before going to blink mode
                              // bit 0, 1, 2, 3 represent led 0, 1, 2, 3
static HalLedStatus_t HalLedStatusControl;

/***************************************************************************************************
 *                                            LOCAL FUNCTION
 ***************************************************************************************************/
void HalLedOnOff(uint8_t leds, uint8_t mode);

/***************************************************************************************************
 *                                            FUNCTIONS - API
 ***************************************************************************************************/

/*********************************************************************
 * @fn      HAL_LedInit
 *
 * @brief   Initialize LED Service
 *
 * @return  none
 */
void HAL_LedInit(void)
{
    /* Initialize all LEDs to OFF */
    LED1_DDR;
    HalLedSet(HAL_LED_ALL, HAL_LED_MODE_OFF);
    // just test
    HalLedBlink(HAL_LED_1, 10, 30, 4000);
    /* Initialize sleepActive to FALSE */
    HalLedStatusControl.sleepActive = FALSE;
}

/*********************************************************************
 * @fn      HalLedSet
 *
 * @brief   Turn ON/OFF/TOGGLE given LEDs
 *
 * @param   led     - bit mask value of leds to be turned ON/OFF/TOGGLE
 * @param   mode    - BLINK, FLASH, TOGGLE, ON, OFF
 *
 * @return  0
 */
uint8_t HalLedSet(uint8_t leds, uint8_t mode)
{
    uint8_t          led;
    HalLedControl_t *sts;

    switch(mode)
    {
        case HAL_LED_MODE_BLINK:
        {
            /* Default blink, 1 time, D% duty cycle */
            HalLedBlink(leds, 1, HAL_LED_DEFAULT_DUTY_CYCLE, HAL_LED_DEFAULT_FLASH_TIME);
            break;
        }

        case HAL_LED_MODE_FLASH:
        {
            /* Default flash, N times, D% duty cycle */
            HalLedBlink(leds, HAL_LED_DEFAULT_FLASH_COUNT, HAL_LED_DEFAULT_DUTY_CYCLE, HAL_LED_DEFAULT_FLASH_TIME);
            break;
        }

        case HAL_LED_MODE_ON:
        case HAL_LED_MODE_OFF:
        case HAL_LED_MODE_TOGGLE:
        {
            led = HAL_LED_1;
            leds &= HAL_LED_ALL;
            sts = HalLedStatusControl.HalLedControlTable;
            while(leds)
            {
                if(leds & led)
                {
                    if(mode != HAL_LED_MODE_TOGGLE)
                    {
                        sts->mode = mode; /* ON or OFF */
                    }
                    else
                    {
                        sts->mode ^= HAL_LED_MODE_ON; /* Toggle */
                    }
                    HalLedOnOff(led, sts->mode);
                    leds ^= led;
                }
                led <<= 1;
                sts++;
            }
            break;
        }

        default:
            break;
    }
    return (0);
}

/*********************************************************************
 * @fn      HalLedBlink
 *
 * @brief   Blink the leds
 *
 * @param   led         - bit mask value of leds to be turned ON/OFF/TOGGLE
 * @param   numBlinks   - number of blinks
 * @param   percent     - the percentage in each period where the led will be on
 * @param   period      - length of each cycle in milliseconds
 *
 * @return  none
 */
void HalLedBlink(uint8_t leds, uint8_t numBlinks, uint8_t percent, uint16_t period)
{
    uint8_t          led;
    HalLedControl_t *sts;

    if(leds && percent && period)
    {
        if(percent < 100)
        {
            led = HAL_LED_1;
            leds &= HAL_LED_ALL;
            sts = HalLedStatusControl.HalLedControlTable;
            while(leds)
            {
                if(leds & led)
                {
                    /* Store the current state of the led before going to blinking */
                    preBlinkState |= (led & HalLedState);
                    sts->mode = HAL_LED_MODE_OFF; /* Stop previous blink */
                    sts->time = period;           /* Time for one on/off cycle */
                    sts->onPct = percent;         /* % of cycle LED is on */
                    sts->todo = numBlinks;        /* Number of blink cycles */
                    if(!numBlinks)
                    {
                        sts->mode |= HAL_LED_MODE_FLASH; /* Continuous */
                    }
                    sts->next = TMOS_GetSystemClock(); /* Start now */
                    sts->mode |= HAL_LED_MODE_BLINK;   /* Enable blinking */
                    leds ^= led;
                }
                led <<= 1;
                sts++;
            }
            tmos_start_task(halTaskID, LED_BLINK_EVENT, 0);
        }
        else
        {
            HalLedSet(leds, HAL_LED_MODE_ON); /* >= 100%, turn on */
        }
    }
    else
    {
        HalLedSet(leds, HAL_LED_MODE_OFF); /* No on time, turn off */
    }
}

/*********************************************************************
 * @fn      HalLedUpdate
 *
 * @brief   Update leds to work with blink
 *
 * @return  none
 */
void HalLedUpdate(void)
{
    uint8_t          led, pct, leds;
    uint16_t         next, wait;
    uint32_t         time;
    HalLedControl_t *sts;

    next = 0;
    led = HAL_LED_1;
    leds = HAL_LED_ALL;
    sts = HalLedStatusControl.HalLedControlTable;

    /* Check if sleep is active or not */
    if(!HalLedStatusControl.sleepActive)
    {
        while(leds)
        {
            if(leds & led)
            {
                if(sts->mode & HAL_LED_MODE_BLINK)
                {
                    time = TMOS_GetSystemClock();
                    if(time >= sts->next)
                    {
                        if(sts->mode & HAL_LED_MODE_ON)
                        {
                            pct = 100 - sts->onPct;             /* Percentage of cycle for off */
                            sts->mode &= ~HAL_LED_MODE_ON;      /* Say it's not on */
                            HalLedOnOff(led, HAL_LED_MODE_OFF); /* Turn it off */
                            if(!(sts->mode & HAL_LED_MODE_FLASH))
                            {
                                if(sts->todo != 0xff)
                                {
                                    sts->todo--; /* Not continuous, reduce count */
                                }
                                if(!sts->todo)
                                {
                                    sts->mode ^= HAL_LED_MODE_BLINK; /* No more blinks */
                                }
                            }
                        }
                        else
                        {
                            pct = sts->onPct;                  /* Percentage of cycle for on */
                            sts->mode |= HAL_LED_MODE_ON;      /* Say it's on */
                            HalLedOnOff(led, HAL_LED_MODE_ON); /* Turn it on */
                        }
                        if(sts->mode & HAL_LED_MODE_BLINK)
                        {
                            wait = (((uint32_t)pct * (uint32_t)sts->time) / 100);
                            sts->next = time + wait;
                        }
                        else
                        {
                            /* no more blink, no more wait */
                            wait = 0;
                            /* After blinking, set the LED back to the state before it blinks */
                            HalLedSet(led, ((preBlinkState & led) != 0) ? HAL_LED_MODE_ON : HAL_LED_MODE_OFF);
                            /* Clear the saved bit */
                            preBlinkState &= (led ^ 0xFF);
                        }
                    }
                    else
                    {
                        wait = sts->next - time; /* Time left */
                    }
                    if(!next || (wait && (wait < next)))
                    {
                        next = wait;
                    }
                }
                leds ^= led;
            }
            led <<= 1;
            sts++;
        }
        if(next)
        {
            tmos_start_task(halTaskID, LED_BLINK_EVENT, next); /* Schedule event */
        }
    }
}

/*********************************************************************
 * @fn      HalLedOnOff
 *
 * @brief   Turns specified LED ON or OFF
 *
 * @param   led         - LED bit mask
 * @param   mode        - LED_ON,LED_OFF,
 *
 * @return  none
 */
void HalLedOnOff(uint8_t leds, uint8_t mode)
{
    if(leds & HAL_LED_1)
    {
        if(mode == HAL_LED_MODE_ON)
        {
            HAL_TURN_ON_LED1();
        }
        else
        {
            HAL_TURN_OFF_LED1();
        }
    }
    if(leds & HAL_LED_2)
    {
        if(mode == HAL_LED_MODE_ON)
        {
            HAL_TURN_ON_LED2();
        }
        else
        {
            HAL_TURN_OFF_LED2();
        }
    }
    if(leds & HAL_LED_3)
    {
        if(mode == HAL_LED_MODE_ON)
        {
            HAL_TURN_ON_LED3();
        }
        else
        {
            HAL_TURN_OFF_LED3();
        }
    }
    if(leds & HAL_LED_4)
    {
        if(mode == HAL_LED_MODE_ON)
        {
            HAL_TURN_ON_LED4();
        }
        else
        {
            HAL_TURN_OFF_LED4();
        }
    }
    /* Remember current state */
    if(mode)
    {
        HalLedState |= leds;
    }
    else
    {
        HalLedState &= (leds ^ 0xFF);
    }
}

/***************************************************************************************************
 * @fn      HalGetLedState
 *
 * @brief   Dim LED2 - Dim (set level) of LED2
 *
 * @return  led state
 */
uint8_t HalLedGetState()
{
    return HalLedState;
}

/******************************** endfile @ led ******************************/
