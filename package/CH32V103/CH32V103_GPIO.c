#include "CH32V103_GPIO.h"
#include "debug.h"


GPIO_TypeDef *get_GPIO_group(char *pin){
    if(strIsStartWith(pin, "PA")){
        return GPIOA;
    }
    if(strIsStartWith(pin, "PB")){
        return GPIOB;
    }
    if(strIsStartWith(pin, "PC")){
        return GPIOC;
    }
    if(strIsStartWith(pin, "PD")){
        return GPIOD;
    }
    return NULL;
}

uint16_t get_GPIO_pin(char *pin){
    pin = pin + 2;
    if (strEqu(pin, "0")) {
        return GPIO_Pin_0;
    }
    if (strEqu(pin, "1")) {
        return GPIO_Pin_1;
    }
    if (strEqu(pin, "2")) {
        return GPIO_Pin_2;
    }
    if (strEqu(pin, "3")) {
        return GPIO_Pin_3;
    }
    if (strEqu(pin, "4")) {
        return GPIO_Pin_4;
    }
    if (strEqu(pin, "5")) {
        return GPIO_Pin_5;
    }
    if (strEqu(pin, "6")) {
        return GPIO_Pin_6;
    }
    if (strEqu(pin, "7")) {
        return GPIO_Pin_7;
    }
    if (strEqu(pin, "8")) {
        return GPIO_Pin_8;
    }
    if (strEqu(pin, "9")) {
        return GPIO_Pin_9;
    }
    if (strEqu(pin, "10")) {
        return GPIO_Pin_10;
    }
    if (strEqu(pin, "11")) {
        return GPIO_Pin_11;
    }
    if (strEqu(pin, "12")) {
        return GPIO_Pin_12;
    }
    if (strEqu(pin, "13")) {
        return GPIO_Pin_13;
    }
    if (strEqu(pin, "14")) {
        return GPIO_Pin_14;
    }
    if (strEqu(pin, "15")) {
        return GPIO_Pin_15;
    }
    return 0;
}

GPIOMode_TypeDef get_GPIO_mode(char *mode){
    if(strEqu(mode, "out")){
        return GPIO_Mode_Out_PP;
    }
    if(strEqu(mode, "in")){
        return GPIO_Mode_IN_FLOATING;
    }
    return GPIO_Mode_Out_PP;
}

uint32_t get_GPIO_Clock_group(char *pin){
    if(strIsStartWith(pin, "PA")){
        return RCC_APB2Periph_GPIOA;
    }
    if(strIsStartWith(pin, "PB")){
        return RCC_APB2Periph_GPIOB;
    }
    if(strIsStartWith(pin, "PC")){
        return RCC_APB2Periph_GPIOC;
    }
    if(strIsStartWith(pin, "PD")){
        return RCC_APB2Periph_GPIOD;
    }
    return 0;
}

void CH32V103_GPIO_platformDisable(PikaObj *self){}
void CH32V103_GPIO_platformEnable(PikaObj *self){
    char *pin = obj_getStr(self, "pin");
    char *mode = obj_getStr(self, "mode");
    GPIO_TypeDef *GPIO_group = get_GPIO_group(pin);
    uint16_t GPIO_pin = get_GPIO_pin(pin);
    GPIOMode_TypeDef GPIO_mode = get_GPIO_mode(mode);
    uint32_t GPIO_clock_group = get_GPIO_Clock_group(pin);

    GPIO_InitTypeDef  GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(GPIO_clock_group,ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_mode;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIO_group, &GPIO_InitStructure);
}
void CH32V103_GPIO_platformHigh(PikaObj *self){
    char *pin = obj_getStr(self, "pin");
    GPIO_TypeDef *GPIO_group = get_GPIO_group(pin);
    uint16_t GPIO_pin = get_GPIO_pin(pin);

    GPIO_WriteBit(GPIO_group, GPIO_pin, Bit_SET);
}
void CH32V103_GPIO_platformLow(PikaObj *self){
    char *pin = obj_getStr(self, "pin");
    GPIO_TypeDef *GPIO_group = get_GPIO_group(pin);
    uint16_t GPIO_pin = get_GPIO_pin(pin);

    GPIO_WriteBit(GPIO_group, GPIO_pin, Bit_RESET);
}
void CH32V103_GPIO_platformSetMode(PikaObj *self){
    char *mode = obj_getStr(self, "mode");
    char *pin = obj_getStr(self, "pin");
    GPIO_TypeDef *GPIO_group = get_GPIO_group(pin);
    uint16_t GPIO_pin = get_GPIO_pin(pin);
    GPIOMode_TypeDef GPIO_mode = get_GPIO_mode(mode);

    GPIO_InitTypeDef  GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_mode;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIO_group, &GPIO_InitStructure);
}
