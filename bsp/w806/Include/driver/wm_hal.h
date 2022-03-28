#ifndef __WM_HAL_H__
#define __WM_HAL_H__

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "wm_type_def.h"
#include "wm_regs.h"
#include "wm_cpu.h"
#include "wm_gpio.h"
#include "wm_gpio_ex.h"
#include "wm_uart.h"
#include "wm_rcc.h"
#include "wm_spi.h"
#include "wm_wdg.h"
#include "wm_tim.h"
#include "wm_internal_flash.h"
#include "wm_adc.h"
#include "wm_pwm.h"
#include "wm_pmu.h"
#include "wm_spi_flash.h"
#include "wm_i2c.h"
#include "wm_touch.h"
#include "wm_i2s.h"
#include "wm_dma.h"

#define VER "0.6.0"
#define USE_FULL_ASSERT

#define __HAL_LOCK(__HANDLE__)                                           \
                                do{                                        \
                                    if((__HANDLE__)->Lock == HAL_LOCKED)   \
                                    {                                      \
                                       return HAL_BUSY;                    \
                                    }                                      \
                                    else                                   \
                                    {                                      \
                                       (__HANDLE__)->Lock = HAL_LOCKED;    \
                                    }                                      \
                                  }while (0)

#define __HAL_UNLOCK(__HANDLE__)                                          \
                                  do{                                       \
                                      (__HANDLE__)->Lock = HAL_UNLOCKED;    \
                                    }while (0)
										

HAL_StatusTypeDef HAL_Init(void);
HAL_StatusTypeDef HAL_DeInit(void);
void HAL_MspInit(void);
void HAL_MspDeInit(void);

#ifdef  USE_FULL_ASSERT
#define assert_param(expr) ((expr) ? (void)0U : assert_failed((uint8_t *)__FILE__, __LINE__))
void assert_failed(uint8_t* file, uint32_t line);
#else
#define assert_param(expr) ((void)0U)
#endif

#endif