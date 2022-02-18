#include "wm_gpio.h"

#define EXTI_MODE 0x80

void HAL_GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_Init)
{
	uint32_t position = 0x00;
	uint32_t ioposition, iocurrent;
	
	assert_param(IS_GPIO_ALL_INSTANCE(GPIOx));
	assert_param(IS_GPIO_PIN(GPIO_Init->Pin));
	assert_param(IS_GPIO_MODE(GPIO_Init->Mode));

	while (((GPIO_Init->Pin) >>  position) != 0x00)
	{
		ioposition = (0x01 << position);
		
		iocurrent = (uint32_t)(GPIO_Init->Pin) & ioposition;
		
		if (iocurrent == ioposition)
		{
			assert_param(IS_GPIO_AF_INSTANCE(GPIOx));
			__AFIO_REMAP_SET_OPT5(GPIOx, ioposition);
			switch (GPIO_Init->Mode)
			{
				case GPIO_MODE_OUTPUT:
					SET_BIT(GPIOx->DIR, ioposition);
					break;
					
				case GPIO_MODE_INPUT:
				case GPIO_MODE_IT_RISING:
				case GPIO_MODE_IT_FALLING:
				case GPIO_MODE_IT_RISING_FALLING:
				case GPIO_MODE_IT_HIGH_LEVEL:
				case GPIO_MODE_IT_LOW_LEVEL:
					CLEAR_BIT(GPIOx->DIR, ioposition);
					break;
					
				default:
					break;
				
			}
			
			assert_param(IS_GPIO_PULL(GPIO_Init->Pull));
			if (GPIO_Init->Pull == GPIO_NOPULL)
			{
					SET_BIT(GPIOx->PULLUP_EN, ioposition);
					CLEAR_BIT(GPIOx->PULLDOWN_EN, ioposition);
			}
			else if (GPIO_Init->Pull == GPIO_PULLUP)
			{
					CLEAR_BIT(GPIOx->PULLUP_EN, ioposition);
					CLEAR_BIT(GPIOx->PULLDOWN_EN, ioposition);
			}
			else if(GPIO_Init->Pull == GPIO_PULLDOWN)
			{
					SET_BIT(GPIOx->PULLUP_EN, ioposition);
					SET_BIT(GPIOx->PULLDOWN_EN, ioposition);
			}
			
			switch (GPIO_Init->Mode)
			{
				case GPIO_MODE_IT_RISING:
					CLEAR_BIT(GPIOx->IS, ioposition);
					CLEAR_BIT(GPIOx->IBE, ioposition);
					SET_BIT(GPIOx->IEV, ioposition);
					break;
					
				case GPIO_MODE_IT_FALLING:
					CLEAR_BIT(GPIOx->IS, ioposition);
					CLEAR_BIT(GPIOx->IBE, ioposition);
					CLEAR_BIT(GPIOx->IEV, ioposition);
					break;
					
				case GPIO_MODE_IT_RISING_FALLING:
					CLEAR_BIT(GPIOx->IS, ioposition);
					SET_BIT(GPIOx->IBE, ioposition);
					break;
					
				case GPIO_MODE_IT_HIGH_LEVEL:
					SET_BIT(GPIOx->IS, ioposition);
					SET_BIT(GPIOx->IEV, ioposition);
					break;
					
				case GPIO_MODE_IT_LOW_LEVEL:
					SET_BIT(GPIOx->IS, ioposition);
					CLEAR_BIT(GPIOx->IEV, ioposition);
					break;
					
				default:
					break;
			}
			if ((GPIO_Init->Mode & EXTI_MODE) == EXTI_MODE)
			{
				SET_BIT(GPIOx->IE, ioposition);
			}
		}
		
		position++;
	}
}

void HAL_GPIO_DeInit(GPIO_TypeDef  *GPIOx, uint32_t GPIO_Pin)
{
	uint32_t position = 0x00, iocurrent;
	
	assert_param(IS_GPIO_ALL_INSTANCE(GPIOx));
	assert_param(IS_GPIO_PIN(GPIO_Pin));

	while ((GPIO_Pin >> position) != 0)
	{
		iocurrent = (GPIO_Pin) & (1uL << position);
		
		if (iocurrent)
		{
			CLEAR_BIT(GPIOx->DIR, iocurrent);
			
			SET_BIT(GPIOx->PULLUP_EN, iocurrent);
			CLEAR_BIT(GPIOx->PULLDOWN_EN, iocurrent);
			
			CLEAR_BIT(GPIOx->IS, iocurrent);
			CLEAR_BIT(GPIOx->IBE, iocurrent);
			CLEAR_BIT(GPIOx->IEV, iocurrent);
			CLEAR_BIT(GPIOx->IE, iocurrent);
		}
		
		position++;
	}
}

GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin)
{
	GPIO_PinState bitstatus;
	
	assert_param(IS_GPIO_PIN(GPIO_Pin));

	if ((GPIOx->DATA & GPIO_Pin) != GPIO_PIN_RESET)
	{
		bitstatus = GPIO_PIN_SET;
	}
	else
	{
		bitstatus = GPIO_PIN_RESET;
	}
	return bitstatus;
}

void HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin, GPIO_PinState PinState)
{
	uint32_t data_en;
	
	assert_param(IS_GPIO_PIN(GPIO_Pin));
	assert_param(IS_GPIO_PIN_ACTION(PinState));

	data_en = READ_REG(GPIOx->DATA_B_EN);
	SET_BIT(GPIOx->DATA_B_EN, GPIO_Pin);
	if (PinState != GPIO_PIN_RESET)
	{
		SET_BIT(GPIOx->DATA, GPIO_Pin);
	}
	else
	{
		CLEAR_BIT(GPIOx->DATA, GPIO_Pin);
	}
	WRITE_REG(GPIOx->DATA_B_EN, data_en);
}

void HAL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin)
{
	uint32_t data_en, position = 0x00, iocurrent;
	
	assert_param(IS_GPIO_PIN(GPIO_Pin));
	
	data_en = READ_REG(GPIOx->DATA_B_EN);
	SET_BIT(GPIOx->DATA_B_EN, GPIO_Pin);
	while ((GPIO_Pin >> position) != 0)
	{
		iocurrent = (GPIO_Pin) & (1uL << position);
		
		if (iocurrent)
		{
			if ((GPIOx->DATA & iocurrent) != GPIO_PIN_RESET)
			{
				CLEAR_BIT(GPIOx->DATA, iocurrent);
			}
			else
			{
				SET_BIT(GPIOx->DATA, iocurrent);
			}
		}
		
		position++;
	}
	WRITE_REG(GPIOx->DATA_B_EN, data_en);
}

void HAL_GPIO_EXTI_IRQHandler(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin)
{
	if ((GPIOx->MIS & GPIO_Pin) != 0)
	{
		SET_BIT(GPIOx->IC, GPIO_Pin);
		HAL_GPIO_EXTI_Callback(GPIOx, GPIO_Pin);
	}
}

__attribute__((weak)) void HAL_GPIO_EXTI_Callback(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin)
{
	UNUSED(GPIO_Pin);
}