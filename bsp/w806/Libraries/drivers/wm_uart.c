#include "wm_uart.h"


__attribute__((weak)) void HAL_UART_MspInit(UART_HandleTypeDef *huart);
__attribute__((weak)) void HAL_UART_MspDeInit(UART_HandleTypeDef *huart);
static HAL_StatusTypeDef UART_Transmit_IT(UART_HandleTypeDef *huart);
static HAL_StatusTypeDef UART_EndTransmit_IT(UART_HandleTypeDef *huart);
static HAL_StatusTypeDef UART_Receive_IT(UART_HandleTypeDef *huart);
static HAL_StatusTypeDef UART_WaitOnFlagUntilTimeout(UART_HandleTypeDef *huart, uint32_t Flag, uint8_t Status, uint32_t Tickstart, uint32_t Timeout);
static void UART_SetConfig(UART_HandleTypeDef *huart);

HAL_StatusTypeDef HAL_UART_Init(UART_HandleTypeDef *huart)
{
	if (huart == NULL)
	{
		return HAL_ERROR;
	}
	
	assert_param(IS_UART_INSTANCE(huart->Instance));
	
	if (huart->gState == HAL_UART_STATE_RESET)
	{
		huart->Lock = HAL_UNLOCKED;

		HAL_UART_MspInit(huart);
	}

	huart->gState = HAL_UART_STATE_BUSY;

	__HAL_UART_DISABLE(huart);

	UART_SetConfig(huart);

	__HAL_UART_ENABLE(huart);

	huart->ErrorCode = HAL_UART_ERROR_NONE;
	huart->gState = HAL_UART_STATE_READY;
	huart->RxState = HAL_UART_STATE_READY;

	return HAL_OK;
}

HAL_StatusTypeDef HAL_UART_DeInit(UART_HandleTypeDef *huart)
{
	if (huart == NULL)
	{
		return HAL_ERROR;
	}

	assert_param(IS_UART_INSTANCE(huart->Instance));

	huart->gState = HAL_UART_STATE_BUSY;

	__HAL_UART_DISABLE(huart);
	HAL_UART_MspDeInit(huart);

	huart->ErrorCode = HAL_UART_ERROR_NONE;
	huart->gState = HAL_UART_STATE_RESET;
	huart->RxState = HAL_UART_STATE_RESET;

	__HAL_UNLOCK(huart);

	return HAL_OK;
}

__attribute__((weak)) void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	UNUSED(huart);
}

__attribute__((weak)) void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
	UNUSED(huart);
}

HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
	uint8_t  *pdata8bits;
	uint32_t tickstart = 0U;
	
	if (huart->gState == HAL_UART_STATE_READY)
	{
		if ((pData == NULL) || (Size == 0))
		{
			return  HAL_ERROR;
		}
		
		__HAL_LOCK(huart);

		huart->ErrorCode = HAL_UART_ERROR_NONE;
		huart->gState = HAL_UART_STATE_BUSY_TX;

		tickstart = HAL_GetTick();

		huart->TxXferSize = Size;
		huart->TxXferCount = Size;
		pdata8bits  = pData;

		__HAL_UNLOCK(huart);

		while (huart->TxXferCount > 0)
		{
			if (UART_WaitOnFlagUntilTimeout(huart, UART_FIFOS_TFC, UART_FIFO_TX_NOT_FULL, tickstart, Timeout) != HAL_OK)
			{
				return HAL_TIMEOUT;
			}
			huart->Instance->TDW = *pdata8bits;
			pdata8bits++;
			huart->TxXferCount--;
		}

		if (UART_WaitOnFlagUntilTimeout(huart, UART_FIFOS_TFC, UART_FIFO_TX_EMPTY, tickstart, Timeout) != HAL_OK)
		{
			return HAL_TIMEOUT;
		}

		huart->gState = HAL_UART_STATE_READY;

		return HAL_OK;
	}
	else
	{
		return HAL_BUSY;
	}
}

HAL_StatusTypeDef HAL_UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
	uint8_t  *pdata8bits;
	uint32_t tickstart = 0U;

	if (huart->RxState == HAL_UART_STATE_READY)
	{
		if ((pData == NULL) || (Size == 0U))
		{
			return  HAL_ERROR;
		}

		__HAL_LOCK(huart);

		huart->ErrorCode = HAL_UART_ERROR_NONE;
		huart->RxState = HAL_UART_STATE_BUSY_RX;

		tickstart = HAL_GetTick();

		huart->RxXferSize = Size;
		huart->RxXferCount = Size;
		
		pdata8bits  = pData;
		
		__HAL_UNLOCK(huart);

		while (huart->RxXferCount > 0U)
		{
			if (UART_WaitOnFlagUntilTimeout(huart, UART_FIFOS_RFC, UART_FIFO_RX_NOT_EMPTY, tickstart, Timeout) != HAL_OK)
			{
				return HAL_TIMEOUT;
			}
			*pdata8bits = huart->Instance->RDW;
			pdata8bits++;
			huart->RxXferCount--;
		}

		/* At end of Rx process, restore huart->RxState to Ready */
		huart->RxState = HAL_UART_STATE_READY;

		return HAL_OK;
	}
	else
	{
		return HAL_BUSY;
	}
}

HAL_StatusTypeDef HAL_UART_Transmit_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
	if (huart->gState == HAL_UART_STATE_READY)
	{
		if ((pData == NULL) || (Size == 0))
		{
			return HAL_ERROR;
		}

		__HAL_LOCK(huart);

		huart->pTxBuffPtr = pData;
		huart->TxXferSize = Size;
		huart->TxXferCount = Size;

		huart->ErrorCode = HAL_UART_ERROR_NONE;
		huart->gState = HAL_UART_STATE_BUSY_TX;

		__HAL_UNLOCK(huart);

		__HAL_UART_ENABLE_IT(huart, UART_TX_INT_FLAG);
		
		do {
			if ((huart->Instance->FIFOS & UART_FIFOS_TFC) == UART_FIFO_FULL)
			{
				break;
			}
			huart->Instance->TDW = *(huart->pTxBuffPtr);
			huart->pTxBuffPtr++;
			huart->TxXferCount--;
		} while(0);

		return HAL_OK;
	}
	else
	{
		return HAL_BUSY;
	}
}

HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
	if (huart->RxState == HAL_UART_STATE_READY)
	{
		if (pData == NULL)
		{
			return HAL_ERROR;
		}

		__HAL_LOCK(huart);
		
		huart->pRxBuffPtr = pData;
		huart->RxXferSize = Size;
		huart->RxXferCount = 0;

		huart->ErrorCode = HAL_UART_ERROR_NONE;
		huart->RxState = HAL_UART_STATE_BUSY_RX;

		__HAL_UNLOCK(huart);

		__HAL_UART_CLEAR_FLAG(huart, UART_RX_INT_FLAG);
		__HAL_UART_ENABLE_IT(huart, UART_RX_INT_FLAG);
		
		return HAL_OK;
	}
	else
	{
		return HAL_BUSY;
	}
}

void HAL_UART_IRQHandler(UART_HandleTypeDef *huart)
{
	uint32_t isrflags   = READ_REG(huart->Instance->INTS);
	uint32_t isrmasks   = READ_REG(huart->Instance->INTM);

	__HAL_UART_CLEAR_FLAG(huart, isrflags);
	if (((isrflags & UART_RX_INT_FLAG) != RESET) && ((isrmasks & UART_RX_INT_FLAG) == RESET))
	{
		if ((isrflags & UART_RX_ERR_INT_FLAG) != RESET)
		{
			huart->ErrorCode = isrflags & UART_RX_ERR_INT_FLAG;
		}
		else
		{
			huart->ErrorCode = HAL_UART_ERROR_NONE;
		}
		
		if(isrflags&UART_INTS_RTO)
		{
			huart->RxFlag=1;
		}
		UART_Receive_IT(huart);
	}
	
	if (((isrflags & UART_INTS_TL) != RESET) && ((isrmasks & UART_INTM_RL) == RESET))
	{
		UART_Transmit_IT(huart);
	}

	if (((isrflags & UART_INTS_TEMPT) != RESET) && ((isrmasks & UART_INTM_TEMPT) == RESET))
	{
		UART_EndTransmit_IT(huart);
	}
}

__attribute__((weak)) void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	UNUSED(huart);
}

__attribute__((weak)) void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  UNUSED(huart);
}

static HAL_StatusTypeDef UART_WaitOnFlagUntilTimeout(UART_HandleTypeDef *huart, uint32_t Flag, uint8_t Status, uint32_t Tickstart, uint32_t Timeout)
{
	while (1)
	{
		if (Status == UART_FIFO_TX_NOT_FULL)
		{
			if ((huart->Instance->FIFOS & Flag) < UART_FIFO_FULL)
				break;
		}
		else if (Status == UART_FIFO_TX_EMPTY)
		{
			if ((huart->Instance->FIFOS & Flag) == 0)
				break;
		}
		else if (Status == UART_FIFO_RX_NOT_EMPTY)
		{
			if ((huart->Instance->FIFOS & Flag) > 0)
				break;
		}

		if (Timeout != HAL_MAX_DELAY)
		{
			if ((Timeout == 0U) || ((HAL_GetTick() - Tickstart) > Timeout))
			{
				huart->gState  = HAL_UART_STATE_READY;
				huart->RxState = HAL_UART_STATE_READY;

				__HAL_UNLOCK(huart);

				return HAL_TIMEOUT;
			}
		}
	}
	return HAL_OK;
}

static HAL_StatusTypeDef UART_Transmit_IT(UART_HandleTypeDef *huart)
{
	if (huart->gState == HAL_UART_STATE_BUSY_TX)
	{
		while (huart->TxXferCount > 0)
		{
			if ((huart->Instance->FIFOS & UART_FIFOS_TFC) == UART_FIFO_FULL)
			{
				break;
			}
			huart->Instance->TDW = *(huart->pTxBuffPtr);
			huart->pTxBuffPtr++;
			huart->TxXferCount--;
		}
		return HAL_OK;
	}
	else
	{
		return HAL_BUSY;
	}
}

static HAL_StatusTypeDef UART_EndTransmit_IT(UART_HandleTypeDef *huart)
{
	if (huart->TxXferCount == 0)
	{
		__HAL_UART_DISABLE_IT(huart, UART_INTM_TL | UART_INTM_TEMPT);
		huart->gState = HAL_UART_STATE_READY;
		HAL_UART_TxCpltCallback(huart);
	}

	return HAL_OK;
}

static HAL_StatusTypeDef UART_Receive_IT(UART_HandleTypeDef *huart)
{
	uint8_t  ch, count;

	if (huart->RxState == HAL_UART_STATE_BUSY_RX)
	{
		count = ((READ_REG(huart->Instance->FIFOS) & UART_FIFOS_RFC) >> UART_FIFOS_RFC_Pos);
		while (count-- > 0)
		{
			ch = (uint8_t)(huart->Instance->RDW);
			if ((huart->ErrorCode & UART_RX_ERR_INT_FLAG) != RESET)
			{
				continue;
			}
			*(huart->pRxBuffPtr) = ch;
			huart->pRxBuffPtr++;
			huart->RxXferCount++;
			if (huart->RxXferSize > 0)
			{
				if (huart->RxXferCount == huart->RxXferSize)
				{
					huart->pRxBuffPtr -= huart->RxXferSize;
					HAL_UART_RxCpltCallback(huart);
					huart->RxXferCount = 0;
				}
			}
			else
			{
				if (count == 0)
				{
					huart->pRxBuffPtr -= huart->RxXferCount;
					HAL_UART_RxCpltCallback(huart);
					huart->RxXferCount = 0;
				}
			}
		}
		return HAL_OK;
	}
	else
	{
		return HAL_BUSY;
	}
}

static void UART_SetConfig(UART_HandleTypeDef *huart)
{
	uint32_t value;
	uint32_t apbclk;
	wm_sys_clk sysclk;

	assert_param(IS_UART_BAUDRATE(huart->Init.BaudRate));
	assert_param(IS_UART_STOPBITS(huart->Init.StopBits));
	assert_param(IS_UART_PARITY(huart->Init.Parity));
	assert_param(IS_UART_WORD_LENGTH(huart->Init.WordLength));
	assert_param(IS_UART_MODE(huart->Init.Mode));
	assert_param(IS_UART_HARDWARE_FLOW_CONTROL(huart->Init.HwFlowCtl));
  
  
	MODIFY_REG(huart->Instance->LC,
             (uint32_t)(UART_LC_RE | UART_LC_TE | UART_LC_PS | UART_LC_PCE | UART_LC_STOP | UART_LC_DATAL),
             (uint32_t)(huart->Init.WordLength | huart->Init.Parity | huart->Init.Mode | huart->Init.StopBits));
	if (huart->Instance == UART2)
	{
		CLEAR_BIT(huart->Instance->LC, (1 << 24));
	}
			 
	MODIFY_REG(huart->Instance->FC, (uint32_t)(UART_FC_RTSL | UART_FC_AFCE), (uint32_t)(UART_FC_RTSL_24 | huart->Init.HwFlowCtl));
	
	MODIFY_REG(huart->Instance->DMAC, (uint32_t)(UART_DMAC_RTO | UART_DMAC_RTOE), (uint32_t)((4 << 3) | UART_DMAC_RTOE));
	
	MODIFY_REG(huart->Instance->FIFOC, 
			(uint32_t)(UART_FIFOC_RFL | UART_FIFOC_TFL | UART_FIFOC_RFRST | UART_FIFOC_TFRST), 
			(uint32_t)(UART_FIFOC_RFL_16 | UART_FIFOC_TFL_16 | UART_FIFOC_RFRST | UART_FIFOC_TFRST));
	
	SystemClock_Get(&sysclk);
	apbclk = sysclk.apbclk * 1000000;
    value = (apbclk / (16 * (huart->Init.BaudRate)) - 1) |
            (((apbclk % ((huart->Init.BaudRate) * 16)) * 16 / ((huart->Init.BaudRate) * 16)) << 16);
			
	huart->Instance->BAUDR = value;
}