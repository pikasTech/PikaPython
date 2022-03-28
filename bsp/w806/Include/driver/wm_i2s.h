#ifndef __WM_I2S_H__
#define __WM_I2S_H__

#include "wm_hal.h"
#include "wm_dma.h"

/**
  * @brief I2S Init structure definition
  */
typedef struct
{
	uint32_t Mode;				/*	Specifies the I2S operating mode.
									This parameter can be a value of @ref I2S_MODE */
	
	uint32_t Standard;			/*	Specifies the standard used for the I2S communication.
									This parameter can be a value of @ref I2S_STANDARD */
	
	uint32_t DataFormat;		/*	Specifies the data format for the I2S communication.
									This parameter can be a value of @ref I2S_DATA_FORMAT */
	
	uint32_t MCLKOutput;		/*	Specifies whether the I2S MCLK output is enabled or not.
	  								This parameter can be a value of @ref I2S_MCLK_OUTPUT */
	
	uint32_t AudioFreq;			/*	Specifies the frequency selected for the I2S communication.
	  								This parameter can be a value of @ref I2S_AUDIO_FREQUENCY */
	
	uint32_t Channel;			/*	Specifies the data is mono or stereo.
	  								This parameter can be a value of @ref I2S_CHANNEL */
	
	uint32_t ChannelSel;		/*	Specifies the left channel or the right channel when Channel is mono.
	  								This parameter can be a value of @ref I2S_CHANNELSEL */
	
} I2S_InitTypeDef;

/**
  * @brief  HAL State structures definition
  */
typedef enum
{
	HAL_I2S_STATE_RESET = 0x00,		// I2S not yet initialized or disabled
	HAL_I2S_STATE_READY = 0x01,		// I2S initialized and ready for use
	HAL_I2S_STATE_BUSY = 0x02,		// I2S internal process is ongoing
	HAL_I2S_STATE_BUSY_TX = 0x03,	// Data Transmission process is ongoing
	HAL_I2S_STATE_BUSY_RX = 0x04,	// Data Reception process is ongoing
	HAL_I2S_STATE_BUSY_TXRX = 0x05,	// Data Transmission and Reception process is ongoing
	HAL_I2S_STATE_TIMEOUT = 0x06,	// I2S timeout state
	HAL_I2S_STATE_ERROR = 0x07		// I2S error state
} HAL_I2S_StateTypeDef;

/**
  * @brief I2S handle Structure definition
  */
typedef struct
{
	I2S_TypeDef					*Instance;			/* I2S registers base address */
	
	I2S_InitTypeDef				Init;				/* I2S communication parameters */
	
	uint32_t					*pTxBuffPtr;		/* Pointer to I2S Tx transfer buffer */
	
	__IO uint32_t				TxXferSize;			/* I2S Tx transfer size */
	
	__IO uint32_t				TxXferCount;		/* I2S Tx transfer Counter */
	
	uint32_t					*pRxBuffPtr;		/* Pointer to I2S Rx transfer buffer */
	
	__IO uint32_t				RxXferSize;			/* I2S Rx transfer size */
	
	__IO uint32_t				RxXferCount;		/* I2S Rx transfer counter
													  (This field is initialized at the
													   same value as transfer size at the
													   beginning of the transfer and
													   decremented when a sample is received
													   NbSamplesReceived = RxBufferSize-RxBufferCount) */
	
	DMA_HandleTypeDef			*hdmatx;			/* I2S Tx DMA handle parameters */
	
	DMA_HandleTypeDef			*hdmarx;			/* I2S Rx DMA handle parameters */
	
	__IO HAL_LockTypeDef		Lock;				/* I2S locking object */
	
	__IO HAL_I2S_StateTypeDef	State;				/* I2S communication state */
	
	__IO uint32_t				ErrorCode;			/* I2S Error code
														This parameter can be a value of @ref I2S_ERROR */
	
} I2S_HandleTypeDef;

#define I2S		(I2S_TypeDef *)I2S_BASE				// I2S register base address

// I2S_ERROR
#define HAL_I2S_ERROR_NONE			(0x00)			// No error
#define HAL_I2S_ERROR_TIMEOUT		(0x01)			// Timeout error
#define HAL_I2S_ERROR_TXERR			(0x02)			// Tx error
#define HAL_I2S_ERROR_RXERR			(0x04)			// Rx error
#define HAL_I2S_ERROR_DMA			(0x08)			// DMA transfer error
#define HAL_I2S_ERROR_PRESCALER		(0x10)			// Prescaler Calculation error

// I2S_MODE
#define I2S_MODE_MASTER				(0x00000000U)
#define I2S_MODE_SLAVE				(I2S_CR_MODE)

// I2S_STANDARD
#define I2S_STANDARD_PHILIPS		I2S_CR_FORMAT_I2S
#define I2S_STANDARD_MSB			I2S_CR_FORMAT_MSBJUSTIFIED
#define I2S_STANDARD_PCMA			I2S_CR_FORMAT_PCMA
#define I2S_STANDARD_PCMB			I2S_CR_FORMAT_PCMB

// I2S_DATA_FORMAT
#define I2S_DATAFORMAT_8B			I2S_CR_DATALEN_8BIT
#define I2S_DATAFORMAT_16B			I2S_CR_DATALEN_16BIT
#define I2S_DATAFORMAT_24B			I2S_CR_DATALEN_24BIT
#define I2S_DATAFORMAT_32B			I2S_CR_DATALEN_32BIT

// I2S_MCLK_OUTPUT
#define I2S_MCLKOUTPUT_ENABLE		RCC_I2S_CLK_MCLK_EN
#define I2S_MCLKOUTPUT_DISABLE		(0x00000000U)

// I2S_AUDIO_FREQUENCY
#define I2S_AUDIOFREQ_192K			(192000U)
#define I2S_AUDIOFREQ_96K			(96000U)
#define I2S_AUDIOFREQ_48K			(48000U)
#define I2S_AUDIOFREQ_44K			(44100U)
#define I2S_AUDIOFREQ_32K			(32000U)
#define I2S_AUDIOFREQ_24K			(24000U)
#define I2S_AUDIOFREQ_22K			(22050U)
#define I2S_AUDIOFREQ_16K			(16000U)
#define I2S_AUDIOFREQ_12K			(12000U)
#define I2S_AUDIOFREQ_11K			(11025U)
#define I2S_AUDIOFREQ_8K			(8000U)
#define I2S_AUDIOFREQ_DEFAULT		(2U)

// I2S_CHANNEL
#define I2S_CHANNEL_MONO			(I2S_CR_MONO_STEREO)
#define I2S_CHANNEL_STEREO			(0x00000000U)

// I2S_CHANNELSEL
#define I2S_CHANNELSEL_LEFT			I2S_CR_RXLRCH
#define I2S_CHANNELSEL_RIGHT		(0x00000000U)

#define I2S_FIFO_FULL 8

#define I2S_TIMEOUT_FLAG	100U

#define IS_I2S_ALL_INSTANCE(INSTANCE)	((INSTANCE) == I2S)

#define IS_I2S_MODE(__MODE__)	(((__MODE__) == I2S_MODE_MASTER_TX)    || \
								 ((__MODE__) == I2S_MODE_MASTER_RX)    || \
								 ((__MODE__) == I2S_MODE_MASTER_TX_RX) || \
								 ((__MODE__) == I2S_MODE_SLAVE_TX)     || \
								 ((__MODE__) == I2S_MODE_SLAVE_RX)     || \
								 ((__MODE__) == I2S_MODE_SLAVE_TX_RX))
								 
#define IS_I2S_STANDARD(__STANDARD__)	(((__STANDARD__) == I2S_STANDARD_PHILIPS) || \
										 ((__STANDARD__) == I2S_STANDARD_MSB)     || \
										 ((__STANDARD__) == I2S_STANDARD_PCMA)    || \
										 ((__STANDARD__) == I2S_STANDARD_PCMB))

#define IS_I2S_DATA_FORMAT(__FORMAT__)	(((__FORMAT__) == I2S_DATAFORMAT_8B) || \
										 ((__FORMAT__) == I2S_DATAFORMAT_16B) || \
										 ((__FORMAT__) == I2S_DATAFORMAT_24B) || \
										 ((__FORMAT__) == I2S_DATAFORMAT_32B))
										 
#define IS_I2S_MCLK_OUTPUT(__OUTPUT__)  (((__OUTPUT__) == I2S_MCLKOUTPUT_ENABLE) || \
										 ((__OUTPUT__) == I2S_MCLKOUTPUT_DISABLE))
										 
#define IS_I2S_AUDIO_FREQ(__FREQ__)		(((__FREQ__) == I2S_AUDIOFREQ_192K) || \
										 ((__FREQ__) == I2S_AUDIOFREQ_96K) || \
										 ((__FREQ__) == I2S_AUDIOFREQ_48K) || \
										 ((__FREQ__) == I2S_AUDIOFREQ_44K) || \
										 ((__FREQ__) == I2S_AUDIOFREQ_32K) || \
										 ((__FREQ__) == I2S_AUDIOFREQ_24K) || \
										 ((__FREQ__) == I2S_AUDIOFREQ_22K) || \
										 ((__FREQ__) == I2S_AUDIOFREQ_16K) || \
										 ((__FREQ__) == I2S_AUDIOFREQ_12K) || \
										 ((__FREQ__) == I2S_AUDIOFREQ_11K) || \
										 ((__FREQ__) == I2S_AUDIOFREQ_8K) || \
										 ((__FREQ__) == I2S_AUDIOFREQ_DEFAULT))
										 
#define IS_I2S_CHANNEL(__CHANNEL__)	(((__CHANNEL__) == I2S_AUDIO_MONO) || \
										((__CHANNEL__) == I2S_AUIDO_STEREO))
											
#define IS_I2S_CHANNELSEL(__SEL__)	(((__SEL__) == I2S_CHANNELSEL_LEFT) || \
									 ((__SEL__) == I2S_CHANNELSEL_RIGHT))
											
#define __HAL_I2S_ENABLE(__HANDLE__)	(SET_BIT((__HANDLE__)->Instance->CR, I2S_CR_EN))

#define __HAL_I2S_DISABLE(__HANDLE__)	(CLEAR_BIT((__HANDLE__)->Instance->CR, I2S_CR_EN))

#define __HAL_I2S_ENABLE_TX(__HANDLE__)	(SET_BIT((__HANDLE__)->Instance->CR, I2S_CR_TXEN))

#define __HAL_I2S_DISABLE_TX(__HANDLE__)	(CLEAR_BIT((__HANDLE__)->Instance->CR, I2S_CR_TXEN))

#define __HAL_I2S_ENABLE_RX(__HANDLE__)	(SET_BIT((__HANDLE__)->Instance->CR, I2S_CR_RXEN))

#define __HAL_I2S_DISABLE_RX(__HANDLE__)	(CLEAR_BIT((__HANDLE__)->Instance->CR, I2S_CR_RXEN))

#define __HAL_I2S_GET_TXFIFO(__HANDLE__)	(((__HANDLE__)->Instance->SR & I2S_SR_TXCNT) >> I2S_SR_TXCNT_Pos)

#define __HAL_I2S_GET_RXFIFO(__HANDLE__)	((__HANDLE__)->Instance->SR & I2S_SR_RXCNT)

#define __HAL_I2S_ENABLE_IT(__HANDLE__, __FLAG__)	((__HANDLE__)->Instance->IM &= ~(__FLAG__))

#define __HAL_I2S_DISABLE_IT(__HANDLE__, __FLAG__)	((__HANDLE__)->Instance->IM |= (__FLAG__))

#define __HAL_I2S_GET_IT(__HANDLE__, __FLAG__)	(((__HANDLE__)->Instance->IF) & (__FLAG__))

#define __HAL_I2S_CLEAR_IT(__HANDLE__, __FLAG__)	((__HANDLE__)->Instance->IF |= (__FLAG__))

HAL_StatusTypeDef HAL_I2S_Init(I2S_HandleTypeDef *hi2s);
HAL_StatusTypeDef HAL_I2S_DeInit(I2S_HandleTypeDef *hi2s);
void HAL_I2S_MspInit(I2S_HandleTypeDef *hi2s);
void HAL_I2S_MspDeInit(I2S_HandleTypeDef *hi2s);

HAL_StatusTypeDef HAL_I2S_Transmit(I2S_HandleTypeDef *hi2s, uint32_t *pData, uint32_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_I2S_Receive(I2S_HandleTypeDef *hi2s, uint32_t *pData, uint32_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_I2S_TransmitReceive(I2S_HandleTypeDef *hi2s, uint32_t *pTxData, uint32_t *pRxData,
                                            uint32_t Size, uint32_t Timeout);

HAL_StatusTypeDef HAL_I2S_Transmit_IT(I2S_HandleTypeDef *hi2s, uint32_t *pData, uint32_t Size);
HAL_StatusTypeDef HAL_I2S_Receive_IT(I2S_HandleTypeDef *hi2s, uint32_t *pData, uint32_t Size);
HAL_StatusTypeDef HAL_I2S_TransmitReceive_IT(I2S_HandleTypeDef *hi2s, uint32_t *pTxData, uint32_t *pRxData,
                                               uint32_t Size);

void HAL_I2S_IRQHandler(I2S_HandleTypeDef *hi2s);

HAL_StatusTypeDef HAL_I2S_Transmit_DMA(I2S_HandleTypeDef *hi2s, uint32_t *pData, uint32_t Size);
HAL_StatusTypeDef HAL_I2S_Receive_DMA(I2S_HandleTypeDef *hi2s, uint32_t *pData, uint32_t Size);
HAL_StatusTypeDef HAL_I2S_TransmitReceive_DMA(I2S_HandleTypeDef *hi2s, uint32_t *pTxData, uint32_t *pRxData,
                                                uint32_t Size);

HAL_StatusTypeDef HAL_I2S_DMAPause(I2S_HandleTypeDef *hi2s);
HAL_StatusTypeDef HAL_I2S_DMAResume(I2S_HandleTypeDef *hi2s);
HAL_StatusTypeDef HAL_I2S_DMAStop(I2S_HandleTypeDef *hi2s);


void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s);
void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s);
void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef *hi2s);
void HAL_I2S_RxCpltCallback(I2S_HandleTypeDef *hi2s);
void HAL_I2S_ErrorCallback(I2S_HandleTypeDef *hi2s);

void HAL_I2S_TxRxHalfCpltCallback(I2S_HandleTypeDef *hi2s);
void HAL_I2S_TxRxCpltCallback(I2S_HandleTypeDef *hi2s);

HAL_I2S_StateTypeDef HAL_I2S_GetState(I2S_HandleTypeDef *hi2s);
uint32_t HAL_I2S_GetError(I2S_HandleTypeDef *hi2s);

#endif