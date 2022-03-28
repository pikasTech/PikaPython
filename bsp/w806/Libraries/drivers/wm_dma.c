#include "wm_dma.h"
#include "wm_cpu.h"

static void DMA_SetConfig(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint16_t DataLength);

/**
 * @breief	Initialize the DMA according to the specified
 * 			parameters in the DMA_InitTypeDef and initialize the associated handle.
 * @param	hdma: Pointer to a DMA_HandleTypeDef structure that contains
 * 				  the configuration information for the specified DMA Channel.
 * @retval	HAL status
 */
HAL_StatusTypeDef HAL_DMA_Init(DMA_HandleTypeDef *hdma)
{
	uint32_t temp = 0;
	DMA_LinkDescriptor *desc;
	
	if (hdma == NULL)
	{
		return HAL_ERROR;
	}
	
	assert_param(IS_DMA_ALL_INSTANCE(hdma->Instance));
	assert_param(IS_DMA_DIRECTION(hdma->Init.Direction));
	assert_param(IS_DMA_DEST_INC_STATE(hdma->Init.DestInc));
	assert_param(IS_DMA_SRC_INC_STATE(hdma->Init.SrcInc));
	assert_param(IS_DMA_DATA_SIZE(hdma->Init.DataAlignment));
	assert_param(IS_DMA_MODE(hdma->Init.Mode));
	
	hdma->DmaBaseAddress = DMA;
	hdma->ChannelIndex = (((uint32_t)hdma->Instance - (uint32_t)DMA_Channel0) / ((uint32_t)DMA_Channel1 - (uint32_t)DMA_Channel0));
	
	hdma->State = HAL_DMA_STATE_BUSY;
	CLEAR_REG(hdma->Instance->CR2);
	CLEAR_REG(hdma->Instance->MODE);
	WRITE_REG(hdma->Instance->MODE, hdma->Init.Direction);
	if ((hdma->Init.Direction == DMA_PERIPH_TO_MEMORY) || (hdma->Init.Direction == DMA_MEMORY_TO_PERIPH))
	{
		assert_param(IS_DMA_REQUEST_SOURCE(hdma->Init.RequestSourceSel));
		MODIFY_REG(hdma->Instance->MODE, DMA_MODE_CH, hdma->Init.RequestSourceSel);
		if ((hdma->Init.RequestSourceSel == DMA_REQUEST_SOURCE_UART_RX) || (hdma->Init.RequestSourceSel == DMA_REQUEST_SOURCE_UART_TX))
		{
			assert_param(IS_DMA_UART_CHANNEL(hdma->Init.RequestUartSel));
			MODIFY_REG(hdma->DmaBaseAddress->REQCH, DMA_REQCH_UART, hdma->Init.RequestUartSel);
		}
	}
	if ((hdma->Init.Mode == DMA_MODE_NORMAL_SINGLE) || (hdma->Init.Mode == DMA_MODE_NORMAL_CIRCULAR))
	{
		WRITE_REG(hdma->Instance->CR2, (hdma->Init.DataAlignment | hdma->Init.DestInc | hdma->Init.SrcInc));
		if (hdma->Init.Mode == DMA_MODE_NORMAL_CIRCULAR)
		{
			SET_BIT(hdma->Instance->CR2, DMA_CR2_AUTORELOAD);
		}
	}
	else if ((hdma->Init.Mode == DMA_MODE_LINK_SINGLE) || (hdma->Init.Mode == DMA_MODE_LINK_CIRCULAR))
	{
		SET_BIT(hdma->Instance->MODE, (DMA_MODE_LNM | DMA_MODE_LINK));
		desc = hdma->LinkDesc;
		temp = (hdma->Init.DataAlignment | hdma->Init.DestInc | hdma->Init.SrcInc) >> 1;
		desc[0].Valid = 0;
		desc[0].Control = temp;
		desc[0].Next = (DMA_LinkDescriptor *)&desc[1];
		
		desc[1].Valid = 0;
		desc[1].Control = temp;
		if (hdma->Init.Mode == DMA_MODE_LINK_CIRCULAR)
		{
			desc[1].Next = (DMA_LinkDescriptor *)&desc[0];
		}
		else if(hdma->Init.Mode == DMA_MODE_LINK_SINGLE)
		{
			desc[1].Next = NULL;
		}
	}
	
	hdma->ErrorCode = HAL_DMA_ERROR_NONE;
	hdma->State = HAL_DMA_STATE_READY;
	hdma->Lock = HAL_UNLOCKED;
	
	return HAL_OK;
}

/**
  * @brief  DeInitialize the DMA peripheral.
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA Channel.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMA_DeInit (DMA_HandleTypeDef *hdma)
{
	if (hdma == NULL)
	{
		return HAL_ERROR;
	}
	
	assert_param(IS_DMA_ALL_INSTANCE(hdma->Instance));
	
	__HAL_DMA_DISABLE(hdma);
	
	hdma->Instance->SA = 0U;
	hdma->Instance->DA = 0U;
	hdma->Instance->SWA = 0U;
	hdma->Instance->DWA = 0U;
	hdma->Instance->LA = 0U;
	hdma->Instance->CR1 = 0U;
	hdma->Instance->MODE = 0U;
	hdma->Instance->CR2 = 0U;
	
	hdma->DmaBaseAddress->IF = ((DMA_IF_TRANSFER_DONE | DMA_IF_BURST_DONE) << (hdma->ChannelIndex * 2));
	
	hdma->ErrorCode = HAL_DMA_ERROR_NONE;
	hdma->State = HAL_DMA_STATE_RESET;
	__HAL_UNLOCK(hdma);
	
	return HAL_OK;
}

/**
  * @brief  Start the DMA Transfer.
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA Channel.
  * @param  SrcAddress: The source memory Buffer address
  * @param  DstAddress: The destination memory Buffer address
  * @param  DataLength: The length of data to be transferred from source to destination. Unit: Byte
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMA_Start(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint16_t DataLength)
{
	HAL_StatusTypeDef status = HAL_OK;
	
	assert_param(IS_DMA_BUFFER_SIZE(DataLength));
	
	__HAL_LOCK(hdma);
	if (HAL_DMA_STATE_READY == hdma->State)
	{
		hdma->State = HAL_DMA_STATE_BUSY;
		hdma->ErrorCode = HAL_DMA_ERROR_NONE;
		
		__HAL_DMA_DISABLE(hdma);
		DMA_SetConfig(hdma, SrcAddress, DstAddress, DataLength);
		__HAL_DMA_ENABLE(hdma);
	}
	else
	{
		__HAL_UNLOCK(hdma);
		status = HAL_BUSY;
	}
	
	return status;
}

/**
  * @brief  Start the DMA Transfer with interrupt enabled.
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA Channel.
  * @param  SrcAddress: The source memory Buffer address
  * @param  DstAddress: The destination memory Buffer address
  * @param  DataLength: The length of data to be transferred from source to destination. Unit: Byte
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMA_Start_IT(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint16_t DataLength)
{
	HAL_StatusTypeDef status = HAL_OK;
	
	assert_param(IS_DMA_BUFFER_SIZE(DataLength));
	
	__HAL_LOCK(hdma);
	if (HAL_DMA_STATE_READY == hdma->State)
	{
		hdma->State = HAL_DMA_STATE_BUSY;
		hdma->ErrorCode = HAL_DMA_ERROR_NONE;
		
		__HAL_DMA_DISABLE(hdma);
		DMA_SetConfig(hdma, SrcAddress, DstAddress, DataLength);
		__HAL_DMA_ENABLE_IT(hdma, (DMA_FLAG_TF_DONE << (hdma->ChannelIndex * 2)));
		__HAL_DMA_ENABLE(hdma);
	}
	else
	{
		__HAL_UNLOCK(hdma);
		status = HAL_BUSY;
	}
	
	return status;
}

/**
  * @brief  Abort the DMA Transfer.
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA Channel.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMA_Abort(DMA_HandleTypeDef *hdma)
{
	HAL_StatusTypeDef status = HAL_OK;
	
	if (hdma->State != HAL_DMA_STATE_BUSY)
	{
		hdma->ErrorCode = HAL_DMA_ERROR_NO_XFER;
		__HAL_UNLOCK(hdma);
		
		return HAL_ERROR;
	}
	else
	{
		__HAL_DMA_DISABLE_IT(hdma, ((DMA_IF_TRANSFER_DONE | DMA_IF_BURST_DONE) << (hdma->ChannelIndex * 2)));
		__HAL_DMA_DISABLE(hdma);
		__HAL_DMA_CLEAR_FLAG(hdma, ((DMA_IF_TRANSFER_DONE | DMA_IF_BURST_DONE) << (hdma->ChannelIndex * 2)));
	}
	hdma->State = HAL_DMA_STATE_READY;
	__HAL_UNLOCK(hdma);
	
	return status;
}

/**
  * @brief  Aborts the DMA Transfer in Interrupt mode.
  * @param  hdma  : pointer to a DMA_HandleTypeDef structure that contains
  *                 the configuration information for the specified DMA Channel.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMA_Abort_IT(DMA_HandleTypeDef *hdma)
{
	HAL_StatusTypeDef status = HAL_OK;
	
	if (hdma->State != HAL_DMA_STATE_BUSY)
	{
		hdma->ErrorCode = HAL_DMA_ERROR_NO_XFER;
		
		status = HAL_ERROR;
	}
	else
	{
		__HAL_DMA_DISABLE_IT(hdma, ((DMA_IF_TRANSFER_DONE | DMA_IF_BURST_DONE) << (hdma->ChannelIndex * 2)));
		__HAL_DMA_DISABLE(hdma);
		__HAL_DMA_CLEAR_FLAG(hdma, ((DMA_IF_TRANSFER_DONE | DMA_IF_BURST_DONE) << (hdma->ChannelIndex * 2)));
		hdma->State = HAL_DMA_STATE_READY;
		__HAL_UNLOCK(hdma);
		if (hdma->XferAbortCallback != NULL)
		{
			hdma->XferAbortCallback(hdma);
		}
	}
	
	return status;
}

/**
  * @brief  Polling for transfer complete.
  * @param  hdma:    pointer to a DMA_HandleTypeDef structure that contains
  *                  the configuration information for the specified DMA Channel.
  * @param  CompleteLevel: Specifies the DMA level complete.
  * @param  Timeout:       Timeout duration.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_DMA_PollForTransfer(DMA_HandleTypeDef *hdma, HAL_DMA_LevelCompleteTypeDef CompleteLevel, uint32_t Timeout)
{
	uint32_t tickstart = 0U;
	uint32_t temp;
	
	if (HAL_DMA_STATE_BUSY != hdma->State)
	{
		hdma->ErrorCode = HAL_DMA_ERROR_NO_XFER;
		__HAL_UNLOCK(hdma);
		
		return HAL_ERROR;
	}
	
	if ((hdma->Init.Mode == DMA_MODE_NORMAL_CIRCULAR) || (hdma->Init.Mode == DMA_MODE_LINK_CIRCULAR))
	{
		hdma->ErrorCode = HAL_DMA_ERROR_NOT_SUPPORTED;
		return HAL_ERROR;
	}
	assert_param(IS_DMA_COMPLETELEVEL(CompleteLevel));
	if ((CompleteLevel == HAL_DMA_HALF_TRANSFER) && (hdma->Init.Mode == DMA_MODE_NORMAL_SINGLE))
	{
		hdma->ErrorCode = HAL_DMA_ERROR_NOT_SUPPORTED;
		return HAL_ERROR;
	}
	
	temp = DMA_FLAG_TF_DONE << (hdma->ChannelIndex * 2);
	tickstart = HAL_GetTick();
	
	while (1)
	{
		if (hdma->Init.Mode == DMA_MODE_NORMAL_SINGLE)
		{
			if (__HAL_DMA_GET_FLAG(hdma, temp) != RESET)
			{
				break;
			}
		}
		else 
		{
			if ((hdma->LinkDesc[CompleteLevel]).Valid == RESET)
			{
				break;
			}
		}
		if (Timeout != HAL_MAX_DELAY)
		{
			if ((Timeout == 0U) || ((HAL_GetTick() - tickstart) > Timeout))
			{
				hdma->ErrorCode = HAL_DMA_ERROR_TIMEOUT;
				hdma->State = HAL_DMA_STATE_READY;
				__HAL_UNLOCK(hdma);
				
				return HAL_ERROR;
			}
		}
	}
	
	__HAL_DMA_CLEAR_FLAG(hdma, temp);
	hdma->State = HAL_DMA_STATE_READY;
	__HAL_UNLOCK(hdma);
	
	return HAL_OK;
}

/**
  * @brief  Handles DMA interrupt request.
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA Channel.  
  * @retval None
  */
void HAL_DMA_IRQHandler(DMA_HandleTypeDef *hdma)
{
	uint32_t source_it = hdma->DmaBaseAddress->IM;
	uint32_t flag = DMA_FLAG_TF_DONE << (hdma->ChannelIndex * 2);
	
	if ((__HAL_DMA_GET_FLAG(hdma, flag) != RESET) && ((source_it & flag) == RESET))
	{
		if (hdma->Init.Mode == DMA_MODE_NORMAL_SINGLE)
		{
			__HAL_DMA_DISABLE_IT(hdma, flag);
			hdma->State = HAL_DMA_STATE_READY;
			__HAL_UNLOCK(hdma);
			if (hdma->XferCpltCallback != NULL)
			{
				hdma->XferCpltCallback(hdma);
			}
		}
		else if (hdma->Init.Mode == DMA_MODE_NORMAL_CIRCULAR)
		{
			uint32_t cur_len = (hdma->Instance->CR2 & DMA_CR2_LEN_Msk) >> DMA_CR2_LEN_Pos;
			
			if ((cur_len + hdma->offset) > 0xFFFF)
			{
				cur_len = 0;
				__HAL_DMA_DISABLE(hdma);
				__HAL_DMA_ENABLE(hdma);
			}
			MODIFY_REG(hdma->Instance->CR2, (uint32_t)(DMA_CR2_LEN_Msk), 
							(uint32_t)((cur_len + hdma->offset) << DMA_CR2_LEN_Pos));
			if (hdma->XferCpltCallback != NULL)
			{
				hdma->XferCpltCallback(hdma);
			}
		}
		else if ((hdma->Init.Mode == DMA_MODE_LINK_SINGLE) || (hdma->Init.Mode == DMA_MODE_LINK_CIRCULAR))
		{
			if (hdma->LinkDesc[0].Valid == 0)
			{
				hdma->LinkDesc[0].Valid = (1 << 31);
				if (hdma->XferHalfCpltCallback != NULL)
				{
					hdma->XferHalfCpltCallback(hdma);
				}
			}
			else if (hdma->LinkDesc[1].Valid == 0)
			{
				if (hdma->Init.Mode == DMA_MODE_LINK_SINGLE)
				{
					__HAL_DMA_DISABLE_IT(hdma, flag);
					hdma->State = HAL_DMA_STATE_READY;
				}
				hdma->LinkDesc[1].Valid = (1 << 31);
				__HAL_UNLOCK(hdma);
				if (hdma->XferCpltCallback != NULL)
				{
					hdma->XferCpltCallback(hdma);
				}
			}
		}
	}
	__HAL_DMA_CLEAR_FLAG(hdma, flag);
}

/**
  * @brief  Return the DMA hande state.
  * @param  hdma: pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA Channel.
  * @retval HAL state
  */
HAL_DMA_StateTypeDef HAL_DMA_GetState(DMA_HandleTypeDef *hdma)
{
	return hdma->State;
}

/**
  * @brief  Return the DMA error code.
  * @param  hdma : pointer to a DMA_HandleTypeDef structure that contains
  *              the configuration information for the specified DMA Channel.
  * @retval DMA Error Code
  */
uint32_t HAL_DMA_GetError(DMA_HandleTypeDef *hdma)
{
	return hdma->ErrorCode;
}

/**
  * @brief  Sets the DMA Transfer parameter.
  * @param  hdma:       pointer to a DMA_HandleTypeDef structure that contains
  *                     the configuration information for the specified DMA Channel.
  * @param  SrcAddress: The source memory Buffer address
  * @param  DstAddress: The destination memory Buffer address
  * @param  DataLength: The length of data to be transferred from source to destination. Unit: Byte
  * @retval HAL status
  */
static void DMA_SetConfig(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint16_t DataLength)
{
	assert_param(IS_DMA_SRC_ADDR(SrcAddress));
	assert_param(IS_DMA_DEST_ADDR(DstAddress));
	assert_param(IS_DMA_LENGTH(hdma->Init.DataAlignment, DataLength));
	
	hdma->DmaBaseAddress->IF = ((DMA_IF_TRANSFER_DONE | DMA_IF_BURST_DONE) << (hdma->ChannelIndex * 2));
	
	if (hdma->Init.Mode == DMA_MODE_NORMAL_SINGLE)
	{
		hdma->Instance->SA = SrcAddress;
		hdma->Instance->DA = DstAddress;
		MODIFY_REG(hdma->Instance->CR2, DMA_CR2_LEN, (DataLength << DMA_CR2_LEN_Pos));
	}
	else if (hdma->Init.Mode == DMA_MODE_NORMAL_CIRCULAR)
	{
		hdma->Instance->SA = SrcAddress;
		hdma->Instance->DA = DstAddress;
		hdma->Instance->SWA = SrcAddress;
		hdma->Instance->DWA = DstAddress;
		hdma->Instance->WLEN = 0;
		if (hdma->Init.SrcInc == DMA_SINC_CIRCULAR)
		{
			MODIFY_REG(hdma->Instance->WLEN, DMA_WLEN_S_Msk, (DataLength << DMA_WLEN_S_Pos));
		}
		if (hdma->Init.DestInc == DMA_DINC_CIRCULAR)
		{
			MODIFY_REG(hdma->Instance->WLEN, DMA_WLEN_D_Msk, (DataLength << DMA_WLEN_D_Pos));
		}
		hdma->offset = DataLength;
		MODIFY_REG(hdma->Instance->CR2, DMA_CR2_LEN, (DataLength << DMA_CR2_LEN_Pos));
	}
	else
	{
		assert_param(IS_DMA_LINK_LENGTH(DataLength));
		hdma->LinkDesc[0].Control |= ((DataLength / 2) << 7);
		hdma->LinkDesc[0].Valid = (1 << 31);
		hdma->LinkDesc[0].SrcAddr = SrcAddress;
		hdma->LinkDesc[0].DestAddr = DstAddress;
		
		hdma->LinkDesc[1].Control |= ((DataLength - (DataLength / 2)) << 7);
		hdma->LinkDesc[1].Valid = (1 << 31);
		if (hdma->Init.SrcInc == DMA_SINC_ENABLE)
		{
			hdma->LinkDesc[1].SrcAddr = SrcAddress + (DataLength / 2);
		}
		else
		{
			hdma->LinkDesc[1].SrcAddr = SrcAddress;
		}
		
		if (hdma->Init.DestInc == DMA_DINC_ENABLE)
		{
			hdma->LinkDesc[1].DestAddr = DstAddress + (DataLength / 2);
		}
		else
		{
			hdma->LinkDesc[1].DestAddr = DstAddress;
		}
		WRITE_REG(hdma->Instance->LA, (uint32_t)(hdma->LinkDesc));
	}
}
