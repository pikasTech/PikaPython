#include "CH58x_common.h"
#include "CH582_Time.h"


void mdelay1ms()
{
    R32_TMR0_CNT_END = (FREQ_SYS / 1000);
    R8_TMR0_CTRL_MOD = RB_TMR_ALL_CLEAR;
    R8_TMR0_CTRL_MOD = RB_TMR_COUNT_EN;
    while ((R8_TMR0_INT_FLAG & TMR0_3_IT_CYC_END) == 0)
        ;
    R8_TMR0_INT_FLAG = TMR0_3_IT_CYC_END;
}

void mdelaym(uint32_t ms)
{
    for (uint32_t i = 0; i < ms; i++)
    {
        mdelay1ms();
    }
}

void CH582_Time_sleep_ms(PikaObj *self, int ms)
{
    mdelaym(ms);
}

void CH582_Time_sleep_s(PikaObj *self, int s)
{
    for (uint32_t i = 0; i < s; i++)
    {
        mdelaym(1000);
    }
}
