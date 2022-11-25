#include <FreeRTOS.h>
#include <aos/yloop.h>
#include <bl_gpio.h>
#include <cli.h>
#include <stdio.h>
#include <task.h>
#include <vfs.h>
#include "hosal_uart.h"
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

void main(void) {
    bl_gpio_enable_input(4, 0, 0);
    bl_gpio_output_set(4, false);
    printf("[ Info] In PikaSciprt Demo...\r\n");
    root = pikaScriptInit();
    pikaScriptShell(root);
}
