#include "LCD_driver.h"
#include <FreeRTOS.h>
#include <stdio.h>
#include <stdlib.h>
#include <task.h>

volatile static pika_dev* LCD_DC = NULL;
volatile static pika_dev* LCD_SPI = NULL;

static inline void _LCD_RS_SET(void) {
    uint32_t val = 1;
    pika_hal_write((pika_dev*)LCD_DC, &val, sizeof(val));
}

static inline void _LCD_RS_CLR(void) {
    uint32_t val = 0;
    pika_hal_write((pika_dev*)LCD_DC, &val, sizeof(val));
}

// 管理LCD重要参数
// 默认为竖屏
_lcd_dev lcddev;

// 画笔颜色,背景颜色
u16 POINT_COLOR = 0x0000, BACK_COLOR = 0xFFFF;
u16 DeviceCode;

#define LCD_WRITE_BATCH_SIZE 2048
void LCD_write(uint8_t* data, uint32_t len) {
    for (int i = 0; i < len / LCD_WRITE_BATCH_SIZE; i++) {
        pika_hal_write((pika_dev*)LCD_SPI, data + i * LCD_WRITE_BATCH_SIZE,
                       LCD_WRITE_BATCH_SIZE);
    }
    pika_hal_write((pika_dev*)LCD_SPI,
                   data + len / LCD_WRITE_BATCH_SIZE * LCD_WRITE_BATCH_SIZE,
                   len % LCD_WRITE_BATCH_SIZE);
}

void LCD_WR_REG(u8 data) {
    LCD_RS_CLR();
    LCD_write(&data, 1);
}

void LCD_WR_DATA(u8 data) {
    LCD_RS_SET();
    LCD_write(&data, 1);
}

void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue) {
    LCD_WR_REG(LCD_Reg);
    LCD_WR_DATA(LCD_RegValue);
}

void LCD_WriteRAM_Prepare(void) {
    LCD_WR_REG(lcddev.wramcmd);
}

static inline void color565_to_666(uint16_t color565, uint8_t* color666) {
    color666[0] = (color565 >> 8) & 0xF8;
    color666[1] = (color565 >> 3) & 0xFC;
    color666[2] = (color565 << 3);
}

void LCD_writePoint(u16 color) {
    /* rgb 666 */
    uint8_t data[3] = {0};
    color565_to_666(color, data);
    LCD_RS_SET();
    LCD_write(data, 3);
}

void LCD_drawPoint(u16 x, u16 y) {
    LCD_setCursor(x, y);  // 设置光标位置
    LCD_writePoint(POINT_COLOR);
}

void LCD_drawRegin(u16 x_start,
                   u16 y_start,
                   u16 x_end,
                   u16 y_end,
                   uint8_t* pData) {
    u32 size = (x_end - x_start) * (y_end - y_start) * 2;
    LCD_setRegion(x_start, y_start, x_end - 1, y_end - 1);
    LCD_RS_SET();
    LCD_write(pData, size);
}

#define BUFF_LINE_NUM 4
void LCD_fillRegin(u16 x_start, u16 y_start, u16 x_end, u16 y_end, u16 color) {
    size_t line_pix_size = x_end - x_start + 1;
    size_t buff_pix_size = (x_end - x_start + 1) * BUFF_LINE_NUM;
    uint8_t* buff = malloc(buff_pix_size * 3);
    if (NULL == buff) {
        printf("Error: malloc %d bytes failed\r\n", buff_pix_size * 3);
    }
    uint8_t color666[3] = {0};
    color565_to_666(color, color666);
    for (int i = 0; i < buff_pix_size; i++) {
        buff[i * 3] = color666[0];
        buff[i * 3 + 1] = color666[1];
        buff[i * 3 + 2] = color666[2];
    }
    LCD_setRegion(x_start, y_start, x_end, y_end);
    LCD_RS_SET();
    for (int i = 0; i < (y_end - y_start + 1) / BUFF_LINE_NUM; i++) {
        LCD_write(buff, buff_pix_size * 3);
    }
    if ((y_end - y_start + 1) % BUFF_LINE_NUM) {
        LCD_write(buff,
                  line_pix_size * 3 * ((y_end - y_start + 1) % BUFF_LINE_NUM));
    }
    free(buff);
}

void LCD_clear(u16 Color) {
    LCD_fillRegin(0, 0, lcddev.width - 1, lcddev.height - 1, Color);
}

#define SPI_MODE_SOFT 1
#define SPI_MODE_HARD 2

#define SPI_MODE SPI_MODE_HARD

void LCD_GPIOInit(void) {
/* init spi */
#if SPI_MODE == SPI_MODE_SOFT
    LCD_SPI = pika_hal_open(PIKA_HAL_SOFT_SPI, "SPI0");
    pika_hal_SOFT_SPI_config cfg_SPI = {0};
    cfg_SPI.CS = NULL;
    cfg_SPI.SCK = pika_hal_open(PIKA_HAL_GPIO, "P3");
    cfg_SPI.MISO = pika_hal_open(PIKA_HAL_GPIO, "P21");
    cfg_SPI.MOSI = pika_hal_open(PIKA_HAL_GPIO, "P20");
#elif SPI_MODE == SPI_MODE_HARD
    LCD_SPI = pika_hal_open(PIKA_HAL_SPI, "SPI0");
    pika_hal_SPI_config cfg_SPI = {0};
#endif
    cfg_SPI.master_or_slave = PIKA_HAL_SPI_MASTER;
    cfg_SPI.mode = PIKA_HAL_SPI_MODE_0;
    cfg_SPI.data_width = PIKA_HAL_SPI_DATA_WIDTH_8;
    cfg_SPI.speed = 40 * 1000 * 1000;
    pika_hal_ioctl((pika_dev*)LCD_SPI, PIKA_HAL_IOCTL_CONFIG, &cfg_SPI);
    pika_hal_ioctl((pika_dev*)LCD_SPI, PIKA_HAL_IOCTL_ENABLE);

    /* init gpio */
    LCD_DC = pika_hal_open(PIKA_HAL_GPIO, "P0");
    pika_hal_GPIO_config cfg_DC = {0};
    cfg_DC.dir = PIKA_HAL_GPIO_DIR_OUT;
    pika_hal_ioctl((pika_dev*)LCD_DC, PIKA_HAL_IOCTL_CONFIG, &cfg_DC);
    pika_hal_ioctl((pika_dev*)LCD_DC, PIKA_HAL_IOCTL_ENABLE);

    pika_dev* LCD_CS = pika_hal_open(PIKA_HAL_GPIO, "P22");
    pika_hal_GPIO_config cfg_CS = {0};
    cfg_CS.dir = PIKA_HAL_GPIO_DIR_OUT;
    pika_hal_ioctl((pika_dev*)LCD_CS, PIKA_HAL_IOCTL_CONFIG, &cfg_CS);
    pika_hal_ioctl((pika_dev*)LCD_CS, PIKA_HAL_IOCTL_ENABLE);
    uint32_t val = 0;
    pika_hal_write((pika_dev*)LCD_CS, &val, sizeof(val));
}

void LCD_RESET(void) {}

void LCD_init(void) {
    LCD_GPIOInit();  // LCD GPIO初始化
    LCD_RESET();     // LCD 复位
    //************* ILI9488初始化**********//
    LCD_WR_REG(0XF7);
    LCD_WR_DATA(0xA9);
    LCD_WR_DATA(0x51);
    LCD_WR_DATA(0x2C);
    LCD_WR_DATA(0x82);
    LCD_WR_REG(0xC0);
    LCD_WR_DATA(0x11);
    LCD_WR_DATA(0x09);
    LCD_WR_REG(0xC1);
    LCD_WR_DATA(0x41);
    LCD_WR_REG(0XC5);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x0A);
    LCD_WR_DATA(0x80);
    LCD_WR_REG(0xB1);
    LCD_WR_DATA(0xB0);
    LCD_WR_DATA(0x11);
    LCD_WR_REG(0xB4);
    LCD_WR_DATA(0x02);
    LCD_WR_REG(0xB6);
    LCD_WR_DATA(0x02);
    LCD_WR_DATA(0x42);
    LCD_WR_REG(0xB7);
    LCD_WR_DATA(0xc6);
    LCD_WR_REG(0xBE);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x04);
    LCD_WR_REG(0xE9);
    LCD_WR_DATA(0x00);
    LCD_WR_REG(0x36);
    LCD_WR_DATA((1 << 3) | (0 << 7) | (1 << 6) | (1 << 5));
    LCD_WR_REG(0x3A);
    LCD_WR_DATA(0x66);
    LCD_WR_REG(0xE0);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x07);
    LCD_WR_DATA(0x10);
    LCD_WR_DATA(0x09);
    LCD_WR_DATA(0x17);
    LCD_WR_DATA(0x0B);
    LCD_WR_DATA(0x41);
    LCD_WR_DATA(0x89);
    LCD_WR_DATA(0x4B);
    LCD_WR_DATA(0x0A);
    LCD_WR_DATA(0x0C);
    LCD_WR_DATA(0x0E);
    LCD_WR_DATA(0x18);
    LCD_WR_DATA(0x1B);
    LCD_WR_DATA(0x0F);
    LCD_WR_REG(0XE1);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x17);
    LCD_WR_DATA(0x1A);
    LCD_WR_DATA(0x04);
    LCD_WR_DATA(0x0E);
    LCD_WR_DATA(0x06);
    LCD_WR_DATA(0x2F);
    LCD_WR_DATA(0x45);
    LCD_WR_DATA(0x43);
    LCD_WR_DATA(0x02);
    LCD_WR_DATA(0x0A);
    LCD_WR_DATA(0x09);
    LCD_WR_DATA(0x32);
    LCD_WR_DATA(0x36);
    LCD_WR_DATA(0x0F);
    LCD_WR_REG(0x11);
    vTaskDelay(120 / portTICK_RATE_MS);
    LCD_WR_REG(0x29);

    LCD_direction(USE_HORIZONTAL);  // 设置LCD显示方向
    // LCD_LED = 1;                    // 点亮背光
    LCD_clear(WHITE);  // 清全屏白色
}

void LCD_setRegion(u16 xStar, u16 yStar, u16 xEnd, u16 yEnd) {
    LCD_WR_REG(lcddev.setxcmd);
    LCD_WR_DATA(xStar >> 8);
    LCD_WR_DATA(0x00FF & xStar);
    LCD_WR_DATA(xEnd >> 8);
    LCD_WR_DATA(0x00FF & xEnd);

    LCD_WR_REG(lcddev.setycmd);
    LCD_WR_DATA(yStar >> 8);
    LCD_WR_DATA(0x00FF & yStar);
    LCD_WR_DATA(yEnd >> 8);
    LCD_WR_DATA(0x00FF & yEnd);

    LCD_WriteRAM_Prepare();  // 开始写入GRAM
}

void LCD_setCursor(u16 Xpos, u16 Ypos) {
    LCD_setRegion(Xpos, Ypos, Xpos, Ypos);
}

void LCD_direction(u8 direction) {
    lcddev.setxcmd = 0x2A;
    lcddev.setycmd = 0x2B;
    lcddev.wramcmd = 0x2C;
    switch (direction) {
        case 0:
            lcddev.width = LCD_W;
            lcddev.height = LCD_H;
            LCD_WriteReg(0x36, (1 << 3) | (0 << 6) |
                                   (0 << 7));  // BGR==1,MY==0,MX==0,MV==0
            break;
        case 1:
            lcddev.width = LCD_H;
            lcddev.height = LCD_W;
            LCD_WriteReg(0x36, (1 << 3) | (0 << 7) | (1 << 6) |
                                   (1 << 5));  // BGR==1,MY==1,MX==0,MV==1
            break;
        case 2:
            lcddev.width = LCD_W;
            lcddev.height = LCD_H;
            LCD_WriteReg(0x36, (1 << 3) | (1 << 6) |
                                   (1 << 7));  // BGR==1,MY==0,MX==0,MV==0
            break;
        case 3:
            lcddev.width = LCD_H;
            lcddev.height = LCD_W;
            LCD_WriteReg(0x36, (1 << 3) | (1 << 7) |
                                   (1 << 5));  // BGR==1,MY==1,MX==0,MV==1
            break;
        default:
            break;
    }
}
