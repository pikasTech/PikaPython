/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH58x_i2c.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2021/03/15
 * Description
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#include "CH58x_common.h"

/*********************************************************************
 * @fn      I2C_Init
 *
 * @brief   Initializes the I2Cx peripheral according to the specified
 *          parameters in the I2C_InitStruct.
 *
 * @param   I2C_Mode        - refer to I2C_ModeTypeDef
 * @param   I2C_ClockSpeed  - Specifies the clock frequency(Hz).
 *                            This parameter must be set to a value lower than 400kHz
 * @param   I2C_DutyCycle   - Specifies the I2C fast mode duty cycle.refer to I2C_DutyTypeDef
 * @param   I2C_Ack         - Enables or disables the acknowledgement.refer to I2C_AckTypeDef
 * @param   I2C_AckAddr     - Specifies if 7-bit or 10-bit address is acknowledged.refer to I2C_AckAddrTypeDef
 * @param   I2C_OwnAddress1 - Specifies the first device own address.
 *                            This parameter can be a 7-bit or 10-bit address.
 *
 * @return  none
 */
void I2C_Init(I2C_ModeTypeDef I2C_Mode, UINT32 I2C_ClockSpeed, I2C_DutyTypeDef I2C_DutyCycle,
              I2C_AckTypeDef I2C_Ack, I2C_AckAddrTypeDef I2C_AckAddr, UINT16 I2C_OwnAddress1)
{
    uint32_t sysClock;
    uint16_t tmpreg;

    sysClock = GetSysClock();

    R16_I2C_CTRL2 &= ~RB_I2C_FREQ;
    R16_I2C_CTRL2 |= (sysClock / 1000000);

    R16_I2C_CTRL1 &= ~RB_I2C_PE;

    if(I2C_ClockSpeed <= 100000)
    {
        tmpreg = (sysClock / (I2C_ClockSpeed << 1)) & RB_I2C_CCR;

        if(tmpreg < 0x04)
            tmpreg = 0x04;

        R16_I2C_RTR = (((sysClock / 1000000) + 1) > 0x3F) ? 0x3F : ((sysClock / 1000000) + 1);
    }
    else
    {
        if(I2C_DutyCycle == I2C_DutyCycle_2)
        {
            tmpreg = (sysClock / (I2C_ClockSpeed * 3)) & RB_I2C_CCR;
        }
        else
        {
            tmpreg = (sysClock / (I2C_ClockSpeed * 25)) & RB_I2C_CCR;
            tmpreg |= I2C_DutyCycle_16_9;
        }

        if(tmpreg == 0)
        {
            tmpreg |= (uint16_t)0x0001;
        }

        tmpreg |= RB_I2C_F_S;
        R16_I2C_RTR = (uint16_t)((((sysClock / 1000000) * (uint16_t)300) / (uint16_t)1000) + (uint16_t)1);
    }
    R16_I2C_CKCFGR = tmpreg;

    R16_I2C_CTRL1 |= RB_I2C_PE;

    R16_I2C_CTRL1 &= ~(RB_I2C_SMBUS | RB_I2C_SMBTYPE | RB_I2C_ACK);
    R16_I2C_CTRL1 |= I2C_Mode | I2C_Ack;

    R16_I2C_OADDR1 &= ~0xFFFF;
    R16_I2C_OADDR1 |= I2C_AckAddr | I2C_OwnAddress1;
}

/*********************************************************************
 * @fn      I2C_Cmd
 *
 * @brief   Enables or disables the specified I2C peripheral.
 *
 * @param   NewState    - ENABLE or DISABLE.
 *
 * @return  none
 */
void I2C_Cmd(FunctionalState NewState)
{
    if(NewState != DISABLE)
        R16_I2C_CTRL1 |= RB_I2C_PE;
    else
        R16_I2C_CTRL1 &= ~RB_I2C_PE;
}

/*********************************************************************
 * @fn      I2C_GenerateSTART
 *
 * @brief   Generates I2Cx communication START condition.
 *
 * @param   NewState    - ENABLE or DISABLE.
 *
 * @return  none
 */
void I2C_GenerateSTART(FunctionalState NewState)
{
    if(NewState != DISABLE)
        R16_I2C_CTRL1 |= RB_I2C_START;
    else
        R16_I2C_CTRL1 &= ~RB_I2C_START;
}

/*********************************************************************
 * @fn      I2C_GenerateSTOP
 *
 * @brief   Generates I2Cx communication STOP condition.
 *
 * @param   NewState    - ENABLE or DISABLE.
 *
 * @return  none
 */
void I2C_GenerateSTOP(FunctionalState NewState)
{
    if(NewState != DISABLE)
        R16_I2C_CTRL1 |= RB_I2C_STOP;
    else
        R16_I2C_CTRL1 &= ~RB_I2C_STOP;
}

/*********************************************************************
 * @fn      I2C_AcknowledgeConfig
 *
 * @brief   Enables or disables the specified I2C acknowledge feature.
 *
 * @param   NewState    - ENABLE or DISABLE.
 *
 * @return  none
 */
void I2C_AcknowledgeConfig(FunctionalState NewState)
{
    if(NewState != DISABLE)
        R16_I2C_CTRL1 |= RB_I2C_ACK;
    else
        R16_I2C_CTRL1 &= ~RB_I2C_ACK;
}

/*********************************************************************
 * @fn      I2C_OwnAddress2Config
 *
 * @brief   Configures the specified I2C own address2.
 *
 * @param   Address     - specifies the 7bit I2C own address2.
 *
 * @return  none
 */
void I2C_OwnAddress2Config(uint8_t Address)
{
    R16_I2C_OADDR2 &= ~RB_I2C_ADD2;
    R16_I2C_OADDR2 |= (uint16_t)(Address & RB_I2C_ADD2);
}

/*********************************************************************
 * @fn      I2C_DualAddressCmd
 *
 * @brief   Enables or disables the specified I2C dual addressing mode.
 *
 * @param   NewState    - ENABLE or DISABLE.
 *
 * @return  none
 */
void I2C_DualAddressCmd(FunctionalState NewState)
{
    if(NewState != DISABLE)
        R16_I2C_OADDR2 |= RB_I2C_ENDUAL;
    else
        R16_I2C_OADDR2 &= ~RB_I2C_ENDUAL;
}

/*********************************************************************
 * @fn      I2C_GeneralCallCmd
 *
 * @brief   Enables or disables the specified I2C general call feature.
 *
 * @param   NewState    - ENABLE or DISABLE.
 *
 * @return  none
 */
void I2C_GeneralCallCmd(FunctionalState NewState)
{
    if(NewState != DISABLE)
        R16_I2C_CTRL1 |= RB_I2C_ENGC;
    else
        R16_I2C_CTRL1 &= ~RB_I2C_ENGC;
}

/*********************************************************************
 * @fn      I2C_ITConfig
 *
 * @brief   Enables or disables the specified I2C interrupts.
 *
 * @param   I2C_IT      - specifies the I2C interrupts sources to be enabled or disabled.
 *                        I2C_IT_BUF    - Buffer interrupt mask.
 *                        I2C_IT_EVT    - Event interrupt mask.
 *                        I2C_IT_ERR    - Error interrupt mask.
 * @param   NewState    - ENABLE or DISABLE.
 *
 * @return  none
 */
void I2C_ITConfig(I2C_ITTypeDef I2C_IT, FunctionalState NewState)
{
    if(NewState != DISABLE)
        R16_I2C_CTRL2 |= I2C_IT;
    else
        R16_I2C_CTRL2 &= (uint16_t)~I2C_IT;
}

/*********************************************************************
 * @fn      I2C_SendData
 *
 * @brief   Sends a data byte through the I2Cx peripheral.
 *
 * @param   Data    - Byte to be transmitted.
 *
 * @return  none
 */
void I2C_SendData(uint8_t Data)
{
    R16_I2C_DATAR = Data;
}

/*********************************************************************
 * @fn      I2C_ReceiveData
 *
 * @brief   Returns the most recent received data by the I2Cx peripheral.
 *
 * @return  The value of the received data.
 */
uint8_t I2C_ReceiveData(void)
{
    return (uint8_t)R16_I2C_DATAR;
}

/*********************************************************************
 * @fn      I2C_Send7bitAddress
 *
 * @brief   Transmits the address byte to select the slave device.
 *
 * @param   Address         - specifies the slave address which will be transmitted.
 * @param   I2C_Direction   - specifies whether the I2C device will be a Transmitter or a Receiver.
 *                            I2C_Direction_Transmitter - Transmitter mode.
 *                            I2C_Direction_Receiver    - Receiver mode.
 *
 * @return  none
 */
void I2C_Send7bitAddress(uint8_t Address, uint8_t I2C_Direction)
{
    if(I2C_Direction != I2C_Direction_Transmitter)
        Address |= OADDR1_ADD0_Set;
    else
        Address &= OADDR1_ADD0_Reset;

    R16_I2C_DATAR = Address;
}

/*********************************************************************
 * @fn      I2C_SoftwareResetCmd
 *
 * @brief   Enables or disables the specified I2C software reset.
 *
 * @param   NewState        - ENABLE or DISABLE.
 *
 * @return  none
 */
void I2C_SoftwareResetCmd(FunctionalState NewState)
{
    if(NewState != DISABLE)
        R16_I2C_CTRL1 |= RB_I2C_SWRST;
    else
        R16_I2C_CTRL1 &= ~RB_I2C_SWRST;
}

/*********************************************************************
 * @fn      I2C_NACKPositionConfig
 *
 * @brief   Selects the specified I2C NACK position in master receiver mode.
 *
 * @param   I2C_NACKPosition    - specifies the NACK position.
 *                                I2C_NACKPosition_Next     - indicates that the next byte will be the last received byte.
 *                                I2C_NACKPosition_Current  - indicates that current byte is the last received byte.
 *
 * @return  none
 */
void I2C_NACKPositionConfig(uint16_t I2C_NACKPosition)
{
    if(I2C_NACKPosition == I2C_NACKPosition_Next)
        R16_I2C_CTRL1 |= I2C_NACKPosition_Next;
    else
        R16_I2C_CTRL1 &= I2C_NACKPosition_Current;
}

/*********************************************************************
 * @fn      I2C_SMBusAlertConfig
 *
 * @brief   Drives the SMBusAlert pin high or low for the specified I2C.
 *
 * @param   I2C_SMBusAlert      - specifies SMBAlert pin level.
 *                              I2C_SMBusAlert_Low  - SMBAlert pin driven low.
 *                              I2C_SMBusAlert_High - SMBAlert pin driven high.
 *
 * @return  none
 */
void I2C_SMBusAlertConfig(uint16_t I2C_SMBusAlert)
{
    if(I2C_SMBusAlert == I2C_SMBusAlert_Low)
        R16_I2C_CTRL1 |= I2C_SMBusAlert_Low;
    else
        R16_I2C_CTRL1 &= I2C_SMBusAlert_High;
}

/*********************************************************************
 * @fn      I2C_TransmitPEC
 *
 * @brief   Enables or disables the specified I2C PEC transfer.
 *
 * @param   NewState    - ENABLE or DISABLE.
 *
 * @return  none
 */
void I2C_TransmitPEC(FunctionalState NewState)
{
    if(NewState != DISABLE)
        R16_I2C_CTRL1 |= RB_I2C_PEC;
    else
        R16_I2C_CTRL1 &= ~RB_I2C_PEC;
}

/*********************************************************************
 * @fn      I2C_PECPositionConfig
 *
 * @brief   Selects the specified I2C PEC position.
 *
 * @param   I2C_PECPosition     - specifies the PEC position.
 *                                I2C_PECPosition_Next      - indicates that the next byte is PEC.
 *                                I2C_PECPosition_Current   - indicates that current byte is PEC.
 *
 * @return  none
 */
void I2C_PECPositionConfig(uint16_t I2C_PECPosition)
{
    if(I2C_PECPosition == I2C_PECPosition_Next)
        R16_I2C_CTRL1 |= I2C_PECPosition_Next;
    else
        R16_I2C_CTRL1 &= I2C_PECPosition_Current;
}

/*********************************************************************
 * @fn      I2C_CalculatePEC
 *
 * @brief   Enables or disables the PEC value calculation of the transferred bytes.
 *
 * @param   NewState    - ENABLE or DISABLE.
 *
 * @return  none
 */
void I2C_CalculatePEC(FunctionalState NewState)
{
    if(NewState != DISABLE)
        R16_I2C_CTRL1 |= RB_I2C_ENPEC;
    else
        R16_I2C_CTRL1 &= ~RB_I2C_ENPEC;
}

/*********************************************************************
 * @fn      I2C_GetPEC
 *
 * @brief   Returns the PEC value for the specified I2C.
 *
 * @return  The PEC value.
 */
uint8_t I2C_GetPEC(void)
{
    return (R16_I2C_STAR2 >> 8);
}

/*********************************************************************
 * @fn      I2C_ARPCmd
 *
 * @brief   Enables or disables the specified I2C ARP.
 *
 * @param   NewState    - ENABLE or DISABLE.
 *
 * @return  none
 */
void I2C_ARPCmd(FunctionalState NewState)
{
    if(NewState != DISABLE)
        R16_I2C_CTRL1 |= RB_I2C_EBARP;
    else
        R16_I2C_CTRL1 &= ~RB_I2C_EBARP;
}

/*********************************************************************
 * @fn      I2C_StretchClockCmd
 *
 * @brief   Enables or disables the specified I2C Clock stretching.
 *
 * @param   NewState    - ENABLE or DISABLE.
 *
 * @return  none
 */
void I2C_StretchClockCmd(FunctionalState NewState)
{
    if(NewState == DISABLE)
        R16_I2C_CTRL1 |= RB_I2C_NOSTRETCH;
    else
        R16_I2C_CTRL1 &= ~RB_I2C_NOSTRETCH;
}

/*********************************************************************
 * @fn      I2C_FastModeDutyCycleConfig
 *
 * @brief   Selects the specified I2C fast mode duty cycle.
 *
 * @param   I2C_DutyCycle   - specifies the fast mode duty cycle.
 *                            I2C_DutyCycle_2       - I2C fast mode Tlow/Thigh = 2.
 *                            I2C_DutyCycle_16_9    - I2C fast mode Tlow/Thigh = 16/9.
 *
 * @return  none
 */
void I2C_FastModeDutyCycleConfig(uint16_t I2C_DutyCycle)
{
    if(I2C_DutyCycle != I2C_DutyCycle_16_9)
        R16_I2C_CKCFGR &= ~I2C_DutyCycle_16_9;
    else
        R16_I2C_CKCFGR |= I2C_DutyCycle_16_9;
}

/*********************************************************************
 * @fn      I2C_CheckEvent
 *
 * @brief   Checks whether the last I2Cx Event is equal to the one passed as parameter.
 *
 * @param   I2C_EVENT   - specifies the event to be checked.
 *                        I2C_EVENT_SLAVE_TRANSMITTER_ADDRESS_MATCHED           : EV1.
 *                        I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED              : EV1.
 *                        I2C_EVENT_SLAVE_TRANSMITTER_SECONDADDRESS_MATCHED     : EV1.
 *                        I2C_EVENT_SLAVE_RECEIVER_SECONDADDRESS_MATCHED        : EV1.
 *                        I2C_EVENT_SLAVE_GENERALCALLADDRESS_MATCHED            : EV1.
 *                        I2C_EVENT_SLAVE_BYTE_RECEIVED                         : EV2.
 *                        (I2C_EVENT_SLAVE_BYTE_RECEIVED | I2C_FLAG_DUALF)      : EV2.
 *                        (I2C_EVENT_SLAVE_BYTE_RECEIVED | I2C_FLAG_GENCALL)    : EV2.
 *                        I2C_EVENT_SLAVE_BYTE_TRANSMITTED                      : EV3.
 *                        (I2C_EVENT_SLAVE_BYTE_TRANSMITTED | I2C_FLAG_DUALF)   : EV3.
 *                        (I2C_EVENT_SLAVE_BYTE_TRANSMITTED | I2C_FLAG_GENCALL) : EV3.
 *                        I2C_EVENT_SLAVE_ACK_FAILURE                           : EV3_2.
 *                        I2C_EVENT_SLAVE_STOP_DETECTED                         : EV4.
 *                        I2C_EVENT_MASTER_MODE_SELECT                          : EV5.
 *                        I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED            : EV6.
 *                        I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED               : EV6.
 *                        I2C_EVENT_MASTER_BYTE_RECEIVED                        : EV7.
 *                        I2C_EVENT_MASTER_BYTE_TRANSMITTING                    : EV8.
 *                        I2C_EVENT_MASTER_BYTE_TRANSMITTED                     : EV8_2.
 *                        I2C_EVENT_MASTER_MODE_ADDRESS10                       : EV9.
 *
 * @return  1 - SUCCESS or 0 - ERROR.
 */
uint8_t I2C_CheckEvent(uint32_t I2C_EVENT)
{
    uint32_t lastevent = 0;
    uint32_t flag1 = 0, flag2 = 0;
    uint8_t  status = 0;

    flag1 = R16_I2C_STAR1;
    flag2 = R16_I2C_STAR2;
    flag2 = flag2 << 16;

    lastevent = (flag1 | flag2) & FLAG_Mask;

    if((lastevent & I2C_EVENT) == I2C_EVENT)
    {
        status = !0;
    }
    else
    {
        status = 0;
    }

    return status;
}

/*********************************************************************
 * @fn      I2C_GetLastEvent
 *
 * @brief   Returns the last I2Cx Event.
 *
 * @return  The last event.
 */
uint32_t I2C_GetLastEvent(void)
{
    uint32_t lastevent = 0;
    uint32_t flag1 = 0, flag2 = 0;

    flag1 = R16_I2C_STAR1;
    flag2 = R16_I2C_STAR2;
    flag2 = flag2 << 16;
    lastevent = (flag1 | flag2) & FLAG_Mask;

    return lastevent;
}

/*********************************************************************
 * @fn      I2C_GetFlagStatus
 *
 * @brief   Checks whether the last I2Cx Event is equal to the one passed as parameter.
 *
 * @param   I2C_FLAG    - specifies the flag to check.
 *                        I2C_FLAG_DUALF        - Dual flag (Slave mode).
 *                        I2C_FLAG_SMBHOST      - SMBus host header (Slave mode).
 *                        I2C_FLAG_SMBDEFAULT   - SMBus default header (Slave mode).
 *                        I2C_FLAG_GENCALL      - General call header flag (Slave mode).
 *                        I2C_FLAG_TRA          - Transmitter/Receiver flag.
 *                        I2C_FLAG_BUSY         - Bus busy flag.
 *                        I2C_FLAG_MSL          - Master/Slave flag.
 *                        I2C_FLAG_SMBALERT     - SMBus Alert flag.
 *                        I2C_FLAG_TIMEOUT      - Timeout or Tlow error flag.
 *                        I2C_FLAG_PECERR       - PEC error in reception flag.
 *                        I2C_FLAG_OVR          - Overrun/Underrun flag (Slave mode).
 *                        I2C_FLAG_AF           - Acknowledge failure flag.
 *                        I2C_FLAG_ARLO         - Arbitration lost flag (Master mode).
 *                        I2C_FLAG_BERR         - Bus error flag.
 *                        I2C_FLAG_TXE          - Data register empty flag (Transmitter).
 *                        I2C_FLAG_RXNE         - Data register not empty (Receiver) flag.
 *                        I2C_FLAG_STOPF        - Stop detection flag (Slave mode).
 *                        I2C_FLAG_ADD10        - 10-bit header sent flag (Master mode).
 *                        I2C_FLAG_BTF          - Byte transfer finished flag.
 *                        I2C_FLAG_ADDR         - Address sent flag (Master mode) "ADSL"
 *                                                Address matched flag (Slave mode)"ENDA".
 *                        I2C_FLAG_SB           - Start bit flag (Master mode).
 *
 * @return  FlagStatus  - SET or RESET.
 */
FlagStatus I2C_GetFlagStatus(uint32_t I2C_FLAG)
{
    FlagStatus    bitstatus = RESET;
    __IO uint32_t i2creg = 0, i2cxbase = 0;

    i2cxbase = (uint32_t)BA_I2C;
    i2creg = I2C_FLAG >> 28;
    I2C_FLAG &= FLAG_Mask;

    if(i2creg != 0)
    {
        i2cxbase += 0x14;
    }
    else
    {
        I2C_FLAG = (uint32_t)(I2C_FLAG >> 16);
        i2cxbase += 0x18;
    }

    if(((*(__IO uint32_t *)i2cxbase) & I2C_FLAG) != (uint32_t)RESET)
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }

    return bitstatus;
}

/*********************************************************************
 * @fn      I2C_ClearFlag
 *
 * @brief   Clears the I2Cx's pending flags.
 *
 * @param   I2C_FLAG    - specifies the flag to clear.
 *                        I2C_FLAG_SMBALERT     - SMBus Alert flag.
 *                        I2C_FLAG_TIMEOUT      - Timeout or Tlow error flag.
 *                        I2C_FLAG_PECERR       - PEC error in reception flag.
 *                        I2C_FLAG_OVR          - Overrun/Underrun flag (Slave mode).
 *                        I2C_FLAG_AF           - Acknowledge failure flag.
 *                        I2C_FLAG_ARLO         - Arbitration lost flag (Master mode).
 *                        I2C_FLAG_BERR         - Bus error flag.
 *
 * @return  none
 */
void I2C_ClearFlag(uint32_t I2C_FLAG)
{
    uint32_t flagpos = 0;

    flagpos = I2C_FLAG & FLAG_Mask;
    R16_I2C_STAR1 = (uint16_t)~flagpos;
}

/*********************************************************************
 * @fn      I2C_GetITStatus
 *
 * @brief   Checks whether the specified I2C interrupt has occurred or not.
 *
 * @param   II2C_IT     - specifies the interrupt source to check.
 *                        I2C_FLAG_SMBALERT     - SMBus Alert flag.
 *                        I2C_FLAG_TIMEOUT      - Timeout or Tlow error flag.
 *                        I2C_FLAG_PECERR       - PEC error in reception flag.
 *                        I2C_FLAG_OVR          - Overrun/Underrun flag (Slave mode).
 *                        I2C_FLAG_AF           - Acknowledge failure flag.
 *                        I2C_FLAG_ARLO         - Arbitration lost flag (Master mode).
 *                        I2C_FLAG_BERR         - Bus error flag.
 *                        I2C_FLAG_TXE          - Data register empty flag (Transmitter).
 *                        I2C_FLAG_RXNE         - Data register not empty (Receiver) flag.
 *                        I2C_FLAG_STOPF        - Stop detection flag (Slave mode).
 *                        I2C_FLAG_ADD10        - 10-bit header sent flag (Master mode).
 *                        I2C_FLAG_BTF          - Byte transfer finished flag.
 *                        I2C_FLAG_ADDR         - Address sent flag (Master mode) "ADSL"
 *                                                Address matched flag (Slave mode)"ENDA".
 *                        I2C_FLAG_SB           - Start bit flag (Master mode).
 *
 * @return  none
 */
ITStatus I2C_GetITStatus(uint32_t I2C_IT)
{
    ITStatus bitstatus = RESET;
    uint32_t enablestatus = 0;

    enablestatus = (uint32_t)(((I2C_IT & ITEN_Mask) >> 16) & (R16_I2C_CTRL2));
    I2C_IT &= FLAG_Mask;

    if(((R16_I2C_STAR1 & I2C_IT) != (uint32_t)RESET) && enablestatus)
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }

    return bitstatus;
}

/*********************************************************************
 * @fn      I2C_ClearITPendingBit
 *
 * @brief   Clears the I2Cx interrupt pending bits.
 *
 * @param   I2C_IT      - specifies the interrupt pending bit to clear.
 *                        I2C_IT_SMBALERT   - SMBus Alert interrupt.
 *                        I2C_IT_TIMEOUT    - Timeout or Tlow error interrupt.
 *                        I2C_IT_PECERR     - PEC error in reception  interrupt.
 *                        I2C_IT_OVR        - Overrun/Underrun interrupt (Slave mode).
 *                        I2C_IT_AF         - Acknowledge failure interrupt.
 *                        I2C_IT_ARLO       - Arbitration lost interrupt (Master mode).
 *                        I2C_IT_BERR       - Bus error interrupt.
 *
 * @return  none
 */
void I2C_ClearITPendingBit(uint32_t I2C_IT)
{
    uint32_t flagpos = 0;

    flagpos = I2C_IT & FLAG_Mask;
    R16_I2C_STAR1 = (uint16_t)~flagpos;
}
