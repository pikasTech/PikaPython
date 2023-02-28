#include <stdint.h>
#include "BaseObj.h"

#include "STM32F1_common.h"

void pika_platform_sleep_ms(uint32_t ms){
    HAL_Delay(ms);
}
