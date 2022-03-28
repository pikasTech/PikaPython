
#include "wm_hal.h"

extern WDG_HandleTypeDef hwdg;

#define readl(addr) ({unsigned int __v = (*(volatile unsigned int *) (addr)); __v;})
__attribute__((isr)) void CORET_IRQHandler(void)
{
	readl(0xE000E010);
	HAL_IncTick();
}

__attribute__((isr)) void WDG_IRQHandler(void)
{
	HAL_WDG_IRQHandler(&hwdg);
}