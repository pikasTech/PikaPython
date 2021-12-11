#include "sys.h"
#include "usart.h"	
#if SYSTEM_SUPPORT_OS
#include "includes.h"				
#endif

#if 1
#pragma import(__use_no_semihosting)             
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
void _sys_exit(int x) 
{ 
	x = x; 
} 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 


u8 USART_RX_BUF[USART_REC_LEN]; 

u16 USART_RX_STA=0;     

u8 aRxBuffer[RXBUFFERSIZE];
UART_HandleTypeDef huart1; 

void uart_init(u32 bound)
{	
	huart1.Instance=USART1;					    
	huart1.Init.BaudRate=bound;				   
	huart1.Init.WordLength=UART_WORDLENGTH_8B;   
	huart1.Init.StopBits=UART_STOPBITS_1;	   
	huart1.Init.Parity=UART_PARITY_NONE;		   
	huart1.Init.HwFlowCtl=UART_HWCONTROL_NONE; 
	huart1.Init.Mode=UART_MODE_TX_RX;		    
	HAL_UART_Init(&huart1);					   
	HAL_UART_Receive_IT(&huart1, (u8 *)aRxBuffer, RXBUFFERSIZE);
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	if(huart->Instance==USART1)
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();			
		__HAL_RCC_USART1_CLK_ENABLE();			
		__HAL_RCC_AFIO_CLK_ENABLE();
	
		GPIO_Initure.Pin=GPIO_PIN_9;			
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;		
		GPIO_Initure.Pull=GPIO_PULLUP;			
		GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   

		GPIO_Initure.Pin=GPIO_PIN_10;			
		GPIO_Initure.Mode=GPIO_MODE_AF_INPUT;	
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	
		
#if EN_USART1_RX
		HAL_NVIC_EnableIRQ(USART1_IRQn);				
		HAL_NVIC_SetPriority(USART1_IRQn,3,3);			
#endif	
	}
}

