/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date         Author        Notes
 * 2018-12-04   balanceTWK    first version
 */

#include "board.h"
#include<rtthread.h>
#include<rtdevice.h>

#ifdef BSP_USING_LCD

#define LCD_DB_ADDR   0xC0000000
#define LCD_DB_PTR    ((volatile uint16_t *)LCD_DB_ADDR)

static LTDC_HandleTypeDef LtdcHandle = {0};

int lcd_init(void)
{
    LTDC_LayerCfgTypeDef pLayerCfg = {0};

    /* LTDC Initialization -------------------------------------------------------*/

    /* Polarity configuration */
    /* Initialize the horizontal synchronization polarity as active low */
    LtdcHandle.Init.HSPolarity = LTDC_HSPOLARITY_AL;
    /* Initialize the vertical synchronization polarity as active low */
    LtdcHandle.Init.VSPolarity = LTDC_VSPOLARITY_AL;
    /* Initialize the data enable polarity as active low */
    LtdcHandle.Init.DEPolarity = LTDC_DEPOLARITY_AL;
    /* Initialize the pixel clock polarity as input pixel clock */
    LtdcHandle.Init.PCPolarity = LTDC_PCPOLARITY_IPC;

    /* Timing configuration */
    /* Horizontal synchronization width = Hsync - 1 */
    LtdcHandle.Init.HorizontalSync = LCD_HSYNC_WIDTH - 1;
    /* Vertical synchronization height = Vsync - 1 */
    LtdcHandle.Init.VerticalSync = LCD_VSYNC_HEIGHT - 1;
    /* Accumulated horizontal back porch = Hsync + HBP - 1 */
    LtdcHandle.Init.AccumulatedHBP = LCD_HSYNC_WIDTH + LCD_HBP - 1;
    /* Accumulated vertical back porch = Vsync + VBP - 1 */
    LtdcHandle.Init.AccumulatedVBP = LCD_VSYNC_HEIGHT + LCD_VBP - 1;
    /* Accumulated active width = Hsync + HBP + Active Width - 1 */
    LtdcHandle.Init.AccumulatedActiveW = LCD_HSYNC_WIDTH + LCD_HBP + LCD_WIDTH - 1 ;
    /* Accumulated active height = Vsync + VBP + Active Heigh - 1 */
    LtdcHandle.Init.AccumulatedActiveH = LCD_VSYNC_HEIGHT + LCD_VBP + LCD_HEIGHT - 1;
    /* Total height = Vsync + VBP + Active Heigh + VFP - 1 */
    LtdcHandle.Init.TotalHeigh = LtdcHandle.Init.AccumulatedActiveH + LCD_VFP;
    /* Total width = Hsync + HBP + Active Width + HFP - 1 */
    LtdcHandle.Init.TotalWidth = LtdcHandle.Init.AccumulatedActiveW + LCD_HFP;

    /* Configure R,G,B component values for LCD background color */
    LtdcHandle.Init.Backcolor.Blue = 0;
    LtdcHandle.Init.Backcolor.Green = 0;
    LtdcHandle.Init.Backcolor.Red = 0;

    LtdcHandle.Instance = LTDC;

    /* Layer1 Configuration ------------------------------------------------------*/

    /* Windowing configuration */
    pLayerCfg.WindowX0 = 0;
    pLayerCfg.WindowX1 = LCD_WIDTH;
    pLayerCfg.WindowY0 = 0;
    pLayerCfg.WindowY1 = LCD_HEIGHT;
    pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;

    /* Start Address configuration : frame buffer is located at FLASH memory */
    pLayerCfg.FBStartAdress = LCD_DB_ADDR;

    /* Alpha constant (255 totally opaque) */
    pLayerCfg.Alpha = 255;

    /* Default Color configuration (configure A,R,G,B component values) */
    pLayerCfg.Alpha0 = 255;
    pLayerCfg.Backcolor.Blue = 0;
    pLayerCfg.Backcolor.Green = 0;
    pLayerCfg.Backcolor.Red = 0;

    /* Configure blending factors */
    /* Constant Alpha value:  pLayerCfg.Alpha / 255
       C: Current Layer Color
       Cs: Background color
       BC = Constant Alpha x C + (1 - Constant Alpha ) x Cs */
    /* BlendingFactor1: Pixel Alpha x Constant Alpha */
    pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
    /* BlendingFactor2: 1 - (Pixel Alpha x Constant Alpha) */
    pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;

    /* Configure the number of lines and number of pixels per line */
    pLayerCfg.ImageWidth = LCD_WIDTH;
    pLayerCfg.ImageHeight = LCD_HEIGHT;

    /* Configure the LTDC */
    if (HAL_LTDC_Init(&LtdcHandle) != HAL_OK)
    {
        rt_kprintf("LTDC init failed");
        return -RT_ERROR;
    }

    /* Configure the Background Layer*/
    if (HAL_LTDC_ConfigLayer(&LtdcHandle, &pLayerCfg, 0) != HAL_OK)
    {
        rt_kprintf("LTDC layer init failed");
        return -RT_ERROR;
    }
    return RT_EOK;
}
INIT_PREV_EXPORT(lcd_init);

int32_t GLCD_DrawBitmap (uint32_t x,
                         uint32_t y,
                         uint32_t width,
                         uint32_t height,
                         const uint8_t *bitmap)
{
    volatile uint16_t *phwDes = LCD_DB_PTR + y * LCD_WIDTH + x;
    const uint16_t *phwSrc = (const uint16_t *)bitmap;
    for (int_fast16_t i = 0; i < height; i++) {
        rt_memcpy ((uint16_t *)phwDes, phwSrc, width * 2);
        phwSrc += width;
        phwDes += LCD_WIDTH;
    }
    return 0;
}

#endif /* BSP_USING_LCD */
