#include "BL808_GPIO.h"
#include <bl808_glb.h>
#include <stdio.h>

void BL808_GPIO_platformDisable(PikaObj* self) {}

uint32_t getPinMode(char* mode) {
    if (strEqu(mode, "out")) {
        return GPIO_MODE_OUTPUT;
    }
    if (strEqu(mode, "in")) {
        return GPIO_MODE_INPUT;
    }
    return NULL;
}

uint32_t getPinNum(char* pin) {
    /* xx */
    char* pin_num = pin + 1;
    int pin_num_int = atoi(pin_num);
    return pin_num_int;
}

void BL808_GPIO_platformEnable(PikaObj* self) {
    /* Pxx */
    char* pin = obj_getStr(self, "pin");
    char* mode = obj_getStr(self, "mode");
    printf("[  Info]: pin: %s, mode: %s, enable...\r\n", pin, mode);
    // char *pull = obj_getStr(self, "pull");

    GLB_GPIO_Cfg_Type cfg;
    cfg.drive = 0;
    cfg.smtCtrl = 1;
    cfg.gpioFun = GPIO_FUN_GPIO;
    cfg.outputMode = 0;
    cfg.pullType = GPIO_PULL_NONE;

    cfg.gpioPin = getPinNum(pin);
    cfg.gpioMode = getPinMode(mode);
    GLB_GPIO_Init(&cfg);
}

void BL808_GPIO_platformHigh(PikaObj* self) {
    char* pin = obj_getStr(self, "pin");
    printf("[  Info]: pin: %s set high...\r\n", pin);
    GLB_GPIO_Write(getPinNum(pin), true);
}

void BL808_GPIO_platformLow(PikaObj* self) {
    char* pin = obj_getStr(self, "pin");
    printf("[  Info]: pin: %s set low...\r\n", pin);
    GLB_GPIO_Write(getPinNum(pin), false);
}

void BL808_GPIO_platformRead(PikaObj* self) {
    char* pin = obj_getStr(self, "pin");
    obj_setInt(self, "readBuff", GLB_GPIO_Read(getPinNum(pin)));
}

void BL808_GPIO_platformSetMode(PikaObj* self) {
    char* pin = obj_getStr(self, "pin");
    char* mode = obj_getStr(self, "mode");

    GLB_GPIO_Cfg_Type cfg;
    cfg.drive = 0;
    cfg.smtCtrl = 1;
    cfg.gpioFun = GPIO_FUN_GPIO;
    cfg.outputMode = 0;
    cfg.pullType = GPIO_PULL_NONE;

    cfg.gpioPin = getPinNum(pin);
    cfg.gpioMode = getPinMode(mode);
    GLB_GPIO_Init(&cfg);
}
