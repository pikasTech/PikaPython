#include <stdint.h>
#include "BaseObj.h"
#include "dataStrs.h"
#include "W801Device_IIC.h"
#include "W801_common.h"
#include "wm_hal.h"

//I2C_HandleTypeDef hi2c;

void W801Device_IIC_platformEnable(PikaObj *self)
{
	char* SCLpin = obj_getStr(self, "SCLpin");
    char* SDApin = obj_getStr(self, "SDApin");
    uint8_t deviceAddr = obj_getInt(self, "deviceAddr");

    I2C_HandleTypeDef* iic = obj_getPtr(self, "iic");
    if (NULL == iic) {
        iic = pikaMalloc(sizeof(I2C_HandleTypeDef));
        obj_setPtr(self, "iic", iic);
    }
    iic->SDA_Port = GPIO_get_Group(SDApin);
    iic->SDA_Pin = GPIO_get_pin(SDApin);
    iic->SCL_Port = GPIO_get_Group(SCLpin);
    iic->SCL_Pin = GPIO_get_pin(SCLpin);
    iic->deviceAddr = deviceAddr;
	HAL_I2C_Init(iic);
}

void W801Device_IIC_platformRead(PikaObj* self)
{
	int addr = obj_getInt(self, "addr");
    int length = obj_getInt(self, "length");
    I2C_HandleTypeDef* iic = obj_getPtr(self, "iic");
	HAL_I2C_Read(iic,iic->deviceAddr,addr,(uint8_t *)iic->readBuffer,length);    
    obj_setStr(self, "readData", (char*)iic->readBuffer);
}

void W801Device_IIC_platformWrite(PikaObj* self)
{
	int addr = obj_getInt(self, "addr");
    char* data = obj_getStr(self, "data");
    I2C_HandleTypeDef* iic = obj_getPtr(self, "iic");
    HAL_I2C_Write(iic, iic->deviceAddr, addr, (uint8_t*)data,strGetSize(data));
}