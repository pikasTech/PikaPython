#include "led.h" 

/*********************************************************************************
*********************启明欣欣 STM32F407应用开发板(高配版)*************************
**********************************************************************************
* 文件名称: led.c                                                                *
* 文件简述：LED初始化                                                            *
* 创建日期：2017.08.30                                                           *
* 版    本：V1.0                                                                 *
* 作    者：Clever                                                               *
* 说    明：LED对应IO口初始化                                                    * 
* 淘宝店铺：https://shop125046348.taobao.com                                     *
**********************************************************************************
*********************************************************************************/

//LED对应IO初始化
void LED_Init(void)
{    	 
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOE_CLK_ENABLE();           //开启GPIOE时钟
	  __HAL_RCC_GPIOG_CLK_ENABLE();           //开启GPIOE时钟
	
    GPIO_Initure.Pin=GPIO_PIN_3|GPIO_PIN_4; //PF9,10
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
    HAL_GPIO_Init(GPIOE,&GPIO_Initure);
	
	  GPIO_Initure.Pin=GPIO_PIN_9; //
    HAL_GPIO_Init(GPIOG,&GPIO_Initure);	
	
    HAL_GPIO_WritePin(GPIOE,GPIO_PIN_3,GPIO_PIN_SET);	//PE3置1，默认初始化后灯灭
    HAL_GPIO_WritePin(GPIOE,GPIO_PIN_4,GPIO_PIN_SET);	//PE4置1，默认初始化后灯灭
	  HAL_GPIO_WritePin(GPIOG,GPIO_PIN_9,GPIO_PIN_SET);	//PG9置1，默认初始化后灯灭  启*明*欣*欣
}






