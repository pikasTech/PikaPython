#include "PikaPiZero_RGB.h"
#include <stdint.h>
#include "BaseObj.h"
#include "STM32_common.h"
#include "dataStrs.h"

void RGB_setUp(){
    GPIOB->BSRR = GPIO_PIN_12;    // set
    int i;    
    i++;
    i++;
    i++;                         
    i++;
    i++;
    i++;                         
    i++;                             
    i++;                             
    GPIOB->BRR = GPIO_PIN_12;     // reset
}

void RGB_setDown(){
    GPIOB->BSRR = GPIO_PIN_12;    // set
    int i;    
    i++;
    i++;
    i++;                         
    GPIOB->BRR = GPIO_PIN_12;    
}

void RGB_reset(){
    GPIOB->BRR = GPIO_PIN_12;     // reset
    delay_us(50);
}

#define RED         0x000F00
#define GREEN       0x0F0000
#define BLUE        0x00000F
#define WHITE       0x0F0F0F
#define CUTDOWN     0x000000

void RGB_set(uint32_t G8R8B8)
{
    int i;
    uint8_t byte = 0;
    for(i = 23; i>=0 ;i--){
        byte = ((G8R8B8 >> i) & 0x01);
        if(byte){
            RGB_setUp();
        }
        else{
            RGB_setDown();
        }
    }
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
    RGB_set(GREEN);
    RGB_set(BLUE);
    RGB_set(WHITE);
}
