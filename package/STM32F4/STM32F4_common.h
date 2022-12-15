#ifndef __STM32__COMMON__H
#define __STM32__COMMON__H
#include "PikaObj.h"
#include <stdint.h>

/* support std lib for stm32 */
#define delay_ms HAL_Delay
typedef uint16_t u16;
typedef uint8_t u8;
typedef uint32_t u32;

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

#include "stm32f4xx_hal.h"

/* default hardware config */
#define UART1_EXIST
#define UART2_EXIST
#define UART3_EXIST
#define UART4_EXIST

#define TIM1_EXIST
#define TIM2_EXIST
#define TIM3_EXIST
#define TIM4_EXIST

/* configuration for STM32F411xE */
#ifdef STM32F411xE
    #undef UART3_EXIST
    #undef UART4_EXIST
#endif

/* configuration for STM32F401xC */
#ifdef STM32F401xC
    #undef UART3_EXIST
    #undef UART4_EXIST
#endif

#ifdef STM32F407xx
    #undef UART4_EXIST
#endif

#ifdef STM32F405xx
    #undef UART4_EXIST
#endif

#define RX_BUFF_LENGTH 64

#define FLASH_SCRIPT_START_ADDR (FLASH_BASE + ((FLASH_PAGE_NB - 2) * FLASH_PAGE_SIZE))
#define FLASH_SCRIPT_END_ADDR (FLASH_BASE + FLASH_SIZE - 1)

#define FLASH_PIKA_ASM_START_ADDR FLASH_SCRIPT_START_ADDR
#define FLASH_PIKA_ASM_END_ADDR FLASH_SCRIPT_END_ADDR

uint32_t GetPage(uint32_t Addr);
#define DATA_64 ((uint64_t)0x1234567812345678)
#define DATA_32 ((uint32_t)0x12345678)

typedef struct {
    UART_HandleTypeDef huart;
    uint8_t id;
    char rxBuff[RX_BUFF_LENGTH];
    uint16_t rxBuffOffset;
    PikaObj* obj;
} pika_uart_t;

typedef struct _CodeHeap{
    char *content;
    uint32_t size;
    uint8_t ena;
    uint32_t reciveTime;
    
    uint32_t oldSize;        
}CodeHeap;

GPIO_TypeDef* getGpioPort(char* pin);
uint16_t getGpioPin(char* pin);
uint32_t getPinMode(char* mode);
uint8_t enableClk(char* pin);
void delay_us(uint32_t delay);
void delay_unit(uint32_t delay);
void STM32_UART_clearRxBuff(pika_uart_t* pika_uart);
uint8_t STM32_Code_reciveHandler(char *data, uint32_t rxSize);
void STM32_Code_Init(void);
void STM32_Code_flashHandler(void);
void HARDWARE_PRINTF_Init(void);
#endif
