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

volatile static char g_rx_char = 0;
char CONSOLE_RxBuff[RX_BUFF_LENGTH] = {0};
uint16_t CONSOLE_RxBuff_offset = 0;



int64_t pika_platform_getTick(void){
    return HAL_GetTick();
}

void pika_platform_sleep_ms(uint32_t ms){
    HAL_Delay(ms);
}

void pika_platform_sleep_s(uint32_t s){
    for (int i = 0; i < s; i++) {
        HAL_Delay(1000);
    }
}

extern PikaObj *__pikaMain;


// 选择使用的串口
#define USING_CONSOLE_USART1 1
// #define USING_CONSOLE_USART3 1
// #define USING_CONSOLE_USART4 1

// 检查是否有选择的串口，如果没有定义任何一个，则报错
#if !USING_CONSOLE_USART1 && !USING_CONSOLE_USART3 && !USING_CONSOLE_USART4
#error "Please define a console USART (1, 3, or 4) to use by setting the USING_CONSOLE_USARTX to 1."
#endif

#if (USING_CONSOLE_USART1)
#define USART_CONSOLE USART1
#define USART_CONSOLE_IRQn USART1_IRQn
#define USART_CONSOLE_GPIO GPIOA
#define USART_CONSOLE_PIN_TX LL_GPIO_PIN_9
#define USART_CONSOLE_PIN_RX LL_GPIO_PIN_10
#define USART_CONSOLE_AF LL_GPIO_AF_1
#define USART_CONSOLE_CLK_ENABLE() LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1)
#define USART_CONSOLE_GPIO_CLK_ENABLE() LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA)
#define USART_CONSOLE_IRQHandler USART1_IRQHandler

#elif (USING_CONSOLE_USART3)
#define USART_CONSOLE USART3
#define USART_CONSOLE_IRQn USART3_4_IRQn
#define USART_CONSOLE_GPIO GPIOD
#define USART_CONSOLE_PIN_TX LL_GPIO_PIN_8
#define USART_CONSOLE_PIN_RX LL_GPIO_PIN_9
#define USART_CONSOLE_AF LL_GPIO_AF_0
#define USART_CONSOLE_CLK_ENABLE() LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART3)
#define USART_CONSOLE_GPIO_CLK_ENABLE() LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOD)
#define USART_CONSOLE_IRQHandler USART3_4_IRQHandler

#elif (USING_CONSOLE_USART4)
#define USART_CONSOLE USART4
#define USART_CONSOLE_IRQn USART3_4_IRQn
#define USART_CONSOLE_GPIO GPIOC
#define USART_CONSOLE_PIN_TX LL_GPIO_PIN_10
#define USART_CONSOLE_PIN_RX LL_GPIO_PIN_11
#define USART_CONSOLE_AF LL_GPIO_AF_0
#define USART_CONSOLE_CLK_ENABLE() LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART4)
#define USART_CONSOLE_GPIO_CLK_ENABLE() LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOC)
#define USART_CONSOLE_IRQHandler USART3_4_IRQHandler

#endif

__attribute__((weak)) void __PIKA_USART_IRQHandler(USART_TypeDef* USARTx, char rx_char) {};

void USART_CONSOLE_IRQHandler(void) {
    if (LL_USART_IsActiveFlag_RXNE(USART_CONSOLE)) {
        g_rx_char = LL_USART_ReceiveData8(USART_CONSOLE);
        __PIKA_USART_IRQHandler(USART_CONSOLE, g_rx_char);
        /* clear buff when overflow */
        if (CONSOLE_RxBuff_offset >= RX_BUFF_LENGTH) {
            CONSOLE_RxBuff_offset = 0;
            memset(CONSOLE_RxBuff, 0, sizeof(CONSOLE_RxBuff));
        }
        /* recive char */
        CONSOLE_RxBuff[CONSOLE_RxBuff_offset] = g_rx_char;
        CONSOLE_RxBuff_offset++;
        if ('\n' == g_rx_char || '\r' == g_rx_char) {
            /* handle python script download */
            if (STM32_Code_reciveHandler(CONSOLE_RxBuff, CONSOLE_RxBuff_offset)) {
                goto line_exit;
            }
        line_exit:
            CONSOLE_RxBuff_offset = 0;
            memset(CONSOLE_RxBuff, 0, sizeof(CONSOLE_RxBuff));
            return;
        }
    }
}

/* support printf */
void HARDWARE_PRINTF_Init(void) {
    LL_USART_InitTypeDef USART_InitStruct = {0};
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Enable peripheral and GPIO clocks
    USART_CONSOLE_CLK_ENABLE();
    USART_CONSOLE_GPIO_CLK_ENABLE();

    // Configure USART GPIO
    GPIO_InitStruct.Pin = USART_CONSOLE_PIN_TX;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = USART_CONSOLE_AF;
    LL_GPIO_Init(USART_CONSOLE_GPIO, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = USART_CONSOLE_PIN_RX;
    LL_GPIO_Init(USART_CONSOLE_GPIO, &GPIO_InitStruct);

    // USART interrupt Init
    NVIC_SetPriority(USART_CONSOLE_IRQn, 0);
    NVIC_EnableIRQ(USART_CONSOLE_IRQn);

    USART_InitStruct.PrescalerValue = LL_USART_PRESCALER_DIV1;
    USART_InitStruct.BaudRate = 115200;
    USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
    USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
    USART_InitStruct.Parity = LL_USART_PARITY_NONE;
    USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
    USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
    LL_USART_Init(USART_CONSOLE, &USART_InitStruct);
    LL_USART_SetTXFIFOThreshold(USART_CONSOLE, LL_USART_FIFOTHRESHOLD_1_8);
    LL_USART_SetRXFIFOThreshold(USART_CONSOLE, LL_USART_FIFOTHRESHOLD_1_8);
    LL_USART_DisableFIFO(USART_CONSOLE);
    LL_USART_ConfigAsyncMode(USART_CONSOLE);
    LL_USART_Enable(USART_CONSOLE);

    // Polling USART initialisation
    while ((!(LL_USART_IsActiveFlag_TEACK(USART_CONSOLE))) ||
           (!(LL_USART_IsActiveFlag_REACK(USART_CONSOLE)))) {
    }

    // Enable interrupt
    LL_USART_EnableIT_RXNE(USART_CONSOLE);
    LL_USART_EnableIT_PE(USART_CONSOLE);
}

int fputc(int ch, FILE* f) {
    LL_USART_TransmitData8(USART_CONSOLE, ch);
    while (LL_USART_IsActiveFlag_TC(USART_CONSOLE) != 1)
        ;
    return ch;
}

/* support pikaScript Shell */
char __platform_getchar(){
    char res = 0;
    while(g_rx_char == 0){
    };
    res = g_rx_char;
    g_rx_char = 0;
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
        extern unsigned char pikaModules_py_a[];
        obj_linkLibrary(pikaMain, pikaModules_py_a);
        if (code[0] == 'i') {
            printf("[info]: boot from Script.\r\n");
            obj_run(pikaMain, code);
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
