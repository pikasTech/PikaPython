/*********************************************************************************
 * @file        wm_lcd.c
 * @author      杭州峰海科技有限公司
 * @version     V1.0
 * @date        20211027
 * @brief       linailiang
 *
 * Copyright (c) 2021 Hangzhou Fenghai Technology Co., Ltd. All rights reserved.
 *********************************************************************************/
#include "wm_lcd.h"



/*****************************************************************************
 * @brief   Initialize LCD Frame Counter
 * 
 * @param   freq: LCD reference refresh frequency in Hz
 */
void HAL_LCD_Ratio(uint8_t comNum, uint16_t freq)
{
	if (freq == 0) freq = 60;
	
	LCD->FRAMECNT = (LCD_RTC_CLK/freq)/comNum;
}


/*****************************************************************************
 * @brief select the voltage of LCD module
 *   
 */
void HAL_LCD_VlcdSet(LCD_VlcdDef vlcd)
{	
	LCD->CTRL &= ~LCD_VLCD_MASK;
	LCD->CTRL |=  vlcd;	
}


/*****************************************************************************
 * @brief set the duty of LCD module
 *    
 */
void HAL_LCD_DutySet(LCD_DutyDef duty)
{	
	LCD->CTRL &= ~LCD_DUTY_MASK;
	LCD->CTRL |=  duty;
}


/*****************************************************************************
 * @brief set the bias of LCD module
 *   
 */
void HAL_LCD_BiasSet(LCD_BiasDef bias)
{
	LCD->CTRL &= ~LCD_BIAS_MASK;
	LCD->CTRL |=  bias;		
}


/*****************************************************************************
 * @brief set LCD com & seg
 *   
 */
void HAL_LCD_ComSegEnable(uint32_t com, uint32_t seg)
{
	LCD->COM_EN = com;
	LCD->SEG_EN = seg;	
}


/*****************************************************************************
 * @brief  Turn on or clear a segment 
 *
 * @param  com: Which COM line to update
 *         val: which field to change
 */
void HAL_LCD_ShowSeg(LCD_ComDef com, uint32_t val)
{
	switch(com)
	{
		case LCD_RAM0: LCD->RAM0 = val;	break;
		case LCD_RAM1: LCD->RAM1 = val;	break;
		case LCD_RAM2: LCD->RAM2 = val;	break;
		case LCD_RAM3: LCD->RAM3 = val;	break;
		case LCD_RAM4: LCD->RAM4 = val;	break;
		case LCD_RAM5: LCD->RAM5 = val;	break;
		case LCD_RAM6: LCD->RAM6 = val;	break;
		case LCD_RAM7: LCD->RAM7 = val;	break;
		default: break;
	}
}


/*****************************************************************************
 * @brief set LCD digit module
 *   
 */
void HAL_LCD_DigitPower(LCD_StaDef status)
{
	if(status == LCD_ENABLE) LCD->CTRL |= LCD_EN;
	else LCD->CTRL &= ~LCD_EN;
}


/*****************************************************************************
 * @brief set LCD analog module
 *   
 */
void HAL_LCD_AnalogPower(LCD_StaDef status)
{
	if(status == LCD_ENABLE) LCD->CTRL |= LCD_VDD_ON;
	else LCD->CTRL &= ~LCD_VDD_ON;
}


/*****************************************************************************
 * @brief  initialize the lcd module
 * @param  comNum: COM line 4 - 8
 *         seg:    0-31 
 */
void HAL_LCD_Init(uint8_t comNum, uint32_t seg)
{
	uint32_t com;
	
	switch(comNum)
	{
		case 4: com = 0x0f;	break; // 4 COM
		case 5: com = 0x1f;	break; // 5 COM
		case 6: com = 0x3f;	break; // 6 COM
		case 7: com = 0x7f;	break; // 7 COM
		case 8: com = 0xff;	break; // 8 COM
		default:com = 0x0f; break; // 4 COM
	}
	
	__HAL_RCC_LCD_CLK_ENABLE();        // 确保打开LCD时钟，该宏定义需要在wm_rcc.h中添加，LCD默认是打开时钟的

	HAL_LCD_AnalogPower(LCD_ENABLE);   // 先打开模拟部分电源
	HAL_Delay(5);                      // 适当延时，否则运行不正常！
	
	HAL_LCD_VlcdSet(VLCD33);           // 配置显示参数
	HAL_LCD_DutySet(DUTY_ONEFOURTH);
	HAL_LCD_BiasSet(BIAS_ONETHIRD);
	HAL_LCD_Ratio(comNum, 60);
	HAL_Delay(5);                      // 适当延时，否则运行不正常！
	
	HAL_LCD_ShowSeg(LCD_RAM0, 0x0000); // 清除显示内容
	HAL_LCD_ShowSeg(LCD_RAM1, 0x0000);
	HAL_LCD_ShowSeg(LCD_RAM2, 0x0000);
	HAL_LCD_ShowSeg(LCD_RAM3, 0x0000);
	
	HAL_LCD_ComSegEnable(com, seg);    // 打开需要显示的COM与SEG
	HAL_Delay(5);
	
	HAL_LCD_DigitPower(LCD_ENABLE);    // 最好打开数字部分
	HAL_Delay(5);
}


