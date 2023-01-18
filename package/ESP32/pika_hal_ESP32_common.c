#include "pika_hal_ESP32_common.h"
#include "PikaObj.h"
#include "dataStrs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

int getGpioPin(char* pin) {
    int num = -1;
    sscanf(pin, "P%d\n", &num);
    return num;
}

uint32_t getPinMode(char* mode) {
    if (strEqu(mode, "out")) {
        return GPIO_MODE_OUTPUT;
    } else if (strEqu(mode, "in")) {
        return GPIO_MODE_INPUT;
    } else if (strEqu(mode, "out_od")) {
        return GPIO_MODE_OUTPUT_OD;
    }

    return GPIO_MODE_DISABLE;
}

void pika_platform_sleep_ms(uint32_t ms) {
    vTaskDelay(pdMS_TO_TICKS(ms));
}

void pika_platform_sleep_s(uint32_t s) {
    vTaskDelay(pdMS_TO_TICKS(1000 * s));
}
