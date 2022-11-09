#include "main.h"

//Timer ticks
static __IO uint32_t systick = 0;

void systick_init(void)
{
    /* SystemFrequency / 1000    1ms中断一次
     * SystemFrequency / 100000	 10us中断一次
     * SystemFrequency / 1000000 1us中断一次
     */
    if (SysTick_Config(SystemCoreClock / 1000))
    {
        /* Capture error */
        while (1)
            ;
    }
}

void swm_inctick(void)
{
    systick++;
}

uint32_t swm_gettick(void)
{
    return systick;
}

void swm_delay(__IO uint32_t delay)
{
    uint32_t tickstart = 0;
    tickstart = swm_gettick();
    while ((swm_gettick() - tickstart) < delay)
    {
        ;
    }
}

/*********************************************END OF FILE**********************/
