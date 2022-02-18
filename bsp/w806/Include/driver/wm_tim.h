#ifndef __WM_TIM_H__
#define __WM_TIM_H__

#include "wm_hal.h"

#define TIM                ((TIM_TypeDef *)TIM_BASE)
#define TIM0				0
#define TIM1				1
#define TIM2				2
#define TIM3				3
#define TIM4				4
#define TIM5				5

typedef enum
{
  HAL_TIM_STATE_RESET             = 0x00U,
  HAL_TIM_STATE_READY             = 0x01U,
  HAL_TIM_STATE_BUSY              = 0x02U,
  HAL_TIM_STATE_TIMEOUT           = 0x03U,
  HAL_TIM_STATE_ERROR             = 0x04U
} HAL_TIM_StateTypeDef;

typedef struct
{
	uint32_t Unit;
	uint32_t AutoReload;
	uint32_t Period;
	
} TIM_Base_InitTypeDef;

typedef struct
{
	uint32_t                        Instance;
	TIM_Base_InitTypeDef            Init;
	
	HAL_LockTypeDef                 Lock;
	__IO HAL_TIM_StateTypeDef       State;
	
} TIM_HandleTypeDef;

#define TIM_UNIT_US				0x00000000U
#define TIM_UNIT_MS				0x00000001U

#define TIM_AUTORELOAD_PRELOAD_DISABLE                0x00000001U
#define TIM_AUTORELOAD_PRELOAD_ENABLE                 0x00000000U

#define IS_TIM_INSTANCE(INSTANCE)\
		  (((INSTANCE) == TIM0)    || \
		   ((INSTANCE) == TIM1)    || \
		   ((INSTANCE) == TIM2)    || \
		   ((INSTANCE) == TIM3)    || \
		   ((INSTANCE) == TIM4)    || \
		   ((INSTANCE) == TIM5))

#define IS_TIM_UNIT(UNIT) (((UNIT) == TIM_UNIT_US) || \
						   ((UNIT) == TIM_UNIT_MS))

#define IS_TIM_AUTORELOAD(PRELOAD) (((PRELOAD) == TIM_AUTORELOAD_PRELOAD_DISABLE) || \
                                            ((PRELOAD) == TIM_AUTORELOAD_PRELOAD_ENABLE))

#define __HAL_TIM_ENABLE(__HANDLE__)				(TIM->CR |= TIM_CR_TIM_EN((__HANDLE__)->Instance - TIM0))

#define __HAL_TIM_DISABLE(__HANDLE__)				(TIM->CR &= ~(TIM_CR_TIM_EN((__HANDLE__)->Instance - TIM0)))

#define __HAL_TIM_ENABLE_IT(__HANDLE__)				(TIM->CR |= TIM_CR_TIM_TIE((__HANDLE__)->Instance - TIM0))

#define __HAL_TIM_DISABLE_IT(__HANDLE__)			(TIM->CR &= ~(TIM_CR_TIM_TIE((__HANDLE__)->Instance - TIM0)))

#define __HAL_TIM_GET_FLAG(__HANDLE__)          	((TIM->CR & TIM_CR_TIM_TIF((__HANDLE__)->Instance - TIM0)) == \
														TIM_CR_TIM_TIF((__HANDLE__)->Instance - TIM0))

#define __HAL_TIM_CLEAR_IT(__HANDLE__)      		(TIM->CR |= TIM_CR_TIM_TIF((__HANDLE__)->Instance - TIM0))

HAL_StatusTypeDef HAL_TIM_Base_Init(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_Base_DeInit(TIM_HandleTypeDef *htim);
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim);
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *htim);

HAL_StatusTypeDef HAL_TIM_Base_Start(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_Base_Stop(TIM_HandleTypeDef *htim);

HAL_StatusTypeDef HAL_TIM_Base_Start_IT(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_Base_Stop_IT(TIM_HandleTypeDef *htim);

HAL_TIM_StateTypeDef HAL_TIM_Base_GetState(TIM_HandleTypeDef *htim);

void HAL_TIM_IRQHandler(TIM_HandleTypeDef *htim);

void HAL_TIM_Callback(TIM_HandleTypeDef *htim);

#endif
