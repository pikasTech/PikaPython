#include "pikaScript.h"

#include "hal_gpio.h"
#include "hal_rcc.h"

void MM32F5277E9P_GPIO_High(PikaObj *self)
{
    char* Port = obj_getStr(self, "Port");
    uint16_t pins = obj_getInt(self, "Pin");
    switch(*Port){
        case 'A':GPIO_WriteBit(GPIOA, 1 << pins, 1u);break;
        case 'B':GPIO_WriteBit(GPIOB, 1 << pins, 1u);break;
        case 'C':GPIO_WriteBit(GPIOC, 1 << pins, 1u);break;
        case 'D':GPIO_WriteBit(GPIOD, 1 << pins, 1u);break;
        case 'E':GPIO_WriteBit(GPIOE, 1 << pins, 1u);break;
        case 'F':GPIO_WriteBit(GPIOF, 1 << pins, 1u);break;
        case 'G':GPIO_WriteBit(GPIOG, 1 << pins, 1u);break;
        case 'H':GPIO_WriteBit(GPIOH, 1 << pins, 1u);break;
        case 'I':GPIO_WriteBit(GPIOI, 1 << pins, 1u);break;
    }
}

void MM32F5277E9P_GPIO_Low(PikaObj *self)
{
    
    char* Port = obj_getStr(self, "Port");
    uint16_t pins = obj_getInt(self, "Pin");
    switch(*Port){
        case 'A':GPIO_WriteBit(GPIOA, 1 << pins, 0u);break;
        case 'B':GPIO_WriteBit(GPIOB, 1 << pins, 0u);break;
        case 'C':GPIO_WriteBit(GPIOC, 1 << pins, 0u);break;
        case 'D':GPIO_WriteBit(GPIOD, 1 << pins, 0u);break;
        case 'E':GPIO_WriteBit(GPIOE, 1 << pins, 0u);break;
        case 'F':GPIO_WriteBit(GPIOF, 1 << pins, 0u);break;
        case 'G':GPIO_WriteBit(GPIOG, 1 << pins, 0u);break;
        case 'H':GPIO_WriteBit(GPIOH, 1 << pins, 0u);break;
        case 'I':GPIO_WriteBit(GPIOI, 1 << pins, 0u);break;
    }
}
void MM32F5277E9P_GPIO___init__(PikaObj *self, char* PinMode, char* Speed, char* name)
{
    GPIO_Init_Type gpio_init;
    GPIO_Type* GPIOx;
    uint8_t pin;

    if('P' != *name){
        printf("\r\nNo Pin!\r\n");
        return;
    }

    do{ // 设置引脚
    
            if('\0' == name[3])
            {
                    pin = name[2] - '0';
            }
            else if('\0' == name[4])
            {
                    pin = ((name[2] - '0') * 10) + (name[3] - '0');
            }
            else{
                    printf("\r\nPinName Error! Refer to 'PA9' or 'PA12'.\r\n");
                    return;
            }
            
            if(pin > 15){
                return;
            }
            gpio_init.Pins = 1 << pin;
            obj_setInt(self, "Pin", pin);
            

            switch(name[1]){
                case 'A':
                    GPIOx = GPIOA;
                    obj_setStr(self, "Port", "A");
                    RCC_EnableAHB1Periphs(RCC_AHB1_PERIPH_GPIOA, true);
                    RCC_ResetAHB1Periphs(RCC_AHB1_PERIPH_GPIOA);
                    break;
                case 'B':
                    GPIOx = GPIOB;
                    obj_setStr(self, "Port", "B");
                    RCC_EnableAHB1Periphs(RCC_AHB1_PERIPH_GPIOB, true);
               //     RCC_ResetAHB1Periphs(RCC_AHB1_PERIPH_GPIOB);
                    break;
                case 'C':
                    GPIOx = GPIOC;
                    obj_setStr(self, "Port", "C");
                    RCC_EnableAHB1Periphs(RCC_AHB1_PERIPH_GPIOC, true);
                    RCC_ResetAHB1Periphs(RCC_AHB1_PERIPH_GPIOC);
                    break;
                case 'D':
                    GPIOx = GPIOD;
                    obj_setStr(self, "Port", "D");
                    RCC_EnableAHB1Periphs(RCC_AHB1_PERIPH_GPIOD, true);
                    RCC_ResetAHB1Periphs(RCC_AHB1_PERIPH_GPIOD);
                    break;
                case 'E':
                    GPIOx = GPIOE;
                    obj_setStr(self, "Port", "E");
                    RCC_EnableAHB1Periphs(RCC_AHB1_PERIPH_GPIOE, true);
                    RCC_ResetAHB1Periphs(RCC_AHB1_PERIPH_GPIOE);
                    break;
                case 'F':
                    GPIOx = GPIOF;
                    obj_setStr(self, "Port", "F");
                    RCC_EnableAHB1Periphs(RCC_AHB1_PERIPH_GPIOF, true);
                    RCC_ResetAHB1Periphs(RCC_AHB1_PERIPH_GPIOF);
                    break;
                case 'G':
                    GPIOx = GPIOG;
                    obj_setStr(self, "Port", "G");
                    RCC_EnableAHB1Periphs(RCC_AHB1_PERIPH_GPIOG, true);
                    RCC_ResetAHB1Periphs(RCC_AHB1_PERIPH_GPIOG);
                    break;
                case 'H':
                    GPIOx = GPIOH;
                    obj_setStr(self, "Port", "H");
                    RCC_EnableAHB1Periphs(RCC_AHB1_PERIPH_GPIOH, true);
                    RCC_ResetAHB1Periphs(RCC_AHB1_PERIPH_GPIOH);
                    break;
                case 'I':
                    GPIOx = GPIOI;
                    obj_setStr(self, "Port", "I");
                    RCC_EnableAHB1Periphs(RCC_AHB1_PERIPH_GPIOI, true);
                    RCC_ResetAHB1Periphs(RCC_AHB1_PERIPH_GPIOI);
                    break;
                default:printf("\r\n'Pxnn' x is 'A' - 'I'\r\n");
                return;
            }
            
           printf("\r\n%s\r\n", name);
    }while(0);
    
    do{ // 设置模式
            if(0 == strcmp(PinMode, "IN")){
                gpio_init.PinMode = GPIO_PinMode_In_PullUp;
            }
            else if(0 == strcmp(PinMode, "OUT")){
                gpio_init.PinMode = GPIO_PinMode_Out_PushPull;
            }
            else{
                printf("\r\nMode Error!\r\n");
            }
    }while(0);
    
    do{ // 设置速度
            if(0 == strncmp(Speed, "50", 2)){
                    gpio_init.Speed = GPIO_Speed_50MHz;
            }
            else if(0 == strncmp(Speed, "20", 2)){
                    gpio_init.Speed = GPIO_Speed_20MHz;
            }
            else if(0 == strncmp(Speed, "10", 2)){
                    gpio_init.Speed = GPIO_Speed_10MHz;
            }
            else{
                printf("\r\nSpeed Error!\r\n");
            }
    }while(0);

    GPIO_Init(GPIOx, &gpio_init);
    
}

int MM32F5277E9P_GPIO_Read(PikaObj *self)
{
	char* Port = obj_getStr(self, "Port");
    uint16_t pins = obj_getInt(self, "Pin");
    switch(*Port){
        case 'A':return (int)GPIO_ReadInDataBit(GPIOA, 1 << pins);break;
        case 'B':return (int)GPIO_ReadInDataBit(GPIOB, 1 << pins);break;
        case 'C':return (int)GPIO_ReadInDataBit(GPIOC, 1 << pins);break;
        case 'D':return (int)GPIO_ReadInDataBit(GPIOD, 1 << pins);break;
        case 'E':return (int)GPIO_ReadInDataBit(GPIOE, 1 << pins);break;
        case 'F':return (int)GPIO_ReadInDataBit(GPIOF, 1 << pins);break;
        case 'G':return (int)GPIO_ReadInDataBit(GPIOG, 1 << pins);break;
        case 'H':return (int)GPIO_ReadInDataBit(GPIOH, 1 << pins);break;
        case 'I':return (int)GPIO_ReadInDataBit(GPIOI, 1 << pins);break;
    }
		return -1;
}
void MM32F5277E9P_GPIO_Write(PikaObj *self, int state)
{
    char* Port = obj_getStr(self, "Port");
    uint16_t pins = obj_getInt(self, "Pin");
    switch(*Port){
        case 'A':GPIO_WriteBit(GPIOA, 1 << pins, !!state);break;
        case 'B':GPIO_WriteBit(GPIOB, 1 << pins, !!state);break;
        case 'C':GPIO_WriteBit(GPIOC, 1 << pins, !!state);break;
        case 'D':GPIO_WriteBit(GPIOD, 1 << pins, !!state);break;
        case 'E':GPIO_WriteBit(GPIOE, 1 << pins, !!state);break;
        case 'F':GPIO_WriteBit(GPIOF, 1 << pins, !!state);break;
        case 'G':GPIO_WriteBit(GPIOG, 1 << pins, !!state);break;
        case 'H':GPIO_WriteBit(GPIOH, 1 << pins, !!state);break;
        case 'I':GPIO_WriteBit(GPIOI, 1 << pins, !!state);break;
    }
}
void MM32F5277E9P_GPIO_Toggle(PikaObj *self)
{
    MM32F5277E9P_GPIO_Write(self, !MM32F5277E9P_GPIO_Read(self));
}