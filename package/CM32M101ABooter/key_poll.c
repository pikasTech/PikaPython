/***********************************************
*	描述：板载按键驱动文件
* 日期				修改人				修改信息
--20210409		Magnin				初次修改
************************************************/

/**************开发板对应连接引脚***************
			KEY1 -------------------------- PC7
			KEY2 -------------------------- PC8
			KEY3 -------------------------- PC9
			KEY4 -------------------------- PA0
***********************************************/
#include "key_poll.h"
#include "share.h"
#include "cm32m101a_conf.h"

#define KEY_SCAN_DELAY			100	//按键检测延时时间

typedef struct{
	sp_key_value_t value;
	GPIO_Module* GPIOx;
	uint16_t Pin;
}sp_key_t;

static sp_key_t spKey[] = {
	{SP_KEY_PRESS_KEY1,GPIOC,GPIO_PIN_7},
	{SP_KEY_PRESS_KEY2,GPIOC,GPIO_PIN_8},
	{SP_KEY_PRESS_KEY3,GPIOC,GPIO_PIN_9},
	{SP_KEY_PRESS_KEY4,GPIOA,GPIO_PIN_0},
};
//按键轮询初始化
void Key_PollInit(void)
{
	GPIO_InitType GPIO_InitStructure;
	//开启GPIO端口时钟
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOA, ENABLE);
	RCC_EnableAPB2PeriphClk(RCC_APB2_PERIPH_GPIOC, ENABLE);
	//设置引脚为上拉输入
	GPIO_InitStructure.GPIO_Pull   = GPIO_Pull_Up;		//上拉
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Input;	//输入
	for(uint8_t i = 0;i < ARRAY_SIZE(spKey);i++)
	{		
		GPIO_InitStructure.Pin	= spKey[i].Pin;
		GPIO_InitPeripheral(spKey[i].GPIOx, &GPIO_InitStructure);
	}
}
//按键轮询
sp_key_value_t Key_Poll(void)
{
	for(uint8_t i = 0;i < ARRAY_SIZE(spKey);i++)
	{		
		if(GPIO_ReadInputDataBit(spKey[i].GPIOx,spKey[i].Pin) == Bit_RESET)
		{
			//延时消抖
			Delay_Ms(KEY_SCAN_DELAY);
			if(GPIO_ReadInputDataBit(spKey[i].GPIOx,spKey[i].Pin) == Bit_RESET)
			{
#ifdef SP_KEY_POLL_LOOSE_CHECK
				while(GPIO_ReadInputDataBit(spKey[i].GPIOx,spKey[i].Pin) == Bit_RESET) ;
#endif
				return spKey[i].value;
			}				
		}
	}
	return SP_KEY_PRESS_NONE;
}

