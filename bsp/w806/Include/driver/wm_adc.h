#ifndef __WM_ADC_H__
#define __WM_ADC_H__

#include "wm_hal.h"

typedef struct
{
	uint32_t channel;	/* ADC_CHANNEL_0 	: channel 0
						 * ADC_CHANNEL_1 	: channel 1
						 * ADC_CHANNEL_2 	: channel 2
						 * ADC_CHANNEL_3 	: channel 3
						 * ADC_CHANNEL_0_1 	: channel 0 and channel 1 differential input
						 * ADC_CHANNEL_2_3 	: channel 2 and channel 3 differential input */
	
	uint32_t freq;		/* 307hz ~ 2khz */
	
	uint32_t cmp_val;	/* compare value */
	
	uint32_t cmp_pol;	/* CMP_POL_0 :  when adc_result >= cmp_val interrupt
						 * CMP_POL_1 :  whrn adc_result < cmp_val interrtup */
	
}ADC_InitTypeDef;

typedef struct __ADC_HandleTypeDef
{
	ADC_TypeDef                   *Instance;
	
	ADC_InitTypeDef               Init;
	
	HAL_LockTypeDef               Lock;
	
	int				  	  		  offset;
	
}ADC_HandleTypeDef;


#define ADC                ((ADC_TypeDef *)ADC_BASE)

#define ADC_CHANNEL_0		ADC_ANA_CR_CH_0
#define ADC_CHANNEL_1		ADC_ANA_CR_CH_1
#define ADC_CHANNEL_2		ADC_ANA_CR_CH_2
#define ADC_CHANNEL_3		ADC_ANA_CR_CH_3
#define ADC_CHANNEL_0_1		ADC_ANA_CR_CH_8
#define ADC_CHANNEL_2_3		ADC_ANA_CR_CH_9
#define ADC_CHANNEL_TEMP	ADC_ANA_CR_CH_TEMP
#define ADC_CHANNEL_OFFSET	ADC_ANA_CR_CH_OFFSET

#define ADC_FREQ_MIN		307
#define ADC_FREQ_MAX		2000

#define CMP_POL_0	0x0UL
#define CMP_POL_1	ADC_ADC_CR_CMPPOL

#define HAL_ADC_STATE_RESET             0x00000000U
#define HAL_ADC_STATE_READY             0x00000001U
#define HAL_ADC_STATE_BUSY_INTERNAL     0x00000002U
#define HAL_ADC_STATE_TIMEOUT           0x00000004U


#define IS_ADC_ALL_INSTANCE(INSTANCE) ((INSTANCE) == ADC)

#define IS_ADC_CHANNEL(CHANNEL) (((CHANNEL) == ADC_CHANNEL_0)           || \
                                 ((CHANNEL) == ADC_CHANNEL_1)           || \
                                 ((CHANNEL) == ADC_CHANNEL_2)           || \
                                 ((CHANNEL) == ADC_CHANNEL_3)           || \
                                 ((CHANNEL) == ADC_CHANNEL_0_1)           || \
                                 ((CHANNEL) == ADC_CHANNEL_2_3)            )
								 
#define IS_ADC_CMP_POL(POL)		(((POL) == CMP_POL_0) || \
								 ((POL) == CMP_POL_1))
								 
#define IS_ADC_FREQUENCY(FREQ)  (((FREQ) >= ADC_FREQ_MIN) && ((FREQ) <= ADC_FREQ_MAX))

#define __HAL_ADC_ENABLE(__HANDLE__)	(MODIFY_REG((__HANDLE__)->Instance->ANA_CR, ADC_ANA_CR_CH | ADC_ANA_CR_PD | ADC_ANA_CR_RST | ADC_ANA_CR_LDOEN, \
													(__HANDLE__)->Init.channel | ADC_ANA_CR_RST | ADC_ANA_CR_LDOEN))
													
#define __HAL_ADC_DISABLE(__HANDLE__)	(MODIFY_REG((__HANDLE__)->Instance->ANA_CR, ADC_ANA_CR_PD | ADC_ANA_CR_RST | ADC_ANA_CR_LDOEN, \
													ADC_ANA_CR_PD))

#define __HAL_ADC_CLEAR_FLAG(__HANDLE__, __FLAG__)   	\
		(WRITE_REG((__HANDLE__)->Instance->IF, (__FLAG__)))
		
#define __HAL_ADC_INT_ENABLE(__HANDLE__, __FLAG__)	(SET_BIT((__HANDLE__)->Instance->ADC_CR, __FLAG__))

#define __HAL_ADC_INT_DISABLE(__HANDLE__, __FLAG__)	(CLEAR_BIT((__HANDLE__)->Instance->ADC_CR, __FLAG__))

#define __HAL_ADC_GET_IT_SOURCE(__HANDLE__, __INTERRUPT__)                     \
  (((__HANDLE__)->Instance->ADC_CR & (__INTERRUPT__)) == (__INTERRUPT__))
  
#define __HAL_ADC_GET_FLAG(__HANDLE__, __FLAG__)                               \
  ((((__HANDLE__)->Instance->IF) & (__FLAG__)) == (__FLAG__))
  


HAL_StatusTypeDef       HAL_ADC_Init(ADC_HandleTypeDef* hadc);
HAL_StatusTypeDef       HAL_ADC_DeInit(ADC_HandleTypeDef *hadc);
void                    HAL_ADC_MspInit(ADC_HandleTypeDef* hadc);
void                    HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc);


// AD转换功能以查询方式实现所对应的开始、停止、查询是否转换完成、获取转换结果
HAL_StatusTypeDef       HAL_ADC_Start(ADC_HandleTypeDef* hadc);
HAL_StatusTypeDef       HAL_ADC_Stop(ADC_HandleTypeDef* hadc);
HAL_StatusTypeDef 		HAL_ADC_PollForConversion(ADC_HandleTypeDef* hadc);
int                		HAL_ADC_GetValue(ADC_HandleTypeDef* hadc);

// 该接口调用了以上接口，实现了一个完整的查询转换过程，并返回结果，可以直接调用该接口获取转换结果，返回值单位mv
int 					HAL_ADC_GET_INPUT_VOLTAGE(ADC_HandleTypeDef* hadc);

// AD转换功能以中断方式实现所对应的开始、停止、中断回调函数
HAL_StatusTypeDef       HAL_ADC_Start_IT(ADC_HandleTypeDef* hadc);
HAL_StatusTypeDef       HAL_ADC_Stop_IT(ADC_HandleTypeDef* hadc);
void                    HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);

// AD比较功能以中断方式实现所对应的开始、停止、中断回调函数
HAL_StatusTypeDef		HAL_ADC_Start_Compare_IT(ADC_HandleTypeDef* hadc);
HAL_StatusTypeDef       HAL_ADC_Stop_Compare_IT(ADC_HandleTypeDef* hadc);
void                    HAL_ADC_CompareCallback(ADC_HandleTypeDef* hadc);


void                    HAL_ADC_IRQHandler(ADC_HandleTypeDef* hadc);


int adc_get_inputvolt(uint8_t channel);

uint32_t adc_get_offset(void);

#endif
