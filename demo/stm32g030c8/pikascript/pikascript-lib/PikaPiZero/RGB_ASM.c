#include "RGB_ASM.h"
#include "STM32_common.h"

void RGB_set(uint32_t G8R8B8)
{
   int i = 23;
   uint8_t byte = 0;
   while( i>0 ){
       byte = ((G8R8B8 >> i) & 0x01);
       if(byte){
           GPIOB->BSRR = GPIO_PIN_12;    // set
           __ASM ("nop");__ASM ("nop");__ASM ("nop");
           __ASM ("nop");__ASM ("nop");__ASM ("nop");
           __ASM ("nop");__ASM ("nop");__ASM ("nop");
           __ASM ("nop");__ASM ("nop");__ASM ("nop");
           __ASM ("nop");__ASM ("nop");__ASM ("nop");
           __ASM ("nop");__ASM ("nop");__ASM ("nop");
           __ASM ("nop");__ASM ("nop");__ASM ("nop");
           __ASM ("nop");__ASM ("nop");__ASM ("nop");
           __ASM ("nop");__ASM ("nop");__ASM ("nop");
           __ASM ("nop");__ASM ("nop");__ASM ("nop");
           __ASM ("nop");__ASM ("nop");__ASM ("nop");
           __ASM ("nop");__ASM ("nop");__ASM ("nop");
           __ASM ("nop");__ASM ("nop");__ASM ("nop");
           GPIOB->BRR = GPIO_PIN_12;     // reset
       }
       else{
           GPIOB->BSRR = GPIO_PIN_12;    // set
           __ASM ("nop");__ASM ("nop");__ASM ("nop");
           __ASM ("nop");__ASM ("nop");__ASM ("nop");
           __ASM ("nop");__ASM ("nop");__ASM ("nop");
           __ASM ("nop");__ASM ("nop");__ASM ("nop");
           GPIOB->BRR = GPIO_PIN_12;    
       }
       i -- ;
   }
}