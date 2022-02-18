/*
 * wm_it.c
 *
 *  Created on: Oct 27, 2021
 *      Author: derek
 */

/* Includes ------------------------------------------------------------------*/
#include "wm_hal.h"
#include "wm_regs.h"
#include "FreeRTOS.h"
#include "task.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure WM_IT                                                            */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */
#define readl(addr) ({unsigned int __v = (*(volatile unsigned int *) (addr)); __v;})
/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
__attribute__((isr)) void CORET_IRQHandler(void)
{
    readl(0xE000E010);
	HAL_IncTick();
	
	xPortSysTickHandler();
	portYIELD_FROM_ISR(1);
}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */

/* USER CODE BEGIN 3 */

/********************EXTI IRQ******************/
//__attribute__((isr)) void GPIOB_IRQHandler(void)
//{
//	HAL_GPIO_EXTI_IRQHandler(GPIOB, GPIO_PIN_5);
//}
//
//
//extern TIM_HandleTypeDef htim0;
//extern TIM_HandleTypeDef htim1;
///********************TIM IRQ******************/
//__attribute__((isr)) void TIM0_5_IRQHandler(void)
//{
//	HAL_TIM_IRQHandler(&htim0);
//	HAL_TIM_IRQHandler(&htim1);
//}
///*********************End**********************/
//
extern ADC_HandleTypeDef hadc;
///********************ADC IRQ******************/
__attribute__((isr)) void ADC_IRQHandler(void)
{
 	HAL_ADC_IRQHandler(&hadc);
}
///*********************End**********************/
//
//extern PMU_HandleTypeDef hpmu;
///********************PMU IRQ******************/
//__attribute__((isr)) void PMU_IRQHandler(void)
//{
//	HAL_PMU_IRQHandler(&hpmu);
//}
///*********************End**********************/
//
///* USER CODE END 3 */

/************************ (C) COPYRIGHT winnermicro *****END OF FILE****/
