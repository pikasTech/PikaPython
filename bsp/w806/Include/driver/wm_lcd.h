/*********************************************************************************
 * @file        wm_lcd.c
 * @author      杭州峰海科技有限公司
 * @version     V1.0
 * @date        20211027
 * @brief       linailiang
 *
 * Copyright (c) 2021 Hangzhou Fenghai Technology Co., Ltd. All rights reserved.
 *********************************************************************************/

#ifndef __WM_LCD_H
#define __WM_LCD_H

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * include
 */
#include <stdbool.h>
#include "wm_hal.h"


/*****************************************************************************
 * LCD Register Define 
 */
#define LCD_RTC_CLK		             (32000UL)	

#define LCD_CR_EN_Pos				 (8)
#define LCD_CR_PD_Pos				 (9)

#define LCD_VDD_ON				     (1UL<<9)
#define LCD_VDD_OFF				     (0UL<<9)
#define LCD_EN						 (1UL<<8)

#define LCD_BIAS_MASK				 (3UL<<6)
#define LCD_BIAS_MASK_Pos			 (6)
#define LCD_BIAS_ONEFOURTH			 (0UL<<6)
#define LCD_BIAS_ONEHALF			 (1UL<<6)
#define LCD_BIAS_ONETHIRD			 (2UL<<6)
#define LCD_BIAS_STATIC				 (3UL<<6)

#define LCD_VLCD_MASK				 (7UL<<3)
#define LCD_VLCD_MASK_Pos			 (3)
#define LCD_VLCD_27					 (0UL<<3)
#define LCD_VLCD_29					 (1UL<<3)
#define LCD_VLCD_31					 (2UL<<3)
#define LCD_VLCD_33					 (3UL<<3)

#define LCD_DUTY_MASK				 (7UL<<0)
#define LCD_DUTY_MASK_Pos			 (0)
#define LCD_DUTY_STATIC				 (0UL<<0)
#define LCD_DUTY_ONEHALF			 (1UL<<0)
#define LCD_DUTY_ONETHIRD			 (2UL<<0)
#define LCD_DUTY_ONEFOURTH			 (3UL<<0)
#define LCD_DUTY_ONEFIFTH			 (4UL<<0)
#define LCD_DUTY_ONESIXTH			 (5UL<<0)
#define LCD_DUTY_ONESEVENTH			 (6UL<<0)
#define LCD_DUTY_ONEEIGHTH			 (7UL<<0)


/*****************************************************************************
 * LCD typedef enum
 */
typedef enum
{	
	LCD_UNABLE = 0, /**  */	
	LCD_ENABLE = 1, /**  */	

} LCD_StaDef;

typedef enum
{	
	BIAS_STATIC		= LCD_BIAS_STATIC,   /** Static (2 levels)   */	
	BIAS_ONEHALF 	= LCD_BIAS_ONEHALF,  /** 1/2 Bias (3 levels) */	
	BIAS_ONETHIRD 	= LCD_BIAS_ONETHIRD, /** 1/3 Bias (4 levels) */	
	BIAS_ONEFOURTH	= LCD_BIAS_ONEFOURTH,/** 1/4 Bias (4 levels) */
} LCD_BiasDef;

typedef enum
{
	VLCD27 = LCD_VLCD_27, /** VLCD 2.7v */
	VLCD29 = LCD_VLCD_29, /** VLCD 2.9v */
	VLCD31 = LCD_VLCD_31, /** VLCD 3.1v */
	VLCD33 = LCD_VLCD_33, /** VLCD 3.3v */
} LCD_VlcdDef;

typedef enum
{
	DUTY_STATIC		= LCD_DUTY_STATIC,    /** Static (segments can be multiplexed with LCD_COM[0]) */
	DUTY_ONEHALF	= LCD_DUTY_ONEHALF,   /**  1/2 Duty cycle (segments can be multiplexed with LCD_COM[0:1]) */
	DUTY_ONETHIRD	= LCD_DUTY_ONETHIRD,  /**  1/3 Duty cycle (segments can be multiplexed with LCD_COM[0:2]) */
	DUTY_ONEFOURTH	= LCD_DUTY_ONEFOURTH, /**  1/4 Duty cycle (segments can be multiplexed with LCD_COM[0:3]) */
	DUTY_ONEFIFTH	= LCD_DUTY_ONEFIFTH,  /**  1/5 Duty cycle (segments can be multiplexed with LCD_COM[0:4]) */
	DUTY_ONESIXTH	= LCD_DUTY_ONESIXTH,  /**  1/6 Duty cycle (segments can be multiplexed with LCD_COM[0:5]) */
	DUTY_ONESEVENTH	= LCD_DUTY_ONESEVENTH,/**  1/7 Duty cycle (segments can be multiplexed with LCD_COM[0:6]) */
	DUTY_ONEEIGHTH	= LCD_DUTY_ONEEIGHTH, /**  1/8 Duty cycle (segments can be multiplexed with LCD_COM[0:7]) */
} LCD_DutyDef;

typedef enum
{
	LCD_RAM0 = 0, /**   */
	LCD_RAM1 = 1, /**   */
	LCD_RAM2 = 2, /**   */
	LCD_RAM3 = 3, /**   */
	LCD_RAM4 = 4, /**   */
	LCD_RAM5 = 5, /**   */
	LCD_RAM6 = 6, /**   */
	LCD_RAM7 = 7, /**   */
} LCD_ComDef;


/*****************************************************************************
 * LCD typedef struct
 */
typedef struct lcd_options
{
	bool         enable; /** */
	LCD_BiasDef  bias;   /** Bias configuration */
	LCD_DutyDef  duty;   /** Duty configuration */
	LCD_VlcdDef  vlcd;   /** Vlcd configuration */
	uint8_t	     com;    /** com number */
	uint16_t     rate;   /** Fresh rate configuration */
} lcd_options_t;


/*****************************************************************************
 * LCD Driver APIs
 */
void HAL_LCD_Ratio(uint8_t comNum, uint16_t freq);
void HAL_LCD_VlcdSet(LCD_VlcdDef vlcd);
void HAL_LCD_DutySet(LCD_DutyDef duty);
void HAL_LCD_BiasSet(LCD_BiasDef bias);
void HAL_LCD_ComSegEnable(uint32_t com, uint32_t seg);
void HAL_LCD_ShowSeg(LCD_ComDef com, uint32_t val);
void HAL_LCD_DigitPower(LCD_StaDef status);
void HAL_LCD_AnalogPower(LCD_StaDef status);


/*****************************************************************************
 * LCD Init
 */
void HAL_LCD_Init(uint8_t comNum, uint32_t seg);





#ifdef __cplusplus
}
#endif

#endif



