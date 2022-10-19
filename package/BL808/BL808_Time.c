#include "BL808_Time.h"
#include <FreeRTOS.h>
#include <task.h>

void BL808_Time_sleep_ms(PikaObj* self, int ms) {
    vTaskDelay(pdMS_TO_TICKS(ms));
}

void BL808_Time_sleep_s(PikaObj* self, int s) {
    vTaskDelay(pdMS_TO_TICKS(1000 * s));
}
