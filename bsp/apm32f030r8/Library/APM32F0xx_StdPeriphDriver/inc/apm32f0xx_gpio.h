/*!
 * @file        apm32f0xx_gpio.h
 *
 * @brief       This file contains all the functions prototypes for the GPIO firmware library
 *
 * @version     V1.0.1
 *
 * @date        2021-07-01
 *
 */

#ifndef __GPIO_H
#define __GPIO_H

#include "apm32f0xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup GPIO_Driver GPIO Driver
  @{
*/

/** @addtogroup GPIO_Enumerations Enumerations
  @{
*/

/**
 * @brief   Configuration Mode enumeration
 */
typedef enum
{
    GPIO_MODE_IN  = 0x00,         //!< GPIO Input Mode
    GPIO_MODE_OUT = 0x01,         //!< GPIO Output Mode
    GPIO_MODE_AF  = 0x02,         //!< GPIO Alternate function Mode
    GPIO_MODE_AN  = 0X03,         //!< GPIO Analog In/Out Mode
} GPIO_MODE_T;                    
                                  
/**                               
 * @brief   Output_type_enumeration
 */                               
typedef enum                      
{                                 
    GPIO_OUT_TYPE_PP = 0x00,      //!< General purpose output push-pull
    GPIO_OUT_TYPE_OD = 0x01,      //!< General purpose output Open-drain
} GPIO_OUT_TYPE_T;                
                                  
/**                               
 * @brief   GPIO Output Maximum frequency selection
 */                               
typedef enum                      
{                                 
    GPIO_SPEED_2MHz  = 0x00,      //!< Output speed up to 2 MHz
    GPIO_SPEED_10MHz = 0x01,      //!< Output speed up to 10 MHz
    GPIO_SPEED_50MHz = 0x03,      //!< Output speed up to 50 MHz
} GPIO_SPEED_T;                   
                                  
/**                               
 * @brief   Configuration_Pull-Up_Pull-Down_enumeration
 */                               
typedef enum                      
{                                 
    GPIO_PUPD_NO = 0x00,          
    GPIO_PUPD_PU = 0x01,          //!< Input with pull-up
    GPIO_PUPD_PD = 0x02,          //!< Input with pull-down
} GPIO_PUPD_T;

/**
 * @brief   Bit_SET_and_Bit_RESET_enumeration
 */
typedef enum
{
    Bit_RESET,
    Bit_SET
} GPIO_BSRET_T;

/**
 * @brief    Definition of the GPIO pins
 */
typedef enum
{
    GPIO_PIN_0  = ((uint16_t)BIT0),
    GPIO_PIN_1  = ((uint16_t)BIT1),
    GPIO_PIN_2  = ((uint16_t)BIT2),
    GPIO_PIN_3  = ((uint16_t)BIT3),
    GPIO_PIN_4  = ((uint16_t)BIT4),
    GPIO_PIN_5  = ((uint16_t)BIT5),
    GPIO_PIN_6  = ((uint16_t)BIT6),
    GPIO_PIN_7  = ((uint16_t)BIT7),
    GPIO_PIN_8  = ((uint16_t)BIT8),
    GPIO_PIN_9  = ((uint16_t)BIT9),
    GPIO_PIN_10 = ((uint16_t)BIT10),
    GPIO_PIN_11 = ((uint16_t)BIT11),
    GPIO_PIN_12 = ((uint16_t)BIT12),
    GPIO_PIN_13 = ((uint16_t)BIT13),
    GPIO_PIN_14 = ((uint16_t)BIT14),
    GPIO_PIN_15 = ((uint16_t)BIT15),
    GPIO_PIN_ALL    = ((uint32_t)0XFFFF),
} GPIO_PIN_T;

/**
 * @brief   gpio pin source define
 */
typedef enum
{
    GPIO_PIN_SOURCE_0   = ((uint8_t)0x00),
    GPIO_PIN_SOURCE_1   = ((uint8_t)0x01),
    GPIO_PIN_SOURCE_2   = ((uint8_t)0x02),
    GPIO_PIN_SOURCE_3   = ((uint8_t)0x03),
    GPIO_PIN_SOURCE_4   = ((uint8_t)0x04),
    GPIO_PIN_SOURCE_5   = ((uint8_t)0x05),
    GPIO_PIN_SOURCE_6   = ((uint8_t)0x06),
    GPIO_PIN_SOURCE_7   = ((uint8_t)0x07),
    GPIO_PIN_SOURCE_8   = ((uint8_t)0x08),
    GPIO_PIN_SOURCE_9   = ((uint8_t)0x09),
    GPIO_PIN_SOURCE_10  = ((uint8_t)0x0A),
    GPIO_PIN_SOURCE_11  = ((uint8_t)0x0B),
    GPIO_PIN_SOURCE_12  = ((uint8_t)0x0C),
    GPIO_PIN_SOURCE_13  = ((uint8_t)0x0D),
    GPIO_PIN_SOURCE_14  = ((uint8_t)0x0E),
    GPIO_PIN_SOURCE_15  = ((uint8_t)0x0F),
} GPIO_PIN_SOURCE_T;

/**
 * @brief   gpio alternate_function define
 */
typedef enum
{
    GPIO_AF_PIN0 = ((uint8_t)0x00),
    GPIO_AF_PIN1 = ((uint8_t)0x01),
    GPIO_AF_PIN2 = ((uint8_t)0x02),
    GPIO_AF_PIN3 = ((uint8_t)0x03),
    GPIO_AF_PIN4 = ((uint8_t)0x04),
    GPIO_AF_PIN5 = ((uint8_t)0x05),
    GPIO_AF_PIN6 = ((uint8_t)0x06),
    GPIO_AF_PIN7 = ((uint8_t)0x07),
} GPIO_AF_T;

/**@} end of group GPIO_Enumerations*/


/** @addtogroup GPIO_Structure Data Structure
  @{
*/

/**
 * @brief   GPIO Config structure definition
 */
typedef struct
{
    uint16_t         pin;
    GPIO_MODE_T      mode;
    GPIO_OUT_TYPE_T  outtype;
    GPIO_SPEED_T     speed;
    GPIO_PUPD_T      pupd;
} GPIO_Config_T;

/**@} end of group GPIO_Structure*/


/** @addtogroup GPIO_Fuctions Fuctions
  @{
*/

/** Reset and Configuration */
void GPIO_Reset(GPIO_T* port);
void GPIO_Config(GPIO_T* port, GPIO_Config_T* gpioConfig);
void GPIO_ConfigStructInit(GPIO_Config_T* gpioConfig);

/** Lock */
void GPIO_ConfigPinLock(GPIO_T* port, uint16_t pin);

/** Read */
uint16_t GPIO_ReadOutputPort(GPIO_T* port);
uint16_t GPIO_ReadInputPort(GPIO_T* port);
uint8_t GPIO_ReadInputBit(GPIO_T* port, uint16_t pin);
uint8_t GPIO_ReadOutputBit(GPIO_T* port, uint16_t pin);

/** Write */
void GPIO_SetBit(GPIO_T* port, uint16_t pin);
void GPIO_ClearBit(GPIO_T* port, uint16_t pin);
void GPIO_WriteBitValue(GPIO_T* port, uint16_t pin, GPIO_BSRET_T bitVal);
void GPIO_WriteOutputPort(GPIO_T* port, uint16_t portValue);

/** Alternate function */
void GPIO_ConfigPinAF(GPIO_T* port, GPIO_PIN_SOURCE_T pinSource, GPIO_AF_T afPin);

/**@} end of group GPIO_Fuctions*/
/**@} end of group GPIO_Driver */
/**@} end of group Peripherals_Library*/

#ifdef __cplusplus
}
#endif

#endif /* __GPIO_H */
