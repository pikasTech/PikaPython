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
#ifndef __LCD_2IN_H
#define __LCD_2IN_H

#include "DEV_Config.h"
#include <stdint.h>

#include <stdlib.h>     //itoa()
#include <stdio.h>


#define LCD_2IN_HEIGHT 240
#define LCD_2IN_WIDTH 320


#define HORIZONTAL 0
#define VERTICAL   1

#define LCD_2IN_SetBacklight(Value) ; 


typedef struct{
    UWORD WIDTH;
    UWORD HEIGHT;
    UBYTE SCAN_DIR;
}LCD_2IN_ATTRIBUTES;
extern LCD_2IN_ATTRIBUTES LCD_2IN;

/********************************************************************************
function:	
			Macro definition variable name
********************************************************************************/
void LCD_2IN_Init(UBYTE Scan_dir);
void LCD_2IN_Clear(UWORD Color);
void LCD_2IN_Display(UBYTE *Image);
void LCD_2IN_DisplayWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD *Image);
void LCD_2IN_DisplayPoint(UWORD X, UWORD Y, UWORD Color);

void Handler_2IN_LCD(int signo);
#endif
