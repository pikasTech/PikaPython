
#include <stdio.h>
#include <string.h>
#include "wm_hal.h"

TOUCH_HandleTypeDef htouch;

static void Touch_init(void);
void Error_Handler(void);

int main(void)
{
	SystemClock_Config(CPU_CLK_160M);
	printf("enter main\r\n");
	
	Touch_init();
	
	while(1)
	{	
		HAL_Delay(1000);
	}
	
    return 0;
}

// 在使用TOUCH功能时，TOUCH0（PA7）必须复用为TOUCH功能，不可以作为其他功能！！！

static void Touch_init(void)
{
	htouch.Instance = TOUCH;
	htouch.Init.Channel = TOUCH_CH_0 | TOUCH_CH_8 | TOUCH_CH_9 | TOUCH_CH_11 | TOUCH_CH_12 | TOUCH_CH_13 | TOUCH_CH_14;
	htouch.Init.ScanPeriod = 16;
	htouch.Init.Window = 32;
	htouch.Init.Threshold[0] = 120;
	htouch.Init.Threshold[8] = 120;
	htouch.Init.Threshold[9] = 120;
	htouch.Init.Threshold[11] = 120;
	htouch.Init.Threshold[12] = 120;
	htouch.Init.Threshold[13] = 120;
	htouch.Init.Threshold[14] = 120;
	
	htouch.Init.Irq_en = 1 << 0 | 1 << 8 | 1 << 9 | 1 << 11 | 1 << 12 | 1 << 13 | 1 << 14;	
	
	HAL_TOUCH_Init(&htouch);
}

void HAL_TOUCH_Callback(TOUCH_HandleTypeDef *htouch, uint16_t Flag)
{
	printf("%x\r\n", Flag);
}

void Error_Handler(void)
{
	while (1)
	{
	}
}

void assert_failed(uint8_t *file, uint32_t line)
{
	printf("Wrong parameters value: file %s on line %d\r\n", file, line);
}