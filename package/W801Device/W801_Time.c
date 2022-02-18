#include <stdint.h>
#include "BaseObj.h"
#include "dataStrs.h"
#include "W801Device_Time.h"
#include "wm_hal.h"
#include "FreeRTOS.h"


void W801Device_Time_sleep_ms(PikaObj *self, int ms)
{
	vTaskDelay(1000);

}
void W801Device_Time_sleep_s(PikaObj *self, int s)
{
	vTaskDelay(s*1000);

}
