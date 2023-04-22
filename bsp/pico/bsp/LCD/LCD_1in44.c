/*****************************************************************************
* | File      	:   LCD_1in44.c
* | Author      :   Waveshare team
* | Function    :   Hardware underlying interface
* | Info        :
*                Used to shield the underlying layers of each master
*                and enhance portability
*----------------
* |	This version:   V1.0
* | Date        :   2020-05-20
* | Info        :   Basic version
*
******************************************************************************/
#include "LCD_1in44.h"
#include "DEV_Config.h"

#include <stdlib.h>		//itoa()
#include <stdio.h>

LCD_1IN44_ATTRIBUTES LCD_1IN44;


/******************************************************************************
function :	Hardware reset
parameter:
******************************************************************************/
static void LCD_1IN44_Reset(void)
{
    DEV_Digital_Write(EPD_RST_PIN, 1);
    DEV_Delay_ms(100);
    DEV_Digital_Write(EPD_RST_PIN, 0);
    DEV_Delay_ms(100);
    DEV_Digital_Write(EPD_RST_PIN, 1);
    DEV_Delay_ms(100);
}

/******************************************************************************
function :	send command
parameter:
     Reg : Command register
******************************************************************************/
static void LCD_1IN44_SendCommand(UBYTE Reg)
{
    DEV_Digital_Write(EPD_DC_PIN, 0);
    DEV_Digital_Write(EPD_CS_PIN, 0);
    DEV_SPI_WriteByte(Reg);
   // DEV_Digital_Write(EPD_CS_PIN, 1);
}

/******************************************************************************
function :	send data
parameter:
    Data : Write data
******************************************************************************/
static void LCD_1IN44_SendData_8Bit(UBYTE Data)
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
static void LCD_1IN44_SendData_16Bit(UWORD Data)
{
    DEV_Digital_Write(EPD_DC_PIN, 1);
    DEV_Digital_Write(EPD_CS_PIN, 0);
    DEV_SPI_WriteByte((Data >> 8) & 0xFF);
    DEV_SPI_WriteByte(Data & 0xFF);
    DEV_Digital_Write(EPD_CS_PIN, 1);
}

/******************************************************************************
function :	Initialize the lcd register
parameter:
******************************************************************************/
static void LCD_1IN44_InitReg(void)
{
    LCD_1IN44_SendCommand(0x3A);
    LCD_1IN44_SendData_8Bit(0x05);

     //ST7735R Frame Rate
    LCD_1IN44_SendCommand(0xB1);
    LCD_1IN44_SendData_8Bit(0x01);
    LCD_1IN44_SendData_8Bit(0x2C);
    LCD_1IN44_SendData_8Bit(0x2D);

    LCD_1IN44_SendCommand(0xB2);
    LCD_1IN44_SendData_8Bit(0x01);
    LCD_1IN44_SendData_8Bit(0x2C);
    LCD_1IN44_SendData_8Bit(0x2D);

    LCD_1IN44_SendCommand(0xB3);
    LCD_1IN44_SendData_8Bit(0x01);
    LCD_1IN44_SendData_8Bit(0x2C);
    LCD_1IN44_SendData_8Bit(0x2D);
    LCD_1IN44_SendData_8Bit(0x01);
    LCD_1IN44_SendData_8Bit(0x2C);
    LCD_1IN44_SendData_8Bit(0x2D);

    LCD_1IN44_SendCommand(0xB4); //Column inversion
    LCD_1IN44_SendData_8Bit(0x07);

    //ST7735R Power Sequence
    LCD_1IN44_SendCommand(0xC0);
    LCD_1IN44_SendData_8Bit(0xA2);
    LCD_1IN44_SendData_8Bit(0x02);
    LCD_1IN44_SendData_8Bit(0x84);
    LCD_1IN44_SendCommand(0xC1);
    LCD_1IN44_SendData_8Bit(0xC5);

    LCD_1IN44_SendCommand(0xC2);
    LCD_1IN44_SendData_8Bit(0x0A);
    LCD_1IN44_SendData_8Bit(0x00);

    LCD_1IN44_SendCommand(0xC3);
    LCD_1IN44_SendData_8Bit(0x8A);
    LCD_1IN44_SendData_8Bit(0x2A);
    LCD_1IN44_SendCommand(0xC4);
    LCD_1IN44_SendData_8Bit(0x8A);
    LCD_1IN44_SendData_8Bit(0xEE);

    LCD_1IN44_SendCommand(0xC5); //VCOM
    LCD_1IN44_SendData_8Bit(0x0E);

    //ST7735R Gamma Sequence
    LCD_1IN44_SendCommand(0xe0);
    LCD_1IN44_SendData_8Bit(0x0f);
    LCD_1IN44_SendData_8Bit(0x1a);
    LCD_1IN44_SendData_8Bit(0x0f);
    LCD_1IN44_SendData_8Bit(0x18);
    LCD_1IN44_SendData_8Bit(0x2f);
    LCD_1IN44_SendData_8Bit(0x28);
    LCD_1IN44_SendData_8Bit(0x20);
    LCD_1IN44_SendData_8Bit(0x22);
    LCD_1IN44_SendData_8Bit(0x1f);
    LCD_1IN44_SendData_8Bit(0x1b);
    LCD_1IN44_SendData_8Bit(0x23);
    LCD_1IN44_SendData_8Bit(0x37);
    LCD_1IN44_SendData_8Bit(0x00);
    LCD_1IN44_SendData_8Bit(0x07);
    LCD_1IN44_SendData_8Bit(0x02);
    LCD_1IN44_SendData_8Bit(0x10);

    LCD_1IN44_SendCommand(0xe1);
    LCD_1IN44_SendData_8Bit(0x0f);
    LCD_1IN44_SendData_8Bit(0x1b);
    LCD_1IN44_SendData_8Bit(0x0f);
    LCD_1IN44_SendData_8Bit(0x17);
    LCD_1IN44_SendData_8Bit(0x33);
    LCD_1IN44_SendData_8Bit(0x2c);
    LCD_1IN44_SendData_8Bit(0x29);
    LCD_1IN44_SendData_8Bit(0x2e);
    LCD_1IN44_SendData_8Bit(0x30);
    LCD_1IN44_SendData_8Bit(0x30);
    LCD_1IN44_SendData_8Bit(0x39);
    LCD_1IN44_SendData_8Bit(0x3f);
    LCD_1IN44_SendData_8Bit(0x00);
    LCD_1IN44_SendData_8Bit(0x07);
    LCD_1IN44_SendData_8Bit(0x03);
    LCD_1IN44_SendData_8Bit(0x10);
	
	LCD_1IN44_SendCommand(0x21);// reverse
    LCD_1IN44_SendCommand(0x11);
    DEV_Delay_ms(120);

    //Turn on the LCD display
    LCD_1IN44_SendCommand(0x29);

}

/********************************************************************************
function:	Set the resolution and scanning method of the screen
parameter:
		Scan_dir:   Scan direction
********************************************************************************/
static void LCD_1IN44_SetAttributes(UBYTE Scan_dir)
{
    //Get the screen scan direction
    LCD_1IN44.SCAN_DIR = Scan_dir;
    UBYTE MemoryAccessReg = 0x00;

    //Get GRAM and LCD width and height
    if(Scan_dir == HORIZONTAL) {
        LCD_1IN44.HEIGHT	= LCD_1IN44_WIDTH;
        LCD_1IN44.WIDTH   = LCD_1IN44_HEIGHT;
        MemoryAccessReg = 0X70;
    } else {
        LCD_1IN44.HEIGHT	= LCD_1IN44_HEIGHT;       
        LCD_1IN44.WIDTH   = LCD_1IN44_WIDTH;
        MemoryAccessReg = 0X00;
    }

    // Set the read / write scan direction of the frame memory
    LCD_1IN44_SendCommand(0x36); //MX, MY, RGB mode
    LCD_1IN44_SendData_8Bit(MemoryAccessReg);	//0x08 set RGB
}

/********************************************************************************
function :	Initialize the lcd
parameter:
********************************************************************************/
void LCD_1IN44_Init(UBYTE Scan_dir)
{
    DEV_SET_PWM(90);
    //Hardware reset   
    LCD_1IN44_Reset();

    //Set the resolution and scanning method of the screen
    LCD_1IN44_SetAttributes(Scan_dir);
    
    //Set the initialization register
    LCD_1IN44_InitReg();


}

/********************************************************************************
function:	Sets the start position and size of the display area
parameter:
		Xstart 	:   X direction Start coordinates
		Ystart  :   Y direction Start coordinates
		Xend    :   X direction end coordinates
		Yend    :   Y direction end coordinates
********************************************************************************/
void LCD_1IN44_SetWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend)
{
    UBYTE x,y;
    if(LCD_1IN44.SCAN_DIR == HORIZONTAL){x=1;y=2;}
    else{ x=2; y=1; }
    //set the X coordinates
    LCD_1IN44_SendCommand(0x2A);
    
    
    LCD_1IN44_SendData_16Bit(Xstart	+x);
    LCD_1IN44_SendData_16Bit(Xend-1	+x);
    //set the Y coordinates
    LCD_1IN44_SendCommand(0x2B);
    LCD_1IN44_SendData_16Bit(Ystart +y);
    LCD_1IN44_SendData_16Bit(Yend-1	  +y);

    LCD_1IN44_SendCommand(0X2C);
    // printf("%d %d\r\n",x,y);
}

/******************************************************************************
function :	Clear screen
parameter:
******************************************************************************/
void LCD_1IN44_Clear(UWORD Color)
{
    UWORD j,i;
    UWORD Image[LCD_1IN44.WIDTH*LCD_1IN44.HEIGHT];
    
    Color = ((Color<<8)&0xff00)|(Color>>8);
   
    for (j = 0; j < LCD_1IN44.HEIGHT*LCD_1IN44.WIDTH; j++) {
        Image[j] = Color;
    }
    
    LCD_1IN44_SetWindows(0, 0, LCD_1IN44.WIDTH, LCD_1IN44.HEIGHT);
    DEV_Digital_Write(EPD_DC_PIN, 1);
    DEV_Digital_Write(EPD_CS_PIN, 0);
    // printf("HEIGHT %d, WIDTH %d\r\n",LCD_1IN44.HEIGHT,LCD_1IN44.WIDTH);
    for(j = 0; j < LCD_1IN44.HEIGHT; j++){
        DEV_SPI_Write_nByte((uint8_t *)&Image[j*LCD_1IN44.WIDTH], LCD_1IN44.WIDTH*2);
        
    }
    DEV_Digital_Write(EPD_CS_PIN, 1);
}

/******************************************************************************
function :	Sends the image buffer in RAM to displays
parameter:
******************************************************************************/
void LCD_1IN44_Display(UWORD *Image)
{
    UWORD j;
    LCD_1IN44_SetWindows(0, 0, LCD_1IN44.WIDTH, LCD_1IN44.HEIGHT);
    DEV_Digital_Write(EPD_DC_PIN, 1);
    DEV_Digital_Write(EPD_CS_PIN, 0);
    for (j = 0; j < LCD_1IN44.HEIGHT; j++) {
        DEV_SPI_Write_nByte((uint8_t *)&Image[j*LCD_1IN44.WIDTH], LCD_1IN44.WIDTH*2);
    }
    DEV_Digital_Write(EPD_CS_PIN, 1);
    LCD_1IN44_SendCommand(0x29);
}
  
void LCD_1IN44_DisplayWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD *Image)
{
    // display
    UDOUBLE Addr = 0;

    UWORD j;
    LCD_1IN44_SetWindows(Xstart, Ystart, Xend , Yend);
    DEV_Digital_Write(EPD_DC_PIN, 1);
    DEV_Digital_Write(EPD_CS_PIN, 0);
    for (j = Ystart; j < Yend - 1; j++) {
        Addr = Xstart + j * LCD_1IN44.WIDTH ;
        DEV_SPI_Write_nByte((uint8_t *)&Image[Addr], (Xend-Xstart)*2);
    }
    DEV_Digital_Write(EPD_CS_PIN, 1);
}

void LCD_1IN44_DisplayPoint(UWORD X, UWORD Y, UWORD Color)
{
    LCD_1IN44_SetWindows(X,Y,X,Y);
    LCD_1IN44_SendData_16Bit(Color);
}

void  Handler_1IN44_LCD(int signo)
{
    //System Exit
    printf("\r\nHandler:Program stop\r\n");     
    DEV_Module_Exit();
	exit(0);
}

