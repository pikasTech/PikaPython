#ifndef __WM_HAL_GPIO_EX_H__
#define __WM_HAL_GPIO_EX_H__

#include "wm_hal.h"

#define __AFIO_REMAP_SET_OPT1(__HANDLE__, __IOPOSITION__) do{                             \
													SET_BIT(__HANDLE__->AF_SEL, __IOPOSITION__);	\
													CLEAR_BIT(__HANDLE__->AF_S1, __IOPOSITION__);	\
													CLEAR_BIT(__HANDLE__->AF_S0, __IOPOSITION__);	\
											}while (0)
												
#define __AFIO_REMAP_SET_OPT2(__HANDLE__, __IOPOSITION__) do{                             \
													SET_BIT(__HANDLE__->AF_SEL, __IOPOSITION__);	\
													CLEAR_BIT(__HANDLE__->AF_S1, __IOPOSITION__);	\
													SET_BIT(__HANDLE__->AF_S0, __IOPOSITION__);	\
											}while (0)
												
#define __AFIO_REMAP_SET_OPT3(__HANDLE__, __IOPOSITION__) do{                             \
													SET_BIT(__HANDLE__->AF_SEL, __IOPOSITION__);	\
													SET_BIT(__HANDLE__->AF_S1, __IOPOSITION__);	\
													CLEAR_BIT(__HANDLE__->AF_S0, __IOPOSITION__);	\
											}while (0)
												
#define __AFIO_REMAP_SET_OPT4(__HANDLE__, __IOPOSITION__) do{                             \
													SET_BIT(__HANDLE__->AF_SEL, __IOPOSITION__);	\
													SET_BIT(__HANDLE__->AF_S1, __IOPOSITION__);	\
													SET_BIT(__HANDLE__->AF_S0, __IOPOSITION__);	\
											}while (0)
												
#define __AFIO_REMAP_SET_OPT5(__HANDLE__, __IOPOSITION__) do{                             \
													CLEAR_BIT(__HANDLE__->AF_SEL, __IOPOSITION__);	\
											}while (0)
												
#define __AFIO_REMAP_SET_OPT6(__HANDLE__, __IOPOSITION__) do{                             \
													CLEAR_BIT(__HANDLE__->AF_SEL, __IOPOSITION__);	\
													CLEAR_BIT(__HANDLE__->DIR, __IOPOSITION__);	\
													SET_BIT(__HANDLE__->PULLUP_EN, __IOPOSITION__);	\
													CLEAR_BIT(__HANDLE__->PULLDOWN_EN, __IOPOSITION__);	\
											}while (0)
												
#define __AFIO_REMAP_SET_OPT7(__HANDLE__, __IOPOSITION__) do{								\
													CLEAR_BIT(__HANDLE__->AF_SEL, __IOPOSITION__);	\
													CLEAR_BIT(__HANDLE__->DIR, __IOPOSITION__);	\
													SET_BIT(__HANDLE__->PULLUP_EN, __IOPOSITION__);	\
													CLEAR_BIT(__HANDLE__->PULLDOWN_EN, __IOPOSITION__);	\
											}while (0)

#define __HAL_AFIO_REMAP_SPI_CLK(__HANDLE__, __IOPOSITION__)	do{                             \
												if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_1))	\
												{													\
													__AFIO_REMAP_SET_OPT2(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_2))	\
												{													\
													__AFIO_REMAP_SET_OPT2(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_15))	\
												{													\
													__AFIO_REMAP_SET_OPT3(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_24))	\
												{													\
													__AFIO_REMAP_SET_OPT1(__HANDLE__, __IOPOSITION__);	\
												}													\
											}while (0)												

#define __HAL_AFIO_REMAP_SPI_CS(__HANDLE__, __IOPOSITION__)	do{                             \
												if ((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_0))	\
												{													\
													__AFIO_REMAP_SET_OPT2(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_4))	\
												{													\
													__AFIO_REMAP_SET_OPT1(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_14))	\
												{													\
													__AFIO_REMAP_SET_OPT3(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_23))	\
												{													\
													__AFIO_REMAP_SET_OPT1(__HANDLE__, __IOPOSITION__);	\
												}													\
											}while (0)	
												
#define __HAL_AFIO_REMAP_SPI_MISO(__HANDLE__, __IOPOSITION__)	do{                             \
												if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_0))	\
												{													\
													__AFIO_REMAP_SET_OPT2(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_3))	\
												{													\
													__AFIO_REMAP_SET_OPT2(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_16))	\
												{													\
													__AFIO_REMAP_SET_OPT3(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_25))	\
												{													\
													__AFIO_REMAP_SET_OPT1(__HANDLE__, __IOPOSITION__);	\
												}													\
											}while (0)	

#define __HAL_AFIO_REMAP_SPI_MOSI(__HANDLE__, __IOPOSITION__)	do{                             \
												if ((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_7))	\
												{													\
													__AFIO_REMAP_SET_OPT2(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_5))	\
												{													\
													__AFIO_REMAP_SET_OPT1(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_17))	\
												{													\
													__AFIO_REMAP_SET_OPT3(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_26))	\
												{													\
													__AFIO_REMAP_SET_OPT1(__HANDLE__, __IOPOSITION__);	\
												}													\
											}while (0)													
												
#define __HAL_AFIO_REMAP_UART0_TX(__HANDLE__, __IOPOSITION__)	do{                             \
												if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_19))	\
												{													\
													__AFIO_REMAP_SET_OPT1(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_27))	\
												{													\
													__AFIO_REMAP_SET_OPT2(__HANDLE__, __IOPOSITION__);	\
												}													\
											}while (0)

#define __HAL_AFIO_REMAP_UART0_RX(__HANDLE__, __IOPOSITION__)	do{                             \
												if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_20))	\
												{													\
													__AFIO_REMAP_SET_OPT1(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_28))	\
												{													\
													__AFIO_REMAP_SET_OPT2(__HANDLE__, __IOPOSITION__);	\
												}													\
												CLEAR_BIT(__HANDLE__->PULLUP_EN, __IOPOSITION__);	\
											}while (0)

#define __HAL_AFIO_REMAP_UART0_RTS(__HANDLE__, __IOPOSITION__)	do{                             \
												if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_21))	\
												{													\
													__AFIO_REMAP_SET_OPT1(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_29))	\
												{													\
													__AFIO_REMAP_SET_OPT2(__HANDLE__, __IOPOSITION__);	\
												}													\
											}while (0)
												
#define __HAL_AFIO_REMAP_UART0_CTS(__HANDLE__, __IOPOSITION__)	do{                             \
												if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_22))	\
												{													\
													__AFIO_REMAP_SET_OPT1(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_30))	\
												{													\
													__AFIO_REMAP_SET_OPT2(__HANDLE__, __IOPOSITION__);	\
												}													\
											}while (0)

#define __HAL_AFIO_REMAP_UART1_TX(__HANDLE__, __IOPOSITION__)	do{                             \
												if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_6))	\
												{													\
													__AFIO_REMAP_SET_OPT1(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_31))	\
												{													\
													__AFIO_REMAP_SET_OPT2(__HANDLE__, __IOPOSITION__);	\
												}													\
											}while (0)

#define __HAL_AFIO_REMAP_UART1_RX(__HANDLE__, __IOPOSITION__)	do{                             \
												if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_7))	\
												{													\
													__AFIO_REMAP_SET_OPT1(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_16))	\
												{													\
													__AFIO_REMAP_SET_OPT4(__HANDLE__, __IOPOSITION__);	\
												}													\
												CLEAR_BIT(__HANDLE__->PULLUP_EN, __IOPOSITION__);	\
											}while (0)

#define __HAL_AFIO_REMAP_UART1_RTS(__HANDLE__, __IOPOSITION__)	do{                             \
												if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_19))	\
												{													\
													__AFIO_REMAP_SET_OPT3(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_2))	\
												{													\
													__AFIO_REMAP_SET_OPT1(__HANDLE__, __IOPOSITION__);	\
												}													\
											}while (0)

#define __HAL_AFIO_REMAP_UART1_CTS(__HANDLE__, __IOPOSITION__)	do{                             \
												if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_20))	\
												{													\
													__AFIO_REMAP_SET_OPT3(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_3))	\
												{													\
													__AFIO_REMAP_SET_OPT1(__HANDLE__, __IOPOSITION__);	\
												}													\
											}while (0)

#define __HAL_AFIO_REMAP_UART2_TX(__HANDLE__, __IOPOSITION__)	do{                             \
												if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_2))	\
												{													\
													__AFIO_REMAP_SET_OPT3(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_2))	\
												{													\
													__AFIO_REMAP_SET_OPT2(__HANDLE__, __IOPOSITION__);	\
												}													\
											}while (0)

#define __HAL_AFIO_REMAP_UART2_RX(__HANDLE__, __IOPOSITION__)	do{                             \
												if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_3))	\
												{													\
													__AFIO_REMAP_SET_OPT3(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_3))	\
												{													\
													__AFIO_REMAP_SET_OPT2(__HANDLE__, __IOPOSITION__);	\
												}													\
												CLEAR_BIT(__HANDLE__->PULLUP_EN, __IOPOSITION__);	\
											}while (0)

#define __HAL_AFIO_REMAP_UART2_RTS(__HANDLE__, __IOPOSITION__)	do{                             \
												if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_4))	\
												{													\
													__AFIO_REMAP_SET_OPT2(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_5))	\
												{													\
													__AFIO_REMAP_SET_OPT2(__HANDLE__, __IOPOSITION__);	\
												}													\
											}while (0)
												
#define __HAL_AFIO_REMAP_UART2_CTS(__HANDLE__, __IOPOSITION__)	do{                             \
												if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_5))	\
												{													\
													__AFIO_REMAP_SET_OPT2(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_6))	\
												{													\
													__AFIO_REMAP_SET_OPT2(__HANDLE__, __IOPOSITION__);	\
												}													\
											}while (0)

#define __HAL_AFIO_REMAP_UART3_TX(__HANDLE__, __IOPOSITION__)	do{                             \
												if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_0))	\
												{													\
													__AFIO_REMAP_SET_OPT3(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_5))	\
												{													\
													__AFIO_REMAP_SET_OPT1(__HANDLE__, __IOPOSITION__);	\
												}													\
											}while (0)

#define __HAL_AFIO_REMAP_UART3_RX(__HANDLE__, __IOPOSITION__)	do{                             \
												if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_1))	\
												{													\
													__AFIO_REMAP_SET_OPT3(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_6))	\
												{													\
													__AFIO_REMAP_SET_OPT1(__HANDLE__, __IOPOSITION__);	\
												}													\
												CLEAR_BIT(__HANDLE__->PULLUP_EN, __IOPOSITION__);	\
											}while (0)

#define __HAL_AFIO_REMAP_UART3_RTS(__HANDLE__, __IOPOSITION__)	do{                             \
												if ((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_2))	\
												{													\
													__AFIO_REMAP_SET_OPT4(__HANDLE__, __IOPOSITION__);	\
												}													\
											}while (0)
												
#define __HAL_AFIO_REMAP_UART3_CTS(__HANDLE__, __IOPOSITION__)	do{                             \
												if ((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_3))	\
												{													\
													__AFIO_REMAP_SET_OPT4(__HANDLE__, __IOPOSITION__);	\
												}													\
											}while (0)

#define __HAL_AFIO_REMAP_UART4_TX(__HANDLE__, __IOPOSITION__)	do{                             \
												if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_4))	\
												{													\
													__AFIO_REMAP_SET_OPT3(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_8))	\
												{													\
													__AFIO_REMAP_SET_OPT2(__HANDLE__, __IOPOSITION__);	\
												}													\
											}while (0)

#define __HAL_AFIO_REMAP_UART4_RX(__HANDLE__, __IOPOSITION__)	do{                             \
												if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_5))	\
												{													\
													__AFIO_REMAP_SET_OPT3(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_9))	\
												{													\
													__AFIO_REMAP_SET_OPT2(__HANDLE__, __IOPOSITION__);	\
												}													\
												CLEAR_BIT(__HANDLE__->PULLUP_EN, __IOPOSITION__);	\
											}while (0)

#define __HAL_AFIO_REMAP_UART4_RTS(__HANDLE__, __IOPOSITION__)	do{                             \
												if ((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_5))	\
												{													\
													__AFIO_REMAP_SET_OPT4(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_10))	\
												{													\
													__AFIO_REMAP_SET_OPT2(__HANDLE__, __IOPOSITION__);	\
												}													\
											}while (0)

#define __HAL_AFIO_REMAP_UART4_CTS(__HANDLE__, __IOPOSITION__)	do{                             \
												if ((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_6))	\
												{													\
													__AFIO_REMAP_SET_OPT4(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_11))	\
												{													\
													__AFIO_REMAP_SET_OPT2(__HANDLE__, __IOPOSITION__);	\
												}													\
											}while (0)
#define __HAL_AFIO_REMAP_UART5_TX(__HANDLE__, __IOPOSITION__)	do{                             \
												if ((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_12))	\
												{													\
													__AFIO_REMAP_SET_OPT2(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_8))	\
												{													\
													__AFIO_REMAP_SET_OPT3(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_18))	\
												{													\
													__AFIO_REMAP_SET_OPT1(__HANDLE__, __IOPOSITION__);	\
												}													\
											}while (0)

#define __HAL_AFIO_REMAP_UART5_RX(__HANDLE__, __IOPOSITION__)	do{                             \
												if ((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_13))	\
												{													\
													__AFIO_REMAP_SET_OPT2(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_9))	\
												{													\
													__AFIO_REMAP_SET_OPT3(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_17))	\
												{													\
													__AFIO_REMAP_SET_OPT1(__HANDLE__, __IOPOSITION__);	\
												}													\
												CLEAR_BIT(__HANDLE__->PULLUP_EN, __IOPOSITION__);	\
											}while (0)

#define __HAL_AFIO_REMAP_UART5_RTS(__HANDLE__, __IOPOSITION__)	do{                             \
												if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_12))	\
												{													\
													__AFIO_REMAP_SET_OPT3(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_14))	\
												{													\
													__AFIO_REMAP_SET_OPT2(__HANDLE__, __IOPOSITION__);	\
												}													\
											}while (0)

#define __HAL_AFIO_REMAP_UART5_CTS(__HANDLE__, __IOPOSITION__)	do{                             \
												if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_13))	\
												{													\
													__AFIO_REMAP_SET_OPT3(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_15))	\
												{													\
													__AFIO_REMAP_SET_OPT2(__HANDLE__, __IOPOSITION__);	\
												}													\
											}while (0)

#define __HAL_AFIO_REMAP_ADC(__HANDLE__, __IOPOSITION__)	do{                             \
												if ((__HANDLE__ == GPIOA) && 		\
												((__IOPOSITION__ == GPIO_PIN_1) || (__IOPOSITION__ == GPIO_PIN_2) || \
												 (__IOPOSITION__ == GPIO_PIN_3) || (__IOPOSITION__ == GPIO_PIN_4)))	\
												{													\
													__AFIO_REMAP_SET_OPT6(__HANDLE__, __IOPOSITION__);	\
												}													\
											}while (0)
												
#define __HAL_AFIO_REMAP_PWM0(__HANDLE__, __IOPOSITION__)	do{                             \
												if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_0))	\
												{													\
													__AFIO_REMAP_SET_OPT1(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_19))	\
												{													\
													__AFIO_REMAP_SET_OPT2(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_12))	\
												{													\
													__AFIO_REMAP_SET_OPT2(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_2))	\
												{													\
													__AFIO_REMAP_SET_OPT3(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_10))	\
												{													\
													__AFIO_REMAP_SET_OPT3(__HANDLE__, __IOPOSITION__);	\
												}													\
											}while (0)
												
#define __HAL_AFIO_REMAP_PWM1(__HANDLE__, __IOPOSITION__)	do{                             \
												if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_1))	\
												{													\
													__AFIO_REMAP_SET_OPT1(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_20))	\
												{													\
													__AFIO_REMAP_SET_OPT2(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_3))	\
												{													\
													__AFIO_REMAP_SET_OPT3(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_11))	\
												{													\
													__AFIO_REMAP_SET_OPT3(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_13))	\
												{													\
													__AFIO_REMAP_SET_OPT3(__HANDLE__, __IOPOSITION__);	\
												}													\
											}while (0)

#define __HAL_AFIO_REMAP_PWM2(__HANDLE__, __IOPOSITION__)	do{                             \
												if ((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_0))	\
												{													\
													__AFIO_REMAP_SET_OPT3(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_2))	\
												{													\
													__AFIO_REMAP_SET_OPT1(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_12))	\
												{													\
													__AFIO_REMAP_SET_OPT3(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_14))	\
												{													\
													__AFIO_REMAP_SET_OPT2(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_24))	\
												{													\
													__AFIO_REMAP_SET_OPT2(__HANDLE__, __IOPOSITION__);	\
												}													\
											}while (0)

#define __HAL_AFIO_REMAP_PWM3(__HANDLE__, __IOPOSITION__)	do{                             \
												if ((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_1))	\
												{													\
													__AFIO_REMAP_SET_OPT3(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_3))	\
												{													\
													__AFIO_REMAP_SET_OPT1(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_13))	\
												{													\
													__AFIO_REMAP_SET_OPT3(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_15))	\
												{													\
													__AFIO_REMAP_SET_OPT2(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_25))	\
												{													\
													__AFIO_REMAP_SET_OPT2(__HANDLE__, __IOPOSITION__);	\
												}													\
											}while (0)

#define __HAL_AFIO_REMAP_PWM4(__HANDLE__, __IOPOSITION__)	do{                             \
												if ((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_4))	\
												{													\
													__AFIO_REMAP_SET_OPT3(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_7))	\
												{													\
													__AFIO_REMAP_SET_OPT1(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_14))	\
												{													\
													__AFIO_REMAP_SET_OPT3(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_16))	\
												{													\
													__AFIO_REMAP_SET_OPT2(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_26))	\
												{													\
													__AFIO_REMAP_SET_OPT2(__HANDLE__, __IOPOSITION__);	\
												}													\
											}while (0)
											
#define __HAL_AFIO_REMAP_I2C_SCL(__HANDLE__, __IOPOSITION__)	do{                             \
												if ((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_1))	\
												{													\
													__AFIO_REMAP_SET_OPT2(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_20))	\
												{													\
													__AFIO_REMAP_SET_OPT4(__HANDLE__, __IOPOSITION__);	\
												}													\
											}while (0)	

#define __HAL_AFIO_REMAP_I2C_SDA(__HANDLE__, __IOPOSITION__)	do{                             \
												if ((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_4))	\
												{													\
													__AFIO_REMAP_SET_OPT2(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_19))	\
												{													\
													__AFIO_REMAP_SET_OPT4(__HANDLE__, __IOPOSITION__);	\
												}													\
											}while (0)	

#define __HAL_AFIO_REMAP_TOUCH(__HANDLE__, __IOPOSITION__)		do{									\
												if (((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_7)) || \
													((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_9)) || \
													((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_10)) || \
													((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_0)) || \
													((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_1)) || \
													((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_2)) || \
													((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_3)) || \
													((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_4)) || \
													((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_5)) || \
													((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_6)) || \
													((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_7)) || \
													((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_8)) || \
													((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_9)) || \
													((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_12)) || \
													((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_14)) || \
													((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_29)))	\
												{													\
													if ((__HANDLE__ != GPIOA) || (__IOPOSITION__ != GPIO_PIN_7))	\
													{												\
														__AFIO_REMAP_SET_OPT7(GPIOA, GPIO_PIN_7);		\
													}												\
													__AFIO_REMAP_SET_OPT7(__HANDLE__, __IOPOSITION__);		\
												}													\
											}while (0)

#define __HAL_AFIO_REMAP_I2S_EXT_MCK(__HANDLE__, __IOPOSITION__)	do{									\
												if ((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_7))	\
												{													\
													__AFIO_REMAP_SET_OPT3(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_17))	\
												{													\
													__AFIO_REMAP_SET_OPT4(__HANDLE__, __IOPOSITION__);	\
												}													\
											}while (0)
												
#define __HAL_AFIO_REMAP_I2S_MCK(__HANDLE__, __IOPOSITION__)	do{									\
												if ((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_0))	\
												{													\
													__AFIO_REMAP_SET_OPT1(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_7))	\
												{													\
													__AFIO_REMAP_SET_OPT3(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_17))	\
												{													\
													__AFIO_REMAP_SET_OPT4(__HANDLE__, __IOPOSITION__);	\
												}													\
											}while (0)
												
#define __HAL_AFIO_REMAP_I2S_WS(__HANDLE__, __IOPOSITION__)	do{									\
												if ((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_1))	\
												{													\
													__AFIO_REMAP_SET_OPT4(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_9))	\
												{													\
													__AFIO_REMAP_SET_OPT1(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_9))	\
												{													\
													__AFIO_REMAP_SET_OPT4(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_13))	\
												{													\
													__AFIO_REMAP_SET_OPT4(__HANDLE__, __IOPOSITION__);	\
												}													\
											}while (0)
												
#define __HAL_AFIO_REMAP_I2S_CK(__HANDLE__, __IOPOSITION__)	do{									\
												if ((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_4))	\
												{													\
													__AFIO_REMAP_SET_OPT4(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_8))	\
												{													\
													__AFIO_REMAP_SET_OPT1(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_8))	\
												{													\
													__AFIO_REMAP_SET_OPT4(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_12))	\
												{													\
													__AFIO_REMAP_SET_OPT4(__HANDLE__, __IOPOSITION__);	\
												}													\
											}while (0)
												
#define __HAL_AFIO_REMAP_I2S_MOSI(__HANDLE__, __IOPOSITION__)	do{									\
												if ((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_0))	\
												{													\
													__AFIO_REMAP_SET_OPT4(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_11))	\
												{													\
													__AFIO_REMAP_SET_OPT1(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_10))	\
												{													\
													__AFIO_REMAP_SET_OPT4(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_14))	\
												{													\
													__AFIO_REMAP_SET_OPT4(__HANDLE__, __IOPOSITION__);	\
												}													\
											}while (0)
												
#define __HAL_AFIO_REMAP_I2S_MISO(__HANDLE__, __IOPOSITION__)	do{									\
												if ((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_7))	\
												{													\
													__AFIO_REMAP_SET_OPT4(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_10))	\
												{													\
													__AFIO_REMAP_SET_OPT1(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOA) && (__IOPOSITION__ == GPIO_PIN_11))	\
												{													\
													__AFIO_REMAP_SET_OPT4(__HANDLE__, __IOPOSITION__);	\
												}													\
												else if ((__HANDLE__ == GPIOB) && (__IOPOSITION__ == GPIO_PIN_15))	\
												{													\
													__AFIO_REMAP_SET_OPT4(__HANDLE__, __IOPOSITION__);	\
												}													\
											}while (0)
												
#define __HAL_AFIO_REMAP_SWJ_ENABLE	do {									\
										__AFIO_REMAP_SET_OPT1(GPIOA, GPIO_PIN_1);	\
										__AFIO_REMAP_SET_OPT1(GPIOA, GPIO_PIN_4);	\
									} while(0)

#define __HAL_AFIO_REMAP_SWJ_DISABLE	do {								\
											__AFIO_REMAP_SET_OPT5(GPIOA, GPIO_PIN_1);	\
											__AFIO_REMAP_SET_OPT5(GPIOA, GPIO_PIN_4);	\
										} while(0)										

#endif