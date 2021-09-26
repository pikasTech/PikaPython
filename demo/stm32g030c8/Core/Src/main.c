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
#include "stdbool.h"
#include "STM32_common.h"
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
void Flash_OB_Handle(void) {
    FLASH_OBProgramInitTypeDef optionsbytesstruct;
    bool UPDATE = false;

    HAL_FLASHEx_OBGetConfig(&optionsbytesstruct);
    uint32_t userconfig = optionsbytesstruct.USERConfig;

    if ((userconfig & FLASH_OPTR_nBOOT_SEL_Msk) != OB_BOOT0_FROM_PIN) {
        userconfig &= ~FLASH_OPTR_nBOOT_SEL_Msk;
        userconfig |= OB_BOOT0_FROM_PIN;
        UPDATE = true;
    }

    if (UPDATE) {
        optionsbytesstruct.USERConfig = userconfig;
        HAL_FLASH_Unlock();
        HAL_FLASH_OB_Unlock();
        HAL_FLASHEx_OBProgram(&optionsbytesstruct);
        HAL_FLASH_OB_Launch();
        HAL_FLASH_OB_Lock();
        HAL_FLASH_Lock();
    }
}

void __pikaDisableIrqHandle(){
    __disable_irq();
}
void __pikaEnableIrqHandle(){
    __enable_irq();
}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
    /* USER CODE BEGIN 1 */
    Flash_OB_Handle();
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
    /* USER CODE BEGIN 2 */
    char *code = (char *)FLASH_CODE_START_ADDR;
    PikaObj* pikaMain;
    uint16_t codeOffset = 0;
    if( code[0] == 'i'){
        /* boot from flash */
        pikaMain = newRootObj("pikaMain", New_PikaMain);
        obj_run(pikaMain, "uart.init()");
        obj_run(pikaMain, "uart.setId(1)");
        obj_run(pikaMain, "uart.setBaudRate(115200)");
        obj_run(pikaMain, "uart.enable()");
        obj_run(pikaMain, "print('[info]: boot from flash.')");
        while(1){
            char lineBuff[64] = {0};
            char *codePointer = code + codeOffset;
            char *line = strGetFirstToken(lineBuff, codePointer, '\n');
            codeOffset += strGetSize(line) + 1;
            
            obj_run(pikaMain, line);
            /* not any line in code */
            if( !strIsContain(codePointer, '\n') ){
                break;
            }
        }
        obj_run(pikaMain, "");
    }
    else{
        /* boot from function */
        pikaMain = pikaScriptInit();
    }
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

    /** Configure the main internal regulator output voltage
     */
    HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
    RCC_OscInitStruct.PLL.PLLN = 16;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV3;
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
