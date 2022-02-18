#include "BaseObj.h"
#include "dataStrs.h"
#include "W801Device_PWM.h"
#include "wm_hal.h"
#include "W801_common.h"

PWM_HandleTypeDef hpwm;

void HAL_PWM_MspInit(PWM_HandleTypeDef *hpwm)
{
	__HAL_RCC_PWM_CLK_ENABLE();
	__HAL_AFIO_REMAP_PWM0(GPIO_get_Group(hpwm->pin), GPIO_get_pin(hpwm->pin));
	printf("%s enter\r\n",__func__);
}

void HAL_PWM_MspDeInit(PWM_HandleTypeDef *hpwm)
{
	__HAL_RCC_PWM_CLK_DISABLE();
	HAL_GPIO_DeInit(GPIO_get_Group(hpwm->pin), GPIO_get_pin(hpwm->pin));
}

int GetPWMChannel(char *pin)
{
	 if (strEqu("PB0", pin) || strEqu("PB19", pin) || strEqu("PB12", pin) ||
        strEqu("PA2", pin) || strEqu("PA10", pin) ) {
        return PWM_CHANNEL_0;
    }
    if (strEqu("PB1", pin) || strEqu("PB20", pin) || strEqu("PA3", pin) ||
        strEqu("PA11", pin)||strEqu("PB12", pin)) {
        return PWM_CHANNEL_1;
    }
    if (strEqu("PA0", pin) || strEqu("PB2", pin) || strEqu("PA12", pin) ||
        strEqu("PB14", pin)||strEqu("PB24", pin)) {
        return PWM_CHANNEL_2;
    }
    if (strEqu("PA1", pin) || strEqu("PB3", pin) || strEqu("PA13", pin) ||
        strEqu("PB15", pin)||strEqu("PB25", pin)) {
        return PWM_CHANNEL_3;
    }
	if (strEqu("PA4", pin) || strEqu("PA7", pin) || strEqu("PA14", pin) ||
        strEqu("PB16", pin)||strEqu("PB26", pin)) {
        return PWM_CHANNEL_4;
    }
    /* Chennel not match */
    return 99999;
}

void W801Device_PWM_platformEnable(PikaObj *self)
{
	float duty = obj_getFloat(self,"duty");
    int freq = obj_getInt(self, "freq");
    //char* pin = obj_getStr(self, "pin");
	printf("%s: duty is %f\r\n",__func__,duty);
	
	hpwm.Instance = PWM;
	hpwm.Init.AutoReloadPreload = PWM_AUTORELOAD_PRELOAD_ENABLE;
	hpwm.Init.CounterMode = PWM_COUNTERMODE_EDGEALIGNED_DOWN;
	hpwm.Init.Prescaler = 200000/freq;
	hpwm.Init.Period = 200;	// 40M / 2 / 100K - 1
	hpwm.Init.Pulse = (uint32_t)duty;	// 20% DUTY
	hpwm.Init.OutMode = PWM_OUT_MODE_INDEPENDENT;
	hpwm.pin=obj_getStr(self, "pin");
	hpwm.Channel = GetPWMChannel(hpwm.pin);	
	printf("%s: freq is %d, pin is %s\r\n",__func__,freq,hpwm.pin);
	
	printf("%s:prescale is %d, pulse is %d\r\n",__func__,hpwm.Init.Prescaler,hpwm.Init.Pulse);
	HAL_PWM_Init(&hpwm);
	
	HAL_PWM_Start(&hpwm, hpwm.Channel);
	
	
	
}
void W801Device_PWM_platformSetDuty(PikaObj *self)
{
	uint32_t duty = (uint32_t)obj_getFloat(self, "duty");
    char* pin = obj_getStr(self, "pin");
    uint32_t ch = GetPWMChannel(pin);  
	if(duty>100){duty=100;}
	
    HAL_PWM_Duty_Set(&hpwm, ch, (uint32_t)duty);
	printf("%s: duty is %d,pin is %s\r\n",__func__,(uint32_t)duty,pin);
}
void W801Device_PWM_platformSetFrequency(PikaObj *self)
{
	int freq = obj_getInt(self, "freq");	
    char* pin = obj_getStr(self, "pin");
	uint32_t ch = GetPWMChannel(pin);
	
	hpwm.Init.Prescaler = 200000/freq;
	hpwm.Init.Period = 200;	// 40M / 2 / 100K - 1
	
	HAL_PWM_Freq_Set(&hpwm,ch,hpwm.Init.Prescaler,hpwm.Init.Period);
	
   printf("%s: freq is %d, pin is %s\r\n",__func__,freq,pin);
}
