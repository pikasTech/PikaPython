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

static int rt_pika_main(void)
{
    PikaObj * pikaMain = pikaScriptInit();
    printf(">>>\r\n");
    while(1){

    }
    return 0;
}
INIT_APP_EXPORT(rt_pika_main);
