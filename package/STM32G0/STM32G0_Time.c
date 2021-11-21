#include "STM32G0_Time.h"
#include <stdint.h>
#include "BaseObj.h"

#include "STM32G0_common.h"

void STM32G0_Time_sleep_ms(PikaObj* self, int ms) {
    HAL_Delay(ms);
}
void STM32G0_Time_sleep_s(PikaObj* self, int s) {
    for (int i = 0; i < s; i++) {
        HAL_Delay(1000);
    }
}
