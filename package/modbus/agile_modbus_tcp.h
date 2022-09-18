/**
 * @file    agile_modbus_tcp.h
 * @brief   Agile Modbus 软件包 TCP 头文件
 * @author  马龙伟 (2544047213@qq.com)
 * @date    2021-12-02
 *
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 Ma Longwei.
 * All rights reserved.</center></h2>
 *
 */

#ifndef __PKG_AGILE_MODBUS_TCP_H
#define __PKG_AGILE_MODBUS_TCP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/** @addtogroup TCP
 * @{
 */

/** @defgroup TCP_Exported_Constants TCP Exported Constants
 * @{
 */
#define AGILE_MODBUS_TCP_HEADER_LENGTH     7
#define AGILE_MODBUS_TCP_PRESET_REQ_LENGTH 12
#define AGILE_MODBUS_TCP_PRESET_RSP_LENGTH 8

#define AGILE_MODBUS_TCP_CHECKSUM_LENGTH 0

/**
 @verbatim
    Modbus_Application_Protocol_V1_1b.pdf Chapter 4 Section 1 Page 5
    TCP MODBUS ADU = 253 bytes + MBAP (7 bytes) = 260 bytes

 @endverbatim
 */
#define AGILE_MODBUS_TCP_MAX_ADU_LENGTH 260
/**
 * @}
 */

/** @defgroup TCP_Exported_Types TCP Exported Types
 * @{
 */

/**
 * @brief   TCP 结构体
 */
typedef struct agile_modbus_tcp {
    agile_modbus_t _ctx; /**< modbus 句柄 */
    uint16_t t_id;       /**< Extract from MODBUS Messaging on TCP/IP Implementation Guide V1.0b
                         (page 23/46):
                         The transaction identifier is used to associate the future response
                         with the request. This identifier is unique on each TCP connection. */
} agile_modbus_tcp_t;

/**
 * @}
 */

/** @addtogroup TCP_Exported_Functions
 * @{
 */
int agile_modbus_tcp_init(agile_modbus_tcp_t *ctx, uint8_t *send_buf, int send_bufsz, uint8_t *read_buf, int read_bufsz);
/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
