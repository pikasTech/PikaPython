#include "beep.h" 

/*********************************************************************************
************************启明欣欣 STM32F407核心开发板******************************
**********************************************************************************
* 文件名称: beep.c                                                               *
* 文件简述：beep初始化                                                           *
* 创建日期：2017.08.30                                                           *
* 版    本：V1.0                                                                 *
* 作    者：Clever                                                               *
* 说    明：蜂鸣器对应IO口初始化                                                 * 
**********************************************************************************
*********************************************************************************/
					  	 
    
//BEEP对应IO初始化
void BEEP_Init(void)
{   
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOG_CLK_ENABLE();           //开启GPIOE时钟
  
  //初始化蜂鸣器对应引脚GPIOG7
    GPIO_Initure.Pin=GPIO_PIN_7;  
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
    HAL_GPIO_Init(GPIOG,&GPIO_Initure);
	
    HAL_GPIO_WritePin(GPIOG,GPIO_PIN_7,GPIO_PIN_RESET);   //蜂鸣器对应引脚GPIOG7拉低,关闭蜂鸣器  启、明、欣、欣
}






