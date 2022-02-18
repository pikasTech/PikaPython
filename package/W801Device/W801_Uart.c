#include "BaseObj.h"
#include "dataStrs.h"
#include "W801Device_Uart.h"
#include "wm_hal.h"
#include "fifo.h"


UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;


uint8_t uart1RxBuffer[32];
uint8_t uart2RxBuffer[32];
uint8_t uart3RxBuffer[32];
uint8_t uart4RxBuffer[32];
uint8_t uart5RxBuffer[32];

#define uartFiFoBufferSize	512

uint8_t	FiFoBuffer[5][uartFiFoBufferSize];





void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
	if (huart->Instance == UART1)
	{
		__HAL_RCC_UART1_CLK_ENABLE();
		__HAL_RCC_GPIO_CLK_ENABLE();
		
		__HAL_AFIO_REMAP_UART1_TX(GPIOB, GPIO_PIN_6);
		__HAL_AFIO_REMAP_UART1_RX(GPIOB, GPIO_PIN_7);
		HAL_NVIC_SetPriority(UART1_IRQn, 0);
		HAL_NVIC_EnableIRQ(UART1_IRQn);
		
	}	
	
	if (huart->Instance == UART2)
	{
		__HAL_RCC_UART2_CLK_ENABLE();
		__HAL_RCC_GPIO_CLK_ENABLE();
		
		__HAL_AFIO_REMAP_UART2_TX(GPIOA, GPIO_PIN_2);
		__HAL_AFIO_REMAP_UART2_RX(GPIOA, GPIO_PIN_3);
		HAL_NVIC_SetPriority(UART2_5_IRQn, 0);
		HAL_NVIC_EnableIRQ(UART2_5_IRQn);
		
	}
	if (huart->Instance == UART3)
	{
		__HAL_RCC_UART3_CLK_ENABLE();
		__HAL_RCC_GPIO_CLK_ENABLE();
		
		__HAL_AFIO_REMAP_UART3_TX(GPIOA, GPIO_PIN_5);
		__HAL_AFIO_REMAP_UART3_RX(GPIOA, GPIO_PIN_6);
		HAL_NVIC_SetPriority(UART2_5_IRQn, 0);
		HAL_NVIC_EnableIRQ(UART2_5_IRQn);
		
	}
	if (huart->Instance == UART4)
	{
		__HAL_RCC_UART4_CLK_ENABLE();
		__HAL_RCC_GPIO_CLK_ENABLE();
		
		__HAL_AFIO_REMAP_UART4_TX(GPIOA, GPIO_PIN_8);
		__HAL_AFIO_REMAP_UART4_RX(GPIOA, GPIO_PIN_9);
		HAL_NVIC_SetPriority(UART2_5_IRQn, 0);
		HAL_NVIC_EnableIRQ(UART2_5_IRQn);
		
	}
	if (huart->Instance == UART5)
	{
		__HAL_RCC_UART5_CLK_ENABLE();
		__HAL_RCC_GPIO_CLK_ENABLE();
		
		__HAL_AFIO_REMAP_UART5_TX(GPIOA, GPIO_PIN_12);
		__HAL_AFIO_REMAP_UART5_RX(GPIOA, GPIO_PIN_13);
		HAL_NVIC_SetPriority(UART2_5_IRQn, 0);
		HAL_NVIC_EnableIRQ(UART2_5_IRQn);
		
	}
	
	
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
	if (huart->Instance == UART1)
	{
		__HAL_RCC_UART1_CLK_DISABLE();
		HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6|GPIO_PIN_7);
	}
	if (huart->Instance == UART2)
	{
		__HAL_RCC_UART2_CLK_DISABLE();
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);
	}	
	if (huart->Instance == UART3)
	{
		__HAL_RCC_UART3_CLK_DISABLE();
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5|GPIO_PIN_6);
	}
	if (huart->Instance == UART4)
	{
		__HAL_RCC_UART4_CLK_DISABLE();
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_8|GPIO_PIN_9);
	}
	if (huart->Instance == UART5)
	{
		__HAL_RCC_UART5_CLK_DISABLE();
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_12|GPIO_PIN_13);
	}
	
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{	
	
	if(huart->Instance==UART1)
	{
//		printf("uart2 recieved %s\r\n",huart->pRxBuffPtr);
		if (FifoSpaceLen(0)>= huart->RxXferCount)
		{
			FifoWrite(0,huart->pRxBuffPtr, huart->RxXferCount);
			
		}
	}
	if(huart->Instance==UART2)
	{

		if (FifoSpaceLen(1)>= huart->RxXferCount)
		{
			FifoWrite(1,huart->pRxBuffPtr, huart->RxXferCount);
			
		}
	}
	
	if(huart->Instance==UART3)
	{

		if (FifoSpaceLen(2)>= huart->RxXferCount)
		{
			FifoWrite(2,huart->pRxBuffPtr, huart->RxXferCount);
			
		}
	}
	
	if(huart->Instance==UART4)
	{

		if (FifoSpaceLen(3)>= huart->RxXferCount)
		{
			FifoWrite(3,huart->pRxBuffPtr, huart->RxXferCount);
			
		}
	}
	
	if(huart->Instance==UART5)
	{

		if (FifoSpaceLen(4)>= huart->RxXferCount)
		{
			FifoWrite(4,huart->pRxBuffPtr, huart->RxXferCount);
			
		}
	}
	
	
	

}


static USART_TypeDef* UART_get_instance(uint8_t id) {
    if (1 == id) {
        return UART1;
    }
    if (2 == id) {
        return UART2;
    }
	if (3 == id) {
        return UART3;
    }
	if (4 == id) {
        return UART4;
    }
	if (5 == id) {
        return UART5;
    }
    return NULL;
}




static uint8_t UART_hardware_init(uint32_t baudRate, uint8_t id) {
    uint8_t errCode = 0;
    USART_TypeDef* UARTx = UART_get_instance(id);
    if (NULL == UARTx) {
        errCode = 5;
        goto exit;
    }
 
    /* init USART1 */
    if (UART1 == UARTx) {
	
		huart1.Instance = UART1;
		huart1.Init.BaudRate = baudRate;
		huart1.Init.WordLength = UART_WORDLENGTH_8B;
		huart1.Init.StopBits = UART_STOPBITS_1;
		huart1.Init.Parity = UART_PARITY_NONE;
		huart1.Init.Mode = UART_MODE_TX_RX;
		huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
			
		if (HAL_UART_Init(&huart1) != HAL_OK)
		{
			Error_Handler();
		}
		
		HAL_UART_Receive_IT(&huart1, uart1RxBuffer, 0);	
		
		FifoInit(0,(uint8_t *)FiFoBuffer[0],uartFiFoBufferSize);
   
	}
	  if (UART2 == UARTx) {
	
		huart2.Instance = UART2;
		huart2.Init.BaudRate = baudRate;
		huart2.Init.WordLength = UART_WORDLENGTH_8B;
		huart2.Init.StopBits = UART_STOPBITS_1;
		huart2.Init.Parity = UART_PARITY_NONE;
		huart2.Init.Mode = UART_MODE_TX_RX;
		huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
			
		if (HAL_UART_Init(&huart2) != HAL_OK)
		{
			Error_Handler();
		}
		
		HAL_UART_Receive_IT(&huart2, uart2RxBuffer, 0);	
		
		FifoInit(1,(uint8_t *)FiFoBuffer[1],uartFiFoBufferSize);
   
	}
	if (UART3 == UARTx) {
	
		huart3.Instance = UART3;
		huart3.Init.BaudRate = baudRate;
		huart3.Init.WordLength = UART_WORDLENGTH_8B;
		huart3.Init.StopBits = UART_STOPBITS_1;
		huart3.Init.Parity = UART_PARITY_NONE;
		huart3.Init.Mode = UART_MODE_TX_RX;
		huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
			
		if (HAL_UART_Init(&huart3) != HAL_OK)
		{
			Error_Handler();
		}
		
		HAL_UART_Receive_IT(&huart3, uart1RxBuffer, 0);	
		
		FifoInit(2,(uint8_t *)FiFoBuffer[2],uartFiFoBufferSize);
   
	}
	if (UART4 == UARTx) {
	
		huart4.Instance = UART4;
		huart4.Init.BaudRate = baudRate;
		huart4.Init.WordLength = UART_WORDLENGTH_8B;
		huart4.Init.StopBits = UART_STOPBITS_1;
		huart4.Init.Parity = UART_PARITY_NONE;
		huart4.Init.Mode = UART_MODE_TX_RX;
		huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
			
		if (HAL_UART_Init(&huart4) != HAL_OK)
		{
			Error_Handler();
		}
		
		HAL_UART_Receive_IT(&huart4, uart1RxBuffer, 0);	
		
		FifoInit(3,(uint8_t *)FiFoBuffer[3],uartFiFoBufferSize);
   
	}
	if (UART5 == UARTx) {
	
		huart5.Instance = UART5;
		huart5.Init.BaudRate = baudRate;
		huart5.Init.WordLength = UART_WORDLENGTH_8B;
		huart5.Init.StopBits = UART_STOPBITS_1;
		huart5.Init.Parity = UART_PARITY_NONE;
		huart5.Init.Mode = UART_MODE_TX_RX;
		huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
			
		if (HAL_UART_Init(&huart5) != HAL_OK)
		{
			Error_Handler();
		}
		
		HAL_UART_Receive_IT(&huart5, uart1RxBuffer, 0);	
		
		FifoInit(4,(uint8_t *)FiFoBuffer[4],uartFiFoBufferSize);
   
	}
exit:
    return errCode;
}

void W801Device_UART_platformEnable(PikaObj *self)
{
	int id = obj_getInt(self, "id");
    int baudRate = obj_getInt(self, "baudRate");
    USART_TypeDef* USARTx = UART_get_instance(id);
    int errCode = UART_hardware_init(baudRate, id);
    if (0 != errCode) {
        obj_setErrorCode(self, 1);
        obj_setSysOut(self, "[error] USART init faild.");
        return;
    }
}
void W801Device_UART_platformRead(PikaObj *self)
{
	int id = obj_getInt(self, "id");   
    int length = obj_getInt(self, "length");
   
	Args* buffs = New_strBuff();
    char* readBuff = args_getBuff(buffs, length);
	
	if(id<1||id>5)
	{
		args_deinit(buffs);
		return;
	}
	
	if(length<FifoDataLen(id-1))
	{
			length=FifoDataLen(id-1);
	}
	FifoRead(id-1,readBuff,length);    
	
    obj_setStr(self, "readBuff", readBuff);	   
	args_deinit(buffs);
    
}
void W801Device_UART_platformWrite(PikaObj *self)
{
	char* data = obj_getStr(self, "data");
    int id = obj_getInt(self, "id");
    
	switch(id)
	{
		case 1:			
			HAL_UART_Transmit(&huart1,data,strGetSize(data),5000);
		break;
		case 2:
			HAL_UART_Transmit(&huart2,data,strGetSize(data),5000);
		break;
		
		case 3:
			HAL_UART_Transmit(&huart3,data,strGetSize(data),5000);
		break;
		
		case 4:
			HAL_UART_Transmit(&huart4,data,strGetSize(data),5000);
		break;
		
		case 5:
			HAL_UART_Transmit(&huart5,data,strGetSize(data),5000);
		break;
		
		default:
		
		break;
		
	}

}