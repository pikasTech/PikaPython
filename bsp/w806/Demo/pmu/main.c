
#include <stdio.h>
#include "wm_hal.h"

PMU_HandleTypeDef hpmu;
void Error_Handler(void);
static void PMU_Init(void);
static void PMU_Sleep_Tim0(void);
static void PMU_Sleep_IO(void);
static void PMU_Standby_Tim0(void);
static void PMU_Standby_IO(void);

int main(void)
{
	SystemClock_Config(CPU_CLK_160M);
	printf("enter main\r\n");
	
	PMU_Init();
	PMU_Standby_IO();
	while (1)
	{
		printf(".");
		HAL_Delay(1000);
	}
}

static void PMU_Init(void)
{
	hpmu.Instance = PMU;
	hpmu.ClkSource = PMU_CLKSOURCE_32RC;
	
	HAL_PMU_Init(&hpmu);
}

static void PMU_Sleep_Tim0(void)
{
	HAL_PMU_TIMER0_Start(&hpmu, 5);
	while (1)
	{
		HAL_PMU_Enter_Sleep(&hpmu);
		printf(".");
	}
}

static void PMU_Sleep_IO(void)
{
	HAL_PMU_Enter_Sleep(&hpmu);
}

static void PMU_Standby_Tim0(void)
{
	HAL_PMU_TIMER0_Start(&hpmu, 5);
	HAL_PMU_Enter_Standby(&hpmu);
}

static void PMU_Standby_IO(void)
{
	HAL_PMU_Enter_Standby(&hpmu);
}

void HAL_PMU_Tim0_Callback(PMU_HandleTypeDef *hpmu)
{
	printf("pmu tim0 callback\r\n");
}

void HAL_PMU_IO_Callback(PMU_HandleTypeDef *hpmu)
{
	printf("pmu io callback\r\n");
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