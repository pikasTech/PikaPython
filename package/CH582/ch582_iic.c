#include "CH58x_common.h"
#include "CH582_IIC.h"

typedef struct pika_IIC_info_t
{
    uint8_t deviceAddr;
    uint8_t readBuff[32];
} pika_IIC_info;

void CH582_IIC_platformDisable(PikaObj *self)
{
    I2C_Cmd(DISABLE);
}

void CH582_IIC_platformEnable(PikaObj *self)
{
    uint8_t deviceAddr = obj_getInt(self, "deviceAddr");
    pika_IIC_info *iic = obj_getPtr(self, "iic");
    if (NULL == iic)
    {
        iic = pikaMalloc(sizeof(pika_IIC_info));
        obj_setPtr(self, "iic", iic);
    }
    iic->deviceAddr = deviceAddr;

    I2C_SoftwareResetCmd(ENABLE);
    I2C_SoftwareResetCmd(DISABLE);
    GPIOB_ModeCfg(GPIO_Pin_13 | GPIO_Pin_12, GPIO_ModeIN_PU);
    I2C_Init(I2C_Mode_I2C, 100000, I2C_DutyCycle_16_9, I2C_Ack_Enable, I2C_AckAddr_7bit, 0x01);
    I2C_Cmd(ENABLE);
}

void CH582_IIC_platformRead(PikaObj *self)
{
    uint8_t len = obj_getInt(self, "length");
    uint8_t reg = obj_getInt(self, "readAddr");
    pika_IIC_info *iic = obj_getPtr(self, "iic");
    uint8_t i = 0;

    I2C_AcknowledgeConfig(ENABLE);
    I2C_GenerateSTART(ENABLE);
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT))
        ;
    I2C_Send7bitAddress((iic->deviceAddr << 1) | 0x00, I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
        ;
    I2C_SendData(reg);
    I2C_GenerateSTART(ENABLE);
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT))
        ;
    I2C_Send7bitAddress(((iic->deviceAddr << 1) | 0x01), I2C_Direction_Receiver);
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
        ;
    if (len == 1)
    {
        I2C_AcknowledgeConfig(DISABLE);
        iic->readBuff[i] = I2C_ReceiveData();
    }
    else
    {
        while (i < len)
        {
            if (I2C_GetFlagStatus(I2C_FLAG_RXNE) != RESET)
            {
                if (i == (len - 2))
                {
                    I2C_AcknowledgeConfig(DISABLE);
                    iic->readBuff[i] = I2C_ReceiveData();
                }
                else
                {
                    iic->readBuff[i] = I2C_ReceiveData();
                }
                i++;
            }
        }
    }
    I2C_GenerateSTOP(ENABLE);

    obj_setBytes(self, "readData", iic->readBuff, len);
}

void CH582_IIC_platformWrite(PikaObj *self)
{
    char *writeData = NULL;
    size_t len = obj_loadBytes(self, "writeData", writeData);
    uint8_t reg = obj_getInt(self, "writeAddr");
    pika_IIC_info *iic = obj_getPtr(self, "iic");
    uint8_t i = 0;

    I2C_AcknowledgeConfig(ENABLE);
    I2C_GenerateSTART(ENABLE);
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_MODE_SELECT))
        ;
    I2C_Send7bitAddress(((iic->deviceAddr << 1) | 0), I2C_Direction_Transmitter);
    while (!I2C_CheckEvent(I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
        ;
    while (I2C_GetFlagStatus(I2C_FLAG_TXE) == RESET)
        ;
    I2C_SendData(reg);
    while (i < len)
    {
        if (I2C_GetFlagStatus(I2C_FLAG_TXE) != RESET)
        {
            I2C_SendData(writeData[i]);
            i++;
        }
    }
    while (I2C_GetFlagStatus(I2C_FLAG_TXE) == RESET)
        ;
    I2C_GenerateSTOP(ENABLE);
}