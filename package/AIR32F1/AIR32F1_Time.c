#include "AIR32F1_Time.h"
#include "AIR32F1_common.h"

void AIR32F1_Time_sleep_ms(PikaObj *self, int ms){
	Delay_Ms(ms);
}
void AIR32F1_Time_sleep_s(PikaObj *self, int s){
	Delay_Ms(s*1000);
}

