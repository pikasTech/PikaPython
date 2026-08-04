/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author            Notes
 * 2018-10-30     SummerGift        first version
 * 2020-10-14     Dozingfiretruck   Porting for stm32wbxx
 */

#ifndef __DRV_CONFIG_H__
#define __DRV_CONFIG_H__

#include "pika_hal_stm32_common.h"

#ifndef PIKA_HAL
#include <board.h>
#include <rtthread.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(SOC_SERIES_STM32F0)
#include "./config/f0/dma_config.h"
#include "./config/f0/uart_config.h"
#include "./config/f0/spi_config.h"
#include "./config/f0/tim_config.h"
#include "./config/f0/pwm_config.h"
#include "./config/f0/adc_config.h"
#elif defined(SOC_SERIES_STM32F1)
#include "./config/f1/dma_config.h"
#include "./config/f1/uart_config.h"
#include "./config/f1/spi_config.h"
#include "./config/f1/adc_config.h"
#include "./config/f1/tim_config.h"
#include "./config/f1/sdio_config.h"
#include "./config/f1/pwm_config.h"
#include "./config/f1/usbd_config.h"
#include "./config/f1/pulse_encoder_config.h"
#elif  defined(SOC_SERIES_STM32F2)
#include "./config/f2/dma_config.h"
#include "./config/f2/uart_config.h"
#include "./config/f2/spi_config.h"
#include "./config/f2/adc_config.h"
#include "./config/f2/tim_config.h"
#include "./config/f2/sdio_config.h"
#include "./config/f2/pwm_config.h"
#elif  defined(SOC_SERIES_STM32F3)
#include "./config/f3/uart_config.h"
#include "./config/f3/tim_config.h"
#include "./config/f3/pwm_config.h"
#include "./config/f3/dma_config.h"
#elif  defined(SOC_SERIES_STM32F4)
#include "./config/f4/dma_config.h"
#include "./config/f4/uart_config.h"
#include "./config/f4/spi_config.h"
#include "./config/f4/qspi_config.h"
#include "./config/f4/usbd_config.h"
#include "./config/f4/adc_config.h"
#include "./config/f4/dac_config.h"
#include "./config/f4/tim_config.h"
#include "./config/f4/sdio_config.h"
#include "./config/f4/pwm_config.h"
#include "./config/f4/pulse_encoder_config.h"
#elif  defined(SOC_SERIES_STM32F7)
#include "./config/f7/dma_config.h"
#include "./config/f7/uart_config.h"
#include "./config/f7/spi_config.h"
#include "./config/f7/qspi_config.h"
#include "./config/f7/adc_config.h"
#include "./config/f7/tim_config.h"
#include "./config/f7/sdio_config.h"
#include "./config/f7/pwm_config.h"
#elif  defined(SOC_SERIES_STM32L0)
#include "./config/l0/dma_config.h"
#include "./config/l0/uart_config.h"
#elif  defined(SOC_SERIES_STM32L4)
#include "./config/l4/dma_config.h"
#include "./config/l4/uart_config.h"
#include "./config/l4/spi_config.h"
#include "./config/l4/qspi_config.h"
#include "./config/l4/adc_config.h"
#include "./config/l4/dac_config.h"
#include "./config/l4/tim_config.h"
#include "./config/l4/sdio_config.h"
#include "./config/l4/pwm_config.h"
#include "./config/l4/usbd_config.h"
#elif  defined(SOC_SERIES_STM32L5)
#include "./config/l5/dma_config.h"
#include "./config/l5/uart_config.h"
#include "./config/l5/spi_config.h"
#include "./config/l5/qspi_config.h"
#include "./config/l5/adc_config.h"
#include "./config/l5/dac_config.h"
#include "./config/l5/tim_config.h"
#include "./config/l5/sdio_config.h"
#include "./config/l5/pwm_config.h"
#include "./config/l5/usbd_config.h"
#elif  defined(SOC_SERIES_STM32G0)
#include "./config/g0/dma_config.h"
#include "./config/g0/uart_config.h"
#include "./config/g0/spi_config.h"
#include "./config/g0/adc_config.h"
#include "./config/g0/tim_config.h"
#include "./config/g0/pwm_config.h"
#elif  defined(SOC_SERIES_STM32G4)
#include "./config/g4/dma_config.h"
#include "./config/g4/uart_config.h"
#include "./config/g4/spi_config.h"
#include "./config/g4/qspi_config.h"
#include "./config/g4/usbd_config.h"
#include "./config/g4/adc_config.h"
#include "./config/g4/tim_config.h"
#include "./config/g4/sdio_config.h"
#include "./config/g4/pwm_config.h"
#include "./config/g4/pulse_encoder_config.h"
#elif  defined(SOC_SERIES_STM32H7)
#include "./config/h7/dma_config.h"
#include "./config/h7/uart_config.h"
#include "./config/h7/spi_config.h"
#include "./config/h7/qspi_config.h"
#include "./config/h7/adc_config.h"
#include "./config/h7/dac_config.h"
#include "./config/h7/tim_config.h"
#include "./config/h7/sdio_config.h"
#include "./config/h7/pwm_config.h"
#include "./config/h7/usbd_config.h"
#elif defined(SOC_SERIES_STM32U5)
#include "./config/u5/dma_config.h"
#include "./config/u5/uart_config.h"
#include "./config/u5/spi_config.h"
#include "./config/u5/qspi_config.h"
#include "./config/u5/adc_config.h"
#include "./config/u5/dac_config.h"
#include "./config/u5/tim_config.h"
#include "./config/u5/sdio_config.h"
#include "./config/u5/pwm_config.h"
#include "./config/u5/usbd_config.h"
#elif  defined(SOC_SERIES_STM32MP1)
#include "./config/mp1/dma_config.h"
#include "./config/mp1/uart_config.h"
#include "./config/mp1/qspi_config.h"
#include "./config/mp1/spi_config.h"
#include "./config/mp1/adc_config.h"
#include "./config/mp1/dac_config.h"
#include "./config/mp1/tim_config.h"
#include "./config/mp1/pwm_config.h"
#elif  defined(SOC_SERIES_STM32WL)
#include "./config/wl/dma_config.h"
#include "./config/wl/uart_config.h"
#include "./config/wl/spi_config.h"
#include "./config/wl/tim_config.h"
#elif  defined(SOC_SERIES_STM32WB)
#include "./config/wb/adc_config.h"
#include "./config/wb/dma_config.h"
#include "./config/wb/pwm_config.h"
#include "./config/wb/qspi_config.h"
#include "./config/wb/spi_config.h"
#include "./config/wb/tim_config.h"
#include "./config/wb/uart_config.h"
#include "./config/wb/usbd_config.h"
#endif


#ifndef UART1_CONFIG
#undef BSP_USING_UART1
#endif

#ifndef UART2_CONFIG
#undef BSP_USING_UART2
#endif

#ifndef UART3_CONFIG
#undef BSP_USING_UART3
#endif

#ifndef UART4_CONFIG
#undef BSP_USING_UART4
#endif

#ifndef UART5_CONFIG
#undef BSP_USING_UART5
#endif

#ifndef UART6_CONFIG
#undef BSP_USING_UART6
#endif

#ifndef UART7_CONFIG
#undef BSP_USING_UART7
#endif

#ifndef UART8_CONFIG
#undef BSP_USING_UART8
#endif

#ifdef __cplusplus
}
#endif

#endif
