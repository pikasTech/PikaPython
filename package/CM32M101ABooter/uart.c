/***********************************************
*	描述串口驱动文件
* 日期				修改人				修改信息
--20210409		Magnin				初次修改
************************************************/

/**************开发板对应连接引脚***************
			UART1 TX -------------------------- PA9
			UART1 RX -------------------------- PA10
***********************************************/
#include "uart.h"
#include "share.h"
#include "cm32m101a_conf.h"
#include <stdio.h>

static USART_InitType USART_InitStructure;

void Usart1_Init(void)
{		
	GPIO_InitType GPIO_InitStructure;
	//开启GPIO口时钟
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);
	//开启USART时钟
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_USART1, ENABLE);
	// 初始化GPIO结构
	GPIO_InitStruct(&GPIO_InitStructure);
	//配置 USARTx Tx
	GPIO_InitStructure.Pin            = GPIO_PIN_9;
	GPIO_InitStructure.GPIO_Mode      = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Alternate = GPIO_AF4_USART1;	//复用为串口
	GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);
	//配置 USARTx Rx
	GPIO_InitStructure.Pin            = GPIO_PIN_10;
	GPIO_InitStructure.GPIO_Pull      = GPIO_Pull_Up;
	GPIO_InitStructure.GPIO_Alternate = GPIO_AF4_USART1;	//复用为串口
	GPIO_InitPeripheral(GPIOA, &GPIO_InitStructure);
	//配置串口参数
	USART_InitStructure.BaudRate            = USART1_BANDRATE;	//波特率
	USART_InitStructure.WordLength          = USART_WL_8B;			//数据位
	USART_InitStructure.StopBits            = USART_STPB_1;			//停止位
	USART_InitStructure.Parity              = USART_PE_NO;			//校验位
	USART_InitStructure.HardwareFlowControl = USART_HFCTRL_NONE;	//关闭硬件流使能
	USART_InitStructure.Mode                = USART_MODE_RX | USART_MODE_TX;	//发送接收模式
	//初始化USART1
	USART_Init(USART1, &USART_InitStructure);
	//使能USART1
	USART_Enable(USART1, ENABLE);
}
//重定向fputc
int fputc(int ch, FILE* f)
{
	USART_SendData(USART1, (uint8_t)ch);	//向USART1发送数据
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXDE) == RESET)	;	//等待发送完成
	return (ch);
}

