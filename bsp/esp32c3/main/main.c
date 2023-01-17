#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_vfs_fat.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "sdmmc_cmd.h"

#include "esp_http_client.h"
#include "esp_https_ota.h"
#include "esp_ota_ops.h"
#include "esp_wifi.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "pikaScript.h"

static volatile char gRXC = EOF;
static volatile uint8_t gNeedPut = 0;

char pika_platform_getchar() {
    gRXC = getc(stdin);
    while (gRXC == (char)EOF) {
        gRXC = getc(stdin);
        /* Replace (fflush(stdout) or printf("\n"))'s function!!! */
        if (gNeedPut) {
#if CONFIG_IDF_TARGET_ESP32C3
            uint32_t EP1_Conf_Reg = 0x00;
            /* Read USB_SERIAL_JTAG_EP1_CONF_REG (BaseAddr: 0x60043000
             * Offset: 0x0004) */
            EP1_Conf_Reg = *((uint32_t*)(0x60043004));

            /* Set 'USB_SERIAL_JTAG_WR_DONE' bit*/
            EP1_Conf_Reg |= 0x01;

            /* Write USB_SERIAL_JTAG_EP1_CONF_REG Back */
            *((uint32_t*)(0x60043004)) = EP1_Conf_Reg;

            gNeedPut = 0;
#else
            fflush(stdout);
#endif
        }
        vTaskDelay(1);
    }
    return gRXC;
}

int pika_platform_putchar(char ch) {
    putc(ch, stdout);
    gNeedPut = 1;
    return 0;
}

void app_main(void) {
    printf("Minimum free heap size: %" PRIu32 " bytes\n",
           esp_get_minimum_free_heap_size());

    pikaScriptShell(pikaScriptInit());
}
