
#include <stdio.h>
#include "wm_hal.h"

void Error_Handler(void);
static void WDG_Init(void);

WDG_HandleTypeDef hwdg;

int main(void)
{
	SystemClock_Config(CPU_CLK_160M);
	printf("enter main\r\n");
	
	WDG_Init();
	while (1)
	{
		printf(".");
		HAL_Delay(1000);
	}
}

static void WDG_Init(void)
{
	hwdg.Instance = WDG;
	hwdg.Init.Reload = 5 * 1000 * 1000; // 5s
	if (HAL_WDG_Init(&hwdg) != HAL_OK)
	{
		Error_Handler();
	}
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