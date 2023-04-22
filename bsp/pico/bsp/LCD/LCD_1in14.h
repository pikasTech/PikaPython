#ifndef __LCD_1IN14_H
#define __LCD_1IN14_H

#include "DEV_Config.h"
#include <stdint.h>

#include <stdlib.h>     //itoa()
#include <stdio.h>


#define LCD_1IN14_HEIGHT 240
#define LCD_1IN14_WIDTH 135


#define HORIZONTAL 0
#define VERTICAL   1

#define LCD_1IN14_SetBacklight(Value) ; 


typedef struct{
    UWORD WIDTH;
    UWORD HEIGHT;
    UBYTE SCAN_DIR;
}LCD_1IN14_ATTRIBUTES;
extern LCD_1IN14_ATTRIBUTES LCD_1IN14;

/********************************************************************************
function:	
			Macro definition variable name
********************************************************************************/
void LCD_1IN14_Init(UBYTE Scan_dir);
void LCD_1IN14_Clear(UWORD Color);
void LCD_1IN14_Display(UWORD *Image);
void LCD_1IN14_DisplayWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD *Image);
void LCD_1IN14_DisplayPoint(UWORD X, UWORD Y, UWORD Color);

void Handler_1IN14_LCD(int signo);
#endif
