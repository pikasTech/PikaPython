/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    stm32g0xx_it.c
 * @brief   Interrupt Service Routines.
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
#include "stm32g0xx_it.h"
#include "main.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "STM32_UART.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

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
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/

/* USER CODE BEGIN EV */
extern struct CodeHeap {
    char* content;
    uint32_t size;
    uint8_t ena;
    uint32_t reciveTime;
} codeHeap;

#define FLASH_USER_START_ADDR \
    (FLASH_BASE +             \
     ((FLASH_PAGE_NB - 1) * FLASH_PAGE_SIZE)) /* Start @ of user Flash area */
#define FLASH_USER_END_ADDR \
    (FLASH_BASE + FLASH_SIZE - 1) /* End @ of user Flash area */
static uint32_t GetPage(uint32_t Addr) {
    return (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
    ;
}
#define DATA_64 ((uint64_t)0x1234567812345678)
#define DATA_32 ((uint32_t)0x12345678)

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M0+ Processor Interruption and Exception Handlers */
/******************************************************************************/
/**
 * @brief This function handles Non maskable interrupt.
 */
void NMI_Handler(void) {
    /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

    /* USER CODE END NonMaskableInt_IRQn 0 */
    /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
    while (1) {
    }
    /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
 * @brief This function handles Hard fault interrupt.
 */
void HardFault_Handler(void) {
    /* USER CODE BEGIN HardFault_IRQn 0 */

    /* USER CODE END HardFault_IRQn 0 */
    while (1) {
        /* USER CODE BEGIN W1_HardFault_IRQn 0 */
        /* USER CODE END W1_HardFault_IRQn 0 */
    }
}

/**
 * @brief This function handles System service call via SWI instruction.
 */
void SVC_Handler(void) {
    /* USER CODE BEGIN SVC_IRQn 0 */

    /* USER CODE END SVC_IRQn 0 */
    /* USER CODE BEGIN SVC_IRQn 1 */

    /* USER CODE END SVC_IRQn 1 */
}

/**
 * @brief This function handles Pendable request for system service.
 */
void PendSV_Handler(void) {
    /* USER CODE BEGIN PendSV_IRQn 0 */

    /* USER CODE END PendSV_IRQn 0 */
    /* USER CODE BEGIN PendSV_IRQn 1 */

    /* USER CODE END PendSV_IRQn 1 */
}

/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void) {
    /* USER CODE BEGIN SysTick_IRQn 0 */

    /* USER CODE END SysTick_IRQn 0 */
    HAL_IncTick();
    /* USER CODE BEGIN SysTick_IRQn 1 */
    if (codeHeap.ena) {
        /* transmite is finished */
        if (uwTick - codeHeap.reciveTime > 200) {
            uint32_t FirstPage = 0, NbOfPages = 0;
            uint32_t Address = 0, PageError = 0;
            __IO uint32_t data32 = 0, MemoryProgramStatus = 0;
            static FLASH_EraseInitTypeDef EraseInitStruct = {0};

            printf("==============[Programer]==============\r\n");
            printf("[info]: Recived byte: %d\r\n", codeHeap.size);
            printf("[info]: Updating flash... \r\n");
            HAL_FLASH_Unlock();
            /* Get the 1st page to erase */
            FirstPage = GetPage(FLASH_USER_START_ADDR);

            /* Get the number of pages to erase from 1st page */
            NbOfPages = GetPage(FLASH_USER_END_ADDR) - FirstPage + 1;

            /* Fill EraseInit structure*/
            EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
            EraseInitStruct.Page = FirstPage;
            EraseInitStruct.NbPages = NbOfPages;
            printf("[info]: Erasing flash... \r\n");

            if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK) {
                printf("[error]: Erase faild! \r\n");
                while (1) {
                }
            }
            Address = FLASH_USER_START_ADDR;

            while (Address < FLASH_USER_END_ADDR) {
                if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, Address,
                                      DATA_64) == HAL_OK) {
                    Address = Address + 8;
                } else {
                    printf("[error]: Write flash faild. \r\n");
                    while (1) {
                    }
                }
            }
            HAL_FLASH_Lock();
            printf("[info]: Write flash ok! \r\n");

            Address = FLASH_USER_START_ADDR;
            MemoryProgramStatus = 0x0;

            printf("[info]: Checking flash... \r\n");

            while (Address < FLASH_USER_END_ADDR) {
                data32 = *(__IO uint32_t*)Address;
                if (data32 != DATA_32) {
                    MemoryProgramStatus++;
                }
                Address = Address + 4;
            }

            /*Check if there is an issue to program data*/
            if (MemoryProgramStatus != 0) {
                printf("memory state: %d\r\n", MemoryProgramStatus);
                printf("[error]: Check flash faild.\r\n");
                while (1) {
                }
            }

            printf("[OK]: Programing ok! \r\n");
            printf("[info]: Restarting... \r\n");
            printf("==============[Programer]==============\r\n");
            printf("\r\n");
            HAL_NVIC_SystemReset();
        }
    }
    /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32G0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32g0xx.s).                    */
/******************************************************************************/

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
