
#include <stdio.h>
#include "wm_hal.h"

PMU_HandleTypeDef hpmu;

static void RTC_Init(void);
static void RTC_Demo(void);
static void RTC_Alarm_Demo(void);
void Error_Handler(void);

int main(void)
{
	SystemClock_Config(CPU_CLK_160M);
	printf("enter main\r\n");
	
	RTC_Demo();
//	RTC_Alarm_Demo();
	while (1)
	{
		printf(".");
		HAL_Delay(1000);
	}
}

static void RTC_Demo(void)
{
	RTC_TimeTypeDef time;
	
	RTC_Init();
	time.Year = 121;
	time.Month = 6;
	time.Date = 10;
	time.Hours = 14;
	time.Minutes = 28;
	time.Seconds = 10;
	HAL_PMU_RTC_Start(&hpmu, &time);

	while (1)
	{
		HAL_PMU_RTC_GetTime(&hpmu, &time);
		printf("%d-%d-%d %d:%d:%d\r\n", (time.Year + 1900), time.Month, time.Date, time.Hours, time.Minutes, time.Seconds);
		HAL_Delay(1000);
	}
}

static void RTC_Alarm_Demo(void)
{
	RTC_TimeTypeDef time;
	
	RTC_Init();
	time.Year = 121;
	time.Month = 6;
	time.Date = 10;
	time.Hours = 14;
	time.Minutes = 28;
	time.Seconds = 10;
	HAL_PMU_RTC_Start(&hpmu, &time);
	
	time.Seconds = 15;
	HAL_PMU_RTC_Alarm_Enable(&hpmu, &time);
	
	while (1)
	{
		HAL_PMU_RTC_GetTime(&hpmu, &time);
		printf("%d-%d-%d %d:%d:%d\r\n", (time.Year + 1900), time.Month, time.Date, time.Hours, time.Minutes, time.Seconds);
		HAL_Delay(1000);
	}
}

static void RTC_Init(void)
{
	hpmu.Instance = PMU;
	hpmu.ClkSource = PMU_CLKSOURCE_32RC;
	HAL_PMU_Init(&hpmu);
}

void HAL_PMU_RTC_Callback(PMU_HandleTypeDef *hpmu)
{
	printf("rtc irq callback\r\n");
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
