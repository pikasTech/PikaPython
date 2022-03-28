
#include "wm_hal.h"

#define readl(addr) ({unsigned int __v = (*(volatile unsigned int *) (addr)); __v;})
__attribute__((isr)) void CORET_IRQHandler(void)
{
	readl(0xE000E010);
	HAL_IncTick();
}

extern UART_HandleTypeDef huart1;

__attribute__((isr)) void UART1_IRQHandler(void)
{
	HAL_UART_IRQHandler(&huart1);
}
