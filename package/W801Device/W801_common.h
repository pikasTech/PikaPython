#ifndef __W801__COMMON__H
#define __W801__COMMON__H
#include "PikaObj.h"
//#include "main.h"
#include "wm_hal.h"
#include <stdint.h>

/* support std lib for w801 */
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
#define GPIO_Pin_16 GPIO_PIN_16 
#define GPIO_Pin_17 GPIO_PIN_17
#define GPIO_Pin_18 GPIO_PIN_18
#define GPIO_Pin_19 GPIO_PIN_19
#define GPIO_Pin_20 GPIO_PIN_20
#define GPIO_Pin_21 GPIO_PIN_21
#define GPIO_Pin_22 GPIO_PIN_22
#define GPIO_Pin_23 GPIO_PIN_23
#define GPIO_Pin_24 GPIO_PIN_24
#define GPIO_Pin_25 GPIO_PIN_25
#define GPIO_Pin_26 GPIO_PIN_26 
#define GPIO_Pin_27 GPIO_PIN_27
#define GPIO_Pin_28 GPIO_PIN_28
#define GPIO_Pin_29 GPIO_PIN_29
#define GPIO_Pin_30 GPIO_PIN_30
#define GPIO_Pin_31 GPIO_PIN_31


typedef struct pika_IIC_info_t {
    GPIO_TypeDef* SCL_GPIO;
    GPIO_TypeDef* SDA_GPIO;

    uint32_t SCL_GPIO_Pin;
    uint32_t SDA_GPIO_Pin;
    uint8_t deviceAddr;

    uint8_t readBuff[32];
} pika_IIC_info;

GPIO_TypeDef* GPIO_get_Group(char* pin);
uint32_t GPIO_get_pin(char* pin);
uint32_t getPinMode(char* mode);
uint8_t GPIO_enable_clock(char* pin);
//void delay_us(uint32_t delay);
//void delay_unit(uint32_t delay);

#endif
