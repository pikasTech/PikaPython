#include "PikaPiZero_RGB.h"
#include <stdint.h>
#include "BaseObj.h"
#include "STM32_common.h"
#include "dataStrs.h"
#include "RGB_ASM.h"

void RGB_reset(){
    GPIOB->BRR = GPIO_PIN_12;     // reset
    delay_us(50);
}

#define RED         0x000F00
#define GREEN       0x0F0000
#define BLUE        0x00000F
#define WHITE       0x0F0F0F
#define CUTDOWN     0x000000

void RGB_setVoid(){
    __asm(
        "nop"
    );
}



void PikaPiZero_RGB_enable(PikaObj *self){
    obj_run(self, "pin.init()");
    obj_run(self, "pin.setPin('PB12')");
    obj_run(self, "pin.setMode('out')");
    obj_run(self, "pin.enable()");
}
void PikaPiZero_RGB_init(PikaObj *self){
    
}

void PikaPiZero_RGB_red(PikaObj *self){
    RGB_set(RED);    
    RGB_set(RED);
    RGB_set(RED);
    RGB_set(RED);
}

void PikaPiZero_RGB_blue(PikaObj *self){
    RGB_set(BLUE);    
    RGB_set(BLUE);
    RGB_set(BLUE);
    RGB_set(BLUE);
}

void PikaPiZero_RGB_green(PikaObj *self){
    RGB_set(GREEN);    
    RGB_set(GREEN);
    RGB_set(GREEN);
    RGB_set(GREEN);
}

void PikaPiZero_RGB_white(PikaObj *self){
    RGB_set(WHITE);    
    RGB_set(WHITE);
    RGB_set(WHITE);
    RGB_set(WHITE);
}