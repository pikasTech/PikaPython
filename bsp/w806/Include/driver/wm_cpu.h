/**
 * @file    wm_cpu.h
 *
 * @brief   cpu driver module
 *
 * @author  dave
 *
 * @copyright (c) 2014 Winner Microelectronics Co., Ltd.
 */
#ifndef __WM_CPU_H__
#define __WM_CPU_H__

#include "wm_hal.h"

/**BASE PLL CLOCK*/
#define W805_PLL_CLK_MHZ  		(480)

#define IS_NVIC_DEVICE_IRQ(IRQ)                ((IRQ) >= (IRQn_Type)0x00U)

enum CPU_CLK{
	CPU_CLK_240M = 2,
	CPU_CLK_160M = 3,
	CPU_CLK_80M  = 6,
	CPU_CLK_40M  = 12,
	CPU_CLK_2M  = 240,		
};

typedef union {
    struct {
        uint32_t CPU: 8;                     /*!< bit:  0.. 7  cpu clock divider */
        uint32_t WLAN: 8;                    /*!< bit:  8.. 15 Wlan clock divider */
        uint32_t BUS2: 8;                    /*!< bit:  16.. 23 clock dividing ratio of bus2 & bus1 */
        uint32_t PD: 4;                      /*!< bit:  24.. 27  peripheral divider */
        uint32_t RSV: 3;                     /*!< bit:  28.. 30  Reserved */
        uint32_t DIV_EN: 1;                  /*!< bit:  31     divide frequency enable */
    } b;
    uint32_t w;
} clk_div_reg;

#define UNIT_MHZ		(1000000)


typedef struct{
	uint32_t apbclk;
	uint32_t cpuclk;
	uint32_t wlanclk;
}wm_sys_clk;

typedef enum
{
	HAL_TICK_FREQ_10HZ         = 10,
	HAL_TICK_FREQ_100HZ        = 100,
	HAL_TICK_FREQ_1KHZ         = 1000,
	HAL_TICK_FREQ_DEFAULT      = HAL_TICK_FREQ_1KHZ
} HAL_TickFreqTypeDef;


void SystemClock_Config(uint32_t clk);
void SystemClock_Get(wm_sys_clk *sysclk);

HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority);
void HAL_IncTick(void);
uint32_t HAL_GetTick(void);
void HAL_Delay(uint32_t Delay);

void HAL_NVIC_SetPriority(IRQn_Type IRQn, uint32_t Priority);
void HAL_NVIC_EnableIRQ(IRQn_Type IRQn);
void HAL_NVIC_DisableIRQ(IRQn_Type IRQn);

#endif /* WM_CPU_H */
