#ifndef __STM32G030_PIKA_PORT__H
#define __STM32G030_PIKA_PORT__H

#include <stdint.h>
#include "pikaObj.h"
typedef struct _CodeHeap {
    char* content;
    uint32_t size;
    uint8_t ena;
    uint32_t reciveTime;
    uint32_t oldSize;
} CodeHeap;

/* support std lib for stm32 */
#define delay_ms HAL_Delay
#undef u16
#undef u8
#undef u32
#define u16 uint16_t
#define u8 uint8_t
#define u32 uint32_t

#define GPIO_Pin_0 GPIO_PIN_0
#define GPIO_Pin_1 GPIO_PIN_1
#define GPIO_Pin_2 GPIO_PIN_2
#define GPIO_Pin_3 GPIO_PIN_3
#define GPIO_Pin_4 GPIO_PIN_4
#define GPIO_Pin_5 GPIO_PIN_5
#define GPIO_Pin_6 GPIO_PIN_6
#define GPIO_Pin_7 GPIO_PIN_7
#define GPIO_Pin_8 GPIO_PIN_8
#define GPIO_Pin_9 GPIO_PIN_9
#define GPIO_Pin_10 GPIO_PIN_10
#define GPIO_Pin_11 GPIO_PIN_11
#define GPIO_Pin_12 GPIO_PIN_12
#define GPIO_Pin_13 GPIO_PIN_13
#define GPIO_Pin_14 GPIO_PIN_14
#define GPIO_Pin_15 GPIO_PIN_15

/* support pinrtf */
void HARDWARE_PRINTF_Init(void);

/* support write asm to flash */
#define FLASH_SCRIPT_START_ADDR \
    (FLASH_BASE + ((FLASH_PAGE_NB - 2) * FLASH_PAGE_SIZE))
#define FLASH_SCRIPT_END_ADDR (FLASH_BASE + FLASH_SIZE - 1)
#define FLASH_PIKA_ASM_START_ADDR FLASH_SCRIPT_START_ADDR
#define FLASH_PIKA_ASM_END_ADDR FLASH_SCRIPT_END_ADDR

#define RX_BUFF_LENGTH 32

/* support download python script by uart1 */
uint8_t STM32_Code_reciveHandler(char* data, uint32_t rxSize);
void STM32_Code_Init();
void STM32_Code_flashHandler();

/* handler for usart1 */
void __PIKA_USART1_IRQHandler(char rx_char);
#endif