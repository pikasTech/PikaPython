/*!
 * @file        apm32f0xx_rtc.h
 *
 * @brief       This file contains all the functions prototypes for the RTC firmware library.
 *
 * @version     V1.0.1
 *
 * @date        2021-07-01
 *
 */

#ifndef __RTC_H
#define __RTC_H

#include "apm32f0xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Peripherals_Library Standard Peripheral Library
  @{
*/

/** @addtogroup RTC_Driver RTC Driver
  @{
*/

/** @addtogroup RTC_Enumerations Enumerations
  @{
*/

/**
 * @brief RTC Hour Formats
 */
typedef enum
{
    RTC_HourFormat_24,   //!< 24 hour/day format
    RTC_HourFormat_12    //!< AM/PM hour format
} RTC_HOUR_FORMAT_T;

/**
 * @brief RTC Input parameter format
 */
typedef enum
{
    RTC_FORMAT_BIN,
    RTC_FORMAT_BCD
} RTC_FORMAT_T;

/**
 * @brief RTC_AM_PM
 */
typedef enum
{
    RTC_H12_AM,
    RTC_H12_PM
} RTC_H12_T;

/**
 * @brief RTC DayLightSaving
 */
typedef enum
{
    RTC_DLS_SUB1H,
    RTC_DLS_ADD1H
} RTC_DAYLIGHT_SAVING_T;

/**
 * @brief RTC DayLightSaving
 */
typedef enum
{
    RTC_SO_RESET = BIT_RESET,
    RTC_SO_SET   = BIT_SET,
} RTC_STORE_OPERATION_T;

/**
 * @brief RTC Output selection
 */
typedef enum
{
    RTC_OPSEL_DISABLE = 0x00,
    RTC_OPSEL_ALARMA  = 0x01,
    RTC_OPSEL_WAKEUP  = 0x03,  //!< Only for APM32F072 and APM32F091 devices
} RTC_OPSEL_T;

/**
 * @brief RTC Output Polarity
 */
typedef enum
{
    RTC_OPP_HIGH,
    RTC_OPP_LOW
} RTC_OPP_T;

/**
 * @brief RTC Calib Output selection
 */
typedef enum
{
    RTC_CALIBOUTPUT_512Hz,
    RTC_CALIBOUTPUT_1Hz
} RTC_CALIB_OUTPUT_T;

/**
 * @brief RTC Smooth calib period
 */
typedef enum
{
    RTC_SCP_16SEC,
    RTC_SCP_8SEC
} RTC_SCP_T;

/**
 * @brief RTC Smooth calib period
 */
typedef enum
{
    RTC_SCPP_RESET,
    RTC_SCPP_SET
} RTC_SCPP_T;

/**
 * @brief RTC Time Stamp Edges
 */
typedef enum
{
    RTC_TIME_STAMPEDGE_RISING,
    RTC_TIME_STAMPEDGE_FALLING
} RTC_TIMESTAMP_EDGE_T;

/**
 * @brief RTC Tamper Trigger
 */
typedef enum
{
    RTC_TAMPER_TRIGGER_RISINGEDGE   = 0x00,
    RTC_TAMPER_TRIGGER_FALLINGEDGE  = 0x01,
    RTC_TAMPER_TRIGGER_LOWLEVEL     = 0x00,
    RTC_TAMPER_TRIGGER_HIGHLEVEL    = 0x01,
} RTC_TAMPER_TRIGGER_T;

/**
 * @brief RTC Tamper Pins
 */
typedef enum
{
    RTC_TAMPER_1,
    RTC_TAMPER_2,
    RTC_TAMPER_3   //!< Only for APM32F072 and APM32F091 devices 
} RTC_TAMPER_T;

/**
 * @brief Tampers Sampling Frequency
 */
typedef enum
{
    RTC_TAMPER_FILTER_DISABLE,
    RTC_TAMPER_FILTER_2SAMPLE,
    RTC_TAMPER_FILTER_4SAMPLE,
    RTC_TAMPER_FILTER_8SAMPLE
} RTC_TAMPER_FILTER_T;

/**
 * @brief Tampers Sampling Frequency
 */
typedef enum
{
    RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV32768,
    RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV16384,
    RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV8192,
    RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV4096,
    RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV2048,
    RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV1024,
    RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV512,
    RTC_TAMPERSAMPLINGFREQ_RTCCLK_DIV256
} RTC_TAMPER_SAMPLING_FREQ_T;

/**
 * @brief Precharge Duration
 */
typedef enum
{
    RTC_PRECHARGEDURATION_1RTCCLK,
    RTC_PRECHARGEDURATION_2RTCCLK,
    RTC_PRECHARGEDURATION_4RTCCLK,
    RTC_PRECHARGEDURATION_8RTCCLK
} RTC_PRECHARGE_DURATION_T;

/**
 * @brief RTC Add 1 Second Parameter
 */
typedef enum
{
    RTC_SHIFTADD1S_RESET,
    RTC_SHIFTADD1S_SET
} RTC_SHIFT_ADD1S_T;

/**
 * @brief
 */
typedef enum
{
    RTC_OPENDRAIN,
    RTC_PUSHPULL
} RTC_OUTPUT_T;

/**
 * @brief RTC AlarmDateWeekDay
 */
typedef enum
{
    RTC_WEEKDAY_SEL_DATE,
    RTC_WEEKDAY_SEL_WEEKDAY
} RTC_WEEKDAY_SEL_T;

/**
  * @brief  RTC Interrupts  Soure
  */
typedef enum
{
    RTC_INT_ALR         = 0x00001000,
    RTC_INT_WT          = 0x00004000,
    RTC_INT_TS          = 0x00008000,
    RTC_INT_TAMP        = 0x00000004,
} RTC_INT_T;

/**
  * @brief  RTC Interrupts  Flag
  */
typedef enum
{
    RTC_INT_FLAG_ALR    = 0x00001000,
    RTC_INT_FLAG_WT     = 0x00004000, //!< Only for APM32F072 and APM32F091 devices
    RTC_INT_FLAG_TS     = 0x00008000,
    RTC_INT_FLAG_TAMP1  = 0x00020004,
    RTC_INT_FLAG_TAMP2  = 0x00040004,
    RTC_INT_FLAG_TAMP3  = 0x00080004, //!< Only for APM32F072 and APM32F091 devices
} RTC_INT_FLAG_T;

/**
  * @brief  RTC flag
  */
typedef enum
{
    RTC_FLAG_AWF        = BIT0,
    RTC_FLAG_WTWF       = BIT2, //!< Only for APM32F072 and APM32F091 devices
    RTC_FLAG_SOPF       = BIT3,
    RTC_FLAG_ISF        = BIT4,
    RTC_FLAG_RSF        = BIT5,
    RTC_FLAG_INTF       = BIT6,
    RTC_FLAG_ALRF       = BIT8,
    RTC_FLAG_WTF        = BIT10, //!< Only for APM32F072 and APM32F091 devices
    RTC_FLAG_TSF        = BIT11,
    RTC_FLAG_TSOF       = BIT12,
    RTC_FLAG_TP1F       = BIT13,
    RTC_FLAG_TP2F       = BIT14,
    RTC_FLAG_TP3F       = BIT15,  //!< Only for APM32F072 and APM32F091 devices
    RTC_FLAG_RPF        = BIT16,
} RTC_FLAG_T;

/**
  * @brief  RTC_Backup
  */
typedef enum
{
    RTC_BAKP_DATA0,
    RTC_BAKP_DATA1,
    RTC_BAKP_DATA2,
    RTC_BAKP_DATA3,
    RTC_BAKP_DATA4
} RTC_BAKP_DATA_T;

/**
  * @brief  RTC_AlarmMask
  */
typedef enum
{
    RTC_MASK_NONE        = 0x00000000,
    RTC_MASK_DATEWEEK    = (int)0x80000000,
    RTC_MASK_HOURS       = 0x00800000,
    RTC_MASK_MINUTES     = 0x00008000,
    RTC_MASK_SECONDS     = 0x00000080,
    RTC_MASK_ALL         = (int)0x80808080,
} RTC_MASK_T;

/**
  * @brief  RTC Wakeup clock select (only APM32F072 and APM32F091 devices)
  */
typedef enum
{
    RTC_WAKEUP_CLOCK_RTCDIV16 = 0x00,
    RTC_WAKEUP_CLOCK_RTCDIV8,
    RTC_WAKEUP_CLOCK_RTCDIV4,
    RTC_WAKEUP_CLOCK_RTCDIV2,
    RTC_WAKEUP_CLOCK_CK_SPRE_16B,
    RTC_WAKEUP_CLOCK_CK_SPRE_17B,
} RTC_WAKEUP_CLOCK_T;

/**@} end of group RTC_Enumerations*/


/** @addtogroup RTC_Macros Macros
  @{
*/

/** Macros description */
#define RTC_INITMODE_TIMEOUT      ((uint32_t) 0x00004000)
#define RTC_SYNCHRO_TIMEOUT       ((uint32_t) 0x00008000)
#define RTC_RECALPF_TIMEOUT       ((uint32_t) 0x00001000)
#define RTC_SHPF_TIMEOUT          ((uint32_t) 0x00001000)

#define RTC_CTRL_INT              ((uint32_t) 0x0000D000)
#define RTC_TAFCFG_INT            ((uint32_t) 0x00000004)
#define RTC_CTRL_INT              ((uint32_t) 0x0000D000)
#define RTC_TAFCFG_INT            ((uint32_t) 0x00000004)

#define RTC_MONTH_JANUARY         ((uint8_t)0x01)
#define RTC_MONTH_FEBRUARY        ((uint8_t)0x02)
#define RTC_MONTH_MARCH           ((uint8_t)0x03)
#define RTC_MONTH_APRIL           ((uint8_t)0x04)
#define RTC_MONTH_MAY             ((uint8_t)0x05)
#define RTC_MONTH_JUNE            ((uint8_t)0x06)
#define RTC_MONTH_JULY            ((uint8_t)0x07)
#define RTC_MONTH_AUGUST          ((uint8_t)0x08)
#define RTC_MONTH_SEPTEMBER       ((uint8_t)0x09)
#define RTC_MONTH_OCTOBER         ((uint8_t)0x0A)
#define RTC_MONTH_NOVEMBER        ((uint8_t)0x0B)
#define RTC_MONTH_DECEMBER        ((uint8_t)0x0C)

#define RTC_WEEKDAY_MONDAY        ((uint8_t)0x01)
#define RTC_WEEKDAY_TUESDAY       ((uint8_t)0x02)
#define RTC_WEEKDAY_WEDNESDAY     ((uint8_t)0x03)
#define RTC_WEEKDAY_THURSDAY      ((uint8_t)0x04)
#define RTC_WEEKDAY_FRIDAY        ((uint8_t)0x05)
#define RTC_WEEKDAY_SATURDAY      ((uint8_t)0x06)
#define RTC_WEEKDAY_SUNDAY        ((uint8_t)0x07)

/**@} end of group RTC_Macros*/


/** @addtogroup RTC_Structure Data Structure
  @{
*/

/**
  * @brief  RTC Init structures definition
  */
typedef struct
{
    RTC_HOUR_FORMAT_T format;
    uint32_t AsynchPrediv; //!< AsynchPrediv<= 0x7F
    uint32_t SynchPrediv;  //!< SynchPrediv<= 0x7FFF
} RTC_Config_T;

/**
  * @brief  RTC Time structure definition
  */
typedef struct
{
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
    uint8_t H12;
} RTC_TIME_T;

/**
  * @brief  RTC Date structure definition
  */
typedef struct
{
    uint8_t weekday;
    uint8_t month;
    uint8_t date;
    uint8_t year;
} RTC_DATE_T;

/**
  * @brief  RTC ALRMA structure definition
  */
typedef struct
{
    RTC_TIME_T time;
    uint32_t AlarmMask;
    RTC_WEEKDAY_SEL_T AlarmDateWeekDaySel;
    uint8_t AlarmDateWeekDay;
} RTC_ALARM_T;

/**@} end of group RTC_Structure*/


/** @addtogroup RTC_Fuctions Fuctions
  @{
*/

/** Initialization and Configuration functions */
uint8_t RTC_Reset(void);

uint8_t RTC_Config(RTC_Config_T* Struct);
void RTC_ConfigStructInit(RTC_Config_T* Struct);
void RTC_EnableWriteProtection(void);
void RTC_DisableWriteProtection(void);

uint8_t RTC_EnableInit(void);
void RTC_DisableInit(void);

uint8_t RTC_WaitForSynchro(void);
uint8_t RTC_EnableRefClock(void);
uint8_t RTC_DisableRefClock(void);
void RTC_EnableBypassShadow(void);
void RTC_DisableBypassShadow(void);

/** Time and Date configuration functions */
uint8_t RTC_ConfigTime(RTC_FORMAT_T format, RTC_TIME_T* timeStruct);
void RTC_ConfigTimeStructInit(RTC_TIME_T* timeStruct);
void RTC_ReadTime(RTC_FORMAT_T format, RTC_TIME_T* timeStruct);
uint32_t RTC_ReadSubSecond(void);
uint8_t RTC_ConfigDate(RTC_FORMAT_T format, RTC_DATE_T* dateStruct);
void RTC_ConfigDateStructInit(RTC_DATE_T* dateStruct);
void RTC_ReadDate(RTC_FORMAT_T format, RTC_DATE_T* dateStruct);

/**  Alarms (ALRMA A) configuration functions  */
void RTC_ConfigAlarm(RTC_FORMAT_T format, RTC_ALARM_T* alarmStruct);
void RTC_ConfigAlarmStructInit(RTC_ALARM_T* alarmStruct);
void RTC_ReadAlarm(RTC_FORMAT_T format, RTC_ALARM_T* alarmStruct);
void RTC_EnableAlarm(void);
uint8_t RTC_DisableAlarm(void);
void RTC_ConfigAlarmSubSecond(uint32_t val, uint8_t mask);
uint32_t RTC_ReadAlarmSubSecond(void);

/** WakeUp Timer configuration functions */ 
void RTC_ConfigWakeUpClock(RTC_WAKEUP_CLOCK_T wakeUpClock);  //!< Only for APM32F072 and APM32F091 devices
void RTC_SetWakeUpValue(uint32_t wakeUpValue);               //!< Only for APM32F072 and APM32F091 devices
uint32_t RTC_ReadWakeUpValue(void);                          //!< Only for APM32F072 and APM32F091 devices
void RTC_EnableWakeUp(void);                                 //!< Only for APM32F072 and APM32F091 devices
uint8_t RTC_DisableWakeUp(void);                             //!< Only for APM32F072 and APM32F091 devices

/* Daylight Saving configuration functions */
void RTC_ConfigDayLightSaving(RTC_DAYLIGHT_SAVING_T sav, RTC_STORE_OPERATION_T bit);
uint32_t RTC_ReadStoreOperation(void);

/** Output pin Configuration function */
void RTC_ConfigOutput(RTC_OPSEL_T opsel, RTC_OPP_T opp);

/** Digital Calibration configuration functions */
void RTC_EnableCalibOutput(void);
void RTC_DisableCalibOutput(void);
void RTC_ConfigCalibOutput(RTC_CALIB_OUTPUT_T calib);
uint8_t RTC_ConfigSmoothCalib(RTC_SCP_T period, RTC_SCPP_T bit, uint32_t value);

/** TimeStamp configuration functions */
void RTC_EnableTimeStamp(RTC_TIMESTAMP_EDGE_T edge);
void RTC_DisableTimeStamp(RTC_TIMESTAMP_EDGE_T edge);
void RTC_ReadTimeDate(RTC_FORMAT_T format, RTC_TIME_T* timeStruct,
                      RTC_DATE_T* dateStruct);
uint32_t RTC_ReadTimeStampSubSecond(void);

/** Tampers configuration functions */
void RTC_ConfigTamperTrigger(RTC_TAMPER_T tamper, RTC_TAMPER_TRIGGER_T trigger);
void RTC_EnableTamper(RTC_TAMPER_T tamper);
void RTC_DisableTamper(RTC_TAMPER_T tamper);
void RTC_ConfigFilter(RTC_TAMPER_FILTER_T filter);
void RTC_ConfigSamplingFreq(RTC_TAMPER_SAMPLING_FREQ_T freq);
void RTC_PinsPrechargeDuration(RTC_PRECHARGE_DURATION_T duration);
void RTC_EnableTDE(void);
void RTC_DisableTDE(void);
void RTC_EnablePullUp(void);
void RTC_DisablePullUp(void);

/* Backup Data Registers configuration functions */
void RTC_WriteBackup(RTC_BAKP_DATA_T backup, uint32_t data);
uint32_t RTC_ReadBackup(RTC_BAKP_DATA_T backup);

/* Output Type Config configuration functions */
void RTC_ConfigOutputType(RTC_OUTPUT_T output);

/** RTC Shift control synchonisation functions */
uint8_t RTC_ConfigSynchroShift(RTC_SHIFT_ADD1S_T add1S, uint32_t subFS);

/** Interrupts and flags management functions */
void RTC_EnableInterrupt(uint32_t interrupt);
void RTC_DisableInterrupt(uint32_t interrupt);
uint8_t RTC_ReadStatusFlag(RTC_FLAG_T flag);
void RTC_ClearStatusFlag(uint32_t flag);
uint8_t RTC_ReadIntFlag(RTC_INT_FLAG_T flag);
void RTC_ClearIntFlag(uint32_t flag);

/**@} end of group RTC_Fuctions*/
/**@} end of group RTC_Driver*/
/**@} end of group Peripherals_Library*/

#ifdef __cplusplus
}
#endif

#endif /* __RTC_H */
