/*****************************************************************************
* | File      	:   LCD_1in54.c
* | Author      :   Waveshare team
* | Function    :   Hardware underlying interface
* | Info        :
*                Used to shield the underlying layers of each master 
*                and enhance portability
*----------------
* |	This version:   V1.0
* | Date        :   2020-05-20
* | Info        :   Basic version
*
******************************************************************************/
#ifndef __LCD_1IN44_H
#define __LCD_1IN44_H

#include "DEV_Config.h"
#include <stdint.h>

#include <stdlib.h>     //itoa()
#include <stdio.h>


#define LCD_1IN44_HEIGHT 128
#define LCD_1IN44_WIDTH 128


#define HORIZONTAL 0
#define VERTICAL   1

#define LCD_1IN44_SetBacklight(Value) ; 


typedef struct{
    UWORD WIDTH;
    UWORD HEIGHT;
    UBYTE SCAN_DIR;
}LCD_1IN44_ATTRIBUTES;
extern LCD_1IN44_ATTRIBUTES LCD_1IN44;

/********************************************************************************
function:	
			Macro definition variable name
********************************************************************************/
void LCD_1IN44_Init(UBYTE Scan_dir);
void LCD_1IN44_Clear(UWORD Color);
void LCD_1IN44_Display(UWORD *Image);
void LCD_1IN44_DisplayWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD *Image);
void LCD_1IN44_DisplayPoint(UWORD X, UWORD Y, UWORD Color);
void Handler_1IN44_LCD(int signo);
#endif
