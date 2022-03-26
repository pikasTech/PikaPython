#include "CH58x_common.h"
#include "CH582_IIC.h"


void CH582_IIC_platformDisable(PikaObj *self)
{
    I2C_Cmd(DISABLE);
}

void CH582_IIC_platformEnable(PikaObj *self)
{
    uint8_t deviceAddr = obj_getInt(self, "deviceAddr");
    GPIOB_ModeCfg(GPIO_Pin_13 | GPIO_Pin_12, GPIO_ModeIN_PU);
    I2C_Init(I2C_Mode_I2C, 100000, I2C_DutyCycle_16_9, I2C_Ack_Enable, I2C_AckAddr_7bit, deviceAddr);
    I2C_Cmd(ENABLE);
}

void CH582_IIC_platformRead(PikaObj *self)
{
    uint8_t length = obj_getInt(self, "length");
    uint8_t readAddr = obj_getInt(self, "readAddr");
    uint8_t i = 0;
    uint8_t *data = {0};

    I2C_AcknowledgeConfig(ENABLE);
    I2C_GenerateSTART(ENABLE);
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT))
        ;
    I2C_Send7bitAddress(readAddr, I2C_Direction_Receiver);
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
        ;
    while (i < length)
    {
        if (I2C_GetFlagStatus(I2C_FLAG_RXNE) != RESET)
        {
            if (i == (length - 2))
            {
                I2C_AcknowledgeConfig(DISABLE);
                data[i] = I2C_ReceiveData(); //读数据,发送nACK
            }
            else
            {
                data[i] = I2C_ReceiveData(); //读数据,发送ACK
            }
            i++;
        }
    }
    I2C_GenerateSTOP(ENABLE);
    obj_setStr(self, "readData", (char *)data);
}

void CH582_IIC_platformWrite(PikaObj *self)
{
    char *writeData = obj_getStr(self, "writeData");
    uint8_t writeAddr = obj_getInt(self, "writeAddr");
    uint8_t i = 0;
    uint8_t length = strGetSize(writeData);

    while (I2C_GetFlagStatus(I2C_FLAG_BUSY) != RESET)
        ;
    I2C_GenerateSTART(ENABLE);
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT))
        ;
    I2C_Send7bitAddress(writeAddr, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
        ;
    while (i < length)
    {
        if (I2C_GetFlagStatus(I2C_FLAG_TXE) != RESET)
        {
            I2C_SendData((uint8_t)writeData[i]);
            i++;
        }
    }
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_BYTE_TRANSMITTED))
        ;
    I2C_GenerateSTOP(ENABLE);
}