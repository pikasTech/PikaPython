#include "STM32_PWM.h"
#include <stdint.h>
#include "BaseObj.h"
#include "STM32_common.h"
#include "dataStrs.h"

#ifdef TIM1_EXIST
    TIM_HandleTypeDef pika_tim1;
#endif
#ifdef TIM3_EXIST
    TIM_HandleTypeDef pika_tim3;
#endif
#ifdef TIM14_EXIST
    TIM_HandleTypeDef pika_tim14;
#endif
#ifdef TIM16_EXIST
    TIM_HandleTypeDef pika_tim16;
#endif
#ifdef TIM17_EXIST
    TIM_HandleTypeDef pika_tim17;
#endif

static TIM_HandleTypeDef* getTimHandle(char *pin)
{
    #ifdef TIM1_EXIST
        if( strEqu("PA8", pin) || strEqu("PA9", pin) || strEqu("PA10", pin) || strEqu("PA11", pin) ){
            return &pika_tim1;
        }
    #endif
    #ifdef TIM3_EXIST
        if( strEqu("PA6", pin) || strEqu("PA7", pin) || strEqu("PB0", pin) || strEqu("PB1", pin) ){
            return &pika_tim3;
        }
    #endif
    #ifdef TIM14_EXIST
        if( strEqu("PA4", pin) ){
            return &pika_tim14;
        }
    #endif
    #ifdef TIM16_EXIST
        if( strEqu("PD0", pin) ){
            return &pika_tim16;
        }
    #endif
    #ifdef TIM17_EXIST
        if( strEqu("PD1", pin) ){
            return &pika_tim17;
        }
    #endif    
    return NULL;
}

static TIM_TypeDef* getTimInstance(char *pin)
{
    #ifdef TIM1_EXIST
        if( strEqu("PA8", pin) || strEqu("PA9", pin) || strEqu("PA10", pin) || strEqu("PA11", pin) ){
            return TIM1;
        }
    #endif
    #ifdef TIM3_EXIST
        if( strEqu("PA6", pin) || strEqu("PA7", pin) || strEqu("PB0", pin) || strEqu("PB1", pin) ){
            return TIM3;
        }
    #endif
    #ifdef TIM14_EXIST
        if( strEqu("PA4", pin) ){
            return TIM14;
        }
    #endif
    #ifdef TIM16_EXIST
        if( strEqu("PD0", pin) ){
            return TIM16;
        }
    #endif
    #ifdef TIM17_EXIST
        if( strEqu("PD1", pin) ){
            return TIM17;
        }
    #endif    
    return NULL;
}

uint8_t PWM_MspPostInit(char* pin){
    if (NULL == getTimInstance(pin))
    {   
        /* this Pin do not match any PWM generator */
        return 1;
    }
    enableClk(pin);
    return 0;
}

void STM32_PWM_platformEnable(PikaObj *self, float duty, int freq, char * pin){
}
void STM32_PWM_platformSetDuty(PikaObj *self, float duty, char * pin){
}
void STM32_PWM_platformSetFrequency(PikaObj *self, int freq, char * pin){
}
