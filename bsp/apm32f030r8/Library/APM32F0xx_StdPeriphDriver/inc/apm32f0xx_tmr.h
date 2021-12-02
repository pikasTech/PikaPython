/*!
 * @file        apm32f0xx_tmr.h
 *
 * @brief       This file contains all functions prototype and macros for the TMR peripheral
 *
 * @version     V1.0.1
 *
 * @date        2021-07-01
 *
 */

#ifndef __TMR_H
#define __TMR_H

#include "apm32f0xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup TMR_Driver  TMR Driver
  @{
*/

/** @addtogroup  TMR_Enumerations Enumerations
  @{
*/

/**
 * @brief   Counter_Mode
 */
typedef enum
{
    TMR_COUNTER_MODE_UP = 0,
    TMR_COUNTER_MODE_DOWN = 1,
    TMR_COUNTER_MODE_CENTERALIGNED1 = 2,
    TMR_COUNTER_MODE_CENTERALIGNED2 = 4,
    TMR_COUNTER_MODE_CENTERALIGNED3 = 6
} TMR_COUNTER_MODE_T;

/**
 * @brief   Clock_Division_CKD
 */
typedef enum
{
    TMR_CKD_DIV1 = 0,
    TMR_CKD_DIV2 = 1,
    TMR_CKD_DIV4 = 2
} TMR_CKD_T;

/**
 * @brief   Prescaler_Reload_Mode
 */
typedef enum
{
    TMR_PRESCALER_RELOAD_UPDATA = 0,
    TMR_PRESCALER_RELOAD_IMMEDIATE = 1
} TMR_PRESCALER_RELOAD_T;

/**
 * @brief    TMR UpdateSource
 */
typedef enum
{
    TMR_UPDATE_SOURCE_GLOBAL = 0,
    TMR_UPDATE_SOURCE_REGULAR = 1
} TMR_UPDATE_SOURCE_T;

/**
 * @brief    TMR OPMode
 */
typedef enum
{
    TMR_OPMODE_REPETITIVE = 0,
    TMR_OPMODE_SINGLE = 1
} TMR_OPMODE_T;

/**
 * @brief TMR Specifies the Off-State selection used in Run mode
 */
typedef enum
{
    TMR_RMOS_STATE_DISABLE = 0,
    TMR_RMOS_STATE_ENABLE = 1
} TMR_RMOS_STATE_T;

/**
 * @brief TMR Closed state configuration in idle mode
 */
typedef enum
{
    TMR_IMOS_STATE_DISABLE = 0,
    TMR_IMOS_STATE_ENABLE = 1
} TMR_IMOS_STATE_T;

/**
 * @brief TMR Protect mode configuration values
 */
typedef enum
{
    TMR_LOCK_LEVEL_OFF = 0,
    TMR_LOCK_LEVEL_1 = 1,
    TMR_LOCK_LEVEL_2 = 2,
    TMR_LOCK_LEVEL_3 = 3
} TMR_LOCK_LEVEL_T;

/**
 * @brief TMR break state
 */
typedef enum
{
    TMR_BREAK_STATE_DISABLE,
    TMR_BREAK_STATE_ENABLE
} TMR_BREAK_STATE_T;

/**
 * @brief TMR Specifies the Break Input pin polarity.
 */
typedef enum
{
    TMR_BREAK_POLARITY_LOW,
    TMR_BREAK_POLARITY_HIGH
} TMR_BREAK_POLARITY_T;

/**
 * @brief TMR Automatic Output feature is enable or disable 
 */
typedef enum
{
    TMR_AUTOMATIC_OUTPUT_DISABLE,
    TMR_AUTOMATIC_OUTPUT_ENABLE
} TMR_AUTOMATIC_OUTPUT_T;

/**
 * @brief TMR_Output_Compare_and_PWM_modes
 */
typedef enum
{
    TMR_OC_MODE_TMRING     = 0x00,
    TMR_OC_MODE_ACTIVE     = 0x01,
    TMR_OC_MODE_INACTIVE   = 0x02,
    TMR_OC_MODE_TOGGEL     = 0x03,
    TMR_OC_MODE_LOWLEVEL   = 0x04,
    TMR_OC_MODE_HIGHLEVEL  = 0x05,
    TMR_OC_MODE_PWM1       = 0x06,
    TMR_OC_MODE_PWM2       = 0x07
} TMR_OC_MODE_T;

/**
 * @brief TMR_Output_Compare_state
 */
typedef enum
{
    TMR_OUTPUT_STATE_DISABLE,
    TMR_OUTPUT_STATE_ENABLE
} TMR_OC_OUTPUT_STATE_T;

/**
 * @brief TMR_Output_Compare_N_state
 */
typedef enum
{
    TMR_OUTPUT_NSTATE_DISABLE,
    TMR_OUTPUT_NSTATE_ENABLE
} TMR_OC_OUTPUT_NSTATE_T;

/**
 * @brief TMR_Output_Compare_Polarity
 */
typedef enum
{
    TMR_OC_POLARITY_HIGH,
    TMR_OC_POLARITY_LOW
} TMR_OC_POLARITY_T;

/**
 * @brief TMR_Output_Compare_N_Polarity
 */
typedef enum
{
    TMR_OC_NPOLARITY_HIGH,
    TMR_OC_NPOLARITY_LOW
} TMR_OC_NPOLARITY_T;

/**
 * @brief TMR_Output_Compare_Idle_State
 */
typedef enum
{
    TMR_OCIDLESTATE_RESET,
    TMR_OCIDLESTATE_SET
} TMR_OC_IDLE_STATE_T;

/**
 * @brief TMR_Output_Compare_N_Idle_State
 */
typedef enum
{
    TMR_OCNIDLESTATE_RESET,
    TMR_OCNIDLESTATE_SET
} TMR_OC_NIDLE_STATE_T;

/**
 * @brief TMR Input Capture Init structure definition
 */
typedef enum
{
    TMR_CHANNEL_1 = 0x0000,
    TMR_CHANNEL_2 = 0x0004,
    TMR_CHANNEL_3 = 0x0008,
    TMR_CHANNEL_4 = 0x000C
} TMR_CHANNEL_T;

/**
 * @brief    TMR ForcedAction
 */
typedef enum
{
    TMR_FORCEDACTION_INACTIVE = 0x04,
    TMR_FORCEDACTION_ACTIVE   = 0x05
} TMR_FORCED_ACTION_T;

/**
 * @brief    TMR Output_Compare_Preload_State
 */
typedef enum
{
    TMR_OC_PRELOAD_DISABLE,
    TMR_OC_PRELOAD_ENABLE
} TMR_OC_PRELOAD_T;

/**
 * @brief    TMR Output_Compare_Fast_State
 */
typedef enum
{
    TMR_OCFAST_DISABLE,
    TMR_OCFAST_ENABLE
} TMR_OCFAST_T;

/**
 * @brief    TMR Output_Compare_Clear_State 
 */
typedef enum
{
    TMR_OCCLER_DISABLE,
    TMR_OCCLER_ENABLE
} TMR_OCCLER_T;

/**
 * @brief    TMR_OCReferenceClear Clear source
 */
typedef enum
{
    TMR_OCCS_ETRF,
    TMR_OCCS_OCREFCLR
} TMR_OCCSEL_T;

/**
 * @brief TMR Input_Capture_Polarity
 */
typedef enum
{
    TMR_IC_POLARITY_RISING   = 0x00,
    TMR_IC_POLARITY_FALLING  = 0x02,
    TMR_IC_POLARITY_BOTHEDGE = 0x0A
} TMR_IC_POLARITY_T;

/**
 * @brief TMR Input_Capture_Selection
 */
typedef enum
{
    TMR_IC_SELECTION_DIRECT_TI   = 0x01,
    TMR_IC_SELECTION_INDIRECT_TI = 0x02,
    TMR_IC_SELECTION_TRC         = 0x03
} TMR_IC_SELECTION_T;

/**
 * @brief TMR_Input_Capture_Prescaler 
 */
typedef enum
{
    TMR_ICPSC_DIV1 = 0x00,
    TMR_ICPSC_DIV2 = 0x01,
    TMR_ICPSC_DIV4 = 0x02,
    TMR_ICPSC_DIV8 = 0x03
} TMR_IC_PRESCALER_T;

/**
 * @brief    TMR_interrupt_sources
 */
typedef enum
{
    TMR_INT_UPDATE = 0x0001,
    TMR_INT_CH1    = 0x0002,
    TMR_INT_CH2    = 0x0004,
    TMR_INT_CH3    = 0x0008,
    TMR_INT_CH4    = 0x0010,
    TMR_INT_CCU    = 0x0020,
    TMR_INT_TRG    = 0x0040,
    TMR_INT_BRK    = 0x0080
} TMR_INT_T;

/**
 * @brief    TMR_event_sources
 */
typedef enum
{
    TMR_EVENT_UPDATE = 0x0001,
    TMR_EVENT_CH1    = 0x0002,
    TMR_EVENT_CH2    = 0x0004,
    TMR_EVENT_CH3    = 0x0008,
    TMR_EVENT_CH4    = 0x0010,
    TMR_EVENT_CCU    = 0x0020,
    TMR_EVENT_TRG    = 0x0040,
    TMR_EVENT_BRK    = 0x0080
} TMR_EVENT_T;

/**
 * @brief    TMR_interrupt_flag
 */
typedef enum
{
    TMR_INT_FLAG_UPDATE = 0x0001,
    TMR_INT_FLAG_CH1    = 0x0002,
    TMR_INT_FLAG_CH2    = 0x0004,
    TMR_INT_FLAG_CH3    = 0x0008,
    TMR_INT_FLAG_CH4    = 0x0010,
    TMR_INT_FLAG_CCU    = 0x0020,
    TMR_INT_FLAG_TRG    = 0x0040,
    TMR_INT_FLAG_BRK    = 0x0080
} TMR_INT_FLAG_T;

/**
 * @brief    TMR Flag
 */
typedef enum
{
    TMR_FLAG_UPDATE  = 0x0001,
    TMR_FLAG_CH1     = 0x0002,
    TMR_FLAG_CH2     = 0x0004,
    TMR_FLAG_CH3     = 0x0008,
    TMR_FLAG_CH4     = 0x0010,
    TMR_FLAG_CCU     = 0x0020,
    TMR_FLAG_TRG     = 0x0040,
    TMR_FLAG_BRK     = 0x0080,
    TMR_FLAG_CH1OC   = 0x0200,
    TMR_FLAG_CH2OC   = 0x0400,
    TMR_FLAG_CH3OC   = 0x0800,
    TMR_FLAG_CH4OC   = 0x1000
} TMR_FLAG_T;

/**
 * @brief    TMR DMA Base Address
 */
typedef enum
{
    TMR_DMABASE_CTRL1   = 0x0000,
    TMR_DMABASE_CTRL2   = 0x0001,
    TMR_DMABASE_SMCTRL  = 0x0002,
    TMR_DMABASE_DIEN    = 0x0003,
    TMR_DMABASE_STS     = 0x0004,
    TMR_DMABASE_SCEG    = 0x0005,
    TMR_DMABASE_CCM1    = 0x0006,
    TMR_DMABASE_CCM2    = 0x0007,
    TMR_DMABASE_CHCTRL  = 0x0008,
    TMR_DMABASE_CNT     = 0x0009,
    TMR_DMABASE_DIV     = 0x000A,
    TMR_DMABASE_AUTORLD = 0x000B,
    TMR_DMABASE_REPCNT  = 0x000C,
    TMR_DMABASE_CH1CC   = 0x000D,
    TMR_DMABASE_CH2CC   = 0x000E,
    TMR_DMABASE_CH3CC   = 0x000F,
    TMR_DMABASE_CH4CC   = 0x0010,
    TMR_DMABASE_BDT     = 0x0011,
    TMR_DMABASE_DMAB    = 0x0012
} TMR_DMA_BASE_ADDERSS_T;

/**
 * @brief    TMR DMA Burst Lenght
 */
typedef enum
{
    TMR_DMA_BURSTLENGHT_1TRANSFER   = 0x0000,
    TMR_DMA_BURSTLENGHT_2TRANSFERS  = 0x0100,
    TMR_DMA_BURSTLENGHT_3TRANSFERS  = 0x0200,
    TMR_DMA_BURSTLENGHT_4TRANSFERS  = 0x0300,
    TMR_DMA_BURSTLENGHT_5TRANSFERS  = 0x0400,
    TMR_DMA_BURSTLENGHT_6TRANSFERS  = 0x0500,
    TMR_DMA_BURSTLENGHT_7TRANSFERS  = 0x0600,
    TMR_DMA_BURSTLENGHT_8TRANSFERS  = 0x0700,
    TMR_DMA_BURSTLENGHT_9TRANSFERS  = 0x0800,
    TMR_DMA_BURSTLENGHT_10TRANSFERS = 0x0900,
    TMR_DMA_BURSTLENGHT_11TRANSFERS = 0x0A00,
    TMR_DMA_BURSTLENGHT_12TRANSFERS = 0x0B00,
    TMR_DMA_BURSTLENGHT_13TRANSFERS = 0x0C00,
    TMR_DMA_BURSTLENGHT_14TRANSFERS = 0x0D00,
    TMR_DMA_BURSTLENGHT_15TRANSFERS = 0x0E00,
    TMR_DMA_BURSTLENGHT_16TRANSFERS = 0x0F00,
    TMR_DMA_BURSTLENGHT_17TRANSFERS = 0x1000,
    TMR_DMA_BURSTLENGHT_18TRANSFERS = 0x1100,
} TMR_DMA_BURST_LENGHT_T;

/**
 * @brief    TMR DMA Soueces
 */
typedef enum
{
    TMR_DMA_UPDATE    = 0x0100,
    TMR_DMA_CH1       = 0x0200,
    TMR_DMA_CH2       = 0x0400,
    TMR_DMA_CH3       = 0x0800,
    TMR_DMA_CH4       = 0x1000,
    TMR_DMA_CCU       = 0x2000,
    TMR_DMA_TRG       = 0x4000
} TMR_DMA_SOUCES_T;

/**
 * @brief    TMR Internal_Trigger_Selection
 */
typedef enum
{
    TMR_TS_ITR0     = 0x00,
    TMR_TS_ITR1     = 0x01,
    TMR_TS_ITR2     = 0x02,
    TMR_TS_ITR3     = 0x03,
    TMR_TS_TI1F_ED  = 0x04,
    TMR_TS_TI1FP1   = 0x05,
    TMR_TS_TI2FP2   = 0x06,
    TMR_TS_ETRF     = 0x07
} TMR_INPUT_TRIGGER_SOURCE_T;

/**
 * @brief    TMR  The external Trigger Prescaler.
 */
typedef enum
{
    TMR_ExtTRGPSC_OFF   = 0x00,
    TMR_EXTTRGPSC_DIV2  = 0x01,
    TMR_EXTTRGPSC_DIV4  = 0x02,
    TMR_EXTTRGPSC_DIV8  = 0x03
} TMR_EXTTRG_PRESCALER_T;

/**
 * @brief    TMR External_Trigger_Polarity
 */
typedef enum
{
    TMR_EXTTRGPOLARITY_INVERTED      = 0x01,
    TMR_EXTTGRPOLARITY_NONINVERTED   = 0x00
} TMR_EXTTRG_POLARITY_T;

/**
 * @brief    TMR OPMode
 */
typedef enum
{
    TMR_TRGOSOURCE_RESET,
    TMR_TRGOSOURCE_ENABLE,
    TMR_TRGOSOURCE_UPDATE,
    TMR_TRGOSOURCE_OC1,
    TMR_TRGOSOURCE_OC1REF,
    TMR_TRGOSOURCE_OC2REF,
    TMR_TRGOSOURCE_OC3REF,
    TMR_TRGOSOURCE_OC4REF
} TMR_TRGOSOURCE_T;

/**
 * @brief    TMR OPMode
 */
typedef enum
{
    TMR_SLAVEMODE_RESET     = 0x04,
    TMR_SLAVEMODE_GATED     = 0x05,
    TMR_SLAVEMODE_TRIGGER   = 0x06,
    TMR_SLAVEMODE_EXTERNALL = 0x07
} TMR_SLAVEMODE_T;

/**
 * @brief    TMR Encoder_Mode
 */
typedef enum
{
    TMR_ENCODER_MODE_TI1      = 0x01,
    TMR_ENCODER_MODE_TI2      = 0x02,
    TMR_ENCODER_MODE_TI12     = 0x03
} TMR_ENCODER_MODE_T;

/**
 * @brief    TMR Remap Select
 */
typedef enum
{
    TMR_REMAP_GPIO      = 0x00,
    TMR_REMAP_RTC_CLK   = 0x01,
    TMR_REMAP_HSEDiv32  = 0x02, 
    TMR_REMAP_MCO       = 0x03
} TMR_REMAP_T;

/**@} end of group TMR_Enumerations*/


/** @addtogroup TMR_Structure Data Structure
  @{
*/

/**
  * @brief  TMR Time Base Init structure definition
  * @note   This sturcture is used with all TMRx.
  */
typedef struct
{
    uint16_t              div;
    TMR_COUNTER_MODE_T    counterMode;
    uint32_t              period;
    TMR_CKD_T             clockDivision;
    uint8_t               repetitionCounter;
} TMR_TimeBase_T;

/**
 * @brief    TMR BDT structure definition
 */
typedef struct
{
    TMR_RMOS_STATE_T        RMOS_State;
    TMR_IMOS_STATE_T        IMOS_State;
    TMR_LOCK_LEVEL_T        lockLevel;
    uint8_t                 deadTime;
    TMR_BREAK_STATE_T       breakState;
    TMR_BREAK_POLARITY_T    breakPolarity;
    TMR_AUTOMATIC_OUTPUT_T  automaticOutput;
} TMR_BDTInit_T;
/**
 * @brief    TMR Config struct definition
 */
typedef struct
{
    TMR_OC_MODE_T           OC_Mode;            //!< Specifies the TMR mode.

    TMR_OC_OUTPUT_STATE_T   OC_OutputState;     //!< Specifies the TMR Output Compare state.

    TMR_OC_OUTPUT_NSTATE_T  OC_OutputNState;    //!< Specifies the TMR complementary Output Compare state.  @note This parameter is valid only for TMR1 and TMR8.

    TMR_OC_POLARITY_T       OC_Polarity;        //!<  Specifies the output polarity.

    TMR_OC_NPOLARITY_T      OC_NPolarity;       //!<  Specifies the complementary output polarity.  @note This parameter is valid only for TMR1 and TMR8.

    TMR_OC_IDLE_STATE_T     OC_Idlestate;       //!<  Specifies the TMR Output Compare pin state during Idle state. @note This parameter is valid only for TMR1 and TMR8.

    TMR_OC_NIDLE_STATE_T    OC_NIdlestate;      //!<  Specifies the TMR Output Compare pin state during Idle state. @note This parameter is valid only for TMR1 and TMR8.

    uint16_t              Pulse;                //!< Specifies the pulse value to be loaded into the Capture Compare Register.

} TMR_OCConfig_T;

/**
 * @brief    TMR Input Capture Config struct definition
 */
typedef struct
{
    TMR_CHANNEL_T channel;          //!<  Specifies the TMR channel.

    TMR_IC_POLARITY_T ICpolarity;   //!< Specifies the active edge of the input signal.

    TMR_IC_SELECTION_T ICselection; //!<  Specifies the input.

    TMR_IC_PRESCALER_T ICprescaler; //!<  Specifies the Input Capture Prescaler.

    uint16_t  ICfilter;             //!< Specifies the input capture filter.

} TMR_ICConfig_T;

/**@} end of group TMR_Structure*/


/** @addtogroup  TMR_Fuctions Fuctions
  @{
*/
/** TimeBase management */
void TMR_Reset(TMR_T* TMRx);
void TMR_ConfigTimeBase(TMR_T* TMRx, TMR_TimeBase_T* timeBaseConfig);
void TMR_ConfigTimeBaseStruct(TMR_TimeBase_T* timeBaseConfig);
void TMR_ConfigDIV(TMR_T* TMRx, uint16_t div, TMR_PRESCALER_RELOAD_T mode);
void TMR_ConfigCounterMode(TMR_T* TMRx, TMR_COUNTER_MODE_T mode);
void TMR_SetCounter(TMR_T* TMRx, uint32_t counter);
void TMR_SetAutoReload(TMR_T* TMRx, uint32_t autoReload);
uint32_t TMR_ReadCounter(TMR_T* TMRx);
uint32_t TMR_ReadDiv(TMR_T* TMRx);
void TMR_EnableNGUpdate(TMR_T* TMRx);
void TMR_DisableNGUpdate(TMR_T* TMRx);
void TMR_ConfigUPdateRequest(TMR_T* TMRx, TMR_UPDATE_SOURCE_T source);
void TMR_EnableAUTOReload(TMR_T* TMRx);
void TMR_DisableAUTOReload(TMR_T* TMRx);
void TMR_SetClockDivision(TMR_T* TMRx, TMR_CKD_T clockDivision);
void TMR_Enable(TMR_T* TMRx);
void TMR_Disable(TMR_T* TMRx);
void TMR_ConfigBDT(TMR_T* TMRx, TMR_BDTInit_T* structure);
void TMR_ConfigBDTStructInit( TMR_BDTInit_T* structure);

void TMR_EnablePWMOutputs(TMR_T* TMRx);
void TMR_DisablePWMOutputs(TMR_T* TMRx);

void TMR_OC1Config(TMR_T* TMRx, TMR_OCConfig_T* OCcongigStruct);
void TMR_OC2Config(TMR_T* TMRx, TMR_OCConfig_T* OCcongigStruct);
void TMR_OC3Config(TMR_T* TMRx, TMR_OCConfig_T* OCcongigStruct);
void TMR_OC4Config(TMR_T* TMRx, TMR_OCConfig_T* OCcongigStruct);
void TMR_OCConfigStructInit(TMR_OCConfig_T* OCcongigStruct);

void TMR_SelectOCxMode(TMR_T* TMRx, TMR_CHANNEL_T channel, TMR_OC_MODE_T mode);
void TMR_SelectSlaveMode(TMR_T* TMRx, TMR_SLAVEMODE_T mode);
void TMR_SelectOnePulseMode(TMR_T* TMRx, TMR_OPMODE_T OPMode);

void TMR_SetCompare1(TMR_T* TMRx, uint32_t compare);
void TMR_SetCompare2(TMR_T* TMRx, uint32_t compare);
void TMR_SetCompare3(TMR_T* TMRx, uint32_t compare);
void TMR_SetCompare4(TMR_T* TMRx, uint32_t compare);

void TMR_ForcedOC1Config(TMR_T* TMRx, TMR_FORCED_ACTION_T action);
void TMR_ForcedOC2Config(TMR_T* TMRx, TMR_FORCED_ACTION_T action);
void TMR_ForcedOC3Config(TMR_T* TMRx, TMR_FORCED_ACTION_T action);
void TMR_ForcedOC4Config(TMR_T* TMRx, TMR_FORCED_ACTION_T action);

void TMR_EnableCCPreload(TMR_T* TMRx);
void TMR_DisableCCPreload(TMR_T* TMRx);

void TMR_OC1PreloadConfig(TMR_T* TMRx, TMR_OC_PRELOAD_T OCPreload);
void TMR_OC2PreloadConfig(TMR_T* TMRx, TMR_OC_PRELOAD_T OCPreload);
void TMR_OC3PreloadConfig(TMR_T* TMRx, TMR_OC_PRELOAD_T OCPreload);
void TMR_OC4PreloadConfig(TMR_T* TMRx, TMR_OC_PRELOAD_T OCPreload);

void TMR_OC1FastConfit(TMR_T* TMRx, TMR_OCFAST_T OCFast);
void TMR_OC2FastConfit(TMR_T* TMRx, TMR_OCFAST_T OCFast);
void TMR_OC3FastConfit(TMR_T* TMRx, TMR_OCFAST_T OCFast);
void TMR_OC4FastConfit(TMR_T* TMRx, TMR_OCFAST_T OCFast);

void TMR_OC1PolarityConfig(TMR_T* TMRx, TMR_OC_POLARITY_T OCPolarity);
void TMR_OC1NPolarityConfig(TMR_T* TMRx, TMR_OC_NPOLARITY_T OCNPolarity);
void TMR_OC2PolarityConfig(TMR_T* TMRx, TMR_OC_POLARITY_T OCPolarity);
void TMR_OC2NPolarityConfig(TMR_T* TMRx, TMR_OC_NPOLARITY_T OCNPolarity);
void TMR_OC3PolarityConfig(TMR_T* TMRx, TMR_OC_POLARITY_T OCPolarity);
void TMR_OC3NPolarityConfig(TMR_T* TMRx, TMR_OC_NPOLARITY_T OCNPolarity);
void TMR_OC4PolarityConfig(TMR_T* TMRx, TMR_OC_POLARITY_T OCPolarity);

void TMR_SelectOCREFClear(TMR_T* TMRx, TMR_OCCSEL_T OCReferenceClear);

void TMR_EnableCCxChannel(TMR_T* TMRx, TMR_CHANNEL_T channel);
void TMR_DisableCCxChannel(TMR_T* TMRx, TMR_CHANNEL_T channel);
void TMR_EnableCCxNChannel(TMR_T* TMRx, TMR_CHANNEL_T channel);
void TMR_DisableCCxNChannel(TMR_T* TMRx, TMR_CHANNEL_T channel);

void TMR_EnableAUTOReload(TMR_T* TMRx);
void TMR_DisableAUTOReload(TMR_T* TMRx);
void TMR_EnableSelectCOM(TMR_T* TMRx);
void TMR_DisableSelectCOM(TMR_T* TMRx);

void TMR_ICConfig(TMR_T* TMRx, TMR_ICConfig_T* ICconfigstruct);
void TMR_ICConfigStructInit(TMR_ICConfig_T* ICconfigstruct);

void TMR_PWMConfig(TMR_T* TMRx, TMR_ICConfig_T* ICconfigstruct);

uint16_t TMR_ReadCaputer1(TMR_T* TMRx);
uint16_t TMR_ReadCaputer2(TMR_T* TMRx);
uint16_t TMR_ReadCaputer3(TMR_T* TMRx);
uint16_t TMR_ReadCaputer4(TMR_T* TMRx);

void TMR_SetIC1Prescal(TMR_T* TMRx, TMR_IC_PRESCALER_T prescaler);
void TMR_SetIC2Prescal(TMR_T* TMRx, TMR_IC_PRESCALER_T prescaler);
void TMR_SetIC3Prescal(TMR_T* TMRx, TMR_IC_PRESCALER_T prescaler);
void TMR_SetIC4Prescal(TMR_T* TMRx, TMR_IC_PRESCALER_T prescaler);

/** Interrupts and Event management functions */
void TMR_EnableInterrupt(TMR_T* TMRx, uint16_t interrupt);
void TMR_DisableInterrupt(TMR_T* TMRx, uint16_t interrupt);
void TMR_GenerateEvent(TMR_T* TMRx, uint16_t event);

uint16_t TMR_ReadStatusFlag(TMR_T* TMRx, TMR_FLAG_T flag);
void TMR_ClearStatusFlag(TMR_T* TMRx, uint16_t flag);
uint16_t TMR_ReadIntFlag(TMR_T* TMRx,  TMR_INT_FLAG_T flag);
void TMR_ClearIntFlag(TMR_T* TMRx,  uint16_t flag);

void TMR_ConfigDMA(TMR_T* TMRx, TMR_DMA_BASE_ADDERSS_T address, TMR_DMA_BURST_LENGHT_T lenght);
void TMR_EnableDMASoure(TMR_T* TMRx, uint16_t souces);
void TMR_DisableDMASoure(TMR_T* TMRx, uint16_t souces);
void TMR_EnableCCDMA(TMR_T* TMRx);
void TMR_DisableCCDMA(TMR_T* TMRx);

/** Clocks management */
void TMR_ConfigInternalClock(TMR_T* TMRx);
void TMR_ConfigITRxExternalClock(TMR_T* TMRx, TMR_INPUT_TRIGGER_SOURCE_T input);
void TMR_ConfigTIxExternalClock(TMR_T* TMRx, TMR_INPUT_TRIGGER_SOURCE_T input,
                                TMR_IC_POLARITY_T ICpolarity, uint16_t ICfilter);
void TMR_ConfigExternalClockMode1(TMR_T* TMRx, TMR_EXTTRG_PRESCALER_T prescaler,
                                 TMR_EXTTRG_POLARITY_T polarity, uint16_t filter);
void TMR_ConfigExternalClockMode2(TMR_T* TMRx, TMR_EXTTRG_PRESCALER_T prescaler,
                                  TMR_EXTTRG_POLARITY_T polarity, uint16_t filter);
/** Synchronization management */
void TMR_SelectInputTrigger(TMR_T* TMRx, TMR_INPUT_TRIGGER_SOURCE_T input);
void TMR_SelectOutputTrigger(TMR_T* TMRx, TMR_TRGOSOURCE_T source);
void TMR_EnableMasterSlaveMode(TMR_T* TMRx);
void TMR_DisableMasterSlaveMode(TMR_T* TMRx);
void TMR_ConfigExternalTrigger(TMR_T* TMRx, TMR_EXTTRG_PRESCALER_T prescaler,
                               TMR_EXTTRG_POLARITY_T polarity, uint16_t filter);

/** Specific interface management */
void TMR_ConfigEncodeInterface(TMR_T* TMRx, TMR_ENCODER_MODE_T encodeMode, TMR_IC_POLARITY_T IC1Polarity,
                               TMR_IC_POLARITY_T IC2Polarity);
void TMR_EnableHallSensor(TMR_T* TMRx);
void TMR_DisableHallSensor(TMR_T* TMRx);

/** Specific remapping management */
void TMR_ConfigRemap(TMR_T* TMRx, TMR_REMAP_T remap);

/**@} end of group TMR_Fuctions*/
/**@} end of group TMR_Driver */
/**@} end of group Peripherals_Library*/

#ifdef __cplusplus
}
#endif

#endif /* __TMR_H */
