/*
 * main.c
 *
 *  Created on: Oct 27, 2021
 *      Author: derek
 */

#include "wm_hal.h"
#include "freeRTOS.h"
#include "portmacro.h"
#include "pikascript/pikascript-api/pikaScript.h"
#include "mytask.h"

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();
	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config(CPU_CLK_240M);
	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	
	/* Call init function for freertos objects (in freertos.c) */
	MX_FREERTOS_Init();
	/* Start scheduler */
	/* We should never get here as control is now taken by the scheduler */
	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		HAL_Delay(10);
	}
  /* USER CODE END 3 */
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