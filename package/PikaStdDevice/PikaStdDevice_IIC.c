#include "PikaStdDevice_IIC.h"
#include "STM32_GPIO.h"
#include "BaseObj.h"
#include "PikaObj.h"
void PikaStdDevice_IIC_init(PikaObj *self){
    obj_run(self, "deviceAddr = 0");
    obj_run(self, "SCLpin = 'PA0'");
    obj_run(self, "SDApin = 'PA1'");
}

void PikaStdDevice_IIC_enable(PikaObj *self){
    obj_run(self, "platformEnable()");
}

void PikaStdDevice_IIC_setDeviceAddr(PikaObj *self, int addr){
    obj_setInt(self, "deviceAddr", addr);
}

void PikaStdDevice_IIC_setPinSCL(PikaObj *self, char * pin){
    obj_setStr(self, "SCLpin", pin);
}

void PikaStdDevice_IIC_setPinSDA(PikaObj *self, char * pin){
    obj_setStr(self, "SDApin", pin);
}

void PikaStdDevice_IIC_write(PikaObj *self, int addr, char * data){
    obj_setStr(self, "writeData", data);
    obj_setInt(self, "writeAddr", addr);
    obj_run(self, "platformWrite(writeAddr, writeData)");
}

char * PikaStdDevice_IIC_read(PikaObj *self, int addr, int length){
    obj_setInt(self, "length", length);
    obj_setInt(self, "readAddr", addr);
    obj_run(self, "readData = platformRead(readAddr, length)");
    return obj_getStr(self, "readData");
}

void PikaStdDevice_IIC_platformWrite(PikaObj *self, int addr, char * data){
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error] platform method need to be override.");
}

void PikaStdDevice_IIC_platformEnable(PikaObj *self){
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error] platform method need to be override.");
}

char * PikaStdDevice_IIC_platformRead(PikaObj *self, int addr, int length){
    obj_setErrorCode(self, 1);
    obj_setSysOut(self, "[error] platform method need to be override.");
    return NULL;
}
