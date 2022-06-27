#include <stdint.h>
#include "BaseObj.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "ESP32_common.h"

void ESP32_Time_sleep_ms(PikaObj *self, int ms)
{
    vTaskDelay(pdMS_TO_TICKS(ms));
}
void ESP32_Time_sleep_s(PikaObj *self, int s)
{
    vTaskDelay(pdMS_TO_TICKS(1000 * s));
}
