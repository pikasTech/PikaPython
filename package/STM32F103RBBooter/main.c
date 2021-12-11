#include "sys.h"
#include "usart.h"
#include "pikaScript.h"

int main(void)
{
    HAL_Init();                    	
    Stm32_Clock_Init(RCC_PLL_MUL9); 
    uart_init(115200);
    PikaObj* pika_main = pikaScriptInit(); 
    while(1){
			
    }
}


