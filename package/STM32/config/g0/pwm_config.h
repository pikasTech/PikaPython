/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-01-05     zylx         first version
 * 2019-01-08     SummerGift   clean up the code
 */

#ifndef __PWM_CONFIG_H__
#define __PWM_CONFIG_H__

#include "../../pika_hal_stm32_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef BSP_USING_PWM2
#define PWM2_CONFIG                             \
    {                                           \
       .tim_handle.Instance     = TIM2,         \
       .name                    = "pwm2",       \
       .channel                 = RT_NULL       \
    }
#endif /* BSP_USING_PWM2 */

#ifdef BSP_USING_PWM3
#define PWM3_CONFIG                             \
    {                                           \
       .tim_handle.Instance     = TIM3,         \
       .name                    = "pwm3",       \
       .channel                 = RT_NULL       \
    }
#endif /* BSP_USING_PWM2 */

#ifdef __cplusplus
}
#endif

#endif /* __PWM_CONFIG_H__ */
