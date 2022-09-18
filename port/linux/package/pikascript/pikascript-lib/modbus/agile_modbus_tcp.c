/**
 * @file    agile_modbus_tcp.c
 * @brief   Agile Modbus 软件包 TCP 源文件
 * @author  马龙伟 (2544047213@qq.com)
 * @date    2021-12-02
 *
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 Ma Longwei.
 * All rights reserved.</center></h2>
 *
 */

#include "agile_modbus.h"
#include "agile_modbus_tcp.h"

/** @defgroup TCP TCP
 * @{
 */

/** @defgroup TCP_Private_Functions TCP Private Functions
 * @{
 */

/**
 * @brief   TCP 设置地址接口
 * @param   ctx modbus 句柄
 * @param   slave 从机地址
 * @return  0:成功
 */
static int agile_modbus_tcp_set_slave(agile_modbus_t *ctx, int slave)
{
    ctx->slave = slave;
    return 0;
}

/**
 * @brief   TCP 构建基础请求报文接口(头部报文)
 * @param   ctx modbus 句柄
 * @param   function 功能码
 * @param   addr 寄存器地址
 * @param   nb 寄存器数目
 * @param   req 数据存放指针
 * @return  数据长度
 */
static int agile_modbus_tcp_build_request_basis(agile_modbus_t *ctx, int function,
                                                int addr, int nb,
                                                uint8_t *req)
{
    agile_modbus_tcp_t *ctx_tcp = ctx->backend_data;

    /* Increase transaction ID */
    if (ctx_tcp->t_id < UINT16_MAX)
        ctx_tcp->t_id++;
    else
        ctx_tcp->t_id = 0;
    req[0] = ctx_tcp->t_id >> 8;
    req[1] = ctx_tcp->t_id & 0x00ff;

    /* Protocol Modbus */
    req[2] = 0;
    req[3] = 0;

    /* Length will be defined later by set_req_length_tcp at offsets 4
       and 5 */

    req[6] = ctx->slave;
    req[7] = function;
    req[8] = addr >> 8;
    req[9] = addr & 0x00ff;
    req[10] = nb >> 8;
    req[11] = nb & 0x00ff;

    return AGILE_MODBUS_TCP_PRESET_REQ_LENGTH;
}

/**
 * @brief   TCP 构建基础响应报文接口(头部报文)
 * @param   sft modbus 头部参数结构体指针
 * @param   rsp 数据存放指针
 * @return  数据长度
 */
static int agile_modbus_tcp_build_response_basis(agile_modbus_sft_t *sft, uint8_t *rsp)
{
    /* Extract from MODBUS Messaging on TCP/IP Implementation
       Guide V1.0b (page 23/46):
       The transaction identifier is used to associate the future
       response with the request. */
    rsp[0] = sft->t_id >> 8;
    rsp[1] = sft->t_id & 0x00ff;

    /* Protocol Modbus */
    rsp[2] = 0;
    rsp[3] = 0;

    /* Length will be set later by send_msg (4 and 5) */

    /* The slave ID is copied from the indication */
    rsp[6] = sft->slave;
    rsp[7] = sft->function;

    return AGILE_MODBUS_TCP_PRESET_RSP_LENGTH;
}

/**
 * @brief   TCP 准备响应接口
 * @param   req 请求数据指针
 * @param   req_length 请求数据长度
 * @return  事务标识符
 */
static int agile_modbus_tcp_prepare_response_tid(const uint8_t *req, int *req_length)
{
    return (req[0] << 8) + req[1];
}

/**
 * @brief   TCP 预发送数据接口(计算长度字段的值并存入)
 * @param   req 数据存放指针
 * @param   req_length 已有数据长度
 * @return  数据长度
 */
static int agile_modbus_tcp_send_msg_pre(uint8_t *req, int req_length)
{
    /* Substract the header length to the message length */
    int mbap_length = req_length - 6;

    req[4] = mbap_length >> 8;
    req[5] = mbap_length & 0x00FF;

    return req_length;
}

/**
 * @brief   TCP 检查接收数据完整性接口
 * @param   ctx modbus 句柄
 * @param   msg 接收数据指针
 * @param   msg_length 有效数据长度
 * @return  有效数据长度
 */
static int agile_modbus_tcp_check_integrity(agile_modbus_t *ctx, uint8_t *msg, const int msg_length)
{
    return msg_length;
}

/**
 * @brief   TCP 预检查确认接口(对比事务标识符和协议标识符)
 * @param   ctx modbus 句柄
 * @param   req 请求数据指针
 * @param   rsp 响应数据指针
 * @param   rsp_length 响应数据长度
 * @return  0:成功; 其他:异常
 */
static int agile_modbus_tcp_pre_check_confirmation(agile_modbus_t *ctx, const uint8_t *req,
                                                   const uint8_t *rsp, int rsp_length)
{
    /* Check transaction ID */
    if (req[0] != rsp[0] || req[1] != rsp[1])
        return -1;

    /* Check protocol ID */
    if (rsp[2] != 0x0 && rsp[3] != 0x0)
        return -1;

    return 0;
}

/**
 * @}
 */

/** @defgroup TCP_Private_Constants TCP Private Constants
 * @{
 */

/**
 * @brief   TCP 后端接口
 */
static const agile_modbus_backend_t agile_modbus_tcp_backend =
    {
        AGILE_MODBUS_BACKEND_TYPE_TCP,
        AGILE_MODBUS_TCP_HEADER_LENGTH,
        AGILE_MODBUS_TCP_CHECKSUM_LENGTH,
        AGILE_MODBUS_TCP_MAX_ADU_LENGTH,
        agile_modbus_tcp_set_slave,
        agile_modbus_tcp_build_request_basis,
        agile_modbus_tcp_build_response_basis,
        agile_modbus_tcp_prepare_response_tid,
        agile_modbus_tcp_send_msg_pre,
        agile_modbus_tcp_check_integrity,
        agile_modbus_tcp_pre_check_confirmation};

/**
 * @}
 */

/** @defgroup TCP_Exported_Functions TCP Exported Functions
 * @{
 */

/**
 * @brief   TCP 初始化
 * @param   ctx TCP 句柄
 * @param   send_buf 发送缓冲区
 * @param   send_bufsz 发送缓冲区大小
 * @param   read_buf 接收缓冲区
 * @param   read_bufsz 接收缓冲区大小
 * @return  0:成功
 */
int agile_modbus_tcp_init(agile_modbus_tcp_t *ctx, uint8_t *send_buf, int send_bufsz, uint8_t *read_buf, int read_bufsz)
{
    agile_modbus_common_init(&(ctx->_ctx), send_buf, send_bufsz, read_buf, read_bufsz);
    ctx->_ctx.backend = &agile_modbus_tcp_backend;
    ctx->_ctx.backend_data = ctx;

    ctx->t_id = 0;

    return 0;
}

/**
 * @}
 */

/**
 * @}
 */
