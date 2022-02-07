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
#include "gpio.h"

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
PikaObj *pikaMain;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

volatile static char rx_char = 0;
char UART1_RxBuff[RX_BUFF_LENGTH] = {0};
uint16_t UART1_RXBuff_offset = 0;
__attribute__((weak)) void __PIKA_USART1_IRQHandler(char rx_char) {}
void USART1_IRQHandler(void){
    if (LL_USART_IsActiveFlag_RXNE(USART1)) {
        rx_char = LL_USART_ReceiveData8(USART1);
        __PIKA_USART1_IRQHandler(rx_char);
        /* clear buff when overflow */
        if (UART1_RXBuff_offset >= RX_BUFF_LENGTH) {
            UART1_RXBuff_offset = 0;
            memset(UART1_RxBuff, 0, sizeof(UART1_RxBuff));
        }
        /* recive char */
        UART1_RxBuff[UART1_RXBuff_offset] = rx_char;
        UART1_RXBuff_offset++;
        if ('\n' == rx_char) {
            /* handle python script download */
            if (STM32_Code_reciveHandler(UART1_RxBuff, UART1_RXBuff_offset)) {
                goto line_exit;
            }
        line_exit:
            UART1_RXBuff_offset = 0;
            memset(UART1_RxBuff, 0, sizeof(UART1_RxBuff));
            return;
        }
    }
}

/* support pikaScript Shell */
char __platform_getchar(){
    char res = 0;
    while(rx_char == 0){
    };
    res = rx_char;
    rx_char = 0;
    return res;
}

int main(void){
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    HARDWARE_PRINTF_Init();
    printf("[info]: stm32g070 system init ok.\r\n");
    printf("------------------------------------------------------------------\r\n");
    printf("|                                                                |\r\n");
    printf("|     ____   _   __            _____              _          __  |\r\n");
    printf("|    / __ \\ (_) / /__ ____ _  / ___/ _____ _____ (_) ____   / /_ |\r\n");
    printf("|   / /_/ // / / //_// __ `/  \\__ \\ / ___// ___// / / __ \\ / __/ |\r\n");
    printf("|  / ____// / / ,<  / /_/ /  ___/ // /__ / /   / / / /_/ // /_   |\r\n");
    printf("| /_/    /_/ /_/|_| \\__,_/  /____/ \\___//_/   /_/ / .___/ \\__/   |\r\n");
    printf("|                                                /_/             |\r\n");
    printf("|          PikaScript - An Ultra Lightweight Python Engine       |\r\n");
    printf("|                                                                |\r\n");
    printf("|           [ https://github.com/pikastech/pikascript ]          |\r\n");
    printf("|           [  https://gitee.com/lyon1998/pikascript  ]          |\r\n");
    printf("|                                                                |\r\n");
    printf("------------------------------------------------------------------\r\n");  
    /* boot pikaScript */
    char* code = (char*)FLASH_SCRIPT_START_ADDR;
    if (code[0] != 0xFF) {
        /* boot from flash */
        pikaMain = newRootObj("pikaMain", New_PikaMain);
        if (code[0] == 'i') {
            printf("[info]: boot from Script.\r\n");
            Arg* codeBuff = arg_setStr(NULL, "", code);
            obj_run(pikaMain, arg_getStr(codeBuff));
            arg_deinit(codeBuff);
            goto main_loop;
        }
    } else {
        /* boot from firmware */
        pikaMain = pikaScriptInit();
        goto main_loop;
    }

    pikaMain = pikaScriptInit();
    goto main_loop;

main_loop:
    pikaScriptShell(pikaMain);
    /* after exit() from pika shell */
    NVIC_SystemReset();
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 16;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

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
