/***********************************************
*	描述：板载LED驱动文件
* 日期				修改人				修改信息
--20210409		Magnin				初次修改
************************************************/

/**************开发板对应连接引脚***************
		LED1 -------------------------- PC12
		LED2 -------------------------- PC11
		LED3 -------------------------- PC10
		LED4 -------------------------- PA15
***********************************************/

#include "led.h"
#include "cm32m101a_conf.h"
#include "share.h"

//定义LED灯参数类型
typedef struct{
	sp_led_t led;
	GPIO_Module* GPIOx;
	uint16_t Pin;
}sp_led_param_t;

//板载LED灯参数数组
static sp_led_param_t ledParam[] = {
	{SP_LED1,GPIOC,GPIO_PIN_12},
	{SP_LED2,GPIOC,GPIO_PIN_11},
	{SP_LED3,GPIOC,GPIO_PIN_10},
	{SP_LED4,GPIOA,GPIO_PIN_15},

};
//初始化LED
void Led_Init(void)
{
	GPIO_InitType GPIO_InitStructure;
	//开启GPIO端口时钟
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOC, ENABLE);
	//设置引脚为推挽输出
	GPIO_InitStructure.GPIO_Current = GPIO_DC_8mA;		//电流大小
	GPIO_InitStructure.GPIO_Pull    = GPIO_No_Pull;		//不进行上下拉
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;	//推挽输出
	
	for(uint8_t i = 0;i < ARRAY_SIZE(ledParam);i++)
	{		
		GPIO_InitStructure.Pin	= ledParam[i].Pin;
		GPIO_InitPeripheral(ledParam[i].GPIOx, &GPIO_InitStructure);
		//初始化关闭LED
		Led_SetState(ledParam[i].led,SP_LED_STATE_CLOSE);
	}
	
}
//设置LED状态
void Led_SetState(sp_led_t led,sp_led_State_t state)
{
	for(uint8_t i = 0;i < ARRAY_SIZE(ledParam);i++)
	{		
		if(ledParam[i].led == led)	//找到对应LED
		{
			if(state == SP_LED_STATE_OPEN)	GPIO_SetBits(ledParam[i].GPIOx,ledParam[i].Pin);		//高电平开灯
			else 									GPIO_ResetBits(ledParam[i].GPIOx,ledParam[i].Pin);	
		}
	}
}
//反转LED
void Led_Toggles(sp_led_t led)
{
	for(uint8_t i = 0;i < ARRAY_SIZE(ledParam);i++)
	{		
		if(ledParam[i].led == led)	//找到对应LED
		{
			ledParam[i].GPIOx->POD ^= ledParam[i].Pin;	//反转电平
		}
	}

}
//获取LED状态
sp_led_State_t Led_GetState(sp_led_t led)
{
	for(uint8_t i = 0;i < ARRAY_SIZE(ledParam);i++)
	{		
		if(ledParam[i].led == led)	//找到对应LED
		{
			//读取LED状态
			if(GPIO_ReadOutputDataBit(ledParam[i].GPIOx,ledParam[i].Pin))	return SP_LED_STATE_OPEN;		
			else 	return SP_LED_STATE_CLOSE;	
		}
	}
	//LED不存在返回关闭
	return SP_LED_STATE_CLOSE;
}


