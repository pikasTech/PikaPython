#ifndef __LCD_DRIVER__H
#define __LCD_DRIVER__H

#include "STM32_common.h"

#define ST7735_NOP 0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID 0x04
#define ST7735_RDDST 0x09
#define ST7735_SLPIN 0x10
#define ST7735_SLPOUT 0x11
#define ST7735_PTLON 0x12
#define ST7735_NORON 0x13
#define ST7735_INVOFF 0x20
#define ST7735_INVON 0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON 0x29
#define ST7735_CASET 0x2A
#define ST7735_RASET 0x2B
#define ST7735_RAMWR 0x2C
#define ST7735_RAMRD 0x2E
#define ST7735_PTLAR 0x30
#define ST7735_COLMOD 0x3A
#define ST7735_MADCTL 0x36
#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR 0xB4
#define ST7735_DISSET5 0xB6
#define ST7735_PWCTR1 0xC0
#define ST7735_PWCTR2 0xC1
#define ST7735_PWCTR3 0xC2
#define ST7735_PWCTR4 0xC3
#define ST7735_PWCTR5 0xC4
#define ST7735_VMCTR1 0xC5
#define ST7735_RDID1 0xDA
#define ST7735_RDID2 0xDB
#define ST7735_RDID3 0xDC
#define ST7735_RDID4 0xDD
#define ST7735_PWCTR6 0xFC
#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

#define RED 0x00f8
#define GREEN 0xe007
#define BLUE 0x1f00
#define WHITE 0xffff
#define BLACK 0x0000
#define YELLOW 0xE0FF
#define GRAY0 0x7DEF  //灰色0 3165 00110 001011 00101
#define GRAY1 0x1084  //灰色1      00000 000000 00000
#define GRAY2 0x0842  //灰色2  1111111111011111

#define LCD_CTRLA GPIOA  //定义TFT数据端口
#define LCD_CTRLB GPIOB  //定义TFT数据端口

#define LCD_SCL GPIO_Pin_1 /* A1 */
#define LCD_SDA GPIO_Pin_2 /* A2 */
#define LCD_CS GPIO_Pin_1  /* B1 */

#define LCD_RS GPIO_Pin_5  /* A5 */
#define LCD_RST GPIO_Pin_3 /* A3 */

//#define LCD_CS_SET(x) LCD_CTRL->ODR=(LCD_CTRL->ODR&~LCD_CS)|(x ? LCD_CS:0)

//液晶控制口置1操作语句宏定义
#define LCD_SCL_SET LCD_CTRLA->BSRR = LCD_SCL
#define LCD_SDA_SET LCD_CTRLA->BSRR = LCD_SDA
#define LCD_CS_SET LCD_CTRLB->BSRR = LCD_CS

#define LCD_RS_SET LCD_CTRLA->BSRR = LCD_RS
#define LCD_RST_SET LCD_CTRLA->BSRR = LCD_RST
//液晶控制口置0操作语句宏定义
#define LCD_SCL_CLR LCD_CTRLA->BRR = LCD_SCL
#define LCD_SDA_CLR LCD_CTRLA->BRR = LCD_SDA
#define LCD_CS_CLR LCD_CTRLB->BRR = LCD_CS

#define LCD_RST_CLR LCD_CTRLA->BRR = LCD_RST
#define LCD_RS_CLR LCD_CTRLA->BRR = LCD_RS

void LCD_GPIO_Init(void);
void LCD_WriteIndex(u8 Index);
void LCD_Write_u8(u8 Data);
void LCD_WriteReg(u8 Index, u8 Data);
void LCD_Reset(void);
void LCD_Init(void);
void LCD_Clear(u16 Color);
void LCD_SetXY(u16 x, u16 y);
void LCD_DrawPoint(u16 x, u16 y, u16 Data);
void LCD_SetRegion(u16 x_start, u16 y_start, u16 x_end, u16 y_end);
void LCD_Write_u16(u16 Data);
void LCD_DrawRegin(u16 x_start, u16 y_start, u16 x_end, u16 y_end, u16* pData);

#endif