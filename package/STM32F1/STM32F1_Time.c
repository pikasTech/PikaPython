#include <stdint.h>
#include "BaseObj.h"
#include "STM32F1_common.h"
#include "delay.h"


void STM32F1_Time_sleep_ms(PikaObj* self, int ms) {
//    HAL_Delay(ms);
			delay_ms(ms);
	
}
void STM32F1_Time_sleep_s(PikaObj* self, int s) {
    for (int i = 0; i < s; i++) {
        delay_ms(1000);
    }
}
