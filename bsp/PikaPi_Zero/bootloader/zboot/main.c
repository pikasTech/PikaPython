#include "zboot_misc.h"
#include "usart.h"
#include "iap.h"
#include "xprintf.h"
#include "flash_eeprom.h"
#include <stdio.h>

static int flag_jump = 0;

void SystemInit(void) {
#if defined (GD32F350) || defined (GD32F130_150)
//    rcu_deinit();
    RCU_APB2EN = BIT(0);
    RCU_CTL0 |= RCU_CTL0_IRC8MEN;// enable IRC8M
    ( {  while(RESET == (RCU_CTL0 & RCU_CTL0_IRC8MSTB));}); // reset RCU
    RCU_CFG0 = 0x00000000;
    RCU_CFG1 = 0x00000000;
    RCU_CFG2 = 0x00000000;
    RCU_CTL0 &= ~(RCU_CTL0_HXTALEN | RCU_CTL0_CKMEN | RCU_CTL0_PLLEN
            | RCU_CTL0_HXTALBPS);
#if defined (GD32F350)
    RCU_CTL1 &= ~RCU_CTL1_IRC28MEN;
    RCU_ADDCTL &= ~RCU_ADDCTL_IRC48MEN;
    RCU_ADDINT = 0x00000000U;
#endif
    RCU_INT = 0x00000000U;
#elif defined (STM32F303xC) || defined (STM32F072) || defined (STM32F030) || defined (STM32F10X_HD)|| defined (STM32F401xx) || defined (STM32F042) || defined (STM32F10X_MD_VL) || defined (STM32F051) || defined (STM32F030xC)
    RCC_DeInit();
#if _USE_PLL
    FLASH_SetLatency(FLASH_Latency_1);
    RCC_HCLKConfig(RCC_SYSCLK_Div1);    // AHB CLK = SYSCLK
#if defined (STM32F10X_HD) || defined (STM32F10X_MD_VL) || defined (STM32F303xC)
    RCC_PCLK1Config(RCC_HCLK_Div1);    // APB1 CLK = SYSCLK
    RCC_PCLK2Config(RCC_HCLK_Div1);    // APB1 CLK = SYSCLK
    RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_6); // 4M * 6 = 24M, which is the highest freq could be supported by all stm32's.
#else
    RCC_PCLKConfig(RCC_HCLK_Div1);    // APB1 CLK = SYSCLK
    RCC_PLLConfig(RCC_PLLSource_HSI, RCC_PLLMul_3); // 8M * 3 = 24M, which is the highest freq could be supported by all stm32's.
#endif
    RCC_PLLCmd(ENABLE);
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    while(RCC_GetSYSCLKSource() != 0x08);    
#endif
//    FLASH_PrefetchBufferCmd(ENABLE);
#if defined (STM32F401xx)
    FLASH->ACR |= FLASH_ACR_PRFTEN;
#else
    FLASH->ACR |= FLASH_ACR_PRFTBE;
#endif
    FLASH->ACR &= ~FLASH_ACR_LATENCY;
#endif

#if defined (GD32F350) || defined (GD32F130_150)
    SCB->VTOR = NVIC_VECTTAB_FLASH;
#elif defined (STM32F303xC) | defined (STM32F10X_HD) | defined (STM32F401xx) || defined (STM32F10X_MD_VL)
    SCB->VTOR = NVIC_VectTab_FLASH;     // invalid on cortex-m0
#endif

}

int main(void) {
	SystemInit();
	SysTick_Config(8000000UL); // delay 1s and then jump to app

	IAP_Config();
	USART_Config();
#if _USE_EEPROM
	IAP_Sysinfo_t *inf = IAP_GetInfo();
	FLASH_EEPROM_Config(inf->eeprom_base, FLASH_PAGE_SIZE);
#endif

#if defined (GD32F350) || defined (STM32F303xC) || defined (STM32F10X_HD) || defined (GD32F130_150) || defined (STM32F10X_MD_VL)
    *(unsigned long*)0xe000ed24 = 0x00070000; // enable usage fault
#endif
	while (1) {
		USART_Poll();
		if (flag_jump) {
			// xprintf("jump\n");
			IAP_JumpToApp();
		}
	}
}

void SysTick_Handler(void) {
	if (IAP_IsAppValid() && USART_NoComm()) {
		flag_jump = 1;      // do not simply jump from here, use a flag instead
	}
}

void BusFault_Handler(void) {
#if defined (GD32F350) || defined (STM32F303xC) || defined (STM32F10X_HD) || defined (GD32F130_150) || defined (STM32F10X_MD_VL)
    if(SCB->CFSR & 0x0200) {
        xprintf("Read error @ %08lx\n", (unsigned long)(SCB->BFAR));
    }
#endif
	NVIC_SystemReset();
    ({	while (1);});
}
void _init(void) {
}
