#include <stdint.h>
#include "BaseObj.h"
#include "STM32F4_common.h"
#include "dataStrs.h"

typedef struct pika_IIC_info_t {
    GPIO_TypeDef* SCL_GPIO;
    GPIO_TypeDef* SDA_GPIO;

    uint32_t SCL_GPIO_Pin;
    uint32_t SDA_GPIO_Pin;
    uint8_t deviceAddr;

    uint8_t readBuff[32];
} pika_IIC_info;

#define delay_rate 1

void SDA_OUT(pika_IIC_info* iic) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    /*Configure GPIO*/
    GPIO_InitStruct.Pin = iic->SDA_GPIO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(iic->SDA_GPIO, &GPIO_InitStruct);
}

void SDA_IN(pika_IIC_info* iic) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    /*Configure GPIO*/
    GPIO_InitStruct.Pin = iic->SDA_GPIO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(iic->SDA_GPIO, &GPIO_InitStruct);
}

void IIC_SDA_high(pika_IIC_info* iic) {
    HAL_GPIO_WritePin(iic->SDA_GPIO, iic->SDA_GPIO_Pin, GPIO_PIN_SET);
}

void IIC_SDA_low(pika_IIC_info* iic) {
    HAL_GPIO_WritePin(iic->SDA_GPIO, iic->SDA_GPIO_Pin, GPIO_PIN_RESET);
}

void IIC_SCL_high(pika_IIC_info* iic) {
    HAL_GPIO_WritePin(iic->SCL_GPIO, iic->SCL_GPIO_Pin, GPIO_PIN_SET);
}

void IIC_SCL_low(pika_IIC_info* iic) {
    HAL_GPIO_WritePin(iic->SCL_GPIO, iic->SCL_GPIO_Pin, GPIO_PIN_RESET);
}

uint8_t READ_SDA(pika_IIC_info* iic) {
    return HAL_GPIO_ReadPin(iic->SDA_GPIO, iic->SDA_GPIO_Pin);
}

void WRITE_SDA(pika_IIC_info* iic, uint8_t data) {
    HAL_GPIO_WritePin(iic->SDA_GPIO, iic->SDA_GPIO_Pin, (GPIO_PinState) data);
}

void IIC_Start(pika_IIC_info* iic) {
    SDA_OUT(iic);
    IIC_SDA_high(iic);
    IIC_SCL_high(iic);
    delay_us(delay_rate * 4);
    IIC_SDA_low(iic);
    delay_us(delay_rate * 4);
    IIC_SCL_low(iic);
}

void IIC_Stop(pika_IIC_info* iic) {
    SDA_OUT(iic);
    IIC_SCL_low(iic);
    IIC_SDA_low(iic);
    delay_us(delay_rate * 4);
    IIC_SCL_high(iic);
    IIC_SDA_high(iic);
    delay_us(delay_rate * 4);
}

uint8_t IIC_Wait_Ack(pika_IIC_info* iic) {
    uint8_t ucErrTime = 0;
    SDA_IN(iic);

    IIC_SDA_high(iic);
    delay_us(delay_rate * 1);
    IIC_SCL_high(iic);
    delay_us(delay_rate * 1);
    while (READ_SDA(iic)) {
        ucErrTime++;
        if (ucErrTime > 250) {
            IIC_Stop(iic);
            return 1;
        }
    }
    IIC_SCL_low(iic);
    return 0;
}

void IIC_Ack(pika_IIC_info* iic) {
    IIC_SCL_low(iic);
    SDA_OUT(iic);

    IIC_SDA_low(iic);
    delay_us(delay_rate * 2);
    IIC_SCL_high(iic);
    delay_us(delay_rate * 2);
    IIC_SCL_low(iic);
}

void IIC_NAck(pika_IIC_info* iic) {
    IIC_SCL_low(iic);
    SDA_OUT(iic);

    IIC_SDA_high(iic);
    delay_us(delay_rate * 2);
    IIC_SCL_high(iic);
    delay_us(delay_rate * 2);
    IIC_SCL_low(iic);
}

void IIC_Send_Byte(pika_IIC_info* iic, uint8_t txd) {
    uint8_t t;
    SDA_OUT(iic);

    IIC_SCL_low(iic);
    for (t = 0; t < 8; t++) {
        WRITE_SDA(iic, (txd & 0x80) >> 7);
        txd <<= 1;
        delay_us(delay_rate * 2);
        IIC_SCL_high(iic);
        delay_us(delay_rate * 2);
        IIC_SCL_low(iic);
        delay_us(delay_rate * 2);
    }
}

uint8_t IIC_Read_Byte(pika_IIC_info* iic, unsigned char ack) {
    unsigned char i, receive = 0;
    SDA_IN(iic);

    for (i = 0; i < 8; i++) {
        IIC_SCL_low(iic);
        delay_us(delay_rate * 2);
        IIC_SCL_high(iic);
        receive <<= 1;
        if (READ_SDA(iic))
            receive++;
        delay_us(delay_rate * 1);
    }
    if (!ack)
        IIC_NAck(iic);
    else
        IIC_Ack(iic);
    return receive;
}

uint8_t MPU_Write_Len(pika_IIC_info* iic,
                      uint8_t addr,
                      uint8_t reg,
                      uint8_t len,
                      uint8_t* buf) {
    uint8_t i;
    IIC_Start(iic);
    IIC_Send_Byte(iic, (addr << 1) | 0);
    if (IIC_Wait_Ack(iic)) {
        IIC_Stop(iic);
        return 1;
    }
    IIC_Send_Byte(iic, reg);
    IIC_Wait_Ack(iic);
    for (i = 0; i < len; i++) {
        IIC_Send_Byte(iic, buf[i]);
        if (IIC_Wait_Ack(iic)) {
            IIC_Stop(iic);
            return 1;
        }
    }
    IIC_Stop(iic);
    return 0;
}

uint8_t MPU_Read_Len(pika_IIC_info* iic,
                     uint8_t addr,
                     uint8_t reg,
                     uint8_t len,
                     uint8_t* buf) {
    IIC_Start(iic);
    IIC_Send_Byte(iic, (addr << 1) | 0);
    if (IIC_Wait_Ack(iic)) {
        IIC_Stop(iic);
        return 1;
    }
    IIC_Send_Byte(iic, reg);
    IIC_Wait_Ack(iic);
    IIC_Start(iic);
    IIC_Send_Byte(iic, (addr << 1) | 1);
    IIC_Wait_Ack(iic);
    while (len) {
        if (len == 1)
            *buf = IIC_Read_Byte(iic, 0);
        else
            *buf = IIC_Read_Byte(iic, 1);
        len--;
        buf++;
    }
    IIC_Stop(iic);
    return 0;
}

void STM32F4_IIC_platformEnable(PikaObj* self) {
    obj_run(self, "SCL.init()");
    obj_run(self, "SDA.init()");
    obj_run(self, "SCL.setPin(SCLpin)");
    obj_run(self, "SDA.setPin(SDApin)");
    obj_run(self, "SCL.setMode('out')");
    obj_run(self, "SDA.setMode('out')");
    obj_run(self, "SCL.enable()");
    obj_run(self, "SDA.enable()");
    obj_run(self, "SCL.low()");
    obj_run(self, "SDA.low()");
    char* SCLpin = obj_getStr(self, "SCLpin");
    char* SDApin = obj_getStr(self, "SDApin");
    uint8_t deviceAddr = obj_getInt(self, "deviceAddr");

    pika_IIC_info* iic = obj_getPtr(self, "iic");
    if (NULL == iic) {
        iic = pikaMalloc(sizeof(pika_IIC_info));
        obj_setPtr(self, "iic", iic);
    }
    iic->SDA_GPIO = getGpioPort(SDApin);
    iic->SDA_GPIO_Pin = getGpioPin(SDApin);
    iic->SCL_GPIO = getGpioPort(SCLpin);
    iic->SCL_GPIO_Pin = getGpioPin(SCLpin);
    iic->deviceAddr = deviceAddr;
    SDA_OUT(iic);
}

void STM32F4_IIC_platformRead(PikaObj* self) {
    int addr = obj_getInt(self, "addr");
    int length = obj_getInt(self, "length");
    pika_IIC_info* iic = obj_getPtr(self, "iic");

    MPU_Read_Len(iic, iic->deviceAddr, addr, length, (uint8_t*)iic->readBuff);
    obj_setStr(self, "readData", (char*)iic->readBuff);
}

void STM32F4_IIC_platformWrite(PikaObj* self) {
    int addr = obj_getInt(self, "addr");
    char* data = obj_getStr(self, "writeData");
    pika_IIC_info* iic = obj_getPtr(self, "iic");

    MPU_Write_Len(iic, iic->deviceAddr, addr, strGetSize(data), (uint8_t*)data);
}
