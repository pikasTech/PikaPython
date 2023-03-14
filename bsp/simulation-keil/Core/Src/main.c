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
#ifdef USING_PERF_COUNTER
#include "perf_counter.h"
#endif
#include "PikaVM.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "pikaScript.h"
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


char __platform_getchar(void) {
    uint8_t ch;
    while(1){
        if(HAL_OK == HAL_UART_Receive(&huart1, &ch, 1, 0)){
            return ch;
        }
    }
}



static void prime_number_100_c(void) {
    volatile int num = 0;
    /* run */
    for (volatile int i = 2; i < 100; i++) {
        volatile int is_prime = 1;
        for (volatile int j = 2; j < i; j++) {
            if (i % j == 0) {
                is_prime = 0;
                break;
            }
        }
        if (is_prime) {
            num = num + i;
        }
    }
    if ( num != 1060){
        printf("[error] in benchmark prime_number_100_c \r\n");
    }
}


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

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  /* user input buff */
    PikaObj* pikaMain = NULL;

    /* run unit test */
    pikaMain = pikaScriptInit();
    
    #ifdef USING_PERF_COUNTER

    /* benchmark */
    uint64_t nCycleUsed_c,nCycleUsed_pika = 0;
    printf("[----------benchmark----------]\r\n");
    __cycleof__("", {
            nCycleUsed_c = _;
            printf("[prime_number_100_c] Cycle Used %lld\r\n", _);

        }) {
        prime_number_100_c();
    }
        
    /* create pikaMain root obj */
    pikaMain = newRootObj((char*)"pikaMain", New_PikaMain);
        /* clang-format off */
    PIKA_PYTHON(
    num = 0
    i = 2
    for i in range(2,100):
        j=2
        is_prime = 1
        for j in range(2,i):
            if i%j==0 :
                is_prime = 0
                break
        if is_prime:
            num = num + i


    )
    /* clang-format on */
    const uint8_t bytes[] = {
        0xb4, 0x00, 0x00, 0x00, /* instruct array size */
        0x00, 0x85, 0x01, 0x00, 0x00, 0x04, 0x03, 0x00, 0x00, 0x85, 0x07, 0x00, 
        0x00, 0x04, 0x09, 0x00, 0x20, 0x85, 0x07, 0x00, 0x20, 0x05, 0x0b, 0x00, 
        0x10, 0x02, 0x0f, 0x00, 0x00, 0x02, 0x15, 0x00, 0x00, 0x04, 0x1a, 0x00, 
        0x00, 0x82, 0x1e, 0x00, 0x00, 0x04, 0x09, 0x00, 0x00, 0x0d, 0x09, 0x00, 
        0x00, 0x07, 0x07, 0x00, 0x01, 0x85, 0x07, 0x00, 0x01, 0x04, 0x2b, 0x00, 
        0x01, 0x85, 0x2d, 0x00, 0x01, 0x04, 0x2f, 0x00, 0x21, 0x85, 0x07, 0x00, 
        0x21, 0x01, 0x09, 0x00, 0x11, 0x02, 0x0f, 0x00, 0x01, 0x02, 0x15, 0x00, 
        0x01, 0x04, 0x38, 0x00, 0x01, 0x82, 0x3c, 0x00, 0x01, 0x04, 0x2b, 0x00, 
        0x01, 0x0d, 0x2b, 0x00, 0x01, 0x07, 0x07, 0x00, 0x22, 0x81, 0x09, 0x00, 
        0x22, 0x01, 0x2b, 0x00, 0x12, 0x08, 0x49, 0x00, 0x12, 0x05, 0x01, 0x00, 
        0x02, 0x08, 0x4b, 0x00, 0x02, 0x07, 0x2d, 0x00, 0x03, 0x85, 0x01, 0x00, 
        0x03, 0x04, 0x2f, 0x00, 0x03, 0x8e, 0x00, 0x00, 0x01, 0x86, 0x4e, 0x00, 
        0x01, 0x8c, 0x38, 0x00, 0x01, 0x81, 0x2f, 0x00, 0x01, 0x07, 0x2d, 0x00, 
        0x12, 0x81, 0x03, 0x00, 0x12, 0x01, 0x09, 0x00, 0x02, 0x08, 0x51, 0x00, 
        0x02, 0x04, 0x03, 0x00, 0x00, 0x86, 0x4e, 0x00, 0x00, 0x8c, 0x1a, 0x00, 
        /* instruct array */
        0x53, 0x00, 0x00, 0x00, /* const pool size */
        0x00, 0x30, 0x00, 0x6e, 0x75, 0x6d, 0x00, 0x32, 0x00, 0x69, 0x00, 0x31, 0x30, 
        0x30, 0x00, 0x72, 0x61, 0x6e, 0x67, 0x65, 0x00, 0x69, 0x74, 0x65, 0x72, 
        0x00, 0x24, 0x6c, 0x30, 0x00, 0x24, 0x6c, 0x30, 0x2e, 0x5f, 0x5f, 0x6e, 
        0x65, 0x78, 0x74, 0x5f, 0x5f, 0x00, 0x6a, 0x00, 0x31, 0x00, 0x69, 0x73, 
        0x5f, 0x70, 0x72, 0x69, 0x6d, 0x65, 0x00, 0x24, 0x6c, 0x31, 0x00, 0x24, 
        0x6c, 0x31, 0x2e, 0x5f, 0x5f, 0x6e, 0x65, 0x78, 0x74, 0x5f, 0x5f, 0x00, 
        0x25, 0x00, 0x3d, 0x3d, 0x00, 0x2d, 0x31, 0x00, 0x2b, 0x00, /* const pool */
        };
     __cycleof__("", {
            nCycleUsed_pika = _;
            printf("[prime_number_100_pika] Cycle Used %lld\r\n", _);
        }) {
        /* run pika ByteCode */
        pikaVM_runByteCode(pikaMain, (uint8_t*)bytes);
    }
    int num = obj_getInt(pikaMain, "num");
    if ( num != 1060){
        printf("[error] in benchmark prime_number_100_pika \r\n");
    }
    /* free the pikaMain obj */
    obj_deinit(pikaMain);
    
    uint64_t benchmark_result = ((double) nCycleUsed_c / (double)nCycleUsed_pika) * 100 * 100000;
    
    printf("\r\n[------benchmark finished ---------]\r\n");
    printf("benchmakr result :%lld\r\n", benchmark_result);
    #endif

    pikaScriptShell(pikaMain);
  while (1)
  {
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
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
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

#ifdef USE_FULL_ASSERT
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
