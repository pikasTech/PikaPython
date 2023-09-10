#include "pika_hal_stm32_common.h"

int64_t pika_platform_get_tick(void) {
    return HAL_GetTick();
}
