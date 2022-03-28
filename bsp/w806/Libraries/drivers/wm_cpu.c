/**
 * @file    wm_cpu.c
 *
 * @brief   cpu driver module
 *
 * @author  kevin
 *
 * Copyright (c) 2014 Winner Microelectronics Co., Ltd.
 */
#include "wm_regs.h"
#include "wm_cpu.h"
#include "core_804.h"
#include "wm_hal.h"

#define TICK_INT_PRIORITY 7

__IO uint32_t uwTick;
uint32_t uwTickPrio;
static HAL_TickFreqTypeDef uwTickFreq = HAL_TICK_FREQ_DEFAULT;  /* 1KHz */
/**
 * @brief          This function is used to set cpu clock
 *
 * @param[in]      	clk    select cpu clock, this parameter can be a value of @ref enum CPU_CLK
 * 							
 * @return         None
 *
 * @note           None
 */
void SystemClock_Config(uint32_t clk)
{
	uint32_t RegValue, bus2Fac, wlanDiv, cpuDiv = clk;

	if ((clk < 2) || (clk > 240))
	{
		return;
	}

	/*Close those not initialized clk. Except uart and gpio. */
    RegValue = READ_REG(RCC->CLK_EN);
    RegValue &= ~0x3FFFFF;
    RegValue |= 0x802;
    WRITE_REG(RCC->CLK_EN, RegValue);

	/* Close bbp clk */
	WRITE_REG(RCC->BBP_CLK, 0x0F);
	
	/* Config clk div */
	RegValue = READ_REG(RCC->CLK_DIV);
	wlanDiv = (RegValue>>8)&0xFF;
	RegValue &= 0xFF000000;
	RegValue |= 0x80000000;
	if(cpuDiv > 12)
	{
		bus2Fac = 1;
		wlanDiv = cpuDiv/4;
	}
	else  /*wlan can run*/
	{
		wlanDiv=3;
		bus2Fac = (wlanDiv*4/cpuDiv)&0xFF;
	}
	RegValue |= (bus2Fac<<16) | (wlanDiv<<8) | cpuDiv;
	WRITE_REG(RCC->CLK_DIV, RegValue);
	
	HAL_InitTick(TICK_INT_PRIORITY);
	return;
}

/**
 * @brief          	This function is used to get cpu clock
 *
 * @param[out]     *sysclk	point to the addr for system clk output
 *
 * @return         	None
 *
 * @note           	None
 */
void SystemClock_Get(wm_sys_clk *sysclk)
{
	clk_div_reg clk_div;

	clk_div.w = READ_REG(RCC->CLK_DIV);
	sysclk->cpuclk = W805_PLL_CLK_MHZ/(clk_div.b.CPU);
	sysclk->wlanclk = W805_PLL_CLK_MHZ/(clk_div.b.WLAN);
	sysclk->apbclk = sysclk->cpuclk / clk_div.b.BUS2;
}


__attribute__((weak)) HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
	wm_sys_clk sysclk;
	
	SystemClock_Get(&sysclk);
	SysTick_Config(sysclk.cpuclk * UNIT_MHZ / uwTickFreq);
	HAL_NVIC_SetPriority(SYS_TICK_IRQn, TickPriority);
	HAL_NVIC_EnableIRQ(SYS_TICK_IRQn);
	uwTickPrio = TickPriority;
	return HAL_OK;
}

__attribute__((weak)) void HAL_IncTick(void)
{
	uwTick += 1;
}

__attribute__((weak)) uint32_t HAL_GetTick(void)
{
	return uwTick;
}

__attribute__((weak)) void HAL_Delay(uint32_t Delay)
{
	uint32_t tickstart = HAL_GetTick();
	uint32_t wait = Delay;

	while ((HAL_GetTick() - tickstart) < wait)
	{
	}
}

/* Priority: a value between 0 and 15
 * A lower priority value indicates a higher priority */
void HAL_NVIC_SetPriority(IRQn_Type IRQn, uint32_t Priority)
{   
	NVIC_SetPriority(IRQn, Priority);
}

void HAL_NVIC_EnableIRQ(IRQn_Type IRQn)
{
	/* Check the parameters */
	assert_param(IS_NVIC_DEVICE_IRQ(IRQn));

	/* Enable interrupt */
	NVIC_EnableIRQ(IRQn);
}

void HAL_NVIC_DisableIRQ(IRQn_Type IRQn)
{
	/* Check the parameters */
	assert_param(IS_NVIC_DEVICE_IRQ(IRQn));

	/* Disable interrupt */
	NVIC_DisableIRQ(IRQn);
}