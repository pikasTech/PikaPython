#ifndef __WM_WDG_H__
#define __WM_WDG_H__

#include "wm_hal.h"

#define WDG                ((WDG_TypeDef *)WDG_BASE)

typedef struct
{
	uint32_t Reload;	// unit: us
	
} WDG_InitTypeDef;

typedef struct
{
	WDG_TypeDef *Instance;
	
	WDG_InitTypeDef Init;
	
} WDG_HandleTypeDef;


#define IS_WDG_ALL_INSTANCE(INSTANCE)  ((INSTANCE) == WDG)

#define IS_WDG_RELOAD(__RELOAD__)            ((__RELOAD__) <= WDG_LD)

#define __HAL_WDG_CLEAR_FLAG(__HANDLE__, __FLAG__)         ((__HANDLE__)->Instance->CLR = __FLAG__)

HAL_StatusTypeDef     HAL_WDG_Init(WDG_HandleTypeDef *hwdg);

void HAL_WDG_MspInit(WDG_HandleTypeDef* hwdg);

HAL_StatusTypeDef HAL_WDG_DeInit(WDG_HandleTypeDef *hwdg);

void HAL_WDG_IRQHandler(WDG_HandleTypeDef *hwdg);

#endif
