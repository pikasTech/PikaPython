/*!
 * @file        system_apm32f0xx.c
 *
 * @brief       CMSIS Cortex-M0+ Device Peripheral Access Layer System Source File
 *
 * @version     V1.0.1
 *
 * @date        2021-07-01
 *
 */

#include "apm32f0xx.h"

#define SYSTEM_CLOCK_HSE    HSE_VALUE
//#define SYSTEM_CLOCK_24MHz  (24000000)
//#define SYSTEM_CLOCK_36MHz  (36000000)
//#define SYSTEM_CLOCK_48MHz  (48000000)



#ifdef SYSTEM_CLOCK_HSE
uint32_t SystemCoreClock         = SYSTEM_CLOCK_HSE;
#elif defined SYSTEM_CLOCK_24MHz
uint32_t SystemCoreClock         = SYSTEM_CLOCK_24MHz;
#elif defined SYSTEM_CLOCK_36MHz
uint32_t SystemCoreClock         = SYSTEM_CLOCK_36MHz;
#elif defined SYSTEM_CLOCK_48MHz
uint32_t SystemCoreClock         = SYSTEM_CLOCK_48MHz;
#else
uint32_t SystemCoreClock         = HSI_VALUE;
#endif

static void SystemClockConfig(void);

#ifdef SYSTEM_CLOCK_HSE
static void SystemClockHSE(void);
#elif defined SYSTEM_CLOCK_24MHz
static void SystemClock24M(void);
#elif defined SYSTEM_CLOCK_36MHz
static void SystemClock36M(void);
#elif defined SYSTEM_CLOCK_48MHz
static void SystemClock48M(void);

#endif

/*!
 * @brief       Setup the microcontroller system
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void SystemInit (void)
{
    /** Set HSIEN bit */
    RCM->CTRL1_B.HSIEN = BIT_SET;
    /** Reset SCLKSEL, AHBPSC, APB1PSC, APB2PSC, ADCPSC and COC bits */
    RCM->CFG1 &= (uint32_t)0x08FFB80CU;
    /** Reset HSEEN, CSSEN and PLLEN bits */
    RCM->CTRL1 &= (uint32_t)0xFEF6FFFFU;
    /** Reset HSEBCFG bit */
    RCM->CTRL1_B.HSEBCFG = BIT_RESET;
    /** Reset PLLSRCSEL, PLLHSEPSC, PLLMULCFG bits */
    RCM->CFG1 &= (uint32_t)0xFFC0FFFFU;
    /** Reset PREDIV[3:0] bits */
    RCM->CFG1 &= (uint32_t)0xFFFFFFF0U;
    /** Reset USARTSW[1:0], I2CSW, CECSW and ADCSW bits */
    RCM->CFG3 &= (uint32_t)0xFFFFFEAC;
    /* Reset  HSI14 bit */
    RCM->CTRL2_B.HSI14EN = BIT_RESET;
    /** Disable all interrupts */
    RCM->INT = 0x00000000U;

    SystemClockConfig();

}

/*!
 * @brief       Update SystemCoreClock variable according to Clock Register Values
 *              The SystemCoreClock variable contains the core clock (HCLK)
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void SystemCoreClockUpdate (void)
{
    uint32_t sysClock, pllMull, pllSource, Prescaler;
    uint8_t AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};

    /* Get SYSCLK source -------------------------------------------------------*/
    sysClock = RCM->CFG1_B.SCLKSWSTS;

    switch (sysClock)
    {
        case 0:
            SystemCoreClock = HSI_VALUE;
            break;

        /** sys clock is HSE */
        case 1:
            SystemCoreClock = HSE_VALUE;
            break;

        /** sys clock is PLL */
        case 2:
            pllMull = RCM->CFG1_B.PLLMULCFG + 2;
            pllSource = RCM->CFG1_B.PLLSRCSEL;

            /** PLL entry clock source is HSE */
            if (pllSource == BIT_SET)
            {
                SystemCoreClock = HSE_VALUE * pllMull;

                /** HSE clock divided by 2 */
                if (pllSource == RCM->CFG1_B.PLLHSEPSC)
                {
                    SystemCoreClock >>= 1;
                }
            }
            /** PLL entry clock source is HSI/2 */
            else
            {
                SystemCoreClock = (HSI_VALUE >> 1) * pllMull;
            }

            break;

        default:
            SystemCoreClock  = HSI_VALUE;
            break;
    }

    Prescaler = AHBPrescTable[(RCM->CFG1_B.AHBPSC)];
    SystemCoreClock >>= Prescaler;
}
/*!
 * @brief       Configures the System clock frequency, HCLK, PCLK2 and PCLK1 prescalers
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
static void SystemClockConfig(void)
{
    #ifdef SYSTEM_CLOCK_HSE
    SystemClockHSE();
    #elif defined SYSTEM_CLOCK_24MHz
    SystemClock24M();
    #elif defined SYSTEM_CLOCK_36MHz
    SystemClock36M();
    #elif defined SYSTEM_CLOCK_48MHz
    SystemClock48M();
    #endif
}

#if defined SYSTEM_CLOCK_HSE
/*!
 * @brief       Selects HSE as System clock source and configure HCLK, PCLK2 and PCLK1 prescalers
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
static void SystemClockHSE(void)
{
    uint32_t i;

    RCM->CTRL1_B.HSEEN= BIT_SET;

    for (i = 0; i < HSE_STARTUP_TIMEOUT; i++)
    {
        if (RCM->CTRL1_B.HSERDYFLG)
        {
            break;
        }
    }

    if (RCM->CTRL1_B.HSERDYFLG)
    {
        /* Enable Prefetch Buffer */
        FMC->CTRL1_B.PBEN = BIT_SET;
        /* Flash 0 wait state */
        FMC->CTRL1_B.WS = 0;

        /* HCLK = SYSCLK */
        RCM->CFG1_B.AHBPSC= 0X00;

        /* PCLK = HCLK */
        RCM->CFG1_B.APB1PSC = 0X00;

        /* Select HSE as system clock source */
        RCM->CFG1_B.SCLKSEL = 1;

        /** Wait till HSE is used as system clock source */
        while (RCM->CFG1_B.SCLKSWSTS!= 0x01);
    }
}


#elif defined SYSTEM_CLOCK_24MHz
/*!
 * @brief       Sets System clock frequency to 24MHz and configure HCLK, PCLK2 and PCLK1 prescalers
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
static void SystemClock24M(void)
{
    uint32_t i;

    RCM->CTRL1_B.HSEEN= BIT_SET;

    for (i = 0; i < HSE_STARTUP_TIMEOUT; i++)
    {
        if (RCM->CTRL1_B.HSERDYFLG)
        {
            break;
        }
    }

    if (RCM->CTRL1_B.HSERDYFLG)
    {
        /* Enable Prefetch Buffer */
        FMC->CTRL1_B.PBEN = BIT_SET;
        /* Flash 1 wait state */
        FMC->CTRL1_B.WS = 1;

        /* HCLK = SYSCLK */
        RCM->CFG1_B.AHBPSC= 0X00;

        /* PCLK = HCLK */
        RCM->CFG1_B.APB1PSC = 0X00;

        /** PLL: (HSE / 2) * 6 */
        RCM->CFG1_B.PLLSRCSEL = 1;
        RCM->CFG1_B.PLLHSEPSC = 1;
        RCM->CFG1_B.PLLMULCFG = 4;

        /** Enable PLL */
        RCM->CTRL1_B.PLLEN = 1;

        /** Wait PLL Ready */
        while (RCM->CTRL1_B.PLLRDYFLG == BIT_RESET);

        /* Select PLL as system clock source */
        RCM->CFG1_B.SCLKSEL = 2;

        /* Wait till PLL is used as system clock source */
        while (RCM->CFG1_B.SCLKSWSTS!= 0x02);
    }
}

#elif defined SYSTEM_CLOCK_36MHz
/*!
 * @brief       Sets System clock frequency to 36MHz and configure HCLK, PCLK2 and PCLK1 prescalers
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
static void SystemClock36M(void)
{
    uint32_t i;

    RCM->CTRL1_B.HSEEN= BIT_SET;

    for (i = 0; i < HSE_STARTUP_TIMEOUT; i++)
    {
        if (RCM->CTRL1_B.HSERDYFLG)
        {
            break;
        }
    }

    if (RCM->CTRL1_B.HSERDYFLG)
    {
        /* Enable Prefetch Buffer */
        FMC->CTRL1_B.PBEN = BIT_SET;
        /* Flash 1 wait state */
        FMC->CTRL1_B.WS = 1;

        /* HCLK = SYSCLK */
        RCM->CFG1_B.AHBPSC= 0X00;

        /* PCLK = HCLK */
        RCM->CFG1_B.APB1PSC = 0X00;

        /** PLL: (HSE / 2) * 9 */
        RCM->CFG1_B.PLLSRCSEL = 1;
        RCM->CFG1_B.PLLHSEPSC = 1;
        RCM->CFG1_B.PLLMULCFG = 7;

        /** Enable PLL */
        RCM->CTRL1_B.PLLEN = 1;

        /** Wait PLL Ready */
        while (RCM->CTRL1_B.PLLRDYFLG == BIT_RESET);

        /* Select PLL as system clock source */
        RCM->CFG1_B.SCLKSEL = 2;

        /* Wait till PLL is used as system clock source */
        while (RCM->CFG1_B.SCLKSWSTS != 0x02);
    }
}

#elif defined SYSTEM_CLOCK_48MHz
/*!
 * @brief       Sets System clock frequency to 46MHz and configure HCLK, PCLK2 and PCLK1 prescalers
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
static void SystemClock48M(void)
{
    uint32_t i;

    RCM->CTRL1_B.HSEEN= BIT_SET;

    for (i = 0; i < HSE_STARTUP_TIMEOUT; i++)
    {
        if (RCM->CTRL1_B.HSERDYFLG)
        {
            break;
        }
    }

    if (RCM->CTRL1_B.HSERDYFLG)
    {
        /* Enable Prefetch Buffer */
        FMC->CTRL1_B.PBEN = BIT_SET;
        /* Flash 1 wait state */
        FMC->CTRL1_B.WS = 1;

        /* HCLK = SYSCLK */
        RCM->CFG1_B.AHBPSC= 0X00;

        /* PCLK = HCLK */
        RCM->CFG1_B.APB1PSC = 0X00;

        /** PLL: HSE * 6 */
        RCM->CFG1_B.PLLSRCSEL = 1;
        RCM->CFG1_B.PLLMULCFG = 4;

        /** Enable PLL */
        RCM->CTRL1_B.PLLEN = 1;

        /** Wait PLL Ready */
        while (RCM->CTRL1_B.PLLRDYFLG == BIT_RESET);

        /* Select PLL as system clock source */
        RCM->CFG1_B.SCLKSEL = 2;

        /* Wait till PLL is used as system clock source */
        while (RCM->CFG1_B.SCLKSWSTS!= 0x02);
    }
}
#endif
