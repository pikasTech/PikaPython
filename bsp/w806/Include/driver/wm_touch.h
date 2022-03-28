#ifndef __WM_TOUCH_H__
#define __WM_TOUCH_H__

#include "wm_hal.h"

typedef struct
{
	uint32_t Channel;               /* Specifies the touch channel used.
									 This parameter can be a value of @ref TOUCH_CHANNEL */

	uint32_t ScanPeriod;         	/* Specifies the scan period. The unit is 16ms.
									  This parameter can be a number between Min_Data = 0x01 and Max_Data = 0x3F.*/

	uint32_t Window;            	/* Specifies Output pulse number as counting window.
									 Note: Set to N, the actual value is N - 2.
									This parameter can be a number between Min_Data = 0x03 and Max_Data = 0x3F.*/

	uint8_t Threshold[16];          /* Specifies Counting threshold for each channel.Threshold[0] corresponds to channel 0,
	  								and Threshold[15] corresponds to channel 15.
									This parameter can be a number between Min_Data = 0x01 and Max_Data = 0x7F.*/
									
	uint16_t Irq_en;				/* Specifies irq enable for each channel.
	  								Each bit corresponds to a channel, the LSB corresponds to channel 0, 
									and the MSB corresponds to channel 15.*/

} TOUCH_InitTypeDef;

typedef struct __TOUCH_HandleTypeDef
{
	TOUCH_TypeDef              *Instance;      /*!< TOUCH registers base address               */

	TOUCH_InitTypeDef           Init;           /*!< TOUCH SCAN parameters             */

} TOUCH_HandleTypeDef;


#define TOUCH		((TOUCH_TypeDef *)TOUCH_BASE)


// TOUCH_CHANNEL
#define TOUCH_CH_0		(0x0001U)	// PA7
#define TOUCH_CH_1		(0x0002U)	// PA9
#define TOUCH_CH_2		(0x0004U)	// PA10
#define TOUCH_CH_3		(0x0008U)	// PB0
#define TOUCH_CH_4		(0x0010U)	// PB1
#define TOUCH_CH_5		(0x0020U)	// PB2
#define TOUCH_CH_6		(0x0040U)	// PB3
#define TOUCH_CH_7		(0x0080U)	// PB4
#define TOUCH_CH_8		(0x0100U)	// PB5
#define TOUCH_CH_9		(0x0200U)	// PB6
#define TOUCH_CH_10		(0x0400U)	// PB7
#define TOUCH_CH_11		(0x0800U)	// PB8
#define TOUCH_CH_12		(0x1000U)	// PB9
#define TOUCH_CH_13		(0x2000U)	// PA12
#define TOUCH_CH_14		(0x4000U)	// PA14
#define TOUCH_CH_15		(0x8000U)	// PB29
#define TOUCH_CH_NONE	(0x0000U)
#define TOUCH_CH_ALL	(0xFFFFU)

#define IS_TOUCH_INSTANCE(INSTANCE)	(((INSTANCE) == TOUCH))
#define IS_TOUCH_CHANNELS(__CHANNEL__)		(((__CHANNEL__) == TOUCH_CH_0) || \
                                            ((__CHANNEL__) == TOUCH_CH_1) || \
                                            ((__CHANNEL__) == TOUCH_CH_2) || \
                                            ((__CHANNEL__) == TOUCH_CH_3) || \
                                            ((__CHANNEL__) == TOUCH_CH_4) || \
                                            ((__CHANNEL__) == TOUCH_CH_5) || \
                                            ((__CHANNEL__) == TOUCH_CH_6) || \
                                            ((__CHANNEL__) == TOUCH_CH_7) || \
                                            ((__CHANNEL__) == TOUCH_CH_8) || \
                                            ((__CHANNEL__) == TOUCH_CH_9) || \
                                            ((__CHANNEL__) == TOUCH_CH_10) || \
                                            ((__CHANNEL__) == TOUCH_CH_11) || \
                                            ((__CHANNEL__) == TOUCH_CH_12) || \
                                            ((__CHANNEL__) == TOUCH_CH_13) || \
                                            ((__CHANNEL__) == TOUCH_CH_14) || \
                                            ((__CHANNEL__) == TOUCH_CH_15) || \
                                            ((__CHANNEL__) == TOUCH_CH_NONE) || \
                                            ((__CHANNEL__) == TOUCH_CH_ALL))

#define IS_TOUCH_SCANPERIOD(__PERIOD__)	(((__PERIOD__) >= 0x01) && ((__PERIOD__) <= 0x03F))	

#define IS_TOUCH_WINDOW(__WINDOW__)		(((__WINDOW__) >= 0x03) && ((__WINDOW__) <= 0x03F))

#define IS_TOUCH_THRESHOLD(__THRESHOLD__)		(((__THRESHOLD__) >= 0x01) && ((__THRESHOLD__) <= 0x07F))

#define __HAL_TOUCH_GET_FLAG(__HANDLE__)		(READ_REG((__HANDLE__)->Instance->IE_IF))

#define __HAL_TOUCH_CLEAR_IT(__HANDLE__, __FLAG__)		(WRITE_REG((__HANDLE__)->Instance->IE_IF, __FLAG__))

HAL_StatusTypeDef	HAL_TOUCH_Init(TOUCH_HandleTypeDef *htouch);
HAL_StatusTypeDef	HAL_TOUCH_DeInit(TOUCH_HandleTypeDef *htouch);
void				HAL_TOUCH_MspInit(TOUCH_HandleTypeDef *htouch);
void				HAL_TOUCH_MspDeInit(TOUCH_HandleTypeDef *htouch);

void HAL_TOUCH_IRQHandler(TOUCH_HandleTypeDef *htouch);

/* Flag: irq flag, 
 * Each bit corresponds to a channel, the LSB corresponds to channel 0, 
 * and the MSB corresponds to channel 15 
 */
void HAL_TOUCH_Callback(TOUCH_HandleTypeDef *htouch, uint16_t Flag);

#endif