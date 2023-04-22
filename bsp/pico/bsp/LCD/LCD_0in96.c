/*****************************************************************************
* | File      	:   LCD_0in96.c
* | Author      :   Waveshare team
* | Function    :   Hardware underlying interface
* | Info        :
*                Used to shield the underlying layers of each master
*                and enhance portability
*----------------
* |	This version:   V1.0
* | Date        :   2021-03-11
* | Info        :   Basic version
*
******************************************************************************/
#include "LCD_0in96.h"
#include "DEV_Config.h"

#include <stdlib.h>		//itoa()
#include <stdio.h>

LCD_0IN96_ATTRIBUTES LCD_0IN96;


/******************************************************************************
function :	Hardware reset
parameter:
******************************************************************************/
static void LCD_0IN96_Reset(void)
{
	DEV_Digital_Write(EPD_RST_PIN, 1);
    DEV_Delay_ms(200);
    DEV_Digital_Write(EPD_RST_PIN, 0);
    DEV_Delay_ms(200);
    DEV_Digital_Write(EPD_RST_PIN, 1);
    DEV_Delay_ms(200);
}

/******************************************************************************
function :	send command
parameter:
     Reg : Command register
******************************************************************************/
static void LCD_0IN96_SendCommand(UBYTE Reg)
{
    DEV_Digital_Write(EPD_DC_PIN, 0);
	DEV_Digital_Write(EPD_CS_PIN, 0);
    DEV_SPI_WriteByte(Reg);
	//DEV_Digital_Write(EPD_CS_PIN, 1);
}

/******************************************************************************
function :	send data
parameter:
    Data : Write data
******************************************************************************/
static void LCD_0IN96_SendData_8Bit(UBYTE Data)
{
    DEV_Digital_Write(EPD_DC_PIN, 1);
	DEV_Digital_Write(EPD_CS_PIN, 0);
    DEV_SPI_WriteByte(Data);
	DEV_Digital_Write(EPD_CS_PIN, 1);
}

/******************************************************************************
function :	send data
parameter:
    Data : Write data
******************************************************************************/
static void LCD_0IN96_SendData_16Bit(UWORD Data)
{
    DEV_Digital_Write(EPD_DC_PIN, 1);
	DEV_Digital_Write(EPD_CS_PIN, 0);
    DEV_SPI_WriteByte((Data >> 8) & 0xFF);
    DEV_SPI_WriteByte(Data);
	DEV_Digital_Write(EPD_CS_PIN, 1);
}

/******************************************************************************
function :	Initialize the lcd register
parameter:
******************************************************************************/
static void LCD_0IN96_InitReg(void)
{
  LCD_0IN96_SendCommand(0x11);//Sleep exit 
	DEV_Delay_ms (120);
	LCD_0IN96_SendCommand(0x21); 
	LCD_0IN96_SendCommand(0x21); 

	LCD_0IN96_SendCommand(0xB1); 
	LCD_0IN96_SendData_8Bit(0x05);
	LCD_0IN96_SendData_8Bit(0x3A);
	LCD_0IN96_SendData_8Bit(0x3A);

	LCD_0IN96_SendCommand(0xB2);
	LCD_0IN96_SendData_8Bit(0x05);
	LCD_0IN96_SendData_8Bit(0x3A);
	LCD_0IN96_SendData_8Bit(0x3A);

	LCD_0IN96_SendCommand(0xB3); 
	LCD_0IN96_SendData_8Bit(0x05);  
	LCD_0IN96_SendData_8Bit(0x3A);
	LCD_0IN96_SendData_8Bit(0x3A);
	LCD_0IN96_SendData_8Bit(0x05);
	LCD_0IN96_SendData_8Bit(0x3A);
	LCD_0IN96_SendData_8Bit(0x3A);

	LCD_0IN96_SendCommand(0xB4);
	LCD_0IN96_SendData_8Bit(0x03);

	LCD_0IN96_SendCommand(0xC0);
	LCD_0IN96_SendData_8Bit(0x62);
	LCD_0IN96_SendData_8Bit(0x02);
	LCD_0IN96_SendData_8Bit(0x04);

	LCD_0IN96_SendCommand(0xC1);
	LCD_0IN96_SendData_8Bit(0xC0);

	LCD_0IN96_SendCommand(0xC2);
	LCD_0IN96_SendData_8Bit(0x0D);
	LCD_0IN96_SendData_8Bit(0x00);

	LCD_0IN96_SendCommand(0xC3);
	LCD_0IN96_SendData_8Bit(0x8D);
	LCD_0IN96_SendData_8Bit(0x6A);   

	LCD_0IN96_SendCommand(0xC4);
	LCD_0IN96_SendData_8Bit(0x8D); 
	LCD_0IN96_SendData_8Bit(0xEE); 

	LCD_0IN96_SendCommand(0xC5);  /*VCOM*/
	LCD_0IN96_SendData_8Bit(0x0E);    

	LCD_0IN96_SendCommand(0xE0);
	LCD_0IN96_SendData_8Bit(0x10);
	LCD_0IN96_SendData_8Bit(0x0E);
	LCD_0IN96_SendData_8Bit(0x02);
	LCD_0IN96_SendData_8Bit(0x03);
	LCD_0IN96_SendData_8Bit(0x0E);
	LCD_0IN96_SendData_8Bit(0x07);
	LCD_0IN96_SendData_8Bit(0x02);
	LCD_0IN96_SendData_8Bit(0x07);
	LCD_0IN96_SendData_8Bit(0x0A);
	LCD_0IN96_SendData_8Bit(0x12);
	LCD_0IN96_SendData_8Bit(0x27);
	LCD_0IN96_SendData_8Bit(0x37);
	LCD_0IN96_SendData_8Bit(0x00);
	LCD_0IN96_SendData_8Bit(0x0D);
	LCD_0IN96_SendData_8Bit(0x0E);
	LCD_0IN96_SendData_8Bit(0x10);

	LCD_0IN96_SendCommand(0xE1);
	LCD_0IN96_SendData_8Bit(0x10);
	LCD_0IN96_SendData_8Bit(0x0E);
	LCD_0IN96_SendData_8Bit(0x03);
	LCD_0IN96_SendData_8Bit(0x03);
	LCD_0IN96_SendData_8Bit(0x0F);
	LCD_0IN96_SendData_8Bit(0x06);
	LCD_0IN96_SendData_8Bit(0x02);
	LCD_0IN96_SendData_8Bit(0x08);
	LCD_0IN96_SendData_8Bit(0x0A);
	LCD_0IN96_SendData_8Bit(0x13);
	LCD_0IN96_SendData_8Bit(0x26);
	LCD_0IN96_SendData_8Bit(0x36);
	LCD_0IN96_SendData_8Bit(0x00);
	LCD_0IN96_SendData_8Bit(0x0D);
	LCD_0IN96_SendData_8Bit(0x0E);
	LCD_0IN96_SendData_8Bit(0x10);

	LCD_0IN96_SendCommand(0x3A); 
	LCD_0IN96_SendData_8Bit(0x05);

	LCD_0IN96_SendCommand(0x36);
	LCD_0IN96_SendData_8Bit(0xA8);

	LCD_0IN96_SendCommand(0x29);
}


/********************************************************************************
function :	Initialize the lcd
parameter:
********************************************************************************/
void LCD_0IN96_Init(UBYTE Scan_dir)
{
    DEV_SET_PWM(90);
    LCD_0IN96_Reset();
	
    LCD_0IN96.HEIGHT	= LCD_0IN96_HEIGHT;       
    LCD_0IN96.WIDTH   = LCD_0IN96_WIDTH;
	
    //Set the initialization register
    LCD_0IN96_InitReg();
	
}

/********************************************************************************
function:	Sets the start position and size of the display area
parameter:
		Xstart 	:   X direction Start coordinates
		Ystart  :   Y direction Start coordinates
		Xend    :   X direction end coordinates
		Yend    :   Y direction end coordinates
********************************************************************************/
void LCD_0IN96_SetWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend)
{
	Xstart = Xstart + 1;
	Xend = Xend + 1;
	Ystart = Ystart + 26;
	Yend = Yend+26;
	
	LCD_0IN96_SendCommand(0x2a);
	LCD_0IN96_SendData_8Bit(Xstart >> 8);
	LCD_0IN96_SendData_8Bit(Xstart);
	LCD_0IN96_SendData_8Bit(Xend >> 8);
	LCD_0IN96_SendData_8Bit(Xend );

	LCD_0IN96_SendCommand(0x2b);
	LCD_0IN96_SendData_8Bit(Ystart >> 8);
	LCD_0IN96_SendData_8Bit(Ystart);
	LCD_0IN96_SendData_8Bit(Yend >> 8);
	LCD_0IN96_SendData_8Bit(Yend);

	LCD_0IN96_SendCommand(0x2C);
}

/******************************************************************************
function :	Clear screen
parameter:
******************************************************************************/
void LCD_0IN96_Clear(UWORD Color)
{
    UWORD j,i;
    UWORD Image[LCD_0IN96.WIDTH*LCD_0IN96.HEIGHT];
    
    Color = ((Color<<8)&0xff00)|(Color>>8);
   
    for (j = 0; j < LCD_0IN96.HEIGHT*LCD_0IN96.WIDTH; j++) {
        Image[j] = Color;
    }
    
    LCD_0IN96_SetWindows(0, 0, LCD_0IN96.WIDTH-1, LCD_0IN96.HEIGHT-1);
    DEV_Digital_Write(EPD_DC_PIN, 1);
    DEV_Digital_Write(EPD_CS_PIN, 0);
	
    // for(j = 0; j < LCD_0IN96.WIDTH*LCD_0IN96.HEIGHT; j++){
        // LCD_0IN96_SendData_16Bit(Image[j]);	
		
	for(j = 0; j < LCD_0IN96.HEIGHT; j++){
        DEV_SPI_Write_nByte((uint8_t *)&Image[j*LCD_0IN96.WIDTH], LCD_0IN96.WIDTH*2);
    }
    DEV_Digital_Write(EPD_CS_PIN, 1);
}

/******************************************************************************
function :	Sends the image buffer in RAM to displays
parameter:
******************************************************************************/
void LCD_0IN96_Display(UWORD *Image)
{
    UWORD j;
    LCD_0IN96_SetWindows(0, 0, LCD_0IN96.WIDTH-1, LCD_0IN96.HEIGHT-1);
    DEV_Digital_Write(EPD_DC_PIN, 1);
 //   DEV_Digital_Write(EPD_CS_PIN, 0);
	
    for (j = 0; j < LCD_0IN96.HEIGHT; j++) {
        DEV_SPI_Write_nByte((uint8_t *)&Image[j*LCD_0IN96.WIDTH], LCD_0IN96.WIDTH*2);
		
	// for(j = 0; j < LCD_0IN96.WIDTH*LCD_0IN96.HEIGHT; j++){
        // LCD_0IN96_SendData_16Bit(Image[j]);		
    }
//    DEV_Digital_Write(EPD_CS_PIN, 1);
    //LCD_0IN96_SendCommand(0x29);
}

void LCD_0IN96_DisplayWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD *Image)
{
    // display
    UDOUBLE Addr = 0;

    UWORD j;
    LCD_0IN96_SetWindows(Xstart, Ystart, Xend , Yend);
    DEV_Digital_Write(EPD_DC_PIN, 1);
    DEV_Digital_Write(EPD_CS_PIN, 0);
    for (j = Ystart; j < Yend - 1; j++) {
        Addr = Xstart + j * LCD_0IN96.WIDTH ;
        DEV_SPI_Write_nByte((uint8_t *)&Image[Addr], (Xend-Xstart)*2);
    }
    DEV_Digital_Write(EPD_CS_PIN, 1);
}

void LCD_0IN96_DisplayPoint(UWORD X, UWORD Y, UWORD Color)
{
    LCD_0IN96_SetWindows(X,Y,X,Y);
    LCD_0IN96_SendData_16Bit(Color);
}

void  Handler_0IN96_LCD(int signo)
{
    //System Exit
    printf("\r\nHandler:Program stop\r\n");     
    DEV_Module_Exit();
	exit(0);
}
