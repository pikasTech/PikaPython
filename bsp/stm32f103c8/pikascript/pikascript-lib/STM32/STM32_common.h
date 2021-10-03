#ifndef __STM32__COMMON__H
#define __STM32__COMMON__H
#include "PikaObj.h"

#ifdef STM32G070xx
#include "stm32g0xx_hal.h"
#define UART1_EXIST
#define UART2_EXIST
#define UART3_EXIST
#define UART4_EXIST
#endif

#ifdef STM32F103xB
#include "stm32f1xx_hal.h"
#define UART1_EXIST
#define UART2_EXIST
#define UART3_EXIST

#define TIM1_EXIST
#define TIM2_EXIST
#define TIM3_EXIST
#define TIM4_EXIST
#endif

#ifdef STM32G030xx
#include "stm32g0xx_hal.h"
#define UART1_EXIST
#define UART2_EXIST

#define TIM1_EXIST
#define TIM3_EXIST
#define TIM14_EXIST
#define TIM16_EXIST
#define TIM17_EXIST
#define Code_ENABLE
#endif

#define RX_BUFF_LENGTH 64

#define FLASH_CODE_START_ADDR \
    (FLASH_BASE +             \
     ((FLASH_PAGE_NB - 1) * FLASH_PAGE_SIZE)) /* Start @ of user Flash area */
#define FLASH_USER_END_ADDR \
    (FLASH_BASE + FLASH_SIZE - 1) /* End @ of user Flash area */
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
void STM32_Code_Init();
void STM32_Code_flashHandler();
#endif