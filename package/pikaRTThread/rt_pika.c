/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-12-07     lyon       the first version
 */
#include <rtthread.h>
#include <pikaScript.h>


#define PIKASCRIPT_STACK_SIZE   4096
#define PIKASCRIPT_STACK_PRIO   20

static void pikascript_entry(void *parameter)
{
    pikaScriptInit();
}

static int rt_pika_init(void)
{
    rt_thread_t tid;
    tid = rt_thread_create("PikaScript",
                            pikascript_entry, RT_NULL,
                            PIKASCRIPT_STACK_SIZE,
                            PIKASCRIPT_STACK_PRIO, 0);

    if (tid != RT_NULL)
    {
        rt_thread_startup(tid);
    }
    else
    {

    }

    return 0;
}

INIT_APP_EXPORT(rt_pika_init);
