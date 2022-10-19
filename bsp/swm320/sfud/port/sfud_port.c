/*
 * This file is part of the Serial Flash Universal Driver Library.
 *
 * Copyright (c) 2016, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: Portable interface for each platform.
 * Created on: 2016-04-23
 */

#include <sfud.h>
#include <stdarg.h>
#include <stdio.h>
#include "SWM320.h"

typedef struct
{
    SPI_TypeDef *spix;
    GPIO_TypeDef *cs_gpiox;
    uint16_t cs_gpio_pin;
} spi_user_data, *spi_user_data_t;

static char log_buf[256];

void sfud_log_debug(const char *file, const long line, const char *format, ...);

static spi_user_data spi0 = {.spix = SPI0, .cs_gpiox = GPIOP, .cs_gpio_pin = PIN22};
static void spi_configuration(spi_user_data_t spi)
{
    SPI_InitStructure SPI_initStruct;

    GPIO_Init(GPIOP, PIN22, 1, 0, 0);

    PORT_Init(PORTP, PIN23, FUNMUX1_SPI0_SCLK, 0);
    PORT_Init(PORTP, PIN18, FUNMUX0_SPI0_MOSI, 0);
    PORT_Init(PORTP, PIN19, FUNMUX1_SPI0_MISO, 1);

    SPI_initStruct.clkDiv = SPI_CLKDIV_4;
    SPI_initStruct.FrameFormat = SPI_FORMAT_SPI;
    SPI_initStruct.SampleEdge = SPI_SECOND_EDGE;
    SPI_initStruct.IdleLevel = SPI_HIGH_LEVEL;
    SPI_initStruct.WordSize = 8;
    SPI_initStruct.Master = 1;
    SPI_initStruct.RXHFullIEn = 0;
    SPI_initStruct.TXEmptyIEn = 0;
    SPI_initStruct.TXCompleteIEn = 0;
    SPI_Init(spi->spix, &SPI_initStruct);
    SPI_Open(spi->spix);
}

static void spi_lock(const sfud_spi *spi)
{
    __disable_irq();
}

static void spi_unlock(const sfud_spi *spi)
{
    __enable_irq();
}

/**
 * SPI write data then read data
 */
static sfud_err spi_write_read(const sfud_spi *spi, const uint8_t *write_buf, size_t write_size, uint8_t *read_buf,
                               size_t read_size)
{
    sfud_err result = SFUD_SUCCESS;
    uint8_t send_data, read_data;
    spi_user_data_t spi_dev = (spi_user_data_t)spi->user_data;

    if (write_size)
    {
        SFUD_ASSERT(write_buf);
    }
    if (read_size)
    {
        SFUD_ASSERT(read_buf);
    }

    GPIO_ClrBit(spi_dev->cs_gpiox, spi_dev->cs_gpio_pin);
    /* 开始读写数据 */
    for (size_t i = 0, retry_times; i < write_size + read_size; i++)
    {
        /* 先写缓冲区中的数据到 SPI 总线，数据写完后，再写 dummy(0xFF) 到 SPI 总线 */
        if (i < write_size)
        {
            send_data = *write_buf++;
        }
        else
        {
            send_data = SFUD_DUMMY_DATA;
        }
        /* 发送数据 */
        retry_times = 1000;
        while (SPI_IsTXFull(spi_dev->spix))
        {
            SFUD_RETRY_PROCESS(NULL, retry_times, result);
        }
        if (result != SFUD_SUCCESS)
        {
            goto exit;
        }
        SPI_Write(spi_dev->spix, send_data);
        /* 接收数据 */
        retry_times = 1000;
        while (SPI_IsRXEmpty(spi_dev->spix))
        {
            SFUD_RETRY_PROCESS(NULL, retry_times, result);
        }
        if (result != SFUD_SUCCESS)
        {
            goto exit;
        }
        read_data = SPI_Read(spi_dev->spix);
        /* 写缓冲区中的数据发完后，再读取 SPI 总线中的数据到读缓冲区 */
        if (i >= write_size)
        {
            *read_buf++ = read_data;
        }
    }

exit:
    GPIO_SetBit(spi_dev->cs_gpiox, spi_dev->cs_gpio_pin);

    return result;
}

/* about 100 microsecond delay */
static void retry_delay_100us(void)
{
    uint32_t delay = 120;
    while (delay--)
        ;
}

sfud_err sfud_spi_port_init(sfud_flash *flash)
{
    sfud_err result = SFUD_SUCCESS;

    switch (flash->index)
    {
    case SFUD_W25_DEVICE_INDEX:
    {

        /* SPI 外设初始化 */
        spi_configuration(&spi0);
        /* 同步 Flash 移植所需的接口及数据 */
        flash->spi.wr = spi_write_read;
        flash->spi.lock = spi_lock;
        flash->spi.unlock = spi_unlock;
        flash->spi.user_data = &spi0;
        /* about 100 microsecond delay */
        flash->retry.delay = retry_delay_100us;
        /* adout 60 seconds timeout */
        flash->retry.times = 60 * 10000;

        break;
    }
    }

    return result;
}

/**
 * This function is print debug info.
 *
 * @param file the file which has call this function
 * @param line the line number which has call this function
 * @param format output format
 * @param ... args
 */
void sfud_log_debug(const char *file, const long line, const char *format, ...)
{
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);
    printf("[SFUD](%s:%ld) ", file, line);
    /* must use vprintf to print */
    vsnprintf(log_buf, sizeof(log_buf), format, args);
    printf("%s\r\n", log_buf);
    va_end(args);
}

/**
 * This function is print routine info.
 *
 * @param format output format
 * @param ... args
 */
void sfud_log_info(const char *format, ...)
{
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);
    printf("[SFUD]");
    /* must use vprintf to print */
    vsnprintf(log_buf, sizeof(log_buf), format, args);
    printf("%s\r\n", log_buf);
    va_end(args);
}
