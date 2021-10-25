/***********************************************
*	描述：继电器驱动文件
* 日期				修改人				修改信息
--20210409		Magnin				初次修改
************************************************/

/**************开发板对应连接引脚***************
			RELAY -------------------------- PD2
***********************************************/
#include "relay.h"
#include "share.h"
#include "cm32m101a_conf.h"

//定义Relay灯参数类型
typedef struct{
	sp_relay_t relay;
	GPIO_Module* GPIOx;
	uint16_t Pin;
}sp_led_param_t;

//板载Relay灯参数数组
static sp_led_param_t relayParam[] = {
	{SP_RELAY1,GPIOD,GPIO_PIN_2}
};
//初始化Relay
void Relay_Init(void)
{
	GPIO_InitType GPIO_InitStructure;
	//开启GPIO端口时钟
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOD, ENABLE);
	//设置引脚为推挽输出
	GPIO_InitStructure.GPIO_Current = GPIO_DC_4mA;		//电流大小
	GPIO_InitStructure.GPIO_Pull    = GPIO_No_Pull;		//不进行上下拉
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;	//推挽输出
	
	for(uint8_t i = 0;i < ARRAY_SIZE(relayParam);i++)
	{		
		GPIO_InitStructure.Pin	= relayParam[i].Pin;
		GPIO_InitPeripheral(relayParam[i].GPIOx, &GPIO_InitStructure);
		//初始化关闭Relay
		Relay_SetState(relayParam[i].relay,SP_RELAY_STATE_CLOSE);
	}
	
}
//设置Relay状态
void Relay_SetState(sp_relay_t relay,sp_relay_state_t state)
{
	for(uint8_t i = 0;i < ARRAY_SIZE(relayParam);i++)
	{		
		if(relayParam[i].relay == relay)	//找到对应LED
		{
			if(state == SP_RELAY_STATE_OPEN)	GPIO_SetBits(relayParam[i].GPIOx,relayParam[i].Pin);		//高电平开灯
			else 									GPIO_ResetBits(relayParam[i].GPIOx,relayParam[i].Pin);	
		}
	}
}
//反转Relay
void Relay_Toggles(sp_relay_t relay)
{
	for(uint8_t i = 0;i < ARRAY_SIZE(relayParam);i++)
	{		
		if(relayParam[i].relay == relay)	//找到对应LED
		{
			relayParam[i].GPIOx->POD ^= relayParam[i].Pin;	//反转电平
		}
	}

}
//获取RelayD状态
sp_relay_state_t Relay_GetState(sp_relay_t relay)
{
	for(uint8_t i = 0;i < ARRAY_SIZE(relayParam);i++)
	{		
		if(relayParam[i].relay == relay)	//找到对应Relay
		{
			//读取Relay状态
			if(GPIO_ReadOutputDataBit(relayParam[i].GPIOx,relayParam[i].Pin))	return SP_RELAY_STATE_OPEN;		
			else 	return SP_RELAY_STATE_CLOSE;	
		}
	}
	//Relay不存在返回关闭
	return SP_RELAY_STATE_CLOSE;
}


