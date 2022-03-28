
#include "wm_hal.h"

extern ADC_HandleTypeDef hadc;

#define readl(addr) ({unsigned int __v = (*(volatile unsigned int *) (addr)); __v;})
__attribute__((isr)) void CORET_IRQHandler(void)
{
	readl(0xE000E010);
	HAL_IncTick();
}

__attribute__((isr)) void ADC_IRQHandler(void)
{
 	HAL_ADC_IRQHandler(&hadc);
}