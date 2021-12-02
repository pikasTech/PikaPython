/*!
 * @file        apm32f0xx_misc.c
 *
 * @brief       This file provides all the miscellaneous firmware functions (add-on to CMSIS functions).
 *
 * @version     V1.0.1
 *
 * @date        2021-07-01
 *
 */

#include "apm32f0xx_misc.h"

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup MISC_Driver MISC Driver
  @{
*/

/** @addtogroup MISC_Fuctions Fuctions
  @{
*/

/** @defgroup MISC_Private_Functions
  @{
 */

/*!
 * @brief       Enable NVIC request
 *
 * @param       irq:        The NVIC interrupt request, detailed in IRQn_Type
 *
 * @param       priority:   Specifies the priority needed to set
 *
 * @retval      None
 */
void NVIC_EnableIRQRequest(IRQn_Type irq, uint8_t priority)
{
    NVIC_SetPriority(irq, priority);

    NVIC_EnableIRQ(irq);
}

/*!
 * @brief       Disable NVIC request
 *
 * @param       irq:    The NVIC interrupt request, detailed in IRQn_Type
 *
 * @retval      None
 */
void NVIC_DisableIRQRequest(IRQn_Type irq)
{
    NVIC_DisableIRQ(irq);
}

/**
 * @brief       Enables the system to enter low power mode.
 *
 * @param       lowPowerMode: Specifies the system to enter low power mode.
 *                      This parameter can be one of the following values:
 *                      @arg NVIC_LOWPOER_SEVONPEND:   Low Power SEV on Pend.
 *                      @arg NVIC_LOWPOER_SLEEPDEEP:   Low Power DEEPSLEEP request.
 *                      @arg NVIC_LOWPOER_SLEEPONEXIT: Low Power Sleep on Exit.
 *
 * @retval      None
 */
void NVIC_EnableSystemLowPower(uint8_t lowPowerMode)
{
    SCB->SCR |= lowPowerMode;
}

/**
 * @brief       Disables the system to enter low power mode.
 *
 * @param       lowPowerMode: Specifies the system to enter low power mode.
 *                      This parameter can be one of the following values:
 *                      @arg NVIC_LOWPOER_SEVONPEND:   Low Power SEV on Pend.
 *                      @arg NVIC_LOWPOER_SLEEPDEEP:   Low Power DEEPSLEEP request.
 *                      @arg NVIC_LOWPOER_SLEEPONEXIT: Low Power Sleep on Exit.
 *
 * @retval      None
 */
void NVIC_DisableSystemLowPower(uint8_t lowPowerMode)
{
    SCB->SCR &= (uint32_t)(~(uint32_t)lowPowerMode);
}

/**
 * @brief       Configures the SysTick clock source.
 *
 * @param       sysTickCLKSource: specifies the SysTick clock source.
 *                     This parameter can be one of the following values:
 *                     @arg SysTick_CLKSource_HCLK_Div8: AHB clock divided by 8 selected as SysTick clock source.
 *                     @arg SysTick_CLKSource_HCLK:      AHB clock selected as SysTick clock source.
 *
 * @retval      None
 */
void SysTick_ConfigCLKSource(uint32_t sysTickCLKSource)
{
    if (sysTickCLKSource == SysTick_CLKSource_HCLK)
    {
        SysTick->CTRL |= SysTick_CLKSource_HCLK;
    }
    else
    {
        SysTick->CTRL &= SysTick_CLKSource_HCLK_Div8;
    }
}

/*!
 * @brief       Enter Wait Mode
 *
 * @param       None
 *
 * @retval      None
 */
void PMU_EnterWaitMode(void)
{
    SCB->SCR &= (uint32_t)(~(uint32_t)NVIC_LOWPOER_SLEEPDEEP);
    __WFI();
}

/*!
 * @brief       Enter Stop Mode with WFI instruction
 *
 * @param       None
 *
 * @retval      None
 */
void PMU_EnterHaltModeWFI(void)
{
    SCB->SCR |= NVIC_LOWPOER_SLEEPDEEP;
    __DSB();
    __WFI();
}

/*!
 * @brief       Enter Stop Mode with WFE instruction
 *
 * @param       None
 *
 * @retval      None
 */
void PMU_EnterHaltModeWFE(void)
{
    SCB->SCR |= NVIC_LOWPOER_SLEEPDEEP;
    __DSB();
    __WFE();
}

/**@} end of group MISC_Fuctions*/
/**@} end of group MISC_Driver */
/**@} end of group Peripherals_Library*/
