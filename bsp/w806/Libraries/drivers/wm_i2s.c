#include "wm_i2s.h"
#include <math.h>

#define SYS_I2S_CLK		(160000000)

static void I2S_DMATxCplt(DMA_HandleTypeDef *hdma);
static void I2S_DMATxHalfCplt(DMA_HandleTypeDef *hdma);
static void I2S_DMARxCplt(DMA_HandleTypeDef *hdma);
static void I2S_DMARxHalfCplt(DMA_HandleTypeDef *hdma);
static void I2S_Transmit_IT(I2S_HandleTypeDef *hi2s);
static void I2S_EndTransmit_IT(I2S_HandleTypeDef *hi2s);
static void I2S_Receive_IT(I2S_HandleTypeDef *hi2s);
static void I2S_TxRxDMAHalfCplt(DMA_HandleTypeDef *hdma);
static void I2S_TxRxDMACplt(DMA_HandleTypeDef *hdma);

/**
  * @brief  Initializes the I2S according to the specified parameters
  *         in the I2S_InitTypeDef and create the associated handle.
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2S_Init(I2S_HandleTypeDef *hi2s)
{
	uint32_t mclk_div, ws_div, datalen = 0;
	
	if (hi2s == NULL)
	{
		return HAL_ERROR;
	}
	
	assert_param(IS_I2S_ALL_INSTANCE(hi2s->Instance));
	assert_param(IS_I2S_MODE(hi2s->Init.Mode));
	assert_param(IS_I2S_STANDARD(hi2s->Init.Standard));
	assert_param(IS_I2S_DATA_FORMAT(hi2s->Init.DataFormat));
	assert_param(IS_I2S_MCLK_OUTPUT(hi2s->Init.MCLKOutput));
	assert_param(IS_I2S_AUDIO_FREQ(hi2s->Init.AudioFreq));
	assert_param(IS_I2S_CHANNEL(hi2s->Init.Channel));
	
	if (hi2s->State == HAL_I2S_STATE_RESET)
	{
		hi2s->Lock = HAL_UNLOCKED;
		HAL_I2S_MspInit(hi2s);
	}
	hi2s->State = HAL_I2S_STATE_BUSY;
	hi2s->Instance->CR = 0;
	WRITE_REG(hi2s->Instance->CR, (hi2s->Init.Mode | hi2s->Init.Standard | hi2s->Init.DataFormat | hi2s->Init.Channel));
	SET_BIT(hi2s->Instance->CR, (I2S_CR_RXFIFO_CLR | I2S_CR_TXFIFO_CLR | I2S_CR_RXFIFO_TH_1 | I2S_CR_TXFIFO_TH_4));
	if (hi2s->Init.Channel == I2S_CHANNEL_MONO)
	{
		assert_param(IS_I2S_CHANNELSEL(hi2s->Init.ChannelSel));
		SET_BIT(hi2s->Instance->CR, hi2s->Init.ChannelSel);
	}
	if (hi2s->Init.DataFormat == I2S_DATAFORMAT_8B)
	{
		datalen = 8;
	}
	else if (hi2s->Init.DataFormat == I2S_DATAFORMAT_16B)
	{
		datalen = 16;
	}
	else if (hi2s->Init.DataFormat == I2S_DATAFORMAT_24B)
	{
		datalen = 24;
	}
	else if (hi2s->Init.DataFormat == I2S_DATAFORMAT_32B)
	{
		datalen = 32;
	}
	
	ws_div = round((SYS_I2S_CLK / (hi2s->Init.AudioFreq * datalen * 2)));
	if (hi2s->Init.MCLKOutput == I2S_MCLKOUTPUT_ENABLE)
	{
		mclk_div = round(SYS_I2S_CLK / (hi2s->Init.AudioFreq * 256));
		if (mclk_div > 63)
		{
			mclk_div = 63;
		}
		MODIFY_REG(RCC->I2S_CLK, (RCC_I2S_CLK_MCLKDIV | RCC_I2S_CLK_MCLK_EN), ((mclk_div << RCC_I2S_CLK_MCLKDIV_Pos) | RCC_I2S_CLK_MCLK_EN));
	}
	MODIFY_REG(RCC->I2S_CLK, RCC_I2S_CLK_BCLKDIV, (ws_div << RCC_I2S_CLK_BCLKDIV_Pos));
	hi2s->ErrorCode = HAL_I2S_ERROR_NONE;
	hi2s->State = HAL_I2S_STATE_READY;
	
	return HAL_OK;
}

/**
  * @brief DeInitializes the I2S peripheral
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2S_DeInit(I2S_HandleTypeDef *hi2s)
{
	if (hi2s == NULL)
	{
		return HAL_ERROR;
	}
	
	assert_param(IS_I2S_ALL_INSTANCE(hi2s->Instance));
	hi2s->State = HAL_I2S_STATE_BUSY;
	__HAL_I2S_DISABLE(hi2s);
	HAL_I2S_MspDeInit(hi2s);
	
	hi2s->ErrorCode = HAL_I2S_ERROR_NONE;
	hi2s->State = HAL_I2S_STATE_RESET;
	
	__HAL_UNLOCK(hi2s);
	
	return HAL_OK;
}

/**
  * @brief I2S MSP Init
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval None
  */
__attribute__((weak)) void HAL_I2S_MspInit(I2S_HandleTypeDef *hi2s)
{
	UNUSED(hi2s);
}

/**
  * @brief I2S MSP DeInit
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval None
  */
__attribute__((weak)) void HAL_I2S_MspDeInit(I2S_HandleTypeDef *hi2s)
{
	UNUSED(hi2s);
}

/**
  * @brief  Transmit an amount of data in blocking mode
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @param  pData a 32-bit pointer to data buffer.
  * @param  Size number of data sample to be sent:
  * @note   When DataFormat = I2S_DATAFORMAT_8B,  1 pData : 4 data sample. E.g: if you want to send 100 8bit data sample, Size = 100
  * 		When DataFormat = I2S_DATAFORMAT_16B, 1 pData : 2 data sample. E.g: if you want to send 100 16bit data sample, Size = 100
  * 		When DataFormat = I2S_DATAFORMAT_24B, 1 pData : 1 data sample. E.g: if you want to send 100 24bit data sample, Size = 100
  * 		When DataFormat = I2S_DATAFORMAT_32B, 1 pData : 1 data sample. E.g: if you want to send 100 32bit data sample, Size = 100
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2S_Transmit(I2S_HandleTypeDef *hi2s, uint32_t *pData, uint32_t Size, uint32_t Timeout)
{
	uint32_t tickstart = 0U, fifo_count = 0, i = 0;
	
	if ((pData == NULL) || (Size == 0U))
	{
		return HAL_ERROR;
	}
	__HAL_LOCK(hi2s);
	if (hi2s->State != HAL_I2S_STATE_READY)
	{
		__HAL_UNLOCK(hi2s);
		return HAL_BUSY;
	}
	hi2s->State = HAL_I2S_STATE_BUSY_TX;
	hi2s->ErrorCode = HAL_I2S_ERROR_NONE;
	tickstart = HAL_GetTick();

	hi2s->pTxBuffPtr = pData;
	if (hi2s->Init.DataFormat == I2S_DATAFORMAT_8B)
	{
		hi2s->TxXferSize = (Size + 3) / 4;
		hi2s->TxXferCount = (Size + 3) / 4;
	}
	else if (hi2s->Init.DataFormat == I2S_DATAFORMAT_16B)
	{
		hi2s->TxXferSize = (Size + 1) / 2;
		hi2s->TxXferCount = (Size + 1) / 2;
	}
	else
	{
		hi2s->TxXferSize = Size;
		hi2s->TxXferCount = Size;
	}
	SET_BIT(hi2s->Instance->CR, I2S_CR_TXFIFO_CLR);
	while (hi2s->TxXferCount > 0)
	{
		fifo_count = I2S_FIFO_FULL - __HAL_I2S_GET_TXFIFO(hi2s);
		fifo_count = (fifo_count > hi2s->TxXferCount) ? hi2s->TxXferCount : fifo_count;
		for (i = 0; i < fifo_count; i++)
		{
			hi2s->Instance->TXDR = (*hi2s->pTxBuffPtr);
			hi2s->pTxBuffPtr++;
			hi2s->TxXferCount--;
		}
		if ((hi2s->Instance->CR & I2S_CR_EN) != I2S_CR_EN)
		{
			__HAL_I2S_ENABLE(hi2s);
		}
		if ((hi2s->Instance->CR & I2S_CR_TXEN) != I2S_CR_TXEN)
		{
			__HAL_I2S_ENABLE_TX(hi2s);
		}
		if ((((HAL_GetTick() - tickstart) >= Timeout) && (Timeout != HAL_MAX_DELAY)) || (Timeout == 0))
		{
			__HAL_I2S_DISABLE_TX(hi2s);
			hi2s->ErrorCode = HAL_I2S_ERROR_TIMEOUT;
			hi2s->State = HAL_I2S_STATE_READY;
			__HAL_UNLOCK(hi2s);
			
			return HAL_ERROR;
		}
	}	
	while (__HAL_I2S_GET_IT(hi2s, I2S_IF_TX_UNDERFLOW) != I2S_IF_TX_UNDERFLOW)
	{
		if ((((HAL_GetTick() - tickstart) >= Timeout) && (Timeout != HAL_MAX_DELAY)) || (Timeout == 0))
		{
			__HAL_I2S_DISABLE_TX(hi2s);
			hi2s->ErrorCode = HAL_I2S_ERROR_TIMEOUT;
			hi2s->State = HAL_I2S_STATE_READY;
			__HAL_UNLOCK(hi2s);
			
			return HAL_ERROR;
		}
	}
	__HAL_I2S_DISABLE_TX(hi2s);
	hi2s->State = HAL_I2S_STATE_READY;
	__HAL_UNLOCK(hi2s);
	return HAL_OK;
}

/**
  * @brief  Receive an amount of data in blocking mode
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @param  pData a 32-bit pointer to data buffer.
  * @param  Size number of data sample to be received:
  * @note   When DataFormat = I2S_DATAFORMAT_8B,  1 pData : 4 data sample. E.g: if you want to receive 100 8bit data sample, Size = 100
  * 		When DataFormat = I2S_DATAFORMAT_16B, 1 pData : 2 data sample. E.g: if you want to receive 100 16bit data sample, Size = 100
  * 		When DataFormat = I2S_DATAFORMAT_24B, 1 pData : 1 data sample. E.g: if you want to receive 100 24bit data sample, Size = 100
  * 		When DataFormat = I2S_DATAFORMAT_32B, 1 pData : 1 data sample. E.g: if you want to receive 100 32bit data sample, Size = 100
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2S_Receive(I2S_HandleTypeDef *hi2s, uint32_t *pData, uint32_t Size, uint32_t Timeout)
{
	uint32_t tickstart = 0, fifo_count = 0, i = 0;
	
	if ((pData == NULL) || (Size == 0U))
	{
		return HAL_ERROR;
	}
	__HAL_LOCK(hi2s);
	if (hi2s->State != HAL_I2S_STATE_READY)
	{
		__HAL_UNLOCK(hi2s);
		return HAL_BUSY;
	}
	
	hi2s->State = HAL_I2S_STATE_BUSY_RX;
	hi2s->ErrorCode = HAL_I2S_ERROR_NONE;
	hi2s->pRxBuffPtr = pData;
	if (hi2s->Init.DataFormat == I2S_DATAFORMAT_8B)
	{
		hi2s->RxXferSize = (Size + 3) / 4;
		hi2s->RxXferCount = (Size + 3) / 4;
	}
	else if (hi2s->Init.DataFormat == I2S_DATAFORMAT_16B)
	{
		hi2s->RxXferSize = (Size + 1) / 2;
		hi2s->RxXferCount = (Size + 1) / 2;
	}
	else
	{
		hi2s->RxXferSize = Size;
		hi2s->RxXferCount = Size;
	}
	
	if ((hi2s->Instance->CR & I2S_CR_EN) != I2S_CR_EN)
	{
		__HAL_I2S_ENABLE(hi2s);
	}
	__HAL_I2S_ENABLE_RX(hi2s);
	while (hi2s->RxXferCount > 0)
	{
		fifo_count = __HAL_I2S_GET_RXFIFO(hi2s);
		fifo_count = (fifo_count > hi2s->RxXferCount) ? hi2s->RxXferCount : fifo_count;
		for (i = 0; i < fifo_count; i++)
		{
			(*hi2s->pRxBuffPtr) = (uint32_t)hi2s->Instance->RXDR;
			hi2s->pRxBuffPtr++;
			hi2s->RxXferCount--;
		}
		if (__HAL_I2S_GET_IT(hi2s, I2S_IF_RX_OVERFLOW))
		{
			__HAL_I2S_CLEAR_IT(hi2s, I2S_IF_RX_OVERFLOW);
			SET_BIT(hi2s->ErrorCode, HAL_I2S_ERROR_RXERR);
		}
		if ((((HAL_GetTick() - tickstart) >= Timeout) && (Timeout != HAL_MAX_DELAY)) || (Timeout == 0))
		{
			__HAL_I2S_DISABLE_RX(hi2s);
			SET_BIT(hi2s->ErrorCode, HAL_I2S_ERROR_TIMEOUT);
			hi2s->State = HAL_I2S_STATE_READY;
			__HAL_UNLOCK(hi2s);
			
			return HAL_ERROR;
		}
	}
	__HAL_I2S_DISABLE_RX(hi2s);
	hi2s->State = HAL_I2S_STATE_READY;
	__HAL_UNLOCK(hi2s);
	
	return HAL_OK;
}

/**
  * @brief  Full-Duplex Transmit/Receive data in blocking mode.
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @param  pTxData a 32-bit pointer to the Transmit data buffer.
  * @param  pRxData a 32-bit pointer to the Receive data buffer.
  * @param  Size number of data sample to be transmission:
  * @note   When DataFormat = I2S_DATAFORMAT_8B,  1 pData : 4 data sample. E.g: if you want to transmission 100 8bit data sample, Size = 100
  * 		When DataFormat = I2S_DATAFORMAT_16B, 1 pData : 2 data sample. E.g: if you want to transmission 100 16bit data sample, Size = 100
  * 		When DataFormat = I2S_DATAFORMAT_24B, 1 pData : 1 data sample. E.g: if you want to transmission 100 24bit data sample, Size = 100
  * 		When DataFormat = I2S_DATAFORMAT_32B, 1 pData : 1 data sample. E.g: if you want to transmission 100 32bit data sample, Size = 100
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2S_TransmitReceive(I2S_HandleTypeDef *hi2s, uint32_t *pTxData, uint32_t *pRxData,
                                            uint32_t Size, uint32_t Timeout)
{
	HAL_StatusTypeDef errorcode = HAL_OK;
	uint32_t tickstart = 0U, fifo_count = 0, i = 0;
	
	if (hi2s->State != HAL_I2S_STATE_READY)
	{
		errorcode = HAL_BUSY;
		goto error;
	}
	
	if ((pTxData == NULL) || (pRxData == NULL) || (Size == 0U))
	{
		return HAL_ERROR;
	}
	
	__HAL_LOCK(hi2s);
	hi2s->pTxBuffPtr = pTxData;
	hi2s->pRxBuffPtr = pRxData;
	if (hi2s->Init.DataFormat == I2S_DATAFORMAT_8B)
	{
		hi2s->TxXferSize = (Size + 3) / 4;
		hi2s->TxXferCount = (Size + 3) / 4;
		hi2s->RxXferSize = (Size + 3) / 4;
		hi2s->RxXferCount = (Size + 3) / 4;
	}
	else if (hi2s->Init.DataFormat == I2S_DATAFORMAT_16B)
	{
		hi2s->TxXferSize = (Size + 1) / 2;
		hi2s->TxXferCount = (Size + 1) / 2;
		hi2s->RxXferSize = (Size + 1) / 2;
		hi2s->RxXferCount = (Size + 1) / 2;
	}
	else
	{
		hi2s->TxXferSize = Size;
		hi2s->TxXferCount = Size;
		hi2s->RxXferSize = Size;
		hi2s->RxXferCount = Size;
	}
	hi2s->ErrorCode = HAL_I2S_ERROR_NONE;
	hi2s->State = HAL_I2S_STATE_BUSY_TXRX;
	tickstart = HAL_GetTick();
	
	SET_BIT(hi2s->Instance->CR, I2S_CR_TXFIFO_CLR);
	SET_BIT(hi2s->Instance->CR, I2S_CR_RXFIFO_CLR);
	
	fifo_count = I2S_FIFO_FULL - __HAL_I2S_GET_TXFIFO(hi2s);
	fifo_count = (fifo_count > hi2s->TxXferCount) ? hi2s->TxXferCount : fifo_count;
	for (i = 0; i < fifo_count; i++)
	{
		hi2s->Instance->TXDR = (*hi2s->pTxBuffPtr);
		hi2s->pTxBuffPtr++;
		hi2s->TxXferCount--;
	}
	if ((hi2s->Instance->CR & I2S_CR_EN) != I2S_CR_EN)
	{
		__HAL_I2S_ENABLE(hi2s);
	}
	if ((hi2s->Instance->CR & I2S_CR_TXEN) != I2S_CR_TXEN)
	{
		__HAL_I2S_ENABLE_TX(hi2s);
	}
	if ((hi2s->Instance->CR & I2S_CR_RXEN) != I2S_CR_RXEN)
	{
		__HAL_I2S_ENABLE_RX(hi2s);
	}
	
	while ((hi2s->RxXferCount > 0U) || (hi2s->TxXferCount > 0U))
	{
		fifo_count = I2S_FIFO_FULL - __HAL_I2S_GET_TXFIFO(hi2s);
		fifo_count = (fifo_count > hi2s->TxXferCount) ? hi2s->TxXferCount : fifo_count;
		for (i = 0; i < fifo_count; i++)
		{
			hi2s->Instance->TXDR = (*hi2s->pTxBuffPtr);
			hi2s->pTxBuffPtr++;
			hi2s->TxXferCount--;
		}
		if (__HAL_I2S_GET_IT(hi2s, I2S_IF_TX_UNDERFLOW))
		{
			__HAL_I2S_CLEAR_IT(hi2s, I2S_IF_TX_UNDERFLOW);
			if (hi2s->TxXferCount > 0)
			{
				SET_BIT(hi2s->ErrorCode, HAL_I2S_ERROR_TXERR);
			}
		}
		
		if ((((HAL_GetTick() - tickstart) >= Timeout) && (Timeout != HAL_MAX_DELAY)) || (Timeout == 0))
		{
			__HAL_I2S_DISABLE_TX(hi2s);
			__HAL_I2S_DISABLE_RX(hi2s);
			SET_BIT(hi2s->ErrorCode, HAL_I2S_ERROR_TIMEOUT);
			hi2s->State = HAL_I2S_STATE_READY;
			__HAL_UNLOCK(hi2s);
			
			errorcode = HAL_ERROR;
			goto error;
		}
		
		fifo_count = __HAL_I2S_GET_RXFIFO(hi2s);
		fifo_count = (fifo_count > hi2s->RxXferCount) ? hi2s->RxXferCount : fifo_count;
		for (i = 0; i < fifo_count; i++)
		{
			(*hi2s->pRxBuffPtr) = (uint32_t)hi2s->Instance->RXDR;
			hi2s->pRxBuffPtr++;
			hi2s->RxXferCount--;
		}
		if (__HAL_I2S_GET_IT(hi2s, I2S_IF_RX_OVERFLOW))
		{
			__HAL_I2S_CLEAR_IT(hi2s, I2S_IF_RX_OVERFLOW);
			if (hi2s->RxXferCount > 0)
			{
				SET_BIT(hi2s->ErrorCode, HAL_I2S_ERROR_RXERR);
			}
		}
	}
	
	if (hi2s->ErrorCode != HAL_I2S_ERROR_NONE)
	{
		errorcode = HAL_ERROR;
	}
	
error :
	__HAL_I2S_DISABLE_TX(hi2s);
	__HAL_I2S_DISABLE_RX(hi2s);
	hi2s->State = HAL_I2S_STATE_READY;
	__HAL_UNLOCK(hi2s);
	
	return errorcode;
}

/**
  * @brief  Transmit an amount of data in non-blocking mode with Interrupt
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @param  pData a 32-bit pointer to data buffer.
  * @param  Size number of data sample to be sent:
  * @note   When DataFormat = I2S_DATAFORMAT_8B,  1 pData : 4 data sample. E.g: if you want to send 100 8bit data sample, Size = 100
  * 		When DataFormat = I2S_DATAFORMAT_16B, 1 pData : 2 data sample. E.g: if you want to send 100 16bit data sample, Size = 100
  * 		When DataFormat = I2S_DATAFORMAT_24B, 1 pData : 1 data sample. E.g: if you want to send 100 24bit data sample, Size = 100
  * 		When DataFormat = I2S_DATAFORMAT_32B, 1 pData : 1 data sample. E.g: if you want to send 100 32bit data sample, Size = 100
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2S_Transmit_IT(I2S_HandleTypeDef *hi2s, uint32_t *pData, uint32_t Size)
{
	if ((pData == NULL) || (Size == 0U))
	{
		return HAL_ERROR;
	}
	__HAL_LOCK(hi2s);
	if (hi2s->State != HAL_I2S_STATE_READY)
	{
		__HAL_UNLOCK(hi2s);
		return HAL_BUSY;
	}
	
	hi2s->State = HAL_I2S_STATE_BUSY_TX;
	hi2s->ErrorCode = HAL_I2S_ERROR_NONE;
	hi2s->pTxBuffPtr = pData;
	if (hi2s->Init.DataFormat == I2S_DATAFORMAT_8B)
	{
		hi2s->TxXferSize = (Size + 3) / 4;
		hi2s->TxXferCount = (Size + 3) / 4;
	}
	else if (hi2s->Init.DataFormat == I2S_DATAFORMAT_16B)
	{
		hi2s->TxXferSize = (Size + 1) / 2;
		hi2s->TxXferCount = (Size + 1) / 2;
	}
	else
	{
		hi2s->TxXferSize = Size;
		hi2s->TxXferCount = Size;
	}
	
	__HAL_I2S_CLEAR_IT(hi2s, (I2S_IF_TXTH | I2S_IF_TXDONE | I2S_IF_TX_UNDERFLOW | I2S_IF_TX_OVERFLOW));
	__HAL_I2S_ENABLE_IT(hi2s, (I2S_IM_TXDONE | I2S_IM_TXTH | I2S_IM_TX_UNDERFLOW));
	if ((hi2s->Instance->CR & I2S_CR_EN) != I2S_CR_EN)
	{
		__HAL_I2S_ENABLE(hi2s);
	}
	I2S_Transmit_IT(hi2s);
	__HAL_I2S_ENABLE_TX(hi2s);
	__HAL_UNLOCK(hi2s);
	return HAL_OK;
}

/**
  * @brief  Receive an amount of data in non-blocking mode with Interrupt
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @param  pData a 32-bit pointer to the Receive data buffer.
  * @param  Size number of data sample to be received:
  * @note   When DataFormat = I2S_DATAFORMAT_8B,  1 pData : 4 data sample. E.g: if you want to receive 100 8bit data sample, Size = 100
  * 		When DataFormat = I2S_DATAFORMAT_16B, 1 pData : 2 data sample. E.g: if you want to receive 100 16bit data sample, Size = 100
  * 		When DataFormat = I2S_DATAFORMAT_24B, 1 pData : 1 data sample. E.g: if you want to receive 100 24bit data sample, Size = 100
  * 		When DataFormat = I2S_DATAFORMAT_32B, 1 pData : 1 data sample. E.g: if you want to receive 100 32bit data sample, Size = 100
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2S_Receive_IT(I2S_HandleTypeDef *hi2s, uint32_t *pData, uint32_t Size)
{
	if ((pData == NULL) || (Size == 0U))
	{
		return HAL_ERROR;
	}
	__HAL_LOCK(hi2s);
	if (hi2s->State != HAL_I2S_STATE_READY)
	{
		__HAL_UNLOCK(hi2s);
		return HAL_BUSY;
	}
	
	hi2s->State = HAL_I2S_STATE_BUSY_RX;
	hi2s->ErrorCode = HAL_I2S_ERROR_NONE;
	hi2s->pRxBuffPtr = pData;
	if (hi2s->Init.DataFormat == I2S_DATAFORMAT_8B)
	{
		hi2s->RxXferSize = (Size + 3) / 4;
		hi2s->RxXferCount = (Size + 3) / 4;
	}
	else if (hi2s->Init.DataFormat == I2S_DATAFORMAT_16B)
	{
		hi2s->RxXferSize = (Size + 1) / 2;
		hi2s->RxXferCount = (Size + 1) / 2;
	}
	else
	{
		hi2s->RxXferSize = Size;
		hi2s->RxXferCount = Size;
	}
	
	__HAL_I2S_CLEAR_IT(hi2s, (I2S_IF_RXTH | I2S_IF_RXDONE | I2S_IF_RX_OVERFLOW | I2S_IF_RX_UNDERFLOW));
	__HAL_I2S_ENABLE_IT(hi2s, (I2S_IM_RXDONE | I2S_IM_RXTH | I2S_IM_RX_OVERFLOW));
	if ((hi2s->Instance->CR & I2S_CR_EN) != I2S_CR_EN)
	{
		__HAL_I2S_ENABLE(hi2s);
	}
	__HAL_I2S_ENABLE_RX(hi2s);
	__HAL_UNLOCK(hi2s);
	return HAL_OK;
}

/**
  * @brief  Full-Duplex Transmit/Receive data in non-blocking mode using Interrupt
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @param  pTxData a 32-bit pointer to the Transmit data buffer.
  * @param  pRxData a 32-bit pointer to the Receive data buffer.
  * @param  Size number of data sample to be transmission:
  * @note   When DataFormat = I2S_DATAFORMAT_8B,  1 pData : 4 data sample. E.g: if you want to transmission 100 8bit data sample, Size = 100
  * 		When DataFormat = I2S_DATAFORMAT_16B, 1 pData : 2 data sample. E.g: if you want to transmission 100 16bit data sample, Size = 100
  * 		When DataFormat = I2S_DATAFORMAT_24B, 1 pData : 1 data sample. E.g: if you want to transmission 100 24bit data sample, Size = 100
  * 		When DataFormat = I2S_DATAFORMAT_32B, 1 pData : 1 data sample. E.g: if you want to transmission 100 32bit data sample, Size = 100
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2S_TransmitReceive_IT(I2S_HandleTypeDef *hi2s, uint32_t *pTxData, uint32_t *pRxData,
                                               uint32_t Size)
{
	if (hi2s->State != HAL_I2S_STATE_READY)
	{
		return HAL_BUSY;
	}
	
	if ((pTxData == NULL) || (pRxData == NULL) || (Size == 0U))
	{
		return HAL_ERROR;
	}
	
	__HAL_LOCK(hi2s);
	hi2s->pTxBuffPtr = pTxData;
	hi2s->pRxBuffPtr = pRxData;
	if (hi2s->Init.DataFormat == I2S_DATAFORMAT_8B)
	{
		hi2s->TxXferSize = (Size + 3) / 4;
		hi2s->TxXferCount = (Size + 3) / 4;
		hi2s->RxXferSize = (Size + 3) / 4;
		hi2s->RxXferCount = (Size + 3) / 4;
	}
	else if (hi2s->Init.DataFormat == I2S_DATAFORMAT_16B)
	{
		hi2s->TxXferSize = (Size + 1) / 2;
		hi2s->TxXferCount = (Size + 1) / 2;
		hi2s->RxXferSize = (Size + 1) / 2;
		hi2s->RxXferCount = (Size + 1) / 2;
	}
	else
	{
		hi2s->TxXferSize = Size;
		hi2s->TxXferCount = Size;
		hi2s->RxXferSize = Size;
		hi2s->RxXferCount = Size;
	}
	hi2s->ErrorCode = HAL_I2S_ERROR_NONE;
	hi2s->State = HAL_I2S_STATE_BUSY_TXRX;
	
	SET_BIT(hi2s->Instance->CR, I2S_CR_TXFIFO_CLR);
	SET_BIT(hi2s->Instance->CR, I2S_CR_RXFIFO_CLR);
	
	__HAL_I2S_CLEAR_IT(hi2s, (I2S_IF_TXTH | I2S_IF_TXDONE | I2S_IF_TX_UNDERFLOW | I2S_IF_TX_OVERFLOW | 
							  I2S_IF_RXTH | I2S_IF_RXDONE | I2S_IF_RX_UNDERFLOW | I2S_IF_RX_OVERFLOW));
	__HAL_I2S_ENABLE_IT(hi2s, (I2S_IM_TXDONE | I2S_IM_TXTH | I2S_IM_TX_UNDERFLOW | 
							   I2S_IM_RXDONE | I2S_IM_RXTH | I2S_IM_RX_OVERFLOW));
	if ((hi2s->Instance->CR & I2S_CR_EN) != I2S_CR_EN)
	{
		__HAL_I2S_ENABLE(hi2s);
	}
	I2S_Transmit_IT(hi2s);
	__HAL_I2S_ENABLE_TX(hi2s);
	__HAL_I2S_ENABLE_RX(hi2s);
	__HAL_UNLOCK(hi2s);
	return HAL_OK;
}

/**
  * @brief  Transmit an amount of data in non-blocking mode with DMA
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @param  pData a 32-bit pointer to the Transmit data buffer.
  * @param  Size number of data sample to be sent:
  * @note   When DataFormat = I2S_DATAFORMAT_8B,  1 pData : 4 data sample. E.g: if you want to receive 100 8bit data sample, Size = 100
  * 		When DataFormat = I2S_DATAFORMAT_16B, 1 pData : 2 data sample. E.g: if you want to receive 100 16bit data sample, Size = 100
  * 		When DataFormat = I2S_DATAFORMAT_24B, 1 pData : 1 data sample. E.g: if you want to receive 100 24bit data sample, Size = 100
  * 		When DataFormat = I2S_DATAFORMAT_32B, 1 pData : 1 data sample. E.g: if you want to receive 100 32bit data sample, Size = 100
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2S_Transmit_DMA(I2S_HandleTypeDef *hi2s, uint32_t *pData, uint32_t Size)
{
	if ((pData == NULL) || (Size == 0U))
	{
		return HAL_ERROR;
	}
	__HAL_LOCK(hi2s);
	if (hi2s->State != HAL_I2S_STATE_READY)
	{
		__HAL_UNLOCK(hi2s);
		return HAL_BUSY;
	}
	
	hi2s->State = HAL_I2S_STATE_BUSY_TX;
	hi2s->ErrorCode = HAL_I2S_ERROR_NONE;
	hi2s->pTxBuffPtr = pData;
	if (hi2s->Init.DataFormat == I2S_DATAFORMAT_8B)
	{
		hi2s->TxXferSize = (Size + 3) / 4;
		hi2s->TxXferCount = (Size + 3) / 4;
	}
	else if (hi2s->Init.DataFormat == I2S_DATAFORMAT_16B)
	{
		hi2s->TxXferSize = (Size + 1) / 2;
		hi2s->TxXferCount = (Size + 1) / 2;
	}
	else
	{
		hi2s->TxXferSize = Size;
		hi2s->TxXferCount = Size;
	}
	hi2s->hdmatx->XferHalfCpltCallback = I2S_DMATxHalfCplt;
	hi2s->hdmatx->XferCpltCallback = I2S_DMATxCplt;
	if (HAL_OK != HAL_DMA_Start_IT(hi2s->hdmatx, 
									(uint32_t)hi2s->pTxBuffPtr, 
									(uint32_t)&hi2s->Instance->TXDR, 
									(hi2s->TxXferSize * 4)))
	{
		hi2s->ErrorCode = HAL_I2S_ERROR_DMA;
		hi2s->State = HAL_I2S_STATE_READY;
		
		__HAL_UNLOCK(hi2s);
		return HAL_ERROR;
	}
	if ((hi2s->Instance->CR & I2S_CR_EN) != I2S_CR_EN)
	{
		__HAL_I2S_ENABLE(hi2s);
	}
	if ((hi2s->Instance->CR & I2S_CR_TXDMA_EN) != I2S_CR_TXDMA_EN)
	{
		SET_BIT(hi2s->Instance->CR, I2S_CR_TXDMA_EN);
	}
	__HAL_I2S_ENABLE_IT(hi2s, I2S_IF_TX_UNDERFLOW);
	__HAL_I2S_ENABLE_TX(hi2s);
	__HAL_UNLOCK(hi2s);
	return HAL_OK;
}

/**
  * @brief  Receive an amount of data in non-blocking mode with DMA
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @param  pData a 32-bit pointer to the Receive data buffer.
  * @param  Size number of data sample to be received:
  * @note   When DataFormat = I2S_DATAFORMAT_8B,  1 pData : 4 data sample. E.g: if you want to receive 100 8bit data sample, Size = 100
  * 		When DataFormat = I2S_DATAFORMAT_16B, 1 pData : 2 data sample. E.g: if you want to receive 100 16bit data sample, Size = 100
  * 		When DataFormat = I2S_DATAFORMAT_24B, 1 pData : 1 data sample. E.g: if you want to receive 100 24bit data sample, Size = 100
  * 		When DataFormat = I2S_DATAFORMAT_32B, 1 pData : 1 data sample. E.g: if you want to receive 100 32bit data sample, Size = 100
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2S_Receive_DMA(I2S_HandleTypeDef *hi2s, uint32_t *pData, uint32_t Size)
{
	if ((pData == NULL) || (Size == 0U))
	{
		return HAL_ERROR;
	}
	__HAL_LOCK(hi2s);
	if (hi2s->State != HAL_I2S_STATE_READY)
	{
		__HAL_UNLOCK(hi2s);
		return HAL_BUSY;
	}
	
	hi2s->State = HAL_I2S_STATE_BUSY_RX;
	hi2s->ErrorCode = HAL_I2S_ERROR_NONE;
	hi2s->pRxBuffPtr = pData;
	if (hi2s->Init.DataFormat == I2S_DATAFORMAT_8B)
	{
		hi2s->RxXferSize = (Size + 3) / 4;
		hi2s->RxXferCount = (Size + 3) / 4;
	}
	else if (hi2s->Init.DataFormat == I2S_DATAFORMAT_16B)
	{
		hi2s->RxXferSize = (Size + 1) / 2;
		hi2s->RxXferCount = (Size + 1) / 2;
	}
	else
	{
		hi2s->RxXferSize = Size;
		hi2s->RxXferCount = Size;
	}
	
	hi2s->hdmarx->XferHalfCpltCallback = I2S_DMARxHalfCplt;
	hi2s->hdmarx->XferCpltCallback = I2S_DMARxCplt;
	if (HAL_OK != HAL_DMA_Start_IT(hi2s->hdmarx,
									(uint32_t)&hi2s->Instance->RXDR,
									(uint32_t)hi2s->pRxBuffPtr,
									(hi2s->RxXferSize * 4)))
	{
		hi2s->ErrorCode = HAL_I2S_ERROR_DMA;
		hi2s->State = HAL_I2S_STATE_READY;
		__HAL_UNLOCK(hi2s);
		return HAL_ERROR;
	}
	if ((hi2s->Instance->CR & I2S_CR_EN) != I2S_CR_EN)
	{
		__HAL_I2S_ENABLE(hi2s);
	}
	
	if ((hi2s->Instance->CR & I2S_CR_RXDMA_EN) != I2S_CR_RXDMA_EN)
	{
		SET_BIT(hi2s->Instance->CR, I2S_CR_RXDMA_EN);
	}
	__HAL_I2S_ENABLE_RX(hi2s);
	__HAL_UNLOCK(hi2s);
	return HAL_OK;
}

/**
  * @brief  Full-Duplex Transmit/Receive data in non-blocking mode using DMA
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @param  pTxData a 32-bit pointer to the Transmit data buffer.
  * @param  pRxData a 32-bit pointer to the Receive data buffer.
  * @param  Size number of data sample to be transmission:
  * @note   When DataFormat = I2S_DATAFORMAT_8B,  1 pData : 4 data sample. E.g: if you want to transmission 100 8bit data sample, Size = 100
  * 		When DataFormat = I2S_DATAFORMAT_16B, 1 pData : 2 data sample. E.g: if you want to transmission 100 16bit data sample, Size = 100
  * 		When DataFormat = I2S_DATAFORMAT_24B, 1 pData : 1 data sample. E.g: if you want to transmission 100 24bit data sample, Size = 100
  * 		When DataFormat = I2S_DATAFORMAT_32B, 1 pData : 1 data sample. E.g: if you want to transmission 100 32bit data sample, Size = 100
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2S_TransmitReceive_DMA(I2S_HandleTypeDef *hi2s, uint32_t *pTxData, uint32_t *pRxData,
                                                uint32_t Size)
{
	if (hi2s->State != HAL_I2S_STATE_READY)
	{
		return HAL_BUSY;
	}
	
	if ((pTxData == NULL) || (pRxData == NULL) || (Size == 0U))
	{
		return HAL_ERROR;
	}
	
	__HAL_LOCK(hi2s);
	hi2s->pTxBuffPtr = pTxData;
	hi2s->pRxBuffPtr = pRxData;
	if (hi2s->Init.DataFormat == I2S_DATAFORMAT_8B)
	{
		hi2s->TxXferSize = (Size + 3) / 4;
		hi2s->TxXferCount = (Size + 3) / 4;
		hi2s->RxXferSize = (Size + 3) / 4;
		hi2s->RxXferCount = (Size + 3) / 4;
	}
	else if (hi2s->Init.DataFormat == I2S_DATAFORMAT_16B)
	{
		hi2s->TxXferSize = (Size + 1) / 2;
		hi2s->TxXferCount = (Size + 1) / 2;
		hi2s->RxXferSize = (Size + 1) / 2;
		hi2s->RxXferCount = (Size + 1) / 2;
	}
	else
	{
		hi2s->TxXferSize = Size;
		hi2s->TxXferCount = Size;
		hi2s->RxXferSize = Size;
		hi2s->RxXferCount = Size;
	}
	hi2s->ErrorCode = HAL_I2S_ERROR_NONE;
	hi2s->State = HAL_I2S_STATE_BUSY_TXRX;
	
	hi2s->hdmatx->XferHalfCpltCallback = I2S_TxRxDMAHalfCplt;
	hi2s->hdmatx->XferCpltCallback = I2S_TxRxDMACplt;
	hi2s->hdmarx->XferHalfCpltCallback = I2S_TxRxDMAHalfCplt;
	hi2s->hdmarx->XferCpltCallback = I2S_TxRxDMACplt;
	
	if (HAL_OK != HAL_DMA_Start_IT(hi2s->hdmatx,
									(uint32_t)hi2s->pTxBuffPtr,
									(uint32_t)&hi2s->Instance->TXDR,
									(hi2s->TxXferSize * 4)))
	{
		hi2s->ErrorCode = HAL_I2S_ERROR_DMA;
		hi2s->State = HAL_I2S_STATE_READY;
		__HAL_UNLOCK(hi2s);
		return HAL_ERROR;
	}
	if (HAL_OK != HAL_DMA_Start_IT(hi2s->hdmarx,
									(uint32_t)&hi2s->Instance->RXDR,
									(uint32_t)hi2s->pRxBuffPtr,
									(hi2s->RxXferSize * 4)))
	{
		hi2s->ErrorCode = HAL_I2S_ERROR_DMA;
		hi2s->State = HAL_I2S_STATE_READY;
		__HAL_UNLOCK(hi2s);
		return HAL_ERROR;
	}
	SET_BIT(hi2s->Instance->CR, (I2S_CR_TXDMA_EN | I2S_CR_RXDMA_EN));
	if ((hi2s->Instance->CR & I2S_CR_EN) != I2S_CR_EN)
	{
		__HAL_I2S_ENABLE(hi2s);
	}
	
	__HAL_I2S_ENABLE_TX(hi2s);
	__HAL_I2S_ENABLE_RX(hi2s);
	
	__HAL_UNLOCK(hi2s);
	return HAL_OK;
}

/**
  * @brief  Pauses the audio DMA Stream/Channel playing from the Media.
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2S_DMAPause(I2S_HandleTypeDef *hi2s)
{
	__HAL_LOCK(hi2s);
	
	if (hi2s->State == HAL_I2S_STATE_BUSY_TX)
	{
		CLEAR_BIT(hi2s->Instance->CR, I2S_CR_TXDMA_EN);
	}
	if (hi2s->State == HAL_I2S_STATE_BUSY_RX)
	{
		CLEAR_BIT(hi2s->Instance->CR, I2S_CR_RXDMA_EN);
	}
	
	__HAL_UNLOCK(hi2s);
	
	return HAL_OK;
}

/**
  * @brief  Resumes the audio DMA Stream/Channel playing from the Media.
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2S_DMAResume(I2S_HandleTypeDef *hi2s)
{
	__HAL_LOCK(hi2s);
	
	if (hi2s->State == HAL_I2S_STATE_BUSY_TX)
	{
		SET_BIT(hi2s->Instance->CR, I2S_CR_TXDMA_EN);
		if ((hi2s->Instance->CR & I2S_CR_TXEN) != I2S_CR_TXEN)
		{
			__HAL_I2S_ENABLE_TX(hi2s);
		}
	}
	if (hi2s->State == HAL_I2S_STATE_BUSY_RX)
	{
		SET_BIT(hi2s->Instance->CR, I2S_CR_RXDMA_EN);
		if ((hi2s->Instance->CR & I2S_CR_RXEN) != I2S_CR_RXEN)
		{
			__HAL_I2S_ENABLE_RX(hi2s);
		}
	}
	
	if ((hi2s->Instance->CR & I2S_CR_EN) != I2S_CR_EN)
	{
		__HAL_I2S_ENABLE(hi2s);
	}
	
	__HAL_UNLOCK(hi2s);
	
	return HAL_OK;
}

/**
  * @brief  Stops the audio DMA Stream/Channel playing from the Media.
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_I2S_DMAStop(I2S_HandleTypeDef *hi2s)
{
	HAL_StatusTypeDef errorcode = HAL_OK;
	
	if ((hi2s->Instance->CR & I2S_CR_TXEN) == I2S_CR_TXEN)
	{
		if (hi2s->hdmatx != NULL)
		{
			if (HAL_OK != HAL_DMA_Abort(hi2s->hdmatx))
			{
				hi2s->ErrorCode = HAL_I2S_ERROR_DMA;
				errorcode = HAL_ERROR;
			}
		}
		__HAL_I2S_DISABLE_TX(hi2s);
		CLEAR_BIT(hi2s->Instance->CR, I2S_CR_TXDMA_EN);
		hi2s->State = HAL_I2S_STATE_READY;
	}
	if ((hi2s->Instance->CR & I2S_CR_RXEN) == I2S_CR_RXEN)
	{
		if (hi2s->hdmarx != NULL)
		{
			if (HAL_OK != HAL_DMA_Abort(hi2s->hdmarx))
			{
				hi2s->ErrorCode = HAL_I2S_ERROR_DMA;
				errorcode = HAL_ERROR;
			}
		}
		__HAL_I2S_DISABLE_RX(hi2s);
		CLEAR_BIT(hi2s->Instance->CR, I2S_CR_RXDMA_EN);
		hi2s->State = HAL_I2S_STATE_READY;
	}
	__HAL_I2S_DISABLE(hi2s);
	return errorcode;
}

/**
  * @brief  This function handles I2S interrupt request.
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval None
  */
void HAL_I2S_IRQHandler(I2S_HandleTypeDef *hi2s)
{
	uint32_t itmask = hi2s->Instance->IM;
	uint32_t itflag = hi2s->Instance->IF;
	
	__HAL_I2S_CLEAR_IT(hi2s, itflag);
	if ((hi2s->Instance->CR & I2S_CR_TXEN) == I2S_CR_TXEN)
	{
		if (((itflag & I2S_IF_TXTH) == I2S_IF_TXTH) && ((itmask & I2S_IM_TXTH) == RESET))
		{
			I2S_Transmit_IT(hi2s);
		}
		if (((itflag & I2S_IF_TXDONE) == I2S_IF_TXDONE) && ((itmask & I2S_IM_TXDONE) == RESET))
		{
			I2S_Transmit_IT(hi2s);
		}
		if (((itflag & I2S_IF_TX_UNDERFLOW) == I2S_IF_TX_UNDERFLOW) && ((itmask & I2S_IM_TX_UNDERFLOW) == RESET))
		{
			I2S_EndTransmit_IT(hi2s);
		}
	}
	if ((hi2s->Instance->CR & I2S_CR_RXEN) == I2S_CR_RXEN)
	{
		if (((itflag & I2S_IF_RXTH) == I2S_IF_RXTH) && ((itmask & I2S_IM_RXTH) == RESET))
		{
			I2S_Receive_IT(hi2s);
		}
		if (((itflag & I2S_IF_RXDONE) == I2S_IF_RXDONE) && ((itmask & I2S_IM_RXDONE) == RESET))
		{
			I2S_Receive_IT(hi2s);
		}
		if (((itflag & I2S_IF_RX_OVERFLOW) == I2S_IF_RX_OVERFLOW) && ((itmask & I2S_IM_RX_OVERFLOW) == RESET))
		{
			SET_BIT(hi2s->ErrorCode, HAL_I2S_ERROR_RXERR);
			HAL_I2S_ErrorCallback(hi2s);
		}
	}
}

/**
  * @brief  Tx Transfer Half completed callbacks
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval None
  */
__attribute__((weak)) void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
	UNUSED(hi2s);
}

/**
  * @brief  Tx Transfer completed callbacks
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval None
  */
__attribute__((weak)) void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s)
{
	UNUSED(hi2s);
}


/**
  * @brief  Rx Transfer half completed callbacks
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval None
  */
__attribute__((weak)) void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
	UNUSED(hi2s);
}

/**
  * @brief  Rx Transfer completed callbacks
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval None
  */
__attribute__((weak)) void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s)
{
	UNUSED(hi2s);
}

/**
  * @brief  Tx and Rx Transfer half completed callback
  * @param  hi2s I2S handle
  * @retval None
  */
__attribute__((weak)) void HAL_I2S_TxRxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
	UNUSED(hi2s);
}

/**
  * @brief  Tx and Rx Transfer completed callback
  * @param  hi2s I2S handle
  * @retval None
  */
__attribute__((weak)) void HAL_I2S_TxRxCpltCallback(I2S_HandleTypeDef *hi2s)
{
	UNUSED(hi2s);
}

/**
  * @brief  I2S error callbacks
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval None
  */
__attribute__((weak)) void HAL_I2S_ErrorCallback(I2S_HandleTypeDef *hi2s)
{
	UNUSED(hi2s);
}

/**
  * @brief  Return the I2S state
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval HAL state
  */
HAL_I2S_StateTypeDef HAL_I2S_GetState(I2S_HandleTypeDef *hi2s)
{
	return hi2s->State;
}

/**
  * @brief  Return the I2S error code
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval I2S Error Code
  */
uint32_t HAL_I2S_GetError(I2S_HandleTypeDef *hi2s)
{
	return hi2s->ErrorCode;
}

/**
  * @brief  DMA I2S transmit process complete callback
  * @param  hdma pointer to a DMA_HandleTypeDef structure that contains
  *                the configuration information for the specified DMA module.
  * @retval None
  */
static void I2S_DMATxCplt(DMA_HandleTypeDef *hdma)
{
	I2S_HandleTypeDef *hi2s = (I2S_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;
	
	if ((hdma->Init.Mode == DMA_MODE_NORMAL_SINGLE) || (hdma->Init.Mode == DMA_MODE_LINK_SINGLE))
	{
		CLEAR_BIT(hi2s->Instance->CR, I2S_CR_TXDMA_EN);
	//	__HAL_I2S_DISABLE_TX(hi2s);
		hi2s->TxXferCount = 0;
	//	hi2s->State = HAL_I2S_STATE_READY;
	}
//	HAL_I2S_TxCpltCallback(hi2s);
}

/**
  * @brief  DMA I2S transmit process half complete callback
  * @param  hdma pointer to a DMA_HandleTypeDef structure that contains
  *                the configuration information for the specified DMA module.
  * @retval None
  */
static void I2S_DMATxHalfCplt(DMA_HandleTypeDef *hdma)
{
	I2S_HandleTypeDef *hi2s = (I2S_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;
	
	HAL_I2S_TxHalfCpltCallback(hi2s);
}

/**
  * @brief  DMA I2S receive process complete callback
  * @param  hdma pointer to a DMA_HandleTypeDef structure that contains
  *                the configuration information for the specified DMA module.
  * @retval None
  */
static void I2S_DMARxCplt(DMA_HandleTypeDef *hdma)
{
	I2S_HandleTypeDef *hi2s = (I2S_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;
	
	if ((hdma->Init.Mode == DMA_MODE_NORMAL_SINGLE) || (hdma->Init.Mode == DMA_MODE_LINK_SINGLE))
	{
		CLEAR_BIT(hi2s->Instance->CR, I2S_CR_RXDMA_EN);
		__HAL_I2S_DISABLE_RX(hi2s);
		hi2s->RxXferCount = 0;
		hi2s->State = HAL_I2S_STATE_READY;
	}
	HAL_I2S_RxCpltCallback(hi2s);
}

/**
  * @brief  DMA I2S receive process half complete callback
  * @param  hdma pointer to a DMA_HandleTypeDef structure that contains
  *                the configuration information for the specified DMA module.
  * @retval None
  */
static void I2S_DMARxHalfCplt(DMA_HandleTypeDef *hdma)
{
	I2S_HandleTypeDef *hi2s = (I2S_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;
	
	HAL_I2S_RxHalfCpltCallback(hi2s);
}

/**
  * @brief  DMA I2S transmit receive process complete callback
  * @param  hdma pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA module.
  * @retval None
  */
static void I2S_TxRxDMACplt(DMA_HandleTypeDef *hdma)
{
	I2S_HandleTypeDef *hi2s = (I2S_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;

	if ((hdma->Init.Mode == DMA_MODE_NORMAL_SINGLE) || (hdma->Init.Mode == DMA_MODE_LINK_SINGLE))
	{
		if (hi2s->hdmarx == hdma)
		{
			CLEAR_BIT(hi2s->Instance->CR, I2S_CR_RXDMA_EN);
			hi2s->RxXferCount = 0;
			
			if (hi2s->TxXferCount == 0U)
			{
				__HAL_I2S_DISABLE_TX(hi2s);
				__HAL_I2S_DISABLE_RX(hi2s);
				hi2s->State = HAL_I2S_STATE_READY;
				HAL_I2S_TxRxCpltCallback(hi2s);
			}
		}
		if (hi2s->hdmatx == hdma)
		{
			CLEAR_BIT(hi2s->Instance->CR, I2S_CR_TXDMA_EN);
			hi2s->TxXferCount = 0;
			
			if (hi2s->RxXferCount == 0U)
			{
				__HAL_I2S_DISABLE_TX(hi2s);
				__HAL_I2S_DISABLE_RX(hi2s);
				hi2s->State = HAL_I2S_STATE_READY;
				HAL_I2S_TxRxCpltCallback(hi2s);
			}
		}
	}
}

/**
  * @brief  DMA I2S transmit receive process half complete callback
  * @param  hdma pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA module.
  * @retval None
  */
static void I2S_TxRxDMAHalfCplt(DMA_HandleTypeDef *hdma)
{
	I2S_HandleTypeDef *hi2s = (I2S_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;
	
	HAL_I2S_TxRxHalfCpltCallback(hi2s);
}

/**
  * @brief  Transmit an amount of data in non-blocking mode with Interrupt
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval None
  */
static void I2S_Transmit_IT(I2S_HandleTypeDef *hi2s)
{
	uint32_t fifo_count = I2S_FIFO_FULL - __HAL_I2S_GET_TXFIFO(hi2s);
	
	if (fifo_count > hi2s->TxXferCount)
	{
		fifo_count = hi2s->TxXferCount;
	}
	while (fifo_count > 0)
	{
		hi2s->Instance->TXDR = *(hi2s->pTxBuffPtr);
		hi2s->pTxBuffPtr++;
		hi2s->TxXferCount--;
		fifo_count--;
	}
}

/**
  * @brief  Wraps up transmission in non-blocking mode with Interrupt
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval None
  */
static void I2S_EndTransmit_IT(I2S_HandleTypeDef *hi2s)
{
	if (hi2s->TxXferCount == 0)
	{
		__HAL_I2S_DISABLE_IT(hi2s, (I2S_IM_TXDONE | I2S_IM_TXTH | I2S_IM_TX_UNDERFLOW));
		if (hi2s->State == HAL_I2S_STATE_BUSY_TX)
		{
			__HAL_I2S_DISABLE_TX(hi2s);
			hi2s->State = HAL_I2S_STATE_READY;
			HAL_I2S_TxCpltCallback(hi2s);
		}
		else if ((hi2s->State == HAL_I2S_STATE_BUSY_TXRX) && (hi2s->RxXferCount == 0))
		{
			__HAL_I2S_DISABLE_TX(hi2s);
			__HAL_I2S_DISABLE_RX(hi2s);
			hi2s->State = HAL_I2S_STATE_READY;
			HAL_I2S_TxRxCpltCallback(hi2s);
		}
	}
	else
	{
		SET_BIT(hi2s->ErrorCode, HAL_I2S_ERROR_TXERR);
	}
}

/**
  * @brief  Receive an amount of data in non-blocking mode with Interrupt
  * @param  hi2s pointer to a I2S_HandleTypeDef structure that contains
  *         the configuration information for I2S module
  * @retval None
  */
static void I2S_Receive_IT(I2S_HandleTypeDef *hi2s)
{
	uint32_t fifo_count = __HAL_I2S_GET_RXFIFO(hi2s);
	
	if (fifo_count > hi2s->RxXferCount)
	{
		fifo_count = hi2s->RxXferCount;
	}
	while (fifo_count > 0)
	{
		(*hi2s->pRxBuffPtr) = (uint32_t)hi2s->Instance->RXDR;
		hi2s->pRxBuffPtr++;
		hi2s->RxXferCount--;
		fifo_count--;
	}
	if (hi2s->RxXferCount == 0)
	{
		__HAL_I2S_DISABLE_IT(hi2s, (I2S_IF_RXTH | I2S_IF_RXDONE | I2S_IF_RX_OVERFLOW));
		if (hi2s->State == HAL_I2S_STATE_BUSY_RX)
		{
			__HAL_I2S_DISABLE_RX(hi2s);
			hi2s->State = HAL_I2S_STATE_READY;
			HAL_I2S_RxCpltCallback(hi2s);
		}
		else if ((hi2s->State == HAL_I2S_STATE_BUSY_TXRX) && (hi2s->TxXferCount == 0))
		{
			__HAL_I2S_DISABLE_TX(hi2s);
			__HAL_I2S_DISABLE_RX(hi2s);
			hi2s->State = HAL_I2S_STATE_READY;
			HAL_I2S_TxRxCpltCallback(hi2s);
		}
	}
}
