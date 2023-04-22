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
#ifndef __LCD_1IN8_H
#define __LCD_1IN8_H

#include "DEV_Config.h"
#include <stdint.h>

#include <stdlib.h>     //itoa()
#include <stdio.h>


#define LCD_1IN8_HEIGHT 160
#define LCD_1IN8_WIDTH 130


#define HORIZONTAL 0
#define VERTICAL   1

#define LCD_1IN8_SetBacklight(Value) ; 


typedef struct{
    UWORD WIDTH;
    UWORD HEIGHT;
    UBYTE SCAN_DIR;
}LCD_1IN8_ATTRIBUTES;
extern LCD_1IN8_ATTRIBUTES LCD_1IN8;

/********************************************************************************
function:	
			Macro definition variable name
********************************************************************************/
void LCD_1IN8_Init(UBYTE Scan_dir);
void LCD_1IN8_Clear(UWORD Color);
void LCD_1IN8_Display(UWORD *Image);
void LCD_1IN8_DisplayWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD *Image);
void LCD_1IN8_DisplayPoint(UWORD X, UWORD Y, UWORD Color);

void Handler_1IN8_LCD(int signo);
#endif
