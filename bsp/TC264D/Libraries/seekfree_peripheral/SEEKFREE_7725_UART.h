/*********************************************************************************************************************
 * COPYRIGHT NOTICE
 * Copyright (c) 2020,逐飞科技
 * All rights reserved.
 * 技术讨论QQ群：三群：824575535
 *
 * 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
 * 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
 *
 * @file            7725_UART
 * @company         成都逐飞科技有限公司
 * @author          逐飞科技(QQ3184284598)
 * @version         查看doc内version文件 版本说明
 * @Software        ADS v1.2.2
 * @Target core     TC264D
 * @Taobao          https://seekfree.taobao.com/
 * @date            2020-3-23
 * @note
					OV7725接线定义：
                    ------------------------------------
                                        模块管脚                     			单片机管脚
                    SDA(51的RX)             		查看SEEKFREE_7725_UART.h文件内的OV7725_COF_UART_TX 宏定义
                    SCL(51的TX)       			查看SEEKFREE_7725_UART.h文件内的OV7725_COF_UART_RX 宏定义
                                        场中断(VSY)           		查看SEEKFREE_7725_UART.h文件内的OV7725_UART_VSYNC_PIN 宏定义
                                        行中断(HREF)          		未使用，因此不接
                                        像素中断(PCLK)   			查看SEEKFREE_7725_UART.h文件内的OV7725_UART_PCLK_PIN 宏定义
                                        数据口(D0-D7)         		查看SEEKFREE_7725_UART.h文件内的OV7725_UART_DATA_PIN 宏定义
                                        默认分辨率              			 	160*120
                  	默认FPS                  	50帧
                    ------------------------------------
 ********************************************************************************************************************/



#ifndef _SEEKFREE_7725_UART_h
#define _SEEKFREE_7725_UART_h


#include "common.h"
#include "IfxDma_cfg.h"
#include "zf_uart.h"


#define OV7725_UART_W    		160	//只能是80 160 240 320 宽度和高度务必一一对应，例如宽度设置为80，高度只能设置为60。
#define OV7725_UART_H    		120	//只能是60 120 180 240
	
	
//配置摄像头参数
#define OV7725_UART_SIZE    	(OV7725_UART_W * OV7725_UART_H/8 )	//图像占用空间大小
#define OV7725_UART_DMA_NUM  	(OV7725_UART_SIZE )               	//DMA采集次数


//--------------------------------------------------------------------------------------------------
//引脚配置
//--------------------------------------------------------------------------------------------------
#define OV7725_COF_UART        	UART_1         		//配置摄像头所使用到的串口
#define OV7725_COF_UART_TX     	UART1_TX_P02_2
#define OV7725_COF_UART_RX     	UART1_RX_P02_3

#define OV7725_UART_VSYNC_PIN 	ERU_CH3_REQ6_P02_0  //场中断引脚	可选范围参考ERU_PIN_enum枚举 不可与像素引脚选择为同一个通道，或者是共用中断的通道
													//例如场中断选择ERU_CH3 那么像素引脚就不能选择ERU_CH7，因为3和7的中断是共用的。

#define OV7725_UART_DATA_PIN  	P00_0  				//定义D0数据引脚  假设D0定义为P00_0 那么D1所使用的引脚则为P00_1，依次类推
													//可设置参数P00_0、P02_0、P15_0，不能设置为其他参数

#define OV7725_UART_PCLK_PIN  	ERU_CH2_REQ14_P02_1 //定义像素时钟引脚 可选范围参考ERU_PIN_enum枚举 不可与场中断引脚选择为同一个通道，或者是共用中断的通道
													//例如场中断选择ERU_CH3 那么像素引脚就不能选择ERU_CH7，因为3和7的中断是共用的。

#define OV7725_UART_DMA_CH		IfxDma_ChannelId_5	//定义使用的DMA通道 0-47可选  通道号越大优先级越低
													//修改此处之后，需要修改对应的ERU中断优先级，例如此处设置为IfxDma_ChannelId_5则对应的ERU中断优先级应该设置为5


typedef enum
{
    OV7725_INIT 	= 0x00,			//初始化命令
	OV7725_RESERVE,					//保留位
    OV7725_CONTRAST,				//对比度调节 实现阈值的调节
    OV7725_FPS,						//帧率设置 只支持50 75 100 112 150
    OV7725_COL,						//分辨率设置 只能是80 160 240 320
    OV7725_ROW,						//分辨率设置 只能是60 120 180 240
	OV7725_CONFIG_FINISH,			//非命令位，主要用来占位计数
    
	OV7725_GET_WHO_AM_I= 0xEF,
	OV7725_GET_STATUS  = 0xF1,
    OV7725_GET_VERSION = 0xF2,
	
    OV7725_SET_ADDR    = 0xFE,
    OV7725_SET_DATA    = 0xFF,
}OV7725_CMD;

extern uint8 ov7725_uart_image_bin[OV7725_UART_H][OV7725_UART_W/8];		//定义存储接收图像的数组
extern uint8 ov7725_uart_image_dec[OV7725_UART_H][OV7725_UART_W];    	//图像解压后的数组

extern uint8 ov7725_uart_finish_flag;									//图像采集完成标志位


//函数声明
void ov7725_uart_callback(void);
void ov7725_uart_init(void);
void ov7725_uart_vsync(void);
void ov7725_uart_dma(void);
void image_decompression(uint8 *data1,uint8 *data2);
void seekfree_sendimg_7725(UARTN_enum uartn, uint8 *image, uint16 width, uint16 height);


#endif



