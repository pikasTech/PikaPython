#ifndef __STM32F1xx_ll_CONF_H
#define __STM32F1xx_ll_CONF_H

// #define LL_MODULE_ENABLED
#define LL_ADC_MODULE_ENABLED 
#define LL_BUS_MODULE_ENABLED 
#define LL_CORTEX_MODULE_ENABLED
#define LL_CRC_MODULE_ENABLED
#define LL_DAC_MODULE_ENABLED 
#define LL_DMA_MODULE_ENABLED 
#define LL_EXTI_MODULE_ENABLED 
#define LL_FSMC_MODULE_ENABLED 
#define LL_GPIO_MODULE_ENABLED
#define LL_I2C_MODULE_ENABLED 
#define LL_IWDG_MODULE_ENABLED 
#define LL_PWR_MODULE_ENABLED 
#define LL_RCC_MODULE_ENABLED
#define LL_RTC_MODULE_ENABLED 
#define LL_SDMMC_MODULE_ENABLED 
#define LL_SPI_MODULE_ENABLED 
#define LL_SYSTEM_MODULE_ENABLED 
#define LL_TIM_MODULE_ENABLED 
#define LL_USART_MODULE_ENABLED
#define LL_USB_MODULE_ENABLED
#define LL_UTILS_MODULE_ENABLED    
#define LL_WWDG_MODULE_ENABLED 


#ifdef LL_ADC_MODULE_ENABLED
 #include "stm32f1xx_ll_adc.h"
#endif /* LL_ADC_MODULE_ENABLED */

#ifdef LL_BUS_MODULE_ENABLED
 #include "stm32f1xx_ll_bus.h"
#endif /* LL_BUS_MODULE_ENABLED */

#ifdef LL_CORTEX_MODULE_ENABLED
 #include "stm32f1xx_ll_cortex.h"
#endif /* LL_CORTEX_MODULE_ENABLED */

#ifdef LL_CRC_MODULE_ENABLED
 #include "stm32f1xx_ll_crc.h"
#endif /* LL_CRC_MODULE_ENABLED */

#ifdef LL_DAC_MODULE_ENABLED
 #include "stm32f1xx_ll_dac.h"
#endif /* LL_DAC_MODULE_ENABLED */

#ifdef LL_DMA_MODULE_ENABLED
  #include "stm32f1xx_ll_dma.h"
#endif /* LL_DMA_MODULE_ENABLED */

#ifdef LL_EXTI_MODULE_ENABLED
  #include "stm32f1xx_ll_exti.h"
#endif /* LL_EXTI_MODULE_ENABLED */

#ifdef LL_FSMC_MODULE_ENABLED
  #include "stm32f1xx_ll_fsmc.h"
#endif /* LL_FSMC_MODULE_ENABLED */

#ifdef LL_GPIO_MODULE_ENABLED
 #include "stm32f1xx_ll_gpio.h"
#endif /* LL_GPIO_MODULE_ENABLED */

#ifdef LL_I2C_MODULE_ENABLED
 #include "stm32f1xx_ll_i2c.h"
#endif /* LL_I2C_MODULE_ENABLED */

#ifdef LL_IWDG_MODULE_ENABLED
 #include "stm32f1xx_ll_iwdg.h"
#endif /* LL_IWDG_MODULE_ENABLED */

#ifdef LL_PWR_MODULE_ENABLED
 #include "stm32f1xx_ll_pwr.h"
#endif /* LL_PWR_MODULE_ENABLED */

#ifdef LL_RCC_MODULE_ENABLED
 #include "stm32f1xx_ll_rcc.h"
#endif /* LL_RCC_MODULE_ENABLED */

#ifdef LL_RTC_MODULE_ENABLED
 #include "stm32f1xx_ll_rtc.h"
#endif /* LL_RTC_MODULE_ENABLED */

#ifdef LL_SDMMC_MODULE_ENABLED
 #include "stm32f1xx_ll_sdmmc.h"
#endif /* LL_SDMMC_MODULE_ENABLED */

#ifdef LL_SPI_MODULE_ENABLED
 #include "stm32f1xx_ll_spi.h"
#endif /* LL_SPI_MODULE_ENABLED */

#ifdef LL_SYSTEM_MODULE_ENABLED
 #include "stm32f1xx_ll_system.h"
#endif /* LL_SYSTEM_MODULE_ENABLED */

#ifdef LL_TIM_MODULE_ENABLED
 #include "stm32f1xx_ll_tim.h"
#endif /* LL_TIM_MODULE_ENABLED */

#ifdef LL_USART_MODULE_ENABLED
 #include "stm32f1xx_ll_usart.h"
#endif /* LL_USART_MODULE_ENABLED */

#ifdef LL_USB_MODULE_ENABLED
 #include "stm32f1xx_ll_usb.h"
#endif /* LL_USB_MODULE_ENABLED */

#ifdef LL_UTILS_MODULE_ENABLED
 #include "stm32f1xx_ll_utils.h"
#endif /* LL_UTILS_MODULE_ENABLED */

#ifdef LL_WWDG_MODULE_ENABLED
 #include "stm32f1xx_ll_wwdg.h"
#endif /* LL_WWDG_MODULE_ENABLED */

#endif
