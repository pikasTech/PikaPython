/*!
 * @file        apm32f0xx_comp.h
 *
 * @brief       This file contains all the functions prototypes for the COMP firmware library
 *
 * @version     V1.0.1
 *
 * @date        2021-07-01
 *
 */

#ifndef __COMP_H
#define __COMP_H

#include "apm32f0xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup COMP_Driver COMP Driver
  @{
*/

/** @addtogroup COMP_Enumerations Enumerations
  @{
*/
/**
 * @brief    COMP_Selection 
 */
typedef enum
{
    COMP_SELECT_COMP1 =  ((uint32_t)0x00000000), //!< COMP1
    COMP_SELECT_COMP2 =  ((uint32_t)0x00000010)  //!< COMP2
}COMP_SELECT_T;

/**
 * @brief    COMP_InvertingInput
 */

typedef enum
{
    COMP_INVERTING_INPUT_1_4VREFINT = 0x00, //!< 1/4 of VREFINT
    COMP_INVERTING_INPUT_1_2VREFINT = 0x01, //!< 1/2 of VREFINT
    COMP_INVERTING_INPUT_3_4VREFINT = 0x02, //!< 3/4 of VREFINT
    COMP_INVERTING_INPUT_VREFINT    = 0x03, //!< VREFINT
    COMP_INVERTING_INPUT_DAC1       = 0x04, //!< COMP1_INM4 (PA4 with DAC_OUT1 if enabled)
    COMP_INVERTING_INPUT_DAC2       = 0x05, //!< COMP1_INM5 (PA5 with DAC_OUT2 if present and enabled, only for APM32F072 and APM32F091 devices)
    COMP_INVERTING_INPUT_IO         = 0x06, //!< COMP1_INM6 (PA0)
}COMP_INVERTING_INPUT_T;
    
/**
 * @brief    COMP_Output
 */
typedef enum
{
    COMP_OUTPUT_NONE         = 0x00, //!< no selection
    COMP_OUTPUT_TIM1BKIN     = 0x01, //!< Timer 1 break input
    COMP_OUTPUT_TIM1IC1      = 0x02, //!< Timer 1 Input capture 1
    COMP_OUTPUT_TIM1OCREFCLR = 0x03, //!< Timer 1 OCrefclear input
    COMP_OUTPUT_TIM2IC4      = 0x04, //!< Timer 2 input capture 4
    COMP_OUTPUT_TIM2OCREFCLR = 0x05, //!< Timer 2 OCrefclear input
    COMP_OUTPUT_TIM3IC1      = 0x06, //!< Timer 3 input capture 1
    COMP_OUTPUT_TIM3OCREFCLR = 0x07, //!< Timer 3 OCrefclear input
}COMP_OUTPUT_T;

/**
 * @brief    COMP_OutputPolarity
 */
typedef enum
{
    COMP_OUTPUTPOL_NONINVERTED, //!< output is not inverted
    COMP_OUTPUTPOL_INVERTED     //!< output is inverted
}COMP_OUTPUTPOL_T;

/**
 * @brief    COMP_Hysteresis
 */
typedef enum
{
    COMP_HYSTERRSIS_NO     = 0x00, //!< No hysteresis
    COMP_HYSTERRSIS_LOW    = 0x01, //!< Low hysteresis
    COMP_HYSTERRSIS_MEDIUM = 0x02, //!< Medium hysteresis
    COMP_HYSTERRSIS_HIGH   = 0x03, //!< High hysteresis
}COMP_HYSTERRSIS_T;

/**
 * @brief    COMP_Mode
 */
typedef enum
{
    COMP_MODE_HIGHSPEED    = 0x00, //!< High speed / full power
    COMP_MODE_MEDIUMSPEED  = 0x01, //!< Medium speed / medium power
    COMP_MODE_LOWPOWER     = 0x02, //!< Low speed / low-power
    COMP_MODE_VERYLOW      = 0x03  //!< Very-low speed / ultra-low power
}COMP_MODE_T;

/**@} end of group COMP_Enumerations*/


/** @addtogroup COMP_Macros Macros
  @{
*/

/** Macros description */
#define COMP_CSTS_COMP1OUT     ((uint32_t)0x00004000)
#define COMP_CSTS_COMP2OUT     ((uint32_t)0x40000000)
#define COMP_OUTPUTLEVEL_HIGH  ((uint32_t)0x00004000)
#define COMP_OUTPUTLEVEL_LOW   ((uint32_t)0x00000000)

/**@} end of group COMP_Macros*/


/** @addtogroup COMP_Structure Data Structure
  @{
*/

/**
 * @brief    OMP Config structure definition
 */
typedef struct
{
    COMP_INVERTING_INPUT_T invertingInput; //!< Comparator inverting input selection
    COMP_OUTPUT_T      output;             //!< Comparator output selection
    COMP_OUTPUTPOL_T   outputPol;          //!< Comparator output polarity
    COMP_HYSTERRSIS_T  hysterrsis;         //!< Comparator hysteresis
    COMP_MODE_T        mode;               //!< Comparator mode
}COMP_Config_T;

/**@} end of group COMP_Structure*/


/** @addtogroup COMP_Fuctions Fuctions
  @{
*/

/** COMP configuration **/
void COMP_Reset(void);
void COMP_Config(COMP_SELECT_T compSelect, COMP_Config_T* compConfig);
void COMP_ConfigStructInit(COMP_Config_T* compConfig);
void COMP_Enable(COMP_SELECT_T compSelect);
void COMP_Disable(COMP_SELECT_T compSelect);
void COMP_EnableSwitch(void);
void COMP_DisableSwitch(void);
uint32_t COMP_ReadOutPutLevel(COMP_SELECT_T compSelect);

/** Window mode control **/
void COMP_EnableWindow(void);
void COMP_DisnableWindow(void);

/** COMP configuration locking **/
void COMP_ConfigLOCK(COMP_SELECT_T compSelect);

/**@} end of group COMP_Fuctions*/
/**@} end of group COMP_Driver */
/**@} end of group Peripherals_Library*/

#ifdef __cplusplus
}
#endif

#endif /* __COMP_H */
