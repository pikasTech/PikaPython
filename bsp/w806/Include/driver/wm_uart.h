#ifndef __WM_UART_H__
#define __WM_UART_H__

#include "wm_hal.h"


typedef struct
{
	uint32_t BaudRate;                  /*!< This member configures the UART communication baud rate.
										   The baud rate is computed using the following formula:
										   - IntegerDivider = ((PCLKx) / (16 * (huart->Init.BaudRate)))
										   - FractionalDivider = ((IntegerDivider - ((uint32_t) IntegerDivider)) * 16) + 0.5 */

	uint32_t WordLength;                /*!< Specifies the number of data bits transmitted or received in a frame.
										   This parameter can be a value of @ref UART_Word_Length */

	uint32_t StopBits;                  /*!< Specifies the number of stop bits transmitted.
										   This parameter can be a value of @ref UART_Stop_Bits */

	uint32_t Parity;                    /*!< Specifies the parity mode.
										   This parameter can be a value of @ref UART_Parity
										   @note When parity is enabled, the computed parity is inserted
												 at the MSB position of the transmitted data (9th bit when
												 the word length is set to 9 data bits; 8th bit when the
												 word length is set to 8 data bits). */

	uint32_t Mode;                      /*!< Specifies whether the Receive or Transmit mode is enabled or disabled.
										   This parameter can be a value of @ref UART_Mode */

	uint32_t HwFlowCtl;                 /*!< Specifies whether the hardware flow control mode is enabled or disabled.
										   This parameter can be a value of @ref UART_Hardware_Flow_Control */

	uint32_t OverSampling;              /*!< Specifies whether the Over sampling 8 is enabled or disabled, to achieve higher speed (up to fPCLK/8).
										   This parameter can be a value of @ref UART_Over_Sampling. This feature is only available 
										   on STM32F100xx family, so OverSampling parameter should always be set to 16. */
} UART_InitTypeDef;

typedef enum
{
	HAL_UART_STATE_RESET             = 0x00U,    /*!< Peripheral is not yet Initialized
												   Value is allowed for gState and RxState */
	HAL_UART_STATE_READY             = 0x20U,    /*!< Peripheral Initialized and ready for use
												   Value is allowed for gState and RxState */
	HAL_UART_STATE_BUSY              = 0x24U,    /*!< an internal process is ongoing
												   Value is allowed for gState only */
	HAL_UART_STATE_BUSY_TX           = 0x21U,    /*!< Data Transmission process is ongoing
												   Value is allowed for gState only */
	HAL_UART_STATE_BUSY_RX           = 0x22U,    /*!< Data Reception process is ongoing
												   Value is allowed for RxState only */
	HAL_UART_STATE_BUSY_TX_RX        = 0x23U,    /*!< Data Transmission and Reception process is ongoing
												   Not to be used for neither gState nor RxState.
												   Value is result of combination (Or) between gState and RxState values */
	HAL_UART_STATE_TIMEOUT           = 0xA0U,    /*!< Timeout state
												   Value is allowed for gState only */
	HAL_UART_STATE_ERROR             = 0xE0U     /*!< Error
												   Value is allowed for gState only */
} HAL_UART_StateTypeDef;

typedef struct __UART_HandleTypeDef
{
	USART_TypeDef                 *Instance;        /*!< UART registers base address        */

	UART_InitTypeDef              Init;             /*!< UART communication parameters      */

	uint8_t                       *pTxBuffPtr;      /*!< Pointer to UART Tx transfer Buffer */

	uint16_t                      TxXferSize;       /*!< UART Tx Transfer size              */

	__IO uint16_t                 TxXferCount;      /*!< UART Tx Transfer Counter           */

	uint8_t                       *pRxBuffPtr;      /*!< Pointer to UART Rx transfer Buffer */

	uint16_t                      RxXferSize;       /*!< UART Rx Transfer size              */

	__IO uint16_t                 RxXferCount;      /*!< UART Rx Transfer Counter           */

	HAL_LockTypeDef               Lock;             /*!< Locking object                     */

	__IO HAL_UART_StateTypeDef    gState;           /*!< UART state information related to global Handle management
													   and also related to Tx operations.
													   This parameter can be a value of @ref HAL_UART_StateTypeDef */

	__IO HAL_UART_StateTypeDef    RxState;          /*!< UART state information related to Rx operations.
													   This parameter can be a value of @ref HAL_UART_StateTypeDef */

	__IO uint32_t                 ErrorCode;        /*!< UART Error code                    */
	
	uint32_t						RxFlag;

}UART_HandleTypeDef;

typedef enum
{
	UART_FIFO_TX_NOT_FULL,
	UART_FIFO_TX_EMPTY,
	UART_FIFO_RX_NOT_EMPTY,
} HAL_UART_WaitFlagDef;

#define UART0		((USART_TypeDef *)UART0_BASE)
#define UART1		((USART_TypeDef *)UART1_BASE)
#define UART2		((USART_TypeDef *)UART2_BASE)
#define UART3		((USART_TypeDef *)UART3_BASE)
#define UART4		((USART_TypeDef *)UART4_BASE)
#define UART5		((USART_TypeDef *)UART5_BASE)

#define UART_FIFO_FULL 32

#define HAL_UART_ERROR_NONE              0x00000000U   /*!< No error            */
#define HAL_UART_ERROR_FE                0x00000040U   /*!< Frame error         */
#define HAL_UART_ERROR_PE                0x00000080U   /*!< Parity error        */
#define HAL_UART_ERROR_ORE               0x00000100U   /*!< Overrun error       */

#define UART_WORDLENGTH_5B                  ((uint32_t)UART_LC_DATAL_5BIT)
#define UART_WORDLENGTH_6B                  ((uint32_t)UART_LC_DATAL_6BIT)
#define UART_WORDLENGTH_7B                  ((uint32_t)UART_LC_DATAL_7BIT)
#define UART_WORDLENGTH_8B                  ((uint32_t)UART_LC_DATAL_8BIT)

#define UART_STOPBITS_1                     0x00000000
#define UART_STOPBITS_2                     ((uint32_t)UART_LC_STOP)

#define UART_PARITY_NONE                    0x00000000
#define UART_PARITY_EVEN                    ((uint32_t)UART_LC_PCE)
#define UART_PARITY_ODD                     ((uint32_t)(UART_LC_PCE | UART_LC_PS))

#define UART_HWCONTROL_NONE                  0x00000000U
#define UART_HWCONTROL_RTS                   ((uint32_t)UART_FC_AFCE)
#define UART_HWCONTROL_CTS                   ((uint32_t)UART_FC_AFCE)
#define UART_HWCONTROL_RTS_CTS               ((uint32_t)UART_FC_AFCE)

#define UART_MODE_RX                        ((uint32_t)UART_LC_RE)
#define UART_MODE_TX                        ((uint32_t)UART_LC_TE)
#define UART_MODE_TX_RX                     ((uint32_t)(UART_LC_RE | UART_LC_TE))

#define UART_STATE_DISABLE                  0x00000000U
#define UART_STATE_ENABLE                   ((uint32_t)(UART_LC_RE | UART_LC_TE))




#define __HAL_UART_ENABLE(__HANDLE__)               ((__HANDLE__)->Instance->LC |= (UART_LC_RE | UART_LC_TE))

#define __HAL_UART_DISABLE(__HANDLE__)              ((__HANDLE__)->Instance->LC &= ~(UART_LC_RE | UART_LC_TE))

#define __HAL_UART_GET_FLAG(__HANDLE__, __FLAG__, __OPERATING__) (__OPERATING__(__HANDLE__, __FLAG__))

#define IS_UART_INSTANCE(INSTANCE) (((INSTANCE) == UART0) || \
                                    ((INSTANCE) == UART1) || \
                                    ((INSTANCE) == UART2) || \
                                    ((INSTANCE) == UART3)  || \
									((INSTANCE) == UART4)  || \
                                    ((INSTANCE) == UART5))
									
#define IS_UART_WORD_LENGTH(LENGTH) (((LENGTH) == UART_WORDLENGTH_5B) || \
                                     ((LENGTH) == UART_WORDLENGTH_6B) || \
									 ((LENGTH) == UART_WORDLENGTH_7B) || \
                                     ((LENGTH) == UART_WORDLENGTH_8B))
									 
#define IS_UART_BAUDRATE(BAUDRATE) (((BAUDRATE) == 2000000) || \
                                    ((BAUDRATE) == 1500000) || \
                                    ((BAUDRATE) == 1250000) || \
                                    ((BAUDRATE) == 1000000) || \
									((BAUDRATE) == 921600)  || \
									((BAUDRATE) == 460800) || \
                                    ((BAUDRATE) == 230400) || \
                                    ((BAUDRATE) == 115200) || \
                                    ((BAUDRATE) == 57600) || \
									((BAUDRATE) == 38400)  || \
									((BAUDRATE) == 19200) || \
                                    ((BAUDRATE) == 9600) || \
                                    ((BAUDRATE) == 4800) || \
                                    ((BAUDRATE) == 2400) || \
									((BAUDRATE) == 1800)  || \
									((BAUDRATE) == 1200)  || \
                                    ((BAUDRATE) == 600))
									
#define IS_UART_STOPBITS(STOPBITS) (((STOPBITS) == UART_STOPBITS_1) || \
                                    ((STOPBITS) == UART_STOPBITS_2))
									
#define IS_UART_PARITY(PARITY) (((PARITY) == UART_PARITY_NONE) || \
                                ((PARITY) == UART_PARITY_EVEN) || \
                                ((PARITY) == UART_PARITY_ODD))
								
#define IS_UART_MODE(MODE) (((MODE) == UART_MODE_RX) || \
                               ((MODE) == UART_MODE_TX) || \
                               ((MODE) == UART_MODE_TX_RX))

#define IS_UART_HARDWARE_FLOW_CONTROL(CONTROL)\
                              (((CONTROL) == UART_HWCONTROL_NONE) || \
                               ((CONTROL) == UART_HWCONTROL_RTS) || \
                               ((CONTROL) == UART_HWCONTROL_CTS) || \
                               ((CONTROL) == UART_HWCONTROL_RTS_CTS))
											
							   
#define UART_RX_INT_FLAG (UART_INTS_OE | UART_INTS_PE | UART_INTS_FE |\
        UART_INTS_RL | UART_INTS_RTO | UART_INTS_BD)
		
#define UART_RX_ERR_INT_FLAG (UART_INTS_BD | UART_INTS_FE | \
        UART_INTS_PE)

#define UART_TX_INT_FLAG (UART_INTM_TL | UART_INTM_TEMPT)
									
#define __HAL_UART_ENABLE_IT(__HANDLE__, __INTERRUPT__)   ((__HANDLE__)->Instance->INTM &= ~ __INTERRUPT__)

#define __HAL_UART_DISABLE_IT(__HANDLE__, __INTERRUPT__)  ((__HANDLE__)->Instance->INTM |= __INTERRUPT__)

#define __HAL_UART_CLEAR_FLAG(__HANDLE__, __FLAG__) ((__HANDLE__)->Instance->INTS |= __FLAG__)


HAL_StatusTypeDef HAL_UART_Init(UART_HandleTypeDef *huart);

HAL_StatusTypeDef HAL_UART_DeInit(UART_HandleTypeDef *huart);

HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout);

HAL_StatusTypeDef HAL_UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout);

HAL_StatusTypeDef HAL_UART_Transmit_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);

/**
  * 以中断方式接收一定长度的数据.
  * 注意：pData指向的地址，空间长度必须大于等于32字节
  * 	  Size大于0，则接收够Size长度的数据执行一次HAL_UART_RxCpltCallback(huart);
  * 	  Sized等于0，则接收不定长的数据就执行一次HAL_UART_RxCpltCallback(huart);
  *       两种情况下，数据都存放在huart->pRxBuffPtr或者pData中，数据长度存放在huart->RxXferCount中
  */
HAL_StatusTypeDef HAL_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);

void HAL_UART_IRQHandler(UART_HandleTypeDef *huart);

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
														   
#endif