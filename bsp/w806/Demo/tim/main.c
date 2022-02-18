
#include <stdio.h>
#include "wm_hal.h"

TIM_HandleTypeDef htim0;
TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim5;

void Error_Handler(void);
static void TIM0_Init(void);
static void TIM1_Init(void);
static void TIM2_Init(void);
static void TIM3_Init(void);
static void TIM4_Init(void);
static void TIM5_Init(void);

int main(void)
{
	SystemClock_Config(CPU_CLK_160M);
	printf("enter main\r\n");

	TIM0_Init();
	HAL_TIM_Base_Start_IT(&htim0);
	TIM1_Init();
	HAL_TIM_Base_Start_IT(&htim1);
	TIM2_Init();
	HAL_TIM_Base_Start_IT(&htim2);
	TIM3_Init();
	HAL_TIM_Base_Start_IT(&htim3);
	TIM4_Init();
	HAL_TIM_Base_Start_IT(&htim4);
	TIM5_Init();
	HAL_TIM_Base_Start_IT(&htim5);
	
	while(1)
	{
		HAL_Delay(1000);
	}
}

static void TIM0_Init(void)
{
	htim0.Instance = TIM0;
	htim0.Init.Unit = TIM_UNIT_US;
	htim0.Init.Period = 1000000;
	htim0.Init.AutoReload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	if (HAL_TIM_Base_Init(&htim0) != HAL_OK)
	{
		Error_Handler();
	}
}

static void TIM1_Init(void)
{
	htim1.Instance = TIM1;
	htim1.Init.Unit = TIM_UNIT_US;
	htim1.Init.Period = 1001000;
	htim1.Init.AutoReload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
	{
		Error_Handler();
	}
}

static void TIM2_Init(void)
{
	htim2.Instance = TIM2;
	htim2.Init.Unit = TIM_UNIT_US;
	htim2.Init.Period = 1002000;
	htim2.Init.AutoReload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
	{
		Error_Handler();
	}
}

static void TIM3_Init(void)
{
	htim3.Instance = TIM3;
	htim3.Init.Unit = TIM_UNIT_US;
	htim3.Init.Period = 1003000;
	htim3.Init.AutoReload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
	{
		Error_Handler();
	}
}

static void TIM4_Init(void)
{
	htim4.Instance = TIM4;
	htim4.Init.Unit = TIM_UNIT_US;
	htim4.Init.Period = 1004000;
	htim4.Init.AutoReload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
	{
		Error_Handler();
	}
}

static void TIM5_Init(void)
{
	htim5.Instance = TIM5;
	htim5.Init.Unit = TIM_UNIT_US;
	htim5.Init.Period = 1005000;
	htim5.Init.AutoReload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	if (HAL_TIM_Base_Init(&htim5) != HAL_OK)
	{
		Error_Handler();
	}
}

void HAL_TIM_Callback(TIM_HandleTypeDef *htim)
{
	printf("%d ", htim->Instance);
	if (htim->Instance == TIM0)
	{
		
	}
	if (htim->Instance == TIM1)
	{
		
	}
	if (htim->Instance == TIM2)
	{
		
	}
	if (htim->Instance == TIM3)
	{
		
	}
	if (htim->Instance == TIM4)
	{
		
	}
	if (htim->Instance == TIM5)
	{
		
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