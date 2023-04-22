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
#ifndef __LCD_1IN3_H
#define __LCD_1IN3_H

#include "DEV_Config.h"
#include <stdint.h>

#include <stdlib.h>     //itoa()
#include <stdio.h>


#define LCD_1IN3_HEIGHT 240
#define LCD_1IN3_WIDTH 240


#define HORIZONTAL 0
#define VERTICAL   1

#define LCD_1IN3_SetBacklight(Value) ; 


typedef struct{
    UWORD WIDTH;
    UWORD HEIGHT;
    UBYTE SCAN_DIR;
}LCD_1IN3_ATTRIBUTES;
extern LCD_1IN3_ATTRIBUTES LCD_1IN3;

/********************************************************************************
function:	
			Macro definition variable name
********************************************************************************/
void LCD_1IN3_Init(UBYTE Scan_dir);
void LCD_1IN3_Clear(UWORD Color);
void LCD_1IN3_Display(UWORD *Image);
void LCD_1IN3_DisplayWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD *Image);
void LCD_1IN3_DisplayPoint(UWORD X, UWORD Y, UWORD Color);

extern 
void GLCD_DrawBitmap(   int_fast16_t x, int_fast16_t y, 
                        int_fast16_t width, int_fast16_t height,
                        uint16_t *frame_ptr);

#endif
