#include <FreeRTOS.h>
#include <aos/yloop.h>
#include <bl_gpio.h>
#include <cli.h>
#include <hosal_timer.h>
#include <hosal_uart.h>
#include <stdio.h>
#include <task.h>
#include <vfs.h>
#include "../pikascript/pikascript-lib/PikaStdDevice/pika_hal.h"
#include "LCD_driver.h"
#include "Touch_driver.h"
#include "lv_conf.h"
#include "lv_port_disp.h"
#include "lvgl.h"
#include "pikaScript.h"

volatile PikaObj* root = NULL;
static volatile char rxbuff[1024 * 10];
static volatile int rxsize = 0;
static volatile int rxbusy = 0;

char __platform_getchar() {
    char inbuf[2] = {0};
    int ret = -1;
    extern hosal_uart_dev_t uart_stdio;
    while (ret != 1) {
        ret = hosal_uart_receive(&uart_stdio, inbuf, 1);
        vTaskDelay(1);
    }
    return inbuf[0];
}

/* file API */
FILE* __platform_fopen(const char* filename, const char* modes) {
    return fopen(filename, modes);
}

int __platform_fclose(FILE* stream) {
    return fclose(stream);
}

size_t __platform_fwrite(const void* ptr, size_t size, size_t n, FILE* stream) {
    return fwrite(ptr, size, n, stream);
}

size_t __platform_fread(void* ptr, size_t size, size_t n, FILE* stream) {
    return fread(ptr, size, n, stream);
}

int __platform_fseek(FILE* stream, long offset, int whence) {
    return fseek(stream, offset, whence);
}

long __platform_ftell(FILE* stream) {
    return ftell(stream);
}

// static void timer_cb(void* arg) {
//     lv_tick_inc(1);
// }

GT911_Config_t sampleConfig = {.X_Resolution = 320,
                               .Y_Resolution = 480,
                               .Number_Of_Touch_Support = 5,
                               .ReverseY = false,
                               .ReverseX = false,
                               .SwithX2Y = false,
                               .SoftwareNoiseReduction = false};

TouchCordinate_t cordinate[5];
uint8_t number;

void main(void) {
    printf("[ Info] In PikaSciprt Demo...\r\n");

    GT911_Init(sampleConfig);
    while (1) {
        GT911_ReadTouch(cordinate, &number);
        for (int i = 0; i < number; i++) {
            printf("Touch[%d] = (%d, %d)\r\n", i, cordinate[i].x,
                   cordinate[i].y);
        }
    }
    LCD_init();
    while (1) {
        LCD_clear(RED);
        vTaskDelay(200);
        LCD_clear(BLUE);
        vTaskDelay(200);
        LCD_clear(GREEN);
        vTaskDelay(200);
    }
#if 0
    static hosal_timer_dev_t lv_timer_dev = {
        .config =
            {
                .arg = NULL,
                .cb = timer_cb,
                .period = 1000,
                .reload_mode = TIMER_RELOAD_PERIODIC,
            },
        .port = 0,
    };
    lv_init();
    lv_port_disp_init();

    hosal_timer_init(&lv_timer_dev);
    hosal_timer_start(&lv_timer_dev);

    lv_obj_t* label1 = lv_label_create(lv_scr_act());

    lv_label_set_text(label1, "Hello lvgl");
    lv_obj_align(label1, LV_ALIGN_CENTER, 1, 1);

   while (1) {
       vTaskDelay(10 / portTICK_PERIOD_MS);
       lv_timer_handler();
   }
#endif
    root = pikaScriptInit();
    pikaScriptShell((PikaObj*)root);
}
