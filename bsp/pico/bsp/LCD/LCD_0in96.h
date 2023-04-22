/*****************************************************************************
* | File      	:   LCD_0in96.h
* | Author      :   Waveshare team
* | Function    :   Hardware underlying interface
* | Info        :
*                Used to shield the underlying layers of each master
*                and enhance portability
*----------------
* |	This version:   V1.0
* | Date        :   2021-03-11
* | Info        :   Basic version
*
******************************************************************************/
#ifndef __LCD_0IN96_H
#define __LCD_0IN96_H

#include "DEV_Config.h"
#include <stdint.h>

#include <stdlib.h>     //itoa()
#include <stdio.h>


#define LCD_0IN96_HEIGHT 80
#define LCD_0IN96_WIDTH 160


#define HORIZONTAL 0
#define VERTICAL   1

#define LCD_0IN96_SetBacklight(Value) ; 


typedef struct{
    UWORD WIDTH;
    UWORD HEIGHT;
    UBYTE SCAN_DIR;
}LCD_0IN96_ATTRIBUTES;
extern LCD_0IN96_ATTRIBUTES LCD_0IN96;

/********************************************************************************
function:	
			Macro definition variable name
********************************************************************************/
void LCD_0IN96_Init(UBYTE Scan_dir);
void LCD_0IN96_Clear(UWORD Color);
void LCD_0IN96_Display(UWORD *Image);
void LCD_0IN96_DisplayWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD *Image);
void LCD_0IN96_DisplayPoint(UWORD X, UWORD Y, UWORD Color);

void Handler_0IN96_LCD(int signo);
#endif
