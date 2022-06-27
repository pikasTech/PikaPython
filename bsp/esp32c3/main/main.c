/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include "esp_log.h"
#include "pikascript.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"

char __platform_getchar()
{
    char data = 0xff;
    do
    {
        vTaskDelay(1);
        data = getchar();
    } while (data == 0xff);
    return data;
}

void app_main(void)
{
    pikaScriptShell(pikaScriptInit());
}
