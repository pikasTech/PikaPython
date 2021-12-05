#include "sys.h"
#include "usart.h"
#include "delay.h"
#include "pikaScript.h"

int main(void)
{
	
    HAL_Init();                    	//初始化HAL库    
    Stm32_Clock_Init(RCC_PLL_MUL9); //设置时钟,72M
		uart_init(115200);
		delay_init(72);
    PikaObj* pika_main = pikaScriptInit(); 
		while(1)
		{
			
		}
}


