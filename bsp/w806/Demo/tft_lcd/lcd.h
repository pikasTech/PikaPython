#ifndef __LCD_H__
#define __LCD_H__

#include "wm_hal.h"

#define ST7789_SPI	1
#define ST7789_8080	1

#if ST7789_SPI
#include "st7789_serial.h"
#endif
#if ST7789_8080
#include "st7789_parallel.h"
#endif

void LCD_Back_On(void);
void LCD_Back_Off(void);
void LCD_Init(void);
void LCD_Address_Set(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye);
void LCD_Fill(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye, uint16_t color);
void LCD_DrawPoint(uint16_t x, uint16_t y, uint16_t color);
void LCD_DrawLine(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye, uint16_t color);
void LCD_DrawRectangle(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye, uint16_t color);
void LCD_DrawCircle(uint16_t x, uint16_t y, uint8_t r, uint16_t color);
void LCD_ShowPicture(uint16_t x, uint16_t y, uint16_t length, uint16_t width, uint8_t *data);

#endif