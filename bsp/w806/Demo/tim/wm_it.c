
#include "wm_hal.h"

extern TIM_HandleTypeDef htim0;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;

#define readl(addr) ({unsigned int __v = (*(volatile unsigned int *) (addr)); __v;})
__attribute__((isr)) void CORET_IRQHandler(void)
{
	readl(0xE000E010);
	HAL_IncTick();
}

__attribute__((isr)) void TIM0_5_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&htim0);
	HAL_TIM_IRQHandler(&htim1);
	HAL_TIM_IRQHandler(&htim2);
	HAL_TIM_IRQHandler(&htim3);
	HAL_TIM_IRQHandler(&htim4);
	HAL_TIM_IRQHandler(&htim5);
}