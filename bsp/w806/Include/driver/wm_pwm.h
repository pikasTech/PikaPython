#ifndef __WM_PWM_H__
#define __WM_PWM_H__

#include "wm_hal.h"

typedef struct
{
	uint32_t Prescaler;			/* Specifies the prescaler value used to divide the PWM clock(40MHz).
									This parameter can be a number between Min_Data = 0x0000 and Max_Data = 0xFFFF */
	
	uint32_t CounterMode;		/* Specifies the counter mode.
									This parameter can be a value of @ref PWM_Counter_Mode */
								   
	uint32_t Period;			/* Specifies the period value to be loaded into the PERIOD
									Register at the next update event.
									This parameter can be a number between Min_Data = 0x00 and Max_Data = 0xFF.  */
								   
	uint32_t Pulse;				/* Specifies the pulse value to be loaded into the Compare Register.
									This parameter can be a number between Min_Data = 0x00 and Max_Data = 0xFF */
							   
	uint32_t AutoReloadPreload;	/* Specifies the auto-reload preload.
                                   This parameter can be a value of @ref TIM_AutoReloadPreload */
								   
	uint32_t OutMode;			/* Specifies the output mode.
								   This parameter can be a value of @ref PWM_Out_Mode*/
	
	uint32_t OutInverse;		/* Specifies the output polarity.
								   This parameter can be a value of @ref PWM_Out_Inverse */
	
	uint32_t Dtdiv;				/* Specifies the prescaler value used to divide the dead zone clock(40MHz) when in complementary mode.
								   This parameter can be a value of @ref PWM_DT_DIV */
	
	uint32_t Dtcnt;				/* Specifies the number of dead time clocks when in complementary mode.
								   This parameter can be a number between Min_Data = 0x00 and Max_Data = 0xFF */
								   
} PWM_InitTypeDef;

typedef struct
{
	PWM_TypeDef			*Instance;
	
	PWM_InitTypeDef		Init;
	
	uint32_t			Channel; /* This parameter can be a value of @ref PWM_Channel */
	char *				pin;
	//uint32_t			pin;
	
} PWM_HandleTypeDef;

#define PWM		((PWM_TypeDef *)PWM_BASE)

// PWM_Channel
#define PWM_CHANNEL_0	0x00
#define PWM_CHANNEL_1	0x01
#define PWM_CHANNEL_2	0x02
#define PWM_CHANNEL_3	0x03
#define PWM_CHANNEL_4	0x04
#define PWM_CHANNEL_ALL	0x01F

// PWM_Counter_Mode
#define PWM_COUNTERMODE_EDGEALIGNED_UP					0x0	// edge-aligned up mode for capture
#define PWM_COUNTERMODE_EDGEALIGNED_DOWN				0x1	// edge-aligned up mode for out
#define PWM_COUNTERMODE_CENTERALIGNED					0x2 // center-aligned mode for out

// PWM_AutoReloadPreload
#define PWM_AUTORELOAD_PRELOAD_DISABLE                	0x00               // TIMx_ARR register is not buffered
#define PWM_AUTORELOAD_PRELOAD_ENABLE                 	0x01              // TIMx_ARR register is buffered

// PWM_Out_Mode
#define PWM_OUT_MODE_INDEPENDENT						0x00
#define PWM_OUT_MODE_2SYNC								0x01
#define PWM_OUT_MODE_2COMPLEMENTARY						0x02
#define PWM_OUT_MODE_5SYNC								0x03
#define PWM_OUT_MODE_BREAK								0x04

// PWM_Out_Inverse
#define PWM_OUT_INVERSE_DISABLE							0x00
#define PWM_OUT_INVERSE_ENABLE							0x01

// PWM_DT_DIV
#define PWM_DTDIV_NONE									PWM_DTCR_DTDIV_1
#define PWM_DTDIV_2										PWM_DTCR_DTDIV_2
#define PWM_DTDIV_4										PWM_DTCR_DTDIV_4
#define PWM_DTDIV_8										PWM_DTCR_DTDIV_8




#define IS_PWM_INSTANCE(INSTANCE)	(((INSTANCE) == PWM))

#define IS_PWM_CHANNELS(__CHANNEL__)       (((__CHANNEL__) == PWM_CHANNEL_0) || \
                                            ((__CHANNEL__) == PWM_CHANNEL_1) || \
                                            ((__CHANNEL__) == PWM_CHANNEL_2) || \
                                            ((__CHANNEL__) == PWM_CHANNEL_3) || \
                                            ((__CHANNEL__) == PWM_CHANNEL_4) || \
                                            ((__CHANNEL__) == PWM_CHANNEL_ALL))

#define IS_PWM_PRESCALER(__PRESCALER__) (((__PRESCALER__) >= 0x0000) && ((__PRESCALER__) <= 0x0FFFF))

#define IS_PWM_COUNTER_MODE(__MODE__)      (((__MODE__) == PWM_COUNTERMODE_EDGEALIGNED_UP)              || \
                                            ((__MODE__) == PWM_COUNTERMODE_EDGEALIGNED_DOWN)            || \
                                            ((__MODE__) == PWM_COUNTERMODE_CENTERALIGNED))

#define IS_PWM_PERIOD(__PERIOD__)	(((__PERIOD__) >= 0x00) && ((__PERIOD__) <= 0x0FF))

#define IS_PWM_PULSE(__PULSE__)	(((__PULSE__) >= 0x00) && ((__PULSE__) <= 0x0FF))

#define IS_PWM_AUTORELOADPRELOAD(__AUTORELOAD__) (((__AUTORELOAD__) == PWM_AUTORELOAD_PRELOAD_DISABLE) || \
												  ((__AUTORELOAD__) == PWM_AUTORELOAD_PRELOAD_ENABLE))

#define IS_PWM_OUTMODE(__MODE__) (((__MODE__) == PWM_OUT_MODE_INDEPENDENT) || \
								  ((__MODE__) == PWM_OUT_MODE_2SYNC) || \
								  ((__MODE__) == PWM_OUT_MODE_2COMPLEMENTARY) || \
								  ((__MODE__) == PWM_OUT_MODE_5SYNC))

#define IS_PWM_OUTINVERSE(__INVERSE__) (((__INVERSE__) == PWM_OUT_INVERSE_DISABLE) || \
									    ((__INVERSE__) == PWM_OUT_INVERSE_ENABLE))

#define IS_PWM_DTDIV(__DIV__) (((__DIV__) == PWM_DTDIV_NONE) || \
							   ((__DIV__) == PWM_DTDIV_2) || \
							   ((__DIV__) == PWM_DTDIV_4) || \
							   ((__DIV__) == PWM_DTDIV_8))

#define IS_PWM_DTCNT(__CNT__) (((__CNT__) >= 0) && ((__CNT__) <= 0x0FF))




HAL_StatusTypeDef	HAL_PWM_Init(PWM_HandleTypeDef *hpwm);
HAL_StatusTypeDef	HAL_PWM_DeInit(PWM_HandleTypeDef *hpwm);
void				HAL_PWM_MspInit(PWM_HandleTypeDef *hpwm);
void				HAL_PWM_MspDeInit(PWM_HandleTypeDef *hpwm);

HAL_StatusTypeDef	HAL_PWM_Start(PWM_HandleTypeDef *hpwm, uint32_t Channel);
HAL_StatusTypeDef	HAL_PWM_Stop(PWM_HandleTypeDef *hpwm, uint32_t Channel);
HAL_StatusTypeDef	HAL_PWM_Duty_Set(PWM_HandleTypeDef *hpwm, uint32_t Channel, uint32_t Duty);
HAL_StatusTypeDef	HAL_PWM_Freq_Set(PWM_HandleTypeDef *hpwm, uint32_t Channel, uint32_t Prescaler, uint32_t Period);
#endif