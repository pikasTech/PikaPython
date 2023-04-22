/*****************************************************************************
* | File        :   LCD_1IN14_V2.h
* | Function    :   test Demo
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2021-03-16
* | Info        :   
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#ifndef __LCD_1IN14_V2_H
#define __LCD_1IN14_V2_H


#include "DEV_Config.h"
#include <stdint.h>

#include <stdlib.h>     //itoa()
#include <stdio.h>


#define LCD_1IN14_V2_HEIGHT 240
#define LCD_1IN14_V2_WIDTH 135


#define HORIZONTAL 0
#define VERTICAL   1

#define LCD_1IN14_V2_SetBacklight(Value) ; 


typedef struct{
    UWORD WIDTH;
    UWORD HEIGHT;
    UBYTE SCAN_DIR;
}LCD_1IN14_V2_ATTRIBUTES;
extern LCD_1IN14_V2_ATTRIBUTES LCD_1IN14_V2;

/********************************************************************************
function:	
			Macro definition variable name
********************************************************************************/
void LCD_1IN14_V2_Init(UBYTE Scan_dir);
void LCD_1IN14_V2_Clear(UWORD Color);
void LCD_1IN14_V2_Display(UWORD *Image);
void LCD_1IN14_V2_DisplayWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD *Image);
void LCD_1IN14_V2_DisplayPoint(UWORD X, UWORD Y, UWORD Color);

void Handler_1IN14_V2_LCD(int signo);
#endif
