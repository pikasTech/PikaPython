
#include <stdio.h>
#include "wm_hal.h"

PWM_HandleTypeDef hpwm;

static void PWM_Init(void);
void Error_Handler(void);

uint32_t ch = PWM_CHANNEL_0;
int main(void)
{
	int i = 0;
	
	SystemClock_Config(CPU_CLK_160M);
	printf("enter main\r\n");
	
	PWM_Init();
	HAL_PWM_Start(&hpwm, ch);
	
	while (1)
	{
		for (i = 0; i < 100; i++)
		{
			HAL_PWM_Duty_Set(&hpwm, ch, i);
			HAL_Delay(20);
		}
		for (i = 99; i >= 0; i--)
		{
			HAL_PWM_Duty_Set(&hpwm, ch, i);
			HAL_Delay(20);
		}
	}
}

/* 输出波形的频率： f = 40MHz / Prescaler / (Period + 1)；
 * 输出波形的占空比： 
 *     沿对齐模式（递减）：(Pulse + 1) / (Period + 1)
 *                         Pulse >= Period：PWM输出一直为高电平
 *                         Pulse < Period ：PWM输出高电平宽度为(Pulse + 1)，低电平宽度为(Period - Pulse)
 *                         Pulse = 0      ：PWM输出高电平宽度为(1)，低电平宽度为(Period)
 * 
 *    中间对齐模式       ：(2 * Pulse + 1) / (2 * (Period + 1))
 *                         Pulse > Period ：PWM输出一直为高电平
 *                         Pulse <= Period：PWM输出高电平宽度为(2 * Pulse + 1)，低电平宽度为(2 * (Period - Pulse) + 1)
 *                         Pulse = 0      ：PWM输出高电平宽度为(1)，低电平宽度为(2 * Period + 1)
 */
static void PWM_Init(void)
{
	// 输出200KHz、占空比40%的波形
	hpwm.Instance = PWM;
	hpwm.Init.AutoReloadPreload = PWM_AUTORELOAD_PRELOAD_ENABLE;
	hpwm.Init.CounterMode = PWM_COUNTERMODE_EDGEALIGNED_DOWN;
	hpwm.Init.Prescaler = 4;
	hpwm.Init.Period = 99;	// 40M / 4 / 100K - 1
	hpwm.Init.Pulse = 19;	// 20% DUTY
	hpwm.Init.OutMode = PWM_OUT_MODE_5SYNC;
	hpwm.Channel = ch;
	
	HAL_PWM_Init(&hpwm);
}

void Error_Handler(void)
{
	while (1)
	{
	}
}

void assert_failed(uint8_t *file, uint32_t line)
{
	printf("Wrong parameters value: file %s on line %d\r\n", file, line);
}