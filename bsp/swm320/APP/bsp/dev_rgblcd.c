#include "main.h"
#include "lv_conf.h"

void lcd_memory_init(void)
{
    SDRAM_InitStructure SDRAM_InitStruct;
        
    PORT->PORTP_SEL0 = 0xAAAAAAAA; //PP0-23 => ADDR0-23
    PORT->PORTP_SEL1 &= ~0x00000F0F;
    PORT->PORTP_SEL1 |= 0x00000A0A;

    PORT->PORTM_SEL0 = 0xAAAAAAAA; //PM0-15 => DATA15-0
    PORT->PORTM_INEN = 0xFFFF;
    //PM16 => OEN,PM17 => WEN,PM18 => NORFL_CSN,PM19 => SDRAM_CSN,PM20 => SRAM_CSN,PM21 => SDRAM_CKE
    PORT->PORTM_SEL1 = 0x888;

    SDRAM_InitStruct.CellSize = SDRAM_CELLSIZE_64Mb;
    SDRAM_InitStruct.CellBank = SDRAM_CELLBANK_4;
    SDRAM_InitStruct.CellWidth = SDRAM_CELLWIDTH_16;
    SDRAM_InitStruct.CASLatency = SDRAM_CASLATENCY_2;
    SDRAM_InitStruct.TimeTMRD = SDRAM_TMRD_3;
    SDRAM_InitStruct.TimeTRRD = SDRAM_TRRD_2;
    SDRAM_InitStruct.TimeTRAS = SDRAM_TRAS_6;
    SDRAM_InitStruct.TimeTRC  = SDRAM_TRC_8;
    SDRAM_InitStruct.TimeTRCD = SDRAM_TRCD_3;
    SDRAM_InitStruct.TimeTRP  = SDRAM_TRP_3;
    SDRAM_Init(&SDRAM_InitStruct);
}

void lcd_rgb_init(void)
{
    LCD_InitStructure LCD_initStruct;

    GPIO_Init(GPIOM, PIN20, 1, 0, 0); //复位
    GPIO_ClrBit(GPIOM, PIN20);
    swm_delay(1);
    GPIO_SetBit(GPIOM, PIN20);

    GPIO_Init(GPIOB, PIN12, 1, 0, 0); //背光控制
    GPIO_SetBit(GPIOB, PIN12);        //点亮背光

    PORT->PORTN_SEL0 = 0xAAAAAAAA; //GPION.0~15  LCD_DATA0~15
    PORT->PORTN_SEL1 = 0xAA;

    LCD_initStruct.Interface = LCD_INTERFACE_RGB;
    LCD_initStruct.HnPixel = LV_VER_RES_MAX;
    LCD_initStruct.VnPixel = LV_HOR_RES_MAX;
    LCD_initStruct.Hfp = 5;
    LCD_initStruct.Hbp = 40;
    LCD_initStruct.Vfp = 8;
    LCD_initStruct.Vbp = 8;
    LCD_initStruct.ClkDiv = LCD_CLKDIV_8;
    LCD_initStruct.ClkAlways = 0;
    LCD_initStruct.SamplEdge = LCD_SAMPLEDGE_FALL;
    LCD_initStruct.HsyncWidth = LCD_HSYNC_1DOTCLK;
    LCD_initStruct.IntEOTEn = 1;
    LCD_Init(LCD, &LCD_initStruct);
}

void LCD_Handler(void)
{
    LCD_INTClr(LCD);
    LCD_Start(LCD);
}
