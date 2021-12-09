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

#ifdef PKG_PIKASCRIPT_USING_AUTORUNNING

#ifdef PKG_PIKASCRIPT_STACK_SIZE
#define PIKASCRIPT_STACK_SIZE   PKG_PIKASCRIPT_STACK_SIZE
#else
#define PIKASCRIPT_STACK_SIZE   4096
#endif

#ifdef PKG_PIKASCRIPT_STACK_PRIO
#define PIKASCRIPT_STACK_PRIO   PKG_PIKASCRIPT_STACK_PRIO
#else
#define PIKASCRIPT_STACK_PRIO   20
#endif

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

#else
static int pika_main(int argc, char *argv[])
{
    pikaScriptInit();
    return 0;
}
MSH_CMD_EXPORT_ALIAS(pika_main, pika, run PikaScript);

#endif /* PKG_PIKASCRIPT_USING_AUTORUNNING */
