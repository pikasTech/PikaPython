/* Contributer: kirto 沧御 */

/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "pikaScript.h"

#define RX_Buff_SIZE 256
char rxBuff[RX_Buff_SIZE] = {0};

void clearBuff(char* buff, uint32_t size) {
    for (int i = 0; i < size; i++) {
        buff[i] = 0;
    }
}

char pika_getchar() {
    char chBuff[2] = {0};
    while (1) {
        fgets(chBuff, 2, stdin);
        if (chBuff[0] != 0) {
            return chBuff[0];
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

char __platform_getchar(void) {
    return pika_getchar();
}

void app_main(void) {
    printf(
        "------------------------------------------------------------------"
        "\r\n");
    printf(
        "|                                                                "
        "|\r\n");
    printf(
        "|     ____   _   __            _____              _          __  "
        "|\r\n");
    printf(
        "|    / __ \\ (_) / /__ ____ _  / ___/ _____ _____ (_) ____   / /_ "
        "|\r\n");
    printf(
        "|   / /_/ // / / //_// __ `/  \\__ \\ / ___// ___// / / __ \\ / __/ "
        "|\r\n");
    printf(
        "|  / ____// / / ,<  / /_/ /  ___/ // /__ / /   / / / /_/ // /_   "
        "|\r\n");
    printf(
        "| /_/    /_/ /_/|_| \\__,_/  /____/ \\___//_/   /_/ / .___/ \\__/   "
        "|\r\n");
    printf(
        "|                                                /_/             "
        "|\r\n");
    printf(
        "|          PikaScript - An Ultra Lightweight Python Engine       "
        "|\r\n");
    printf(
        "|                                                                "
        "|\r\n");
    printf(
        "|           [ https://github.com/pikastech/pikascript ]          "
        "|\r\n");
    printf(
        "|           [  https://gitee.com/lyon1998/pikascript  ]          "
        "|\r\n");
    printf(
        "|                                                                "
        "|\r\n");
    printf(
        "------------------------------------------------------------------"
        "\r\n");
    PikaObj* pikaMain = pikaScriptInit();
    pikaScriptShell(pikaMain);
}
