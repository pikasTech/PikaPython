/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH58x_SPI1.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2018/12/15
 * Description
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#include "CH58x_common.h"

/*********************************************************************
 * @fn      SPI1_MasterDefInit
 *
 * @brief   主机模式默认初始化：模式0+3线全双工+8MHz
 *
 * @param   none
 *
 * @return  none
 */
void SPI1_MasterDefInit(void)
{
    R8_SPI1_CLOCK_DIV = 4; // 主频时钟4分频
    R8_SPI1_CTRL_MOD = RB_SPI_ALL_CLEAR;
    R8_SPI1_CTRL_MOD = RB_SPI1_SDO_OE | RB_SPI_SCK_OE;
    R8_SPI1_CTRL_CFG |= RB_SPI_AUTO_IF; // 访问BUFFER/FIFO自动清除IF_BYTE_END标志
}

/*********************************************************************
 * @fn      SPI1_CLKCfg
 *
 * @brief   SPI1 基准时钟配置，= d*Tsys
 *
 * @param   c       - 时钟分频系数
 *
 * @return  none
 */
void SPI1_CLKCfg(uint8_t c)
{
    if(c == 2)
    {
        R8_SPI1_CTRL_CFG |= RB_SPI_MST_DLY_EN;
    }
    else
    {
        R8_SPI1_CTRL_CFG &= ~RB_SPI_MST_DLY_EN;
    }
    R8_SPI1_CLOCK_DIV = c;
}

/*********************************************************************
 * @fn      SPI1_DataMode
 *
 * @brief   设置数据流模式
 *
 * @param   m       - 数据流模式 refer to ModeBitOrderTypeDef
 *
 * @return  none
 */
void SPI1_DataMode(ModeBitOrderTypeDef m)
{
    switch(m)
    {
        case Mode0_LowBitINFront:
            R8_SPI1_CTRL_MOD &= ~RB_SPI_MST_SCK_MOD;
            R8_SPI1_CTRL_CFG |= RB_SPI_BIT_ORDER;
            break;
        case Mode0_HighBitINFront:
            R8_SPI1_CTRL_MOD &= ~RB_SPI_MST_SCK_MOD;
            R8_SPI1_CTRL_CFG &= ~RB_SPI_BIT_ORDER;
            break;
        case Mode3_LowBitINFront:
            R8_SPI1_CTRL_MOD |= RB_SPI_MST_SCK_MOD;
            R8_SPI1_CTRL_CFG |= RB_SPI_BIT_ORDER;
            break;
        case Mode3_HighBitINFront:
            R8_SPI1_CTRL_MOD |= RB_SPI_MST_SCK_MOD;
            R8_SPI1_CTRL_CFG &= ~RB_SPI_BIT_ORDER;
            break;
        default:
            break;
    }
}

/*********************************************************************
 * @fn      SPI1_MasterSendByte
 *
 * @brief   发送单字节 (buffer)
 *
 * @param   d       - 发送字节
 *
 * @return  none
 */
void SPI1_MasterSendByte(uint8_t d)
{
    R8_SPI1_CTRL_MOD &= ~RB_SPI_FIFO_DIR;
    R8_SPI1_BUFFER = d;
    while(!(R8_SPI1_INT_FLAG & RB_SPI_FREE));
}

/*********************************************************************
 * @fn      SPI1_MasterRecvByte
 *
 * @brief   接收单字节 (buffer)
 *
 * @param   none
 *
 * @return  接收到的字节
 */
uint8_t SPI1_MasterRecvByte(void)
{
    R8_SPI1_CTRL_MOD &= ~RB_SPI_FIFO_DIR;
    R8_SPI1_BUFFER = 0xFF; // 启动传输
    while(!(R8_SPI1_INT_FLAG & RB_SPI_FREE));
    return (R8_SPI1_BUFFER);
}

/*********************************************************************
 * @fn      SPI1_MasterTrans
 *
 * @brief   使用FIFO连续发送多字节
 *
 * @param   pbuf    - 待发送的数据内容首地址
 * @param   len     - 请求发送的数据长度，最大4095
 *
 * @return  none
 */
void SPI1_MasterTrans(uint8_t *pbuf, uint16_t len)
{
    uint16_t sendlen;

    sendlen = len;
    R8_SPI1_CTRL_MOD &= ~RB_SPI_FIFO_DIR; // 设置数据方向为输出
    R16_SPI1_TOTAL_CNT = sendlen;         // 设置要发送的数据长度
    R8_SPI1_INT_FLAG = RB_SPI_IF_CNT_END;
    while(sendlen)
    {
        if(R8_SPI1_FIFO_COUNT < SPI_FIFO_SIZE)
        {
            R8_SPI1_FIFO = *pbuf;
            pbuf++;
            sendlen--;
        }
    }
    while(R8_SPI1_FIFO_COUNT != 0); // 等待FIFO中的数据全部发送完成
}

/*********************************************************************
 * @fn      SPI1_MasterRecv
 *
 * @brief   使用FIFO连续接收多字节
 *
 * @param   pbuf    - 待接收的数据首地址
 * @param   len     - 待接收的数据长度，最大4095
 *
 * @return  none
 */
void SPI1_MasterRecv(uint8_t *pbuf, uint16_t len)
{
    uint16_t readlen;

    readlen = len;
    R8_SPI1_CTRL_MOD |= RB_SPI_FIFO_DIR; // 设置数据方向为输入
    R16_SPI1_TOTAL_CNT = len;            // 设置需要接收的数据长度，FIFO方向为输入长度不为0则会启动传输 */
    R8_SPI1_INT_FLAG = RB_SPI_IF_CNT_END;
    while(readlen)
    {
        if(R8_SPI1_FIFO_COUNT)
        {
            *pbuf = R8_SPI1_FIFO;
            pbuf++;
            readlen--;
        }
    }
}
