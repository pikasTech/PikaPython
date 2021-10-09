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
#include "usart.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define RX_BUFF_LENGTH 256
typedef struct {
    UART_HandleTypeDef huart;
    uint8_t id;
    char rxBuff[RX_BUFF_LENGTH];
    uint16_t rxBuffOffset;
} pika_uart_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define uart_put(str) \
    HAL_UART_Transmit(&huart1, (uint8_t*)str, sizeof(str), 100);
#define FLASH_CODE_START_ADDR 0x08002000
#define FLASH_USER_END_ADDR \
    (FLASH_BASE + FLASH_SIZE - 1) /* End @ of user Flash area */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
pika_uart_t pika_uart1;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

uint32_t writeAddress = 0;
uint8_t isRecived = 0;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart) {
    isRecived = 1;
    uint8_t id = 1;
    pika_uart_t* pika_uart = &pika_uart1;
    char inputChar = pika_uart->rxBuff[pika_uart->rxBuffOffset];

    uint32_t baseAddress = FLASH_CODE_START_ADDR;
    uint64_t writeData64 = 0;

    writeData64 = 0;
    if (pika_uart->rxBuffOffset + 1 < 8) {
        goto next;
    }

    for (int i = 7; i >= 0; i--) {
        char ch = pika_uart->rxBuff[i];
        writeData64 = writeData64 << 8;
        writeData64 += ch;
    }

    __disable_irq();
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD,
                          baseAddress + writeAddress, writeData64) == HAL_OK) {
        writeAddress = writeAddress + 8;
    } else {
        printf("[error]: Write flash faild. \r\n");
        while (1) {
        }
    }
    __enable_irq();
    /* clear recive buff */
    pika_uart->rxBuffOffset = 0;
    pika_uart->rxBuff[0] = 0;
    UART_Start_Receive_IT(
        huart, (uint8_t*)(pika_uart->rxBuff + pika_uart->rxBuffOffset), 1);
    return;

next:
    /* avoid recive buff overflow */
    if (pika_uart->rxBuffOffset + 2 > RX_BUFF_LENGTH) {
        memmove(pika_uart->rxBuff, pika_uart->rxBuff + 1, RX_BUFF_LENGTH);
        UART_Start_Receive_IT(
            huart, (uint8_t*)(pika_uart->rxBuff + pika_uart->rxBuffOffset), 1);
        return;
    }

    /* recive next char */
    pika_uart->rxBuffOffset++;
    pika_uart->rxBuff[pika_uart->rxBuffOffset] = 0;
    UART_Start_Receive_IT(
        huart, (uint8_t*)(pika_uart->rxBuff + pika_uart->rxBuffOffset), 1);
}
uint32_t GetPage(uint32_t Addr) {
    return (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
}
/* support prinf */
int fputc(int ch, FILE* f) {
    HAL_UART_Transmit(&huart1, (uint8_t*)&ch, 1, 0xffff);
    return ch;
}

typedef void (*iapfun)(void);
iapfun jump2app;
void MSR_MSP(unsigned int addr);  // set stack pointer

void MSR_MSP(unsigned int addr) {
    __asm("MSR MSP, r0");  // set Main Stack value
    //    __asm("BX r14");
}

/* jump to app */
void iap_load_app(uint32_t appxaddr) {
    uint32_t Byte = *(__IO uint32_t*)appxaddr;
    uint32_t Top = (Byte & 0x2FFE0000);
    if (Top == 0x20000000)  // check stack pointer
    {
        jump2app =
            (iapfun) *
            (__IO uint32_t*)(appxaddr + 4); /* jump to the reset adress */
        /* deinit rcc */
        HAL_RCC_DeInit();
        /* deinit sysTick */
        SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
        /* deinit USART */
        HAL_UART_DeInit(&huart1);
        /* deinit GPIO */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_All);
        /* deinit HAL */
        HAL_DeInit();

        MSR_MSP(*(__IO uint32_t*)appxaddr);
        jump2app(); /* jump */
    }
    printf("[info]: jump to app faild.\r\n");
}
char* code = (char*)FLASH_CODE_START_ADDR;

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU
     * Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the
     * Systick. */
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
    HAL_UART_Receive_IT(
        &huart1, (uint8_t*)(pika_uart1.rxBuff + pika_uart1.rxBuffOffset), 1);

    if (!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)) {
        iap_load_app(FLASH_CODE_START_ADDR);
    }

    printf("[info]: In bootloader.\r\n");

    uint32_t FirstPage = 0, NbOfPages = 0;
    uint32_t PageError = 0;
    __IO uint32_t data32 = 0, MemoryProgramStatus = 0;
    static FLASH_EraseInitTypeDef EraseInitStruct = {0};

    HAL_FLASH_Unlock();
    /* Get the 1st page to erase */
    FirstPage = GetPage(FLASH_CODE_START_ADDR);

    /* Get the number of pages to erase from 1st page */
    NbOfPages = GetPage(FLASH_USER_END_ADDR) - FirstPage + 1;

    /* Fill EraseInit structure*/
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.Page = FirstPage;
    EraseInitStruct.NbPages = NbOfPages;
    printf("[info]: Erasing flash... \r\n");
    printf("[info]: Erase from 0x%x, size: %ldkB\r\n", FLASH_CODE_START_ADDR,
           (FLASH_USER_END_ADDR - FLASH_CODE_START_ADDR + 1) / 1024);
    __disable_irq();
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK) {
        printf("[error]: Erase faild! \r\n");
        while (1) {
        }
    }
    __enable_irq();
    printf("[ OK ]: Erase flash ok! \r\n");

    while (1) {
        HAL_Delay(1000);
        printf("[info]: Waiting for '*.bin' file...\r\n");
        if (isRecived) {
            break;
        }
    }

    while (1) {
        HAL_Delay(100);
        printf("[info]: Reciving...\r\n");
    }

    HAL_FLASH_Lock();
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1) {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

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
    RCC_OscInitStruct.PLL.PLLN = 8;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType =
        RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
        Error_Handler();
    }
    /** Initializes the peripherals clocks
     */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
    PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state
     */
    __disable_irq();
    while (1) {
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
void assert_failed(uint8_t* file, uint32_t line) {
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line
       number, ex: printf("Wrong parameters value: file %s on line %d\r\n",
       file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
