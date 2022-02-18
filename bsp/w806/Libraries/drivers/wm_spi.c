#include "wm_spi.h"


static void SPI_DMATransmitCplt(DMA_HandleTypeDef *hdma);
static void SPI_DMAHalfTransmitCplt(DMA_HandleTypeDef *hdma);
static void SPI_DMAReceiveCplt(DMA_HandleTypeDef *hdma);
static void SPI_DMAHalfReceiveCplt(DMA_HandleTypeDef *hdma);
static void SPI_DMATransmitReceiveCplt(DMA_HandleTypeDef *hdma);
static void SPI_DMAHalfTransmitReceiveCplt(DMA_HandleTypeDef *hdma);
static void SPI_Transmit_IT(SPI_HandleTypeDef *hspi);
static void SPI_EndTransmit_IT(SPI_HandleTypeDef *hspi);
static void SPI_Receive_IT(SPI_HandleTypeDef *hspi);
static void SPI_EndReceive_IT(SPI_HandleTypeDef *hspi);

/**
  * @brief  Initialize the SPI according to the specified parameters
  *         in the SPI_InitTypeDef and initialize the associated handle.
  * @param  hspi pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_Init(SPI_HandleTypeDef *hspi)
{
	if (hspi == NULL)
	{
		return HAL_ERROR;
	}
	
	assert_param(IS_SPI_ALL_INSTANCE(hspi->Instance));
	assert_param(IS_SPI_MODE(hspi->Init.Mode));
	
//	assert_param(IS_SPI_DATASIZE(hspi->Init.DataSize));
	assert_param(IS_SPI_NSS(hspi->Init.NSS));
	assert_param(IS_SPI_BAUDRATE_PRESCALER(hspi->Init.BaudRatePrescaler));
	assert_param(IS_SPI_CPOL(hspi->Init.CLKPolarity));
    assert_param(IS_SPI_CPHA(hspi->Init.CLKPhase));
	assert_param(IS_SPI_BIG_OR_LITTLE(hspi->Init.FirstByte));
	
	if (hspi->State == HAL_SPI_STATE_RESET)
	{
		hspi->Lock = HAL_UNLOCKED;
		HAL_SPI_MspInit(hspi);
	}
	hspi->State = HAL_SPI_STATE_BUSY;
	
	__HAL_SPI_DISABLE_TXRX(hspi);
	
	WRITE_REG(hspi->Instance->CH_CFG, (hspi->Init.NSS | SPI_CH_CFG_CLEARFIFOS));
	WRITE_REG(hspi->Instance->SPI_CFG, (hspi->Init.Mode | hspi->Init.CLKPolarity | hspi->Init.CLKPhase | hspi->Init.FirstByte));
	WRITE_REG(hspi->Instance->CLK_CFG, hspi->Init.BaudRatePrescaler);
	
	__HAL_SPI_SET_CS_HIGH(hspi);
	
	hspi->ErrorCode = HAL_SPI_ERROR_NONE;
	hspi->State     = HAL_SPI_STATE_READY;
	
	return HAL_OK;
}

/**
  * @brief  De-Initialize the SPI peripheral.
  * @param  hspi pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_DeInit(SPI_HandleTypeDef *hspi)
{
	if (hspi == NULL)
	{
		return HAL_ERROR;
	}
	
	assert_param(IS_SPI_ALL_INSTANCE(hspi->Instance));
	
	hspi->State = HAL_SPI_STATE_BUSY;
	__HAL_SPI_DISABLE_TXRX(hspi);
	HAL_SPI_MspDeInit(hspi);
	
	hspi->ErrorCode = HAL_SPI_ERROR_NONE;
	hspi->State = HAL_SPI_STATE_RESET;
	__HAL_UNLOCK(hspi);
	
	return HAL_OK;
}

/**
  * @brief  Initialize the SPI MSP.
  * @param  hspi pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
__attribute__((weak)) void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
	UNUSED(hspi);
}

/**
  * @brief  De-Initialize the SPI MSP.
  * @param  hspi pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
__attribute__((weak)) void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi)
{
	UNUSED(hspi);
}

/**
  * @brief  Transmit an amount of data in blocking mode.
  * @param  hspi pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @param  pData pointer to data buffer
  * @param  Size amount of data to be sent
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_Transmit(SPI_HandleTypeDef *hspi, uint8_t *pData, uint32_t Size, uint32_t Timeout)
{
	uint32_t tickstart, data = 0, i = 0;
	HAL_StatusTypeDef errorcode = HAL_OK;
	uint32_t fifo_count = 0, block_cnt = 0, tx_block_cnt = 0, tx_size = 0;
	
	__HAL_LOCK(hspi);
	
	tickstart = HAL_GetTick();
	
	if (hspi->State != HAL_SPI_STATE_READY)
	{
		errorcode = HAL_BUSY;
		goto error;
	}

	if ((pData == NULL) || (Size == 0U))
	{
		errorcode = HAL_ERROR;
		goto error;
	}
	
	__HAL_SPI_CLEAR_FIFO(hspi);
	__HAL_SPI_ENABLE_TX(hspi);
	hspi->State       = HAL_SPI_STATE_BUSY_TX;
	hspi->ErrorCode   = HAL_SPI_ERROR_NONE;
	
	hspi->pRxBuffPtr  = (uint8_t *)NULL;
	hspi->RxXferSize  = 0U;
	hspi->RxXferCount = 0U;
	
	block_cnt = Size / BLOCK_SIZE;
	while (tx_block_cnt <= block_cnt)
	{
		if (tx_block_cnt < block_cnt)
		{
			tx_size = BLOCK_SIZE;
		}
		else
		{
			tx_size = Size % BLOCK_SIZE;
		}
		hspi->pTxBuffPtr  = (uint8_t *)(pData + (tx_block_cnt * BLOCK_SIZE));
		hspi->TxXferSize  = tx_size;
		hspi->TxXferCount = tx_size;
		
		__HAL_SPI_SET_CLK_NUM(hspi, tx_size * 8);
		__HAL_SPI_SET_START(hspi);
		
		while (hspi->TxXferCount > 0U)
		{
			fifo_count = (32 - __HAL_SPI_GET_TXFIFO(hspi)) / 4;
			while((fifo_count > 0) && (hspi->TxXferCount > 0))
			{
				data = 0;
				for (i = 0; i < hspi->TxXferCount; i++)
				{
					if (i == 4)
					{
						break;
					}
					data |= (hspi->pTxBuffPtr[i] << (i * 8));
				}
				hspi->pTxBuffPtr += sizeof(uint8_t) * i;
				hspi->TxXferCount -= i;
				WRITE_REG(hspi->Instance->TXDATA, data);
				if (fifo_count > 0)
				{
					fifo_count--;
				}
			}
			if ((((HAL_GetTick() - tickstart) >=  Timeout) && (Timeout != HAL_MAX_DELAY)) || (Timeout == 0U))
			{
				errorcode = HAL_TIMEOUT;
				goto error;
			}
		}
		while (__HAL_SPI_GET_FLAG(hspi, SPI_INT_SRC_DONE) != SPI_INT_SRC_DONE)
		{
			if ((((HAL_GetTick() - tickstart) >=  Timeout) && (Timeout != HAL_MAX_DELAY)) || (Timeout == 0U))
			{
				errorcode = HAL_TIMEOUT;
				goto error;
			}
		}
		__HAL_SPI_CLEAR_FLAG(hspi, SPI_INT_SRC_DONE);
		
		tx_block_cnt++;
	}
error:
	__HAL_SPI_DISABLE_TX(hspi);
	hspi->State = HAL_SPI_STATE_READY;
	
	__HAL_UNLOCK(hspi);
	return errorcode;
}

/**
  * @brief  Receive an amount of data in blocking mode.
  * @param  hspi pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @param  pData pointer to data buffer
  * @param  Size amount of data to be received
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_Receive(SPI_HandleTypeDef *hspi, uint8_t *pData, uint32_t Size, uint32_t Timeout)
{
	uint32_t tickstart, data = 0, i = 0;
	HAL_StatusTypeDef errorcode = HAL_OK;
	uint32_t fifo_count = 0, block_cnt = 0, rx_block_cnt = 0, rx_size = 0;
	
	__HAL_LOCK(hspi);
	
	tickstart = HAL_GetTick();
	if (hspi->State != HAL_SPI_STATE_READY)
	{
		errorcode = HAL_BUSY;
		goto error;
	}
	
	if ((pData == NULL) || (Size == 0U))
	{
		errorcode = HAL_ERROR;
		goto error;
	}
	__HAL_SPI_CLEAR_FIFO(hspi);
	__HAL_SPI_ENABLE_RX(hspi);
	
	hspi->State       = HAL_SPI_STATE_BUSY_RX;
	hspi->ErrorCode   = HAL_SPI_ERROR_NONE;

	hspi->pTxBuffPtr  = (uint8_t *)NULL;
	hspi->TxXferSize  = 0U;
	hspi->TxXferCount = 0U;

	block_cnt = Size / BLOCK_SIZE;
	while (rx_block_cnt <=  block_cnt)
	{
		if (rx_block_cnt < block_cnt)
		{
			rx_size = BLOCK_SIZE;
		}
		else
		{
			rx_size = Size % BLOCK_SIZE;
		}
		hspi->pRxBuffPtr  = (uint8_t *)(pData + rx_block_cnt * BLOCK_SIZE);
		hspi->RxXferSize  = rx_size;
		hspi->RxXferCount = rx_size;
		
		__HAL_SPI_SET_CLK_NUM(hspi, rx_size * 8);
		__HAL_SPI_SET_START(hspi);
		
		while (hspi->RxXferCount > 0U)
		{
			fifo_count = __HAL_SPI_GET_RXFIFO(hspi) / 4;
			while ((fifo_count > 0) || (__HAL_SPI_GET_FLAG(hspi, SPI_INT_SRC_DONE) == SPI_INT_SRC_DONE))
			{
				data = READ_REG(hspi->Instance->RXDATA);
				for (i = 0; i < 4; i++)
				{
					if (i == hspi->RxXferCount)
					{
						break;
					}
					hspi->pRxBuffPtr[i] = (data >> (i * 8)) & 0xFF;
				}
				hspi->pRxBuffPtr += sizeof(uint8_t) * i;
				hspi->RxXferCount -= i;
				if (fifo_count > 0)
				{
					fifo_count--;
				}
				if((hspi->RxXferCount == 0))
				{
					break;
				}
			}
			
			if ((((HAL_GetTick() - tickstart) >=  Timeout) && ((Timeout != HAL_MAX_DELAY))) || (Timeout == 0U))
			{
				errorcode = HAL_TIMEOUT;
				goto error;
			}
		}
		while (__HAL_SPI_GET_FLAG(hspi, SPI_INT_SRC_DONE) != SPI_INT_SRC_DONE)
		{
			if ((((HAL_GetTick() - tickstart) >=  Timeout) && (Timeout != HAL_MAX_DELAY)) || (Timeout == 0U))
			{
				errorcode = HAL_TIMEOUT;
				goto error;
			}
		}
		__HAL_SPI_CLEAR_FLAG(hspi, SPI_INT_SRC_DONE);
		
		rx_block_cnt++;
	}
error :
	__HAL_SPI_DISABLE_RX(hspi);
	hspi->State = HAL_SPI_STATE_READY;
	__HAL_UNLOCK(hspi);
	return errorcode;
}

/**
  * @brief  Transmit and Receive an amount of data in blocking mode.
  * @param  hspi pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @param  pTxData pointer to transmission data buffer
  * @param  pRxData pointer to reception data buffer
  * @param  Size amount of data to be sent and received
  * @param  Timeout Timeout duration
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_TransmitReceive(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint32_t Size,
                                          uint32_t Timeout)
{
	HAL_StatusTypeDef    errorcode = HAL_OK;
	uint32_t tickstart, data = 0, i = 0;
	uint32_t fifo_count = 0, block_cnt = 0, tx_block_cnt = 0, tx_size = 0;

	__HAL_LOCK(hspi);
	
	tickstart = HAL_GetTick();
	
	if (!((hspi->State == HAL_SPI_STATE_READY) || (hspi->State == HAL_SPI_STATE_BUSY_RX)))
	{
		errorcode = HAL_BUSY;
		goto error;
	}

	if ((pTxData == NULL) || (pRxData == NULL) || (Size == 0U))
	{
		errorcode = HAL_ERROR;
		goto error;
	}
	
	if (hspi->State != HAL_SPI_STATE_BUSY_RX)
	{
		hspi->State = HAL_SPI_STATE_BUSY_TX_RX;
	}
	hspi->ErrorCode   = HAL_SPI_ERROR_NONE;
	__HAL_SPI_CLEAR_FIFO(hspi);
	__HAL_SPI_ENABLE_TXRX(hspi);
	
	block_cnt = Size / BLOCK_SIZE;
	while (tx_block_cnt <= block_cnt)
	{
		if (tx_block_cnt < block_cnt)
		{
			tx_size = BLOCK_SIZE;
		}
		else
		{
			tx_size = Size % BLOCK_SIZE;
		}
		
		hspi->pRxBuffPtr  = (uint8_t *)(pRxData + (tx_block_cnt * BLOCK_SIZE));
		hspi->RxXferCount = tx_size;
		hspi->RxXferSize  = tx_size;
		hspi->pTxBuffPtr  = (uint8_t *)(pTxData + (tx_block_cnt * BLOCK_SIZE));
		hspi->TxXferCount = tx_size;
		hspi->TxXferSize  = tx_size;
		
		__HAL_SPI_SET_CLK_NUM(hspi, tx_size * 8);
		__HAL_SPI_SET_START(hspi);
		
		while ((hspi->TxXferCount > 0U) || (hspi->RxXferCount > 0U))
		{
			fifo_count = (32 - __HAL_SPI_GET_TXFIFO(hspi)) / 4;
			while ((fifo_count > 0) && (hspi->TxXferCount > 0U))
			{
				data = 0;
				for (i = 0; i < hspi->TxXferCount; i++)
				{
					if (i == 4)
					{
						break;
					}
					data |= (hspi->pTxBuffPtr[i] << (i * 8));
				}
				hspi->pTxBuffPtr += sizeof(uint8_t) * i;
				hspi->TxXferCount -= i;
				WRITE_REG(hspi->Instance->TXDATA, data);
				if (fifo_count > 0)
				{
					fifo_count--;
				}
			}
			
			fifo_count = __HAL_SPI_GET_RXFIFO(hspi) / 4 ;
			while ((fifo_count > 0) || (__HAL_SPI_GET_FLAG(hspi, SPI_INT_SRC_DONE) == SPI_INT_SRC_DONE))
			{
				data = READ_REG(hspi->Instance->RXDATA);
				for (i = 0; i < 4; i++)
				{
					if (i == hspi->RxXferCount)
					{
						break;
					}
					hspi->pRxBuffPtr[i] = (data >> (i * 8)) & 0xFF;
				}
				hspi->pRxBuffPtr += sizeof(uint8_t) * i;
				hspi->RxXferCount -= i;
				if (fifo_count > 0)
				{
					fifo_count--;
				}
				if (hspi->RxXferCount == 0)
				{
					break;
				}
			}
			
			if ((((HAL_GetTick() - tickstart) >=  Timeout) && ((Timeout != HAL_MAX_DELAY))) || (Timeout == 0U))
			{
				errorcode = HAL_TIMEOUT;
				goto error;
			}
		}
		while (__HAL_SPI_GET_FLAG(hspi, SPI_INT_SRC_DONE) != SPI_INT_SRC_DONE)
		{
			if ((((HAL_GetTick() - tickstart) >=  Timeout) && (Timeout != HAL_MAX_DELAY)) || (Timeout == 0U))
			{
				errorcode = HAL_TIMEOUT;
				goto error;
			}
		}
		__HAL_SPI_CLEAR_FLAG(hspi, SPI_INT_SRC_DONE);
		
		tx_block_cnt++;
	
	}
error :
	__HAL_SPI_DISABLE_TXRX(hspi);
	hspi->State = HAL_SPI_STATE_READY;
	__HAL_UNLOCK(hspi);
	return errorcode;
}

/**
  * @brief  Transmit an amount of data in non-blocking mode with Interrupt.
  * @param  hspi pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @param  pData pointer to data buffer
  * @param  Size amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_Transmit_IT(SPI_HandleTypeDef *hspi, uint8_t *pData, uint32_t Size)
{
	HAL_StatusTypeDef errorcode = HAL_OK;
	uint32_t len = 0, data = 0, i = 0;
	
	__HAL_LOCK(hspi);
	
	if (hspi->State != HAL_SPI_STATE_READY)
	{
		errorcode = HAL_BUSY;
		goto error;
	}
	
	if ((pData == NULL) || (Size == 0U))
	{
		errorcode = HAL_ERROR;
		goto error;
	}
	__HAL_SPI_CLEAR_FIFO(hspi);
	hspi->State = HAL_SPI_STATE_BUSY_TX;
	hspi->ErrorCode = HAL_SPI_ERROR_NONE;
	hspi->pTxBuffPtr = (uint8_t *)pData;
	hspi->TxXferSize = Size;
	hspi->TxXferCount = Size;
	
	hspi->pRxBuffPtr = (uint8_t *)NULL;
	hspi->RxXferSize = 0U;
	hspi->RxXferCount = 0U;
	
	len = (hspi->TxXferCount > BLOCK_SIZE) ? BLOCK_SIZE : hspi->TxXferCount;
	__HAL_SPI_SET_CLK_NUM(hspi, (len * 8));
	
	for (i = 0; i < hspi->TxXferCount; i++)
	{
		if (i == 4)
		{
			break;
		}
		data |= ((hspi->pTxBuffPtr + hspi->TxXferSize - hspi->TxXferCount)[i] << (i * 8));
	}
	hspi->Instance->TXDATA = data;
	hspi->TxXferCount -= i;
	
	__HAL_SPI_ENABLE_TX(hspi);
	__HAL_SPI_CLEAR_FLAG(hspi, (SPI_INT_SRC_DONE | SPI_INT_SRC_TXOV | SPI_INT_SRC_TXUN | SPI_INT_SRC_TXRDY));
	__HAL_SPI_ENABLE_IT(hspi, (SPI_INT_MASK_DONE | SPI_INT_MASK_TXUN | SPI_INT_SRC_TXRDY));
	
	__HAL_SPI_SET_START(hspi);
error:
	__HAL_UNLOCK(hspi);
	return errorcode;
}

/**
  * @brief  Receive an amount of data in non-blocking mode with Interrupt.
  * @param  hspi pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @param  pData pointer to data buffer
  * @param  Size amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_Receive_IT(SPI_HandleTypeDef *hspi, uint8_t *pData, uint32_t Size)
{
	HAL_StatusTypeDef errorcode = HAL_OK;
	uint32_t len = 0;
	
	__HAL_LOCK(hspi);
	
	if (hspi->State != HAL_SPI_STATE_READY)
	{
		errorcode = HAL_BUSY;
		goto error;
	}
	
	if ((pData == NULL) || (Size == 0U))
	{
		errorcode = HAL_ERROR;
		goto error;
	}
	__HAL_SPI_CLEAR_FIFO(hspi);
	hspi->State       = HAL_SPI_STATE_BUSY_RX;
	hspi->ErrorCode   = HAL_SPI_ERROR_NONE;
	hspi->pTxBuffPtr  = (uint8_t *)NULL;
	hspi->TxXferSize  = 0U;
	hspi->TxXferCount = 0U;
	
	hspi->pRxBuffPtr = (uint8_t *)pData;
	hspi->RxXferSize = Size;
	hspi->RxXferCount = Size;
	
	len = (hspi->RxXferCount > BLOCK_SIZE) ? BLOCK_SIZE : hspi->RxXferCount;
	__HAL_SPI_SET_CLK_NUM(hspi, (len * 8));
	
	__HAL_SPI_ENABLE_RX(hspi);
	__HAL_SPI_CLEAR_FLAG(hspi, (SPI_INT_SRC_DONE | SPI_INT_SRC_RXOV | SPI_INT_SRC_RXUN | SPI_INT_SRC_RXRDY));
	__HAL_SPI_ENABLE_IT(hspi, (SPI_INT_MASK_DONE | SPI_INT_MASK_RXOV | SPI_INT_SRC_RXRDY));
	
	__HAL_SPI_SET_START(hspi);
error:
	__HAL_UNLOCK(hspi);
	return errorcode;
}

/**
  * @brief  Transmit and Receive an amount of data in non-blocking mode with Interrupt.
  * @param  hspi pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @param  pTxData pointer to transmission data buffer
  * @param  pRxData pointer to reception data buffer
  * @param  Size amount of data to be sent and received
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_TransmitReceive_IT(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData, uint32_t Size)
{
	HAL_StatusTypeDef errorcode = HAL_OK;
	uint32_t len = 0, data = 0, i = 0;
	
	__HAL_LOCK(hspi);
	
	if (hspi->State != HAL_SPI_STATE_READY)
	{
		errorcode = HAL_BUSY;
		goto error;
	}
	
	if ((pTxData == NULL) || (pRxData == NULL) || (Size == 0U))
	{
		errorcode = HAL_ERROR;
		goto error;
	}
	__HAL_SPI_CLEAR_FIFO(hspi);
	hspi->State = HAL_SPI_STATE_BUSY_TX_RX;
	hspi->ErrorCode = HAL_SPI_ERROR_NONE;
	hspi->pTxBuffPtr = (uint8_t *)pTxData;
	hspi->TxXferSize = Size;
	hspi->TxXferCount = Size;
	
	hspi->pRxBuffPtr = (uint8_t *)pRxData;
	hspi->RxXferSize = Size;
	hspi->RxXferCount = Size;
	
	len = (hspi->TxXferCount > BLOCK_SIZE) ? BLOCK_SIZE : hspi->TxXferCount;
	__HAL_SPI_SET_CLK_NUM(hspi, (len * 8));
	
	for (i = 0; i < hspi->TxXferCount; i++)
	{
		if (i == 4)
		{
			break;
		}
		data |= ((hspi->pTxBuffPtr + hspi->TxXferSize - hspi->TxXferCount)[i] << (i * 8));
	}
	hspi->Instance->TXDATA = data;
	hspi->TxXferCount -= i;
	
	__HAL_SPI_ENABLE_TXRX(hspi);
	__HAL_SPI_CLEAR_FLAG(hspi, (SPI_INT_SRC_DONE | SPI_INT_SRC_TXOV | SPI_INT_SRC_TXUN | SPI_INT_SRC_TXRDY | 
									SPI_INT_SRC_RXOV | SPI_INT_SRC_RXUN | SPI_INT_SRC_RXRDY));
	__HAL_SPI_ENABLE_IT(hspi, (SPI_INT_MASK_DONE | SPI_INT_MASK_TXUN | SPI_INT_SRC_TXRDY | SPI_INT_MASK_RXOV | SPI_INT_SRC_RXRDY));
	
	__HAL_SPI_SET_START(hspi);
error:
	__HAL_UNLOCK(hspi);
	return errorcode;
}

/**
  * @brief  Transmit an amount of data in non-blocking mode with DMA.
  * @param  hspi pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @param  pData pointer to data buffer
  * @param  Size amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_Transmit_DMA(SPI_HandleTypeDef *hspi, uint8_t *pData, uint32_t Size)
{
	HAL_StatusTypeDef errorcode = HAL_OK;
	uint32_t len = 0;
	
	assert_param(IS_SPI_DMA_HANDLE(hspi->hdmatx));
	
	__HAL_LOCK(hspi);
	if (hspi->State != HAL_SPI_STATE_READY)
	{
		errorcode = HAL_BUSY;
		goto error;
	}
	
	if ((pData == NULL) || (Size == 0U))
	{
		errorcode = HAL_ERROR;
		goto error;
	}
	__HAL_SPI_CLEAR_FIFO(hspi);
	hspi->State = HAL_SPI_STATE_BUSY_TX;
	hspi->ErrorCode = HAL_SPI_ERROR_NONE;
	hspi->pTxBuffPtr = (uint8_t *)pData;
	hspi->TxXferSize = Size;
	hspi->TxXferCount = Size;
	
	hspi->pRxBuffPtr = (uint8_t *)NULL;
	hspi->RxXferSize = 0U;
	hspi->RxXferCount = 0U;
	
	hspi->hdmatx->XferHalfCpltCallback = SPI_DMAHalfTransmitCplt;
	hspi->hdmatx->XferCpltCallback = SPI_DMATransmitCplt;
	hspi->hdmatx->XferAbortCallback = NULL;
	
	len = (hspi->TxXferCount > BLOCK_SIZE) ? BLOCK_SIZE : hspi->TxXferCount;
	hspi->TxXferCount -= len;
	__HAL_SPI_SET_CLK_NUM(hspi, (len * 8));
	if (HAL_OK != HAL_DMA_Start_IT(hspi->hdmatx, 
									(uint32_t)hspi->pTxBuffPtr, 
									(uint32_t)&hspi->Instance->TXDATA,
									len))
	{
		SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_DMA);
		errorcode = HAL_ERROR;
		hspi->State = HAL_SPI_STATE_READY;
		goto error;
	}
	__HAL_SPI_ENABLE_TX(hspi);
	__HAL_SPI_CLEAR_FLAG(hspi, (SPI_INT_SRC_DONE | SPI_INT_SRC_TXOV | SPI_INT_SRC_TXUN | SPI_INT_SRC_TXRDY));
	__HAL_SPI_ENABLE_IT(hspi, (SPI_INT_MASK_DONE | SPI_INT_MASK_TXUN));
	SET_BIT(hspi->Instance->MODE_CFG, SPI_MODE_CFG_TXDMA);
	__HAL_SPI_SET_START(hspi);
error:
	__HAL_UNLOCK(hspi);
	return errorcode;
}

/**
  * @brief  Receive an amount of data in non-blocking mode with DMA.
  * @note   In case of MASTER mode and SPI_DIRECTION_2LINES direction, hdmatx shall be defined.
  * @param  hspi pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @param  pData pointer to data buffer
  * @note   When the CRC feature is enabled the pData Length must be Size + 1.
  * @param  Size amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_Receive_DMA(SPI_HandleTypeDef *hspi, uint8_t *pData, uint32_t Size)
{
	HAL_StatusTypeDef errorcode = HAL_OK;
	uint32_t len = 0;
	
	assert_param(IS_SPI_DMA_HANDLE(hspi->hdmarx));
	
	__HAL_LOCK(hspi);
	
	if (hspi->State != HAL_SPI_STATE_READY)
	{
		errorcode = HAL_BUSY;
		goto error;
	}
	if ((pData == NULL) || (Size == 0U))
	{
		errorcode = HAL_ERROR;
		goto error;
	}
	__HAL_SPI_CLEAR_FIFO(hspi);
	hspi->State = HAL_SPI_STATE_BUSY_RX;
	hspi->ErrorCode = HAL_SPI_ERROR_NONE;
	hspi->pRxBuffPtr = (uint8_t *)pData;
	hspi->RxXferSize = Size;
	hspi->RxXferCount = Size;
	
	hspi->pTxBuffPtr = (uint8_t *)NULL;
	hspi->TxXferSize = 0U;
	hspi->TxXferCount = 0U;
	
	hspi->hdmarx->XferHalfCpltCallback = SPI_DMAHalfReceiveCplt;
	hspi->hdmarx->XferCpltCallback = SPI_DMAReceiveCplt;
	hspi->hdmarx->XferAbortCallback = NULL;
	
	len = (hspi->RxXferCount > BLOCK_SIZE) ? BLOCK_SIZE : hspi->RxXferCount;
	hspi->RxXferCount -= len;
	__HAL_SPI_SET_CLK_NUM(hspi, (len * 8));
	if (HAL_OK != HAL_DMA_Start_IT(hspi->hdmarx,
									(uint32_t)&hspi->Instance->RXDATA,
									(uint32_t)hspi->pRxBuffPtr,
									len))
	{
		SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_DMA);
		errorcode = HAL_ERROR;
		hspi->State = HAL_SPI_STATE_READY;
		goto error;
	}
	__HAL_SPI_ENABLE_RX(hspi);
	__HAL_SPI_CLEAR_FLAG(hspi, (SPI_INT_SRC_DONE | SPI_INT_SRC_RXOV | SPI_INT_SRC_RXUN | SPI_INT_SRC_RXRDY));
	__HAL_SPI_ENABLE_IT(hspi, (SPI_INT_MASK_DONE | SPI_INT_MASK_RXOV));
	SET_BIT(hspi->Instance->MODE_CFG, SPI_MODE_CFG_RXDMA);
	__HAL_SPI_SET_START(hspi);
error:
	__HAL_UNLOCK(hspi);
	return errorcode;
}

/**
  * @brief  Transmit and Receive an amount of data in non-blocking mode with DMA.
  * @param  hspi pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @param  pTxData pointer to transmission data buffer
  * @param  pRxData pointer to reception data buffer
  * @note   When the CRC feature is enabled the pRxData Length must be Size + 1
  * @param  Size amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_TransmitReceive_DMA(SPI_HandleTypeDef *hspi, uint8_t *pTxData, uint8_t *pRxData,
                                              uint32_t Size)
{
	HAL_StatusTypeDef errorcode = HAL_OK;
	uint32_t len = 0;
	
	assert_param(IS_SPI_DMA_HANDLE(hspi->hdmatx));
	assert_param(IS_SPI_DMA_HANDLE(hspi->hdmarx));
	
	__HAL_LOCK(hspi);
	
	if (hspi->State != HAL_SPI_STATE_READY)
	{
		errorcode = HAL_BUSY;
		goto error;
	}
	if ((pTxData == NULL) || (pRxData == NULL) || (Size == 0U))
	{
		errorcode = HAL_ERROR;
		goto error;
	}
	__HAL_SPI_CLEAR_FIFO(hspi);
	hspi->State = HAL_SPI_STATE_BUSY_TX_RX;
	hspi->ErrorCode = HAL_SPI_ERROR_NONE;
	hspi->pTxBuffPtr = (uint8_t *)pTxData;
	hspi->TxXferSize = Size;
	hspi->TxXferCount = Size;
	
	hspi->pRxBuffPtr = (uint8_t *)pRxData;
	hspi->RxXferSize = Size;
	hspi->RxXferCount = Size;
	
	hspi->hdmatx->XferHalfCpltCallback = SPI_DMAHalfTransmitReceiveCplt;
	hspi->hdmatx->XferCpltCallback = SPI_DMATransmitReceiveCplt;
	hspi->hdmatx->XferAbortCallback = NULL;
	hspi->hdmarx->XferHalfCpltCallback = SPI_DMAHalfTransmitReceiveCplt;
	hspi->hdmarx->XferCpltCallback = SPI_DMATransmitReceiveCplt;
	hspi->hdmarx->XferAbortCallback = NULL;
	
	len = (hspi->TxXferCount > BLOCK_SIZE) ? BLOCK_SIZE : hspi->TxXferCount;
	hspi->TxXferCount -= len;
	hspi->RxXferCount -= len;
	__HAL_SPI_SET_CLK_NUM(hspi, (len * 8));
	if (HAL_OK != HAL_DMA_Start_IT(hspi->hdmatx, 
									(uint32_t)hspi->pTxBuffPtr, 
									(uint32_t)&hspi->Instance->TXDATA,
									len))
	{
		SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_DMA);
		errorcode = HAL_ERROR;
		hspi->State = HAL_SPI_STATE_READY;
		goto error;
	}
	if (HAL_OK != HAL_DMA_Start_IT(hspi->hdmarx,
									(uint32_t)&hspi->Instance->RXDATA,
									(uint32_t)hspi->pRxBuffPtr,
									len))
	{
		SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_DMA);
		errorcode = HAL_ERROR;
		hspi->State = HAL_SPI_STATE_READY;
		goto error;
	}
	__HAL_SPI_ENABLE_TXRX(hspi);
	__HAL_SPI_CLEAR_FLAG(hspi, (SPI_INT_SRC_DONE | SPI_INT_SRC_TXOV | SPI_INT_SRC_TXUN | SPI_INT_SRC_TXRDY | 
									SPI_INT_SRC_RXOV | SPI_INT_SRC_RXUN | SPI_INT_SRC_RXRDY));
	__HAL_SPI_ENABLE_IT(hspi, (SPI_INT_MASK_DONE | SPI_INT_MASK_TXUN | SPI_INT_MASK_RXOV));
	SET_BIT(hspi->Instance->MODE_CFG, (SPI_MODE_CFG_TXDMA | SPI_MODE_CFG_RXDMA));
	__HAL_SPI_SET_START(hspi);
error:
	__HAL_UNLOCK(hspi);
	return errorcode;
}

/**
  * @brief  Pause the DMA Transfer.
  * @param  hspi pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for the specified SPI module.
  * @retval HAL status
  */											  
HAL_StatusTypeDef HAL_SPI_DMAPause(SPI_HandleTypeDef *hspi)
{
	__HAL_LOCK(hspi);
	CLEAR_BIT(hspi->Instance->MODE_CFG, (SPI_MODE_CFG_TXDMA | SPI_MODE_CFG_RXDMA));
	__HAL_UNLOCK(hspi);
	
	return HAL_OK;
}

/**
  * @brief  Resume the DMA Transfer.
  * @param  hspi pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for the specified SPI module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_DMAResume(SPI_HandleTypeDef *hspi)
{
	__HAL_LOCK(hspi);
	if (hspi->State == HAL_SPI_STATE_BUSY_TX)
	{
		SET_BIT(hspi->Instance->MODE_CFG, SPI_MODE_CFG_TXDMA);
	}
	else if (hspi->State == HAL_SPI_STATE_BUSY_RX)
	{
		SET_BIT(hspi->Instance->MODE_CFG, SPI_MODE_CFG_RXDMA);
	}
	else if (hspi->State == HAL_SPI_STATE_BUSY_TX_RX)
	{
		SET_BIT(hspi->Instance->MODE_CFG, (SPI_MODE_CFG_TXDMA | SPI_MODE_CFG_RXDMA));
	}
	__HAL_UNLOCK(hspi);
	
	return HAL_OK;
}

/**
  * @brief  Stop the DMA Transfer.
  * @param  hspi pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for the specified SPI module.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_SPI_DMAStop(SPI_HandleTypeDef *hspi)
{
	HAL_StatusTypeDef errorcode = HAL_OK;
	
	if (hspi->hdmatx != NULL)
	{
		if (HAL_OK != HAL_DMA_Abort(hspi->hdmatx))
		{
			SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_DMA);
			errorcode = HAL_ERROR;
		}
	}
	if (hspi->hdmarx != NULL)
	{
		if (HAL_OK != HAL_DMA_Abort(hspi->hdmarx))
		{
			SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_DMA);
			errorcode = HAL_ERROR;
		}
	}
	
	CLEAR_BIT(hspi->Instance->MODE_CFG, (SPI_MODE_CFG_TXDMA | SPI_MODE_CFG_RXDMA));
	hspi->State = HAL_SPI_STATE_READY;
	return errorcode;
}

/**
  * @brief  Handle SPI interrupt request.
  * @param  hspi pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for the specified SPI module.
  * @retval None
  */
void HAL_SPI_IRQHandler(SPI_HandleTypeDef *hspi)
{
	uint32_t itmask = hspi->Instance->INT_MASK;
	uint32_t itflag = hspi->Instance->INT_SRC;
	
	__HAL_SPI_CLEAR_FLAG(hspi, itflag);
	if (hspi->Instance->CH_CFG & SPI_CH_CFG_TXON)
	{
		if (((itflag & SPI_INT_SRC_TXRDY) == SPI_INT_SRC_TXRDY) && ((itmask & SPI_INT_MASK_TXRDY) == RESET))
		{
			SPI_Transmit_IT(hspi);
		}
		if (((itflag & SPI_INT_SRC_DONE) == SPI_INT_SRC_DONE) && ((itmask & SPI_INT_MASK_DONE) == RESET))
		{
			SPI_EndTransmit_IT(hspi);
		}
		if (((itflag & SPI_INT_SRC_TXUN) == SPI_INT_SRC_TXUN) && ((itmask & SPI_INT_MASK_TXUN) == RESET))
		{
			SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_TXERR);
			HAL_SPI_ErrorCallback(hspi);
		}
	}
	if (hspi->Instance->CH_CFG & SPI_CH_CFG_RXON)
	{
		if (((itflag & SPI_INT_SRC_RXRDY) == SPI_INT_SRC_RXRDY) && ((itmask & SPI_INT_MASK_RXRDY) == RESET))
		{
			SPI_Receive_IT(hspi);
		}
		if (((itflag & SPI_INT_SRC_DONE) == SPI_INT_SRC_DONE) && ((itmask & SPI_INT_MASK_DONE) == RESET))
		{
			SPI_EndReceive_IT(hspi);
		}
		if (((itflag & SPI_INT_SRC_RXOV) == SPI_INT_SRC_RXOV) && ((itmask & SPI_INT_MASK_RXOV) == RESET))
		{
			SET_BIT(hspi->ErrorCode, HAL_SPI_ERROR_RXERR);
			HAL_SPI_ErrorCallback(hspi);
		}
	}
}

/**
  * @brief  Tx Transfer completed callback.
  * @param  hspi pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
__attribute__((weak)) void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
	UNUSED(hspi);
}

/**
  * @brief  Rx Transfer completed callback.
  * @param  hspi pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
__attribute__((weak)) void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
{
	UNUSED(hspi);
}

/**
  * @brief  Tx and Rx Transfer completed callback.
  * @param  hspi pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
__attribute__((weak)) void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
	UNUSED(hspi);
}

/**
  * @brief  Tx Half Transfer completed callback.
  * @param  hspi pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
__attribute__((weak)) void HAL_SPI_TxHalfCpltCallback(SPI_HandleTypeDef *hspi)
{
	UNUSED(hspi);
}

/**
  * @brief  Rx Half Transfer completed callback.
  * @param  hspi pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
__attribute__((weak)) void HAL_SPI_RxHalfCpltCallback(SPI_HandleTypeDef *hspi)
{
	UNUSED(hspi);
}

/**
  * @brief  Tx and Rx Half Transfer callback.
  * @param  hspi pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
__attribute__((weak)) void HAL_SPI_TxRxHalfCpltCallback(SPI_HandleTypeDef *hspi)
{
	UNUSED(hspi);
}

/**
  * @brief  SPI error callback.
  * @param  hspi pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
	UNUSED(hspi);
}

/**
  * @brief  Return the SPI handle state.
  * @param  hspi pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval SPI state
  */
HAL_SPI_StateTypeDef HAL_SPI_GetState(SPI_HandleTypeDef *hspi)
{
	return hspi->State;
}

/**
  * @brief  Return the SPI error code.
  * @param  hspi pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval SPI error code in bitmap format
  */
uint32_t HAL_SPI_GetError(SPI_HandleTypeDef *hspi)
{
	return hspi->ErrorCode;
}

/**
  * @brief  DMA SPI transmit process complete callback.
  * @param  hdma pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA module.
  * @retval None
  */
static void SPI_DMATransmitCplt(DMA_HandleTypeDef *hdma)
{
	SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)(((DMA_HandleTypeDef *)hdma)->Parent);
	
	if ((hspi->TxXferCount == 0) && ((hdma->Init.Mode == DMA_MODE_NORMAL_SINGLE) || (hdma->Init.Mode == DMA_MODE_LINK_SINGLE)))
	{
		CLEAR_BIT(hspi->Instance->MODE_CFG, SPI_MODE_CFG_TXDMA);
	}
}

/**
  * @brief  DMA SPI half transmit process complete callback.
  * @param  hdma pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA module.
  * @retval None
  */
static void SPI_DMAHalfTransmitCplt(DMA_HandleTypeDef *hdma)
{
	SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)(((DMA_HandleTypeDef *)hdma)->Parent);
	
	if (hspi->TxXferSize <= BLOCK_SIZE)
	{
		HAL_SPI_TxHalfCpltCallback(hspi);
	}
}

/**
  * @brief  DMA SPI receive process complete callback.
  * @param  hdma pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA module.
  * @retval None
  */
static void SPI_DMAReceiveCplt(DMA_HandleTypeDef *hdma)
{
	SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)(((DMA_HandleTypeDef *)hdma)->Parent);
	
	if ((hspi->RxXferCount == 0) && ((hdma->Init.Mode == DMA_MODE_NORMAL_SINGLE) || (hdma->Init.Mode == DMA_MODE_LINK_SINGLE)))
	{
		CLEAR_BIT(hspi->Instance->MODE_CFG, SPI_MODE_CFG_RXDMA);
	}
}

/**
  * @brief  DMA SPI half receive process complete callback
  * @param  hdma pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA module.
  * @retval None
  */
static void SPI_DMAHalfReceiveCplt(DMA_HandleTypeDef *hdma)
{
	SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)(((DMA_HandleTypeDef *)hdma)->Parent);
	
	if (hspi->RxXferSize <= BLOCK_SIZE)
	{
		HAL_SPI_RxHalfCpltCallback(hspi);
	}
}

/**
  * @brief  DMA SPI transmit receive process complete callback.
  * @param  hdma pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA module.
  * @retval None
  */
static void SPI_DMATransmitReceiveCplt(DMA_HandleTypeDef *hdma)
{
	SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)(((DMA_HandleTypeDef *)hdma)->Parent);
	
	if ((hdma->Init.Mode == DMA_MODE_NORMAL_SINGLE) || (hdma->Init.Mode == DMA_MODE_LINK_SINGLE))
	{
		if ((hspi->hdmarx == hdma) && (hspi->RxXferCount == 0))
		{
			CLEAR_BIT(hspi->Instance->MODE_CFG, SPI_MODE_CFG_RXDMA);
		}
		if ((hspi->hdmatx == hdma) && (hspi->TxXferCount == 0))
		{
			CLEAR_BIT(hspi->Instance->MODE_CFG, SPI_MODE_CFG_TXDMA);
		}
	}
}

/**
  * @brief  DMA SPI half transmit receive process complete callback.
  * @param  hdma pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA module.
  * @retval None
  */
static void SPI_DMAHalfTransmitReceiveCplt(DMA_HandleTypeDef *hdma)
{
	SPI_HandleTypeDef *hspi = (SPI_HandleTypeDef *)(((DMA_HandleTypeDef *)hdma)->Parent);
	
	HAL_SPI_TxRxHalfCpltCallback(hspi);
}

static void SPI_Transmit_IT(SPI_HandleTypeDef *hspi)
{
	uint32_t data = 0, i = 0;
	
	if (((hspi->TxXferSize - hspi->TxXferCount) % BLOCK_SIZE) == 0)
	{
		return;
	}
	for (i = 0; i < hspi->TxXferCount; i++)
	{
		if (i == 4)
		{
			break;
		}
		data |= ((hspi->pTxBuffPtr + hspi->TxXferSize - hspi->TxXferCount)[i] << (i * 8));
	}
	hspi->Instance->TXDATA = data;
	hspi->TxXferCount -= i;
}

static void SPI_EndTransmit_IT(SPI_HandleTypeDef *hspi)
{
	uint32_t len = 0;
	
	len = (hspi->TxXferCount > BLOCK_SIZE) ? BLOCK_SIZE : hspi->TxXferCount;
	if (len == 0)
	{
	#if 0
		if ((hspi->hdmatx->Init.Mode == DMA_MODE_NORMAL_CIRCULAR) || (hspi->hdmatx->Init.Mode == DMA_MODE_LINK_CIRCULAR))
		{
			hspi->TxXferCount = hspi->TxXferSize;
			len = (hspi->TxXferCount > BLOCK_SIZE) ? BLOCK_SIZE : hspi->TxXferCount;
			__HAL_SPI_SET_CLK_NUM(hspi, len * 8);
			__HAL_SPI_SET_START(hspi);
		}
		else //if ((hspi->hdmatx->Init.Mode == DMA_MODE_NORMAL_SINGLE) || (hspi->hdmatx->Init.Mode == DMA_MODE_LINK_SINGLE))
	#endif
		{
			__HAL_SPI_DISABLE_IT(hspi, (SPI_INT_MASK_DONE | SPI_INT_MASK_TXRDY | SPI_INT_MASK_TXUN));
			if (hspi->State == HAL_SPI_STATE_BUSY_TX)
			{
				__HAL_SPI_DISABLE_TX(hspi);
				hspi->State = HAL_SPI_STATE_READY;
				HAL_SPI_TxCpltCallback(hspi);
			}
			else if (hspi->State == HAL_SPI_STATE_BUSY_TX_RX)
			{
				__HAL_SPI_DISABLE_TX(hspi);
				__HAL_SPI_DISABLE_RX(hspi);
				hspi->State = HAL_SPI_STATE_READY;
				HAL_SPI_TxRxCpltCallback(hspi);
			}
		}
	}
	else
	{
		__HAL_SPI_SET_CLK_NUM(hspi, (len * 8));
		if ((hspi->Instance->MODE_CFG & SPI_MODE_CFG_TXDMA) == SPI_MODE_CFG_TXDMA)
		{
			HAL_DMA_Start_IT(hspi->hdmatx, 
							(uint32_t)(hspi->pTxBuffPtr + (hspi->TxXferSize - hspi->TxXferCount)), 
							(uint32_t)&(hspi->Instance->TXDATA),
							len);
			hspi->TxXferCount -= len;
		}
		else
		{
			uint32_t data = 0, i = 0;
			for (i = 0; i < hspi->TxXferCount; i++)
			{
				if (i == 4)
				{
					break;
				}
				data |= ((hspi->pTxBuffPtr + hspi->TxXferSize - hspi->TxXferCount)[i] << (i * 8));
			}
			hspi->Instance->TXDATA = data;
			hspi->TxXferCount -= i;
		}
		__HAL_SPI_SET_START(hspi);
	}
}

static void SPI_Receive_IT(SPI_HandleTypeDef *hspi)
{
	uint32_t i = 0, data = 0;
	uint32_t fifo_count = __HAL_SPI_GET_RXFIFO(hspi) / 4;
	while (fifo_count > 0)
	{
		data = READ_REG(hspi->Instance->RXDATA);
		for (i = 0; i < 4; i++)
		{
			if (i == hspi->RxXferCount)
			{
				break;
			}
			(hspi->pRxBuffPtr + hspi->RxXferSize - hspi->RxXferCount)[i] = (data >> (i * 8)) & 0xFF;
		}
		hspi->RxXferCount -= i;
		fifo_count--;
		if((hspi->RxXferCount == 0))
		{
			break;
		}
	}
}

static void SPI_EndReceive_IT(SPI_HandleTypeDef *hspi)
{
	uint32_t len = 0;
	
	len = (hspi->RxXferCount > BLOCK_SIZE) ? BLOCK_SIZE : hspi->RxXferCount;
	if (len == 0)
	{
	#if 0
		if ((hspi->hdmatx->Init.Mode == DMA_MODE_NORMAL_CIRCULAR) || (hspi->hdmatx->Init.Mode == DMA_MODE_LINK_CIRCULAR))
		{
			hspi->RxXferCount = hspi->RxXferSize;
			len = (hspi->RxXferCount > BLOCK_SIZE) ? BLOCK_SIZE : hspi->RxXferCount;
			__HAL_SPI_SET_CLK_NUM(hspi, len * 8);
			__HAL_SPI_SET_START(hspi);
		}
		else //if ((hspi->hdmatx->Init.Mode == DMA_MODE_NORMAL_SINGLE) || (hspi->hdmatx->Init.Mode == DMA_MODE_LINK_SINGLE))
	#endif
		{
			__HAL_SPI_DISABLE_IT(hspi, (SPI_INT_MASK_DONE | SPI_INT_MASK_RXRDY | SPI_INT_MASK_RXOV));
			if (hspi->State == HAL_SPI_STATE_BUSY_RX)
			{
				__HAL_SPI_DISABLE_RX(hspi);
				hspi->State = HAL_SPI_STATE_READY;
				HAL_SPI_RxCpltCallback(hspi);
			}
			else if (hspi->State == HAL_SPI_STATE_BUSY_TX_RX)
			{
				__HAL_SPI_DISABLE_TX(hspi);
				__HAL_SPI_DISABLE_RX(hspi);
				hspi->State = HAL_SPI_STATE_READY;
				HAL_SPI_TxRxCpltCallback(hspi);
			}
		}
	}
	else
	{
		__HAL_SPI_SET_CLK_NUM(hspi, len * 8);
		if ((hspi->Instance->MODE_CFG & SPI_MODE_CFG_RXDMA) == SPI_MODE_CFG_RXDMA)
		{
			HAL_DMA_Start_IT(hspi->hdmarx, 
							(uint32_t)&(hspi->Instance->RXDATA),
							(uint32_t)(hspi->pRxBuffPtr + (hspi->RxXferSize - hspi->RxXferCount)), 
							len);
			hspi->RxXferCount -= len;
		}
		else
		{
			SPI_Receive_IT(hspi);
		}
		__HAL_SPI_SET_START(hspi);
	}
}