/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "PikaObj.h"
#include "MyRoot.h"
#include "SysObj.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void LED_off(PikaObj *self)
{
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
}
void LED_on(PikaObj *self)
{
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
}

void Uart_printName(PikaObj *self)
{
		char *name = obj_getStr(self, "name");
		if(NULL == name)
		{
			printf("[error] Uart: can't find name.\r\n");
			return;
		}
		
		printf("%s\r\n", name);
}

void Uart_setName(PikaObj *self, char * name)
{
		obj_setStr(self, "name", name);
}

void Uart_send(PikaObj *self, char * data)
{
		printf("[uart1]: %s\r\n", data);
}

void MemoryChecker_max(PikaObj *self)
{
		printf("memory used max = %0.2f kB\r\n", pikaMemMax() / 1024.0);

}
void MemoryChecker_now(PikaObj *self)
{
		printf("memory used now = %0.2f kB\r\n", pikaMemNow() / 1024.0);
}

void obj_runWithInfo(PikaObj *self, char *cmd)
{
		printf(">>> %s\r\n", cmd);
		obj_run(self, cmd);
}


char inputBuff[256] = {0};
/* USER CODE END 0 */

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
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	/* user input buff */
	PikaObj *root = newRootObj("root",New_MyRoot);
	obj_runWithInfo(root, "print('hello world')");
	obj_runWithInfo(root, "ls()");
	obj_runWithInfo(root, "ls('__classLoader')");
	obj_runWithInfo(root, "new('mem', 'MemoryChecker')");
	obj_runWithInfo(root, "ls()");
	obj_runWithInfo(root, "mem.max()");
	obj_runWithInfo(root, "mem.now()");
	obj_runWithInfo(root, "type('mem')");

	obj_runWithInfo(root, "new('led', 'LED')");
	obj_runWithInfo(root, "mem.now()");
	obj_runWithInfo(root, "led.on()");
	obj_runWithInfo(root, "type('led')");
	obj_runWithInfo(root, "del('led')");

	obj_runWithInfo(root, "set('a',1)");
	obj_runWithInfo(root, "print(a)");
	obj_runWithInfo(root, "type('a')");
	obj_runWithInfo(root, "del('a')");

	obj_runWithInfo(root, "set('a',1.1)");
	obj_runWithInfo(root, "print(a)");
	obj_runWithInfo(root, "type('a')");
	obj_runWithInfo(root, "del('a')");

	obj_runWithInfo(root, "set('a','test')");
	obj_runWithInfo(root, "print(a)");
	obj_runWithInfo(root, "type('a')");
	obj_runWithInfo(root, "del('a')");

	obj_runWithInfo(root, "set('a',1)");
	obj_runWithInfo(root, "set('b',a)");
	obj_runWithInfo(root, "print(b)");
	obj_runWithInfo(root, "del('a')");
	obj_runWithInfo(root, "del('b')");

	obj_runWithInfo(root, "ls()");
	obj_runWithInfo(root, "mem.now()");
	obj_runWithInfo(root, "del('mem')");

	obj_runWithInfo(root, "new('mem', 'MemoryChecker')");
	obj_runWithInfo(root, "mem.max()");
	obj_runWithInfo(root, "mem.now()");
		
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

		/* get user input */
		fgets(inputBuff, sizeof(inputBuff), stdin);

		/* run mimiScript and get res */
		Args *resArgs = obj_runDirect(root, inputBuff);

		/* get system output of mimiScript*/
		char *sysOut = args_getStr(resArgs, "sysOut");

		if (NULL != sysOut)
		{
			/* print out the system output */
			printf("%s\r\n", sysOut);
		}

		/* deinit the res */
		args_deinit(resArgs);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
