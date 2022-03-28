/*
 * pmu.c
 *
 *  Created on: Oct 27, 2021
 *      Author: jiangyuanyuan
 */

/* Includes ------------------------------------------------------------------*/
#include "pmu.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure PMU                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */
PMU_HandleTypeDef hpmu;
/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void PMU_Sleep_Tim0(void)
{
	HAL_PMU_TIMER0_Start(&hpmu, 5);
	while (1)
	{
		HAL_PMU_Enter_Sleep(&hpmu);
		printf(".");
	}
}

void PMU_Sleep_IO(void)
{
	HAL_PMU_Enter_Sleep(&hpmu);
}

void PMU_Standby_Tim0(void)
{
	HAL_PMU_TIMER0_Start(&hpmu, 5);
	HAL_PMU_Enter_Standby(&hpmu);
}

void PMU_Standby_IO(void)
{
	HAL_PMU_Enter_Standby(&hpmu);
}

void MY_PMU_Init(void)
{
	hpmu.Instance = PMU;
	hpmu.ClkSource = PMU_CLKSOURCE_32RC;
	
	HAL_PMU_Init(&hpmu);
	PMU_Standby_IO();
}

void HAL_PMU_MspInit(PMU_HandleTypeDef *hpmu)
{
	HAL_NVIC_EnableIRQ(PMU_IRQn);
}

void HAL_PMU_MspDeInit(PMU_HandleTypeDef *hpmu)
{
	HAL_NVIC_DisableIRQ(PMU_IRQn);
}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */

/* USER CODE BEGIN 3 */
void HAL_PMU_Tim0_Callback(PMU_HandleTypeDef *hpmu)
{
	printf("pmu tim0 callback\r\n");
}

void HAL_PMU_IO_Callback(PMU_HandleTypeDef *hpmu)
{
	printf("pmu io callback\r\n");
}
/* USER CODE END 3 */

/************************ (C) COPYRIGHT winnermicro *****END OF FILE****/
