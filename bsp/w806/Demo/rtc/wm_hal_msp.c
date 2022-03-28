#include "wm_hal.h"

void HAL_MspInit(void)
{

}

void HAL_PMU_MspInit(PMU_HandleTypeDef *hpmu)
{
	HAL_NVIC_EnableIRQ(PMU_IRQn);
}

void HAL_PMU_MspDeInit(PMU_HandleTypeDef *hpmu)
{
	HAL_NVIC_DisableIRQ(PMU_IRQn);
}