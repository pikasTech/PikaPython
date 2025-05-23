/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-12-13     zylx         first version
 */

#ifndef __PWM_CONFIG_H__
#define __PWM_CONFIG_H__

#include "../../pika_hal_stm32_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef BSP_USING_PWM1
#define PWM1_CONFIG                             \
    {                                           \
       .tim_handle.Instance     = TIM1,         \
       .name                    = "pwm1",       \
       .channel                 = RT_NULL       \
    }

#endif /* BSP_USING_PWM1 */

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
#endif /* BSP_USING_PWM3 */

#ifdef BSP_USING_PWM4
#define PWM4_CONFIG                             \
    {                                           \
       .tim_handle.Instance     = TIM4,         \
       .name                    = "pwm4",       \
       .channel                 = RT_NULL       \
    }
#endif /* BSP_USING_PWM4 */

#ifdef BSP_USING_PWM5
#define PWM5_CONFIG                             \
    {                                           \
       .tim_handle.Instance     = TIM5,         \
       .name                    = "pwm5",       \
       .channel                 = RT_NULL       \
    }
#endif /* BSP_USING_PWM5 */

#ifdef BSP_USING_PWM6
#define PWM6_CONFIG                             \
    {                                           \
       .tim_handle.Instance     = TIM6,         \
       .name                    = "pwm6",       \
       .channel                 = RT_NULL       \
    }
#endif /* BSP_USING_PWM6 */

#ifdef BSP_USING_PWM7
#define PWM7_CONFIG                             \
    {                                           \
       .tim_handle.Instance     = TIM7,         \
       .name                    = "pwm7",       \
       .channel                 = RT_NULL       \
    }
#endif /* BSP_USING_PWM7 */

#ifdef BSP_USING_PWM8
#define PWM8_CONFIG                             \
    {                                           \
       .tim_handle.Instance     = TIM8,         \
       .name                    = "pwm8",       \
       .channel                 = RT_NULL       \
    }
#endif /* BSP_USING_PWM8 */

#ifdef BSP_USING_PWM9
#define PWM9_CONFIG                             \
    {                                           \
       .tim_handle.Instance     = TIM9,         \
       .name                    = "pwm9",       \
       .channel                 = RT_NULL       \
    }
#endif /* BSP_USING_PWM9 */

#ifdef BSP_USING_PWM10
#define PWM10_CONFIG                            \
    {                                           \
       .tim_handle.Instance     = TIM10,        \
       .name                    = "pwm10",      \
       .channel                 = RT_NULL       \
    }
#endif /* BSP_USING_PWM10 */

#ifdef BSP_USING_PWM11
#define PWM11_CONFIG                            \
    {                                           \
       .tim_handle.Instance     = TIM11,        \
       .name                    = "pwm11",      \
       .channel                 = RT_NULL       \
    }
#endif /* BSP_USING_PWM11 */

#ifdef BSP_USING_PWM12
#define PWM12_CONFIG                            \
    {                                           \
       .tim_handle.Instance     = TIM12,        \
       .name                    = "pwm12",      \
       .channel                 = RT_NULL       \
    }
#endif /* BSP_USING_PWM12 */

#ifdef BSP_USING_PWM13
#define PWM13_CONFIG                            \
    {                                           \
       .tim_handle.Instance     = TIM13,        \
       .name                    = "pwm13",      \
       .channel                 = RT_NULL       \
    }
#endif /* BSP_USING_PWM13 */

#ifdef __cplusplus
}
#endif

#endif /* __PWM_CONFIG_H__ */
