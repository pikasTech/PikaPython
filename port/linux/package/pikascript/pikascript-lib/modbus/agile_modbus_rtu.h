/**
 * @file    agile_modbus_rtu.h
 * @brief   Agile Modbus 软件包 RTU 头文件
 * @author  马龙伟 (2544047213@qq.com)
 * @date    2021-12-02
 *
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 Ma Longwei.
 * All rights reserved.</center></h2>
 *
 */

#ifndef __PKG_AGILE_MODBUS_RTU_H
#define __PKG_AGILE_MODBUS_RTU_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/** @addtogroup RTU
 * @{
 */

/** @defgroup RTU_Exported_Constants RTU Exported Constants
 * @{
 */
#define AGILE_MODBUS_RTU_HEADER_LENGTH     1
#define AGILE_MODBUS_RTU_PRESET_REQ_LENGTH 6
#define AGILE_MODBUS_RTU_PRESET_RSP_LENGTH 2

#define AGILE_MODBUS_RTU_CHECKSUM_LENGTH 2

/**
 @verbatim
    Modbus_Application_Protocol_V1_1b.pdf Chapter 4 Section 1 Page 5
    RS232 / RS485 ADU = 253 bytes + slave (1 byte) + CRC (2 bytes) = 256 bytes

 @endverbatim
 */
#define AGILE_MODBUS_RTU_MAX_ADU_LENGTH 256
/**
 * @}
 */

/** @defgroup RTU_Exported_Types RTU Exported Types
 * @{
 */

/**
 * @brief   RTU 结构体
 */
typedef struct agile_modbus_rtu {
    agile_modbus_t _ctx; /**< modbus 句柄 */
} agile_modbus_rtu_t;

/**
 * @}
 */

/** @addtogroup RTU_Exported_Functions
 * @{
 */
int agile_modbus_rtu_init(agile_modbus_rtu_t *ctx, uint8_t *send_buf, int send_bufsz, uint8_t *read_buf, int read_bufsz);
/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __PKG_AGILE_MODBUS_RTU_H */
