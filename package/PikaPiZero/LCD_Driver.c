#include "LCD_Driver.h"
#include "../PikaStdDevice/pika_hal.h"
#include "LCD_Config.h"
#include "main.h"
#include "pika_bsp.h"
/* config SPI mode, chocie one from three */
// #define SPI_SOFT
// #define SPI_HARD

void SPI_Write_u8(u8 Data);
pika_dev* pika_hal_spi;

//液晶IO初始化配置
void LCD_GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /*Configure GPIO pin : PA0 */
    GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_5;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
#ifdef SPI_SOFT
    GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
#endif
}

void SPI_WriteData_u8(u8* pData, uint32_t size) {
    pika_hal_write(pika_hal_spi, pData, size);
}

void SPI_WriteData_u16(u16* pData, uint32_t size) {
#ifdef SPI_SOFT
    for (int i = 0; i < size / 2; i++) {
        SPI_Write_u8(pData[i] >> 8);  //写入低8位数据
        SPI_Write_u8(pData[i]);       //写入高8位数据
    }
#else
    u8* pData_u8 = (u8*)pData;
    SPI_WriteData_u8(pData_u8, size);
#endif
}

//向SPI总线传输一个8位数据
void SPI_Write_u8(u8 Data) {
#ifdef SPI_SOFT
    unsigned char i = 0;
    for (i = 8; i > 0; i--) {
        if (Data & 0x80)
            LCD_SDA_SET;  //输出数据
        else
            LCD_SDA_CLR;

        LCD_SCL_CLR;
        LCD_SCL_SET;
        Data <<= 1;
    }
#else
    SPI_WriteData_u8(&Data, 1);
#endif
}

//向液晶屏写一个8位指令
void LCD_WriteIndex(u8 Index) {
    // SPI 写命令时序开始
    LCD_RS_CLR;
    SPI_Write_u8(Index);
}

//向液晶屏写一个8位数据
void LCD_Write_u8(u8 Data) {
    LCD_RS_SET;
    SPI_Write_u8(Data);
}

//向液晶屏写一个16位数据
void LCD_Write_u16(u16 Data) {
    LCD_RS_SET;
    SPI_Write_u8(Data >> 8);  //写入高8位数据
    SPI_Write_u8(Data);       //写入低8位数据
}

void LCD_WriteData_u16(u16* pData, uint32_t size) {
    LCD_RS_SET;
    SPI_WriteData_u16(pData, size);  //写入高8位数据
}

void LCD_WriteReg(u8 Index, u8 Data) {
    LCD_WriteIndex(Index);
    LCD_Write_u8(Data);
}

void LCD_Reset(void) {
    LCD_RST_CLR;
    delay_ms(100);
    LCD_RST_SET;
    delay_ms(50);
}

// LCD Init For 1.44Inch LCD Panel with ST7735R.
void LCD_Init(void) {
    LCD_GPIO_Init();
    /* enable spi hal */
    pika_hal_spi = pika_hal_open(PIKA_HAL_SPI, "spi1");
    pika_hal_ioctl(pika_hal_spi, PIKA_HAL_IOCTL_ENABLE);
    LCD_Reset();  // Reset before LCD Init.
    // LCD Init For 1.8Inch LCD Panel with ST7735S.
    LCD_WriteIndex(0x11);  // Sleep exit
    delay_ms(120);

    LCD_WriteIndex(0xB1);
    LCD_Write_u8(0x05);
    LCD_Write_u8(0x3C);
    LCD_Write_u8(0x3C);
    LCD_WriteIndex(0xB2);
    LCD_Write_u8(0x05);
    LCD_Write_u8(0x3C);
    LCD_Write_u8(0x3C);
    LCD_WriteIndex(0xB3);
    LCD_Write_u8(0x05);
    LCD_Write_u8(0x3C);
    LCD_Write_u8(0x3C);
    LCD_Write_u8(0x05);
    LCD_Write_u8(0x3C);
    LCD_Write_u8(0x3C);
    //------------------------------------End ST7735S Frame
    // Rate---------------------------------//
    LCD_WriteIndex(0xB4);  // Dot inversion
    LCD_Write_u8(0x03);
    //------------------------------------ST7735S Power
    // Sequence---------------------------------//
    LCD_WriteIndex(0xC0);
    LCD_Write_u8(0x28);
    LCD_Write_u8(0x08);
    LCD_Write_u8(0x04);
    LCD_WriteIndex(0xC1);
    LCD_Write_u8(0XC0);
    LCD_WriteIndex(0xC2);
    LCD_Write_u8(0x0D);
    LCD_Write_u8(0x00);
    LCD_WriteIndex(0xC3);
    LCD_Write_u8(0x8D);
    LCD_Write_u8(0x2A);
    LCD_WriteIndex(0xC4);
    LCD_Write_u8(0x8D);
    LCD_Write_u8(0xEE);
    //---------------------------------End ST7735S Power
    // Sequence-------------------------------------//
    LCD_WriteIndex(0xC5);  // VCOM
    LCD_Write_u8(0x1A);
    LCD_WriteIndex(0x36);  // MX, MY, RGB mode
    LCD_Write_u8(0xC0);
    //------------------------------------ST7735S Gamma
    // Sequence---------------------------------//
    LCD_WriteIndex(0xE0);
    LCD_Write_u8(0x04);
    LCD_Write_u8(0x22);
    LCD_Write_u8(0x07);
    LCD_Write_u8(0x0A);
    LCD_Write_u8(0x2E);
    LCD_Write_u8(0x30);
    LCD_Write_u8(0x25);
    LCD_Write_u8(0x2A);
    LCD_Write_u8(0x28);
    LCD_Write_u8(0x26);
    LCD_Write_u8(0x2E);
    LCD_Write_u8(0x3A);
    LCD_Write_u8(0x00);
    LCD_Write_u8(0x01);
    LCD_Write_u8(0x03);
    LCD_Write_u8(0x13);
    LCD_WriteIndex(0xE1);
    LCD_Write_u8(0x04);
    LCD_Write_u8(0x16);
    LCD_Write_u8(0x06);
    LCD_Write_u8(0x0D);
    LCD_Write_u8(0x2D);
    LCD_Write_u8(0x26);
    LCD_Write_u8(0x23);
    LCD_Write_u8(0x27);
    LCD_Write_u8(0x27);
    LCD_Write_u8(0x25);
    LCD_Write_u8(0x2D);
    LCD_Write_u8(0x3B);
    LCD_Write_u8(0x00);
    LCD_Write_u8(0x01);
    LCD_Write_u8(0x04);
    LCD_Write_u8(0x13);
    //------------------------------------End ST7735S Gamma
    // Sequence-----------------------------//
    LCD_WriteIndex(0x3A);  // 65k mode
    LCD_Write_u8(0x05);
    LCD_WriteIndex(0x29);  // Display on
}

/*************************************************
函数名：LCD_Set_Region
功能：设置lcd显示区域，在此区域写点数据自动换行
入口参数：xy起点和终点
返回值：无
*************************************************/
void LCD_SetRegion(u16 x_start, u16 y_start, u16 x_end, u16 y_end) {
    LCD_WriteIndex(0x2a);
    uint8_t data[4];
    data[0] = (X_OFFSET + x_start) >> 8;
    data[1] = (X_OFFSET + x_start) & 0XFF;
    data[2] = (X_OFFSET + x_end) >> 8;
    data[3] = (X_OFFSET + x_end) & 0XFF;
    LCD_RS_SET;
    SPI_WriteData_u8(data, sizeof(data));
    LCD_WriteIndex(0x2b);
    data[0] = (Y_OFFSET + y_start) >> 8;
    data[1] = (Y_OFFSET + y_start) & 0XFF;
    data[2] = (Y_OFFSET + y_end) >> 8;
    data[3] = (Y_OFFSET + y_end) & 0XFF;
    LCD_RS_SET;
    SPI_WriteData_u8(data, sizeof(data));
    LCD_WriteIndex(0x2c);
}

/*************************************************
函数名：LCD_Set_XY
功能：设置lcd显示起始点
入口参数：xy坐标
返回值：无
*************************************************/
void LCD_SetXY(u16 x, u16 y) {
    LCD_SetRegion(x, y, x, y);
}

/*************************************************
函数名：LCD_DrawPoint
功能：画一个点
入口参数：无
返回值：无
*************************************************/
void LCD_DrawPoint(u16 x, u16 y, u16 Data) {
    LCD_SetRegion(x, y, x + 1, y + 1);
    LCD_Write_u16(Data);
}

void LCD_DrawRegin(u16 x_start, u16 y_start, u16 x_end, u16 y_end, u16* pData) {
    u32 size = (x_end - x_start) * (y_end - y_start) * 2;
    LCD_SetRegion(x_start, y_start, x_end - 1, y_end - 1);
    LCD_RS_SET;
    SPI_WriteData_u16(pData, size);
}

#define BUFF_SIZE (X_MAX_PIXEL)
void LCD_Clear(u16 Color) {
    unsigned int i, m;
    u16 data[BUFF_SIZE];
    for (i = 0; i < BUFF_SIZE; i++) {
        data[i] = Color;
    }
    int m_max = Y_MAX_PIXEL;
    for (m = 0; m < m_max; m++) {
        LCD_DrawRegin(0, m, X_MAX_PIXEL, m + 1, data);
    }
}

void LCD_Fill(u16 x0, u16 y0, u16 hight, u16 wight, u16 color) {
    unsigned int i, y;
    u16 data[BUFF_SIZE];
    for (i = 0; i < BUFF_SIZE; i++) {
        data[i] = color;
    }
    int y_end = y0 + hight;
    for (y = y0; y < y_end; y++) {
        LCD_DrawRegin(x0, y, x0 + wight, y + 1, data);
    }
}
