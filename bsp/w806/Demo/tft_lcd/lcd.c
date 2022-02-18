#include "lcd.h"

void LCD_Reset_On(void)
{
#if ST7789_SPI
	S_RESET_LOW;
#endif
#if ST7789_8080
	P_RESET_LOW;
#endif
}

void LCD_Reset_Off(void)
{
#if ST7789_SPI
	S_RESET_HIGH;
#endif
#if ST7789_8080
	P_RESET_HIGH;
#endif
}

void LCD_Back_On(void)
{
#if ST7789_SPI
	S_Back_On();
#endif
#if ST7789_8080
	P_Back_On();
#endif
}

void LCD_Back_Off(void)
{
#if ST7789_SPI
	S_Back_Off();
#endif
#if ST7789_8080
	P_Back_Off();
#endif
}

static void LCD_WriteReg(uint8_t reg)
{
#if ST7789_SPI
	S_WriteReg(reg);
#endif
#if ST7789_8080
	P_WriteReg(reg);
#endif
}

static void LCD_WriteData8(uint8_t data)
{
#if ST7789_SPI
	S_WriteData8(data);
#endif
#if ST7789_8080
	P_WriteData8(data);
#endif
}

static void LCD_WriteData16(uint16_t data)
{
#if ST7789_SPI
	S_WriteData16(data);
#endif
#if ST7789_8080
	P_WriteData16(data);
#endif
}

static void LCD_WriteData(uint8_t *data, uint32_t len)
{
	uint32_t t1 = 0, t2 = 0;
	
#if ST7789_SPI
	t1 = HAL_GetTick();
	S_WriteData(data, len);
	t2 = HAL_GetTick();
	printf("s_t = %d\r\n", t2 - t1);
#endif
#if ST7789_8080
	t1 = HAL_GetTick();
	P_WriteData(data, len);
	t2 = HAL_GetTick();
	printf("p_t = %d\r\n", t2 - t1);
#endif
}

void LCD_Init(void)
{
	LCD_Reset_On();
	HAL_Delay(120);
	LCD_Reset_Off();
	HAL_Delay(120);
	LCD_Back_On();
	HAL_Delay(100);
	LCD_WriteReg(0x11);
	HAL_Delay(120);
	
	LCD_WriteReg(0x36);	// left->right top->bottom
	LCD_WriteData8(0x00);
	
	LCD_WriteReg(0x3A);	// rgb565
	LCD_WriteData8(0x05);
	
	LCD_WriteReg(0xB2);
	LCD_WriteData8(0x0C);
	LCD_WriteData8(0x0C);
	LCD_WriteData8(0x00);
	LCD_WriteData8(0x33);
	LCD_WriteData8(0x33);
	
	LCD_WriteReg(0xB7);
	LCD_WriteData8(0x35);
	
	LCD_WriteReg(0xBB);
	LCD_WriteData8(0x19);
	
	LCD_WriteReg(0xC0);
	LCD_WriteData8(0x2C);
	
	LCD_WriteReg(0xC2);
	LCD_WriteData8(0x01);
	
	LCD_WriteReg(0xC3);
	LCD_WriteData8(0x12);
	
	LCD_WriteReg(0xC4);
	LCD_WriteData8(0x20);
	
	LCD_WriteReg(0xC6);
	LCD_WriteData8(0x0F);
	
	LCD_WriteReg(0xD0);
	LCD_WriteData8(0xA4);
	LCD_WriteData8(0xA1);
	
	LCD_WriteReg(0xE0);
	LCD_WriteData8(0xD0);
	LCD_WriteData8(0x04);
	LCD_WriteData8(0x0D);
	LCD_WriteData8(0x11);
	LCD_WriteData8(0x13);
	LCD_WriteData8(0x2B);
	LCD_WriteData8(0x3F);
	LCD_WriteData8(0x54);
	LCD_WriteData8(0x4C);
	LCD_WriteData8(0x18);
	LCD_WriteData8(0x0D);
	LCD_WriteData8(0x0B);
	LCD_WriteData8(0x1F);
	LCD_WriteData8(0x23);
	
	LCD_WriteReg(0xE1);
	LCD_WriteData8(0xD0);
	LCD_WriteData8(0x04);
	LCD_WriteData8(0x0C);
	LCD_WriteData8(0x11);
	LCD_WriteData8(0x13);
	LCD_WriteData8(0x2C);
	LCD_WriteData8(0x3F);
	LCD_WriteData8(0x44);
	LCD_WriteData8(0x51);
	LCD_WriteData8(0x2F);
	LCD_WriteData8(0x1F);
	LCD_WriteData8(0x1F);
	LCD_WriteData8(0x20);
	LCD_WriteData8(0x23);
	
	LCD_WriteReg(0x21);
	LCD_WriteReg(0x29);

}

void LCD_Address_Set(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye)
{
	LCD_WriteReg(0x2A);
	LCD_WriteData16(xs);
	LCD_WriteData16(xe);
	LCD_WriteReg(0x2B);
	LCD_WriteData16(ys);
	LCD_WriteData16(ye);
	LCD_WriteReg(0x2C);
}

// 在指定区域田中颜色
void LCD_Fill(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye, uint16_t color)
{
	uint16_t i, j;
	
	LCD_Address_Set(xs, ys, xe - 1, ye - 1);
	for (i = ys; i < ye; i++)
	{
		for (j = xs; j < xe; j++)
		{
			LCD_WriteData16(color);
		}
	}
}

// 画点
void LCD_DrawPoint(uint16_t x, uint16_t y, uint16_t color)
{
	LCD_Address_Set(x, y, x, y);
	LCD_WriteData16(color);
}

// 画线
void LCD_DrawLine(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye, uint16_t color)
{
	uint16_t i;
	int x = 0, y = 0, dx, dy, offset;
	int stepx, stepy, nowx, nowy;
	
	dx = xe - xs;
	dy = ye - ys;
	
	nowx = xs;
	nowy = ys;
	
	stepx = (dx > 0) ? 1 : ((dx == 0) ? 0 : -1);
	stepy = (dy > 0) ? 1 : ((dy == 0) ? 0 : -1);
	dx = (stepx >= 0) ? dx : -dx;
	dy = (stepy >= 0) ? dy : -dy;
	offset = (dx > dy) ? dx : dy;
	
	for (i = 0; i < (offset + 1); i++)
	{
		LCD_DrawPoint(nowx, nowy, color);
		x += dx;
		y += dy;
		if (x > offset)
		{
			x -= offset;
			nowx += stepx;
		}
		if (y > offset)
		{
			y -= offset;
			nowy += stepy;
		}
	}
}

// 画矩形
void LCD_DrawRectangle(uint16_t xs, uint16_t ys, uint16_t xe, uint16_t ye, uint16_t color)
{
	LCD_DrawLine(xs, ys, xe, ys, color);
	LCD_DrawLine(xe, ys, xe, ye, color);
	LCD_DrawLine(xe, ye, xs, ye, color);
	LCD_DrawLine(xs, ye, xs, ys, color);
}

// 画圆
void LCD_DrawCircle(uint16_t x, uint16_t y, uint8_t r, uint16_t color)
{
	int a, b;
	
	a = 0;
	b = r;
	while (a <= b)
	{
		LCD_DrawPoint(x - b, y - a, color);
		LCD_DrawPoint(x - b, y + a, color);
		LCD_DrawPoint(x + b, y - a, color);
		LCD_DrawPoint(x + b, y + a, color);
		LCD_DrawPoint(x + a, y - b, color);
		LCD_DrawPoint(x - a, y - b, color);
		LCD_DrawPoint(x + a, y + b, color);
		LCD_DrawPoint(x - a, y + b, color);
		a++;
		if ((a * a + b * b) > (r * r))
		{
			b--;
		}
	}
}

// 显示图片
void LCD_ShowPicture(uint16_t x, uint16_t y, uint16_t length, uint16_t width, uint8_t *data)
{
	LCD_Address_Set(x, y, x + length - 1, y + width - 1);
	LCD_WriteData(data, length * width * 2);
}
