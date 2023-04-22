#include "LCD_1in54.h"
#include "DEV_Config.h"

#include <stdlib.h>		//itoa()
#include <stdio.h>

LCD_1IN54_ATTRIBUTES LCD_1IN54;


/******************************************************************************
function :	Hardware reset
parameter:
******************************************************************************/
static void LCD_1IN54_Reset(void)
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
static void LCD_1IN54_SendCommand(UBYTE Reg)
{
    DEV_Digital_Write(EPD_DC_PIN, 0);
    DEV_Digital_Write(EPD_CS_PIN, 0);
    DEV_SPI_WriteByte(Reg);
    DEV_Digital_Write(EPD_CS_PIN, 1);
}

/******************************************************************************
function :	send data
parameter:
    Data : Write data
******************************************************************************/
static void LCD_1IN54_SendData_8Bit(UBYTE Data)
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
static void LCD_1IN54_SendData_16Bit(UWORD Data)
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
static void LCD_1IN54_InitReg(void)
{
    LCD_1IN54_SendCommand(0x3A);
    LCD_1IN54_SendData_8Bit(0x05);

    LCD_1IN54_SendCommand(0xB2);
    LCD_1IN54_SendData_8Bit(0x0C);
    LCD_1IN54_SendData_8Bit(0x0C);
    LCD_1IN54_SendData_8Bit(0x00);
    LCD_1IN54_SendData_8Bit(0x33);
    LCD_1IN54_SendData_8Bit(0x33);

    LCD_1IN54_SendCommand(0xB7);  //Gate Control
    LCD_1IN54_SendData_8Bit(0x35);

    LCD_1IN54_SendCommand(0xBB);  //VCOM Setting
    LCD_1IN54_SendData_8Bit(0x19);

    LCD_1IN54_SendCommand(0xC0); //LCM Control     
    LCD_1IN54_SendData_8Bit(0x2C);

    LCD_1IN54_SendCommand(0xC2);  //VDV and VRH Command Enable
    LCD_1IN54_SendData_8Bit(0x01);
    LCD_1IN54_SendCommand(0xC3);  //VRH Set
    LCD_1IN54_SendData_8Bit(0x12);
    LCD_1IN54_SendCommand(0xC4);  //VDV Set
    LCD_1IN54_SendData_8Bit(0x20);

    LCD_1IN54_SendCommand(0xC6);  //Frame Rate Control in Normal Mode
    LCD_1IN54_SendData_8Bit(0x0F);
    
    LCD_1IN54_SendCommand(0xD0);  // Power Control 1
    LCD_1IN54_SendData_8Bit(0xA4);
    LCD_1IN54_SendData_8Bit(0xA1);

    LCD_1IN54_SendCommand(0xE0);  //Positive Voltage Gamma Control
    LCD_1IN54_SendData_8Bit(0xD0);
    LCD_1IN54_SendData_8Bit(0x04);
    LCD_1IN54_SendData_8Bit(0x0D);
    LCD_1IN54_SendData_8Bit(0x11);
    LCD_1IN54_SendData_8Bit(0x13);
    LCD_1IN54_SendData_8Bit(0x2B);
    LCD_1IN54_SendData_8Bit(0x3F);
    LCD_1IN54_SendData_8Bit(0x54);
    LCD_1IN54_SendData_8Bit(0x4C);
    LCD_1IN54_SendData_8Bit(0x18);
    LCD_1IN54_SendData_8Bit(0x0D);
    LCD_1IN54_SendData_8Bit(0x0B);
    LCD_1IN54_SendData_8Bit(0x1F);
    LCD_1IN54_SendData_8Bit(0x23);

    LCD_1IN54_SendCommand(0xE1);  //Negative Voltage Gamma Control
    LCD_1IN54_SendData_8Bit(0xD0);
    LCD_1IN54_SendData_8Bit(0x04);
    LCD_1IN54_SendData_8Bit(0x0C);
    LCD_1IN54_SendData_8Bit(0x11);
    LCD_1IN54_SendData_8Bit(0x13);
    LCD_1IN54_SendData_8Bit(0x2C);
    LCD_1IN54_SendData_8Bit(0x3F);
    LCD_1IN54_SendData_8Bit(0x44);
    LCD_1IN54_SendData_8Bit(0x51);
    LCD_1IN54_SendData_8Bit(0x2F);
    LCD_1IN54_SendData_8Bit(0x1F);
    LCD_1IN54_SendData_8Bit(0x1F);
    LCD_1IN54_SendData_8Bit(0x20);
    LCD_1IN54_SendData_8Bit(0x23);

    LCD_1IN54_SendCommand(0x21);  //Display Inversion On

    LCD_1IN54_SendCommand(0x11);  //Sleep Out

    LCD_1IN54_SendCommand(0x29);  //Display On
}

/********************************************************************************
function:	Set the resolution and scanning method of the screen
parameter:
		Scan_dir:   Scan direction
********************************************************************************/
static void LCD_1IN54_SetAttributes(UBYTE Scan_dir)
{
    //Get the screen scan direction
    LCD_1IN54.SCAN_DIR = Scan_dir;
    UBYTE MemoryAccessReg = 0x00;

    //Get GRAM and LCD width and height
    if(Scan_dir == HORIZONTAL) {
        LCD_1IN54.HEIGHT	= LCD_1IN54_HEIGHT;
        LCD_1IN54.WIDTH   = LCD_1IN54_WIDTH;
        MemoryAccessReg = 0X70;
    } else {
        LCD_1IN54.HEIGHT	= LCD_1IN54_WIDTH;
        LCD_1IN54.WIDTH   = LCD_1IN54_HEIGHT;
        MemoryAccessReg = 0X00;
    }

    // Set the read / write scan direction of the frame memory
    LCD_1IN54_SendCommand(0x36); //MX, MY, RGB mode
    LCD_1IN54_SendData_8Bit(MemoryAccessReg);	//0x08 set RGB
}

/********************************************************************************
function :	Initialize the lcd
parameter:
********************************************************************************/
void LCD_1IN54_Init(UBYTE Scan_dir)
{
    //Turn on the backlight
    // LCD_1IN54_BL_1;

    //Hardware reset
    LCD_1IN54_Reset();

    //Set the resolution and scanning method of the screen
    LCD_1IN54_SetAttributes(Scan_dir);
    
    //Set the initialization register
    LCD_1IN54_InitReg();
}

/********************************************************************************
function:	Sets the start position and size of the display area
parameter:
		Xstart 	:   X direction Start coordinates
		Ystart  :   Y direction Start coordinates
		Xend    :   X direction end coordinates
		Yend    :   Y direction end coordinates
********************************************************************************/
void LCD_1IN54_SetWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend)
{
    //set the X coordinates
    LCD_1IN54_SendCommand(0x2A);
    LCD_1IN54_SendData_8Bit((Xstart >> 8) & 0xFF);
    LCD_1IN54_SendData_8Bit(Xstart & 0xFF);
    LCD_1IN54_SendData_8Bit(((Xend  - 1) >> 8) & 0xFF);
    LCD_1IN54_SendData_8Bit((Xend  - 1) & 0xFF);

    //set the Y coordinates
    LCD_1IN54_SendCommand(0x2B);
    LCD_1IN54_SendData_8Bit((Ystart >> 8) & 0xFF);
    LCD_1IN54_SendData_8Bit(Ystart & 0xFF);
    LCD_1IN54_SendData_8Bit(((Yend  - 1) >> 8) & 0xFF);
    LCD_1IN54_SendData_8Bit((Yend  - 1) & 0xFF);

    LCD_1IN54_SendCommand(0X2C);
}

/******************************************************************************
function :	Clear screen
parameter:
******************************************************************************/
void LCD_1IN54_Clear(UWORD Color)
{
    UWORD j;
    UWORD Image[LCD_1IN54_WIDTH*LCD_1IN54_HEIGHT];
    
    Color = ((Color<<8)&0xff00)|(Color>>8);
   
    for (j = 0; j < LCD_1IN54_HEIGHT*LCD_1IN54_WIDTH; j++) {
        Image[j] = Color;
    }
    
    LCD_1IN54_SetWindows(0, 0, LCD_1IN54_WIDTH, LCD_1IN54_HEIGHT);
    DEV_Digital_Write(EPD_DC_PIN, 1);
    DEV_Digital_Write(EPD_CS_PIN, 0);
    for(j = 0; j < LCD_1IN54_HEIGHT; j++){
        DEV_SPI_Write_nByte((uint8_t *)&Image[j*LCD_1IN54_WIDTH], LCD_1IN54_WIDTH*2);
    }
    DEV_Digital_Write(EPD_CS_PIN, 1);
}

/******************************************************************************
function :	Sends the image buffer in RAM to displays
parameter:
******************************************************************************/
void LCD_1IN54_Display(UWORD *Image)
{
    UWORD j;
    LCD_1IN54_SetWindows(0, 0, LCD_1IN54_WIDTH, LCD_1IN54_HEIGHT);
    DEV_Digital_Write(EPD_DC_PIN, 1);
    DEV_Digital_Write(EPD_CS_PIN, 0);
    for (j = 0; j < LCD_1IN54_HEIGHT; j++) {
        DEV_SPI_Write_nByte((uint8_t *)&Image[j*LCD_1IN54_WIDTH], LCD_1IN54_WIDTH*2);
    }
    DEV_Digital_Write(EPD_CS_PIN, 1);
}

void LCD_1IN54_DisplayWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD *Image)
{
    // display
    UDOUBLE Addr = 0;

    UWORD j;
    LCD_1IN54_SetWindows(Xstart, Ystart, Xend , Yend);
    DEV_Digital_Write(EPD_DC_PIN, 1);
    DEV_Digital_Write(EPD_CS_PIN, 0);
    for (j = Ystart; j < Yend - 1; j++) {
        Addr = Xstart + j * LCD_1IN54_WIDTH ;
        DEV_SPI_Write_nByte((uint8_t *)&Image[Addr], (Xend-Xstart)*2);
    }
    DEV_Digital_Write(EPD_CS_PIN, 1);
}

void LCD_1IN54_DisplayPoint(UWORD X, UWORD Y, UWORD Color)
{
    LCD_1IN54_SetWindows(X,Y,X,Y);
    LCD_1IN54_SendData_16Bit(Color);
}

