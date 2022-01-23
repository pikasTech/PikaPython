#include <stdint.h>
#include "BaseObj.h"
#include "STM32G0_ADC.h"
#include "STM32G0_common.h"
#include "dataStrs.h"
#include "main.h"

void STM32G0_Task_platformGetTick(PikaObj* self) {
    obj_setInt(self, "tick", HAL_GetTick());
}
