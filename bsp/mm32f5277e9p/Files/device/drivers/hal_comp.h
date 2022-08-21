/*
 * Copyright 2021 MindMotion Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __HAL_COMP_H__
#define __HAL_COMP_H__

#include "hal_common.h"

/*!
 * @addtogroup COMP
 * @{
 */

/*!
 * @brief COMP driver version number.
 */
#define COMP_DRIVER_VERSION 1u /*!< COMP_0. */

/*!
 * @brief COMP number.
 */
#define COMP_CHANNEL_NUM 3u

/*!
 * @brief COMP output filter type.
 *
 * If the compare result keeps n APB Clocks unchanged, the output is valid.
 */
typedef enum
{
    COMP_OutFilter_1   = 0x00u, /*!< 1   PCLK filter. */
    COMP_OutFilter_2   = 0x01u, /*!< 2   PCLK filter. */
    COMP_OutFilter_4   = 0x02u, /*!< 4   PCLK filter. */
    COMP_OutFilter_8   = 0x03u, /*!< 8   PCLK filter. */
    COMP_OutFilter_16  = 0x04u, /*!< 16  PCLK filter. */
    COMP_OutFilter_32  = 0x05u, /*!< 32  PCLK filter. */
    COMP_OutFilter_64  = 0x06u, /*!< 64  PCLK filter. */
    COMP_OutFilter_128 = 0x07u, /*!< 128 PCLK filter. */
} COMP_OutFilter_Type;

/*!
 * @brief COMP hysteresis type.
 *
 * If V(InvInput) > V(PosInput), the compare result is high, if V(InvInput) < (V(PosInput) - hysteresis), the compare result is low.
 */
typedef enum
{
    COMP_Hysteresis_Alt0 = 0x00u, /*!< Hysteresis Alt 0. */
    COMP_Hysteresis_Alt1 = 0x01u, /*!< Hysteresis Alt 1. */
    COMP_Hysteresis_Alt2 = 0x02u, /*!< Hysteresis Alt 2. */
    COMP_Hysteresis_Alt3 = 0x03u, /*!< Hysteresis Alt 3. */
} COMP_Hysteresis_Type;

/*!
 * @brief COMP output mux type.
 */
typedef enum
{
    COMP_OutMux_None = 0x00u,  /*!< Not output to other peripheral input. */
    COMP_OutMux_Alt0 = 0x02u,  /*!< Output Mux 0. */
    COMP_OutMux_Alt1 = 0x04u,  /*!< Output Mux 1. */
    COMP_OutMux_Alt2 = 0x06u,  /*!< Output Mux 2. */
    COMP_OutMux_Alt3 = 0x07u,  /*!< Output Mux 3. */
    COMP_OutMux_Alt4 = 0x08u,  /*!< Output Mux 4. */
    COMP_OutMux_Alt5 = 0x09u,  /*!< Output Mux 5. */
    COMP_OutMux_Alt6 = 0x0Au,  /*!< Output Mux 6. */
    COMP_OutMux_Alt7 = 0x0Bu,  /*!< Output Mux 7. */
    COMP_OutMux_Alt8 = 0x0Fu,  /*!< Output Mux 8. */
} COMP_OutMux_Type;

/*!
 * @brief COMP input Mux type.
 */
typedef enum
{
    COMP_InMux_Alt0 = 0x00u, /*!< Input Mux 0. */
    COMP_InMux_Alt1 = 0x01u, /*!< Input Mux 1. */
    COMP_InMux_Alt2 = 0x02u, /*!< Input Mux 2. */
    COMP_InMux_Alt3 = 0x03u, /*!< Input Mux 3. */
    COMP_InMux_Alt4 = 0x04u, /*!< Input Mux 4. */
    COMP_InMux_Alt5 = 0x05u, /*!< Input Mux 5. */
} COMP_InMux_Type;

/*!
 * @brief COMP speed type.
 */
typedef enum
{
    COMP_Speed_High    = 0x00u, /*!< High speed, high power. */
    COMP_Speed_Middle  = 0x01u, /*!< Middle speed, middle power. */
    COMP_Speed_Low     = 0x02u, /*!< Low speed, low power. */
    COMP_Speed_DeepLow = 0x03u, /*!< Deep low speed, deep low power. */
} COMP_Speed_Type;

/*!
 * @brief COMP external reference voltage source type.
 */
typedef enum
{
    COMP_ExtVrefSource_VREFINT = 0x00u,  /*!< Internal reference voltage. */
    COMP_ExtVrefSource_VDDA    = 0x01u,  /*!< VDDA voltage.   */
} COMP_ExtVrefSource_Type;

/*!
 * @brief COMP external reference voltage type.
 */
typedef enum
{
    COMP_ExtVrefVolt_Alt0  = 0x00u,  /*!< External reference voltage Alt 0. */
    COMP_ExtVrefVolt_Alt1  = 0x01u,  /*!< External reference voltage Alt 1. */
    COMP_ExtVrefVolt_Alt2  = 0x02u,  /*!< External reference voltage Alt 2. */
    COMP_ExtVrefVolt_Alt3  = 0x03u,  /*!< External reference voltage Alt 3. */
    COMP_ExtVrefVolt_Alt4  = 0x04u,  /*!< External reference voltage Alt 4. */
    COMP_ExtVrefVolt_Alt5  = 0x05u,  /*!< External reference voltage Alt 5. */
    COMP_ExtVrefVolt_Alt6  = 0x06u,  /*!< External reference voltage Alt 6. */
    COMP_ExtVrefVolt_Alt7  = 0x07u,  /*!< External reference voltage Alt 7. */
    COMP_ExtVrefVolt_Alt8  = 0x08u,  /*!< External reference voltage Alt 8. */
    COMP_ExtVrefVolt_Alt9  = 0x09u,  /*!< External reference voltage Alt 9. */
    COMP_ExtVrefVolt_Alt10 = 0x0Au,  /*!< External reference voltage Alt 10. */
    COMP_ExtVrefVolt_Alt11 = 0x0Bu,  /*!< External reference voltage Alt 11. */
    COMP_ExtVrefVolt_Alt12 = 0x0Cu,  /*!< External reference voltage Alt 12. */
    COMP_ExtVrefVolt_Alt13 = 0x0Du,  /*!< External reference voltage Alt 13. */
    COMP_ExtVrefVolt_Alt14 = 0x0Eu,  /*!< External reference voltage Alt 14. */
    COMP_ExtVrefVolt_Alt15 = 0x0Fu,  /*!< External reference voltage Alt 15. */
} COMP_ExtVrefVolt_Type;

/*!
 * @brief COMP round robin period type.
 *
 * wait n APB clock to compare next channel.
 */
typedef enum
{
    COMP_RoundRobinPeriod_1   = 0x00u,  /*!< Wait 1   PCLK2 to compare next channel. */
    COMP_RoundRobinPeriod_2   = 0x01u,  /*!< Wait 2   PCLK2 to compare next channel. */
    COMP_RoundRobinPeriod_4   = 0x02u,  /*!< Wait 4   PCLK2 to compare next channel. */
    COMP_RoundRobinPeriod_8   = 0x03u,  /*!< Wait 8   PCLK2 to compare next channel. */
    COMP_RoundRobinPeriod_16  = 0x04u,  /*!< Wait 16  PCLK2 to compare next channel. */
    COMP_RoundRobinPeriod_32  = 0x05u,  /*!< Wait 32  PCLK2 to compare next channel. */
    COMP_RoundRobinPeriod_64  = 0x06u,  /*!< Wait 64  PCLK2 to compare next channel. */
    COMP_RoundRobinPeriod_128 = 0x07u,  /*!< Wait 128 PCLK2 to compare next channel. */
} COMP_RoundRobinPeriod_Type;

/*!
 * @brief COMP round robin channel Type.
 */
typedef enum
{
    COMP_RoundRobinChnGroup_Alt0 = 0x00u, /*!< Compare the positive input 1 & 2.    */
    COMP_RoundRobinChnGroup_Alt1 = 0x01u, /*!< Compare the positive input 1, 2 & 3. */
} COMP_RoundRobinChnGroup_Type;

/*!
 * @brief This type of structure instance is used to keep the settings when calling the @ref COMP_Init() to initialize the COMP module.
 */
typedef struct
{
    COMP_Hysteresis_Type Hysteresis; /*!< Specify the hysteresis. */
    COMP_OutMux_Type     OutMux;     /*!< Specify the output. */
    COMP_InMux_Type      PosInMux;   /*!< Specify the positive input. */
    COMP_InMux_Type      InvInMux;   /*!< Specify the nagetive input. */
    COMP_Speed_Type      Speed;      /*!< Specify the compare speed. */
} COMP_Init_Type;

/*!
 * @brief This type of structure instance is used to keep the settings when calling the @ref COMP_EnableExtVrefConf() to enable the COMP ext vref.
 */
typedef struct
{
    COMP_ExtVrefSource_Type VrefSource; /*!< Specify the Vref source.  */
    COMP_ExtVrefVolt_Type   Volt;       /*!< Specify the Vref voltage. */
} COMP_ExtVrefConf_Type;

/*!
 * @brief This type of structure instance is used to keep the settings when calling the @ref COMP_EnableRoundRobinConf() to enable the COMP round robin.
 */
typedef struct
{
    COMP_RoundRobinPeriod_Type   Period;    /*!< Specify the round robin period.  */
    bool                         InvInFix;  /*!< Specify the nagetive input fix.  */
    COMP_RoundRobinChnGroup_Type ChnGroup;  /*!< Specify the round robin channel. */
} COMP_RoundRobinConf_Type;

/*!
 * @brief Initialize the COMP module.
 *
 * @param COMPx         COMP instance.
 * @param channel       COMP channel.
 * @param init          Pointer to the initialization structure. See to @ref COMP_Init_Type.
 * @return              None.
 */
void COMP_Init(COMP_Type * COMPx, uint32_t channel, COMP_Init_Type * init);

/*!
 * @brief Enable the COMP module.
 *
 * @param COMPx         COMP instance.
 * @param channel       COMP channel.
 * @param enable        'true' to enable the module, 'false' to disable the module.
 * @return              None.
 */
void COMP_Enable(COMP_Type * COMPx, uint32_t channel, bool enable);

/*!
 * @brief Keep the COMP settings not changed.
 *
 * @param COMPx         COMP instance.
 * @param channel       COMP channel.
 * @return              None.
 */
void COMP_Lock(COMP_Type * COMPx, uint32_t channel);

/*!
 * @brief Get comp output status.
 *
 * @param COMPx         COMP instance.
 * @param channel       COMP channel.
 * @return              output status.
 */
bool COMP_GetOutputStatus(COMP_Type * COMPx, uint32_t channel);

/*!
 * @brief Enable the COMP ext Vref.
 *
 * @param COMPx         COMP instance.
 * @param init          Pointer to the initialization structure. See to @ref COMP_ExtVrefConf_Type, if null, disable ext Vref.
 * @return              None.
 */
void COMP_EnableExtVrefConf(COMP_Type * COMPx, COMP_ExtVrefConf_Type * conf);

/*!
 * @brief Enable polling function.
 *
 * Enable round robin function, the comp channel can get positive input 1, 2 & 3 status.
 *
 * @param COMPx COMP instance.
 * @param channel COMP channel.
 * @param init Pointer to the initialization structure. See to @ref COMP_RoundRobinConf_Type, if null, disable round robin.
 * @return None.
 */
void COMP_EnableRoundRobinConf(COMP_Type * COMPx, uint32_t channel, COMP_RoundRobinConf_Type * conf);

/*!
 * @brief Get round robin output status.
 *
 * @param COMPx COMP instance.
 * @param channel COMP channel.
 * @param pos_in Positive input Mux.
 * @return Status of comp channel output level.
 */
bool COMP_GetRoundRobinOutStatus(COMP_Type * COMPx, uint32_t channel, COMP_InMux_Type pos_in);

/*!
 * @brief Enable COMP window control.
 *
 * @param COMPx COMP instance.
 * @param channel COMP channel.
 * @param enable 'true' to enable window control, 'false' to disable window control.
 * @return None.
 */
void COMP_EnableWindow(COMP_Type * COMPx, uint32_t channel, bool enable);

/*!
 * @brief Set the output filter.
 *
 * @param COMPx COMP instance.
 * @param channel COMP channel.
 * @param filter Output filter, See to @ref COMP_OutFilter_Type.
 * @return None.
 */
void COMP_SetOutFilter(COMP_Type * COMPx, uint32_t channel, COMP_OutFilter_Type filter);

/*!
 * @brief Enable the out invert.
 *
 * @param COMPx COMP instance.
 * @param channel COMP channel.
 * @param enable 'true' to enable the out invert, 'false' to disable the out invert.
 * @return None.
 */
void COMP_EnableOutInvert(COMP_Type * COMPx, uint32_t channel, bool enable);

/*!
 * @brief Enable the bypass output.
 *
 * @param COMPx COMP instance.
 * @param channel COMP channel.
 * @param enable 'true' to enable the bypass output, 'false' to disable the bypass output.
 * @return None.
 */
void COMP_EnableBypassOutput(COMP_Type * COMPx, uint32_t channel, bool enable);

/*!
 *@}
 */

#endif /* __HAL_COMP_H__ */
