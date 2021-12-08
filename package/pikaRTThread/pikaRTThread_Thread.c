/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-12-08     lyon       the first version
 */
#include <rtthread.h>
#include <pikaScript.h>

void pikaRTThread_Thread_mdelay(PikaObj *self, int ms){
    rt_thread_mdelay(ms);
}
